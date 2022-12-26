/* 
 * pongoOS - https://checkra.in
 * 
 * Copyright (C) 2019-2022 checkra1n team
 *
 * This file is part of pongoOS.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 */
#ifndef XNU_H
#define XNU_H
#include <stdint.h>
#include <stdbool.h>
#include <mach-o/loader.h>

#define XNU_PF_ACCESS_8BIT 0x8
#define XNU_PF_ACCESS_16BIT 0x10
#define XNU_PF_ACCESS_32BIT 0x20
#define XNU_PF_ACCESS_64BIT 0x40
#define kCacheableView 0x400000000ULL

typedef struct xnu_pf_range {
    uint64_t va;
    uint64_t size;
    uint8_t* cacheable_base;
    uint8_t* device_base;
} xnu_pf_range_t;

struct xnu_pf_patchset;

typedef struct xnu_pf_patch {
    bool (*pf_callback)(struct xnu_pf_patch* patch, void* cacheable_stream);
    bool is_required;
    bool has_fired;
    bool should_match;
    uint32_t pfjit_stolen_opcode;
    uint32_t pfjit_max_emit_size;
    uint32_t* (*pf_emit)(struct xnu_pf_patch* patch, struct xnu_pf_patchset *patchset,uint32_t* insn, uint32_t** insn_stream_end, uint8_t access_type);
    void (*pf_match)(struct xnu_pf_patch* patch, uint8_t access_type, void* preread, void* cacheable_stream);
    struct xnu_pf_patch* next_patch;
    uint32_t* pfjit_entry;
    uint32_t* pfjit_exit;
    uint8_t pf_data[0];
    char * name;

    //            patch->pf_match(XNU_PF_ACCESS_32BIT, reads, &stream[index], &dstream[index]);

} xnu_pf_patch_t;

typedef struct xnu_pf_patchset {
    xnu_pf_patch_t* patch_head;
    void* jit_matcher;
    uint64_t p0;
    uint8_t accesstype;
    bool is_required;
} xnu_pf_patchset_t;

extern uint64_t xnu_slide_hdr_va(struct mach_header_64* header, uint64_t hdr_va);
extern uint64_t xnu_slide_value(struct mach_header_64* header);
extern struct mach_header_64* xnu_header(void);
extern uint32_t xnu_platform(void);
extern xnu_pf_range_t* xnu_pf_range_from_va(uint64_t va, uint64_t size);
extern xnu_pf_range_t* xnu_pf_segment(struct mach_header_64* header, char* segment_name);
extern xnu_pf_range_t* xnu_pf_section(struct mach_header_64* header, void* segment, char* section_name);
extern xnu_pf_range_t* xnu_pf_all(struct mach_header_64* header);
extern xnu_pf_range_t* xnu_pf_all_x(struct mach_header_64* header);
extern void xnu_pf_disable_patch(xnu_pf_patch_t* patch);
extern void xnu_pf_enable_patch(xnu_pf_patch_t* patch);
extern struct segment_command_64* macho_get_segment(struct mach_header_64* header, const char* segname);
extern struct section_64 *macho_get_section(struct segment_command_64 *seg, const char *name);
extern struct mach_header_64* xnu_pf_get_first_kext(struct mach_header_64* kheader);
// extern void hexdump(void *mem, unsigned int len);
extern void xnu_pf_emit(xnu_pf_patchset_t* patchset); // converts a patchset to JIT
extern void xnu_pf_apply(xnu_pf_range_t* range, xnu_pf_patchset_t* patchset);
extern xnu_pf_patchset_t* xnu_pf_patchset_create(uint8_t pf_accesstype);
extern void xnu_pf_patchset_destroy(xnu_pf_patchset_t* patchset);
extern void* xnu_va_to_ptr(uint64_t va);
extern uint64_t xnu_ptr_to_va(void* ptr);
extern uint64_t xnu_rebase_va(uint64_t va);
extern uint64_t kext_rebase_va(uint64_t va);
extern struct mach_header_64* xnu_pf_get_kext_header(struct mach_header_64* kheader, const char* kext_bundle_id);
extern xnu_pf_patch_t* xnu_pf_maskmatch(xnu_pf_patchset_t* patchset, char * name, uint64_t* matches, uint64_t* masks, uint32_t entryc, bool required, bool (*callback)(struct xnu_pf_patch* patch, void* cacheable_stream));
extern void xnu_pf_apply_each_kext(struct mach_header_64* kheader, xnu_pf_patchset_t* patchset);
xnu_pf_patch_t* xnu_pf_ptr_to_data(xnu_pf_patchset_t* patchset, uint64_t slide, xnu_pf_range_t* range, void* data, my_size_t datasz, bool required, bool (*callback)(struct xnu_pf_patch* patch, void* cacheable_stream));

extern void xnu_boot(void);
extern void xnu_init(void);
extern void xnu_loadrd(void);
extern void xnu_hook(void);
#endif
