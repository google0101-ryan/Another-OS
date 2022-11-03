#include <string.h>

#undef strlen

#ifndef STRLEN
#define STRLEN strlen
#endif

size_t STRLEN(const char* str)
{
    const char *s;

    for (s = str; *s; ++s)
        ;
    return (s - str);
}

