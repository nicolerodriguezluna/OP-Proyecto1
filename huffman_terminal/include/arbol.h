#ifndef ARBOL_H
#define ARBOL_H

#include "huffman.h"

void encontrarDosMinimos(struct ListaNodos lista, int* min1, int* min2);
struct Nodo* construirArbolHuffman(struct ListaNodos listaInicial);
void imprimirArbol(struct Nodo* raiz, int nivel);
void liberarArbol(struct Nodo* nodo);
void generarCodigosHuffman(struct Nodo* raiz, char* codigo, int profundidad, char** tabla);
char* comprimirTexto(const char* texto, char** tabla);
char* descomprimirTexto(struct Nodo* raiz, const char* textoComprimido);

#endif