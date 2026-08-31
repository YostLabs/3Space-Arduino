#include "tss/sys/endian.h"

void tssSwapEndianess(void *data, uint16_t p_size) {
    uint8_t *buf = data;
    for(uint8_t i = 0; i < p_size / 2; i++) {
        uint8_t tmp = buf[i];
        buf[i] = buf[p_size-1-i];
        buf[p_size-1-i] = tmp;
    }
}