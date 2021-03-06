package cryptonight

/*
#cgo amd64 CFLAGS: -Wall
#include "src/cryptonight.c"
#include "src/hash.c"
#include "src/blake256.c"
#include "src/groestl.c"
#include "src/jh.c"
#include "src/skein.c"
#include "src/keccak.c"
#include "src/oaes_lib.c"


void cn_slow_hash(const void *data, size_t length, char *hash, uint8_t *scratchpad, int variant);

*/
import "C"

import (
	"errors"
	"unsafe"
)

// HashSize is the size of a hash in bytes
const HashSize = 32

// ScratchpadSize is the cryptonight scratchpad size in bytes
const ScratchpadSize = 1 << 21 // 2M scratchpad

// Hash is a cryptonight hash
type Hash [HashSize]byte

var zeroHash = Hash{}

// IsZero checks whether a hash is the zero value
func (h Hash) IsZero() bool {
	return h == zeroHash
}

// HashContext allocates a scratchpad for hashing
type HashContext struct {
	Variant    int
	scratchpad [ScratchpadSize]byte
}

// NewHashContext creates a new hash context for the specified variant
func NewHashContext(variant int) *HashContext {
	ctx := HashContext{
		Variant: variant,
	}
	return &ctx
}

// Hash hashes a blob of data using cryptonight variant for monero v7
func (ctx *HashContext) Hash(data []byte) (h Hash, err error) {
	if len(data) == 0 {
		return
	}
	if ctx.Variant > 0 {
		if len(data) < 43 {
			err = errors.New("Variant hash requires length > 43")
			return
		}
	}
	C.cn_slow_hash(
		unsafe.Pointer(&data[0]),
		C.size_t(len(data)),
		(*C.char)(unsafe.Pointer(&h[0])),
		(*C.uint8_t)(unsafe.Pointer(&ctx.scratchpad[0])),
		C.int(ctx.Variant),
	)
	return
}
