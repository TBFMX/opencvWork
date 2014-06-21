
# Makefile for the arc++ kernel

include		../Makefile.ini



TARGET		= $(ARLIBFILE)

ARKERNEL= 	ARDrawingContext.o \
		CameraCalibration.o \
		GeometryTypes.o \
		ARPipeline.o \
		Pattern.o \
		PatternDetector.o

OBJS		= $(ARKERNEL) 

CPPFLAGS	= $(OVERALLCPPFLAGS) -I$(ARINCLUDE) 

# Implicit rule how to compile
.cc.o:
	$(CCC) $(CPPFLAGS) -c $<



# Create the AR library. Use either ld (for dynamic libraries) or ar
# and ranlib.
$(TARGET) : $(OBJS) 
	$(AR) -r $(TARGET) $(OBJS)
	$(RANLIB) $(TARGET)
#	$(LD) -r -o $(TARGET) $(OBJS)

clean:
	rm -f $(OBJS) *~

superclean: clean
	rm -f $(TARGET)

strip:
	strip $(OBJS)
	make



# File dependencies
ARDrawingContext.o: ARDrawingContext.cpp ../include/ARDrawingContext.hpp
CameraCalibration.o: CameraCalibration.cpp ../include/CameraCalibration.hpp
GeometryTypes.o: GeometryTypes.cpp ../include/GeometryTypes.hpp
ARPipeline.o: ARPipeline.cpp ../include/ARPipeline.hpp
Pattern.o: Pattern.cpp ../include/Pattern.hpp
PatternDetector.o: PatternDetector.cpp ../include/PatternDetector.hpp
