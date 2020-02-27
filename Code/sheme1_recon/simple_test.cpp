/*
Very simple test file that takes in a single json of shares and attempts to reconstruct the first t elements
*/
#include <NTL/ZZ_p.h>
#include <NTL/ZZ.h>
#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>
#include <string>

using namespace std;
using namespace NTL;
using json = nlohmann::json;

//TODO: change to command line arg
const long PRIME = 1000000007;
const long Q = 500000003;
const string FILENAME = "../test_cases/ss1_match.json";
const int SCHEME = 1;  //1 for scheme 1 else scheme 2
const int t = 10;
const long k2 = 5;

class Share
{
	public:
	ZZ_p id;
	ZZ_p SS;
	ZZ_p SS_mac;
	Share(){}
};

void readFile(string filename,Share shares[], int size) //Reads a single file and makes an array of Shares out of it
{	
	std::ifstream shares_file(filename);
	json temp;
	shares_file >> temp;

	for(int i = 0;i<size;i++)
	{
		shares[i].id=temp[i]["id"];
		std::string str = temp[i]["SS"];
		shares[i].SS= atol(str.c_str());
		str = temp[i]["SS_MAC"];
		shares[i].SS_mac= atol(str.c_str());
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

ZZ_p reconScheme2(Share shares[], int size, int mac)
{
	ZZ_p secret, temp;
	ZZ temp2; 
	secret = ZZ_p(1); 
	temp = ZZ_p(1);
	
	for(int i = 0; i<size ; i++)
	{
		{
			ZZ_pPush push;
			ZZ_p::init(ZZ(Q)); //intialize new modulus

			ZZ_p prod_in_expq = ZZ_p(1);
			ZZ_p numerator = ZZ_p(1);
			ZZ_p denominator = ZZ_p(1);
			ZZ_p converted_IDs[size];
			
			// switch modulus for shares
			for (int x=0 ; x<size ; x++)
			{
				conv(converted_IDs[x], shares[x].id);
			}

			for(int j=0; j<size; j++)
			{
				if(i != j)
				{
					numerator *= converted_IDs[j];
					denominator *= (converted_IDs[j] - converted_IDs[i]); 
				}
			}
			prod_in_expq = numerator / denominator;
			temp2 = conv<ZZ>(prod_in_expq);
		}
		
		if(mac == 1)
		{
			temp = NTL::power(shares[i].SS_mac, temp2);
			secret *= temp;
		}
		else
		{
			temp = NTL::power(shares[i].SS, temp2);
			secret *= temp;
		}
	}
	return secret;
}

int in_intersection(Share shares[], int t, long k2) //returns true/false if reconstructed succesfully
{
	ZZ_p secret, secret_mac, k2_secret, k2_mac;
	if(SCHEME == 1)
	{
		secret=reconScheme1(shares,t,0);
		secret_mac = reconScheme1(shares,t,1);
	}
	else
	{
		secret=reconScheme2(shares,t,0);
		secret_mac = reconScheme2(shares,t,1);
	}
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
		if (SCHEME == 1)
		{
			cout<<"H(x)=" << reconScheme1(shares,t,0) << endl;
		}
		else
		{
			cout<<"H(x)=" << reconScheme2(shares,t,0) << endl;
		}	
	}
	else
	{
		cout<< "It didn't reconstruct\n";
	}

	return 0;
}