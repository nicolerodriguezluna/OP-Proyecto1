CC = gcc
CFLAGS = -O2 -Wall -Wextra -std=c11 -pthread


INCLUDES = -Iinclude -I../huffman_terminal/include -Isrc
SRC = \
src/pthreads_freq.c \
src/pthreads_encode.c \
src/bitbuf.c \
src/tree_codec.c


# binarios
BIN_COMP = huff_p_comp
BIN_DECO = huff_p_deco


LIBS = ../huffman_terminal/src/arbol.c ../huffman_terminal/src/frecuencias.c


all: $(BIN_COMP) $(BIN_DECO)


$(BIN_COMP): $(SRC) src/pthreads_main_compress.c $(LIBS)
$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^


$(BIN_DECO): $(SRC) src/pthreads_main_decompress.c $(LIBS)
$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^


clean:
rm -f $(BIN_COMP) $(BIN_DECO) *.o


.PHONY: all clean