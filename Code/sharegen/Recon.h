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

class Share //TODO: use the standard share class from psi_utils.h
{
	public:
	ZZ_p id;
	ZZ_p SS;
	ZZ_p SS_mac;
	Share(){}
};

ZZ_p reconScheme1(Share shares[], int size, int mac); //mac=1 to recon SS_mac and mac=0 to recon SS
ZZ_p reconScheme2(Share shares[], int size, int mac);
int in_intersection(Share shares[], int t, long k2); //returns true/false if reconstructed succesfully

//TODO: a function that takes a list of vectors as input for each use (the bins for each user)
// 		and returns the result of the recons.