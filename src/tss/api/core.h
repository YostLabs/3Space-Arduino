/**
 * @ Author: Andy Riedlinger
 * @ Create Time: 2025-5-22 1:01:00
 *
 * @ Description:
 */

#ifndef __TSS_API_H__
#define __TSS_API_H__

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

#include "tss/export.h"

#include "tss/com/com_class.h"
#include "tss/api/command.h"
#include "tss/api/header.h"

#ifdef __cplusplus
extern "C" {
#endif

TSS_API int tssWriteCommand(struct TSS_Com_Class *com, bool header, const struct TSS_Command *command, const void **data);

//Returns -Error or >= 0 checksum
TSS_API int tssReadCommand(struct TSS_Com_Class *com, const struct TSS_Command *command, ...);
TSS_API int tssReadCommandV(struct TSS_Com_Class *com, const struct TSS_Command *command, va_list args);
TSS_API int tssReadCommandVp(struct TSS_Com_Class *com, const struct TSS_Command *command, va_list *args);
TSS_API int tssReadCommandArray(struct TSS_Com_Class *com, const struct TSS_Command *command, uint16_t *argindex, void **out);
TSS_API int tssReadCommandChecksumOnly(struct TSS_Com_Class *com, const struct TSS_Command *command);


TSS_API int tssReadHeader(struct TSS_Com_Class *com, const struct TSS_Header_Info *header_info, struct TSS_Header *out);
TSS_API int tssPeekHeader(struct TSS_Com_Class *com, const struct TSS_Header_Info *header_info, struct TSS_Header *out);

TSS_API int tssPeekCommandChecksum(struct TSS_Com_Class *com, uint16_t start, uint16_t len);
TSS_API int tssPeekValidateCommand(struct TSS_Com_Class *com, 
    uint8_t header_size, uint16_t header_len_field, uint8_t header_checksum_field, size_t min_data_len, size_t max_data_len);

//Setting Helpers
TSS_API size_t tssBuildGetSettingsString(char *out, size_t out_size, uint16_t count, ...);
TSS_API size_t tssBuildGetSettingsStringV(char *out, size_t out_size, uint16_t count, va_list args);
TSS_API size_t tssBuildGetSettingsStringBinary(uint8_t *out, size_t out_size, uint16_t count, ...);

enum TSS_SettingsCallbackState {
    TSS_SettingsCallbackStateError = -1,
    TSS_SettingsCallbackStateIgnored = 0,
    TSS_SettingsCallbackStateProcessed = 1
};

struct TSS_GetSettingsCallbackInfo {
    struct TSS_Com_Class *com;
    const struct TSS_Setting *setting;
    const char *key;
    uint8_t *checksum;
};

typedef enum TSS_SettingsCallbackState (*TssGetSettingsCallback)(struct TSS_GetSettingsCallbackInfo info, void *user_data);

TSS_API int tssGetSettingsWrite(struct TSS_Com_Class *com, bool header, const char *key_string);
TSS_API int tssGetSettingsRead(struct TSS_Com_Class *com, uint16_t *num_read, ...);
TSS_API int tssGetSettingsReadV(struct TSS_Com_Class *com, uint16_t *num_read, va_list args);
TSS_API int tssGetSettingsReadArray(struct TSS_Com_Class *com, uint16_t *num_read, void **out);
TSS_API int tssGetSettingsReadCb(struct TSS_Com_Class *com, TssGetSettingsCallback callback, void *user_data);

TSS_API int tssSetSettingsWrite(struct TSS_Com_Class *com, bool header, const char **keys, uint8_t num_keys, const void **data);
TSS_API int tssSetSettingsRead(struct TSS_Com_Class *com, struct TSS_Setting_Response *response);

//The settings header is actually just an ID appended to the 
//beggining of the response to help with framing
TSS_API int tssReadSettingsHeader(struct TSS_Com_Class *com, uint32_t *id);
TSS_API int tssPeekSettingsHeader(struct TSS_Com_Class *com, uint32_t *id);

//Low Level Reading Functions
TSS_API int tssReadParams(struct TSS_Com_Class *com, const struct TSS_Param *cur_param, uint8_t *checksum, ...);
TSS_API int tssReadParamsVp(struct TSS_Com_Class *com, const struct TSS_Param *cur_param, uint8_t *checksum, va_list *args);
TSS_API int tssReadParamsArray(struct TSS_Com_Class *com, const struct TSS_Param *cur_param, uint8_t *checksum, uint16_t *argindex, void **outargs);

TSS_API int tssReadParamsChecksumOnly(struct TSS_Com_Class *com, const struct TSS_Param *cur_param, uint8_t *checksum);
TSS_API int tssReadBytesChecksumOnly(struct TSS_Com_Class *com, size_t num_bytes, uint8_t *checksum);

#ifdef __cplusplus
}
#endif

#endif /* __TSS_API_H__ */
