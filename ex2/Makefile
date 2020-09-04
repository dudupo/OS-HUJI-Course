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



lib: bin/libuthreads.o bin/list.o bin/p_uthreads.o
	ar rcs bin/libuthreads.a  bin/libuthreads.o bin/list.o bin/p_uthreads.o

bin/heap.o:
	gcc -c -o bin/heap.o -g datastructure/heap.c

bin/p_uthreads.o: p_uthreads.c p_uthreads.h
	gcc -c -o bin/p_uthreads.o -g p_uthreads.c

bin/list.o: datastructures/list.c p_uthreads.c p_uthreads.h 
	gcc -c -o bin/list.o -g datastructures/list.c 

bin/libuthreads.o: uthreads.c uthreads.h 
	gcc -c -o bin/libuthreads.o -g uthreads.c

clean:
	rm -f *.o *.a $(TARGET)




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
