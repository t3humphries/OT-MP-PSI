output: benchmark.o Elementholder.o Keyholder.o Recon.o client.o psi_utils.o
	g++ -std=c++11 -pthread  benchmark.o Elementholder.o Keyholder.o Recon.o client.o psi_utils.o -o output -lntl -lgmp -lhcs

benchmark.o: benchmark.cpp 
	g++ -c -std=c++11 -pthread benchmark.cpp  -lntl -lgmp -lhcs

Elementholder.o: ../client/Elementholder.h ../client/Elementholder.cpp ../server/Keyholder.h ../server/Keyholder.cpp ../global/psi_utils.cpp ../global/psi_utils.h 
	g++ -c -std=c++11 -pthread ../client/Elementholder.cpp  -lntl -lgmp -lhcs

Keyholder.o: ../server/Keyholder.h ../server/Keyholder.cpp
	g++ -c -std=c++11 -pthread ../server/Keyholder.cpp  -lntl -lgmp -lhcs

Recon.o: ../client/Recon.cpp ../client/Recon.h 
	g++ -c -std=c++11 -pthread ../client/Recon.cpp -lntl -lgmp -lhcs

client.o: ../client/client.cpp ../client/client.h 
	g++ -c -std=c++11 -pthread ../client/client.cpp -lntl -lgmp -lhcs

psi_utils.o : ../global/psi_utils.cpp ../global/psi_utils.h
	g++ -c -std=c++11 -pthread ../global/psi_utils.cpp -lntl -lgmp -lhcs







clean:
	rm *.o output