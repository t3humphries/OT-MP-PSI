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
const string FILENAME = "../test_cases/ss2_match.json";
const int SCHEME = 2;  //1 for scheme 1 else scheme 2
const int t = 10;
const long k2 = 5;

ZZ placeholder; // placeholder for conversions

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

ZZ_p reconScheme2(Share sharesp[], int size, int mac)
{
	ZZ_p::init(ZZ(PRIME));
	ZZ_p prod_in_plain, __term; 
	// ZZ_p prod_in_exp; // prod in plain is the product not in the exponent, while prod in exp is the product in the exponent. Both are intialized to 1
	prod_in_plain = ZZ_p(1); 
	// prod_in_exp = 1;
	__term = ZZ_p(1);
	// ZZ_p converted[size];
	
	for(int i = 0; i<size ; i++)
	{
		{
			ZZ_pPush push;//(ZZ(Q));
			// ZZ q;
			// q=Q;
			ZZ_p::init(ZZ(Q)); //intialize new modulus
			ZZ_p prod_in_expq = ZZ_p(1);
			ZZ_p converted[size];

			// convert prod_in_exp to the new modulus

			// placeholder = conv<ZZ>(prod_in_exp);
			// prod_in_expq = conv<ZZ_p>(placeholder);

			
			// switch modulus for shares
			for (int x=0 ; x<size ; x++)
			{
				// placeholder = conv<ZZ>(sharesp[x].id);
				// converted[x] = conv<ZZ_p>(placeholder);
				conv(converted[x], sharesp[x].id);
			}

			for(int j=0; j<size; j++)
			{
				if(i != j)
				{
					prod_in_expq *= converted[j] / (converted[j] - converted[i]);  //perform the product in the exponent
				}
			}
			// cout << prod_in_expq.modulus() << endl;
			placeholder = conv<ZZ>(prod_in_expq);
			// prod_in_exp = conv<ZZ_p>(placeholder);

		}
		

		if(mac == 1)
		{
			// placeholder = conv<ZZ>(prod_in_exp);
			// cout << sharesp[i].SS_mac << endl;
			__term = NTL::power(sharesp[i].SS_mac, placeholder);
			prod_in_plain *= __term;
		}
		else
		{
			// placeholder = conv<ZZ>(prod_in_exp);
			// cout << placeholder << endl;
			__term = NTL::power(sharesp[i].SS, placeholder);
			prod_in_plain *= __term;
		}
		// prod_in_exp = 1;
	}

	return prod_in_plain;
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

	cout << secret << endl;

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
			cout<<"H(x)="<<reconScheme1(shares,t,0);
		}
		else
		{
			cout<<"H(x)="<<reconScheme2(shares,t,0);
		}	
	}
	else
	{
		cout<< "It didn't reconstruct\n";
	}

	return 0;
}