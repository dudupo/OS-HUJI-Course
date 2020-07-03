CC=gcc
CXX=gcc
RANLIB=ranlib

LIBSRC=*.c 
LIBOBJ=$(LIBSRC:.cpp=.o)

INCS=-I.
CFLAGS = -Wall -fpermissive  -g $(INCS) 
CXXFLAGS = -Wall -fpermissive  -g $(INCS) 

OSMLIB = libuthreads.a
TARGETS = $(OSMLIB)

TAR=tar
TARFLAGS=-cvf
TARNAME=ex2.tar
TARSRCS=$(LIBSRC) Makefile README *.h

all: $(LIBSRC)
	$(CC) demo_itimer.c -o ./bin/demo_itimer
$(TARGETS): $(LIBOBJ)
	$(AR) $(ARFLAGS) $@ $^
	$(RANLIB) $@

clean:
	$(RM) $(TARGETS) $(OSMLIB) *.o *~ *core

depend:
	makedepend -- $(CFLAGS) -- $(SRC) $(LIBSRC)

tar:
	$(TAR) $(TARFLAGS) $(TARNAME) $(TARSRCS)
