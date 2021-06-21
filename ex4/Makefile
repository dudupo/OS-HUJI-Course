CC=g++
CXX=g++
RANLIB=ranlib

LIBSRC=PhysicalMemory.cpp VirtualMemory.cpp
LIBOBJ=$(LIBSRC:.cpp=.o)
LIBHEAD=$(LIBSRC:.cpp=.h)

INCS=-I.
CFLAGS = -Wall -std=c++11 -fpermissive  -g $(INCS) 
CXXFLAGS = -Wall -std=c++11 -fpermissive  -g $(INCS) 

OSMLIB = libMemmory.a
TARGETS = $(OSMLIB)

TAR=tar
TARFLAGS=-cvf
TARNAME=ex4.tar
TARSRCS=$(LIBSRC) Makefile README  $(LIBHEAD) MemoryConstants

all: $(TARGETS)

$(TARGETS): $(LIBOBJ)
	$(AR) $(ARFLAGS) $@ $^
	$(RANLIB) $@

clean:
	$(RM) $(TARGETS) $(OSMLIB) $(OBJ) $(LIBOBJ) *~ *core

depend:
	makedepend -- $(CFLAGS) -- $(SRC) $(LIBSRC)

tar:
	$(TAR) $(TARFLAGS) $(TARNAME) $(TARSRCS)
