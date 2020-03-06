#include "psi_utils.h"

using namespace std;
using namespace NTL;
using json = nlohmann::json;


Share::Share(ZZ id_in, ZZ bin_in, ZZ SS_in, ZZ SS_mac_in){
	id = id_in;
	bin = bin_in;
	SS = SS_in;
	SS_mac = SS_mac_in;
}

Share::Share(ZZ id_in, ZZ bin_in, ZZ p){
	id = id_in;
	bin = bin_in;
	ZZ_p::init(p);
	SS = rep(random_ZZ_p());
	SS_mac = rep(random_ZZ_p());
}

string ZZ_to_str(ZZ zz){
    std::stringstream ssa;
    ssa << zz;
    return ssa.str();
}

KeyholderContext::KeyholderContext(int __t, int __key, int __key_mac, NTL::ZZ __rands[], NTL::ZZ __rands_mac[]){
    t = __t;
    key = NTL::ZZ(__key);
    key_mac = NTL::ZZ(__key_mac);
    randoms = __rands;
    randoms_mac = __rands_mac;
}

void KeyholderContext::initialize_context(NTL::ZZ q, int __t){
    t = __t;
    NTL::ZZ_p::init(q);
    key = NTL::rep(NTL::random_ZZ_p());
    key_mac = NTL::rep(NTL::random_ZZ_p());
    randoms = new NTL::ZZ[t];
    randoms_mac = new NTL::ZZ[t];
    for (int i=0;i<t;i++){
        randoms[i] = NTL::rep(NTL::random_ZZ_p());
        randoms_mac[i] = NTL::rep(NTL::random_ZZ_p());
    }
}

void KeyholderContext::initialize_from_file(std::string filename){
    //TODO: initialize the values from a file, json formats
}

void KeyholderContext::write_to_file(std::string filename){
    json jsonFile;
    ofstream outputFile(filename);
    jsonFile["t"] = t;
    jsonFile["key"] = ZZ_to_str(key);
    jsonFile["key_mac"] = ZZ_to_str(key_mac);
    for(int i = 0 ; i < t ; i++)
    {
        jsonFile["randoms"][i] = ZZ_to_str(randoms[i]);
        jsonFile["randoms_mac"][i] = ZZ_to_str(randoms_mac[i]);
    }
    outputFile << jsonFile;
    outputFile.close();
 }