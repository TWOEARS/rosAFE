CPP = g++
C = gcc 
CFLAGS = -Wall -g -O -std=c++11 -pthread -DHAVE_INLINE

all:
	$(CPP) $(CFLAGS) procVectorTest.cpp -o runProcVectorTest

rebuild: uninstall clean all	

uninstall:
	rm -rf run

clean:
	rm -f *.o all
