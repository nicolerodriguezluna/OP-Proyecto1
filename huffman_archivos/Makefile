CC = gcc
CFLAGS = -Wall -Iinclude
SRCDIR = src
INCDIR = include
SOURCES = $(SRCDIR)/huffman.c $(SRCDIR)/frecuencias.c $(SRCDIR)/arbol.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = huffman

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean