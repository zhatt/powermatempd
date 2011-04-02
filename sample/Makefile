CFLAGS=-Wall -Wextra -g --std=gnu99

all: fakepm

LIBOBJS = 
OBJS = $(LIBOBJS) fakepm.o
HEADERS = inputtrace.h

$(OBJS) : $(HEADERS)

fakepm : $(LIBOBJS) fakepm.o
	$(CC) $^ -o $@ 

clean:
	rm -f *.o *~ fakepm
