#include "common.h"

char *strncpy(char *dest, const char *src, my_size_t n) {
  my_size_t i;
  // copy src to dest
  for (i = 0; i < n && src[i] != '\0'; i++)
    dest[i] = src[i];
  // fill the rest wiih NULL bytes
  for ( ; i < n; i++)
    dest[i] = '\0';
  return dest;
}