
.globl _gprintf
.align 2

.globl _invalidate_icache
.globl start
.align 4
start:

b _payload
b _jump_hook
nop
nop

nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop

_gfusebase:
.quad 0x4040404040404040

_gprintf:
.quad 0x4141414141414141

_gfsboot:
.quad 0x4242424242424242

_gjumpto:
.quad 0x4343434343434343

_gmalloc:
.quad 0x4444444444444444

_gpanic:
.quad 0x4545454545454545

_gfree:
.quad 0x4646464646464646

nop
nop
nop
nop
nop
nop
nop
nop

_invalidate_icache:
dsb ish
ic iallu
dsb ish
isb
ret

nop
nop
nop
nop
nop
nop
nop
nop
