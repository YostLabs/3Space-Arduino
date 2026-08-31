/**
 * @ Author: Andy Riedlinger
 * @ Create Time: 2025-5-23 11:58:00
 *
 * @ Description:
 * Configuration settings for building the TSS API
 */

#ifndef __TSS_CONFIG_H__
#define __TSS_CONFIG_H__

//If enabled, TSS_Params will also include their type.
//The API does not require this to function, but it can be helpful
//for the user to write more dynamic function handlers. The only reason to
//turn this off is if on a memory limited platform.
#define TSS_INCLUDE_PARAM_TYPE 1

//If enabled, standard library functions will be used,
//else un-optimized, less efficent versions will be used as a fallback.
//The used functions are string functions, look at tss_string.c to see
//what is used.
#define TSS_STDC_AVAILABLE 1

//Com classes will implement the begin, write, end protocol
//Useful when using communication interfaces like SPI and I2C
//TODO: Make me default and modify serial com class to just never buffer
//Disabling this will just be a small speed improvement
#define TSS_BUFFERED_WRITES 1

//Switches between building with minimal sensor functionality
//and robust full capability functionality.
//Minimal:
//  Pros:
//      Slightly Faster
//      Does not require implementing peek functions in the Com Class.
//  Cons:
//      Requires more manual calls (EG Does not auto recover from errors. Must be notified when cached settings need updated).
//          Should manually clear all data from the Com Class when stopping streaming when streaming at high speeds.
//      Can not stream and process additional commands at the same time, or stream using multiple methods at the same time (EG Data + File streaming)
//      Can not use the immediate debug mode.
//      Does not validate incoming data
//      UpdateStreaming must be called at the same rate as the streaming is set. Reads 1 packet at a time.
//          This is due to the Com Class not implementing the length() function which would often require extra buffering.
//Regular:
//  Pros:
//      Robust validation, data is guranteed accurate and buffer corruption is automatically managed.
//      Can parse any number of commands at the same time. (Multi-Streaming, Commands at the same time as streaming, ...).
//          This includes immediate debug mode.
//      Automatically manages the cached settings.
//  Cons:
//      Slightly slower
//      Requires implementing the peek functions in the Com Class.
//      Forces the command response header to always be enabled and to always have certain bits enabled.
//          (These bits are automatically handled, the user is not required to worry about the configuration.)
#define TSS_MINIMAL_SENSOR 0

//Types
#define TSS_ENDIAN_AUTO_DETECT 0
#define TSS_ENDIAN_RUN_TIME 1
#define TSS_ENDIAN_LITTLE 2
#define TSS_ENDIAN_BIG 3

//Set this to manually change endian build target.
//It is recommended to set this to the endian type of the system
//this will be ran on if you know ahead of time for speed improvements.
#define TSS_ENDIAN_OVERRIDE TSS_ENDIAN_RUN_TIME

#endif /* __TSS_CONFIG_H__ */