CPP = g++
C = gcc 
CFLAGS = -Wall -g -O -std=c++11 -DHAVE_INLINE

all: 
	$(CPP) $(CFLAGS) -o run main.cpp Filter.hpp

rebuild: uninstall clean all	

uninstall:
	rm -rf run

clean:
	rm -f *.o all
