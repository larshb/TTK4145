package main

import (
	"fmt"
	"net"
	"strconv"
	"time"
)

func CheckError(err error) {
	if err != nil {
		fmt.Println("Error: ", err)
	}
}

func udpRecieve() {
	ServerAddr, err := net.ResolveUDPAddr("udp", ":30000")
	CheckError(err)
	ServerConn, err := net.ListenUDP("udp", ServerAddr)
	CheckError(err)
	defer ServerConn.Close()
	buf := make([]byte, 1024)
	for {
		n, addr, err := ServerConn.ReadFromUDP(buf)
		CheckError(err)
		fmt.Println("Received ", string(buf[0:n]), " from ", addr)
	}
}

func udpSend() {
	ServerAddr, err := net.ResolveUDPAddr("udp", "255.255.255.255:20017")
	CheckError(err)

	LocalAddr, err := net.ResolveUDPAddr("udp", "127.0.0.1:0")
	CheckError(err)

	Conn, err := net.DialUDP("udp", LocalAddr, ServerAddr)
	CheckError(err)

	defer Conn.Close()
	i := 0
	for {
		msg := strconv.Itoa(i)
		i++
		buf := []byte(msg)
		_, err := Conn.Write(buf)
		if err != nil {
			fmt.Println(msg, err)
		}
		time.Sleep(time.Second * 1)
	}
}

func main() {
	//udpRecieve()
	udpSend()
}
