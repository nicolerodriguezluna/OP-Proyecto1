#ifndef PTHREADS_HUFF_H
#define PTHREADS_HUFF_H


#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


// Reusa tus structs existentes
#include "../huffman_terminal/include/huffman.h"
#include "../huffman_terminal/include/frecuencias.h"
#include "../huffman_terminal/include/arbol.h"


// --------- Utilidades ---------


typedef struct {
const uint8_t *data; // puntero a bytes (UTF-8 sin interpretar)
size_t start; // inicio del bloque
size_t end; // fin (exclusivo)
int *freq; // tabla local 256
} freq_task_t;


// para compresión por bloques
typedef struct {
const uint8_t *data;
size_t start;
size_t end;
char **codes; // tabla de códigos Huffman [256] -> string "0101..."
// salida
uint8_t *out_bits; // buffer de bits empaquetados (se llena al final)
size_t out_bytes; // tamaño usado
} encode_task_t;


// Contenedor de cabecera de archivo comprimido
// [MAGIC "HUF1" (4 bytes)]
// [tabla 256 de uint16: largos de código]
// [concatenación de códigos en bits (hasta 256 * largo)] -> aquí simplificamos guardando el árbol via recorrido preorden
// Para mantenerlo simple y compacto usaremos un preorden con banderas (nodo/hoja) + byte cuando hoja.


typedef struct {
uint32_t magic; // 'HUF1'
uint64_t original_size; // bytes del archivo original
uint32_t tree_bits; // cuántos bits ocupa la serialización del árbol
uint64_t data_bits; // cuántos bits ocupa la data comprimida
} huf_header_t; // Total 4 + 8 + 4 + 8 = 24 bytes


// Serialización del árbol (preorden): para cada nodo: bit 0 = interno, bit 1 = hoja y seguido un byte del símbolo
// Guardamos la secuencia de bits en un vector dinámico.


typedef struct {
uint8_t *buf; // bytes
size_t bits; // número de bits válidos
size_t cap; // capacidad en bytes
} bitbuf_t;


void bitbuf_init(bitbuf_t *b);
void bitbuf_push_bit(bitbuf_t *b, int bit);
void bitbuf_push_byte(bitbuf_t *b, uint8_t v);
void bitbuf_write_to_file(const bitbuf_t *b, FILE *fp);
#endif // PTHREADS_HUFF_H