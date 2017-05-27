SVCNAME = liberty-server

CC = gcc
CFLAGS = -g -Wall -Wshadow -Wextra -O2 -fPIC -Wno-unused-parameter -Wno-unused-result

# This makes it so that each c file is compiled in the directory
CFILES = $(wildcard *.c)
OBJECTS = $(patsubst %.c,%.o,$(CFILES))

CFILES_PAYLOADS = $(wildcard payloads/*.c)
OBJECTS_PAYLOADS = $(patsubst %.c,%.o,$(CFILES_PAYLOADS))
PAYLOADS = $(patsubst %.c,%.bin,$(CFILES_PAYLOADS))

CFILES_PAYLOADS_64 = $(wildcard payloads/64/*.c)
OBJECTS_PAYLOADS_64 = $(patsubst %.c,%.o,$(CFILES_PAYLOADS_64))
PAYLOADS_64 = $(patsubst %.c,%.bin,$(CFILES_PAYLOADS_64))

CFILES_PAYLOADS_64_AES = $(wildcard payloads/64/aes/*.c)
OBJECTS_PAYLOADS_64_AES = $(patsubst %.c,%.o,$(CFILES_PAYLOADS_64_AES))
PAYLOADS_64_AES = $(patsubst %.c,%.bin,$(CFILES_PAYLOADS_64_AES))

S_FILES_PAYLOADS = $(wildcard payloads/*.S)
S_OBJECTS = $(patsubst %.S,%.o,$(S_FILES_PAYLOADS))
S_PAYLOADS = $(patsubst %.S,%.bin,$(S_FILES_PAYLOADS))

S_FILES_PAYLOADS_64_AES = $(wildcard payloads/64/aes/*.S)
S_OBJECTS_64_AES = $(patsubst %.S,%.o,$(S_FILES_PAYLOADS_64_AES))

KEYFILE = $(wildcard kernel/*.c)
KEYFILE_OBJECTS = $(patsubst %.c,%.o.z,$(KEYFILE))

RECV = `readelf -r ../client/liberty | grep recv | cut -d" " -f1`
SEND = `readelf -r ../client/liberty | grep send | cut -d" " -f1`
PUTS = `readelf -r ../client/liberty | grep puts | cut -d" " -f1`

all: $(OBJECTS) $(OBJECTS_PAYLOADS) $(OBJECTS_PAYLOADS_64) $(PAYLOADS) $(S_PAYLOADS) $(PAYLOADS_64) $(S_OBJECTS_64_AES) $(PAYLOADS_64_AES) $(KEYFILE_OBJECTS)
	$(CC) $(CFLAGS) $(S_OBJECTS_64_AES) $(OBJECTS) -o $(SVCNAME) $(LIBS)
	strip -s $(SVCNAME)

kernel/%.o.z: kernel/%.c
	$(CC) -Os -c $< -o $@ -m64 -fPIC

payloads/64/aes/%.bin: payloads/64/aes/%.o
	ld --pie -nostdlib -m elf_x86_64 --oformat binary -T payloads/payload-script payloads/64/aes/iaesx64.o $< -o $@

payloads/64/aes/%.o:	payloads/64/aes/%.S
	yasm -DRECV=0x$(RECV) -DSEND=0x$(SEND) -DPUTS=0x$(PUTS) $< -o $@ -f elf64

payloads/64/aes/%.o:	payloads/64/aes/%.c
	$(CC) -DRECV=0x$(RECV) -DSEND=0x$(SEND) -Wno-main -fno-stack-protector -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0 -Wno-implicit-function-declaration -z execstack -m64 -Wall -c $< -o $@ -Os  -fPIC -fno-asynchronous-unwind-tables

payloads/64/%.bin: payloads/64/%.o
	ld --pie -nostdlib -m elf_x86_64 --oformat binary -T payloads/payload-script $< -o $@

payloads/64/%.o:	payloads/64/%.c
	$(CC) -Wno-main -fno-stack-protector -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0 -Wno-implicit-function-declaration -z execstack -m64 -Wall -c $< -o $@ -Os  -fPIC -fno-asynchronous-unwind-tables

payloads/%.bin: payloads/%.S
	yasm -DRECV=0x$(RECV) -DSEND=0x$(SEND) -DPUTS=0x$(PUTS) $< -o $@

payloads/%.bin: payloads/%.o
	ld --pie -nostdlib -m elf_i386 --oformat binary -T payloads/payload-script $< -o $@

payloads/%.o:	payloads/%.c
	$(CC) -Wno-main -fno-stack-protector -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0 -Wno-implicit-function-declaration -z execstack -m32 -Wall -c $< -o $@ -Os  -fPIC -fno-asynchronous-unwind-tables

%.o:  %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(SVCNAME) *.o payloads/*.o payloads/*.bin payloads/64/*.o payloads/64/*.bin payloads/64/aes/*.bin payloads/64/aes/*.o kernel/*.o.z

.PHONY: clean
