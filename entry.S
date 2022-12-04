
.globl _gprintf
.align 2

.globl start
.align 4
start:

// 0x0000
b _payload
b _jump_hook
nop
nop

// 0x0010
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

// 0x0040
_gprintf:
.quad 0x4040404040404040

_gfsboot:
.quad 0x4141414141414141

_gjumpto:
.quad 0x4242424242424242

_gpanic:
.quad 0x4343434343434343

nop
nop
nop
nop
