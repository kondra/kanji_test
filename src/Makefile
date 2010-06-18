#EXES = $(basename $(wildcard *.c))
CC = gcc
CFLAGS = -Wall -g `pkg-config --cflags --libs gtk+-2.0`

OBJ = kanji.o add_kanji.o view_kanji.o main.o
TARGET = kanji_test

#all: 
#	$(MAKE) $(EXES)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

kanji.o: kanji.h
add_kanji.o: add_kanji.h kanji.h
main.o: view_kanji.h add_kanji.h kanji.h
view_kanji.o: view_kanji.h kanji.h add_kanji.h

clean:
	rm -f $(TARGET) *.o 
