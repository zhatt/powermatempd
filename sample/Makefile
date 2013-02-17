CXXFLAGS=-Wall -Wextra -g -Werror

all: fakepm testlog

LIBOBJS = ../log.o
OBJS = $(LIBOBJS) fakepm.o testlog.o
HEADERS = inputtrace.h ../log.h

$(OBJS) : $(HEADERS)

fakepm : $(LIBOBJS) fakepm.o
	$(CXX) $^ -o $@ 

testlog : $(LIBOBJS) testlog.o ../log.o
	$(CXX) $^ -o $@ 

clean:
	rm -f *.o *~ fakepm testlog
