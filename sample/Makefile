CXXFLAGS=-Wall -Wextra -g -Werror

all: fakepm

LIBOBJS = 
OBJS = $(LIBOBJS) fakepm.o
HEADERS = inputtrace.h

$(OBJS) : $(HEADERS)

fakepm : $(LIBOBJS) fakepm.o
	$(CXX) $^ -o $@ 

clean:
	rm -f *.o *~ fakepm
