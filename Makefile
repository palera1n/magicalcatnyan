CC		= xcrun -sdk iphoneos gcc

OBJCOPY	= /opt/homebrew/opt/binutils/bin/gobjcopy

CFLAGS	= -Iinclude/ -Iinclude/drivers/ -Iapple-include/ -I. -DDER_TAG_SIZE=8 -target arm64-apple-ios12.0
CFLAGS	+= -Wall -Wextra -Wno-unused-parameter -Wno-incompatible-library-redeclaration -fno-stack-protector -nostdlib -static -nostdlibinc -Wl,-preload -Wl,-no_uuid
CFLAGS	+= -Wl,-e,start -Wl,-order_file,sym_order.txt -Wl,-image_base,0x100000000 -Wl,-sectalign,__DATA,__common,0x8 -Wl,-segalign,0x4000

OBJ		= payload

SOURCE	=	\
			offsetfinder.c \
			printf.c \
			command.c \
			drivers/dt/dtree.c \
			drivers/dt/dtree_getprop.c \
			drivers/tz/tz.c \
			drivers/framebuffer/fb.c \
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
			lib/atoi.c \
			lib/strcat.c \
			lib/puts.c
			
.PHONY: all

all:
	$(CC) entry.S payload.c -DPAYLOAD_T8015 $(SOURCE) $(CFLAGS) -o $(OBJ).o
	./vmacho -fM 0x80000 $(OBJ).o $(OBJ).bin
	$(CC) entry.S payload_t8010.c -DPAYLOAD_T8010 $(SOURCE) $(CFLAGS) -o $(OBJ)_t8010.o
	./vmacho -fM 0x80000 $(OBJ)_t8010.o $(OBJ)_t8010.bin
	
clean:
	-$(RM) $(OBJ).o
	-$(RM) $(OBJ).bin
	-$(RM) $(OBJ)_t8010.o
	-$(RM) $(OBJ)_t8010.bin
	
