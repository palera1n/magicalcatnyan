#include <common.h>

char *strcpy(char *to, const char *from)
{
    if (to == NULL) {
        return NULL;
    }
    const char* ptr = from;
    uint64_t i = 0;
    for (; from[i] != '\0'; i++) {
        to[i] = *ptr;
        ptr++;
    }
    to[i] = '\0';
    return to;
}

