CC = xcrun -sdk iphoneos clang
CC_FOR_BUILD = clang
CFLAGS_FOR_BUILD += -Os -Wall -Wextra
LDFLAGS_FOR_BUILD ?= -flto=thin

SRC_ROOT = $(shell pwd)
SUBDIRS = kernel lib drivers

OBJCOPY	= /opt/homebrew/opt/binutils/bin/gobjcopy
DRIVERS = plat dt framebuffer

CFLAGS	= -I$(SRC_ROOT)/include -I$(SRC_ROOT)/apple-include -I$(SRC_ROOT) -DDER_TAG_SIZE=8 -target arm64-apple-ios12.0 -Os
CFLAGS	+= -Wall -Wextra -Wno-unused-parameter -Wno-incompatible-library-redeclaration -fno-stack-protector -nostdlib -static -nostdlibinc
LDFLAGS	=  -Wl,-preload -Wl,-no_uuid -Wl,-e,start -Wl,-order_file,sym_order.txt -Wl,-image_base,0x100000000 -Wl,-sectalign,__DATA,__common,0x8 -Wl,-segalign,0x4000

OBJ = payload

OBJECTS	= \
		drivers/dt/dtree.o \
		drivers/dt/dtree_getprop.o \
		drivers/framebuffer/fb.o \
		kernel/command.o \
		kernel/lowlevel.o \
		kernel/printf.o \
		kernel/offsetfinder.o \
		kernel/entry.o \
		lib/memset.o \
		lib/memmem.o \
		lib/memmove.o \
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
		lib/bzero.o

export DRIVERS CC CFLAGS

all: payload

vmacho:
	$(CC_FOR_BUILD) $(CFLAGS_FOR_BUILD) $(LDFLAGS_FOR_BUILD) -o vmacho vmacho.c

payload: $(OBJ)_s8000.bin $(OBJ)_t8010.bin $(OBJ)_t8015.bin

%.o: %.c
	$(CC) -c $(CFLAGS) $<

$(SUBDIRS):
	$(MAKE) -C "$@"

newlib:
	$(MAKE) -C pongoOS/newlib all

pongo:
	$(MAKE) -C pongoOS build/libpongo.a

payload_%.o: $(SUBDIRS) newlib pongo
	$(CC) -DPAYLOAD_$* pongoOS/build/libpongo.a pongoOS/newlib/build/libc.a payload.c drivers/plat/$*.o -DBUILDING_PAYLOAD $(OBJECTS) $(CFLAGS) $(LDFLAGS) -o $(OBJ)_$*.o

payload_%.bin: payload_%.o vmacho
	./vmacho -fM 0x80000 $(OBJ)_$*.o $(OBJ)_$*.bin

clean:
	find . -name '*.bin' -type f -delete
	find . -name '*.o' -type f -delete
	$(MAKE) -C pongoOS clean

distclean: clean
	rm -f vmacho

.PHONY: all distclean clean payload $(SUBDIRS)
