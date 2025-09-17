#include <sys/stat.h>
#include <time.h>
#include "../include/pthreads_huff.h"


static char** build_codes(struct Nodo* raiz) {
}


int main(int argc, char **argv) {
if (argc < 3) {
fprintf(stderr, "Uso: %s <input.txt> <salida.huf> [hilos]\n", argv[0]);
return 1;
}
int threads = (argc >= 4) ? atoi(argv[3]) : 4;


size_t n; uint8_t *data = load_file(argv[1], &n);
if (!data) return 2;


// 1) Frecuencias en paralelo
int freq[256];
long t0 = clock();
parallel_count_freq(data, n, freq, threads);


// 2) Construir árbol y tabla de códigos (serial, poco costoso)
struct ListaNodos ln = crear_lista_nodos(freq);
struct Nodo *root = construir_arbol_huffman(ln);
char **codes = build_codes(root);


// 3) Codificar en paralelo
bitbuf_t bb_codes = parallel_encode(data, n, codes, threads);
long t1 = clock();


// 4) Serializar árbol
bitbuf_t bb_tree; bitbuf_init(&bb_tree);
serialize_tree(root, &bb_tree);


// 5) Escribir archivo: cabecera + árbol + datos
FILE *out = fopen(argv[2], "wb");
if (!out) { perror("fopen out"); return 3; }
huf_header_t H; H.magic = 0x48554631; // 'HUF1'
H.original_size = n; H.tree_bits = (uint32_t)bb_tree.bits; H.data_bits = (uint64_t)bb_codes.bits;
fwrite(&H, sizeof(H), 1, out);
bitbuf_write_to_file(&bb_tree, out);
bitbuf_write_to_file(&bb_codes, out);
fclose(out);


double ms = 1000.0 * (t1 - t0) / CLOCKS_PER_SEC;
fprintf(stderr, "[pthread] Comprimido %zu bytes -> %.0f bytes en ~%.1f ms (solo conteo+encode)\n",
n, (double)((bb_tree.bits + bb_codes.bits + 7)/8 + sizeof(H)), ms);


// liberar
for (int i = 0; i < 256; ++i) free(codes[i]);
free(codes); free(data); bitbuf_free(&bb_tree); bitbuf_free(&bb_codes); liberar_arbol(root);
return 0;
}