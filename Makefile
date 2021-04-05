CC= g++
#CC= clang
CFLAGS= -O3 -g -lm -fopenmp -Wno-unused-function -Wno-absolute-value -std=c++11 -finline-limit=64000 -fno-math-errno

all: main

main:
	$(CC) $(CFLAGS) K9.cpp -o k9.out 

clean:
	rm -f *.exe *.out *.o
