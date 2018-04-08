#make file for Nron Net

ntest: nlib
	g++ -std=c++11 -pthread -Ofast -o ntest nnet.o

nlib: nnet.cpp nnet.h
	g++ -std=c++11 -pthread -Ofast -c nnet.cpp nnet.h

clean:
	rm *.o
