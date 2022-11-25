
#include <common.h>
#include <fuse/fuse.h>

uint64_t gFuseBase;
#define FUSE_REG(n) ((volatile uint32_t*)gFuseBase)[(n)]

static bool fuse_is_demoted(void)
{
    return (FUSE_REG(0) & 0x1) == 0x0;
}

static bool fuse_is_locked(void)
{
    return (FUSE_REG(1) & 0x80000000) != 0x0;
}

static bool fuse_demote(void)
{
    if(fuse_is_demoted())
        return true;
    
    if(fuse_is_locked())
        return false;
    
    FUSE_REG(0) &= 0xfffffffe;
    return true;
}

static void fuse_lock(void)
{
    FUSE_REG(1) |= 0x80000000;
}

void fuse_cmd_status(void)
{
    uint32_t val0 = FUSE_REG(0),
    val1 = FUSE_REG(1);
    iprintf("Reg 0: 0x%08x (demoted: %s)\n"
            "Reg 1: 0x%08x (locked: %s)\n",
            val0, (val0 & 0x1) == 0x0 ? "yes" : "no",
            val1, (val1 & 0x80000000) ? "yes" : "no");
}

void fuse_cmd_demote(void)
{
    if(fuse_is_demoted())
    {
        iprintf("Device is already demoted.\n");
        return;
    }
    if(fuse_is_locked())
    {
        iprintf("Sorry, fuses are already locked.\n");
        return;
    }
    fuse_demote();
}

void fuse_cmd_lock(void)
{
    if(fuse_is_locked())
    {
        iprintf("Fuses are already locked.\n");
        return;
    }
    fuse_lock();
}
