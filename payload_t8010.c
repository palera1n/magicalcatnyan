#include <stdbool.h>
#include <stdint.h>

#include "printf.h"
#include "drivers/tz/tz.h"
#include <common.h>
#include <offsetfinder.h>

extern uint64_t dt_get_u64_prop(const char* device, const char* prop);
extern uint64_t dt_get_u64_prop_i(const char* device, const char* prop, uint32_t idx);
char* rootdev = (char*)(PAYLOAD_BASE_ADDRESS_T8010 + 0x60);

static void usage(void)
{
    printf("usage: %s <cmd>\n", "go");
    printf("cmd:\n");
    printf("\tboot\t\t\t: boot xnu\n");
    printf("\tpeek <addr> <size>\t: dump memory\n");
    printf("\tpoke <addr> <uint64>\t: write <uint64> to <addr>\n");
}


int payload(int argc, struct cmd_arg *args)
{
    if(*(uint32_t*)PAYLOAD_BASE_ADDRESS_T8010 == 0)
    {
        if(iboot_func_init()) return -1;
        printf("-------- relocated --------\n");
        return 0;
    }
    else
    {
        if(iboot_func_init()) return -1;
    }
    
    printf("-------- payload start --------\n");
    
    if (argc == 3) {
        if(!strcmp(args[1].str, "boot")) {
            if (strlen(args[2].str) > 19) {
                puts("rootdev too long");
                return -1;
            }
            strcpy(rootdev, args[2].str);
            fsboot();
            return 0;
        }
    } else if (argc == 4) {
        if (!strcmp(args[1].str, "peek")) {
            peek(args[2].str, args[3].str);
            return 0;
        } else if (!strcmp(args[1].str, "poke")) {
            poke(args[2].str, args[3].str);
            return 0;
        }
    }
    printf("unknown command/ bad arg count\n");
    usage();
    return 0;
}


boot_args *gBootArgs;
void *gEntryPoint;
dt_node_t *gDeviceTree;
char* gDevType;
uint32_t socnum;
char soc_name[9] = {};

bool is_16k() {
    return (socnum == 0x8010) || (socnum == 0x8011) || (socnum == 0x8012) || (socnum == 0x8015);
}

void payload_entry(uint64_t *kernel_args, void *entryp)
{
    
    gBootArgs = (boot_args*)kernel_args;
    gEntryPoint = entryp;
    
    gDeviceTree = (void*)((uint64_t)gBootArgs->deviceTreeP - gBootArgs->virtBase + gBootArgs->physBase);
    gIOBase = dt_get_u64_prop_i("arm-io", "ranges", 1);
    gDevType = dt_get_prop("arm-io", "device_type", NULL);    
    size_t len = strlen(gDevType) - 3;
    len = len < 8 ? len : 8;
    strncpy(soc_name, gDevType, len);
    if  (strcmp(soc_name, "s5l8960x") == 0) socnum = 0x8960;
    else if(strcmp(soc_name, "t7000") == 0) socnum = 0x7000;
    else if(strcmp(soc_name, "t7001") == 0) socnum = 0x7001;
    else if(strcmp(soc_name, "s8001") == 0) socnum = 0x8001;
    else if(strcmp(soc_name, "t8010") == 0) socnum = 0x8010;
    else if(strcmp(soc_name, "t8011") == 0) socnum = 0x8011;
    else if(strcmp(soc_name, "t8012") == 0) socnum = 0x8012;
    else if(strcmp(soc_name, "t8015") == 0) socnum = 0x8015;
    else if(strcmp(soc_name, "s8000") == 0) {
        const char *sgx = dt_get_prop("sgx", "compatible", NULL);
        if(strlen(sgx) > 4 && strcmp(sgx + 4, "s8003") == 0)
        {
            socnum = 0x8003;
            soc_name[4] = '3';
        }
        else
        {
            socnum = 0x8000;
        }
    }
    screen_init();
    screen_puts("");
    screen_puts("");
    screen_puts("");
    screen_puts("");
    screen_puts("");
    screen_puts("");
    screen_puts("==================================");
    screen_puts("");
    screen_puts("Hello from magicalcatnyan!");
    screen_puts("Originally written by dora2-iOS, with modifications from Nick Chan");
    screen_puts("");
    screen_puts("==================================");
    screen_mark_banner();

    pmgr_init();
    wdt_disable();
    
    {
        uint32_t len = 0;
        dt_node_t* dev = dt_find(gDeviceTree, "chosen");
        if (!dev) panic("invalid devicetree: no device!");
        uint32_t* val = dt_prop(dev, "root-matching", &len);
        if (!val) panic("invalid devicetree: no prop!");
        tz_setup();
        tz_command();
	    char str[200] = "<dict ID=\"0\"><key>IOProviderClass</key><string ID=\"1\">IOService</string><key>BSD Name</key><string ID=\"2\">";
        strcat(str, rootdev);
        strcat(str, "</string></dict>");
        unsigned int txt_len = strlen(str);
        memset(val, 0x0, 0x100);
        memcpy(val, str, txt_len);
        printf("set new entry: %016llx: %s \n", (uint64_t)val, rootdev);
    }
    printf("-------- bye payload --------\n");
    
}

int jump_hook(void* boot_image, void* boot_args)
{
    iboot_func_load();
    
    printf("-------- hello payload --------\n");
    
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
    jumpto  = (jumpto_t)offsetBase[2];
    panic   = (panic_t) offsetBase[3];
}

int main(void)
{
    return 0;
}
