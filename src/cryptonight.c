/**
 * Generic implementation
*/

#include "cryptonight.h"
#include "oaes_lib.h"
#include "hash.h"
#include "aesb.c"
#include "int-util.h"

// Extra hash callback lookup
static void (*const extra_hashes[4])(const void *, size_t, char *) = {
  hash_extra_blake, hash_extra_groestl, hash_extra_jh, hash_extra_skein
};

static size_t e2i(const uint8_t* a, size_t count) { return (*((uint64_t*)a) / AES_BLOCK_SIZE) & (count - 1); }

static void mul(const uint8_t* a, const uint8_t* b, uint8_t* res) {
  uint64_t a0, b0;
  uint64_t hi, lo;

  a0 = SWAP64LE(((uint64_t*)a)[0]);
  b0 = SWAP64LE(((uint64_t*)b)[0]);
  lo = mul128(a0, b0, &hi);
  ((uint64_t*)res)[0] = SWAP64LE(hi);
  ((uint64_t*)res)[1] = SWAP64LE(lo);
}

static void sum_half_blocks(uint8_t* a, const uint8_t* b) {
  uint64_t a0, a1, b0, b1;

  a0 = SWAP64LE(((uint64_t*)a)[0]);
  a1 = SWAP64LE(((uint64_t*)a)[1]);
  b0 = SWAP64LE(((uint64_t*)b)[0]);
  b1 = SWAP64LE(((uint64_t*)b)[1]);
  a0 += b0;
  a1 += b1;
  ((uint64_t*)a)[0] = SWAP64LE(a0);
  ((uint64_t*)a)[1] = SWAP64LE(a1);
}

static void copy_block(uint8_t* dst, const uint8_t* src) {
  memcpy(dst, src, AES_BLOCK_SIZE);
}

static void swap_blocks(uint8_t *a, uint8_t *b){
  uint64_t t[2];
  U64(t)[0] = U64(a)[0];
  U64(t)[1] = U64(a)[1];
  U64(a)[0] = U64(b)[0];
  U64(a)[1] = U64(b)[1];
  U64(b)[0] = U64(t)[0];
  U64(b)[1] = U64(t)[1];
}

static void xor_blocks(uint8_t* a, const uint8_t* b) {
  size_t i;
  for (i = 0; i < AES_BLOCK_SIZE; i++) {
    a[i] ^= b[i];
  }
}

static void xor64(uint8_t* left, const uint8_t* right)
{
  size_t i;
  for (i = 0; i < 8; ++i)
  {
    left[i] ^= right[i];
  }
}

#define VARIANT1_1(p) \
  do if (variant > 0) \
  { \
    const uint8_t tmp = ((const uint8_t*)(p))[11]; \
    static const uint32_t table = 0x75310; \
    const uint8_t index = (((tmp >> 3) & 6) | (tmp & 1)) << 1; \
    ((uint8_t*)(p))[11] = tmp ^ ((table >> index) & 0x30); \
  } while(0)

#define VARIANT1_2(p) \
  do if (variant > 0) \
  { \
    xor64(p, tweak1_2); \
  } while(0)

#define NONCE_POINTER (((const uint8_t*)data)+35)

#define VARIANT1_PORTABLE_INIT() \
  uint8_t tweak1_2[8]; \
  do if (variant > 0) \
  { \
    memcpy(&tweak1_2, &state.hs.b[192], sizeof(tweak1_2)); \
    xor64(tweak1_2, NONCE_POINTER); \
  } while(0)



void cn_slow_hash(const void *data, size_t length, char *hash, uint8_t *long_state, int variant) {
  union cn_slow_hash_state state;
  uint8_t text[INIT_SIZE_BYTE];
  uint8_t a[AES_BLOCK_SIZE];
  uint8_t b[AES_BLOCK_SIZE];
  uint8_t c[AES_BLOCK_SIZE];
  uint8_t d[AES_BLOCK_SIZE];
  size_t i, j;
  uint8_t aes_key[AES_KEY_SIZE];
  oaes_ctx *aes_ctx;

  hash_process(&state.hs, data, length);
  memcpy(text, state.init, INIT_SIZE_BYTE);
  memcpy(aes_key, state.hs.b, AES_KEY_SIZE);
  aes_ctx = (oaes_ctx *) oaes_alloc();

  VARIANT1_PORTABLE_INIT();

  oaes_key_import_data(aes_ctx, aes_key, AES_KEY_SIZE);
  for (i = 0; i < MEMORY / INIT_SIZE_BYTE; i++) {
    for (j = 0; j < INIT_SIZE_BLK; j++) {
      aesb_pseudo_round(&text[AES_BLOCK_SIZE * j], &text[AES_BLOCK_SIZE * j], aes_ctx->key->exp_data);
    }
    memcpy(&long_state[i * INIT_SIZE_BYTE], text, INIT_SIZE_BYTE);
  }

  for (i = 0; i < 16; i++) {
    a[i] = state.k[     i] ^ state.k[32 + i];
    b[i] = state.k[16 + i] ^ state.k[48 + i];
  }

  for (i = 0; i < ITER / 2; i++) {
    /* Dependency chain: address -> read value ------+
     * written value <-+ hard function (AES or MUL) <+
     * next address  <-+
     */
    /* Iteration 1 */
    j = e2i(a, MEMORY / AES_BLOCK_SIZE);
    copy_block(c, &long_state[j * AES_BLOCK_SIZE]);
    aesb_single_round(c, c, a);
    xor_blocks(b, c);
    swap_blocks(b, c);
    copy_block(&long_state[j * AES_BLOCK_SIZE], c);
    assert(j == e2i(a, MEMORY / AES_BLOCK_SIZE));
    swap_blocks(a, b);
    VARIANT1_1(&long_state[j * AES_BLOCK_SIZE]);
    /* Iteration 2 */
    j = e2i(a, MEMORY / AES_BLOCK_SIZE);
    copy_block(c, &long_state[j * AES_BLOCK_SIZE]);
    mul(a, c, d);
    sum_half_blocks(b, d);
    swap_blocks(b, c);
    xor_blocks(b, c);
    VARIANT1_2(c + 8);
    copy_block(&long_state[j * AES_BLOCK_SIZE], c);
    assert(j == e2i(a, MEMORY / AES_BLOCK_SIZE));
    swap_blocks(a, b);
  }

  memcpy(text, state.init, INIT_SIZE_BYTE);
  oaes_key_import_data(aes_ctx, &state.hs.b[32], AES_KEY_SIZE);
  for (i = 0; i < MEMORY / INIT_SIZE_BYTE; i++) {
    for (j = 0; j < INIT_SIZE_BLK; j++) {
      xor_blocks(&text[j * AES_BLOCK_SIZE], &long_state[i * INIT_SIZE_BYTE + j * AES_BLOCK_SIZE]);
      aesb_pseudo_round(&text[AES_BLOCK_SIZE * j], &text[AES_BLOCK_SIZE * j], aes_ctx->key->exp_data);
    }
  }
  memcpy(state.init, text, INIT_SIZE_BYTE);
  hash_permutation(&state.hs);
  /*memcpy(hash, &state, 32);*/
  extra_hashes[state.hs.b[0] & 3](&state, 200, hash);
  oaes_free((OAES_CTX **) &aes_ctx);
};