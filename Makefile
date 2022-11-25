CC		= xcrun -sdk iphoneos gcc

OBJCOPY	= /opt/homebrew/opt/binutils/bin/gobjcopy

CFLAGS	= -Iinclude/ -Iinclude/drivers/ -Iapple-include/ -DDER_TAG_SIZE=8 -target arm64-apple-ios12.0
CFLAGS	+= -Wall -Wno-incompatible-library-redeclaration -fno-stack-protector -nostdlib -static -nostdlibinc -Wl,-preload -Wl,-no_uuid
CFLAGS	+= -Wl,-e,start -Wl,-order_file,sym_order.txt -Wl,-image_base,0x100000000 -Wl,-sectalign,__DATA,__common,0x8 -Wl,-segalign,0x4000

OBJ		= payload

SOURCE	=	\
			payload.c \
			offsetfinder.c \
			#kpf.c \
			shellcode.S \
			drivers/fuse/fuse.c \
			drivers/xnu/xnu.s \
			drivers/dt/dtree.c \
			drivers/dt/dtree_getprop.c \
			lib/memset.c \
			lib/memmem.c \
			lib/memcmp.c \
			lib/memmove.c \
			lib/strcmp.c \
			lib/strlen.c \
			lib/strstr.c \
			lib/strtoull.c \
			lib/strcpy.c \
			lib/strchr.c \
			lib/strncmp.c
			
.PHONY: all

all:
	$(CC) entry.S $(SOURCE) $(CFLAGS) -o $(OBJ).o
	./vmacho -fM 0x80000 $(OBJ).o $(OBJ).bin
	
clean:
	-$(RM) $(ASMSOURCE)
	-$(RM) $(OBJ).o
	-$(RM) $(OBJ).bin
	
