CC = g++
CFLAGS = -c -Wall
LDFLAGS = -lGL -lGLU `sdl-config --cflags --libs` -lSDL -I../include -I/usr/include
SOURCES = *.cpp
OBJECTS = $(SOURCES:.cpp = .o)
VERSION = geometry
EXECUTABLE = WavefrontLoader-$(VERSION)

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(EXECUTABLE)

