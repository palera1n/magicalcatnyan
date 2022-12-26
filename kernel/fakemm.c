#define PAYLOAD_t8015 1
#define BUILDING_PAYLOAD 1
#include <common.h>

uint64_t malloc_base = NULL;
uint64_t jit_alloc_base = NULL;
#define JIT_SIZE 8192
#define JIT_OFFSET 0x2e4
#define JIT_BASE (PAYLOAD_BASE_ADDRESS + JIT_OFFSET)

void *malloc(my_size_t size) {
	void* mem = NULL;
	dprintf("malloc: size = %x\n", size);
	if (malloc_base == NULL) malloc_base = gBootArgs->topOfKernelData + 1;
	dprintf("malloc: malloc_base = %p\n", malloc_base);
	mem = (void*)malloc_base;
	malloc_base += size;
	if (malloc_base > (gBootArgs->physBase + gBootArgs->memSize)) {
		panic("out of memory\n");
		return NULL;
	}
	return mem;
}

void free(void *ptr) {}

void *jit_alloc(my_size_t count, my_size_t size) {
	void* mem = NULL;
	dprintf("jit_alloc: count = %x, size = %x\n", count, size);
	if (jit_alloc_base == NULL) jit_alloc_base = JIT_BASE;
	dprintf("jit_alloc: jit_alloc_base = %p\n", jit_alloc_base);
	mem = (void*)jit_alloc_base;
	jit_alloc_base += count * size;
	if (jit_alloc_base > (JIT_BASE + JIT_SIZE)) {
		panic("out of jit memory\n");
		return NULL;
	}
	return mem;
}

void jit_free(void* ptr) {}