#include "../include/pthreads_huff.h"

static void *freq_worker(void *arg) {
freq_task_t *t = (freq_task_t*)arg;
// local table
for (size_t i = 0; i < 256; ++i) t->freq[i] = 0;
for (size_t i = t->start; i < t->end; ++i) {
t->freq[t->data[i]]++;
}
return NULL;
}


// Divide el buffer en T hilos y acumula a una tabla global
void parallel_count_freq(const uint8_t *data, size_t n, int *global_freq, int threads) {
if (threads < 1) threads = 1;
pthread_t th[64];
freq_task_t tasks[64];
if (threads > 64) threads = 64;
size_t chunk = (n + threads - 1) / threads;


// crear
for (int i = 0; i < threads; ++i) {
size_t s = i * chunk;
size_t e = s + chunk; if (e > n) e = n;
tasks[i].data = data;
tasks[i].start = s;
tasks[i].end = e;
tasks[i].freq = (int*)malloc(256 * sizeof(int));
pthread_create(&th[i], NULL, freq_worker, &tasks[i]);
}
// join + reduce
for (int i = 0; i < 256; ++i) global_freq[i] = 0;
for (int i = 0; i < threads; ++i) {
pthread_join(th[i], NULL);
for (int c = 0; c < 256; ++c) global_freq[c] += tasks[i].freq[c];
free(tasks[i].freq);
}
}