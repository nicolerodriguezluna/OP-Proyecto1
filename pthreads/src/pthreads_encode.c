#include "../include/pthreads_huff.h"


static void bitset(uint8_t *dst, size_t bit_index, int bit) {
dst[bit_index >> 3] |= (bit & 1) << (7 - (bit_index & 7));
}


static void *encode_worker(void *arg) {
encode_task_t *t = (encode_task_t*)arg;


// Estima tamaño máximo (pésimo caso: 32 bits por byte). Para evitar realocar, usa 4x.
size_t max_bits = (t->end - t->start) * 32;
size_t out_bytes_cap = (max_bits + 7) / 8;
t->out_bits = (uint8_t*)calloc(out_bytes_cap, 1);


size_t bitpos = 0;
for (size_t i = t->start; i < t->end; ++i) {
const char *code = t->codes[t->data[i]];
for (const char *p = code; *p; ++p) {
bitset(t->out_bits, bitpos++, (*p == '1'));
}
}
t->out_bytes = (bitpos + 7) / 8;
return NULL;
}


// Codifica en paralelo, devuelve un bitbuf con concatenación en orden
bitbuf_t parallel_encode(const uint8_t *data, size_t n, char **codes, int threads) {
if (threads < 1) threads = 1;
pthread_t th[64];
encode_task_t tasks[64];
if (threads > 64) threads = 64;
size_t chunk = (n + threads - 1) / threads;


for (int i = 0; i < threads; ++i) {
size_t s = i * chunk;
size_t e = s + chunk; if (e > n) e = n;
tasks[i].data = data; tasks[i].start = s; tasks[i].end = e;
tasks[i].codes = codes; tasks[i].out_bits = NULL; tasks[i].out_bytes = 0;
pthread_create(&th[i], NULL, encode_worker, &tasks[i]);
}
for (int i = 0; i < threads; ++i) pthread_join(th[i], NULL);


bitbuf_t out; bitbuf_init(&out);
// concatenar salidas de cada hilo
for (int i = 0; i < threads; ++i) {
// copiar bytes crudos; ajustar bits finales no es necesario porque cada tarea ya empaquetó
for (size_t b = 0; b < tasks[i].out_bytes; ++b) {
// Empuja byte con 8 bits
for (int k = 0; k < 8; ++k) {
int bit = (tasks[i].out_bits[b] >> (7 - k)) & 1;
bitbuf_push_bit(&out, bit);
}
}
free(tasks[i].out_bits);
}
return out;
}