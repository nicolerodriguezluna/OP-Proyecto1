#ifndef FRECUENCIAS_H
#define FRECUENCIAS_H

void contarFrecuencias(const char* texto, int* frecuencias);
struct Nodo* nuevoNodo(unsigned char caracter, int frecuencia);
int contarCaracteresConFrecuencia(int* frecuencias);
struct ListaNodos crearListaNodos(int* frecuencias);
void liberarListaNodos(struct ListaNodos* lista);

#endif