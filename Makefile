obj-m := tcp_neoreno.o
IDIR= /lib/modules/$(shell uname -r)/kernel/net/ipv4/
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) 

unload:
	-rmmod tcp_neoreno.ko

insert:	unload default
	insmod tcp_neoreno.ko
clean:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean
