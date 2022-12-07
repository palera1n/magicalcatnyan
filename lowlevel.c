#include <common.h>

uint64_t gInterruptBase;
uint64_t gPMGRBase;
uint64_t gWDTBase;

typedef struct
{
    uint64_t addr;
    uint64_t size;
} pmgr_reg_t;

typedef struct
{
    uint32_t reg;
    uint32_t off;
    uint32_t idk;
} pmgr_map_t;

typedef struct
{
    uint32_t flg : 8,
             a   : 16,
             id  : 8;
    uint32_t b;
    uint32_t c   : 16,
             idx :  8,
             map :  8;
    uint32_t d;
    uint32_t e;
    uint32_t f;
    uint32_t g;
    uint32_t h;
    char name[0x10];
} pmgr_dev_t;

static uint32_t gPMGRreglen = 0;
static uint32_t gPMGRmaplen = 0;
static uint32_t gPMGRdevlen = 0;
static pmgr_reg_t *gPMGRreg = NULL;
static pmgr_map_t *gPMGRmap = NULL;
static pmgr_dev_t *gPMGRdev = NULL;

#define WDT_CHIP_TMR (*(volatile uint32_t*)(gWDTBase + 0x0))
#define WDT_CHIP_RST (*(volatile uint32_t*)(gWDTBase + 0x4))
#define WDT_CHIP_INT (*(volatile uint32_t*)(gWDTBase + 0x8))
#define WDT_CHIP_CTL (*(volatile uint32_t*)(gWDTBase + 0xc))

#define WDT_SYS_TMR (*(volatile uint32_t*)(gWDTBase + 0x10))
#define WDT_SYS_RST (*(volatile uint32_t*)(gWDTBase + 0x14))
#define WDT_SYS_CTL (*(volatile uint32_t*)(gWDTBase + 0x1c))

int panic(const char* panic_string) {
    if (screen_is_initialized == true) {
        screen_write("panic: ");
        screen_puts(panic_string);
    }
    return real_panic(panic_string);
}

void wdt_disable()
{
    if (!gWDTBase) return;
    WDT_CHIP_CTL = 0x0; // Disable WDT
    WDT_SYS_CTL  = 0x0; // Disable WDT
}

void wdt_enable() {
	return;
}

void pmgr_init()
{
    dt_node_t *pmgr = dt_find(gDeviceTree, "pmgr");
    gPMGRreg = dt_prop(pmgr, "reg",     &gPMGRreglen);
    gPMGRmap = dt_prop(pmgr, "ps-regs", &gPMGRmaplen);
    gPMGRdev = dt_prop(pmgr, "devices", &gPMGRdevlen);
    gPMGRreglen /= sizeof(*gPMGRreg);
    gPMGRmaplen /= sizeof(*gPMGRmap);
    gPMGRdevlen /= sizeof(*gPMGRdev);
    gPMGRBase = gIOBase + gPMGRreg[0].addr;
    gWDTBase  = gIOBase + dt_get_u64_prop("wdt", "reg");
}
