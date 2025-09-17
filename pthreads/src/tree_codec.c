#include "../include/pthreads_huff.h"


static void serialize_node(struct Nodo *n, bitbuf_t *bb) {
if (!n) return; // no debe ocurrir
if (!n->izquierda && !n->derecha) {
bitbuf_push_bit(bb, 1); // hoja
bitbuf_push_byte(bb, (uint8_t)n->caracter);
} else {
bitbuf_push_bit(bb, 0); // interno
serialize_node(n->izquierda, bb);
serialize_node(n->derecha, bb);
}
}


void serialize_tree(struct Nodo *root, bitbuf_t *bb) { serialize_node(root, bb); }


static struct Nodo* read_node(const uint8_t *bits, size_t nbits, size_t *p) {
if (*p >= nbits) return NULL;
int flag = (bits[*p >> 3] >> (7 - (*p & 7))) & 1; (*p)++;
if (flag == 1) { // hoja
uint8_t v = 0;
for (int i = 0; i < 8; ++i) {
if (*p >= nbits) break;
v = (v << 1) | ((bits[*p >> 3] >> (7 - (*p & 7))) & 1);
(*p)++;
}
return nuevo_nodo(v, 0);
} else {
struct Nodo *L = read_node(bits, nbits, p);
struct Nodo *R = read_node(bits, nbits, p);
struct Nodo *I = nuevo_nodo(0, 0);
I->izquierda = L; I->derecha = R;
return I;
}
}


struct Nodo* deserialize_tree_from_bits(const uint8_t *bits, size_t nbits, size_t *cursor_bits) {
*cursor_bits = 0; return read_node(bits, nbits, cursor_bits);
}