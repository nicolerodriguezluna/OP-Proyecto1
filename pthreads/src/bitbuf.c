#include "../include/pthreads_huff.h"


void bitbuf_init(bitbuf_t *b) { b->buf = NULL; b->bits = 0; b->cap = 0; }


static void ensure_cap(bitbuf_t *b, size_t need_bits) {
size_t need_bytes = (need_bits + 7) / 8;
if (need_bytes > b->cap) {
size_t ncap = b->cap ? b->cap : 1024;
while (ncap < need_bytes) ncap *= 2;
b->buf = (uint8_t*)realloc(b->buf, ncap);
memset(b->buf + b->cap, 0, ncap - b->cap);
b->cap = ncap;
}
}


void bitbuf_push_bit(bitbuf_t *b, int bit) {
ensure_cap(b, b->bits + 1);
if (bit) b->buf[b->bits >> 3] |= 1 << (7 - (b->bits & 7));
b->bits++;
}


void bitbuf_push_byte(bitbuf_t *b, uint8_t v) {
for (int i = 0; i < 8; ++i) bitbuf_push_bit(b, (v >> (7 - i)) & 1);
}


void bitbuf_write_to_file(const bitbuf_t *b, FILE *fp) {
fwrite(b->buf, 1, (b->bits + 7) / 8, fp);
}


void bitbuf_free(bitbuf_t *b) { free(b->buf); b->buf = NULL; b->bits = b->cap = 0; }