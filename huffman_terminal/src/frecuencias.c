#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/huffman.h"
#include "../include/frecuencias.h"

/**
 * Cuenta la frecuencia de cada carácter en una cadena de texto.
 */
void contarFrecuencias(const char* texto, int* frecuencias) {
    for (int i = 0; i < TAM_MAX; i++) {
        frecuencias[i] = 0;
    }
    for (int i = 0; i < strlen(texto); i++) {
        frecuencias[(unsigned char)texto[i]]++;
    }
}

/**
 * Crea un nuevo nodo.
 */
struct Nodo* nuevoNodo(unsigned char caracter, int frecuencia) {
    struct Nodo* nodo = (struct Nodo*)malloc(sizeof(struct Nodo));
    nodo->caracter = caracter;
    nodo->frecuencia = frecuencia;
    nodo->izquierda = nodo->derecha = NULL;
    return nodo;
}

/**
 * Cuenta cuántos caracteres tienen frecuencia mayor a 0.
 */
int contarCaracteresConFrecuencia(int* frecuencias) {
    int count = 0;
    for (int i = 0; i < TAM_MAX; i++) {
        if (frecuencias[i] > 0) {
            count++;
        }
    }
    return count;
}

/**
 * Crea una lista de nodos a partir de las frecuencias.
 */
struct ListaNodos crearListaNodos(int* frecuencias) {
    struct ListaNodos lista;
    lista.cantidad = contarCaracteresConFrecuencia(frecuencias);
    lista.nodos = (struct Nodo**)malloc(lista.cantidad * sizeof(struct Nodo*));
    int indice = 0;
    for (int i = 0; i < TAM_MAX; i++) {
        if (frecuencias[i] > 0) {
            lista.nodos[indice] = nuevoNodo((unsigned char)i, frecuencias[i]);
            indice++;
        }
    }
    return lista;
}

/**
 * Libera la memoria de una lista de nodos.
 */
void liberarListaNodos(struct ListaNodos* lista) {
    for (int i = 0; i < lista->cantidad; i++) {
        free(lista->nodos[i]);
    }
    free(lista->nodos);
    lista->nodos = NULL;
    lista->cantidad = 0;
}