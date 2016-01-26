package main

import (
	. "fmt"
	. "net"
)

func checkError(err error) {
	if err != nil {
		Println("Error: ", err)
	}
}

func recieve() {
	ServerAddr, err := ResolveUDPAddr("udp", ":30000")
	checkError(err)
	ServerConn, err := ListenUDP("udp", ServerAddr)
	checkError(err)
	defer ServerConn.Close()
	buf := make([]byte, 1024)
	for {
		n, addr, err := ServerConn.ReadFromUDP(buf)
		checkError(err)
		Println("Received ", string(buf[0:n]), " from ", addr)
	}
}

func main() {
	//listen30000()
	send()
}
