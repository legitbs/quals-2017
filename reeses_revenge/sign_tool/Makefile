CC=g++
CCFLAGS=-g3 -g -Wall -w -pedantic -fmessage-length=0

O_FILES = CommonFile.o converter.o meb_file.o crypto.o meps_converter.o

mudserver : $(O_FILES)
	$(CC) $(CCFLAGS) -o mep_converter $(O_FILES) -lcrypto

# dependency stuff, see: http://www.cs.berkeley.edu/~smcpeak/autodepend/autodepend.html
# pull in dependency info for *existing* .o files
-include $(O_FILES:.o=.d)

.SUFFIXES : .o .cpp

.cpp.o :  
	$(CC) $(CCFLAGS) -c $<
	$(CC) -MM $(CFLAGS) $*.cpp > $*.d

clean:
	rm -f mep_converter *.o *.d
