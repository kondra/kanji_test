EXES = $(basename $(wildcard *.c))
CC = gcc
CFLAGS = -Wall -g `pkg-config --cflags --libs gtk+-2.0`

all: 
	$(MAKE) $(EXES)

%: %.c
	$(CC) $(CFLAGS) $@.c -o $@

clean:
	rm -f $(EXES) *.o
