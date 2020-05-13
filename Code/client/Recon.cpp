#include <NTL/ZZ_p.h>
#include <NTL/ZZ.h>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include <vector>

//#include <NTL/BasicThreadPool.h>
#include <omp.h>

#include "Recon.h"

using namespace std;
using namespace NTL;



size_t binCoeff(size_t n, size_t k) {

  if ((k==0)||(n==k)) {
    return 1;
  } else if (k>n/2) {
    return binCoeff(n, n-k);
  } else {
    return binCoeff(n-1, k-1) + binCoeff(n-1, k);
  }

  
}

class Combinations {
public:
    Combinations(vector<int> elems, int n, int r)
        : chosenUsers{elems}
        , n{n}
        , r{r}
    {
      this->s = binCoeff(n, r);
    }

  size_t size() {
    return this->s;
  }

    bool next() {
        int positionToIncrement = r - 1;
        while (chosenUsers[positionToIncrement] == n - r + (positionToIncrement + 1)) {
            --positionToIncrement;
        }
        if (positionToIncrement < 0) {
            return false;
        }
        ++chosenUsers[positionToIncrement];
        for (int i = positionToIncrement + 1; i < r; ++i) {
            chosenUsers[i] = chosenUsers[positionToIncrement] + (i - positionToIncrement);
        }
        return true;
    }
    std::vector<int> getElements()
    {
        return chosenUsers;
    }

private:
    vector<int> chosenUsers;
    int n;
    int r;
    size_t s;
};

int incBinIndices(vector<int> &binIndices, int t, int binSize)
{
	int i = t;
	do
	{
		if(i == 0)
		{
			return 0;
		}
		i = i - 1;
		binIndices[i] = (binIndices[i] + 1) % binSize;
	} while(binIndices[i] == 0);
	return 1;
}

int reconScheme1(vector<Share> shares, Context context) 
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

		secret += conv<ZZ_p>(shares[i].SS) * (numerator / denominator);

		numerator = 1;
		denominator = 1;
	}
	return secret == 0; 
}

int reconScheme2(vector<Share> shares, Context context)
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
	
		temp = NTL::power(conv<ZZ_p>(shares[i].SS), temp2);
		secret *= temp;
	}
	return secret == 1;
}

//Main Logic: takes in m * max_bin_size "matrix" of Shares, outputs a 2D binary vector containing which elements reconstructed for each user
vector<vector<int>> recon_in_bin_x(vector<vector<Share>> shares, Context context, int m, int max_bin_size, int scheme, int* count){

	ZZ_p::init(ZZ(context.p));
	vector<vector<int>> toReturn(m, vector<int>(max_bin_size,0));
	//Initialize first combination (first t bins)
	vector<int> startingPoint(context.t);
	for(int i = 0 ; i < context.t ; i++)
	{
		startingPoint[i] = i;
	} 

   //Memory is cheap
   //Store all combination is memory
    std::vector<std::vector<int>> combArray(binCoeff(m, context.t));
    Combinations comb{startingPoint, m-1, context.t};
    size_t i = 0;
    do {
      combArray[i] = comb.getElements();
      i++;
    } while(comb.next());

    //For each combinations of users do recon on the users in chosen indicies
    
    ZZ_pContext mcontext;
    mcontext.save();
    #pragma omp parallel
    {
      mcontext.restore();
      
    #pragma omp for
    for (size_t runner = 0; runner < comb.size(); runner++) {
      int reconstructed = 0;
      vector<int> chosenUsers=combArray[runner];
     	vector<int> binIndices(max_bin_size);
		//Initialize the bin indices for this combination of users
		for(int i = 0 ; i < max_bin_size ; i++)
		{
			binIndices[i] = 0;
		}
		do{
			//Do recon on chosen users bins (from chosenUsers) using the an element from each bin (from binIndices)
			vector<Share> toRecon(context.t);
			for(int i = 0 ; i < context.t ; i++ )
			{
				toRecon[i] = shares[chosenUsers[i]][binIndices[i]];
			}

			if (scheme==1){
			 
				reconstructed = reconScheme1(toRecon, context);
			}else{
				reconstructed = reconScheme2(toRecon, context);
			}

			if(reconstructed) //If reconstructed, add to the list toReturn
			{
				int alreadyFound = 0;

				#pragma omp critical
				{
				for(int j = 0; j < context.t ; j++)
				{
				  bool b;
				  b = toReturn[chosenUsers[j]][binIndices[j]];
				  
					if(b == 1)
					{
						alreadyFound = 1;
					}

					toReturn[chosenUsers[j]][binIndices[j]] = 1;
				}
				}
				
				if(!alreadyFound)
				{
				  #pragma omp atomic
					(*count)++;
				}	
			}

		} while(incBinIndices(binIndices,context.t,max_bin_size));

    }
    }
	return toReturn;

}
