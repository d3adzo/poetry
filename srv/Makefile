DIRECTORY=out
FLAGS=-ldflags "-s -w"

all: clean poetry

poetry: 
	echo "Compiling static Linux binary"
	docker run --rm=true -itv $(PWD):/mnt alpine:3.7 /mnt/build_static.sh
	
clean:
	rm -rf ${DIRECTORY}
