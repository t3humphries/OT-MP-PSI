#ifndef __UTILS__
#define __UTILS__

#include <NTL/ZZ_p.h>
#include <NTL/ZZ.h>
#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include <vector>

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
};

class KeyholderContext{
    public:
    NTL::ZZ key;
    NTL::ZZ key_mac;
    NTL::ZZ randoms[];
    NTL::ZZ randoms_mac[];
    KeyholderContext(){}

    void initialize_context(NTL::ZZ q){
        NTL::ZZ_p::init(q);
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