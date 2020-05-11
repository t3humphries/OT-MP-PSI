benchmark: benchmark.o Elementholder.o Keyholder.o Recon.o client.o psi_utils.o
	g++ -fopenmp -pthread -L../libhcs/lib -std=c++11 benchmark.o Elementholder.o Keyholder.o Recon.o client.o psi_utils.o -o benchmark -lntl -lgmp -lhcs 

benchmark.o: benchmark.cpp 
	g++ -I../libhcs/include -c -pthread -std=c++11 benchmark.cpp  -lntl -lgmp -lhcs

Elementholder.o: ../client/Elementholder.h ../client/Elementholder.cpp ../server/Keyholder.h ../server/Keyholder.cpp ../global/psi_utils.cpp ../global/psi_utils.h 
	g++ -I../libhcs/include -c -pthread -std=c++11 ../client/Elementholder.cpp  -lntl -lgmp -lhcs

Keyholder.o: ../server/Keyholder.h ../server/Keyholder.cpp
	g++ -I../libhcs/include -c -pthread -std=c++11 ../server/Keyholder.cpp  -lntl -lgmp -lhcs

Recon.o: ../client/Recon.cpp ../client/Recon.h 
	g++ -fopenmp -pthread -I../libhcs/include -c -std=c++11 ../client/Recon.cpp -lntl -lgmp -lhcs

client.o: ../client/client.cpp ../client/client.h 
	g++ -c -pthread -std=c++11 ../client/client.cpp -lntl -lgmp -lhcs

psi_utils.o : ../global/psi_utils.cpp ../global/psi_utils.h
	g++ -pthread -I../libhcs/include -c -std=c++11 ../global/psi_utils.cpp -lntl -lgmp -lhcs

clean:
	rm *.o benchmark
