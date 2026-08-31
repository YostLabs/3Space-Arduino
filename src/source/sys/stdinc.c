#include "tss/sys/stdinc.h"
#if TSS_STDC_AVAILABLE == 0

size_t strlen(const char *string)
{
    const char *start = string;
    while(*string++ != '\0');
    return (string - start) - 1; //-1 to compensate for the ++ in the check
}

int strcmp (const char* str1, const char* str2)
{
    while(*str1 == *str2) {
        if(*str1 == '\0') {
            return 0;
        }
        str1++;
        str2++;
    }
    return (*str1 < *str2) ? -1 : 1;
}

int tolower(int c)
{
    if(c >= 'A' && c <= 'Z') {
        return c | 32;
    }
    return c;
}

#endif

size_t tssStrLenUntil(const char *str, char value) {
    size_t len;
    
    len = 0;
    while(*str != '\0' && *str != value) {
        len++; str++;
    }
    return len;
}
