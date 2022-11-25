#ifndef OFFSETFINDER_H
#define OFFSETFINDER_H

#include <stdint.h>
#include <common.h>

uint64_t find_zero_region(uint64_t region, uint8_t* data, size_t size);

uint64_t find_printf(uint64_t region, uint8_t* data, size_t size);
uint64_t find_mount_and_boot_system(uint64_t region, uint8_t* data, size_t size);
uint64_t find_check_bootmode(uint64_t region, uint8_t* data, size_t size);
uint64_t find_fuse_lock(uint64_t region, uint8_t* data, size_t size);
uint64_t find_get_fuse_lock(uint64_t region, uint8_t* data, size_t size);
uint64_t find_jumpto_bl(uint64_t region, uint8_t* data, size_t size);
uint64_t find_jumpto_func(uint64_t region, uint8_t* data, size_t size);
uint64_t find_debug_enabled(uint64_t region, uint8_t* data, size_t size);
uint64_t find_bootargs_nop(uint64_t region, uint8_t* data, size_t size);
uint64_t find_bootargs_adr(uint64_t region, uint8_t* data, size_t size);
uint64_t find_zero(uint64_t region, uint8_t* data, size_t size);
uint64_t find_bootx_str(uint64_t region, uint8_t* data, size_t size);
uint64_t find_bootx_cmd_handler(uint64_t region, uint8_t* data, size_t size);
uint64_t find_go_cmd_handler(uint64_t region, uint8_t* data, size_t size);
uint64_t find_malloc(uint64_t region, uint8_t* data, size_t size);
uint64_t find_panic(uint64_t region, uint8_t* data, size_t size);
uint64_t find_free(uint64_t region, uint8_t* data, size_t size);

#endif
