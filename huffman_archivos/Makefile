CC = gcc
CFLAGS = -Wall -Iinclude
SRCDIR = src
INCDIR = include
SOURCES = $(SRCDIR)/frecuencias.c $(SRCDIR)/arbol.c $(SRCDIR)/io_handler.c
COMPRESOR_OBJ = $(SRCDIR)/compresor.o
DECOMPRESOR_OBJ = $(SRCDIR)/descompresor.o
TARGET_COMPRESOR = huffman-compresor
TARGET_DECOMPRESOR = huffman-descompresor

all: $(TARGET_COMPRESOR) $(TARGET_DECOMPRESOR)

$(TARGET_COMPRESOR): $(COMPRESOR_OBJ) $(SOURCES:.c=.o)
	$(CC) $(CFLAGS) -o $@ $(COMPRESOR_OBJ) $(SOURCES:.c=.o)

$(TARGET_DECOMPRESOR): $(DECOMPRESOR_OBJ) $(SOURCES:.c=.o)
	$(CC) $(CFLAGS) -o $@ $(DECOMPRESOR_OBJ) $(SOURCES:.c=.o)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SRCDIR)/*.o $(TARGET_COMPRESOR) $(TARGET_DECOMPRESOR)

.PHONY: all clean