# makefile

all: dataserver client

common.o: common.h common.cpp
	g++ -g -w -std=c++11 -c common.cpp

Histogram.o: Histogram.h Histogram.cpp
	g++ -g -w -std=c++11 -c Histogram.cpp

FIFOreqchannel.o: FIFOreqchannel.h FIFOreqchannel.cpp
	g++ -g -w -std=c++11 -c FIFOreqchannel.cpp

MQreqchannel.o: MQreqchannel.h MQreqchannel.cpp
	g++ -g -w -std=c++11 -c MQreqchannel.cpp

SHMreqchannel.o: SHMreqchannel.h SHMreqchannel.cpp
	g++ -g -w -std=c++11 -c SHMreqchannel.cpp

client: client.cpp Histogram.o FIFOreqchannel.o MQreqchannel.o SHMreqchannel.o common.o
	g++ -g -w -std=c++11 -no-pie -o client client.cpp Histogram.o FIFOreqchannel.o MQreqchannel.o SHMreqchannel.o common.o -lpthread -lrt

dataserver: dataserver.cpp FIFOreqchannel.o MQreqchannel.o SHMreqchannel.o common.o
	g++ -g -w -std=c++11 -no-pie -o dataserver dataserver.cpp FIFOreqchannel.o MQreqchannel.o SHMreqchannel.o common.o -lpthread -lrt

clean:
	rm -rf *.o fifo* dataserver client 

