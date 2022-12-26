OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

all: $(OBJECTS)

%.o: %.c
	$(CC) -c $(CFLAGS) $<

.PHONY: all
