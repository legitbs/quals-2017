SVCNAME = liberty

CC = gcc
CFLAGS = -Os -fPIC --pie -m32 -Wl,-Map,$(SVCNAME).map

# This makes it so that each c file is compiled in the directory
CFILES = $(wildcard *.c)

all: $(OBJECTS)
	$(CC) $(CFLAGS) $(CFILES) -o $(SVCNAME)
	strip -s $(SVCNAME)

clean:
	rm -rf $(SVCNAME)

.PHONY: clean
