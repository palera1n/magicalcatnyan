#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stdbool.h>
typedef uint64_t my_size_t;
#define CommandLine pl_CommandLine
#define CommandLine_Temp pl_CommandLine_Temp
#define _atoi pl__atoi
#define _putchar pl__putchar
#define _strnlen_s pl__strnlen_s
#define _vsnprintf pl__vsnprintf
#define args_len_already pl_args_len_already
#define bannerHeight pl_bannerHeight
#define basecolor pl_basecolor
#define bit_range_64 pl_bit_range_64
#define boyermoore_horspool_memmem pl_boyermoore_horspool_memmem
#define bzero pl_bzero
#define color_compose pl_color_compose
#define color_compose_v32 pl_color_compose_v32
#define color_darken pl_color_darken
#define color_decompose pl_color_decompose
#define colors_average pl_colors_average
#define colors_mix_alpha pl_colors_mix_alpha
#define command_tokenize pl_command_tokenize
#define component_darken pl_component_darken
#define dt_alloc_memmap pl_dt_alloc_memmap
#define dt_check pl_dt_check
#define dt_find pl_dt_find
#define dt_find_cb pl_dt_find_cb
#define dt_find_memmap_cb pl_dt_find_memmap_cb
#define dt_get_prop pl_dt_get_prop
#define dt_get_u32_prop pl_dt_get_u32_prop
#define dt_get_u64_prop pl_dt_get_u64_prop
#define dt_get_u64_prop_i pl_dt_get_u64_prop_i
#define dt_parse pl_dt_parse
#define dt_prop pl_dt_prop
#define dt_prop_cb pl_dt_prop_cb
#define elevate_to_el1 pl_elevate_to_el1
#define fctprintf pl_fctprintf
#define find_jumpto_bl pl_find_jumpto_bl
#define find_jumpto_func pl_find_jumpto_func
#define find_literal_ref_64 pl_find_literal_ref_64
#define find_mount_and_boot_system pl_find_mount_and_boot_system
#define find_next_insn_matching_64 pl_find_next_insn_matching_64
#define find_panic pl_find_panic
#define find_printf pl_find_printf
#define flip_video_display pl_flip_video_display
#define font8x8_basic pl_font8x8_basic
#define fsboot pl_fsboot
#define gBootArgs pl_gBootArgs
#define gBootArgs_p pl_gBootArgs_p
#define gDevType pl_gDevType
#define gDeviceTree pl_gDeviceTree
#define gEntryPoint pl_gEntryPoint
#define gEntryPoint_p pl_gEntryPoint_p
#define gFramebuffer pl_gFramebuffer
#define gFramebufferCopy pl_gFramebufferCopy
#define gHeight pl_gHeight
#define gIOBase pl_gIOBase
#define gInterruptBase pl_gInterruptBase
#define gLogoBitmap pl_gLogoBitmap
#define gPMGRBase pl_gPMGRBase
#define gPMGRdev pl_gPMGRdev
#define gPMGRdevlen pl_gPMGRdevlen
#define gPMGRmap pl_gPMGRmap
#define gPMGRmaplen pl_gPMGRmaplen
#define gPMGRreg pl_gPMGRreg
#define gPMGRreglen pl_gPMGRreglen
#define gRowPixels pl_gRowPixels
#define gWDTBase pl_gWDTBase
#define gWidth pl_gWidth
#define gfsboot pl_gfsboot
#define gjumpto pl_gjumpto
#define gpanic pl_gpanic
#define gprintf pl_gprintf
#define hexdump pl_hexdump
#define iboot_func_init pl_iboot_func_init
#define iboot_func_load pl_iboot_func_load
#define insn_add_imm_imm_64 pl_insn_add_imm_imm_64
#define insn_add_imm_rd_64 pl_insn_add_imm_rd_64
#define insn_add_imm_rn_64 pl_insn_add_imm_rn_64
#define insn_add_reg_rd_64 pl_insn_add_reg_rd_64
#define insn_add_reg_rm_64 pl_insn_add_reg_rm_64
#define insn_add_reg_rn_64 pl_insn_add_reg_rn_64
#define insn_adr_imm_64 pl_insn_adr_imm_64
#define insn_adrp_imm_64 pl_insn_adrp_imm_64
#define insn_adrp_rd_64 pl_insn_adrp_rd_64
#define insn_bl_imm32_64 pl_insn_bl_imm32_64
#define insn_is_add_imm_64 pl_insn_is_add_imm_64
#define insn_is_add_reg_64 pl_insn_is_add_reg_64
#define insn_is_adr_64 pl_insn_is_adr_64
#define insn_is_adrp_64 pl_insn_is_adrp_64
#define insn_is_bl_64 pl_insn_is_bl_64
#define insn_is_funcbegin_64 pl_insn_is_funcbegin_64
#define insn_is_ldr_literal_64 pl_insn_is_ldr_literal_64
#define insn_is_mov_imm_64 pl_insn_is_mov_imm_64
#define insn_ldr_literal_imm_64 pl_insn_ldr_literal_imm_64
#define insn_ldr_literal_rt_64 pl_insn_ldr_literal_rt_64
#define insn_mov_imm_imm_64 pl_insn_mov_imm_imm_64
#define insn_mov_imm_rd_64 pl_insn_mov_imm_rd_64
#define invert_fb pl_invert_fb
#define svc_hook pl_svc_hook
#define iprintf pl_iprintf
#define is_16k pl_is_16k
#define isalpha pl_isalpha
#define isdigit pl_isdigit
#define isspace pl_isspace
#define isupper pl_isupper
#define jump_hook pl_jump_hook
#define jumpto pl_jumpto
#define main pl_main
// #define memcpy pl_memcpy
#define memmem pl_memmem
#define memmove pl_memmove
#define memset pl_memset
#define overflow_mode pl_overflow_mode
#define panic pl_panic
#define payload pl_payload
#define payload_entry pl_payload_entry
#define peek pl_peek
#define pmgr_init pl_pmgr_init
#define poke pl_poke
#define printf_ pl_printf_
#define puts pl_puts
#define real_panic pl_real_panic
#define real_signextend_64 pl_real_signextend_64
#define rootdev pl_rootdev
#define scale_factor pl_scale_factor
#define screen_clear_all pl_screen_clear_all
#define screen_clear_row pl_screen_clear_row
#define screen_fill pl_screen_fill
#define screen_fill_basecolor pl_screen_fill_basecolor
#define screen_init pl_screen_init
#define screen_invert pl_screen_invert
#define screen_is_initialized pl_screen_is_initialized
#define screen_mark_banner pl_screen_mark_banner
#define screen_putc pl_screen_putc
#define screen_puts pl_screen_puts
#define screen_write pl_screen_write
#define signextend_64 pl_signextend_64
#define snprintf_ pl_snprintf_
#define soc_name pl_soc_name
#define socnum pl_socnum
#define sprintf_ pl_sprintf_
#define strcat pl_strcat
#define strchr pl_strchr
#define strcmp pl_strcmp
#define strcpy pl_strcpy
#define strlen pl_strlen
#define strncmp pl_strncmp
#define strncpy pl_strncpy
#define strtoull pl_strtoull
#define usage pl_usage
#define vprintf_ pl_vprintf_
#define vsnprintf_ pl_vsnprintf_
#define wdt_disable pl_wdt_disable
#define wdt_enable pl_wdt_enable
#define x_cursor pl_x_cursor
#define xargs_set pl_xargs_set
#define xfb_state pl_xfb_state
#define y_cursor pl_y_cursor

#include "drivers/framebuffer/fb.h"
#include "../printf.h"

extern uint32_t socnum;
extern bool screen_is_initialized;

#ifndef NULL
#define NULL ((void*)0)
#endif

struct cmd_arg {
    bool b;
    my_size_t u;
    my_size_t h;
    char *str;
};

#define PAYLOAD_BASE_ADDRESS_T8015 (0x800F00000) // A11
#define PAYLOAD_BASE_ADDRESS_T8010 (0x800700000) // A10
#define PAYLOAD_BASE_ADDRESS_S8000 (0x800700000) // A9

#if BUILDING_PAYLOAD
#if defined(PAYLOAD_t8015)
#define PAYLOAD_BASE_ADDRESS PAYLOAD_BASE_ADDRESS_T8015
#elif defined(PAYLOAD_t8010)
#define PAYLOAD_BASE_ADDRESS PAYLOAD_BASE_ADDRESS_T8010
#elif defined(PAYLOAD_s8000)
#define PAYLOAD_BASE_ADDRESS PAYLOAD_BASE_ADDRESS_S8000
#else
#error "unsupported platform"
#endif
#endif

#define DT_KEY_LEN              0x20
#define BOOT_LINE_LENGTH_iOS12  0x100
#define BOOT_LINE_LENGTH_iOS13  0x260

bool is_16k();

struct Boot_Video {
    unsigned long    v_baseAddr;    /* Base address of video memory */
    unsigned long    v_display;     /* Display Code (if Applicable) */
    unsigned long    v_rowBytes;    /* Number of bytes per pixel row */
    unsigned long    v_width;       /* Width */
    unsigned long    v_height;      /* Height */
    unsigned long    v_depth;       /* Pixel Depth and other parameters */
};
typedef struct boot_args {
    uint16_t        Revision;           /* Revision of boot_args structure */
    uint16_t        Version;            /* Version of boot_args structure */
    uint32_t        __pad0;
    uint64_t        virtBase;           /* Virtual base of memory */
    uint64_t        physBase;           /* Physical base of memory */
    uint64_t        memSize;            /* Size of memory */
    uint64_t        topOfKernelData;    /* Highest physical address used in kernel data area */
    struct Boot_Video Video;            /* Video Information */
    uint32_t        machineType;        /* Machine Type */
    uint32_t        __pad1;
    void            *deviceTreeP;       /* Base of flattened device tree */
    uint32_t        deviceTreeLength;   /* Length of flattened tree */
    
    char            CommandLine[BOOT_LINE_LENGTH_iOS13];    /* Passed in command line */
    uint32_t        __pad;
    uint64_t        bootFlags;          /* Additional flags specified by the bootloader */
    uint64_t        memSizeActual;      /* Actual size of memory */
    
} __attribute__((packed)) boot_args;

typedef struct
{
    uint32_t nprop;
    uint32_t nchld;
    char prop[];
} dt_node_t;

typedef struct
{
    char key[DT_KEY_LEN];
    uint32_t len;
    char val[];
} dt_prop_t;

extern void* gEntryPoint;
extern boot_args *gBootArgs;
extern dt_node_t *gDeviceTree;
extern uint32_t* gFramebuffer;
uint64_t gIOBase;
extern volatile uint32_t *gTZRegbase;
extern uint64_t gInterruptBase;
extern uint64_t gPMGRBase;
extern uint64_t gWDTBase;

extern int iboot_func_init(void);
extern void iboot_func_load(void);

extern int dt_check(void* mem, uint32_t size, uint32_t* offp);
extern int dt_parse(dt_node_t* node, int depth, uint32_t* offp, int (*cb_node)(void*, dt_node_t*), void* cbn_arg, int (*cb_prop)(void*, dt_node_t*, int, const char*, void*, uint32_t), void* cbp_arg);
extern dt_node_t* dt_find(dt_node_t* node, const char* name);
extern void* dt_prop(dt_node_t* node, const char* key, uint32_t* lenp);
extern void* dt_get_prop(const char* device, const char* prop, uint32_t* size);
extern struct memmap* dt_alloc_memmap(dt_node_t* node, const char* name);
extern uint64_t dt_get_u64_prop(const char* device, const char* prop);
extern uint32_t dt_get_u32_prop(const char* device, const char* prop);
extern uint64_t dt_get_u64_prop_i(const char* device, const char* prop, uint32_t idx);


// iboot
typedef int (*printf_t)(const char *format, ...);
printf_t iprintf;
typedef int (*jumpto_t)(void* arg0, void* arg1);
jumpto_t jumpto;
typedef void (*fsboot_t)(void);
fsboot_t fsboot;
typedef int (*panic_t)(const char *format, ...);
panic_t real_panic;
int panic(const char* panic_string);

// main
int iboot_func_init(void);
void iboot_func_load(void);

// libc
int strcmp(const char *s1, const char *s2);
my_size_t strlen(const char * str);
void *memset(void *s, int c, my_size_t n);
void *memmem(const void *haystack, my_size_t hlen, const void *needle, my_size_t nlen);
void *memcpy(void *dst, const void *src, my_size_t len);
void *memmove(void *dest, const void *src, my_size_t n);
char *strcpy(char *to, const char *from);
int strncmp(const char *s1, const char *s2, my_size_t n);
char *strchr(const char *p, int ch);
int isdigit(int c);
int isalpha(int c);
int isspace(int c);
int isupper(int c);
long atoi(const char* S);
char* strcat (char* dest, char* src);
unsigned long long strtoull(const char *str, char **endptr, int base);
int puts(const char* str);
char *strncpy(char *dest, const char *src, my_size_t n);

// pongo
char* command_tokenize(char* str, unsigned int strbufsz);
void wdt_disable();
void wdt_enable();
void pmgr_init();

// command
void peek(char* addr_str, char* size_str);
void poke(char* addr_str, char* u64_data);

#endif
