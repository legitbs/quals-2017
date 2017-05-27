obj-m += defcon_2017.o 
defcon_2017-y := readsize.o readwrite.o defcon_2017_dev.o iaesx64.o

KDIR  := /lib/modules/$(shell uname -r)/build
PWD   := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
