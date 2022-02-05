obj-m := starter.o
starter-objs := start.o stop.o inc/helper.o
ccflags-y := -I$(src)/inc/

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
