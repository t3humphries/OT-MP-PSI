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

// class Config{
//     public:
//     int m;
//     int num_bins;
//     int max_bin_size;
//     int t;
// };

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
};

class KeyholderContext{
    public:
    NTL::ZZ key;
    NTL::ZZ key_mac;
    NTL::ZZ* randoms;
    NTL::ZZ* randoms_mac;
    KeyholderContext(){}
    KeyholderContext(int __key, int __key_mac, NTL::ZZ __rands[], NTL::ZZ __rands_mac[]){
        key = NTL::ZZ(__key);
        key_mac = NTL::ZZ(__key_mac);
        randoms = __rands;
        randoms_mac = __rands_mac;
    }

    void initialize_context(NTL::ZZ q, int t){
        NTL::ZZ_p::init(q);
        key = NTL::ZZ(4);//TODO
        key_mac = NTL::ZZ(5);//TODO
        randoms = new NTL::ZZ[t];
        randoms_mac = new NTL::ZZ[t];
        for (int i=0;i<t;i++){
            randoms[i] = NTL::rep(NTL::random_ZZ_p());
            randoms_mac[i] = NTL::rep(NTL::random_ZZ_p());
        }
        //TODO: generate a random context mod a prime modulus

    }

    void initialize_from_file(std::string filename){
        //TODO: initialize the values from a file, json formats
    }

    void write_to_file(std::string filename){
        //TODO: write the values to a file, json format
    }
};

#endif