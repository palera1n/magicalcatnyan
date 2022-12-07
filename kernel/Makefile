OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

all: $(OBJECTS) entry.o

%.o: %.c
	$(CC) -c $(CFLAGS) $<

entry.o:
	$(CC) -c $(CFLAGS) entry.S

.PHONY: all
