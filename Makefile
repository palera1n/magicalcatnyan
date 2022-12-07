CC = xcrun -sdk iphoneos clang

SRC_ROOT = $(shell pwd)
OBJCOPY	= /opt/homebrew/opt/binutils/bin/gobjcopy
DRIVERS = tz plat dt recfg framebuffer

CFLAGS	= -I$(SRC_ROOT)/include -I$(SRC_ROOT)/apple-include -I$(SRC_ROOT) -DDER_TAG_SIZE=8 -target arm64-apple-ios12.0 -Os
CFLAGS	+= -Wall -Wextra -Wno-unused-parameter -Wno-incompatible-library-redeclaration -fno-stack-protector -nostdlib -static -nostdlibinc
LDFLAGS	=  -Wl,-preload -Wl,-no_uuid -Wl,-e,start -Wl,-order_file,sym_order.txt -Wl,-image_base,0x100000000 -Wl,-sectalign,__DATA,__common,0x8 -Wl,-segalign,0x4000

OBJ = payload

TOP_OBJECTS = lowlevel.o offsetfinder.o printf.o command.o

OBJECTS	=	\
			drivers/dt/dtree.o \
			drivers/dt/dtree_getprop.o \
			drivers/tz/tz.o \
			drivers/framebuffer/fb.o \
			drivers/recfg/recfg.o \
			drivers/recfg/recfg_soc.o \
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

payload: $(OBJ)_s8000.bin $(OBJ)_t8010.bin $(OBJ)_t8015.bin 

%.o: %.c
	$(CC) -c $(CFLAGS) $<

drivers:
	$(MAKE) -C drivers

lib:
	$(MAKE) -C lib

payload_%.o: drivers lib $(TOP_OBJECTS)
	$(CC) entry.S payload.c drivers/plat/$*.o -DPAYLOAD_$* -DBUILDING_PAYLOAD $(OBJECTS) $(TOP_OBJECTS) $(CFLAGS) $(LDFLAGS) -o $(OBJ)_$*.o

payload_%.bin: payload_%.o
	./vmacho -fM 0x80000 $(OBJ)_$*.o $(OBJ)_$*.bin
	
clean:
	find . -name '*.bin' -type f -delete
	find . -name '*.o' -type f -delete

.PHONY: all clean drivers lib payload