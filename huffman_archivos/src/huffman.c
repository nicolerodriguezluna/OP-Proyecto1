#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include "../include/huffman.h"
#include "../include/frecuencias.h"
#include "../include/arbol.h"

int main() {
    setlocale(LC_ALL, "");

    char texto[LARGO_TEXTO_MAX];
    int frecuencias[TAM_MAX];

    printf("Por favor, introduce el texto que deseas analizar (máximo %d caracteres):\n", LARGO_TEXTO_MAX - 1);
    fgets(texto, LARGO_TEXTO_MAX, stdin);
    texto[strcspn(texto, "\n")] = 0;

    // 1. Contar frecuencias
    contarFrecuencias(texto, frecuencias);

    // 2. Crear lista de nodos
    struct ListaNodos listaNodos = crearListaNodos(frecuencias);

    // 3. Construir árbol de Huffman
    struct Nodo* raiz = construirArbolHuffman(listaNodos);
    
    // Imprimir el árbol
    printf("\nÁrbol de Huffman (estructura):\n");
    imprimirArbol(raiz, 0);

    // 4. Generar códigos de Huffman
    char* tablaCodigos[TAM_MAX] = {NULL};
    char codigoActual[TAM_MAX];
    generarCodigosHuffman(raiz, codigoActual, 0, tablaCodigos);
    
    // Mostrar tabla de códigos
    printf("\nTabla de códigos de Huffman:\n");
    for (int i = 0; i < TAM_MAX; i++) {
        if (tablaCodigos[i] != NULL) {
            unsigned char c = i;
            if (c == ' ') {
                printf("'ESPACIO': %s\n", tablaCodigos[i]);
            } else if (c == '\t') {
                printf("'TAB': %s\n", tablaCodigos[i]);
            } else if (c == '\n') {
                printf("'NUEVA_LÍNEA': %s\n", tablaCodigos[i]);
            } else if (c < 32 || c == 127) {
                printf("'CTRL' (ASCII %d): %s\n", c, tablaCodigos[i]);
            } else if (c >= 128) {
                printf("'EXT' (ASCII %d): %s\n", c, tablaCodigos[i]);
            } else {
                printf("'%c': %s\n", c, tablaCodigos[i]);
            }
        }
    }

    // 5. Comprimir y mostrar el resultado
    char* textoComprimido = comprimirTexto(texto, tablaCodigos);
    printf("\nTexto comprimido:\n%s\n", textoComprimido);
    
    // Calcular y mostrar tasa de compresión
    int longitudOriginal = strlen(texto);
    int longitudComprimida = strlen(textoComprimido);
    float tasaCompresion = (1 - (float)longitudComprimida / (longitudOriginal * 8)) * 100;
    printf("\nTasa de compresión: %.2f%%\n", tasaCompresion);
    printf("Longitud original: %d bits\n", longitudOriginal * 8);
    printf("Longitud comprimida: %d bits\n", longitudComprimida);

    // 6. Descomprimir y mostrar el resultado
    char* textoDescomprimido = descomprimirTexto(raiz, textoComprimido);
    printf("\nTexto descomprimido:\n%s\n", textoDescomprimido);
    
    // Verificar integridad
    if (strcmp(texto, textoDescomprimido) == 0) {
        printf("\n✓ La descompresión fue exitosa (texto idéntico al original).\n");
    } else {
        printf("\n✗ Error en la descompresión (texto diferente al original).\n");
    }

    // 7. Liberar memoria
    for (int i = 0; i < TAM_MAX; i++) {
        free(tablaCodigos[i]);
    }
    free(textoComprimido);
    free(textoDescomprimido);
    liberarArbol(raiz);
    liberarListaNodos(&listaNodos);

    return 0;
}