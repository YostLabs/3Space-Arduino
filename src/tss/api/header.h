#ifndef __TSS_HEADER_H__
#define __TSS_HEADER_H__

#include "tss/export.h"
#include <stdint.h>

#define TSS_HEADER_STATUS_BIT_POS       0
#define TSS_HEADER_TIMESTAMP_BIT_POS    1
#define TSS_HEADER_ECHO_BIT_POS         2
#define TSS_HEADER_CHECKSUM_BIT_POS     3
#define TSS_HEADER_SERIAL_BIT_POS       4
#define TSS_HEADER_LENGTH_BIT_POS       5

#define TSS_HEADER_STATUS_BIT       (1 << TSS_HEADER_STATUS_BIT_POS)
#define TSS_HEADER_TIMESTAMP_BIT    (1 << TSS_HEADER_TIMESTAMP_BIT_POS)
#define TSS_HEADER_ECHO_BIT         (1 << TSS_HEADER_ECHO_BIT_POS)
#define TSS_HEADER_CHECKSUM_BIT     (1 << TSS_HEADER_CHECKSUM_BIT_POS)
#define TSS_HEADER_SERIAL_BIT       (1 << TSS_HEADER_SERIAL_BIT_POS)
#define TSS_HEADER_LENGTH_BIT       (1 << TSS_HEADER_LENGTH_BIT_POS)

struct TSS_Header_Info {
    uint8_t bitfield;
    uint8_t size;
};

#define TSS_HEADER_MAX_SIZE 13 
struct TSS_Header {
    int8_t status;
    uint32_t timestamp;
    uint8_t echo;
    uint8_t checksum;
    uint32_t serial;
    uint16_t length;
};

#ifdef __cplusplus
extern "C" {
#endif

TSS_API struct TSS_Header_Info tssHeaderInfoFromBitfield(uint8_t bitfield);
TSS_API uint8_t tssHeaderSizeFromBitfield(uint8_t bitfield);
TSS_API uint8_t tssHeaderPosFromBitfield(uint8_t bitfield, uint8_t bit);
TSS_API void tssHeaderFromBytes(const struct TSS_Header_Info *info, uint8_t *data, struct TSS_Header *out);

#ifdef __cplusplus
}
#endif

#endif /* __TSS_HEADER_H__ */
