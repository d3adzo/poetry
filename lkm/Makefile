obj-m := poet.o
#poet-objs := kill.o netfilter.o
CC = gcc -Wall 
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
ccflags-y := -I$(src)/ftrace_hook

poet:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
