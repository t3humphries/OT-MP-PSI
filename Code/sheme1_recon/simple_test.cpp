/*
Very simple test file that takes in a single json of shares and attempts to reconstruct the first t elements
*/
#include <NTL/ZZ_p.h>
#include <NTL/ZZ.h>
#include "nlohmann/json.hpp"
#include <fstream>

using namespace std;
using namespace NTL;
using json = nlohmann::json;

//TODO: change to command line arg
const long PRIME = 1000000007;
const string FILENAME = "../prototype/test_cases/ss1_match.json";
const int t = 10;
const long k2 = 5;

class Share
{
	public:
	ZZ_p id;
	ZZ_p SS;
	ZZ_p SS_mac;
	Share(){}
	Share(ZZ_p identity, ZZ_p SShare, ZZ_p SShare_mac)
	{
		id = identity;
		SS = SShare;
		SS_mac = SShare_mac;
	}

};


void readFile(string filename,Share shares[], int size) //Reads a single file and makes an array of Shares out of it
{	
	std::ifstream shares_file(filename);
	json temp;
	shares_file >> temp;
	
	for(int i = 0;i<size;i++)
	{
		shares[i].id=temp[i]["id"];
		shares[i].SS=temp[i]["SS"];
		shares[i].SS_mac=temp[i]["SS_MAC"];
	}
}

ZZ_p reconScheme1(Share shares[], int size, int mac) //mac=1 to recon SS_mac and mac=0 to recon SS
{
	ZZ_p numerator, denominator, secret;
	numerator = 1;
	denominator = 1;
	secret = 0;

	for(int i = 0; i<size; i++)
	{
		for(int j = 0; j<size; j++)
		{
			if(j != i)
			{
				numerator *= shares[j].id;
				denominator *= (shares[j].id - shares[i].id); 
			}
		}

		if(mac==1)
		{
			secret += shares[i].SS_mac * (numerator / denominator);
		}
		else
		{
			secret += shares[i].SS * (numerator / denominator);	
		}

		numerator = 1;
		denominator = 1;
	}
	return secret; 
}

int in_intersection(Share shares[], int t, long k2 ) //returns true/false if reconstructed succesfully
{
	ZZ_p secret, secret_mac, k2_secret;

	secret=reconScheme1(shares,t,0);
	secret_mac = reconScheme1(shares,t,1);

	k2_secret = power(secret,k2);

	return k2_secret == secret_mac;
}
int main()
{
	//Initialize modulus 
	ZZ p;
	p = PRIME;
	ZZ_p::init(p);

	//parse json into an array of shares
	Share shares[t];
	readFile(FILENAME,shares,t); 

	//Quick tests
	int reconstruct = in_intersection(shares,t,k2);
	if(reconstruct)
	{
		cout<< "It reconstruted!\n";
		cout<<"H(x)="<<reconScheme1(shares,t,0);
	}
	else
	{
		cout<< "It didn't reconstruct\n";
	}

	return 0;
}