CFLAGS=-Wall -Werror
GTKFLAGS=`pkg-config --cflags gtk+-3.0`
GTKLIBS=`pkg-config --libs gtk+-3.0`
FILES=bmp_viewer.c bmp_reader.c
OUTPUT=bmp_viewer

all: $(FILES)
	gcc $(CFLAGS) $(GTKFLAGS) $(FILES) -o $(OUTPUT) $(GTKLIBS)

reader: bmp_reader.c
	gcc $(CFLAGS) bmp_reader.c -o bmp_reader
