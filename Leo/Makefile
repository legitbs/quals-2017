

all: leo module.bin

CC=gcc

#CFLAGS= -DDEBUG

leo: leo.o
	$(CC) $(CFLAGS) -o leo leo.o `curl-config --libs` -g -fno-stack-protector
	strip leo


leo.o: leo.c
	$(CC) $(CFLAGS) -c -o leo.o leo.c -g  -fno-stack-protector

module.bin: makemodule
	./makemodule; mv module.bin 23fsf251l10o121415


makemodule: makemodule.o
	$(CC) $(CFLAGS) -o makemodule makemodule.o -fPIC

makemodule.o: makemodule.c
	$(CC) $(CFLAGS) -c  makemodule.c -fno-stack-protector -fPIC

.PHONY: clean

clean:
		rm -f *.o makemodule leo 23fsf251l10o121415 
