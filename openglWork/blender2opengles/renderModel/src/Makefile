
# Makefile for the arc++ kernel

include		../Makefile.ini



TARGET		= $(ARLIBFILE)

ARKERNEL= 	GeometryTypes.o \
		CameraCalibration.o \
		TinyLA.o \
		Marker.o \
		MarkerDetector.o

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
GeometryTypes.o: GeometryTypes.cpp ../include/GeometryTypes.hpp
CameraCalibration.o: CameraCalibration.cpp ../include/CameraCalibration.hpp
TinyLA.o: TinyLA.cc ../include/TinyLA.h
Marker.o: Marker.cpp ../include/Marker.hpp ../include/GeometryTypes.hpp
MarkerDetector.o: MarkerDetector.cpp ../include/MarkerDetector.hpp ../include/GeometryTypes.hpp
