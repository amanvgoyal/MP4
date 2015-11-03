# makefile

all: dataserver simpleclient client

reqchannel.o: reqchannel.h reqchannel.cpp
	g++-4.7 -c -g reqchannel.cpp

dataserver: dataserver.cpp reqchannel.o 
	g++-4.7 -g -o dataserver dataserver.cpp reqchannel.o -lpthread

simpleclient: simpleclient.cpp reqchannel.o
	g++-4.7 -g -o simpleclient simpleclient.cpp reqchannel.o

client: client.cpp
	g++ -std=c++11 -Wno-write-strings -g -o client client.cpp /usr/lib/x86_64-linux-gnu/libboost_program_options.so
