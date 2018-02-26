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

func Test_HashBlob(t *testing.T) {
	for i, p := range pairs {
		data, _ := hex.DecodeString(p.Data)
		expect, _ := hex.DecodeString(p.Hash)
		h := HashBlob(data, 0, nil)
		if !bytes.Equal(h[:], expect) {
			t.Errorf("Invalid hash %d %x", i, h[:])
		}
		break
	}

}
