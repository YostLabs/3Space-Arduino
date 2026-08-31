#ifndef __TSS_STRING_H__
#define __TSS_STRING_H__

#include "tss/sys/config.h"
#include <stddef.h>

//Meant as a way of building if the standard library is not available for the using device.
//Note that these functions are not optimized and will provide worse performance
//then the standard library. To use the standard library, set TSS_STDC_AVAILABLE=1 in tss/sys/config.h

#if TSS_STDC_AVAILABLE
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#else

#define printf(...) {}

size_t strlen(const char *string);
int strcmp (const char* str1, const char* str2);
int tolower(int c);

#endif

size_t tssStrLenUntil(const char *str, char value);

#endif /* __TSS_STRING_H__ */