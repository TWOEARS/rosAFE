CPP = g++
C = gcc 
CFLAGS = -Wall -g -O -std=c++11 -DHAVE_INLINE

all:
	$(CPP) $(CFLAGS) bufferTest.cpp -o runBufferTest

rebuild: uninstall clean all	

uninstall:
	rm -rf run

clean:
	rm -f *.o all
