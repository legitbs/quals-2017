all: sq2c pegem hsq

pegem: pegem.c pegem.h
	gcc -o pegem pegem.c

hsq: hsq.cpp
	g++ -o hsq hsq.cpp

sq2c: sq2c.c
	gcc -o sq2c sq2c.c

pegem.h: peg.hsq
	./hsq -h peg.hsq -q
	./sq2c peg.sq > pegem.h
