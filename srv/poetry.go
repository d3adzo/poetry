package main

import (
	"flag"
	"fmt"
	"io"
	"io/ioutil"
	"log"
	"net"
	"os"
)

// quill logo

func tcp_con_handle(con net.Conn) {
	chan_to_stdout := stream_copy(con, os.Stdout)
	chan_to_remote := stream_copy(os.Stdin, con)
	select {
	case <-chan_to_stdout:
		log.Println("Remote connection is closed")
	case <-chan_to_remote:
		log.Println("Local program is terminated")
	}
}

// Performs copy operation between streams: os and tcp streams
func stream_copy(src io.Reader, dst io.Writer) <-chan int {
	buf := make([]byte, 1024)
	sync_channel := make(chan int)
	go func() {
		defer func() {
			if con, ok := dst.(net.Conn); ok {
				con.Close()
				log.Printf("Connection from %v is closed\n", con.RemoteAddr())
			}
			sync_channel <- 0 // Notify that processing is finished
		}()
		for {
			var nBytes int
			var err error
			nBytes, err = src.Read(buf)
			if err != nil {
				if err != io.EOF {
					log.Printf("Read error: %s\n", err)
				}
				break
			}
			_, err = dst.Write(buf[0:nBytes])
			if err != nil {
				log.Fatalf("Write error: %s\n", err)
			}
		}
	}()
	return sync_channel
}

func main() {
	var iFace string
	var source string
	var target string

	// export IFACE=<interface>
	iFace, ok := os.LookupEnv("IFACE")
	if !ok {
		fmt.Printf("IFACE environment variable not set\n")
		return
	}

	flag.StringVar(&target, "t", "127.0.0.1", "IP address to target")

	flag.Parse()

	if target == "127.0.0.1" {
		flag.Usage()
		return
	}

	source, err := GetInterfaceIpv4Addr(iFace)
	if err != nil {
		panic(err)
		return
	}

	opener := "POET~" + source
	sendUDPPacket(iFace, source, target, opener, 77, 7714)

	listener, err := net.Listen("tcp", source+":5858") // TODO change port
	if err != nil {
		log.Fatalln(err)
	}

	con, err := listener.Accept()
	if err != nil {
		log.Fatalln(err)
	}
	b, err := ioutil.ReadFile("art.txt")
	if err != nil {
		panic(err)
	}
	fmt.Println(string(b))
	
	log.Println("Connected: ", con.RemoteAddr())

	tcp_con_handle(con)
}
