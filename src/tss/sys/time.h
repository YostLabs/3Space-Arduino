/*
*   General time functions that are used by the API to facilitate
*   timeouts for different events.
*/

#ifndef __TSS_TIME_H__
#define __TSS_TIME_H__

#include "tss/export.h"
#include "tss/sys/config.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//The tss_time_t can be any type and in any unit the user wants.
//The only requirement is that tssTimeDiff returns in milliseconds.
typedef uint64_t tss_time_t;


/// @brief Retrieves the current system time
/// @return Time
TSS_API tss_time_t tssTimeGet(void);

/// @brief Returns the time difference between the current
/// system time and the given start time
/// @param start_time The start time as retrieved by tssTimeGet
/// @return Time difference in milliseconds
TSS_API uint32_t tssTimeDiff(tss_time_t start_time);


/// @brief Allows setting the time functions used by the API
/// @param timeGet Returns the current time
/// @param timeDiff Gets the timer difference between the current time and passed time in milliseconds
TSS_API void tssTimeSetFunctions(tss_time_t (*timeGet)(void), uint32_t (*timeDiff)(tss_time_t));

#ifdef __cplusplus
}
#endif

#endif /* __TSS_TIME_H__ */