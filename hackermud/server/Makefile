CXX=g++
#CCFLAGS=-g3 -Wall -w -pedantic -fmessage-length=0 -fno-stack-protector -D_FORTIFY_SOURCE=0 -pie -fPIE
CXXFLAGS=-std=c++0x -O1 -g3 -Wall -w -m64 -maes -mavx -pie -fPIE
#CXXFLAGS=-std=c++0x -O1 -g3 -Wall -w
#CCFLAGS=-g3 -Wall -w -pedantic -fmessage-length=0 -fno-stack-protector 
#CCFLAGS=-g3 -Wall -w -pedantic -fmessage-length=0 -fno-stack-protector -fPIC

O_FILES=main.o common.o sha256.o MersenneRNG.o bitwriter.o md5.o player.o room.o npc.o world.o damage.o script.o ipaddress.o socketbase.o socketexception.o tcpsockets.o udpsockets.o semaphore.o threadbase.o threadexception.o mutualexclusion.o condition.o streamdata.o connection.o item.o networkcommands.o pow.o blake2b.o

hackerserver : $(O_FILES)
	$(CXX) $(CXXFLAGS) -o hackerserver  $(O_FILES) -pthread

# dependency stuff, see: http://www.cs.berkeley.edu/~smcpeak/autodepend/autodepend.html
# pull in dependency info for *existing* .o files
-include $(O_FILES:.o=.d)

.SUFFIXES : .o .cpp

.cpp.o :  
	$(CXX) $(CXXFLAGS) -c $<
	$(CXX) -MM $(CXXFLAGS) $*.cpp > $*.d

.c.o   :
	$(CXX) $(CXXFLAGS) -c $<
	$(CXX) -MM $(CXXFLAGS) $*.c > $*.d

clean:
	rm -f hackerserver *.o *.d
