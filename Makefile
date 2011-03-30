CXXFLAGS=-Wall -Wextra -g

all: powermatempd pmtrace

LIBOBJS = powermate.o mpd.o powermatempd.o
OBJS = $(LIBOBJS) main.o pmtrace.o
HEADERS = powermate.h mpd.h powermatempd.h

$(OBJS) : $(HEADERS)

powermatempd: $(LIBOBJS) main.o
	$(CXX) $^ -o $@

pmtrace: $(LIBOBJS) pmtrace.o
	$(CXX) $^ -o $@

clean:
	rm -f *.o *~ powermatempd pmtrace
