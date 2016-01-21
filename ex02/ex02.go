package main

import (
	. "fmt"
	"runtime"
)

var i = 0

func thread1(done chan<- bool, sync chan bool) {
	for j := 0; j < 1000000; j++ {
		<-sync
		i++
		sync <- true
	}
	done <- true
}

func thread2(done chan<- bool, sync chan bool) {
	for j := 0; j < 1000000; j++ {
		<-sync
		i--
		sync <- true
	}
	done <- true
}

func main() {
	runtime.GOMAXPROCS(runtime.NumCPU())
	done := make(chan bool, 2)
	sync := make(chan bool, 1)
	sync <- true
	go thread1(done, sync)
	go thread2(done, sync)
	<-done
	<-done
	Println(i)
}
