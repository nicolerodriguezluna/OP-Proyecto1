#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#define TAM_MAX 256
#define LARGO_TEXTO_MAX 1000

// Nodo del árbol de Huffman
struct Nodo {
    unsigned char caracter;
    int frecuencia;
    struct Nodo *izquierda, *derecha;
};

// Estructura para retornar múltiples valores de forma limpia
struct ListaNodos {
    struct Nodo** nodos;
    int cantidad;
};

/**
 * Cuenta la frecuencia de cada carácter en una cadena de texto.
 */
void contarFrecuencias(const char* texto, int* frecuencias) {
    // Inicializar el arreglo de frecuencias a 0
    for (int i = 0; i < TAM_MAX; i++) {
        frecuencias[i] = 0;
    }

    // Recorrer el texto y contar la frecuencia de cada carácter
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
    
    // Se determinan los nodos necesarios
    lista.cantidad = contarCaracteresConFrecuencia(frecuencias);
    
    // Se asigna memoria para el arreglo de punteros
    lista.nodos = (struct Nodo**)malloc(lista.cantidad * sizeof(struct Nodo*));
    
    // Se crean los nodos
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

int main() {
    setlocale(LC_ALL, ""); // Permite manejar caracteres especiales y acentos

    char texto[LARGO_TEXTO_MAX];
    int frecuencias[TAM_MAX];

    printf("Por favor, introduce el texto que deseas analizar (máximo %d caracteres):\n", LARGO_TEXTO_MAX - 1);
    
    // Leer el texto del usuario, incluyendo espacios
    fgets(texto, LARGO_TEXTO_MAX, stdin);

    // Eliminar el salto de línea que `fgets` añade al final
    texto[strcspn(texto, "\n")] = 0;

    // Llamar a la función para contar las frecuencias
    contarFrecuencias(texto, frecuencias);

    // Crear lista de nodos a partir de las frecuencias
    struct ListaNodos listaNodos = crearListaNodos(frecuencias);
    
    // Imprimir los nodos creados para verificar
    printf("\nNodos creados (%d en total):\n", listaNodos.cantidad);
    for (int i = 0; i < listaNodos.cantidad; i++) {
        unsigned char caracter = listaNodos.nodos[i]->caracter;
        int frecuencia = listaNodos.nodos[i]->frecuencia;
        
        // Mostrar caracteres especiales con su nombre y código ASCII
        if (caracter == ' ') {
            printf("Carácter 'ESPACIO' (ASCII: %d) | Frecuencia: %d\n", caracter, frecuencia);
        } else if (caracter == '\t') {
            printf("Carácter 'TAB' (ASCII: %d) | Frecuencia: %d\n", caracter, frecuencia);
        } else if (caracter == '\n') {
            printf("Carácter 'NUEVA_LÍNEA' (ASCII: %d) | Frecuencia: %d\n", caracter, frecuencia);
        } else if (caracter == '\r') {
            printf("Carácter 'RETORNO_CARRO' (ASCII: %d) | Frecuencia: %d\n", caracter, frecuencia);
        } else if (caracter < 32 || caracter == 127) {
            // Caracteres de control no imprimibles
            printf("Carácter 'CTRL' (ASCII: %d) | Frecuencia: %d\n", caracter, frecuencia);
        } else if (caracter >= 128) {
            // Caracteres extendidos
            printf("Carácter 'EXT' (ASCII: %d) | Frecuencia: %d\n", caracter, frecuencia);
        } else {
            // Caracteres imprimibles normales
            printf("Carácter '%c' (ASCII: %d) | Frecuencia: %d\n", caracter, caracter, frecuencia);
        }
    }
    
    // Liberar memoria
    liberarListaNodos(&listaNodos);
    
    return 0;
}