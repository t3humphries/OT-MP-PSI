
#include <NTL/ZZ_p.h>
#include <NTL/ZZ.h>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include <vector>
#include "Recon.h"

using namespace std;
using namespace NTL;

class Combinations {
public:
    Combinations(vector<int> elems, int n, int r)
        : elements{elems}
        , n{n}
        , r{r}
    {}

    bool next() {
        int lastNotEqualOffset = r - 1;
        while (elements[lastNotEqualOffset] == n - r + (lastNotEqualOffset + 1)) {
            --lastNotEqualOffset;
        }
        if (lastNotEqualOffset < 0) {
            return false;
        }
        ++elements[lastNotEqualOffset];
        for (int i = lastNotEqualOffset + 1; i < r; ++i) {
            elements[i] = elements[lastNotEqualOffset] + (i - lastNotEqualOffset);
        }
        return true;
    }
    std::vector<int> getElements()
    {
        return elements;
    }

private:
    vector<int> elements;
    int n;
    int r;
};

int incBinIndexs(vector<int> &binIndexs, int t, int binSize)
{
	int i = t;
	do
	{
		if(i == 0)
		{
			return 0;
		}
		i = i - 1;
		binIndexs[i] = (binIndexs[i] + 1) % binSize;
	} while(binIndexs[i] == 0);
	return 1;
}

ZZ_p reconScheme1(vector<Share> shares, Context context, int mac) //mac=1 to recon SS_mac and mac=0 to recon SS
{
	
	ZZ_p numerator, denominator, secret;
	numerator = ZZ_p(1);
	denominator = ZZ_p(1);
	secret = ZZ_p(0);

	for(int i = 0; i<context.t; i++)
	{
		for(int j = 0; j<context.t; j++)
		{
			if(j != i)
			{
				numerator *= conv<ZZ_p>(shares[j].id);
				denominator *= conv<ZZ_p>(shares[j].id - shares[i].id); 
			}
		}

		if(mac==1)
		{
			secret += conv<ZZ_p>(shares[i].SS_mac) * (numerator / denominator);
		}
		else
		{
			secret += conv<ZZ_p>(shares[i].SS) * (numerator / denominator);	
		}

		numerator = 1;
		denominator = 1;
	}
	return secret; 
}

ZZ_p reconScheme2(vector<Share> shares, Context context, int mac)
{
	ZZ_p secret, temp;
	ZZ temp2; 
	secret = ZZ_p(1); 
	temp = ZZ_p(1);
	
	for(int i = 0; i<context.t ; i++)
	{
		{
			ZZ_pPush push;
			ZZ_p::init(ZZ(context.q)); //intialize new modulus

			ZZ_p prod_in_expq = ZZ_p(1);
			ZZ_p numerator = ZZ_p(1);
			ZZ_p denominator = ZZ_p(1);
			ZZ_p converted_IDs[context.t];
			
			// switch modulus for shares
			for (int x=0 ; x<context.t ; x++)
			{
				conv(converted_IDs[x], shares[x].id);//TODO might not need this anymore since ZZ
			}

			for(int j=0; j<context.t; j++)
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
			temp = NTL::power(conv<ZZ_p>(shares[i].SS_mac), temp2);
			secret *= temp;
		}
		else
		{
			temp = NTL::power(conv<ZZ_p>(shares[i].SS), temp2);
			secret *= temp;
		}
	}
	return secret;
}

//Main Logic: takes in m * max_bin_size "matrix" of Shares, outputs a list of what reconstructed
vector<ZZ> recon1_in_bin_x(vector<vector<Share>> shares, Context context, int m, int max_bin_size){

	ZZ_p::init(ZZ(context.p));
	vector<ZZ> toReturn;
	ZZ_p secret, mac;
	//Initialize first combination (first t bins)
	vector<int> startingPoint(context.t);
	for(int i = 0 ; i < context.t ; i++)
	{
		startingPoint[i] = i;
	} 

    Combinations comb{startingPoint, m-1, context.t};
    vector<int> chosenUsers;
    vector<int> binIndexs(max_bin_size);
    //For each combinations of users do recon on the users in chosen indexs
    do {

        chosenUsers=comb.getElements();
  
		//Initialize the bin indexs for this combination of users
		
		for(int i = 0 ; i < max_bin_size ; i++)
		{
			binIndexs[i] = 0;
		} 

		do{

			//Do recon on chosen users bins (from chosenUsers) using the an element from each bin (from binIndexs)
			vector<Share> toRecon(context.t);
			for(int i = 0 ; i < context.t ; i++ )
			{
				toRecon[i] = shares[chosenUsers[i]][binIndexs[i]];
			}
			secret = reconScheme1(toRecon, context, 0);
			mac = reconScheme1(toRecon, context, 1);
			if(secret == mac) //If recontructs add to the list toReturn
			{
				bool alreadyFound = false;
				for (int k=0;k<toReturn.size();k++){
					if (toReturn[k] == rep(secret)){
						alreadyFound=true; break;
					}
				}
				if (!alreadyFound) toReturn.push_back(rep(secret));
			}

		}while(incBinIndexs(binIndexs,context.t,max_bin_size));

    } while (comb.next());

	return toReturn;

}

vector<ZZ> recon2_in_bin_x(vector<vector<Share>> shares, Context context, int m, int max_bin_size){

	ZZ_p::init(ZZ(context.p));
	vector<ZZ> toReturn;
	ZZ_p secret, mac;
	//Initialize first combination (first t bins)
	vector<int> startingPoint(context.t);
	for(int i = 0 ; i < context.t ; i++)
	{
		startingPoint[i] = i;
	} 

    Combinations comb{startingPoint, m-1, context.t};
    vector<int> chosenUsers;
    vector<int> binIndexs(max_bin_size);
    //For each combinations of users do recon on the users in chosen indexs
    do {

        chosenUsers=comb.getElements();
  
		//Initialize the bin indexs for this combination of users
		
		for(int i = 0 ; i < max_bin_size ; i++)
		{
			binIndexs[i] = 0;
		} 

		do{

			//Do recon on chosen users bins (from chosenUsers) using the an element from each bin (from binIndexs)
			vector<Share> toRecon(context.t);
			for(int i = 0 ; i < context.t ; i++ )
			{
				toRecon[i] = shares[chosenUsers[i]][binIndexs[i]];
			}
			secret = reconScheme2(toRecon, context, 0);
			mac = reconScheme2(toRecon, context, 1);
			if(secret == mac) //If recontructs add to the list toReturn
			{
				bool alreadyFound = false;
				for (int k=0;k<toReturn.size();k++){
					if (toReturn[k] == rep(secret)){
						alreadyFound=true; break;
					}
				}
				if (!alreadyFound) toReturn.push_back(rep(secret));
			}

		}while(incBinIndexs(binIndexs,context.t,max_bin_size));

    } while (comb.next());

	return toReturn;

}

 //-----------------------OLD CODE------------------------------------------

 // void readFile(string filename,Share shares[], int size) //Reads a single file and makes an array of Shares out of it
// {	
// 	std::ifstream shares_file(filename);
// 	json temp;
// 	shares_file >> temp;

// 	for(int i = 0;i<size;i++)
// 	{
// 		shares[i].id=temp[i]["id"];
// 		std::string str = temp[i]["SS"];
// 		shares[i].SS= atol(str.c_str());
// 		str = temp[i]["SS_MAC"];
// 		shares[i].SS_mac= atol(str.c_str());
// 	}
// }

// int in_intersection(Share shares[], int t, long k2) //returns true/false if reconstructed succesfully
// {
// 	ZZ_p secret, secret_mac, k2_secret, k2_mac;
// 	if(SCHEME == 1)
// 	{
// 		secret=reconScheme1(shares,t,0);
// 		secret_mac = reconScheme1(shares,t,1);
// 	}
// 	else
// 	{
// 		secret=reconScheme2(shares,t,0);
// 		secret_mac = reconScheme2(shares,t,1);
// 	}
// 	k2_secret = power(secret,k2);

// 	return k2_secret == secret_mac;
// }


//   int main()
// {
// 	//Initialize modulus 
// 	ZZ p;
// 	p = PRIME;
// 	ZZ_p::init(p);

// 	//parse json into an array of shares
// 	Share shares[t];
// 	readFile(FILENAME,shares,t); 

// 	//Quick tests
// 	int reconstruct = in_intersection(shares,t,k2);
// 	if(reconstruct)
// 	{
// 		cout<< "It reconstruted!\n";
// 		if (SCHEME == 1)
// 		{
// 			cout<<"H(x)=" << reconScheme1(shares,t,0) << endl;
// 		}
// 		else
// 		{
// 			cout<<"H(x)=" << reconScheme2(shares,t,0) << endl;
// 		}	
// 	}
// 	else
// 	{
// 		cout<< "It didn't reconstruct\n";
// 	}

 // 	return 0;
 // }