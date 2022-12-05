#ifndef OFFSETFINDER_H
#define OFFSETFINDER_H

#include <stdint.h>
#include <common.h>

uint64_t find_printf(uint64_t region, uint8_t* data, my_size_t size);
uint64_t find_mount_and_boot_system(uint64_t region, uint8_t* data, my_size_t size);
uint64_t find_jumpto_bl(uint64_t region, uint8_t* data, my_size_t size);
uint64_t find_jumpto_func(uint64_t region, uint8_t* data, my_size_t size);
uint64_t find_panic(uint64_t region, uint8_t* data, my_size_t size);

#endif
