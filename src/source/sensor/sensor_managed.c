/*
*  Contains the regular sensor functionality/managed functionality
*/
#include "tss/sys/config.h"
#if !TSS_MINIMAL_SENSOR
#include "tss/api/sensor.h"
#include "internal.h"
#include "tss/api/core.h"
#include "tss/sys/stdinc.h"
#include "tss/errors.h"
#include "tss/sys/time.h"

#define REQUIRED_HEADER_BITS (TSS_HEADER_CHECKSUM_BIT | TSS_HEADER_LENGTH_BIT | TSS_HEADER_ECHO_BIT)

#define THREESPACE_AWAIT_COMMAND_FOUND 0
#define THREESPACE_AWAIT_COMMAND_TIMEOUT 1
#define THREESPACE_AWAIT_BOOTLOADER_FOUND 2

#define THREESPACE_UPDATE_COMMAND_PARSED 0
#define THREESPACE_UPDATE_COMMAND_MISALIGNED 1
#define THREESPACE_UPDATE_COMMAND_NOT_ENOUGH_DATA 2
#define THREESPACE_UPDATE_COMMAND_NONE -1

void tssCreateSensor(TSS_Sensor *sensor, struct TSS_Com_Class *com)
{
    *sensor = (TSS_Sensor) {
        .com = com,
        ._header_enabled = true
    };
}

//----------------------------------CORE SENSOR FUNCTIONS------------------------------------------------------

static int cacheHeader(TSS_Sensor *sensor);
static int cacheStreamSlots(TSS_Sensor *sensor);

static int initFirmware(TSS_Sensor *sensor);

//----------------------------------------ALIGNMENT & VALIDATION FUNCTIONS-----------------------------------------
static inline void handleMisalignment(TSS_Sensor *sensor);
static int peekValidatePacket(TSS_Sensor *sensor, const struct TSS_Header *header, size_t min_data_len, size_t max_data_len);
static struct TSS_Header* tryPeekHeader(TSS_Sensor *sensor, struct TSS_Header *out);
static int peekCheckDebugMessage(TSS_Sensor *sensor);

static int internalUpdate(TSS_Sensor *sensor, const struct TSS_Header *header);
static int awaitCommandResponse(TSS_Sensor *sensor, uint8_t cmd_num, uint16_t min_data_len, uint16_t max_data_len);
static int awaitGetSettingResponse(TSS_Sensor *sensor, uint16_t min_len, bool check_bootloader);
static int awaitSetSettingResponse(TSS_Sensor *sensor, uint16_t num_keys);

//Helper Macros
#define comLength(sensor) tss_com_length((sensor)->com)
#define peekCapacity(sensor) tss_com_peek_capacity((sensor)->com)
#define getTimeout(sensor) tss_com_get_timeout((sensor)->com)

int tssInitSensor(TSS_Sensor *sensor) {
    int err;
    uint8_t in_bootloader;

    //Some bootloaders require this pattern first thing
    //to work properly over certain communication interfaces.
    //Without this, or moving it later, may cause a situation
    //where the sensor can not be properly initialized when in bootloader.
    TSS_COM_BEGIN_WRITE(sensor->com);
    tss_com_write(sensor->com, (uint8_t*)"UUU", 3);
    TSS_COM_END_WRITE(sensor->com);

    sensorInternalForceStopStreaming(sensor);

    //Clear out any garbage data
    tss_com_clear_timeout(sensor->com, 5);

    //Check for bootloader
    err = sensorInternalBootloaderCheckActive(sensor, &in_bootloader);
    if(err != TSS_SUCCESS) return err;
    sensor->_in_bootloader = in_bootloader;
    if(!sensor->_in_bootloader) {
        err = initFirmware(sensor);
    }
    else {
        //Cache so if reenumerates can find when booting into firmware
        err = sensorBootloaderGetSerialNumber(sensor, &sensor->serial_number);
    }

    return err;
}


static int initFirmware(TSS_Sensor *sensor) {
    sensor->dirty = false;
    sensor->_header_enabled = true;
    return sensorUpdateCachedSettings(sensor);
}

//----------------------------------CACHEING FUNCTIONS-------------------------------------------

static int cacheHeader(TSS_Sensor *sensor) {
    int err;
    uint8_t header;
    err = sensorReadHeader(sensor, &header);
    if(err) return err;
    if(header == sensor->header_cfg.bitfield && (header & REQUIRED_HEADER_BITS) == REQUIRED_HEADER_BITS) return TSS_SUCCESS; //Nothing to update

    //Do not allow changing the header if currently streaming to prevent misalignment issues
    if(sensorIsStreaming(sensor)) {
        return sensorWriteHeader(sensor, sensor->header_cfg.bitfield);
    }

    //Force required bits to be enabled
    if((header & REQUIRED_HEADER_BITS) != REQUIRED_HEADER_BITS) {
        header |= REQUIRED_HEADER_BITS;
        err = sensorWriteHeader(sensor, header);
    }
    sensor->header_cfg = tssHeaderInfoFromBitfield(header);

    return err;
}

static int cacheStreamSlots(TSS_Sensor *sensor) {
    char stream_slots[130];
    int err;
    uint16_t output_size, size;
    uint8_t i;
    
    err = sensorReadStreamSlots(sensor, stream_slots, sizeof(stream_slots));
    if(err) return err;
    tssUtilStreamSlotStringToCommands(stream_slots, sensor->streaming.data.commands);
    
    output_size = 0;
    for(i = 0; i < TSS_NUM_STREAM_SLOTS && sensor->streaming.data.commands[i] != NULL; i++) {
        tssGetParamListSize(sensor->streaming.data.commands[i]->out_format, &size, &size);
        output_size += size;
    }
    sensor->streaming.data.output_size = output_size;

    return TSS_SUCCESS;
}

int sensorUpdateCachedSettings(TSS_Sensor *sensor) {
    int err;
    uint8_t value;
    
    sensor->dirty = false;

    //Cache the header
    err = cacheHeader(sensor);
    if(err) return err;

    //Cache debug mode
    err = sensorReadDebugMode(sensor, &value);
    if(err) return err;
    sensor->debug._immediate = value;

    err = cacheStreamSlots(sensor);
    if(err) return err;

    err = sensorReadSerialNumber(sensor, &sensor->serial_number);
    if(err) return err;

    return TSS_SUCCESS;
}

static int checkDirty(TSS_Sensor *sensor) {
    int err;
    uint8_t in_bootloader;
    if(!sensor->dirty) return TSS_SUCCESS;

    //Ensure connected
    if(tss_com_open(sensor->com)) {
        err = sensorReconnect(sensor, 2000); //Will cause reenumeration if enabled
        if(err) return err;
    }
    sensorInternalForceStopStreaming(sensor);
    err = sensorInternalBootloaderCheckActive(sensor, &in_bootloader);
    if(err) return err;
    sensor->_in_bootloader = in_bootloader;
    
    if(!in_bootloader) {
        err = initFirmware(sensor);
    }
    
    sensor->dirty = false;

    return err;
}

//--------------------------------GENERIC FUNCTIONS-------------------------------------

int sensorInternalExecuteCommandCustomV(TSS_Sensor *sensor, const struct TSS_Command *command, const void **input, SensorInternalReadFunction read_func, va_list outputs)
{
    int err_or_checksum;
    err_or_checksum = checkDirty(sensor);
    if(err_or_checksum) return err_or_checksum;
    tssWriteCommand(sensor->com, sensor->_header_enabled, command, input);
    err_or_checksum = read_func(sensor, command, outputs);
    if(err_or_checksum < 0) return err_or_checksum;
    return TSS_SUCCESS;
}

int sensorInternalExecuteCommandCustomArray(TSS_Sensor *sensor, const struct TSS_Command *command, const void **input, SensorInternalReadFunctionArray read_func, void **outputs)
{
    int err_or_checksum;
    err_or_checksum = checkDirty(sensor);
    if(err_or_checksum) return err_or_checksum;
    tssWriteCommand(sensor->com, sensor->_header_enabled, command, input);
    err_or_checksum = read_func(sensor, command, outputs);
    if(err_or_checksum < 0) return err_or_checksum;
    return TSS_SUCCESS;
}

int sensorInternalBaseCommandRead(TSS_Sensor *sensor, const struct TSS_Command *command, va_list outputs)
{
    int result;
    uint16_t min_size, max_size;
    tssGetParamListSize(command->out_format, &min_size, &max_size);
    result = awaitCommandResponse(sensor, command->num, min_size, max_size);
    if(result != THREESPACE_AWAIT_COMMAND_FOUND) {
        return TSS_ERR_RESPONSE_NOT_FOUND;
    }
    sensorInternalHandleHeader(sensor);
    return tssReadCommandV(sensor->com, command, outputs);
}

int sensorInternalProcessStreamingBatch(TSS_Sensor *sensor, const struct TSS_Command *command, va_list outputs)
{
    int result;
    result = awaitCommandResponse(sensor, command->num, sensor->streaming.data.output_size, sensor->streaming.data.output_size);
    if(result != THREESPACE_AWAIT_COMMAND_FOUND) {
        return TSS_ERR_RESPONSE_NOT_FOUND;
    }
    sensorInternalHandleHeader(sensor);
    return sensorInternalReadStreamingBatch(sensor, command, outputs);
}

int sensorInternalProcessStreamingBatchArray(TSS_Sensor *sensor, const struct TSS_Command *command, void **outputs)
{
    int result;
    result = awaitCommandResponse(sensor, command->num, sensor->streaming.data.output_size, sensor->streaming.data.output_size);
    if(result != THREESPACE_AWAIT_COMMAND_FOUND) {
        return TSS_ERR_RESPONSE_NOT_FOUND;
    }
    sensorInternalHandleHeader(sensor);
    return sensorInternalReadStreamingBatchArray(sensor, command, outputs);   
}

//Common to both read setting functions
inline static int baseReadSettings(TSS_Sensor *sensor, const char *key_string)
{
    int err;
    uint32_t id;
    uint16_t min_response_len;
    err = checkDirty(sensor);
    if(err) return err;
    err = tssGetSettingsWrite(sensor->com, true, key_string);
    if(err) return err;

    //+1 is because the terminating character/delimiter is also required in the response.
    min_response_len = (uint16_t)tssStrLenUntil(key_string, ';');
    if(min_response_len > TSS_SETTING_KEY_ERR_STRING_LEN) {
        min_response_len = TSS_SETTING_KEY_ERR_STRING_LEN;
    }
    min_response_len += (1 + TSS_BINARY_SETTINGS_ID_SIZE); //Add null terminator and header size
    if(awaitGetSettingResponse(sensor, min_response_len, false) != THREESPACE_AWAIT_COMMAND_FOUND) {
        return TSS_ERR_RESPONSE_NOT_FOUND;
    }  

    err = tssReadSettingsHeader(sensor->com, &id);
    if(err < 0) return err;
    return TSS_SUCCESS;
}

int sensorReadSettingsV(TSS_Sensor *sensor, const char *key_string, va_list outputs)
{
    int result;
    result = baseReadSettings(sensor, key_string);
    if(result < 0) {
        return result;
    }
    return tssGetSettingsReadV(sensor->com, &sensor->last_num_settings_read, outputs);
}

int sensorReadSettingsArray(TSS_Sensor *sensor, const char *key_string, void **outputs)
{
    int result;
    result = baseReadSettings(sensor, key_string);
    if(result < 0) {
        return result;
    }
    return tssGetSettingsReadArray(sensor->com, &sensor->last_num_settings_read, outputs);
}

int sensorReadSettingsQuery(TSS_Sensor *sensor, const char *key_string, TssGetSettingsCallback cb, void *user_data)
{
    int result;
    result = baseReadSettings(sensor, key_string);
    if(result < 0) {
        return result;
    }
    return tssGetSettingsReadCb(sensor->com, cb, user_data);
}

inline static int keyInArray(const char *key, const char * const *array, uint8_t size)
{
    uint16_t i = 0;
    for(i = 0; i < size; i++) {
        if(tssSettingKeyCmp(key, array[i]) == 0) {
            return i;
        }
    }
    return -1;
}

inline static void checkAndCacheDebugMode(TSS_Sensor *sensor, const char **keys, 
    uint8_t num_keys, const void **data) 
{
    for(uint16_t i = 0; i < num_keys; i++) {
        if(tssSettingKeyCmp(keys[i], "debug_mode") == 0) {
            uint8_t value = *((uint8_t*)(*data));
            sensor->debug._immediate = (value == 1);
            return;
        }
        else {
            //Advance the data by params input length so that can get the value associated with the debug key when found
            const struct TSS_Setting *setting = tssGetSetting(keys[i]);
            if(setting == NULL) return;
            const struct TSS_Param *param = setting->in_format;
            while(!TSS_PARAM_IS_NULL(param)) {
                data++;
                param++;
            }
        }
    }
}

static const char * const K_HEADER_KEYS[] = { "header", "header_status", "header_timestamp", "header_echo", "header_checksum", "header_serial", "header_length" }; 
int sensorWriteSettings(TSS_Sensor *sensor, const char **keys, uint8_t num_keys, 
    const void **data)
{
    int err;
    err = checkDirty(sensor);
    if(err) return err;

    //Must check for debug_mode=1 before sending the change to be able to properly handle
    //reading the response (since debug messages may be output immediately before the response happens)
    checkAndCacheDebugMode(sensor, keys, num_keys, data);

    err = tssSetSettingsWrite(sensor->com, true, keys, num_keys, data);
    if(err) return err;

    if(awaitSetSettingResponse(sensor, num_keys) != THREESPACE_AWAIT_COMMAND_FOUND) {
        return TSS_ERR_RESPONSE_NOT_FOUND;
    }

    uint32_t id;
    err = tssReadSettingsHeader(sensor->com, &id);
    if(err < 0) return err;
    err = tssSetSettingsRead(sensor->com, &sensor->last_write_setting_response);
    if(err) return err;

    //Check for keys that may need cacheing. This is done here to allow the user to not have
    //to manually call updateCachedSettings when using the base API. Speed is not really a concern
    //for the settings protocol either since changes should be infrequent.

    if(keyInArray("default", keys, num_keys) >= 0) {
        err = sensorUpdateCachedSettings(sensor);
    }
    else if(keyInArray("stream_slots", keys, num_keys) >= 0) {
        err = cacheStreamSlots(sensor);
    }
    else{
        //Check for header keys
        for(uint16_t i = 0; i < num_keys; i++) {
            if(keyInArray(keys[i], K_HEADER_KEYS, sizeof(K_HEADER_KEYS) / sizeof(K_HEADER_KEYS[0])) >= 0) {
                err = cacheHeader(sensor);
                if(err) return err;
                break;
            }
        }
    }
    
    if(err < 0) return err;
    return TSS_SUCCESS;
}

int sensorUpdateStreaming(TSS_Sensor *sensor)
{
    struct TSS_Header header;
    tss_time_t start_time;
    int result;
    result = checkDirty(sensor);
    if(result != TSS_SUCCESS) return false;
    
    start_time = tssTimeGet();
    //Update until either a command is successfully parsed or not enough data for a command
    //Mainly just don't want to have to call this function multiple times to fix misalignments.
    do {
        if(comLength(sensor) < sensor->header_cfg.size) {
            return false;
        }

        tssPeekHeader(sensor->com, &sensor->header_cfg, &header);
        result = internalUpdate(sensor, &header);
    } while(result == THREESPACE_UPDATE_COMMAND_MISALIGNED && 
            tssTimeDiff(start_time) < getTimeout(sensor));

    return result == THREESPACE_UPDATE_COMMAND_PARSED;
}

//-------------------------------------------AWAITING/ALIGNMENT FUNCTIONS--------------------------------------------

static inline void handleMisalignment(TSS_Sensor *sensor) {
    //Continously read 1 byte until aligned
    uint8_t tmp;
    tss_com_read(sensor->com, 1, &tmp);
}


/// @brief Fast fail version of peeking a header that returns a pointer to the out
/// structure or NULL if no header to peek.
static struct TSS_Header* tryPeekHeader(TSS_Sensor *sensor, struct TSS_Header *out) {
    int err;
    if(comLength(sensor) < sensor->header_cfg.size) {
        return NULL;
    }
    err = tssPeekHeader(sensor->com, &sensor->header_cfg, out);
    if(err) {
        return NULL;
    }
    return out;
}

static int peekValidatePacket(TSS_Sensor *sensor, const struct TSS_Header *header, size_t min_data_len, size_t max_data_len)
{
    int err;
    err = tssPeekValidateCommand(sensor->com, sensor->header_cfg.size, 
        header->length, header->checksum, min_data_len, max_data_len);
    if(err == TSS_ERR_INSUFFICIENT_BUFFER) {
        //Pretend this is a success, the com class internal buffer
        //is not big enough to validate. The validation will be performed
        //by the read operation. This does mean if it actually is invalid
        //more data will be lost then normal.
        //TODO: Add a warning mechanism here.
        return TSS_SUCCESS; 
    }
    return err;
}

static int awaitCommandResponse(TSS_Sensor *sensor, uint8_t cmd_num, uint16_t min_data_len, uint16_t max_data_len) {
    //Nothing to do but pretend it was found. Can't check if header isn't enabled.
    if(!sensor->_header_enabled) return THREESPACE_AWAIT_COMMAND_FOUND;

    tss_time_t start_time = tssTimeGet();
    while(tssTimeDiff(start_time) < getTimeout(sensor)) {
        struct TSS_Header header;
        int err = tssPeekHeader(sensor->com, &sensor->header_cfg, &header);
        if(err) {
            continue;
        }

        if(header.echo == cmd_num) {
            if(peekValidatePacket(sensor, &header, min_data_len, max_data_len) == 0) {
                return THREESPACE_AWAIT_COMMAND_FOUND;
            }
            handleMisalignment(sensor);
        }
        else {
            //The data read was not a response to the command being awaited, but
            //may be a response to something else, so call the internal update system to handle
            internalUpdate(sensor, &header);
        }
        
    }

    return THREESPACE_AWAIT_COMMAND_TIMEOUT;
}

static int awaitGetSettingResponse(TSS_Sensor *sensor, uint16_t min_len, bool check_bootloader)
{
    struct TSS_Header header;
    const struct TSS_Setting *setting;
    char buffer[TSS_MAX_SETTINGS_KEY_LEN];
    int num_read_or_err;
    uint32_t id;
    tss_time_t start_time;

    char boot_check[3] = {0};

    //Size of the header, bootloader check may not have header
    if(min_len < TSS_BINARY_SETTINGS_ID_SIZE && !check_bootloader) {
        min_len = TSS_BINARY_SETTINGS_ID_SIZE;
    }

    start_time = tssTimeGet();
    while(tssTimeDiff(start_time) < getTimeout(sensor)) {
        if(comLength(sensor) < min_len) {
            continue;
        }

        if(check_bootloader) {
            tss_com_peek(sensor->com, 0, 2, (uint8_t*)boot_check);
            if(strcmp(boot_check, "OK") == 0) {
                return THREESPACE_AWAIT_BOOTLOADER_FOUND;
            }

            //Don't go on to check if a setting response yet cause not enough length for that
            if(comLength(sensor) < TSS_BINARY_SETTINGS_ID_SIZE) {
                continue;
            }
        }

        //Check for the ID/Echo for getting settings
        tssPeekSettingsHeader(sensor->com, &id);
        if(id != TSS_BINARY_READ_SETTINGS_ID) {
            internalUpdate(sensor, tryPeekHeader(sensor, &header));
            continue;
        }

        //Check to see if the response after the ID looks like a setting
        num_read_or_err = tss_com_peek_until(sensor->com, TSS_BINARY_SETTINGS_ID_SIZE, '\0', (uint8_t*)buffer, sizeof(buffer));
        if(num_read_or_err <= 0) {
            if(num_read_or_err == TSS_ERR_INSUFFICIENT_BUFFER) {
                //Not enough room to peek the full key, so have to assume it is a success...
                //TODO: Add Warning
                return THREESPACE_AWAIT_COMMAND_FOUND;
            }
            //May just not have enough data yet
            continue;
        }

        if(buffer[num_read_or_err-1] != '\0') {
            internalUpdate(sensor, tryPeekHeader(sensor, &header));
            continue;
        }

        setting = tssGetSetting(buffer);
        if(setting == NULL && strcmp(buffer, TSS_SETTING_KEY_ERR_STRING) != 0) {
            internalUpdate(sensor, tryPeekHeader(sensor, &header));
            continue;
        }

        //Good enough, this is more then likely a GetSetting response
        return THREESPACE_AWAIT_COMMAND_FOUND;
    }

    return THREESPACE_AWAIT_COMMAND_TIMEOUT;
}

static int awaitSetSettingResponse(TSS_Sensor *sensor, uint16_t num_keys)
{
    struct TSS_Header header;
    uint8_t buffer[TSS_BINARY_WRITE_SETTING_RESPONSE_LEN], err, num_success, checksum;
    uint32_t id;
    tss_time_t start_time;

    start_time = tssTimeGet();
    while(tssTimeDiff(start_time) < getTimeout(sensor)) {
        if(comLength(sensor) < TSS_BINARY_WRITE_SETTING_WITH_HEADER_RESPONSE_LEN) {
            continue;
        }
        
        //Check for the ID/Echo for getting settings
        tssPeekSettingsHeader(sensor->com, &id);
        if(id != TSS_BINARY_WRITE_SETTINGS_ID) {
            internalUpdate(sensor, tryPeekHeader(sensor, &header));
            continue;
        }

        //Peek the full response and validate the checksum and proper format
        //No need to check the response length because the length was already validated by the first if statement
        tss_com_peek(sensor->com, TSS_BINARY_SETTINGS_ID_SIZE, TSS_BINARY_WRITE_SETTING_RESPONSE_LEN, buffer);
        err = buffer[0];
        num_success = buffer[1];
        checksum = buffer[2];

        if( (checksum != err + num_success) ||  //Mismatch Checksum
            (err == TSS_SUCCESS && num_success != num_keys) || //Success but not all success?
            (err != TSS_SUCCESS && num_success >= num_keys) || //Not success but all success?
            (num_success > num_keys)) //Invalid num_success value
        {
            internalUpdate(sensor, tryPeekHeader(sensor, &header));
            continue;
        }

        return THREESPACE_AWAIT_COMMAND_FOUND;
    }
    return THREESPACE_AWAIT_COMMAND_TIMEOUT;
}

static int internalUpdate(TSS_Sensor *sensor, const struct TSS_Header *header) {
    if(header != NULL) {
        size_t com_length = comLength(sensor);
        if(sensor->streaming.data.active && header->echo == TSS_STREAMING_DATA_BATCH_COMMAND_NUM) {
            uint16_t expected_out_size = sensor->streaming.data.output_size;
            if(com_length < (uint16_t)(expected_out_size + sensor->header_cfg.size) && com_length < peekCapacity(sensor)) {
                return THREESPACE_UPDATE_COMMAND_NOT_ENOUGH_DATA;
            }
            if(peekValidatePacket(sensor, header, expected_out_size, expected_out_size) == TSS_SUCCESS) {
                sensorInternalUpdateDataStreaming(sensor);
                return THREESPACE_UPDATE_COMMAND_PARSED;
            }
        }
        else if(sensor->streaming.log.active && header->echo == TSS_STREAMING_FILE_READ_BYTES_COMMAND_NUM) {
            uint16_t expected_out_size = (header->length < TSS_LOG_STREAMING_MAX_PACKET_SIZE) ? header->length : TSS_LOG_STREAMING_MAX_PACKET_SIZE;
            if(com_length < (uint16_t)(expected_out_size + sensor->header_cfg.size) && com_length < peekCapacity(sensor)) {
                return THREESPACE_UPDATE_COMMAND_NOT_ENOUGH_DATA;
            }
            if(peekValidatePacket(sensor, header, expected_out_size, expected_out_size) == TSS_SUCCESS) {
                sensorInternalUpdateLogStreaming(sensor);
                return THREESPACE_UPDATE_COMMAND_PARSED;
            }
        }
        else if(sensor->streaming.file.active && header->echo == TSS_STREAMING_FILE_READ_BYTES_COMMAND_NUM) {
            uint16_t expected_out_size = (header->length < TSS_FILE_STREAMING_MAX_PACKET_SIZE) ? header->length : TSS_FILE_STREAMING_MAX_PACKET_SIZE;
            if(com_length < (uint16_t)(expected_out_size + sensor->header_cfg.size) && com_length < peekCapacity(sensor)) {
                return THREESPACE_UPDATE_COMMAND_NOT_ENOUGH_DATA;
            }
            if(peekValidatePacket(sensor, header, expected_out_size, expected_out_size) == TSS_SUCCESS) {
                sensorInternalUpdateFileStreaming(sensor);
                return THREESPACE_UPDATE_COMMAND_PARSED;
            }
        }
    }

    //Check for a debug message before considering misaligned
    if(peekCheckDebugMessage(sensor) == TSS_SUCCESS) {
        sensorInternalUpdateDebugMessage(sensor);
        return THREESPACE_UPDATE_COMMAND_PARSED;
    }

    handleMisalignment(sensor);
    return THREESPACE_UPDATE_COMMAND_MISALIGNED;
}

static int peekCheckDebugMessage(TSS_Sensor *sensor) {
    char buffer[28];
    uint8_t peek_len;
    size_t com_length;
    int err_or_num_read;
    const char *found;

    //Debug Message Format: "%llu Level: 0x%x Module: 0x%x  %s\r\n"
    //The %llu represents the timestamp and can be up to 20 digits
    
    //To initially search for the debug message, look for " Level:" which means
    //need to lock past up to 20 digits, then read 7 chars + null terminator

    //If not enabled, no callback registered, or not enough data to determine if a debug message or not, then skip
    if(!sensor->debug._immediate || sensor->debug.cb == NULL || comLength(sensor) < 7)  {
        //NOTE: If a debug callback is not registered, then debug messages will be treated as unknown data.
        //May want to change this at some point or compare speed of continuing this check to debug messages being generated.
        return TSS_ERR_UNEXPECTED_PACKET_LENGTH;
    }
    
    peek_len = sizeof(buffer) -1;
    com_length = comLength(sensor);
    if(com_length < peek_len) {
        peek_len = (uint8_t)com_length;
    }

    err_or_num_read = tss_com_peek(sensor->com, 0, peek_len, (uint8_t*)buffer);
    if(err_or_num_read <= 0) {
        return TSS_ERR_READ;
    }

    buffer[err_or_num_read] = '\0';

    //Check for " Level:" in the buffer
    found = strstr(buffer, " Level:");
    if(found == NULL) {
        return TSS_ERR_RESPONSE_NOT_FOUND;
    }

    //It was found, now just make sure all characters before it are valid ASCII DIGITS
    while(--found >= buffer) {
        if((*found < '0') || (*found > '9')) {
            return TSS_ERR_UNEXPECTED_CHARACTER;
        }
    }

    return TSS_SUCCESS;
}


//--------------------------------------BOOTLOADER----------------------------------------------
int sensorBootloaderIsActive(TSS_Sensor *sensor, uint8_t *active)
{
    int err;
    err = checkDirty(sensor);
    if(err) return err;
    *active = sensor->_in_bootloader;
    return TSS_SUCCESS;
}

int sensorInternalBootloaderCheckActive(TSS_Sensor *sensor, uint8_t *active)
{
    //This first part primes the potential Automatic Uart Baudrate detection the bootloader does
    TSS_COM_BEGIN_WRITE(sensor->com);
    tss_com_write(sensor->com, (uint8_t*)"UUU", 3);
    TSS_COM_END_WRITE(sensor->com);
    //Sending the ?'s as a setting causes an instant error response in firmware mode, which prevents
    //having to wait for the bootloader response to timeout to know if in bootloader or firmware
    tssGetSettingsWrite(sensor->com, true, "?");

    //Await a setting response with the bootloader check toggled to true
    int result = awaitGetSettingResponse(sensor, 2, true);
    if(result == THREESPACE_AWAIT_BOOTLOADER_FOUND) {
        *active = 1;
        result = TSS_SUCCESS;

        //Clear the up to 6 "OK" responses in the com class
        //Just clear everything, no risk of streaming or anything
        //like that going on because in the bootloader
        tss_com_clear_immediate(sensor->com);
        //sensor->com->in.clear_timeout(sensor->com->user_data, 5000);
    }
    else if(result == THREESPACE_AWAIT_COMMAND_FOUND) {
        *active = 0;
        result = TSS_SUCCESS;

        //Clear the <KEY_ERROR> response, only what is necessary
        //in case doing something like streaming. Don't want to clear too much.
        //On firmware response, receives "<KEY_ERROR>\0\0\x", so len + 3
        char response[TSS_SETTING_KEY_ERR_STRING_LEN + 3];
        uint32_t id;
        tssReadSettingsHeader(sensor->com, &id);
        tss_com_read(sensor->com, TSS_SETTING_KEY_ERR_STRING_LEN + 3, (uint8_t*)response);
    }
    else {
        result = TSS_ERR_READ;
    }

    return result;
}

#else

/* Dummy variable to prevent empty translation unit warning under ISO C */
typedef int tss_dummy_managed_sensor_tu;

#endif /* !TSS_MINIMAL_SENSOR */