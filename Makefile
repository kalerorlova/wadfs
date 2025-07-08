all: libWad.a

Wad.o: Wad.cpp
	g++ -c Wad.cpp

libWad.a: Wad.o
	ar cr libWad.a Wad.o

libs: libWad.a
