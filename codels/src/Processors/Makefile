CPP = g++
C = gcc 
CFLAGS = -Wall -g -O -pthread -std=c++11 -DHAVE_INLINE

all: 
	$(CPP) $(CFLAGS) -o run test.cpp

rebuild: uninstall clean all

uninstall:
	rm -rf run

clean:
	rm -f *.o all
