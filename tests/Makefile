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

ex1:

ex22: bin/ex2.o 
	gcc bin/ex2.o ../ex2/bin/libuthreads.o ../ex2/bin/list.o ../ex2/bin/p_uthreads.o -o ./bin/ex2test	


# ex2: bin/ex2.o  
# 	gcc bin/ex2.o ../ex2/bin/libuthreads.a -o ./bin/ex2test 

bin/ex2.o: ex2.c
	gcc -c -g ex2.c -o bin/ex2.o -I../ex2 -std=c17

ex3:

ex4:

ex5:

all: ex1, ex2, ex3, ex4, ex5

clean:
	$(RM) $(TARGETS) $(OSMLIB) *.o *~ *core

depend:
	makedepend -- $(CFLAGS) -- $(SRC) $(LIBSRC)

tar:
	$(TAR) $(TARFLAGS) $(TARNAME) $(TARSRCS)
