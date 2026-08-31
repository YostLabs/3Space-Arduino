#ifndef __TSS_FIRMWARE_H__
#define __TSS_FIRMWARE_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "tss/export.h"
#include "tss/api/sensor.h"

#define TSS_FIRMWARE_UPLOAD_IN_PROGRESS 0
#define TSS_FIRMWARE_UPLOAD_COMPLETE 1

struct TSS_Firmware_Uploader {
    TSS_Sensor *sensor;

    uint8_t *buffer;

    //Minimum size of 16, must be a power of 2
    uint16_t buffer_size;
    uint16_t index;

    int status;
    uint32_t packet_count;

    //Configurable timeouts
    uint32_t timeout_erase_ms; //Defaults to 20000
    uint32_t timeout_program_ms; //Defaults to 5000
    uint32_t timeout_load_firmware_ms; //Defaults to 2000
};

#ifdef __cplusplus
extern "C" {
#endif

TSS_API int tssFirmwareUploaderCreate(TSS_Sensor *sensor, uint8_t *buffer, size_t buffer_size, struct TSS_Firmware_Uploader *out);
TSS_API int tssFirmwareUpload(struct TSS_Firmware_Uploader *uploader, const char *data, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* __TSS_FIRMWARE_H__ */