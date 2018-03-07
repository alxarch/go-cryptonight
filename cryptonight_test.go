package cryptonight

import (
	"bytes"
	"encoding/hex"
	"encoding/json"
	"io/ioutil"
	"testing"
)

type testData struct {
	Hash, Data string
}

var pairs []testData

func init() {
	data, err := ioutil.ReadFile("testdata.json")
	if err != nil {
		panic(err)
	}
	if err = json.Unmarshal(data, &pairs); err != nil {
		panic(err)
	}
}

func Test_HashContext(t *testing.T) {
	ctx := new(HashContext)
	for i, p := range pairs {
		data, _ := hex.DecodeString(p.Data)
		expect, _ := hex.DecodeString(p.Hash)
		h, err := ctx.Hash(data)
		if err != nil {
			t.Error(err)
		} else if !bytes.Equal(h[:], expect) {
			t.Errorf("Invalid hash %d %x", i, h[:])
		}
	}

}
