# all: libuthreads.a

all: uthreads.o  #let's link library files into a static library
	ar rcs libuthreads.a uthreads.o Threads.o

uthreads.o: uthreads.cpp Threads.cpp
	g++ -c uthreads.cpp Threads.cpp  -Wall


