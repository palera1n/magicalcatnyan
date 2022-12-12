#include <common.h>

/*
	This memory allocation system hardcodes two tables with a list of allocated
	pointers and free memory respectively. These tables can each hold 255 entries
	and may have a pointer to the next table, so the amount of allocation should
	only be limited by the amount of contiguous memory available.

	Memory layout:
	============================
	|  iBoot payload, firmware |
	============================
	|           XNU            |
	============================
	| first free memory table  |
	============================
	|  first allocation table  |
	============================
	|   Allocatable memory,    |
	|    including all other   |
	| free/allocation tables   |
	============================
*/

typedef struct free_region {
	uint8_t valid ;
	void* bottom;
	void* top;
} free_region_t;

typedef struct alloc_region {
	uint8_t valid;
	void* addr;
	my_size_t size;
} alloc_region_t;

typedef struct alloc_region_table {
	uint8_t count;
	alloc_region_t entries[UINT8_MAX]; 
	struct alloc_region_table* next_alloc_table;
} alloc_table_t;

typedef struct free_table {
	uint8_t count;
	free_region_t entries[UINT8_MAX];
	struct free_table* next_free_table;
} free_table_t;

typedef struct free_entry_info {
	uint8_t valid;
	free_table_t* free_table;
	free_region_t* entry;
} free_entry_info_t;

free_table_t* first_free_table = NULL;
alloc_table_t* first_alloc_table = NULL;
uint64_t memory_size;
uint64_t alloc_base;
uint64_t alloc_end;

void new_free_table(free_table_t* table) {
	table->count = 0;
	table->next_free_table = NULL;
	for (uint8_t i = 0; i < UINT8_MAX; i++) {		
		table->entries[i].valid = 0;
	}
	return ;
}

void new_alloc_table(alloc_table_t* table) {
	table->count = 0;
	table->next_alloc_table = NULL;
	for (uint8_t i = 0; i < UINT8_MAX; i++) {		
		table->entries[i].valid = 0;
	}
	return ;
}

int mm_init() {
	if (first_free_table != NULL) return 0;
	memory_size = gBootArgs->memSize;
	alloc_base = (gBootArgs->topOfKernelData + 1 + sizeof(free_table_t) + sizeof(alloc_table_t));
	alloc_end = 0x800000000ULL + gBootArgs->memSize;
	// note the first alloc and free table is under alloc_base and will never be freed
	new_free_table((free_table_t*)alloc_base);
	new_alloc_table((alloc_table_t*)(alloc_base + sizeof(free_table_t)));
	first_free_table = (free_table_t*)alloc_base;
	first_free_table->count = 1;
	first_free_table->entries[0].bottom = (void*)alloc_base;
	first_free_table->entries[0].top = (void*)alloc_end;
	return 0;
}


static void* malloc_in_free_table(free_table_t* table, my_size_t size) {
	void* ret = NULL;
	for (uint8_t i = 0; i < UINT8_MAX; i++) {
		free_region_t* block = &table->entries[i];
		if (block->valid != 1) continue;
		if ((my_size_t)(block->top - block->bottom) > size) {
			ret = block->bottom;
			block->bottom += size;
			break;
		} else if ((my_size_t)(block->top - block->bottom) == size) {
			ret = block->bottom;
			block->valid = 0;
			block->bottom = 0;
			block->top = 0;
			table->count--;
			if (table->count == 0 && table != first_free_table) free(table);
			break;
		}
	}
	return ret;
}

static int write_alloc_table(alloc_table_t* table, void* ptr, my_size_t size) {
	int has_written = 0;
	for (uint8_t i = 0; i < UINT8_MAX; i++) {
		alloc_region_t* block = &table->entries[i];
		if (block->valid == 1) continue;
		block->valid = 1;
		block->addr = ptr;
		block->size = size;
		has_written = 1;
		table->count += 1;
	}
	if (has_written == 0) return -1;
	else return 0;
}

void* malloc(my_size_t size) {
	if (size == 0) panic("malloc(0)\n");
	void* ret = NULL;
	uint8_t oom_table_alloc = 0;
	uint32_t free_table_count = 0;
	alloc_table_t* current_alloc_table = first_alloc_table;
	free_table_t* current_free_table = first_free_table;
	for (; current_free_table != NULL; current_free_table = current_free_table->next_free_table) {
		printf("current_free_table @ %p\n", current_free_table);
		void* addr = malloc_in_free_table(current_free_table, size);
		free_table_count += 1;
		if (addr != NULL) {
			ret = addr;
			break;
		}
	}
	if (ret == NULL) goto oom;
	current_free_table = first_free_table;
	for (; current_alloc_table->count != UINT8_MAX; current_alloc_table = current_alloc_table->next_alloc_table) {
		if (current_alloc_table == NULL) {
			// this really should be unreachable...
			panic("Null allocation table\n");
			while(1) {};
		}
		if (current_alloc_table->count == (UINT8_MAX - 1) && current_alloc_table->next_alloc_table == NULL) {
			for (; current_free_table != NULL; current_free_table = current_free_table->next_free_table) {
				// malloc is just shrinking or removing free entries, so won't run out of table slots
				void* addr = malloc_in_free_table(current_free_table, sizeof(alloc_table_t));
				current_free_table = NULL;
				if (addr != NULL) {
					current_alloc_table->next_alloc_table = addr;
				}
			}
			// fully full tables with no next table should not exist unless OOM
			if (current_alloc_table->next_alloc_table == NULL) {
				oom_table_alloc = 1;
				goto oom;
			}
			new_alloc_table(current_alloc_table->next_alloc_table);
			// now, the current table will be full, and the new one will be used instead
			write_alloc_table(current_alloc_table, current_alloc_table->next_alloc_table, sizeof(alloc_table_t));
			continue;
		} else {
			// write the newly made allocation into the allocation table
			write_alloc_table(current_alloc_table, ret, size);
		}
	}
	return ret;
oom: {
		char reason[200];
		char panicString[2000];
		if (ret == NULL) {
			strcpy(reason, "Not enough memory to be allocated to caller");
		} else if (oom_table_alloc == 1) {
			strcpy(reason, "Cannot allocate memory for allocation table");
		}
		char panic_fmt[] =
			"Out of contiguous memory of size %llu\n"
			"Number of free tables: %u\n"
			"first_alloc_table @ %p\n"
			"first_free_table @ %p\n"
			"Reason: %s\n";
		sprintf(panicString, panic_fmt, size, free_table_count, first_alloc_table, first_free_table, reason);
		if (screen_is_initialized == true) {
			screen_write("panic: ");
			screen_puts(panicString);
		}
		while(1) {};
		real_panic("%s", panicString);
	}
}

void* calloc(my_size_t nitems, my_size_t size) {
	void* ptr = malloc(size * nitems);
	if (ptr) bzero(ptr, size * nitems);
	return ptr;
}

static free_table_t* find_prev_free_table(free_table_t* table) {
	free_table_t* current_free_table = first_free_table;
	if (table == first_free_table) return NULL;
	for (; current_free_table->next_free_table != NULL; current_free_table = current_free_table->next_free_table) {
		if (current_free_table->next_free_table == table) break;
	}
	return current_free_table;
}

static alloc_table_t* find_prev_alloc_table(alloc_table_t* table) {
	alloc_table_t* current_alloc_table = first_alloc_table;
	for (; current_alloc_table->next_alloc_table != NULL; current_alloc_table = current_alloc_table->next_alloc_table) {
		if (current_alloc_table->next_alloc_table == table) break;
	}
	return current_alloc_table;
}

void free(void* ptr) {
	alloc_table_t* current_alloc_table = first_alloc_table;
	free_table_t* current_free_table = first_free_table;
	my_size_t size = 0;
	for (; current_alloc_table != NULL; current_alloc_table = current_alloc_table->next_alloc_table) {
		for (uint8_t i = 0; i < UINT8_MAX; i++) {
			if (current_alloc_table->entries[i].valid != 1) continue;
			if (current_alloc_table->entries[i].addr != ptr) continue;
			// zero-on-free
			size = current_alloc_table->entries[i].size;
			bzero(current_alloc_table->entries[i].addr, size);
			current_alloc_table->entries[i].valid = 0;
			current_alloc_table->count--;
		}
	}
	free_entry_info_t above_free;
	free_entry_info_t below_free;
	above_free.valid = 0;
	below_free.valid = 0;
	/*
	There are 4 cases to consider
	1. We are at the bottom of a free memory region, we merge the newly freed memory into it.
	=============================
	|  used memory/alloc_start  |
	=============================
	|        freed memory       |
	=============================
	|     more free memory      |
	=============================
	2. We are at the top of another free memory region, we also merge the freed memory into it.
	=============================
	|      more free memory     |
	=============================
	|        freed memory       |
	=============================
	|   used memory/alloc_end   |
	=============================
	3. We are sandwiched between two free memory regions, we merge the 3 regions together.
	=============================
	|     more free memory      |
	=============================
	|       freed memory        |
	=============================
	|   even more free memory   |
	=============================
	4. The block of memory being freed is not next to any other free region.
	In this case, we just make a new entry in the free table.
	=============================
	|   used memory/alloc_base  |
	=============================
	|        freed memory       |
	=============================
	|   used memory/alloc_end   |
	=============================
	*/
	for (; current_free_table != NULL; current_free_table = current_free_table->next_free_table) {
		if (current_free_table == NULL) {
			panic("Null free table\n");
		}
		for (uint8_t i = 0; i < UINT8_MAX; i++) {
			if (above_free.valid == 1 && below_free.valid == 1) break;
			if (current_free_table->entries[i].valid != 1) {
				continue;
			} else if ((current_free_table->entries[i].bottom - 1) == ((char*)ptr + size)) {
				above_free.free_table = current_free_table;
				above_free.entry = &current_free_table->entries[i];
			} else if ((current_free_table->entries[i].top + 1) == ptr) {
				below_free.free_table = current_free_table;
				below_free.free_table = current_free_table;
			}
		}
	}
	current_free_table = first_free_table;
	if (above_free.valid == 1 && below_free.valid == 0) {
		above_free.entry->bottom -= size;
	} else if (above_free.valid == 0 && below_free.valid == 1) {
		below_free.entry->top += 1;
		below_free.entry->top += size;
	} else if (above_free.valid == 1 && below_free.valid == 1) {
		above_free.entry->bottom = below_free.entry->bottom;
		below_free.entry->valid = 0;
		below_free.free_table->count--;
		if (below_free.free_table->count == 0 && below_free.free_table->next_free_table == NULL && below_free.free_table != first_free_table) {
			free_table_t* prev_table = find_prev_free_table(below_free.free_table);
			if (prev_table->count < UINT8_MAX) {
				free(below_free.free_table);
				prev_table->next_free_table = NULL;
			}
		}
	} else {
		uint8_t freed = 0;
		for (; current_free_table != NULL; current_free_table = current_free_table->next_free_table) {
			if (current_free_table == NULL) {
				panic("Null free table\n");
			}
			if (current_free_table->count == UINT8_MAX && current_free_table->next_free_table) continue;
			if (current_free_table->count == UINT8_MAX && current_free_table->next_free_table == NULL) {
				current_free_table->next_free_table = malloc(sizeof(free_table_t));
				new_free_table(current_free_table->next_free_table);
				continue;
			}
			for (uint8_t i = 0; i < UINT8_MAX; i++) {
				if (current_free_table->entries[i].valid == 1) continue;
				current_free_table->entries[i].bottom = ptr;
				current_free_table->entries[i].top = (char*)ptr + size;
				current_free_table->entries[i].valid = 1;
				current_free_table->count++;
				freed = 1;
				break;
			}
			if (!freed) {
				printf("free: unable to free memory of size %llu at 0x%x, current_free_table->count: %u is probably incorrect\n", size, ptr, current_free_table->count);
				break;
			}
		}

	}
	if (current_alloc_table->count == 0 && current_alloc_table != first_alloc_table && current_alloc_table->next_alloc_table == NULL) {
		alloc_table_t* prev_table = find_prev_alloc_table(current_alloc_table);
		if (prev_table->count < UINT8_MAX) {
			free(current_alloc_table);
			prev_table->next_alloc_table = NULL;
		}
	}

}