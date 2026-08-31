#include "tss/tools/firmware.h"
#include "tss/errors.h"
#include "tss/sys/stdinc.h"

#define POW_2(x) ((x) && ((x) & ((x) - 1)) == 0)

#define STATUS_NONE 0
#define STATUS_TAG 1
#define STATUS_DATA 2
#define STATUS_COMPLETE 3

#define MAX_DATA_PACKET_SIZE 4096

int tssFirmwareUploaderCreate(TSS_Sensor *sensor, uint8_t *buffer, size_t buffer_size, struct TSS_Firmware_Uploader *out)
{
    if(!POW_2(buffer_size)) {
        return TSS_ERR_INVALID_SIZE;
    }
    if(buffer_size > MAX_DATA_PACKET_SIZE * 2) {
        buffer_size = MAX_DATA_PACKET_SIZE * 2; //Clamp 
    }

    *out = (struct TSS_Firmware_Uploader) {
        .sensor = sensor,
        .buffer = buffer,
        .buffer_size = (uint16_t)buffer_size, //Clamped to this range above
        .timeout_erase_ms = 20000,
        .timeout_load_firmware_ms = 2000,
        .timeout_program_ms = 5000
    };
    
    return TSS_SUCCESS;
}

inline static int parse(struct TSS_Firmware_Uploader *uploader, char c);
inline static int parseNone(struct TSS_Firmware_Uploader *uploader, char c);
inline static int parseTag(struct TSS_Firmware_Uploader *uploader, char c);
inline static int parseData(struct TSS_Firmware_Uploader *uploader, char c);

inline static int processTag(struct TSS_Firmware_Uploader *uploader);

int tssFirmwareUpload(struct TSS_Firmware_Uploader *uploader, const char *data, size_t len)
{
    for(size_t i = 0; i < len; i++) {
        int result = parse(uploader, data[i]);
        if(result) return result;
    }
    return TSS_FIRMWARE_UPLOAD_IN_PROGRESS;
}

inline static int parse(struct TSS_Firmware_Uploader *uploader, char c)
{
    switch (uploader->status)
    {
    case STATUS_NONE:
        return parseNone(uploader, c);
    case STATUS_TAG:
        return parseTag(uploader, c);
    case STATUS_DATA:
        return parseData(uploader, c);
    default:
        return TSS_ERR_FIRMWARE_UPLOAD_INVALID_FORMAT;
    }
}

inline static int buffer_add(struct TSS_Firmware_Uploader *uploader, char c) {
    if(uploader->index >= uploader->buffer_size) return -1;
    uploader->buffer[uploader->index++] = c;
    return TSS_FIRMWARE_UPLOAD_IN_PROGRESS;
}

inline static int parseNone(struct TSS_Firmware_Uploader *uploader, char c) {
    if(c == '<') {
        uploader->status = STATUS_TAG;
        uploader->index = 0;
    }
    return TSS_FIRMWARE_UPLOAD_IN_PROGRESS;
}

inline static int parseTag(struct TSS_Firmware_Uploader *uploader, char c) {
    switch(c) {
        case '/': //End of TAG (Or an end tag which is ignored)
        case '>':
            if(buffer_add(uploader, '\0')) return TSS_ERR_BUFFER_OVERFLOW; //Null terminate the tag and check
            return processTag(uploader);
        default:
            if(buffer_add(uploader, c)) return TSS_ERR_BUFFER_OVERFLOW;
            return TSS_SUCCESS;
    }
}

inline static int hexCharToValue(uint8_t c) {
    if ('0' <= c && c <= '9') return (c - '0');
    if ('A' <= c && c <= 'F') return (c - 'A' + 10);
    if ('a' <= c && c <= 'f') return (c - 'a' + 10);
    return -1;
}

inline static int hexStringToBytes(uint8_t *buffer, uint16_t buffer_size) {
    for(uint32_t i = 0, j = 0; j < buffer_size; i++, j+= 2) {
        int high = hexCharToValue(buffer[j]);
        int low = hexCharToValue(buffer[j+1]);
        if(high < 0 || low < 0) return TSS_ERR_UNEXPECTED_CHARACTER;
        buffer[i] = (uint8_t)((high << 4) | (low));
    }
    return TSS_SUCCESS;
}

inline static int parseData(struct TSS_Firmware_Uploader *uploader, char c) {
    if(c != '<') {
        buffer_add(uploader, c);
    }
    if(uploader->index == uploader->buffer_size || c == '<') {
        if(uploader->index & 1) return TSS_ERR_FIRMWARE_UPLOAD_INVALID_FORMAT; //Must be a multiple of 2
        int err = hexStringToBytes(uploader->buffer, uploader->index);
        if(err) return err;
        err = sensorBootloaderProgram(uploader->sensor, uploader->buffer, uploader->index / 2, uploader->timeout_program_ms);   
        if(err) {
            return TSS_FIRMWARE_UPLOAD_COMPLETE;
        }
        uploader->index = 0;
        uploader->packet_count++;
    }

    if(c == '<') {
        uploader->status = STATUS_NONE;
    }

    return TSS_FIRMWARE_UPLOAD_IN_PROGRESS;
}

inline static int processTag(struct TSS_Firmware_Uploader *uploader)
{
    int err;
    uploader->status = STATUS_NONE;

    if(strcmp((char*)uploader->buffer, "SetAddr") == 0) {
        err = sensorBootloaderEraseFirmware(uploader->sensor, uploader->timeout_erase_ms);
        if(err) {
            return TSS_ERR_FIRMWARE_UPLOAD_ERASE;
        }
        return TSS_FIRMWARE_UPLOAD_IN_PROGRESS;
    }
    else if(strcmp((char*)uploader->buffer, "MemProgC") == 0) {
        uploader->status = STATUS_DATA;
        uploader->index = 0;
        uploader->packet_count = 0;
        return TSS_FIRMWARE_UPLOAD_IN_PROGRESS;
    }
    else if(strcmp((char*)uploader->buffer, "Run") == 0) {
        err = sensorBootloaderLoadFirmware(uploader->sensor, uploader->timeout_load_firmware_ms);
        uploader->status = STATUS_COMPLETE;
        if(err) {
            return TSS_ERR_LOAD_FIRMWARE;
        }
        return TSS_FIRMWARE_UPLOAD_COMPLETE;
    }

    //Ignore any other tags
    return TSS_FIRMWARE_UPLOAD_IN_PROGRESS;
}