DIRECTORY=bin
LINUX=linux-agent
FLAGS=-ldflags "-s -w"

all: clean create-directory poetry

create-directory:
	mkdir ${DIRECTORY}

poetry: 
	echo "Compiling static Linux binary"
	docker run --rm=true -itv $(PWD):/mnt alpine:3.7 /mnt/build_static.sh
	
clean:
	rm -rf ${DIRECTORY}