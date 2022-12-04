#include <stdint.h>
#include <stdbool.h>

#include <common.h>
#include <offsetfinder.h>

static void usage(void)
{
    iprintf("usage: %s <cmd>\n", "go");
    iprintf("cmd:\n");
    iprintf("\tboot\t\t: boot xnu\n");
}

int payload(int argc, struct cmd_arg *args)
{
    if(*(uint32_t*)PAYLOAD_BASE_ADDRESS == 0)
    {
        if(iboot_func_init()) return -1;
        iprintf("-------- relocated --------\n");
        return 0;
    }
    else
    {
        if(iboot_func_init()) return -1;
    }
    
    iprintf("-------- payload start --------\n");
    
    if (argc != 2)
    {
        usage();
        return 0;
    }
    
    if (argc == 2)
    {
        if(!strcmp(args[1].str, "boot"))
        {
            fsboot();
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
    
    {
        uint32_t len = 0;
        dt_node_t* dev = dt_find(gDeviceTree, "chosen");
        if (!dev) panic("invalid devicetree: no device!");
        uint32_t* val = dt_prop(dev, "root-matching", &len);
        if (!val) panic("invalid devicetree: no prop!");
        
        //char str[0x100];
        //memset(&str, 0x0, 0x100);
        //sprintf(str, "<dict ID=\"0\"><key>IOProviderClass</key><string ID=\"1\">IOService</string><key>BSD Name</key><string ID=\"2\">disk0s1s8</string></dict>");
        
        unsigned char str[] = {
            0x3c, 0x64, 0x69, 0x63, 0x74, 0x20, 0x49, 0x44, 0x3d, 0x22, 0x30, 0x22,
            0x3e, 0x3c, 0x6b, 0x65, 0x79, 0x3e, 0x49, 0x4f, 0x50, 0x72, 0x6f, 0x76,
            0x69, 0x64, 0x65, 0x72, 0x43, 0x6c, 0x61, 0x73, 0x73, 0x3c, 0x2f, 0x6b,
            0x65, 0x79, 0x3e, 0x3c, 0x73, 0x74, 0x72, 0x69, 0x6e, 0x67, 0x20, 0x49,
            0x44, 0x3d, 0x22, 0x31, 0x22, 0x3e, 0x49, 0x4f, 0x53, 0x65, 0x72, 0x76,
            0x69, 0x63, 0x65, 0x3c, 0x2f, 0x73, 0x74, 0x72, 0x69, 0x6e, 0x67, 0x3e,
            0x3c, 0x6b, 0x65, 0x79, 0x3e, 0x42, 0x53, 0x44, 0x20, 0x4e, 0x61, 0x6d,
            0x65, 0x3c, 0x2f, 0x6b, 0x65, 0x79, 0x3e, 0x3c, 0x73, 0x74, 0x72, 0x69,
            0x6e, 0x67, 0x20, 0x49, 0x44, 0x3d, 0x22, 0x32, 0x22, 0x3e, 0x64, 0x69,
            0x73, 0x6b, 0x30, 0x73, 0x31, 0x73, 0x38, 0x3c, 0x2f, 0x73, 0x74, 0x72,
            0x69, 0x6e, 0x67, 0x3e, 0x3c, 0x2f, 0x64, 0x69, 0x63, 0x74, 0x3e
        };
        unsigned int txt_len = 131;

        
        memset(val, 0x0, 0x100);
        memcpy(val, str, txt_len);
        iprintf("set new entry: %016llx: disk0s1s8\n", (uint64_t)val);
    }
    
    
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
    // T8015
    if(*(uint32_t*)PAYLOAD_BASE_ADDRESS == 0)
    {
        //memcpy(relocate_addr, load_addr, size)
        memcpy((void*)PAYLOAD_BASE_ADDRESS, (void*)0x801000000, 0x80000);
        
        uint64_t iboot_base = 0x18001c000;
        
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
        
        uint64_t _panic = find_panic(iboot_base, idata, isize);
        if(!_panic)
            return -1;
        _panic += iboot_base;
        
        offsetBase[0] = _printf;
        offsetBase[1] = _mount_and_boot_system;
        offsetBase[2] = _jumpto_func;
        offsetBase[3] = _panic;
    }
    
    iboot_func_load();
    
    return 0;
}

void iboot_func_load(void)
{
    uint64_t* offsetBase = (uint64_t*)(PAYLOAD_BASE_ADDRESS + 0x40);
    iprintf = (printf_t)offsetBase[0];
    fsboot  = (fsboot_t)offsetBase[1];
    jumpto  = (jumpto_t)offsetBase[2];
    panic   = (panic_t) offsetBase[3];
}

int main(void)
{
    return 0;
}
