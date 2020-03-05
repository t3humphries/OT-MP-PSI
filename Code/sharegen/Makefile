output: main.o ShareGen.o
	g++ -std=c++11 -pthread  main.o ShareGen.o -o output -lntl -lgmp -lhcs

main.o: main.cpp
	g++ -c -std=c++11 -pthread main.cpp  -lntl -lgmp -lhcs


ShareGen.o: ShareGen.cpp ShareGen.h
	g++ -c -std=c++11 -pthread ShareGen.cpp -lntl -lgmp -lhcs


clean:
	rm *.o output