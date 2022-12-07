#include <common.h>

void bzero (void *s, my_size_t n) {
	for (my_size_t i = 0; i < n; i++) {
		*((char*)s + i) = '\0';
	}
}