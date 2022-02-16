// +build linux

package main

import (
	"fmt"
	"net"
	"os"
	"syscall"

	"github.com/google/gopacket"
	"github.com/google/gopacket/layers"
)

// This example sends an UDP packet to 127.0.0.1:5000 using Linux raw socket. This program needs root priviledge or CAP_NET_RAW capability.
// Run "nc -ul 127.0.0.1 5000" to see the "HELLO" message in the payload. 

// http://www.pdbuchan.com/rawsock/rawsock.html

func open(ifName string) (net.PacketConn, error) {
	fd, err := syscall.Socket(syscall.AF_INET, syscall.SOCK_RAW, syscall.IPPROTO_RAW)
	if err != nil {
		return nil, fmt.Errorf("Failed open socket(syscall.AF_INET, syscall.SOCK_RAW, syscall.IPPROTO_RAW): %s", err)
	}
	syscall.SetsockoptInt(fd, syscall.IPPROTO_IP, syscall.IP_HDRINCL, 1)

	if ifName != "" {
		iface, err := net.InterfaceByName(ifName)
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

func buildUDPPacket(dst, src *net.UDPAddr) ([]byte, error) {
	buffer := gopacket.NewSerializeBuffer()
	payload := gopacket.Payload("HELLO")
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

func main() {
	conn, err := open("lo")
	if err != nil {
		panic(err)
	}
	dst := &net.UDPAddr{
		IP:   net.ParseIP("127.0.0.1"),
		Port: 5000,
	}
	b, err := buildUDPPacket(dst, &net.UDPAddr{IP: net.ParseIP("127.0.0.1"), Port: 5001})
	if err != nil {
		panic(err)
	}
	wlen, err := conn.WriteTo(b, &net.IPAddr{IP: dst.IP})
	if err != nil {
		panic(err)
	}
	fmt.Printf("Sent IP packet %d bytes to %s\n", wlen, dst)