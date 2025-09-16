#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include "../include/huffman.h"
#include "../include/frecuencias.h"
#include "../include/arbol.h"

int main() {
    setlocale(LC_ALL, ""); // Permite manejar caracteres especiales y acentos

    char texto[LARGO_TEXTO_MAX];
    int frecuencias[TAM_MAX];

    printf("Por favor, introduce el texto que deseas analizar (máximo %d caracteres):\n", LARGO_TEXTO_MAX - 1);
    
    // Leer el texto del usuario, incluyendo espacios
    fgets(texto, LARGO_TEXTO_MAX, stdin);

    // Eliminar el salto de línea que `fgets` añade al final
    texto[strcspn(texto, "\n")] = 0;

    // Contar frecuencias
    contarFrecuencias(texto, frecuencias);

    // Crear lista de nodos
    struct ListaNodos listaNodos = crearListaNodos(frecuencias);
    
    // Construir árbol de Huffman
    struct Nodo* raiz = construirArbolHuffman(listaNodos);
    
    // Imprimir el árbol
    printf("\nÁrbol de Huffman (estructura):\n");
    imprimirArbol(raiz, 0);
    
    // Liberar memoria
    liberarArbol(raiz);
    
    return 0;
}