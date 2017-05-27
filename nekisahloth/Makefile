CC=g++
CCFLAGS=-fPIC -pie -Wno-unused-result -O2 -m64 -Wall -I./jsoncpp/include -z now
LDFLAGS=-L. -ljsoncpp -lsqlite3 -lOpenSSL -lZLib -lBase64 -lmysqlclient

LIB_CCFLAGS=-fPIC -Wno-unused-result -O2 -m64 -Wall -z now
LIB_LDFLAGS=-shared -fPIC -lssl -lcrypto -lz -lbsd

O_FILES = nekisahloth.o
O_LIBS = OpenSSL.so

CPPFILES = $(wildcard *.cpp)
OBJECTS = $(patsubst %.cpp,%.o,$(CPPFILES))

LIB_CPPFILES = $(wildcard lib/*.cpp)
LIB_OBJECTS = $(patsubst %.cpp,%.o,$(LIB_CPPFILES))
LIBS = $(patsubst lib/%.cpp,lib%.so,$(LIB_CPPFILES))

lib/%.o: lib/%.cpp
	$(CC) $(LIB_CCFLAGS) -c $< -o $@

lib%.so: lib/%.o
	$(CC) -Wl,-soname,$@ -o $@ $< $(LIB_LDFLAGS)
	strip -s -x -X $@

all : $(OBJECTS) $(LIBS) $(LIB_OBJECTS)
	python modify-lib.py libssl.so.1.0.0 0x3f9be9420000
	$(CC) $(CCFLAGS) -o nekisahloth $(O_FILES) $(LDFLAGS)
	strip -s -x -X nekisahloth

%.o: %.cpp
	$(CC) $(CCFLAGS) -c $<

clean:
	rm -f nekisahloth $(OBJECTS) $(LIB_OBJECTS) $(LIBS)
