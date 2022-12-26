OBJECTS = main.o shellcode.S.o

all: $(OBJECTS)

%.o: %.c
	$(CC) -c $(CFLAGS) $<

shellcode.S.o: shellcode.S
	$(CC) -c $(CFLAGS) shellcode.S -o shellcode.S.o

.PHONY: all
