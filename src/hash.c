#include <assert.h>
#include "hash.h"

#include "skein.h"
#include "jh.h"
#include "groestl.h"
#include "blake256.h"
#include "keccak.h"

void hash_extra_skein(const void *data, size_t length, char *hash) {
  int r = skein_hash(8 * HASH_SIZE, data, 8 * length, (uint8_t*)hash);
  assert(SKEIN_SUCCESS == r);
}

void hash_extra_jh(const void *data, size_t length, char *hash) {
  int r = jh_hash(HASH_SIZE * 8, data, 8 * length, (uint8_t*)hash);
  assert(SUCCESS == r);
}

void hash_extra_groestl(const void *data, size_t length, char *hash) {
  groestl(data, length * 8, (uint8_t*)hash);
}

void hash_extra_blake(const void *data, size_t length, char *hash) {
  blake256_hash((uint8_t*)hash, data, length);
}

void hash_permutation(union hash_state *state) {
  keccakf((uint64_t*)state, 24);
}

void hash_process(union hash_state *state, const uint8_t *buf, size_t count) {
  keccak1600(buf, count, (uint8_t*)state);
}