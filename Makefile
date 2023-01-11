CC = xcrun -sdk iphoneos clang
CC_FOR_BUILD = clang
CFLAGS_FOR_BUILD += -Os -Wall -Wextra
LDFLAGS_FOR_BUILD ?= -flto=thin

SRC_ROOT = $(shell pwd)
SUBDIRS = kernel lib drivers kpf

OBJCOPY	= /opt/homebrew/opt/binutils/bin/gobjcopy
DRIVERS = plat dt framebuffer xnu tz

CFLAGS	= -I$(SRC_ROOT)/include -I$(SRC_ROOT)/apple-include -I$(SRC_ROOT) -DDER_TAG_SIZE=8 -target arm64-apple-ios12.0 -g -ffreestanding -flto=thin
CFLAGS	+= -Wall -Wextra -Wno-unused-parameter -Wno-incompatible-library-redeclaration -fno-stack-protector -nostdlib -static -nostdlibinc
LDFLAGS	=  -Wl,-preload -Wl,-no_uuid -Wl,-e,start -Wl,-order_file,sym_order.txt -Wl,-sectalign,__DATA,__common,0x8 -Wl,-segalign,0x4000

ifneq ($(DEV_BUILD),)
CFLAGS += -DDEV_BUILD=$(DEV_BUILD) -Og
OBJECTS = kernel/command.o
else
CFLAGS += -Os
endif

MAGICALCATNYAN_VERSION               ?= 1.0.0~b2-$(shell git rev-parse HEAD | cut -c1-8)
P = )

CFLAGS += -DMAGICALCATNYAN_VERSION='"$(MAGICALCATNYAN_VERSION)"'
# CFLAGS += -DXNU_PF_DUMP_JIT -DDEBUG_DOUNMOUNT -DDEBUG_MM -DXNU_PF_DEBUG_SPAM
OBJ = payload

OBJECTS	+= \
		kernel/lowlevel.o \
		kernel/fakemm.o \
		kernel/printf.o \
		kernel/offsetfinder.o \
		kernel/entry.o \
		kpf/main.o \
		kpf/shellcode.S.o \
		drivers/dt/dtree.o \
		drivers/dt/dtree_getprop.o \
		drivers/framebuffer/fb.o \
		drivers/xnu/xnu.o \
		drivers/xnu/xnu.S.o \
		drivers/tz/tz.o \
		lib/memset.o \
		lib/memmem.o \
		lib/memmove.o \
		lib/memcmp.o \
		lib/strstr.o \
		lib/strcmp.o \
		lib/strlen.o \
		lib/strcpy.o \
		lib/strncpy.o \
		lib/strchr.o \
		lib/strncmp.o \
		lib/isalpha.o \
		lib/isdigit.o \
		lib/isspace.o \
		lib/isupper.o \
		lib/strtoull.o \
		lib/strcat.o \
		lib/puts.o \
		lib/bzero.o \
		lib/strtoul.o \
		lib/errno.o \
		lib/strtoimax.o

export DRIVERS CC CFLAGS

all: payload

vmacho: vmacho.c
	$(CC_FOR_BUILD) $(CFLAGS_FOR_BUILD) $(LDFLAGS_FOR_BUILD) -o vmacho vmacho.c

payload: $(OBJ)_s8000.bin $(OBJ)_t8010.bin $(OBJ)_t8015.bin

%.o: %.c
	$(CC) -c $(CFLAGS) $<

$(SUBDIRS):
	$(MAKE) -C "$@"

payload_%.o: $(SUBDIRS)
	$(CC) -DPAYLOAD_$* -Wl,-image_base,$(shell if [ "$*" = "t8015" ]; then echo 0x800F00000; else echo 0x800700000; fi) payload.c drivers/plat/$*.o -DBUILDING_PAYLOAD $(OBJECTS) $(CFLAGS) $(LDFLAGS) -o $(OBJ)_$*.o

payload_%.bin: payload_%.o vmacho
	./vmacho -fM 0x80000 $(OBJ)_$*.o $(OBJ)_$*.bin

clean:
	find . -name '*.bin' -type f -delete
	find . -name '*.o' -type f -delete

distclean: clean
	rm -f vmacho

.PHONY: all distclean clean payload $(SUBDIRS)
