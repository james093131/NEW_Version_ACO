all:
	g++ -Wall -O3 -o PR_ACO main.cpp

clean:
	rm -f main *.o
