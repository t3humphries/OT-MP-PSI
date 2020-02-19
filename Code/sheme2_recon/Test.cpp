#include <NTL/ZZ_p.h>
#include <NTL/ZZ.h>
#include "nlohmann/json.hpp"
#include <fstream>

using namespace std;
using namespace NTL;
using json = nlohmann::json;
const int size = 10;
long placeholder; // placeholder for conversions


class Share
{
	public:
	ZZ_p id;
	ZZ_p SS;
	ZZ_p SS_mac;
	Share(){}
	Share(const Share &oldshare)
	{
		placeholder = conv<long>(oldshare.id);
		id = conv<ZZ_p>(placeholder);
		placeholder = conv<long>(oldshare.SS);
		SS = conv<ZZ_p>(placeholder);
		placeholder = conv<long>(oldshare.SS_mac);
		SS_mac = conv<ZZ_p>(placeholder);
	}

};


void readFile(string filename, Share shares[], int size)
{
	//Read the files and make the array
	
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




ZZ_p reconScheme(Share sharesp[], int mac, int size)
{

ZZ_pPush push;

ZZ q; // Value of q given
q = 500000003;
ZZ p ; // value of p
p = 1000000007;


ZZ_p prod_in_plain, prod_in_exp, y; // y is the final answer, prod in plain is the product not in the exponent, while prod in exp is the product in the exponent. Both are intialized to 1
prod_in_plain = 1; 
prod_in_exp = 1;
ZZ_p exponent;
Share sharesq[size] ;  // this is used to store shares under base q. The shares are received by the function under base p

for(int i = 0; i<size; i++)
{



	ZZ_p::init(q); //intialize new modulus

	// convert prod_in_exp to the new modulus

	placeholder = conv<long>(prod_in_exp);
	ZZ_p prod_in_expq= conv<ZZ_p>(placeholder);

	
	// switch modulus for shares
	for (int x=0; x< size; x++)
	{
		placeholder = conv<long>(sharesp[i].id);
		sharesq[i].id = conv<ZZ_p>(placeholder);
		placeholder = conv<long>(sharesp[i].SS);
		sharesq[i].SS = conv<ZZ_p>(placeholder);
		placeholder = conv<long>(sharesp[i].SS_mac);
		sharesq[i].SS_mac = conv<ZZ_p>(placeholder);
	}

	for(int j=0; j<size; j++)
	{
		if(i!=j)
		{
		
			prod_in_expq *= sharesq[j].id / (sharesq[j].id - sharesq[i].id);  //perform the product in the exponent
			

            
		}
		
	}
	ZZ_p::init(p);   //reinitizlize the modulus p
	


	placeholder = conv<long>(prod_in_expq);
	prod_in_exp= conv<ZZ_p>(placeholder);
	
	
	// for (int x=0; x< size; x++)
	// {
	// 	placeholder = conv<long>(sharesq[i].id);
	// 	sharesp[i].id = conv<ZZ_p>(placeholder);
	// 	placeholder = conv<long>(sharesq[i].SS);
	// 	sharesp[i].SS = conv<ZZ_p>(placeholder);
	// 	placeholder = conv<long>(sharesq[i].SS_mac);
	// 	sharesp[i].SS_mac = conv<ZZ_p>(placeholder);
	// }



	if(mac == 0)
	{
		const ZZ_p share = sharesp[i].SS;
		const ZZ prod= conv<ZZ>(prod_in_exp);
		power(exponent, share,prod);

		prod_in_plain*= exponent;
	}
	else
	{
		const ZZ_p share = sharesp[i].SS_mac;
		const ZZ prod= conv<ZZ>(prod_in_exp);
		power( exponent, share,prod);

		prod_in_plain*= exponent;
	}
	prod_in_exp = 1;

	
}


y = prod_in_plain;


return y;



}


int in_intersection(Share shares[], ZZ )
{

}
int main()
{
	ZZ p;
	p = 1000000007;
	ZZ_p::init(p);
	
	string filename = "ss1_match.json";
	Share shares[size];
	readFile(filename, shares, size); // a pointer

	ZZ_p k1, k2;

	k1 = reconScheme(shares, 0, size);
	k2 = reconScheme(shares, 1, size);
	ZZ_p answer;
	long k = 5;
	power(answer, k1, k );
	cout<<"the two answers are"<<answer<<"and"<<k2;
    
	return 0;
}