CXXFLAGS=-Wall -O3 -g -fno-strict-aliasing
BINARIES=negativespaceclock

RGB_INCDIR=include
RGB_LIBDIR=lib
LDFLAGS+=-L$(RGB_LIBDIR) -lrgbmatrix -lrt -lm -lpthread

all : $(BINARIES)


negativespaceclock : negativespaceclock.o $(RGB_LIBRARY)
	$(CXX) $(CXXFLAGS) negativespaceclock.o -o $@ $(LDFLAGS)


%.o : %.cpp
	$(CXX) -I$(RGB_INCDIR) $(CXXFLAGS) -DADAFRUIT_RGBMATRIX_HAT -c -o $@ $<

clean:
	rm -f *.o $(OBJECTS) $(BINARIES)

