CXXFLAGS = -Wall -Wextra -g
SUBDIRS = sample

all: powermatempd pmtrace subdirs

LIBOBJS = powermate.o mpd.o powermatempd.o
OBJS = $(LIBOBJS) main.o pmtrace.o
HEADERS = powermate.h mpd.h powermatempd.h

$(OBJS) : $(HEADERS)

powermatempd: $(LIBOBJS) main.o
	$(CXX) $^ -o $@ -lmpdclient

pmtrace: $(LIBOBJS) pmtrace.o
	$(CXX) $^ -o $@ -lmpdclient

.PHONY : subdirs $(SUBDIRS)

subdirs: $(SUBDIRS)

$(SUBDIRS) :
	$(MAKE) -C $@

cleanall : clean cleansubs

clean : 
	rm -f *.o *~ powermatempd pmtrace

cleansubs :
	$(MAKE) -C $(SUBDIRS) clean

