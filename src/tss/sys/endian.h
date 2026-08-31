#ifndef __TSS_ENDIAN_H__
#define __TSS_ENDIAN_H__

#include <stdint.h>
#include "tss/sys/config.h"
#include "tss/export.h"

//Set endianness based on override or auto detect
#if TSS_ENDIAN_OVERRIDE 
    #define TSS_ENDIAN_CONFIG TSS_ENDIAN_OVERRIDE
#else //Auto detect based on build system
    #ifdef __BYTE_ORDER__
        #if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
            #define TSS_ENDIAN_CONFIG TSS_ENDIAN_BIG
        #elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
            #define TSS_ENDIAN_CONFIG TSS_ENDIAN_LITTLE
        #else
            #error "Unknown Endianness"
        #endif
    #else
        #if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
            #define TSS_ENDIAN_CONFIG TSS_ENDIAN_BIG
        #elif defined(__BIG_ENDIAN__) || defined(__BIG_ENDIAN) || defined(_BIG_ENDIAN)
            #define TSS_ENDIAN_CONFIG TSS_ENDIAN_BIG
        #else
            #error "Undefined Endianness"
        #endif
    #endif
#endif

#if TSS_ENDIAN_CONFIG == TSS_ENDIAN_RUN_TIME
    static const int _endianess_test = 1;
    #define TSS_ENDIAN_IS_BIG ((*(char*)&_endianess_test) == 0)
    #define TSS_ENDIAN_IS_LITTLE !TSS_ENDIAN_IS_BIG
#else
    #define TSS_ENDIAN_IS_LITTLE (TSS_ENDIAN_CONFIG == TSS_ENDIAN_LITTLE)
    #define TSS_ENDIAN_IS_BIG (TSS_ENDIAN_CONFIG == TSS_ENDIAN_BIG)
#endif

#define TSS_ENDIAN_SWAP_DEVICE_TO_BIG(data, size) do { if(TSS_ENDIAN_IS_LITTLE) { tssSwapEndianess(data, size); } } while(false)
#define TSS_ENDIAN_SWAP_DEVICE_TO_LITTLE(data, size) do { if(TSS_ENDIAN_IS_BIG) { tssSwapEndianess(data, size); } } while(false)

#define TSS_ENDIAN_SWAP_BIG_TO_DEVICE(data, size) TSS_ENDIAN_SWAP_DEVICE_TO_BIG(data, size)
#define TSS_ENDIAN_SWAP_LITTLE_TO_DEVICE(data, size) TSS_ENDIAN_SWAP_DEVICE_TO_LITTLE(data, size)

#ifdef __cplusplus
extern "C" {
#endif

TSS_API void tssSwapEndianess(void *data, uint16_t p_size);

#ifdef __cplusplus
}
#endif

#endif /* __TSS_ENDIAN_H__ */