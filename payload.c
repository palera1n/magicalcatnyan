#include <stdint.h>
#include <stdbool.h>

#include "printf.h"
#include "drivers/tz/tz.h"
#include <common.h>
#include <offsetfinder.h>

char* rootdev = (char*)(PAYLOAD_BASE_ADDRESS + 0x60);
uint8_t* invert_fb = (uint8_t*)(PAYLOAD_BASE_ADDRESS + 0x70);
uint8_t* xargs_set = (uint8_t*)(PAYLOAD_BASE_ADDRESS + 0x71);
uint8_t* xfb_state = (uint8_t*)(PAYLOAD_BASE_ADDRESS + 0x72);
char* CommandLine = (char*)(PAYLOAD_BASE_ADDRESS + 0x73);
char CommandLine_Temp[BOOT_LINE_LENGTH_iOS13];
uint16_t args_len_already;

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
#if DEV_BUILD
    printf("\tpeek <addr> <size>\t: dump memory\n");
    printf("\tpoke <addr> <uint64>\t: write <uint64> to <addr>\n");
#endif
}

int payload(int argc, struct cmd_arg *args)
{
    if(*(uint32_t*)PAYLOAD_BASE_ADDRESS == 0)
    {
        if(iboot_func_init()) return -1;
        printf("-------- relocated --------\n");
        *invert_fb = 0;
        *xargs_set = 0;
        *xfb_state = 0;
        return 0;
    }
    else
    {
        if(iboot_func_init()) return -1;
    }
    
    printf("-------- payload start --------\n");

    if (argc == 2) {
        if(!strcmp(args[1].str, "fbinvert")) {
            if (*invert_fb == 1) *invert_fb = 0;
            else *invert_fb = 1;
            printf("invert_fb = %u\n", *invert_fb);
            return 0;
        } else if (!strcmp(args[1].str, "default_xargs")) {
            *xargs_set = 0;
            printf("using default xnu boot cmdline\n");
            return 0;
        } else if(!strcmp(args[1].str, "xfb")) {
            if (*xfb_state == 1) *xfb_state = 0;
            else *xfb_state = 1;
            printf("xfb_state = %u\n", *xfb_state);
            return 0;
        }
    } else if (argc == 3) {
        if (!strcmp(args[1].str, "boot")) {
            if (strlen(args[2].str) > 19) {
                puts("rootdev too long");
                return -1;
            }
            strcpy(rootdev, args[2].str);
            fsboot();
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
        if (argc <= 2) printf("cleared xnu boot arg cmdline, use default_xargs to use the default ones\n");
        printf("set xnu boot arg cmdline to [%s]\n", CommandLine);
        return 0;
    }

    if (argc == 1 || (argc > 2 && strcmp(args[1].str, "help"))) { 
        printf("unknown command/ bad arg count\n");
    }
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
        
    pmgr_init();
    wdt_disable();
    mm_init();
    screen_init();

    if (*invert_fb == 1) {
        screen_invert();
    }

    screen_puts("");
    screen_puts("");
    screen_puts("");
    screen_puts("");
    screen_puts("");
    screen_puts("");
    screen_puts("==================================");
    screen_puts("");
    screen_puts("Hello from magicalcatnyan!");
    screen_puts("Originally written by dora2-iOS, with modifications from palera1n team");
    screen_puts("Also thanks to pongoOS developers!");
    screen_puts("");
    screen_puts("==================================");
    screen_mark_banner();

#if 0
    uint16_t h = 0;
    char** ptrs = malloc(1024 * sizeof(char*));
    my_size_t malloc_test_sizes[] = { 1, 8, 43, 90, 431, 5, 133, 91 };
    for (uint8_t i = 0; i < 128; i++) {
        for (uint8_t j = 0; j < 8; j++) {
            // dprintf("h = %u, i = %u, j = %u\n", h, i ,j);
            ptrs[h] = malloc(malloc_test_sizes[j]);
            for (my_size_t k = 0; k < malloc_test_sizes[j]; k++) {
                *(char*)ptrs[h] = 0x7f;
            }
            h++;
        }
    }
    printf("done!\n");
    for (uint16_t i = 0; i < 1024; i++) {
        dprintf("i = %u\n", i);
        free(ptrs[i]);
    }
#endif

    memcpy((char*)dt_get_prop("chosen", "firmware-version", NULL), "iMoot-", 6);
    sprintf((char*)dt_get_prop("chosen", "firmware-version", NULL), "%s-magicalcatnyan", (char*)dt_get_prop("chosen", "firmware-version", NULL));
    printf("Booted by: %s\n", (const char*)dt_get_prop("chosen", "firmware-version", NULL));
#ifdef __clang__
    printf("Built with: Clang %s\n", __clang_version__);
#else
    printf("Built with: GCC %s\n", __VERSION__);
#endif
#if DEV_BUILD
    printf("Build type: DEVELOPMENT\n");
#else
    printf("Build type: RELEASE\n");
#endif
    printf("Ruuning on %x\n", socnum);
    tz_setup();

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
#if DEV_BUILD
    tz_command();
#endif
    dprintf("gBootArgs:\n"
        "\tRevision: 0x%x\n"
        "\tVersion: 0x%x\n"
        "\tvirtBase: 0x%llx\n"
        "\tphysBase 0x%llx\n"
        "\tmemSize: 0x%llx\n"
        "\ttopOfKernelData: 0x%llx\n"
        "\tmachineType: 0x%x\n"
        "\tdeviceTreeP: 0x%llx\n"
        "\tdeviceTreeLength: 0x%x\n"
        "\tCommandLine: %s\n"
        "\tbootFlags: %llx\n"
        "\tmemSizeActual: %llx\n",
        gBootArgs->Revision,
        gBootArgs->Version,
        gBootArgs->virtBase,
        gBootArgs->physBase,
        gBootArgs->memSize,
        gBootArgs->topOfKernelData,
        gBootArgs->machineType,
        (uint64_t)gBootArgs->deviceTreeP,
        gBootArgs->deviceTreeLength,
        gBootArgs->CommandLine,
        gBootArgs->bootFlags,
        gBootArgs->memSizeActual
    );
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

int main(void)
{
    return 0;
}
