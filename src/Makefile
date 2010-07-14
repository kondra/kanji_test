#EXES = $(basename $(wildcard *.c))
CC = gcc
CFLAGS = -Wall -g `pkg-config --cflags --libs gtk+-2.0`

OBJ1 = kanji.o add_kanji.o view_kanji.o kanji_article_view.o main.o
OBJ2 = kanji.o add_kanji.o view_kanji.o kanji_article_view.o load_new_kanji.o

TARGET1 = kanji_test
TARGET2 = load_new 

#all: 
#	$(MAKE) $(EXES)

$(TARGET1): $(OBJ1)
	$(CC) $(CFLAGS) $(OBJ1) -o $(TARGET1)
	$(CC) $(CFLAGS) main2.c -o main2

$(TARGET2): $(OBJ2)
	$(CC) $(CFLAGS) $(OBJ2) -o $(TARGET2)

load_new_kanji.o: view_kanji.h add_kanji.h kanji.h
kanji.o: kanji.h
add_kanji.o: add_kanji.h kanji.h
main.o: view_kanji.h add_kanji.h kanji.h
view_kanji.o: view_kanji.h kanji.h add_kanji.h
kanji_article_view.o: kanji_article_view.h kanji.h

clean:
	rm -f $(TARGET1) *.o 
	rm -f multirad
	rm -f main2
	rm -f $(TARGET2)
