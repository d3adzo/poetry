obj-m := poet.o
PWD := $(shell pwd)
EXTRA_CFLAGS=-I$(PWD)/inc -I$(PWD)/src
poet-objs := main.o src/poet.o
CC = gcc -Wall 
KDIR := /lib/modules/$(shell uname -r)/build

poet:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

