
# Makefile

include		../Makefile.ini

TARGET		= main

OBJS		= main.o

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
main.o: main.cpp ../include/ARDrawingContext.hpp ../include/ARPipeline.hpp ../include/DebugHelpers.hpp



