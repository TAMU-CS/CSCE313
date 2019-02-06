# makefile

all: memtest

Ackerman.o: Ackerman.cpp 
	g++ -std=c++11 -c -g Ackerman.cpp

BuddyAllocator.o : BuddyAllocator.cpp
	g++ -std=c++11 -c -g BuddyAllocator.cpp

Main.o : Main.cpp
	g++ -std=c++11 -c -g Main.cpp

memtest: Main.o Ackerman.o BuddyAllocator.o
	g++ -o memtest Main.o Ackerman.o BuddyAllocator.o

clean:
	rm *.o