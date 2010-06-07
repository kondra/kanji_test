#EXES = $(basename $(wildcard *.c))
CC = gcc
CFLAGS = -Wall -g `pkg-config --cflags --libs gtk+-2.0`

OBJ = kanji.o add_kanji.o

#all: 
#	$(MAKE) $(EXES)

add_kanji: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o add_kanji

$(OBJ): kanji.h

clean:
	rm -f add_kanji *.o 
