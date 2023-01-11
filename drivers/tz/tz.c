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
#include <common.h>

volatile uint32_t* gTZRegbase;
#if DEV_BUILD
void tz_command(void) {
    uint32_t raw[4];
    uint32_t shift;

    raw[0] = gTZRegbase[0];
    raw[1] = gTZRegbase[1];
    raw[2] = gTZRegbase[2];
    raw[3] = gTZRegbase[3];
    shift = 12;
    uint64_t real[4];
    real[0] = ( (uint64_t)raw[0]      << shift) + 0x800000000ULL;
    real[1] = (((uint64_t)raw[1] + 1) << shift) + 0x800000000ULL;
    real[2] = ( (uint64_t)raw[2]      << shift) + 0x800000000ULL;
    real[3] = (((uint64_t)raw[3] + 1) << shift) + 0x800000000ULL;
    printf("gTZRegBase = %p\n", gTZRegbase);
    printf("TZ0 (%s):\n"
            "    base: %x (%llx)\n"
            "    end:  %x (%llx)\n"
            "\n"
            "TZ1 (%s):\n"
            "    base: %x (%llx)\n"
            "    end:  %x (%llx)\n"
            "\n",
            gTZRegbase[4] ? "locked" : "unlocked",
            raw[0], real[0],
            raw[1], real[1],
            gTZRegbase[5] ? "locked" : "unlocked",
            raw[2], real[2],
            raw[3], real[3]);
}

void tz0_set(char* base_str, char *end_str) {

    uint64_t base = strtoull(base_str, NULL, 16);
    uint64_t end = strtoull(end_str, NULL, 16);
    if (gTZRegbase[4]) {
        printf("registers are locked\n");
        return;
    }
    gTZRegbase[0] = base;
    gTZRegbase[1] = end;
}

void tz_lockdown(void) {
    bool have_tz0, have_tz1;
    have_tz0 = gTZRegbase[0] != 0;
    have_tz1 = gTZRegbase[2] != 0;
    if(have_tz0) gTZRegbase[4] = 1;
    if(have_tz1) gTZRegbase[5] = 1;
}

bool tz_blackbird(void) {
    if(gTZRegbase[4]) {
        printf("Registers are locked\n");
        return false;
    }
    // XXX: This used to be XOR, but that doesn't work well with the expectations in sep.c.
    // This was probably here to allow toggling from the shell, but that could be done via tz0_set.
    gTZRegbase[0] |= 0x100000;
    return true;
}

void *tz0_calculate_encrypted_block_addr(uint64_t offset) {
    uint64_t offset_block = (offset & (~0x1f));
    offset_block <<= 1; // * 2
    // TODO: get rid of this magic constant
    // Maybe change the API to just return an offset and let the caller add it to their memory base?
    return (void*)(0xc00000000ULL + offset_block);
}

bool tz0_is_locked(void)
{
    return gTZRegbase[4] != 0;
}

uint64_t tz0_base(void) {
    return ((uint64_t)gTZRegbase[0] << 12) + 0x800000000ULL;
}

uint64_t tz0_size(void)
{
    return (uint64_t)(gTZRegbase[1] - gTZRegbase[0] + 1) << 12;
}

#endif
void tz_setup(void) {
    gTZRegbase = (volatile uint32_t*)(gIOBase + 0x480);
}

