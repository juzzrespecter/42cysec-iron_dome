package main

import (
	"crypto/aes"
	"io/ioutil"
	"fmt"
	"os"
)

var key_aes = [16]byte{226, 97, 2, 136, 60, 123, 178, 35, 125, 144, 94, 0, 12, 164, 58, 23}

func aes_en64(data []byte) []byte {
	for len(data) % 16 != 0 {
		data = append(data, 0)
	}
	cipher, err := aes.NewCipher(key_aes[:])
	if err != nil {
		fmt.Println(err)
	}
	data_encrypted := make([]byte, len(data))
	for i := 0; i < len(data_encrypted)/16; i++ {
		cipher.Encrypt(data_encrypted[i*16:(i+1)*16], data[i*16:(i+1)*16])
	}
	return data_encrypted
}

func main() {
	a, _ := ioutil.ReadFile(os.Args[1])
	ioutil.WriteFile(os.Args[1] + ".ft", aes_en64([]byte(a)), 0600)
}