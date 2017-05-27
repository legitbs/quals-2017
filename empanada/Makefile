NAME=empanada
CFLAGS=-std=c++14 -m32 -O0

all: CFLAGS += -DDEBUG=0 -fomit-frame-pointer -fdata-sections -ffunction-sections -L -Wl,--gc-sections
all: service
all: strip

debug: CFLAGS += -DDEBUG=1 -g3
debug: service

service:
	clang++ $(CFLAGS) service.cpp -o $(NAME)

strip:
	strip -g -S -d -x $(NAME)
	
clean:
	rm -rf $(NAME)
