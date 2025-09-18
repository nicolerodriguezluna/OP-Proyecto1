#ifndef HUFFIO_H
#define HUFFIO_H

#include "common.h"

typedef struct __attribute__((packed)) {
    char magic[4];     // "HFA1"
    uint32_t nfiles;
} hfa_header_t;

typedef struct {
    char  *name;       // nombre base, ej: "pg84.txt"
    char  *txt;        // contenido original (solo mientras se comprime)
    size_t txt_len;
    struct Nodo *raiz; // árbol por archivo
    char  **tabla;     // tabla de códigos
    char  *bitstr;     // '0'/'1' string (de tu comprimir_texto)
    uint64_t bit_count;
    uint8_t *packed;   // bits empaquetados
    size_t packed_len; // bytes
} hfa_entry_t;

typedef struct {
    char     *name;
    uint64_t  orig_len;
    uint64_t  bit_count;
    uint64_t  byte_count;
    long      payload_off;    // desplazamiento dentro del .hfa
    uint8_t  *tree_blob;      // árbol serializado (incluye el 255 final)
    size_t    tree_len;
} hfa_meta_t;

void     pack_bits_from_bitstr(const char *bitstr, uint8_t **out, size_t *out_len, uint64_t *out_bit_count);
char*    unpack_bits_to_bitstr(const uint8_t *buf, size_t len, uint64_t bit_count);

int hfa_write(const char *archive_path, hfa_entry_t *entries, uint32_t nfiles);
int hfa_read_and_extract(const char *archive_path, const char *dir);

int  hfa_index(const char *archive_path, hfa_meta_t **out_meta, uint32_t *out_nfiles);
void hfa_free_index(hfa_meta_t *meta, uint32_t nfiles);

#endif
