all: lib/obj.o lib/misc.o
	g++ parser.cc lib/obj.o lib/misc.o -o parser -L/usr/lib `sdl-config --cflags --libs` `pkg-config opencv --cflags --libs` -lGL -lGLU -lpthread

lib/obj.o: src/obj.h src/obj.cc
	g++ src/obj.cc -c -o lib/obj.o

lib/misc.o: src/misc.h src/misc.cc
	g++ src/misc.cc -c -o lib/misc.o

clean:
	@rm -f *~ src/*~ lib/* bin/*
