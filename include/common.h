#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stdbool.h>
#include "drivers/framebuffer/fb.h"
#include "drivers/tz/tz.h"

#include "../printf.h"

typedef uint64_t my_size_t;

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

#define DT_KEY_LEN              0x20
#define BOOT_LINE_LENGTH_iOS12  0x100
#define BOOT_LINE_LENGTH_iOS13  0x260

bool is_16k();

struct Boot_Video {
    unsigned long    v_baseAddr;    /* Base address of video memory */
    unsigned long    v_display;     /* Display Code (if Applicable */
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
