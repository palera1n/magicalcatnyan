CC = xcrun -sdk iphoneos clang

OBJCOPY	= /opt/homebrew/opt/binutils/bin/gobjcopy

CFLAGS	= -Iinclude/ -Iinclude/drivers/ -Iapple-include/ -I. -DDER_TAG_SIZE=8 -target arm64-apple-ios12.0 -Os
CFLAGS	+= -Wall -Wextra -Wno-unused-parameter -Wno-incompatible-library-redeclaration -fno-stack-protector -nostdlib -static -nostdlibinc -Wl,-preload -Wl,-no_uuid
CFLAGS	+= -Wl,-e,start -Wl,-order_file,sym_order.txt -Wl,-image_base,0x100000000 -Wl,-sectalign,__DATA,__common,0x8 -Wl,-segalign,0x4000

OBJ = payload

SOURCE	=	\
			offsetfinder.c \
			printf.c \
			command.c \
			lowlevel.c \
			drivers/dt/dtree.c \
			drivers/dt/dtree_getprop.c \
			drivers/tz/tz.c \
			drivers/framebuffer/fb.c \
			drivers/recfg/recfg.c \
			drivers/recfg/recfg_soc.c \
			lib/memset.c \
			lib/memmem.c \
			lib/memmove.c \
			lib/strcmp.c \
			lib/strlen.c \
			lib/strcpy.c \
			lib/strncpy.c \
			lib/strchr.c \
			lib/strncmp.c \
			lib/isalpha.c \
			lib/isdigit.c \
			lib/isspace.c \
			lib/isupper.c \
			lib/strtoull.c \
			lib/strcat.c \
			lib/puts.c \
			lib/bzero.c
			
.PHONY: all

all:
	$(CC) entry.S payload.c drivers/iboot/t8015.c -DPAYLOAD_T8015 $(SOURCE) $(CFLAGS) -o $(OBJ)_t8015.o
	./vmacho -fM 0x80000 $(OBJ)_t8015.o $(OBJ)_t8015.bin
	$(CC) entry.S payload.c drivers/iboot/t8010.c -DPAYLOAD_T8010 $(SOURCE) $(CFLAGS) -o $(OBJ)_t8010.o
	./vmacho -fM 0x80000 $(OBJ)_t8010.o $(OBJ)_t8010.bin
	$(CC) entry.S payload.c drivers/iboot/s8000.c -DPAYLOAD_S8000 $(SOURCE) $(CFLAGS) -o $(OBJ)_s8000.o
	./vmacho -fM 0x80000 $(OBJ)_s8000.o $(OBJ)_s8000.bin
	
clean:
	-$(RM) $(OBJ).o
	-$(RM) $(OBJ).bin
	-$(RM) $(OBJ)_t8010.o
	-$(RM) $(OBJ)_t8010.bin
	-$(RM) $(OBJ)_s8000.o
	-$(RM) $(OBJ)_s8000.bin
