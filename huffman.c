#include <stdio.h>
#include <string.h>
#include <locale.h>

#define TAM_MAX 256
#define LARGO_TEXTO_MAX 1000

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

    // Imprimir las frecuencias para verificar
    printf("\nFrecuencias de caracteres:\n");
    for (int i = 0; i < TAM_MAX; i++) {
        if (frecuencias[i] > 0) {
            printf("Carácter '%c' (ASCII: %d) | Frecuencia: %d\n", i, i, frecuencias[i]);
        }
    }

    return 0;
}