# go-cryptonight

Cryptonight hash cgo binding

Uses stripped down version of `src/crypto` from monero-project

## Usage

```go
import cryptonight "github.com/alxarch/go-cryptonight"

var (
    data = []byte("Hello World")
    // Allocate a scratchpad for cryptonight hashing.
    ctx = new(cryptonight.HashContext)
    hash = ctx.HashBlob(data)
)
```
