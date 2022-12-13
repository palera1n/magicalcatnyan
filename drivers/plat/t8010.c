#include <common.h>
#include <offsetfinder.h>

int iboot_func_init(void)
{
    // T8010
    if(*(uint32_t*)PAYLOAD_BASE_ADDRESS_T8010 == 0)
    {
        //memcpy(relocate_addr, load_addr, size)
        memcpy((void*)PAYLOAD_BASE_ADDRESS_T8010, (void*)0x800800000, 0x80000);
        
        uint64_t iboot_base = 0x1800b0000;
        
        void* idata = (void *)(0x1800b0000);
        my_size_t isize = *(uint64_t *)(idata + 0x308) - iboot_base;
        
        uint64_t* offsetBase = (uint64_t*)(PAYLOAD_BASE_ADDRESS_T8010 + 0x40);
        
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
    uint64_t* offsetBase = (uint64_t*)(PAYLOAD_BASE_ADDRESS_T8010 + 0x40);
    iprintf = (printf_t)offsetBase[0];
    fsboot  = (fsboot_t)offsetBase[1];
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wincompatible-function-pointer-types"
    jumpto  = (jumpto_t)offsetBase[2];
    real_panic   = (panic_t) offsetBase[3];
}
#pragma clang diagnostic pop