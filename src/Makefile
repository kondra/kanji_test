#EXES = $(basename $(wildcard *.c))
CC = gcc
CFLAGS = -Wall -g `pkg-config --cflags --libs gtk+-2.0`

OBJ1 = kanji.o add_kanji.o view_kanji.o kanji_article_view.o main.o
OBJ2 = kanji.o add_kanji.o view_kanji.o kanji_article_view.o load_new_kanji.o
OBJ3 = kanji.o add_kanji.o view_kanji.o kanji_article_view.o main2.o

TARGET1 = kanji_test
TARGET2 = load_new 
TARGET3 = multirad 

#all: 
#	$(MAKE) $(EXES)

$(TARGET3): $(OBJ3)
	$(CC) $(CFLAGS) $(OBJ3) -o $(TARGET3)

$(TARGET1): $(OBJ1)
	$(CC) $(CFLAGS) $(OBJ1) -o $(TARGET1)

$(TARGET2): $(OBJ2)
	$(CC) $(CFLAGS) $(OBJ2) -o $(TARGET2)

load_new_kanji.o: view_kanji.h add_kanji.h kanji.h
kanji.o: kanji.h
add_kanji.o: add_kanji.h kanji.h
main.o: view_kanji.h add_kanji.h kanji.h
main2.o: view_kanji.h add_kanji.h kanji.h kanji_article_view.h
view_kanji.o: view_kanji.h kanji.h add_kanji.h
kanji_article_view.o: kanji_article_view.h kanji.h

clean:
	rm -f $(TARGET1) *.o 
	rm -f $(TARGET2)
	rm -f $(TARGET3)
