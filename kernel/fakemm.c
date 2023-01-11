#include <common.h>

uint64_t malloc_base = (uint64_t)NULL;
uint64_t jit_alloc_base = (uint64_t)NULL;
#define JIT_SIZE 32768
#define JIT_BASE ((uint64_t)payload_baseaddr + 491520) 

void *malloc(my_size_t size) {
	void* mem = NULL;
	if ((void*)malloc_base == NULL) panic("mm is not initialized yet\n");
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
	if (jit_alloc_base == (uint64_t)NULL) panic("mm is not initialized yet\n");
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

void mm_init() {
	malloc_base = gBootArgs->topOfKernelData;
	jit_alloc_base = JIT_BASE;
}

void mem_stat() {
	dprintf("Memory usage: %llu bytes\n", (malloc_base - (gBootArgs->topOfKernelData + 1)));
	dprintf("JIT memory usage: %llu bytes\n", jit_alloc_base - JIT_BASE);
	return;
}
