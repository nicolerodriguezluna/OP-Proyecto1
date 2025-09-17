#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/io_handler.h"
#include "../../huffman/include/huffman.h"
#include "../../huffman/include/frecuencias.h"
#include "../../huffman/include/arbol.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Uso: %s <directorio_entrada> <archivo_salida>\n", argv[0]);
        return 1;
    }

    printf("Comprimiendo directorio '%s' en archivo '%s'...\n", argv[1], argv[2]);
    
    if (comprimir_directorio(argv[1], argv[2])) {
        printf("Compresión completada exitosamente.\n");
        return 0;
    } else {
        printf("Error en la compresión.\n");
        return 1;
    }
}