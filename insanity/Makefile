CC=gcc
CCFLAGS= -g -Wno-unused-result -O2 -m64 -Wall -fPIC -pie

O_FILES = insanity.o

all : $(O_FILES)
	$(CC) $(CCFLAGS) -o insanity $(O_FILES) -L./lib -lpocketsphinx -lsphinxbase -lsphinxad -lz
	strip -s insanity
	strip -s lib/*.0


# dependency stuff, see: http://www.cs.berkeley.edu/~smcpeak/autodepend/autodepend.html
# pull in dependency info for *existing* .o files
-include $(O_FILES:.o=.d)

.SUFFIXES : .o .c

.c.o :  
	$(CC) $(CCFLAGS) -c $< -I./include -I./include/pocketsphinx -I./include/sphinxbase

clean:
	rm -f insanity *.o
