#ifndef __PSI_UTILS__
#define __PSI_UTILS__

#include <NTL/ZZ_p.h>
#include <NTL/ZZ.h>
#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <time.h>
#include <vector>
#include <gmp.h>
#include <libhcs.h> // master header includes everything
#include <string>
#include <stdlib.h>

using namespace std;
using namespace NTL;
using json = nlohmann::json;

std::string ZZ_to_str(NTL::ZZ zz);
NTL::ZZ str_to_ZZ(std::string str);
NTL::ZZ read_prime(int bitsize);
NTL::ZZ read_generator(int bitsize);
string pcs_to_str(pcs_public_key* pk);
ZZ_p hash_(ZZ x, ZZ p);
void ZZ_to_mpz_t(mpz_t __out, ZZ __temp_ZZ);
void ZZ_p_to_mpz_t(mpz_t __out, ZZ_p& num);
void mpz_t_to_ZZ(ZZ& __out, mpz_t num);
void mpz_t_to_ZZ_p(ZZ_p& __out, mpz_t num);

pcs_public_key* str_to_pcs(string str);
void str_to_mpz_t(mpz_t toReturn, string str);
string mpz_t_to_str(mpz_t num);

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
    ContextScheme1(){}
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
    ContextScheme2(){}
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

class Scheme1_Round1_receive{ //TODO: try to pass ZZ_p
    public:
    ZZ masked_secret_alpha;
    vector<ZZ> masked_coefficients_alpha;
    ZZ masked_mac_alpha;
    vector<ZZ> masked_mac_coefficients_alpha;
    Scheme1_Round1_receive(string str);
    string toString();
};

class Scheme1_Round1_send{
    public:
    ZZ h_x_alpha; 
    ZZ g_alpha;
    Scheme1_Round1_send(string str);
    string toString();
};

class Scheme1_Round2_send{
    public:
    pcs_public_key *pk;
    int id;
    mpz_t mpz_secret;
    mpz_t mpz_mac;
    mpz_t* mpz_coefficients;
    mpz_t* mpz_mac_coefficients;
    Scheme1_Round2_send(){}
    Scheme1_Round2_send(int t, pcs_public_key *__pk, int __id);
    Scheme1_Round2_send(string str);
    string toString();
};

#endif