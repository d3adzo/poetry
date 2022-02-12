obj-m := poet.o
poet-objs := main.o
CC = gcc -Wall 
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
ccflags-y += -I$(src)/inc

poet:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
