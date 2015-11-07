# makefile

all: dataserver simpleclient client

reqchannel.o: reqchannel.h reqchannel.cpp
	g++-4.7 -c -g reqchannel.cpp

BoundedBuffer.o: BoundedBuffer.h BoundedBuffer.cpp semaphore.h
	g++-4.7 -c -g BoundedBuffer.cpp

dataserver: dataserver.cpp reqchannel.o 
	g++-4.7 -g -o dataserver dataserver.cpp reqchannel.o -lpthread

client: client.cpp reqchannel.o BoundedBuffer.o
	g++-4.7 -g -o client client.cpp reqchannel.o BufferBuffer.o
