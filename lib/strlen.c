
#include <common.h>

my_size_t strlen(const char * str) {
    my_size_t len = 0;
    while (*str++) {
        len++;
    }
    
    return len;
}
