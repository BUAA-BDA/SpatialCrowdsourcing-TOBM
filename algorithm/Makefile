##
#	\Author: Yuxiang Zeng
##
CC = gcc
CXX = g++
LIBS = -lrt
CFLAGS = --std=c++11 -W -Wall -Wno-sign-compare -O2 -s -pipe -mmmx -msse -msse2 -msse3
TARGET = ./all

all: Greedy Random ext-Ranking Batch-GR Batch-LLEP Batch-NNP Guide POLAR-OP
Greedy: monitor.h global.h Greedy.cpp
	$(CXX) $(CFLAGS) -o Greedy Greedy.cpp $(LIBS) -D WATCH_MEM

Random: monitor.h global.h Random.cpp
	$(CXX) $(CFLAGS) -o Random Random.cpp $(LIBS) -D WATCH_MEM

ext-Ranking: monitor.h global.h ERank.cpp
	$(CXX) $(CFLAGS) -o ext-Ranking ERank.cpp $(LIBS) -D WATCH_MEM

Batch-GR: monitor.h global.h Batch.cpp
	$(CXX) $(CFLAGS) -o Batch-GR Batch.cpp $(LIBS) -D WATCH_MEM

Batch-LLEP: monitor.h global.h Hungarian.h LLEP.cpp
	$(CXX) $(CFLAGS) -o Batch-LLEP LLEP.cpp $(LIBS) -D WATCH_MEM

Batch-NNP: monitor.h global.h NNP.cpp
	$(CXX) $(CFLAGS) -o Batch-NNP NNP.cpp $(LIBS) -D WATCH_MEM
	
Guide: monitor.h global.h Guide.cpp
	$(CXX) $(CFLAGS) -o Guide Guide.cpp $(LIBS) -D WATCH_MEM

POLAR-OP: monitor.h global.h Polar-op.cpp
	$(CXX) $(CFLAGS) -o POLAR-OP Polar-op.cpp $(LIBS) -D WATCH_MEM

.PHONY: clean 
clean:
		-@rm *.o *.gcno *~ 2> /dev/null || true
		-@rm Greedy Random ext-Ranking Batch-GR Batch-LLEP Batch-NNP Guide POLAR-OP 2> /dev/null || true	
		
