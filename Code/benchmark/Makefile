output: benchmark.o 
	g++ -std=c++11 -pthread  benchmark.o -o output -lntl -lgmp -lhcs

benchmark.o: benchmark.cpp ../sharegen/ShareGen.h
	g++ -c -std=c++11 -pthread ../sharegen/ShareGen.cpp ../sharegen/Recon.cpp benchmark.cpp  -lntl -lgmp -lhcs

clean:
	rm *.o output