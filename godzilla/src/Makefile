CXX=g++
#CCFLAGS=-g3 -Wall -w -pedantic -fmessage-length=0 -fno-stack-protector -D_FORTIFY_SOURCE=0 -pie -fPIE
#CXXFLAGS=-std=c++0x -g3 -Wall -w -fmessage-length=0 -fno-stack-protector -D_FORTIFY_SOURCE=0 -m64 -maes -mavx
#CXXFLAGS=-std=c++0x -g3 -Wall -w -fmessage-length=0 -fno-stack-protector -D_FORTIFY_SOURCE=0 -m64 -maes -mavx
CXXFLAGS=-std=c++0x -O2 -Wall -w -m64 -maes -mavx -pie -fPIE
#CCFLAGS=-g3 -Wall -w -pedantic -fmessage-length=0 -fno-stack-protector 
#CCFLAGS=-g3 -Wall -w -pedantic -fmessage-length=0 -fno-stack-protector -fPIC

O_FILES=main.o streamdata.o cioconnection.o bigint.o common.o commands.o cipher.o FastCodeTimer.o rsa.o

hackermud : $(O_FILES)
	$(CXX) $(CXXFLAGS) -o customrsa  $(O_FILES)

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
	rm -f customrsa *.o *.d
