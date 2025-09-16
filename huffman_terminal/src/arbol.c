#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/arbol.h"
#include "../include/frecuencias.h"

/**
 * Encuentra los dos nodos con las frecuencias más bajas en una lista.
 */
void encontrarDosMinimos(struct ListaNodos lista, int* min1, int* min2) {
    *min1 = 0;
    *min2 = 1;
    if (lista.nodos[0]->frecuencia > lista.nodos[1]->frecuencia) {
        *min1 = 1;
        *min2 = 0;
    }
    for (int i = 2; i < lista.cantidad; i++) {
        if (lista.nodos[i]->frecuencia < lista.nodos[*min1]->frecuencia) {
            *min2 = *min1;
            *min1 = i;
        } else if (lista.nodos[i]->frecuencia < lista.nodos[*min2]->frecuencia) {
            *min2 = i;
        }
    }
}

/**
 * Construye el árbol de Huffman a partir de una lista de nodos.
 */
struct Nodo* construirArbolHuffman(struct ListaNodos listaInicial) {
    struct ListaNodos listaActual = listaInicial;
    while (listaActual.cantidad > 1) {
        int min1, min2;
        encontrarDosMinimos(listaActual, &min1, &min2);
        
        struct Nodo* nuevo = nuevoNodo(0, 
            listaActual.nodos[min1]->frecuencia + listaActual.nodos[min2]->frecuencia);
        nuevo->izquierda = listaActual.nodos[min1];
        nuevo->derecha = listaActual.nodos[min2];
        
        // Crear nueva lista sin los dos mínimos y con el nuevo nodo
        struct ListaNodos nuevaLista;
        nuevaLista.cantidad = listaActual.cantidad - 1;
        nuevaLista.nodos = (struct Nodo**)malloc(nuevaLista.cantidad * sizeof(struct Nodo*));
        
        int j = 0;
        for (int i = 0; i < listaActual.cantidad; i++) {
            if (i != min1 && i != min2) {
                nuevaLista.nodos[j] = listaActual.nodos[i];
                j++;
            }
        }
        nuevaLista.nodos[j] = nuevo;
        
        // Liberar solo el arreglo de la lista actual (los nodos se conservan en el árbol)
        free(listaActual.nodos);
        listaActual = nuevaLista;
    }
    
    struct Nodo* raiz = listaActual.nodos[0];
    free(listaActual.nodos); // Liberar el arreglo final
    return raiz;
}

/**
 * Imprime el árbol de Huffman con indentación para mostrar la estructura.
 */
void imprimirArbol(struct Nodo* raiz, int nivel) {
    if (raiz == NULL) return;
    
    for (int i = 0; i < nivel; i++) {
        printf("  ");
    }
    
    if (raiz->caracter == 0) {
        printf("Nodo interno: %d\n", raiz->frecuencia);
    } else {
        // Mostrar caracteres especiales
        unsigned char c = raiz->caracter;
        if (c == ' ') {
            printf("Hoja: 'ESPACIO' (ASCII: %d) - Frecuencia: %d\n", c, raiz->frecuencia);
        } else if (c == '\t') {
            printf("Hoja: 'TAB' (ASCII: %d) - Frecuencia: %d\n", c, raiz->frecuencia);
        } else if (c == '\n') {
            printf("Hoja: 'NUEVA_LÍNEA' (ASCII: %d) - Frecuencia: %d\n", c, raiz->frecuencia);
        } else if (c == '\r') {
            printf("Hoja: 'RETORNO_CARRO' (ASCII: %d) - Frecuencia: %d\n", c, raiz->frecuencia);
        } else if (c < 32 || c == 127) {
            printf("Hoja: 'CTRL' (ASCII: %d) - Frecuencia: %d\n", c, raiz->frecuencia);
        } else if (c >= 128) {
            printf("Hoja: 'EXT' (ASCII: %d) - Frecuencia: %d\n", c, raiz->frecuencia);
        } else {
            printf("Hoja: '%c' (ASCII: %d) - Frecuencia: %d\n", c, c, raiz->frecuencia);
        }
    }
    
    imprimirArbol(raiz->izquierda, nivel + 1);
    imprimirArbol(raiz->derecha, nivel + 1);
}

/**
 * Libera la memoria del árbol de Huffman.
 */
void liberarArbol(struct Nodo* nodo) {
    if (nodo == NULL) return;
    liberarArbol(nodo->izquierda);
    liberarArbol(nodo->derecha);
    free(nodo);
}

/**
 * Genera los códigos de Huffman para cada carácter recursivamente.
 */
void generarCodigosHuffman(struct Nodo* raiz, char* codigo, int profundidad, char** tabla) {
    if (raiz == NULL) return;
    
    // Si es un nodo hoja, guardar el código
    if (raiz->izquierda == NULL && raiz->derecha == NULL) {
        codigo[profundidad] = '\0';
        tabla[raiz->caracter] = strdup(codigo);
        return;
    }
    
    // Recorrer izquierda (agregar '0')
    codigo[profundidad] = '0';
    generarCodigosHuffman(raiz->izquierda, codigo, profundidad + 1, tabla);
    
    // Recorrer derecha (agregar '1')
    codigo[profundidad] = '1';
    generarCodigosHuffman(raiz->derecha, codigo, profundidad + 1, tabla);
}

/**
 * Comprime texto usando la tabla de códigos de Huffman.
 */
char* comprimirTexto(const char* texto, char** tabla) {
    int longitudOriginal = strlen(texto);
    int longitudComprimida = 0;
    
    // Calcular longitud del texto comprimido
    for (int i = 0; i < longitudOriginal; i++) {
        unsigned char c = texto[i];
        if (tabla[c] != NULL) {
            longitudComprimida += strlen(tabla[c]);
        }
    }
    
    // Asignar memoria para el texto comprimido
    char* textoComprimido = (char*)malloc(longitudComprimida + 1);
    textoComprimido[0] = '\0';
    
    // Construir texto comprimido
    for (int i = 0; i < longitudOriginal; i++) {
        unsigned char c = texto[i];
        if (tabla[c] != NULL) {
            strcat(textoComprimido, tabla[c]);
        }
    }
    
    return textoComprimido;
}

/**
 * Descomprime texto usando el árbol de Huffman.
 */
char* descomprimirTexto(struct Nodo* raiz, const char* textoComprimido) {
    int longitudComprimida = strlen(textoComprimido);
    char* textoOriginal = (char*)malloc(LARGO_TEXTO_MAX);
    int indiceOriginal = 0;
    
    struct Nodo* actual = raiz;
    
    for (int i = 0; i < longitudComprimida; i++) {
        if (textoComprimido[i] == '0') {
            actual = actual->izquierda;
        } else if (textoComprimido[i] == '1') {
            actual = actual->derecha;
        }
        
        // Si es un nodo hoja, agregar carácter al texto original
        if (actual->izquierda == NULL && actual->derecha == NULL) {
            textoOriginal[indiceOriginal++] = actual->caracter;
            actual = raiz; // Volver a la raíz para el próximo carácter
        }
    }
    
    textoOriginal[indiceOriginal] = '\0';
    return textoOriginal;
}