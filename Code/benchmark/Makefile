output: benchmark.o ShareGen.o Recon.o psi_utils.o
	g++ -std=c++11 -pthread  benchmark.o ShareGen.o Recon.o psi_utils.o -o output -lntl -lgmp -lhcs

benchmark.o: benchmark.cpp 
	g++ -c -std=c++11 -pthread benchmark.cpp  -lntl -lgmp -lhcs

ShareGen.o: ../sharegen/ShareGen.h ../sharegen/ShareGen.cpp ../sharegen/psi_utils.cpp ../sharegen/psi_utils.h
	g++ -c -std=c++11 -pthread ../sharegen/ShareGen.cpp  -lntl -lgmp -lhcs

Recon.o: ../sharegen/Recon.cpp ../sharegen/Recon.h 
	g++ -c -std=c++11 -pthread ../sharegen/Recon.cpp -lntl -lgmp -lhcs

psi_utils.o : ../sharegen/psi_utils.cpp ../sharegen/psi_utils.h
	g++ -c -std=c++11 -pthread ../sharegen/psi_utils.cpp -lntl -lgmp -lhcs







clean:
	rm *.o output