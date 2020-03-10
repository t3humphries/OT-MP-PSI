#ifndef __PSI_UTILS__
#define __PSI_UTILS__

#include <NTL/ZZ_p.h>
#include <NTL/ZZ.h>
#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include <vector>



std::string ZZ_to_str(NTL::ZZ zz);
NTL::ZZ read_prime(int bitsize);
NTL::ZZ read_generator(int bitsize);

class Share{
    public:
	NTL::ZZ id;
	NTL::ZZ bin;
	NTL::ZZ SS;
	NTL::ZZ SS_mac;
    Share(){}
    Share(NTL::ZZ id_in, NTL::ZZ bin_in, NTL::ZZ SS_in, NTL::ZZ SS_mac_in);
    Share(NTL::ZZ id_in, NTL::ZZ bin_in, NTL::ZZ p); //For generating random nonsense shares for padding
};


class ContextScheme1{
    public:
    NTL::ZZ p;
    NTL::ZZ g;
    int t;
    ContextScheme1(int pp, int gg, int tt){
        p = NTL::ZZ(pp);
        g = NTL::ZZ(gg);
        t = tt;
    }

    ContextScheme1(NTL::ZZ pp, NTL::ZZ gg, int tt){
        p = NTL::ZZ(pp);
        g = NTL::ZZ(gg);
        t = tt;
    }


    void write_to_file(std::string filename){
        //write this to file
    }
};

class ContextScheme2{
    public:
    NTL::ZZ p;
    NTL::ZZ q;
    int t;
    ContextScheme2(int pp, int tt){
        p = NTL::ZZ(pp);
        q = NTL::ZZ((p-1)/2);
        t = tt;
    }
    ContextScheme2(int pp, int qq, int tt){
        p = NTL::ZZ(pp);
        q = NTL::ZZ(qq);
        t = tt;
    }
    ContextScheme2(NTL::ZZ pp, NTL::ZZ qq, int tt){
        p = NTL::ZZ(pp);
        q = NTL::ZZ(qq);
        t = tt;
    }
};

class KeyholderContext{
    public:
    int t;
    NTL::ZZ key;
    NTL::ZZ key_mac;
    NTL::ZZ* randoms;
    NTL::ZZ* randoms_mac;
    KeyholderContext(){}
    KeyholderContext(int __t, int __key, int __key_mac, NTL::ZZ __rands[], NTL::ZZ __rands_mac[]);

    void initialize_context(NTL::ZZ q, int __t);

    void initialize_from_file(std::string filename);

    void write_to_file(std::string filename);
};

#endif