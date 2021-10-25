CC=clang++-12
CFLAGS= -std=c++20 -O3

GCC=g++-10
GCCFLAGS= -std=gnu++20 -O3

LIB= -Wl,-rpath,/home/manycore6/lib
LDFLAGS= -ltbb -lm
DEBUG=-g

all: main

main: conv2d_layer.o filter.o NDGrid.o src/main.cc
	$(CC) $(LIB) $(CFLAGS) src/main.cc -o main $(LDFLAGS)

gcc: conv2d_layer.o filter.o NDGrid.o src/main.cc
	$(GCC) $(LIB) $(GCCFLAGS) src/main.cc -o main $(LDFLAGS)

conv2d_layer.o: src/conv2d_layer.hpp filter.o
	$(CC) $(CFLAGS) src/conv2d_layer.hpp -o conv2d_layer.o

filter.o: src/filter.hpp
	$(CC) $(CFLAGS) src/filter.hpp -o filter.o

NDGrid.o: src/NDGrid.hpp
	$(CC) $(CFLAGS) src/NDGrid.hpp -o NDGrid.o

clean:
	rm *.o main
