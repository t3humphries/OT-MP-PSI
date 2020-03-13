#ifndef __ELEMENTHOLDER_H__
#define __ELEMENTHOLDER_H__

#include <NTL/ZZ_p.h>
#include <iostream>
#include <gmp.h>    // gmp is included implicitly
#include <libhcs.h> // master header includes everything
#include<fstream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include "psi_utils.h"
#include "Keyholder.h"

using namespace std;
using namespace NTL;

class Scheme1_Round2_output{
    public:
    mpz_t mpz_secret;
    mpz_t mpz_mac;
    mpz_t* mpz_coefficients;
    mpz_t* mpz_mac_coefficients;
    Scheme1_Round2_output(){}
    Scheme1_Round2_output(int t);
};

class Elementholder{
    public:
    int id;
    int* elements;
    int num_elements;
    ZZ alpha;
    ZZ alpha_inv;
    pcs_public_key *pk;
    pcs_private_key *vk;
    hcs_random *hr;

    Elementholder(){}
    Elementholder(int __id, int* __elements, int __num_elements);

    void Scheme1_Round1(ZZ *h_x_alpha, ZZ *g_alpha, ContextScheme1 public_context, int __X);
    Scheme1_Round2_output Scheme1_Round2(ContextScheme1 context, Scheme1_Round1_output out);
    void Scheme1_Final(ZZ secret_share, ZZ mac_share, mpz_t __mpz_secret, mpz_t __mpz_mac);

    Share get_share(ContextScheme1 context, int __X, Keyholder k, int num_bins);

};

#endif