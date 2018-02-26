#pragma once

#include <stddef.h>
#include <stdint.h>
#include <assert.h>

enum {
  HASH_SIZE = 32
};

#pragma pack(push, 1)
union hash_state {
  uint8_t b[200];
  uint64_t w[25];
};
#pragma pack(pop)
static_assert(sizeof(union hash_state) == 200, "Invalid structure size");

void hash_permutation(union hash_state *state);
void hash_process(union hash_state *state, const uint8_t *buf, size_t count);

void hash_extra_blake(const void *data, size_t length, char *hash);
void hash_extra_groestl(const void *data, size_t length, char *hash);
void hash_extra_jh(const void *data, size_t length, char *hash);
void hash_extra_skein(const void *data, size_t length, char *hash);
