OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

all: $(OBJECTS) entry.o

%.o: %.c
	$(CC) -c $(CFLAGS) $<

entry.o: entry.S
	$(CC) -c $(CFLAGS) entry.S

.PHONY: all
