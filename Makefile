.PHONY: all clean
.DEFAULT_GOAL := all

LIBS=-lrt -lm -lssl -lcrypto
INCLUDES=-./include
CFLAGS=-O3 -g

output = danzer

all: main

main: tracer.cc
	g++ $(CFLAGS) -o danzer tracer.cc fastcdc.c $(LIBS)

clean:
	rm $(output)
