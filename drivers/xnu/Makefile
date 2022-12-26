OBJECTS = xnu.o xnu.S.o

all: $(OBJECTS)

%.o: %.c
	$(CC) -c $(CFLAGS) $<

xnu.S.o: xnu.S
	$(CC) -c $(CFLAGS) xnu.S -o xnu.S.o

.PHONY: all
