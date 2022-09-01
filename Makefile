.PHONY: all clean
.DEFAULT_GOAL := all

LIBS=-lrt -lm -lstdc++fs -lssl -lcrypto
INCLUDES=-./include
CFLAGS=-std=c++17 -O3 -g

output = danzer

all: main

main: tracer.cc
	g++ $(CFLAGS) -o danzer tracer.cc $(LIBS)

clean:
	rm $(output)
