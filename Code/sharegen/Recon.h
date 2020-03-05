
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

ZZ_p reconScheme1(vector<Share> shares, ContextScheme1 context, int mac); //mac=1 to recon SS_mac and mac=0 to recon SS
ZZ_p reconScheme2(vector<Share> shares, ContextScheme2 context, int mac);
vector<ZZ> recon1_in_bin_x(vector<vector<Share>> shares, ContextScheme1 context, int k2, int m, int max_bin_size);


