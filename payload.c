/* payload.c
 *
 * 2022/11/01
 * (c) dora2ios
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdint.h>
#include <stdbool.h>

#include <common.h>
#include <offsetfinder.h>

#include <fuse/fuse.h>

static bool RAMDisk = 0;

static void usage(void)
{
    iprintf("usage: %s <cmd>\n", "go");
    iprintf("cmd:\n");
    iprintf("\tboot\t\t: boot xnu\n");
    iprintf("\tramdisk\t\t: boot xnu with ramdisk\n");
    iprintf("\tfuse_demote\t: demote fuse\n");
    iprintf("\tfuse_lock\t: lock fuse\n");
    iprintf("\tfuse_info\t: check fuse info\n");
}

int payload(int argc, struct cmd_arg *args)
{
    if(*(uint32_t*)PAYLOAD_BASE_ADDRESS == 0)
    {
        if(iboot_func_init()) return -1;
        iprintf("-------- relocated --------\n");
        usage();
        return 0;
    }
    else
    {
        if(iboot_func_init()) return -1;
    }
    
    iprintf("-------- payload start --------\n");
    
    // args[1].str: arg str
    // args[1].u: num
    if (argc == 1)
    {
        usage();
        return 0;
    }
    
    if (argc == 2)
    {
        if(!strcmp(args[1].str, "boot"))
        {
            fuse_cmd_lock();
            RAMDisk = 0;
            fsboot();
            return 0;
        }
        
        if(!strcmp(args[1].str, "ramdisk"))
        {
            fuse_cmd_lock();
            RAMDisk = 1;
            fsboot();
            return 0;
        }
        
        if(!strcmp(args[1].str, "fuse_demote"))
        {
            fuse_cmd_demote();
            return 0;
        }
        
        if(!strcmp(args[1].str, "fuse_lock"))
        {
            fuse_cmd_lock();
            return 0;
        }
        
        if(!strcmp(args[1].str, "fuse_info"))
        {
            fuse_cmd_status();
            return 0;
        }
    }
     
    return 0;
}


boot_args *gBootArgs;
void *gEntryPoint;
dt_node_t *gDeviceTree;

void payload_entry(uint64_t *kernel_args, void *entryp)
{
    
    gBootArgs = (boot_args*)kernel_args;
    gEntryPoint = entryp;
    
    gDeviceTree = (void*)((uint64_t)gBootArgs->deviceTreeP - gBootArgs->virtBase + gBootArgs->physBase);
    
    iprintf("entryp: %016llx: %08x\n", (uint64_t)entryp, *(uint32_t*)entryp);
    iprintf("virtBase: %016llx\n", gBootArgs->virtBase);
    iprintf("physBase: %016llx\n", gBootArgs->physBase);
    
    kpf(RAMDisk);
    
    iprintf("old bootArgs: %s\n", gBootArgs->CommandLine);
    
    // TODO
    if(RAMDisk)
        memcpy((void*)gBootArgs->CommandLine, "rd=md0 wdt=-1 serial=3\x00", sizeof("rd=md0 wdt=-1 serial=3\x00"));
    else
        memcpy((void*)gBootArgs->CommandLine, "rd=disk0s1s8 serial=3\x00", sizeof("rd=disk0s1s8 serial=3\x00"));
        
    iprintf("new bootArgs: %s\n", gBootArgs->CommandLine);
    
    iprintf("-------- bye payload --------\n");
    
}

int jump_hook(void* boot_image, void* boot_args)
{
    iboot_func_load();
    
    iprintf("-------- hello payload --------\n");
    
    if (*(uint8_t*)(boot_args + 8 + 7)) {
        // kernel
        payload_entry((uint64_t*)boot_args, boot_image);
    } else {
        // hypv
        payload_entry(*(uint64_t**)(boot_args + 0x20), (void*)*(uint64_t*)(boot_args + 0x28));
    }
    
    return jumpto(boot_image, boot_args);
}

int iboot_func_init(void)
{
    // T8015 only
    if(*(uint32_t*)PAYLOAD_BASE_ADDRESS == 0)
    {
        
        memcpy((void*)PAYLOAD_BASE_ADDRESS, (void*)0x801000000, 0x80000);
        
        uint64_t iboot_base = 0x18001c000;
        uint64_t fuseBase = 0x2352bc000;
        
        void* idata = (void *)(0x18001c000);
        size_t isize = *(uint64_t *)(idata + 0x308) - iboot_base;
        
        uint64_t* offsetBase = (uint64_t*)(PAYLOAD_BASE_ADDRESS + 0x40);
        
        // OF
        uint64_t _printf = find_printf(iboot_base, idata, isize);
        if(!_printf)
            return -1;
        _printf += iboot_base;
        
        uint64_t _mount_and_boot_system = find_mount_and_boot_system(iboot_base, idata, isize);
        if(!_mount_and_boot_system)
            return -1;
        _mount_and_boot_system += iboot_base;
        
        uint64_t _jumpto_func = find_jumpto_func(iboot_base, idata, isize);
        if(!_jumpto_func)
            return -1;
        _jumpto_func += iboot_base;
        
        uint64_t _malloc = find_malloc(iboot_base, idata, isize);
        if(!_malloc)
            return -1;
        _malloc += iboot_base;
        
        uint64_t _panic = find_panic(iboot_base, idata, isize);
        if(!_panic)
            return -1;
        _panic += iboot_base;
        
        uint64_t _free = find_free(iboot_base, idata, isize);
        if(!_free)
            return -1;
        _free += iboot_base;
        
        offsetBase[0] = fuseBase;
        offsetBase[1] = _printf;
        offsetBase[2] = _mount_and_boot_system;
        offsetBase[3] = _jumpto_func;
        offsetBase[4] = _malloc;
        offsetBase[5] = _panic;
        offsetBase[6] = _free;
    }
    
    iboot_func_load();
    
    return 0;
}

void iboot_func_load(void)
{
    uint64_t* offsetBase = (uint64_t*)(PAYLOAD_BASE_ADDRESS + 0x40);
    gFuseBase = offsetBase[0];
    iprintf = (printf_t)offsetBase[1];
    fsboot  = (fsboot_t)offsetBase[2];
    jumpto  = (jumpto_t)offsetBase[3];
    imalloc = (malloc_t)offsetBase[4];
    panic   = (panic_t) offsetBase[5];
    ifree   = (free_t)  offsetBase[6];
}

int main(void)
{
    return 0;
}
