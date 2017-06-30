package main;

import (
	"fmt"
	"io/ioutil"
	"compress/zlib"
	"bytes"
)

func main() {
	buff, _ := ioutil.ReadFile("trans_1.trans")
	fmt.Println(len(buff))
	buff = append(buff[:0], buff[4:]...)
	fmt.Println(len(buff))
	b := bytes.NewReader(buff)
	r, err := zlib.NewReader(b)
	if err != nil {
		panic(err)
	}
	defer r.Close()
	x, _ := ioutil.ReadAll(r)
	fmt.Println(len(x))
	fmt.Println(string(x))
}