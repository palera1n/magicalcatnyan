#include <common.h>

uint64_t malloc_base = NULL;
uint64_t jit_alloc_base = NULL;
#define JIT_SIZE 65536
#define JIT_BASE ((uint64_t)payload_baseaddr + 458752) 

void *malloc(my_size_t size) {
	void* mem = NULL;
	if (malloc_base == NULL) malloc_base = gBootArgs->topOfKernelData + 1;
	mem = (void*)malloc_base;
	malloc_base += size;
	if (malloc_base > (gBootArgs->physBase + gBootArgs->memSize)) {
		screen_puts("panic: out of memory");
		panic("out of memory\n");
		return NULL;
	}
	return mem;
}

void free(void *ptr) {}

void *jit_alloc(my_size_t count, my_size_t size) {
	void* mem = NULL;
	if (jit_alloc_base == NULL) jit_alloc_base = JIT_BASE;
	mem = (void*)jit_alloc_base;
	jit_alloc_base += count * size;
	if (jit_alloc_base > (JIT_BASE + JIT_SIZE)) {
		screen_puts("panic: out of jit memory");
		panic("out of jit memory\n");
		return NULL;
	}
	return mem;
}

void jit_free(void* ptr) {}