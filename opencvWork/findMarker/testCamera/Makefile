
# Makefile

include		../Makefile.ini

TARGET		= main

OBJS		= geometryStructs.o \
		SolidSphere.o \
		ARDrawingContext.o \
		main.o

CPPFLAGS	= $(OVERALLCPPFLAGS) -I$(ARINCLUDE)
LDFLAGS		= $(OVERALLLDFLAGS) -L$(ARLIB) -lar -lm `pkg-config --cflags --libs opencv` \
		-lX11 -lGL -lGLU -lglut `sdl-config --cflags --libs` -lSDL

# Implicit rule how to compile
.cc.o:
	$(CCC) $(CPPFLAGS) -c $<

all: $(TARGET) $(OBJS)

$(TARGET) : $(OBJS) $(ARLIBFILE)
	$(CCC) -o $(TARGET) $(OBJS) $(LDFLAGS) $(LIBS)

clean:
	rm -f $(OBJS) data.* *~

superclean: clean
	rm -f $(TARGET)

strip:
	strip $(OBJS) $(TARGET)

# File dependencies
geometryStructs.o: geometryStructs.cpp geometryStructs.hpp
SolidSphere.o: SolidSphere.cpp SolidSphere.hpp
ARDrawingContext.o: ARDrawingContext.cpp ARDrawingContext.hpp geometryStructs.hpp
main.o: main.cc ../include/Marker.hpp ../include/MarkerDetector.hpp



