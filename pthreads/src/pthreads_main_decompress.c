#include "../include/pthreads_huff.h"


static uint8_t* read_all(FILE *fp, size_t *n) {
fseek(fp, 0, SEEK_END); long sz = ftell(fp); fseek(fp, 0, SEEK_SET);
uint8_t *buf = (uint8_t*)malloc(sz);
fread(buf, 1, sz, fp); *n = (size_t)sz; return buf;
}


int main(int argc, char **argv) {
if (argc < 3) { fprintf(stderr, "Uso: %s <in.huf> <out.txt>\n", argv[0]); return 1; }
FILE *fp = fopen(argv[1], "rb"); if (!fp) { perror("in"); return 2; }


huf_header_t H; if (fread(&H, sizeof(H), 1, fp) != 1) { fprintf(stderr, "cabecera inválida\n"); return 3; }
if (H.magic != 0x48554631) { fprintf(stderr, "formato no soportado\n"); return 4; }


// leer resto a memoria
size_t remaining_sz; uint8_t *buf = read_all(fp, &remaining_sz); fclose(fp);


// reconstruir árbol desde los primeros H.tree_bits
size_t tree_bytes = (H.tree_bits + 7) / 8;
size_t cursor_bits = 0;
struct Nodo *root = deserialize_tree_from_bits(buf, H.tree_bits, &cursor_bits);


// puntero al stream de datos comprimidos
const uint8_t *bits = buf + tree_bytes; size_t nbits = H.data_bits;


// decodificar serialmente (paralelizar requiere dividir por límites de símbolo, omitido por simplicidad)
uint8_t *out = (uint8_t*)malloc(H.original_size);
size_t out_pos = 0; size_t bitpos = 0;
while (out_pos < H.original_size && bitpos < nbits) {
struct Nodo *cur = root;
while (cur->izquierda || cur->derecha) {
int bit = (bits[bitpos >> 3] >> (7 - (bitpos & 7))) & 1; bitpos++;
cur = bit ? cur->derecha : cur->izquierda;
}
out[out_pos++] = (uint8_t)cur->caracter;
}


FILE *outf = fopen(argv[2], "wb"); fwrite(out, 1, out_pos, outf); fclose(outf);
free(out); free(buf); liberar_arbol(root);
fprintf(stderr, "[pthread] Descomprimido %zu bytes.\n", out_pos);
return 0;
}