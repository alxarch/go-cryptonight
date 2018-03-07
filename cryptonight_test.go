package cryptonight

import (
	"bytes"
	"encoding/hex"
	"testing"
)

func Test_HashContextV1(t *testing.T) {
	ctx := NewHashContext(1)
	for h, d := range testDataHashV1 {
		data, _ := hex.DecodeString(d)
		expect, _ := hex.DecodeString(h)
		h, err := ctx.Hash(data)
		if err != nil {
			t.Error(err)
		} else if !bytes.Equal(h[:], expect) {
			t.Errorf("Invalid hash %s %x", h, h[:])
		}
	}

}
func Test_HashContextV0(t *testing.T) {
	ctx := NewHashContext(0)
	for h, d := range testDataHashV0 {
		data, _ := hex.DecodeString(d)
		expect, _ := hex.DecodeString(h)
		h, err := ctx.Hash(data)
		if err != nil {
			t.Error(err)
		} else if !bytes.Equal(h[:], expect) {
			t.Errorf("Invalid hash %s %x", h, h[:])
		}
	}

}

var testDataHashV0 = map[string]string{
	"2f8e3df40bd11f9ac90c743ca8e32bb391da4fb98612aa3b6cdc639ee00b31f5": "6465206f6d6e69627573206475626974616e64756d",
	"722fa8ccd594d40e4a41f3822734304c8d5eff7e1b528408e2229da38ba553c4": "6162756e64616e732063617574656c61206e6f6e206e6f636574",
	"bbec2cacf69866a8e740380fe7b818fc78f8571221742d729d9d02d7f8989b87": "63617665617420656d70746f72",
	"b1257de4efc5ce28c6b40ceb1c6c8f812a64634eb3e81c5220bee9b2b76a6f05": "6578206e6968696c6f206e6968696c20666974",
}

var testDataHashV1 = map[string]string{
	"b5a7f63abb94d07d1a6445c36c07c7e8327fe61b1647e391b4c7edae5de57a3d": "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
	"80563c40ed46575a9e44820d93ee095e2851aa22483fd67837118c6cd951ba61": "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
	"5bb40c5880cef2f739bdb6aaaf16161eaae55530e7b10d7ea996b751a299e949": "8519e039172b0d70e5ca7b3383d6b3167315a422747b73f019cf9528f0fde341fd0f2a63030ba6450525cf6de31837669af6f1df8131faf50aaab8d3a7405589",
	"613e638505ba1fd05f428d5c9f8e08f8165614342dac419adc6a47dce257eb3e": "37a636d7dafdf259b7287eddca2f58099e98619d2f99bdb8969d7b14498102cc065201c8be90bd777323f449848b215d2977c92c4c1c2da36ab46b2e389689ed97c18fec08cd3b03235c5e4c62a37ad88c7b67932495a71090e85dd4020a9300",
	"ed082e49dbd5bbe34a3726a0d1dad981146062b39d36d62c71eb1ed8ab49459b": "38274c97c45a172cfc97679870422e3a1ab0784960c60514d816271415c306ee3a3ed1a77e31f6a885c3cb",
}
