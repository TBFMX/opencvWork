
# Makefile

include		../Makefile.ini

TARGET		= main

OBJS		= main.o

CPPFLAGS	= $(OVERALLCPPFLAGS) -I$(ARINCLUDE)
LDFLAGS		= $(OVERALLLDFLAGS) -L$(ARLIB) -lar -lm `pkg-config --cflags --libs opencv `

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
main.o: main.cc ../include/Marker.h ../include/MarkerDetector.h



