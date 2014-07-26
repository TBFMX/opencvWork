
# Makefile

include		../Makefile.ini

TARGET		= renderModel

OBJS		= geometryStructs.o \
		SolidSphere.o \
		DrawingContext.o \
		renderModel.o

CPPFLAGS	= $(OVERALLCPPFLAGS) -I$(ARINCLUDE)
LDFLAGS		= $(OVERALLLDFLAGS) -L$(ARLIB) -lar -lm `pkg-config --cflags --libs opencv` \
		-lX11 -lGL -lGLU -lglut -lGLEW

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
DrawingContext.o: DrawingContext.cpp DrawingContext.hpp geometryStructs.hpp
renderModel.o: renderModel.cpp



