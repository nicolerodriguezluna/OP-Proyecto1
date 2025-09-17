#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "../include/io_handler.h"
#include "../include/arbol.h"
#include "../include/frecuencias.h"

// Función para comprimir un solo archivo y agregarlo al archivo de salida
int comprimir_archivo(const char* nombre_archivo, struct Nodo* raiz, char** tabla_codigos, FILE* archivo_salida) {
    FILE* archivo_entrada = fopen(nombre_archivo, "r");
    if (!archivo_entrada) {
        perror("Error al abrir archivo de entrada");
        return 0;
    }

    // Obtener el tamaño del archivo
    fseek(archivo_entrada, 0, SEEK_END);
    long tamaño_archivo = ftell(archivo_entrada);
    fseek(archivo_entrada, 0, SEEK_SET);

    // Leer el contenido del archivo
    char* contenido = (char*)malloc(tamaño_archivo + 1);
    if (!contenido) {
        fclose(archivo_entrada);
        return 0;
    }

    fread(contenido, 1, tamaño_archivo, archivo_entrada);
    contenido[tamaño_archivo] = '\0';
    fclose(archivo_entrada);

    // Comprimir el contenido
    char* texto_comprimido = comprimir_texto(contenido, tabla_codigos);
    free(contenido);

    if (!texto_comprimido) {
        return 0;
    }

    // Escribir metadatos del archivo
    int longitud_nombre = strlen(nombre_archivo);
    fwrite(&longitud_nombre, sizeof(int), 1, archivo_salida);
    fwrite(nombre_archivo, 1, longitud_nombre, archivo_salida);
    fwrite(&tamaño_archivo, sizeof(long), 1, archivo_salida);

    // Escribir contenido comprimido
    int longitud_comprimida = strlen(texto_comprimido);
    fwrite(&longitud_comprimida, sizeof(int), 1, archivo_salida);
    fwrite(texto_comprimido, 1, longitud_comprimida, archivo_salida);

    free(texto_comprimido);
    return 1;
}

// Función para comprimir todos los archivos de texto en un directorio
int comprimir_directorio(const char* directorio_entrada, const char* archivo_salida) {
    DIR* dir = opendir(directorio_entrada);
    if (!dir) {
        perror("Error al abrir directorio");
        return 0;
    }

    FILE* archivo_comprimido = fopen(archivo_salida, "wb");
    if (!archivo_comprimido) {
        closedir(dir);
        return 0;
    }

    // Primero, leer todos los archivos y calcular frecuencias globales
    int frecuencias[TAM_MAX] = {0};
    struct dirent* entrada;
    char ruta_completa[1024];

    while ((entrada = readdir(dir)) != NULL) {
        if (entrada->d_type == DT_REG) {
            snprintf(ruta_completa, sizeof(ruta_completa), "%s/%s", directorio_entrada, entrada->d_name);
            
            FILE* archivo = fopen(ruta_completa, "r");
            if (archivo) {
                // Calcular frecuencias de este archivo
                int frecuencias_archivo[TAM_MAX] = {0};
                char buffer[1024];
                while (fgets(buffer, sizeof(buffer), archivo)) {
                    for (int i = 0; buffer[i] != '\0'; i++) {
                        frecuencias[(unsigned char)buffer[i]]++;
                    }
                }
                fclose(archivo);
                
                // Sumar al total global
                for (int i = 0; i < TAM_MAX; i++) {
                    frecuencias[i] += frecuencias_archivo[i];
                }
            }
        }
    }
    closedir(dir);

    // Construir árbol de Huffman global
    struct ListaNodos lista_nodos = crear_lista_nodos(frecuencias);
    struct Nodo* raiz = construir_arbol_huffman(lista_nodos);
    
    // Generar tabla de códigos
    char* tabla_codigos[TAM_MAX] = {NULL};
    char codigo_actual[TAM_MAX];
    generar_codigos_huffman(raiz, codigo_actual, 0, tabla_codigos);

    // Serializar y guardar el árbol en el archivo comprimido
    serializar_arbol(raiz, archivo_comprimido);

    // Volver a abrir el directorio para procesar cada archivo
    dir = opendir(directorio_entrada);
    if (!dir) {
        fclose(archivo_comprimido);
        liberar_arbol(raiz);
        for (int i = 0; i < TAM_MAX; i++) free(tabla_codigos[i]);
        return 0;
    }

    // Escribir número de archivos
    int contador_archivos = 0;
    while ((entrada = readdir(dir)) != NULL) {
        if (entrada->d_type == DT_REG) contador_archivos++;
    }
    rewinddir(dir);

    fwrite(&contador_archivos, sizeof(int), 1, archivo_comprimido);

    // Comprimir cada archivo
    while ((entrada = readdir(dir)) != NULL) {
        if (entrada->d_type == DT_REG) {
            snprintf(ruta_completa, sizeof(ruta_completa), "%s/%s", directorio_entrada, entrada->d_name);
            if (!comprimir_archivo(ruta_completa, raiz, tabla_codigos, archivo_comprimido)) {
                printf("Error al comprimir archivo: %s\n", entrada->d_name);
            }
        }
    }

    // Liberar recursos
    closedir(dir);
    fclose(archivo_comprimido);
    liberar_arbol(raiz);
    for (int i = 0; i < TAM_MAX; i++) {
        if (tabla_codigos[i]) free(tabla_codigos[i]);
    }

    return 1;
}

// Función para descomprimir un archivo comprimido
int descomprimir_archivo_completo(const char* archivo_entrada, const char* directorio_salida) {
    FILE* archivo_comprimido = fopen(archivo_entrada, "rb");
    if (!archivo_comprimido) {
        perror("Error al abrir archivo comprimido");
        return 0;
    }

    // Crear directorio de salida si no existe
    struct stat st = {0};
    if (stat(directorio_salida, &st) == -1) {
        mkdir(directorio_salida, 0700);
    }

    // Deserializar el árbol de Huffman
    struct Nodo* raiz = deserializar_arbol(archivo_comprimido);
    if (!raiz) {
        fclose(archivo_comprimido);
        return 0;
    }

    // Leer número de archivos
    int numero_archivos;
    fread(&numero_archivos, sizeof(int), 1, archivo_comprimido);

    // Procesar cada archivo
    for (int i = 0; i < numero_archivos; i++) {
        // Leer metadatos del archivo
        int longitud_nombre;
        fread(&longitud_nombre, sizeof(int), 1, archivo_comprimido);
        
        char nombre_archivo[256];
        fread(nombre_archivo, 1, longitud_nombre, archivo_comprimido);
        nombre_archivo[longitud_nombre] = '\0';
        
        long tamaño_original;
        fread(&tamaño_original, sizeof(long), 1, archivo_comprimido);
        
        int longitud_comprimida;
        fread(&longitud_comprimida, sizeof(int), 1, archivo_comprimido);
        
        // Leer contenido comprimido
        char* texto_comprimido = (char*)malloc(longitud_comprimida + 1);
        fread(texto_comprimido, 1, longitud_comprimida, archivo_comprimido);
        texto_comprimido[longitud_comprimida] = '\0';
        
        // Descomprimir contenido
        char* texto_original = descomprimir_texto(raiz, texto_comprimido);
        free(texto_comprimido);
        
        // Crear ruta completa de salida
        char ruta_salida[1024];
        snprintf(ruta_salida, sizeof(ruta_salida), "%s/%s", directorio_salida, nombre_archivo);
        
        // Escribir archivo descomprimido
        FILE* archivo_salida = fopen(ruta_salida, "w");
        if (archivo_salida) {
            fwrite(texto_original, 1, strlen(texto_original), archivo_salida);
            fclose(archivo_salida);
        }
        
        free(texto_original);
    }

    liberar_arbol(raiz);
    fclose(archivo_comprimido);
    return 1;
}