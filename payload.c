#include <stdint.h>
#include <stdbool.h>

#include "printf.h"
#include <common.h>
#include <offsetfinder.h>

const uint64_t payload_baseaddr = PAYLOAD_BASE_ADDRESS;
char* rootdev = (char*)(PAYLOAD_BASE_ADDRESS + 0x60);
uint8_t* xargs_set = (uint8_t*)(PAYLOAD_BASE_ADDRESS + 0x71);
uint8_t* xfb_state = (uint8_t*)(PAYLOAD_BASE_ADDRESS + 0x72);
#if DEV_BUILD
uint8_t* skip_md0_patch = (uint8_t*)(PAYLOAD_BASE_ADDRESS + 0x73);
uint8_t* invert_fb = (uint8_t*)(PAYLOAD_BASE_ADDRESS + 0x70);
#endif
char* gLaunchdString = (char*)(PAYLOAD_BASE_ADDRESS + 0x74);
char* enable_kpf = (char*)(PAYLOAD_BASE_ADDRESS + 0x83);
char* CommandLine = (char*)(PAYLOAD_BASE_ADDRESS + 0x84);
char CommandLine_Temp[BOOT_LINE_LENGTH_iOS13];
bool serial_is_initialized = false;
uint16_t args_len_already;
void command_kpf();
void kpf_banner();
extern void crash();
extern void mm_init();
extern uint64_t get_x18();

#if DEV_BUILD
const char build_style[] = "DEVELOPMENT";
#else
const char build_style[] = "RELEASE";
#endif
#if DEV_BUILD
static void usage(void)
{
    printf("usage: %s <cmd>\n", "go");
    printf("cmd:\n");
    printf("\thelp\t\t\t: print this help\n");
    printf("\tboot <rootdev>\t\t: boot xnu\n");
    printf("\txfb\t\t\t: flip/unflip xnu framebuffer state\n");
    printf("\txargs [boot cmdline]\t: set or clear xnu boot command line\n");
    printf("\tdefault_xargs \t\t: use the default xnu boot command line\n");
    printf("\tfbinvert\t\t: invert boot framebuffer\n");
    printf("\tcrash\t\t\t: Crash iBoot\n");
    printf("\trestore_mode \t\t: toggle md0 patches (need KPF)\n");
    printf("\tlaunchd <path>\t\t: set launchd path\n");
    printf("\tenable_kpf\t\t: Enable KPF (needed for launchd path change)\n");
    printf("\ttz\t\t\t: Trustzone Info\n");
    printf("\ttz_lockdown\t\t: Trustzone lockdown (crash iBoot)\n");
    printf("\tprint_x18\t\t: print value in x18 register\n");
    printf("\tpeek <addr> <size>\t: dump memory\n");
    printf("\tpoke <addr> <uint64>\t: write <uint64> to <addr>\n");
    printf("\tbzero <addr> <size>\t: zero memory of <size> at addr\n");
}
#endif

int payload(int argc, struct cmd_arg *args)
{
    if(*(uint32_t*)PAYLOAD_BASE_ADDRESS == 0)
    {
        if(iboot_func_init()) return -1;
        serial_is_initialized = true;
        dprintf("-------- relocated --------\n");
#if DEV_BUILD
        *invert_fb = 0;
#endif
        *xargs_set = 0;
        *xfb_state = 0;
        *enable_kpf = 0;
        return 0;
    }
    else
    {
        if(iboot_func_init()) return -1;
    }
    serial_is_initialized = true;

    dprintf("-------- payload start --------\n");

    if (argc == 2) {
#if DEV_BUILD
        if(!strcmp(args[1].str, "fbinvert")) {
            if (*invert_fb == 1) *invert_fb = 0;
            else *invert_fb = 1;
            dprintf("invert_fb = %u\n", *invert_fb);
            return 0;
        } else if (!strcmp(args[1].str, "default_xargs")) {
            *xargs_set = 0;
            dprintf("using default xnu boot cmdline\n");
            return 0;
        } else if(!strcmp(args[1].str, "restore_mode")) {
            if (*skip_md0_patch == 1) *skip_md0_patch = 0;
            else *skip_md0_patch = 1;
            dprintf("skip_md0_patch = %u\n", *skip_md0_patch);
            return 0;
        } else if(!strcmp(args[1].str, "crash")) {
            crash();
            return -1;
        } else if(!strcmp(args[1].str, "tz")) {
            gTZRegbase = (volatile uint32_t*)0x0000000200000480;
            tz_command();
            return 0; 
        } else if(!strcmp(args[1].str, "tz_lockdown")) {
            gTZRegbase = (volatile uint32_t*)0x0000000200000480;
            tz_lockdown();
            return 0; 
        } else if(!strcmp(args[1].str, "print_x18")) {
            printf("x18 = %x\n", get_x18());
            return 0; 
        } else
#endif
    if(!strcmp(args[1].str, "xfb")) {
        if (*xfb_state == 1) *xfb_state = 0;
        else *xfb_state = 1;
        dprintf("xfb_state = %u\n", *xfb_state);
        return 0;
    } else if(!strcmp(args[1].str, "enable_kpf")) {
        if (*enable_kpf == 1) *enable_kpf = 0;
        else *enable_kpf = 1;
        dprintf("enable_kpf = %u\n", *enable_kpf);
        return 0;
    }  
    } else if (argc == 3) {
        if (!strcmp(args[1].str, "boot")) {
            if (strlen(args[2].str) > 19) {
                dprintf("rootdev too long\n");
                return -1;
            }
            strcpy(rootdev, args[2].str);
            fsboot();
            return 0;
        } else if(!strcmp(args[1].str, "launchd")) {
            if (strlen(args[2].str) > sizeof("/sbin/launchd") - 1) {
                printf("launchd string too long (max %llu got %llu)\n", sizeof("/sbin/launchd") - 1, strlen(args[2].str));
                return -1;
            }
            strncpy(gLaunchdString, args[2].str, sizeof("/sbin/launchd"));
            dprintf("set launchd string: %s\n", gLaunchdString);
            return 0;
        }
    } else if (argc == 4) {
#if DEV_BUILD
        if (!strcmp(args[1].str, "peek")) {
            peek(args[2].str, args[3].str);
            return 0;
        } else if (!strcmp(args[1].str, "poke")) {
            poke(args[2].str, args[3].str);
            return 0;
        } else if (!strcmp(args[1].str, "bzero")) {
            bzero_command(args[2].str, args[3].str);
            return 0;
        }
#endif
    }

    if (argc > 1 && (!strcmp(args[1].str, "xargs"))) {
        args_len_already = 1; // terminating NULL byte
        *CommandLine = '\0';
        for (uint16_t i = 2; i < argc; i++) {
            args_len_already += (strlen(args[i].str) + 1); 
            if (args_len_already > BOOT_LINE_LENGTH_iOS13) {
                printf("max boot arg length %d excceed.\n", BOOT_LINE_LENGTH_iOS13);
                return -1;
            }
            strcat(CommandLine_Temp, args[i].str);
            if ((i + 1) != argc) {
                strcat(CommandLine_Temp, " ");
            }
        }
        strcpy(CommandLine, CommandLine_Temp);
        *xargs_set = 1;
        if (argc <= 2) dprintf("cleared xnu boot arg cmdline, use default_xargs to use the default ones\n");
        dprintf("set xnu boot arg cmdline to [%s]\n", CommandLine);
        return 0;
    }
#if DEV_BUILD
    if (argc == 1 || (argc > 2 && strcmp(args[1].str, "help"))) { 
        printf("unknown command/ bad arg count\n");
    }
    usage();
#endif
    return 0;
}


boot_args *gBootArgs;
void *gEntryPoint;
dt_node_t *gDeviceTree;
char* gDevType;
uint32_t socnum;
char soc_name[9] = {};

bool is_16k() {
    return (socnum == 0x8000) || (socnum == 0x8001) || (socnum == 0x8003) || (socnum == 0x8010) || (socnum == 0x8011) || (socnum == 0x8012) || (socnum == 0x8015);
}

void flip_video_display() {
    gBootArgs->Video.v_display = !gBootArgs->Video.v_display;
    if (!gBootArgs->Video.v_display) {
        printf("xnu now owns the framebuffer\n");
    } else {
        printf("xnu no longer owns the framebuffer\n");
    }
}

void payload_entry(uint64_t *kernel_args, void *entryp)
{
    gBootArgs = (boot_args*)kernel_args;
    gEntryPoint = entryp;
    gDeviceTree = (void*)((uint64_t)gBootArgs->deviceTreeP - gBootArgs->virtBase + gBootArgs->physBase);
    gIOBase = dt_get_u64_prop_i("arm-io", "ranges", 1);
    gDevType = dt_get_prop("arm-io", "device_type", NULL);

    // Determine the CPID
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

    // Initialize malloc() and jit_alloc()
    mm_init();

    // Enable the framebuffer
    screen_init();

#if DEV_BUILD
    if (*invert_fb == 1) {
        screen_invert();
    }
#endif

#if PAYLOAD_t8015
    screen_puts("");
    screen_puts("");
    screen_puts("");
    screen_puts("");
    screen_puts("");
    screen_puts("");
    screen_puts("");
    screen_puts("");
    screen_puts("");
#endif
    printf("\n==================================\n::\n");
    printf(":: magicalcatnyan for %x, palera1n team, dora2ios\n::\n", socnum);
    printf("::\tBUILD_TAG: %s\n::\n", MAGICALCATNYAN_VERSION);
    printf("::\tBUILD_STYLE: %s\n::\n", build_style);
    printf("::\tAlso thanks to pongoOS developers!\n::\n");
    printf("==================================\n\n");
    screen_mark_banner();
    
    pmgr_init();
    wdt_disable();

    memcpy((char*)dt_get_prop("chosen", "firmware-version", NULL), "iMoot-", 6);
    sprintf((char*)dt_get_prop("chosen", "firmware-version", NULL), "%s-magicalcatnyan", (char*)dt_get_prop("chosen", "firmware-version", NULL));

    printf("Booted by: %s\n", (const char*)dt_get_prop("chosen", "firmware-version", NULL));
#ifdef __clang__
    dprintf("Built with: Clang %s\n", __clang_version__);
#else
    dprintf("Built with: GCC %s\n", __VERSION__);
#endif

    // Patch root-matching
    {
        uint32_t len = 0;
        dt_node_t* dev = dt_find(gDeviceTree, "chosen");
        if (!dev) panic("invalid devicetree: no device!");
        char* val = (char*)dt_prop(dev, "root-matching", &len);
        if (!val) panic("invalid devicetree: no prop!");
        // There are exactly 256 bytes of space
	    char str[256] = "<dict ID=\"0\"><key>IOProviderClass</key><string ID=\"1\">IOService</string><key>BSD Name</key><string ID=\"2\">";
        strcat(str, rootdev);
        strcat(str, "</string></dict>");
        memset((void*)val, 0x0, 256);
        sprintf(val, str);
        printf("set new entry: %016llx: %s \n", (uint64_t)val, rootdev);
    }
    if (*xargs_set == 1) strcpy(gBootArgs->CommandLine, CommandLine);
    if (*xfb_state == 1) flip_video_display();
    printf("xnu boot arg cmdline: [%s]\n", gBootArgs->CommandLine);
    tz_setup();
#if DEV_BUILD
    tz_command();
    log_bootargs();
#endif
    // Run KPF if enabled
    if (*enable_kpf == 1) {
        kpf_banner();
        command_kpf();
    }
    xnu_boot();
#if DEV_BUILD
    mem_stat();
#endif
    puts("Booting");
    iprintf("======== End of magicalcatnyan serial output. ========\n");
}

int jump_hook(void* boot_image, void* boot_args)
{
    iboot_func_load();
    serial_is_initialized = true;
    
    dprintf("-------- hello payload --------\n");
    
    if (*(uint8_t*)(boot_args + 8 + 7)) {
        // kernel
        payload_entry((uint64_t*)boot_args, boot_image);
    } else {
        // hypv
        payload_entry(*(uint64_t**)(boot_args + 0x20), (void*)*(uint64_t*)(boot_args + 0x28));
    }
    return jumpto(boot_image, boot_args);
}

int main(void)
{
    return 0;
}
