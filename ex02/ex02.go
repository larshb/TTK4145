package main

import (
	. "fmt"
	"runtime"
	"time"
)

var i = 0

func thread1() {
	for j := 0; j < 1000000; j++ {
		i++
	}
}

func thread2() {
	for j := 0; j < 1000000; j++ {
		i--
	}
}

func main() {
	runtime.GOMAXPROCS(runtime.numCPU())
	go thread1()
	go thread2()

	time.Sleep(100 * time.Millisecond)
	Println(i)
}
