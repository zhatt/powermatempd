CXXFLAGS=-Wall -Wextra -g

all:	powermatempd

OBJS = main.o powermate.o mpdpowermate.o
HEADERS = powermate.h mpdpowermate.h

OBJS : $(HEADERS)

powermatempd: $(OBJS)
	$(CXX) $(OBJS) -o powermatempd

clean:
	rm -f *.o *~ powermatempd
