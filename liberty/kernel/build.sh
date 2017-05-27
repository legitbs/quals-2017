#/bin/bash
make
python ../server/compress.py defcon_2017_dev.o
python ../server/compress.py iaesx64.o
python ../server/compress.py readsize.o
python ../server/compress.py readwrite.o
python ../server/compress.py ../server/kernel/Makefile

cp *.o.a ../server/kernel

