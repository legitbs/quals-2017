obj-m += defcon_2017.o 
defcon_2017-y := a.o b.o c.o d.o

A := /lib/modules/4.11.0-rc5/build
B := /tmp/lightning_defcon_2017

default:
	$(MAKE) -C $(A) M=$(B) modules
