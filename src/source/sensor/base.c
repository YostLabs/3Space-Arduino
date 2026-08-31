/*
*  Common to all sensor types
*/

#include "tss/api/sensor.h"
#include "internal.h"
#include "tss/constants.h"
#include "tss/sys/config.h"
#include "tss/sys/endian.h"
#include "tss/errors.h"
#include "tss/sys/stdinc.h"
#include "tss/sys/time.h"
#include <stdarg.h>

//----------------------------USER FACING--------------------------------

int sensorReadSettings(TSS_Sensor *sensor, const char *key_string, ...) {
    va_list args;
    int result;

    va_start(args, key_string);
    result = sensorReadSettingsV(sensor, key_string, args);
    va_end(args);
    return result;
}

int sensorProcessDataStreamingCallbackOutput(TSS_Sensor *sensor, ...)
{
    va_list outputs;
    int result;

    va_start(outputs, sensor);
    result = sensorInternalReadStreamingBatch(sensor, NULL, outputs);
    va_end(outputs);

    return result;
}

int sensorProcessDataStreamingCallbackOutputArray(TSS_Sensor *sensor, void **outputs)
{
    return sensorInternalReadStreamingBatchArray(sensor, NULL, outputs);
}

int sensorProcessFileStreamingCallbackOutput(TSS_Sensor *sensor, void *output, uint16_t size)
{
    int num_read;
    uint16_t read_len;
    read_len = sensor->streaming.file.remaining_cur_packet_len;
    if(read_len == 0) {
        return 0;
    }
    if(read_len > size) {
        read_len = size;
    }
    num_read = tss_com_read(sensor->com, read_len, output);
    if(num_read < 0) {
        return TSS_ERR_READ;
    }
    sensor->streaming.file.remaining_cur_packet_len -= (uint16_t)num_read;
    if(num_read != (int)read_len) {
        return TSS_ERR_READ;
    }
    return num_read;
}

int sensorProcessDebugCallbackOutput(TSS_Sensor *sensor, char *output, size_t size)
{
    int num_read;
    uint16_t read_len, remaining_len;

    if(sensor->debug.message_processed) {
        return 0;
    }

    remaining_len = TSS_DEBUG_MESSAGE_MAX_SIZE - sensor->debug.bytes_read;
    read_len = (size < remaining_len) ? (uint16_t)size : remaining_len;

    num_read = tss_com_read_until(sensor->com, '\n', (uint8_t*)output, read_len);

    if(num_read > 0) {
        sensor->debug.bytes_read += (uint16_t)num_read;
        sensor->debug.message_processed = (output[num_read-1] == '\n');
        remaining_len -= read_len;
    }
    
    if(remaining_len == 0 && !sensor->debug.message_processed) {
        return TSS_ERR_RESPONSE_NOT_FOUND;
    }

    return num_read;
}

struct ReconnectionInfo {
    uint64_t serial_number;
    TSS_Sensor *out_sensor;

    uint32_t com_timeout;
};

static int discoverReconnectCom(struct TSS_Com_Class *com, void *user_data)
{
    int result;
    struct ReconnectionInfo *info;
    info = user_data;
    result = tss_com_open(com);
    if(result) { //Failed to open
        return TSS_AUTO_DETECT_CONTINUE;
    }
    tss_com_set_timeout(com, info->com_timeout);

    tssCreateSensor(info->out_sensor, com);
    result = tssInitSensor(info->out_sensor);
    if(result != TSS_SUCCESS) { //If just needed to retry, will retry when rediscovered.
        return TSS_AUTO_DETECT_CONTINUE;
    }

    if(info->out_sensor->serial_number == info->serial_number) {
        return TSS_AUTO_DETECT_SUCCESS;
    }
    tss_com_close(com);
    return TSS_AUTO_DETECT_CONTINUE;
}

int sensorReconnect(TSS_Sensor *sensor, uint32_t timeout_ms)
{
    struct ReconnectionInfo info;
    //Cache these before potentially removing them from the com class
    tss_time_t start_time;
    int result;

    if(!sensor->com->reenumerates) {
        //If it doesn't reenumerate, then just ensure the port is open
        //Close first though to clean up any resources that may need released.
        tss_com_close(sensor->com);
        result = tss_com_open(sensor->com);
        if(result != TSS_SUCCESS) return TSS_ERR_DETECTION;
        start_time = tssTimeGet();
        do {
            tss_com_clear_immediate(sensor->com);
            result = tssInitSensor(sensor);
        } while(result != TSS_SUCCESS && tssTimeDiff(start_time) < timeout_ms);
        return result;
    }

    //Sensor does reenumerate, so may have to find a new port. Close and search.
    tss_com_close(sensor->com);
    
    info.serial_number = sensor->serial_number;
    info.out_sensor = sensor;
    info.com_timeout = tss_com_get_timeout(sensor->com);

    start_time = tssTimeGet();
    do {
        result = tss_com_reenumerate(sensor->com, discoverReconnectCom, &info);
    } while(result != TSS_AUTO_DETECT_SUCCESS && tssTimeDiff(start_time) < timeout_ms);

    //No need to initialize the sensor once found because part of finding it involves initializing it
    if(result == TSS_AUTO_DETECT_SUCCESS) {
        return TSS_SUCCESS;
    }
    return TSS_ERR_DETECTION;
}

int sensorCleanup(TSS_Sensor *sensor)
{
    sensorInternalForceStopStreaming(sensor);
    //TODO: Has to work on all models before added here
    //sensorCloseFile(sensor);
    return tss_com_close(sensor->com);
}

//----------------------------BOOTLOADER-----------------------------------------

//NOTE: Bootloader works in Big Endian instead of Little Endian, so the swaps are backwards

#define VALIDATE_BOOTLOADER if(!sensor->_in_bootloader) { return TSS_ERR_NOT_IN_BOOTLOADER; }
int sensorBootloaderGetSerialNumber(TSS_Sensor *sensor, uint64_t *serial_number)
{
    uint8_t buf[9]; //Getting the serial number also returns a line feed
    VALIDATE_BOOTLOADER
    int result;

    TSS_COM_BEGIN_WRITE(sensor->com);
    tss_com_write(sensor->com, (uint8_t*)"Q", 1);
    TSS_COM_END_WRITE(sensor->com);

    result = tss_com_read(sensor->com, sizeof(buf), buf);
    if(result != sizeof(buf)) {
        return TSS_ERR_READ;
    }
    memcpy(serial_number, buf, 8);
    if(TSS_ENDIAN_LITTLE) {
        tssSwapEndianess(serial_number, 8);
    }
    return TSS_SUCCESS;
}

int sensorBootloaderLoadFirmware(TSS_Sensor *sensor, uint32_t timeout_ms)
{
    int result;
    VALIDATE_BOOTLOADER

    TSS_COM_BEGIN_WRITE(sensor->com);
    tss_com_write(sensor->com, (uint8_t*)"B", 1);
    TSS_COM_END_WRITE(sensor->com);

    result = sensorReconnect(sensor, timeout_ms);
    if(result != TSS_SUCCESS) return result;
    if(sensor->_in_bootloader) return TSS_ERR_LOAD_FIRMWARE;

    return TSS_SUCCESS;
}
int sensorBootloaderEraseFirmware(TSS_Sensor *sensor, uint32_t timeout_ms)
{
    uint32_t cached_timeout;
    uint8_t response;
    int num_read;

    VALIDATE_BOOTLOADER

    TSS_COM_BEGIN_WRITE(sensor->com);
    tss_com_write(sensor->com, (uint8_t*)"S", 1);
    TSS_COM_END_WRITE(sensor->com);

    cached_timeout = tss_com_get_timeout(sensor->com);
    tss_com_set_timeout(sensor->com, timeout_ms);
    num_read = tss_com_read(sensor->com, 1, &response);
    tss_com_set_timeout(sensor->com, cached_timeout);

    if(num_read != 1) {
        return TSS_ERR_READ;
    }

    return response;
}

int sensorBootloaderGetInfo(TSS_Sensor *sensor, struct TSS_Bootloader_Info *info)
{
    int result;
    uint8_t buf[12];
    VALIDATE_BOOTLOADER

    TSS_COM_BEGIN_WRITE(sensor->com);
    tss_com_write(sensor->com, (uint8_t*)"I", 1);
    TSS_COM_END_WRITE(sensor->com);

    result = tss_com_read(sensor->com, sizeof(buf), buf);
    if(result != sizeof(buf)) {
        return TSS_ERR_READ;
    }
    memcpy(&info->memstart, buf,        4);
    memcpy(&info->memend,   buf + 4,    4);
    memcpy(&info->pagesize, buf + 8,    2);
    memcpy(&info->version,  buf + 10,   2);
    if(TSS_ENDIAN_IS_LITTLE) {
        tssSwapEndianess(&info->memstart, 4);
        tssSwapEndianess(&info->memend, 4);
        tssSwapEndianess(&info->pagesize, 2);
        tssSwapEndianess(&info->version, 2);
    }
    return TSS_SUCCESS;
}

int sensorBootloaderProgram(TSS_Sensor *sensor, const uint8_t *bytes, uint16_t num_bytes, uint32_t timeout_ms)
{
    uint32_t i, cached_timeout;
    uint16_t checksum, num_bytes_converted;
    int num_read_or_err;
    uint8_t result;

    VALIDATE_BOOTLOADER
    
    //Compute Checksum
    checksum = 0;
    for(i = 0; i < num_bytes; i++) {
        checksum += bytes[i];
    }

    num_bytes_converted = num_bytes;
    TSS_ENDIAN_SWAP_DEVICE_TO_BIG(&num_bytes_converted, sizeof(num_bytes_converted));
    TSS_ENDIAN_SWAP_DEVICE_TO_BIG(&checksum, sizeof(checksum));

    TSS_COM_BEGIN_WRITE(sensor->com);
    tss_com_write(sensor->com, (uint8_t*)"C", 1);
    tss_com_write(sensor->com, (uint8_t*)&num_bytes_converted, sizeof(num_bytes_converted));
    tss_com_write(sensor->com, bytes, num_bytes);
    tss_com_write(sensor->com, (uint8_t*)&checksum, sizeof(checksum));
    TSS_COM_END_WRITE(sensor->com);

    //Wait for response
    cached_timeout = tss_com_get_timeout(sensor->com);
    tss_com_set_timeout(sensor->com, timeout_ms);
    num_read_or_err = tss_com_read(sensor->com, 1, &result);
    tss_com_set_timeout(sensor->com, cached_timeout);

    if(num_read_or_err != 1) {
        return TSS_ERR_READ;
    }

    return result;
}

int sensorBootloaderGetStatus(TSS_Sensor *sensor, uint32_t *status)
{
    int result;
    VALIDATE_BOOTLOADER
    TSS_COM_BEGIN_WRITE(sensor->com);

    //Sends twice because of a bug in an old version of the bootloader.
    //This will cause the fixed version to respond twice.
    tss_com_write(sensor->com, (uint8_t*)"OO", 2);
    TSS_COM_END_WRITE(sensor->com);

    result = tss_com_read(sensor->com, 4, (uint8_t*)status);
    if(result != 4) {
        return TSS_ERR_READ;
    }
    TSS_ENDIAN_SWAP_BIG_TO_DEVICE(status, 4);

    //Clearing the potential second response
    tss_com_clear_immediate(sensor->com);
    return TSS_SUCCESS;
}

int sensorBootloaderRestoreFactorySettings(TSS_Sensor *sensor)
{
    int result;

    VALIDATE_BOOTLOADER
    TSS_COM_BEGIN_WRITE(sensor->com);
    result = tss_com_write(sensor->com, (uint8_t*)"RR", 2);
    TSS_COM_END_WRITE(sensor->com);
    return result;
}

//--------------------------INTERNAL--------------------------------------
void sensorInternalForceStopStreaming(TSS_Sensor *sensor)
{   
    bool header_was_enabled, was_dirty;
    header_was_enabled = sensor->_header_enabled;
    was_dirty = sensor->dirty;
    sensor->_header_enabled = false;
    sensor->dirty = false;

    //These functions return no data, so can be called regardless of if available.
    //This affectively sends these and reads nothing by having header disabled for these
    sensorStreamingStop(sensor);
    sensorFileStreamingStop(sensor);
    sensorLoggingStop(sensor);

    sensor->_header_enabled = header_was_enabled;
    sensor->dirty = was_dirty;
}

//---------------------------------------STREAMING FUNCTIONALITY-------------------------------------------------

int sensorInternalReadStreamingBatch(TSS_Sensor *sensor, const struct TSS_Command *command, va_list outputs) {
    (void) command;
    
    uint8_t checksum = 0;
    const struct TSS_Command **cur_slot = sensor->streaming.data.commands;
    while(*cur_slot != NULL) {
        int err_or_checksum = tssReadCommandVp(sensor->com, *cur_slot, &outputs);
        if(err_or_checksum < 0) {
            return err_or_checksum;
        }
        checksum += (uint8_t)err_or_checksum;
        cur_slot++;
    }

    return checksum;
}

int sensorInternalReadStreamingBatchArray(TSS_Sensor *sensor, const struct TSS_Command *command, void **outputs)
{
    (void) command;
    
    uint8_t checksum = 0;
    const struct TSS_Command **cur_slot = sensor->streaming.data.commands;
    uint16_t argindex = 0;
    while(*cur_slot != NULL) {
        int err_or_checksum = tssReadCommandArray(sensor->com, *cur_slot, &argindex, outputs);
        if(err_or_checksum < 0) {
            return err_or_checksum;
        }
        checksum += (uint8_t)err_or_checksum;
        cur_slot++;
    }

    return checksum;
}

int sensorInternalReadStreamingBatchChecksumOnly(TSS_Sensor *sensor) {
    uint8_t checksum = 0;
    const struct TSS_Command **cur_slot = sensor->streaming.data.commands;
    while(*cur_slot != NULL) {
        int err_or_checksum = tssReadCommandChecksumOnly(sensor->com, *cur_slot);
        if(err_or_checksum < 0) {
            return err_or_checksum;
        }
        checksum += (uint8_t)err_or_checksum;
        cur_slot++;
    }

    return checksum;
}

int sensorInternalUpdateDataStreaming(TSS_Sensor *sensor) {
    sensorInternalHandleHeader(sensor);
    enum TSS_DataCallbackState state = sensor->streaming.data.cb(sensor);
    if(state == TSS_DataCallbackStateIgnored) {
        sensorInternalReadStreamingBatchChecksumOnly(sensor);
    }
    return state;
}

int sensorInternalUpdateFileStreaming(TSS_Sensor *sensor)
{
    uint16_t packet_len;

    sensorInternalHandleHeader(sensor);

#if TSS_MINIMAL_SENSOR
    if(sensor->streaming.file.remaining_len > TSS_FILE_STREAMING_MAX_PACKET_SIZE) {
        packet_len = TSS_FILE_STREAMING_MAX_PACKET_SIZE;
    }
    else {
        packet_len = sensor->streaming.file.remaining_len;
    }
#else
    packet_len = sensor->last_header.length;
#endif
    sensor->streaming.file.remaining_cur_packet_len = packet_len;

    enum TSS_DataCallbackState state = sensor->streaming.file.cb(sensor);
    if(sensor->streaming.file.remaining_cur_packet_len > 0) { //Read unread data out
        tssReadBytesChecksumOnly(sensor->com, sensor->streaming.file.remaining_cur_packet_len, NULL);
    }

    sensor->streaming.file.remaining_len -= packet_len;
    if(sensor->streaming.file.remaining_len == 0) {
        sensor->streaming.file.active = false;
    }

    return state;
}

int sensorInternalUpdateLogStreaming(TSS_Sensor *sensor) {
    if(sensor->streaming.log.header_enabled) {
        tssReadHeader(sensor->com, &sensor->header_cfg, &sensor->last_header);
    }
    enum TSS_DataCallbackState state = sensor->streaming.log.cb(sensor);
    if(state == TSS_DataCallbackStateIgnored) {
#if TSS_MINIMAL_SENSOR
        tss_com_clear_immediate(sensor->com);
#else
        tssReadBytesChecksumOnly(sensor->com, sensor->last_header.length, NULL);
#endif
    }
    return state;
}

static int consumeDebugMessage(TSS_Sensor *sensor)
{
    char buffer[40];
    while(!sensor->debug.message_processed) {
        int result = sensorProcessDebugCallbackOutput(sensor, buffer, sizeof(buffer));
        if(result < 0) {
            return result;
        }
    }
    return 0;
}

int sensorInternalUpdateDebugMessage(TSS_Sensor *sensor) {
    sensor->debug.bytes_read = 0;
    sensor->debug.message_processed = false;
    sensor->debug.cb(sensor);
    if(!sensor->debug.message_processed) {
        consumeDebugMessage(sensor);
    }
    return 0;
}

//------------------------WRAPPERS---------------------------------
int sensorInternalExecuteCommand(TSS_Sensor *sensor, const struct TSS_Command *command, const void **input, ...)
{
    va_list args;
    int result;

    va_start(args, input);
    result = sensorInternalExecuteCommandV(sensor, command, input, args);
    va_end(args);
    return result;
}

int sensorInternalExecuteCommandV(TSS_Sensor *sensor, const struct TSS_Command *command, const void **input, va_list outputs)
{
    return sensorInternalExecuteCommandCustomV(sensor, command, input, sensorInternalBaseCommandRead, outputs);
}

int sensorInternalExecuteCommandCustom(TSS_Sensor *sensor, const struct TSS_Command *command, const void **input, SensorInternalReadFunction read_func, ...)
{
    va_list args;
    int result;

    va_start(args, read_func);
    result = sensorInternalExecuteCommandCustomV(sensor, command, input, read_func, args);
    va_end(args);
    return result;
}