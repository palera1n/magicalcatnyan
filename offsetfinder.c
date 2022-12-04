
#include <stdint.h>
#include <offsetfinder.h>

static uint32_t* find_next_insn_matching_64(uint64_t region, uint8_t* kdata, size_t ksize, uint32_t* current_instruction, int (*match_func)(uint32_t*))
{
    while((uintptr_t)current_instruction < (uintptr_t)kdata + ksize - 4) {
        current_instruction++;
        
        if(match_func(current_instruction)) {
            return current_instruction;
        }
    }
    
    return NULL;
}

static uint32_t bit_range_64(uint32_t x, int start, int end)
{
    x = (x << (31 - start)) >> (31 - start);
    x = (x >> end);
    return x;
}

static uint64_t real_signextend_64(uint64_t imm, uint8_t bit)
{
    if ((imm >> bit) & 1) {
        return (-1LL << (bit + 1)) + imm;
    } else
        return imm;
}

static uint64_t signextend_64(uint64_t imm, uint8_t bit)
{
    //assert(bit > 0);
    return real_signextend_64(imm, bit - 1);
    /*
     if ((imm >> bit) & 1)
     return (uint64_t)(-1) - (~((uint64_t)1 << bit)) + imm;
     else
     return imm;
     */
}

static int insn_is_mov_imm_64(uint32_t* i)
{
    if ((*i & 0x7f800000) == 0x52800000)
        return 1;
    
    return 0;
}

static int insn_mov_imm_rd_64(uint32_t* i)
{
    return (*i & 0x1f);
}

static uint32_t insn_mov_imm_imm_64(uint32_t* i)
{
    return bit_range_64(*i, 20, 5);
}

static int insn_is_ldr_literal_64(uint32_t* i)
{
    // C6.2.84 LDR (literal) LDR Xt
    if ((*i & 0xff000000) == 0x58000000)
        return 1;
    
    // C6.2.84 LDR (literal) LDR Wt
    if ((*i & 0xff000000) == 0x18000000)
        return 1;
    
    // C6.2.95 LDR (literal) LDRSW Xt
    if ((*i & 0xff000000) == 0x98000000)
        return 1;
    
    return 0;
}

static int insn_add_reg_rm_64(uint32_t* i)
{
    return ((*i >> 16) & 0x1f);
}

static int insn_ldr_literal_rt_64(uint32_t* i)
{
    return (*i & 0x1f);
}

static uint64_t insn_ldr_literal_imm_64(uint32_t* i)
{
    uint64_t imm = (*i & 0xffffe0) >> 3;
    return signextend_64(imm, 21);
}

static uint64_t insn_adr_imm_64(uint32_t* i)
{
    uint64_t immhi = bit_range_64(*i, 23, 5);
    uint64_t immlo = bit_range_64(*i, 30, 29);
    uint64_t imm = (immhi << 2) + (immlo);
    return signextend_64(imm, 19+2);
}

static uint64_t insn_adrp_imm_64(uint32_t* i)
{
    uint64_t immhi = bit_range_64(*i, 23, 5);
    uint64_t immlo = bit_range_64(*i, 30, 29);
    uint64_t imm = (immhi << 14) + (immlo << 12);
    return signextend_64(imm, 19+2+12);
}

static int insn_is_adrp_64(uint32_t* i)
{
    if ((*i & 0x9f000000) == 0x90000000) {
        return 1;
    }
    
    return 0;
}

static int insn_adrp_rd_64(uint32_t* i)
{
    return (*i & 0x1f);
}

static int insn_is_add_imm_64(uint32_t* i)
{
    if ((*i & 0x7f000000) == 0x11000000)
        return 1;
    
    return 0;
}

static int insn_add_imm_rd_64(uint32_t* i)
{
    return (*i & 0x1f);
}

static int insn_add_imm_rn_64(uint32_t* i)
{
    return ((*i >> 5) & 0x1f);
}

static uint64_t insn_add_imm_imm_64(uint32_t* i)
{
    uint64_t imm = bit_range_64(*i, 21, 10);
    if (((*i >> 22) & 3) == 1)
        imm = imm << 12;
    return imm;
}

static int insn_add_reg_rd_64(uint32_t* i)
{
    return (*i & 0x1f);
}

static int insn_add_reg_rn_64(uint32_t* i)
{
    return ((*i >> 5) & 0x1f);
}

static int insn_is_add_reg_64(uint32_t* i)
{
    if ((*i & 0x7fe00c00) == 0x0b200000)
        return 1;
    
    return 0;
}

static int insn_is_adr_64(uint32_t* i)
{
    if ((*i & 0x9f000000) == 0x10000000)
        return 1;
    
    return 0;
}

static int insn_is_bl_64(uint32_t* i)
{
    if ((*i & 0xfc000000) == 0x94000000)
        return 1;
    else
        return 0;
}

static uint64_t insn_bl_imm32_64(uint32_t* i)
{
    uint64_t imm = (*i & 0x3ffffff) << 2;
    //PFExtLog("imm = %llx\n", imm);
    // sign extend
    uint64_t res = real_signextend_64(imm, 27);
    
    //PFExtLog("real_signextend_64 = %llx\n", res);
    
    return res;
}

int insn_is_funcbegin_64(uint32_t* i)
{
    if (*i == 0xa9bf7bfd)
        return 1;
    if (*i == 0xa9bc5ff8)
        return 1;
    if (*i == 0xa9bd57f6)
        return 1;
    if (*i == 0xa9ba6ffc)
        return 1;
    if (*i == 0xa9bb67fa)
        return 1;
    if (*i == 0xa9be4ff4)
        return 1;
    return 0;
}

static uint32_t* find_literal_ref_64(uint64_t region, uint8_t* kdata, size_t ksize, uint32_t* insn, uint64_t address)
{
    uint32_t* current_instruction = insn;
    uint64_t registers[32];
    memset(registers, 0, sizeof(registers));
    
    while((uintptr_t)current_instruction < (uintptr_t)(kdata + ksize))
    {
        if (insn_is_mov_imm_64(current_instruction))
        {
            int reg = insn_mov_imm_rd_64(current_instruction);
            uint64_t value = insn_mov_imm_imm_64(current_instruction);
            registers[reg] = value;
            //PFExtLog("%s:%d mov (immediate): reg[%d] is reset to %#llx\n", __func__, __LINE__, reg, value);
        }
        else if (insn_is_ldr_literal_64(current_instruction))
        {
            uintptr_t literal_address  = (uintptr_t)current_instruction + (uintptr_t)insn_ldr_literal_imm_64(current_instruction);
            if(literal_address >= (uintptr_t)kdata && (literal_address + 4) <= ((uintptr_t)kdata + ksize))
            {
                int reg = insn_ldr_literal_rt_64(current_instruction);
                uint64_t value =  *(uint64_t*)(literal_address);
                registers[reg] = value;
                //PFExtLog("%s:%d ldr (literal): reg[%d] is reset to %#llx\n", __func__, __LINE__, reg, value);
            }
        }
        else if (insn_is_adrp_64(current_instruction))
        {
            int reg = insn_adrp_rd_64(current_instruction);
            uint64_t value = ((((uintptr_t)current_instruction - (uintptr_t)kdata) >> 12) << 12) + insn_adrp_imm_64(current_instruction);
            registers[reg] = value;
            //PFExtLog("%s:%d adrp: reg[%d] is reset to %#llx\n", __func__, __LINE__, reg, value);
        }
        else if (insn_is_adr_64(current_instruction))
        {
            uint64_t value = (uintptr_t)current_instruction - (uintptr_t)kdata + insn_adr_imm_64(current_instruction);
            if (value == address)
            {
                //PFExtLog("%s:%d FINAL pointer is %#llx\n", __func__, __LINE__, (uint64_t)current_instruction - (uint64_t)kdata);
                return current_instruction;
            }
        }
        else if(insn_is_add_reg_64(current_instruction))
        {
            int reg = insn_add_reg_rd_64(current_instruction);
            if(insn_add_reg_rm_64(current_instruction) == 15 && insn_add_reg_rn_64(current_instruction) == reg)
            {
                uint64_t value = ((uintptr_t)current_instruction - (uintptr_t)kdata) + 4;
                registers[reg] += value;
                //PFExtLog("%s:%d adrp: reg[%d] += %#llx\n", __func__, __LINE__, reg, value);
                if(registers[reg] == address)
                {
                    //PFExtLog("%s:%d FINAL pointer is %#llx\n", __func__, __LINE__, (uint64_t)current_instruction - (uint64_t)kdata);
                    return current_instruction;
                }
            }
        }
        else if (insn_is_add_imm_64(current_instruction))
        {
            int reg = insn_add_imm_rd_64(current_instruction);
            if (insn_add_imm_rn_64(current_instruction) == reg)
            {
                uint64_t value = insn_add_imm_imm_64(current_instruction);
                registers[reg] += value;
                //PFExtLog("%s:%d adrp: reg[%d] += %#llx\n", __func__, __LINE__, reg, value);
                if (registers[reg] == address)
                {
                    //PFExtLog("%s:%d FINAL pointer is %#llx\n", __func__, __LINE__, (uint64_t)current_instruction - (uint64_t)kdata);
                    return current_instruction;
                }
            }
            
        }
        
        current_instruction++;
    }
    
    //PFExtLog("%s:%d FINAL pointer is NULL\n", __func__, __LINE__);
    return NULL;
}

uint64_t find_printf(uint64_t region, uint8_t* data, size_t size)
{
    uint8_t* str = memmem(data, size, "Entering recovery mode, starting command prompt\n", sizeof("Entering recovery mode, starting command prompt\n"));
    if(!str)
        return 0;
    
    // Find a reference to the string.
    uint32_t* ref = find_literal_ref_64(region, data, size, (uint32_t*)data, (uintptr_t)str - (uintptr_t)data);
    if (!ref)
        return 0;
    
    // find BL
    uint32_t *bl_addr = find_next_insn_matching_64(region, data, size, ref, insn_is_bl_64);
    if (!bl_addr)
        return 0;
    
    
    return ((uintptr_t)bl_addr - (uintptr_t)data) + insn_bl_imm32_64(bl_addr);
}

uint64_t find_mount_and_boot_system(uint64_t region, uint8_t* data, size_t size)
{
    if(!region)
        return 0;
    
    uint64_t maxsize = region + size;
    uint64_t search[1];
    
    uint8_t* str = memmem(data, size, "fsboot", sizeof("fsboot"));
    if(!str)
        return 0;
    
    uint64_t addr = ((uintptr_t)str - (uintptr_t)data) + region;
    if(maxsize < addr)
        return 0;
    
    search[0] = addr;
    uint8_t* ptr = memmem(data, size, search, sizeof(search));
    if(!ptr)
        return 0;
    
    uint64_t offset = ((uintptr_t)ptr - (uintptr_t)data) + 8;
    if(size < offset)
        return 0;
    
    uint64_t* dq = (uint64_t*)(data + offset);
    if(!dq)
        return 0;
    
    if(dq[0] < region)
        return 0;
    
    uint64_t ret = dq[0] - region;
    
    return ret;
}

uint64_t find_jumpto_bl(uint64_t region, uint8_t* data, size_t size)
{
    uint32_t search[2];
    
    uint8_t* str = memmem(data, size, "======== End of %s serial output. ========\n", sizeof("======== End of %s serial output. ========\n"));
    if(!str)
        return 0;
    
    // Find a reference to the string.
    uint32_t* ref = find_literal_ref_64(region, data, size, (uint32_t*)data, (uintptr_t)str - (uintptr_t)data);
    if (!ref)
        return 0;
    
    search[0] = 0xaa1403e0; // mov x0, x20
    search[1] = 0xaa1303e1; // mov x1, x19
    
    uint32_t* jump = memmem(ref, size - ((uintptr_t)ref - (uintptr_t)data), search, sizeof(search));
    if (!jump)
        return 0;
    
    uint32_t *bl_addr = find_next_insn_matching_64(region, data, size, jump, insn_is_bl_64);
    if (!bl_addr)
        return 0;
    
    return ((uintptr_t)bl_addr - (uintptr_t)data);
}

//uint64_t find_jumpto_func(uint64_t region, uint8_t* data, size_t size)
//{
//    uint32_t search[2];
//
//    uint8_t* str = memmem(data, size, "======== End of %s serial output. ========\n", sizeof("======== End of %s serial output. ========\n"));
//    if(!str)
//        return 0;
//
//    // Find a reference to the string.
//    uint32_t* ref = find_literal_ref_64(region, data, size, (uint32_t*)data, (uintptr_t)str - (uintptr_t)data);
//    if (!ref)
//        return 0;
//
//    search[0] = 0xaa1403e0; // mov x0, x20
//    search[1] = 0xaa1303e1; // mov x1, x19
//
//    uint32_t* jump = memmem(ref, size - ((uintptr_t)ref - (uintptr_t)data), search, sizeof(search));
//    if (!jump)
//        return 0;
//
//    uint32_t *bl_addr = find_next_insn_matching_64(region, data, size, jump, insn_is_bl_64);
//    if (!bl_addr)
//        return 0;
//
//    return ((uintptr_t)bl_addr - (uintptr_t)data) + insn_bl_imm32_64(bl_addr);
//}

uint64_t find_jumpto_func(uint64_t region, uint8_t* data, size_t size)
{
    uint32_t search[2];
    
    uint8_t* str = memmem(data, size, "======== End of %s serial output. ========\n", sizeof("======== End of %s serial output. ========\n"));
    if(!str)
        return 0;
    
    // Find a reference to the string.
    uint32_t* ref = find_literal_ref_64(region, data, size, (uint32_t*)data, (uintptr_t)str - (uintptr_t)data);
    if (!ref)
        return 0;
    
    search[0] = 0xaa1403e0; // mov x0, x20
    search[1] = 0xaa1303e1; // mov x1, x19
    
    uint32_t* jump = memmem(ref, size - ((uintptr_t)ref - (uintptr_t)data), search, sizeof(search));
    if (!jump)
        return 0;
    
    return ((uintptr_t)jump - (uintptr_t)data) + 8 + 4;
}

uint64_t find_panic(uint64_t region, uint8_t* data, size_t size)
{
    uint8_t* str = memmem(data, size, "unknown LPDDR4 density %d", sizeof("unknown LPDDR4 density %d"));
    
    if(!str)
        str = memmem(data, size, "Failed to find adfe tunables info, bailing adfe_init()\n", sizeof("Failed to find adfe tunables info, bailing adfe_init()\n"));
    
    if(!str)
        return 0;
    
    // Find a reference to the string.
    uint32_t* ref = find_literal_ref_64(region, data, size, (uint32_t*)data, (uintptr_t)str - (uintptr_t)data);
    if (!ref)
        return 0;
    
    // find BL
    uint32_t *bl_addr = find_next_insn_matching_64(region, data, size, ref, insn_is_bl_64);
    if (!bl_addr)
        return 0;
    
    
    return ((uintptr_t)bl_addr - (uintptr_t)data) + insn_bl_imm32_64(bl_addr);
}
