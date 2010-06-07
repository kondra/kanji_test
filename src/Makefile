#EXES = $(basename $(wildcard *.c))
CC = gcc
CFLAGS = -Wall -g `pkg-config --cflags --libs gtk+-2.0`

OBJ = kanji.o add_kanji.o
TARGET = kanji_test

#all: 
#	$(MAKE) $(EXES)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

$(OBJ): kanji.h

clean:
	rm -f add_kanji *.o 
