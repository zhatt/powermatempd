#CXXFLAGS = -Wall -Wextra -g -Os
CXXFLAGS = -Wall -Wextra -g 
#-march=i486 -mtune=i686 -Os
LDFLAGS=-Wl,-O1

SUBDIRS = sample

all: powermatempd pmtrace subdirs

LIBOBJS = powermate.o mpd.o powermatempd.o log.o
OBJS = $(LIBOBJS) main.o pmtrace.o
HEADERS = powermate.h mpd.h powermatempd.h hgversion.h log.h

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

#
# Generate hgversion.h
# Verify that it has changed before moving the new copy in place.
#
.PHONY : hgversion
hgversion :
hgversion.h : hgversion
	@ echo "namespace HgVersion { ;" > hgversion.h.tmp
	@ echo "static const char* const hash = \"$$(hg id --id)\";" >> hgversion.h.tmp
	@ echo "static const char* const revision = \"$$(hg id --num)\";" >> hgversion.h.tmp
	@ echo "}" >> hgversion.h.tmp
	@ cmp -s hgversion.h.tmp hgversion.h || mv -f hgversion.h.tmp hgversion.h
	@ [[ -e hgversion.h.tmp ]] || echo "Generated hgversion.h"
	@ rm -f hgversion.h.tmp
