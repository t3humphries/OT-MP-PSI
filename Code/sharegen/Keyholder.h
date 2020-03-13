#ifndef __KEYHOLDER_H__
#define __KEYHOLDER_H__

#include <NTL/ZZ_p.h>
#include <iostream>
#include <gmp.h>    // gmp is included implicitly
#include <libhcs.h> // master header includes everything
#include<fstream>
#include <sstream>
#include <string>
#include <vector>
#include "psi_utils.h"

using namespace std;
using namespace NTL;

class Scheme1_Round1_output{ //TODO: try to pass ZZ_p
    public:
    ZZ masked_secret_alpha;
    vector<ZZ> masked_coefficients_alpha;
    ZZ masked_mac_alpha;
    vector<ZZ> masked_mac_coefficients_alpha;
};

class Keyholder{
	public:
	ContextScheme1 public_context;
    ContextScheme2 context2;
	ZZ key;
	ZZ key_mac;
	ZZ* randoms;
	ZZ* randoms_mac;
	ZZ r, __R, __R_inverse;

    // Keyholder(){}
    // Keyholder(ContextScheme1 __c1, int __key, int __key_mac, ZZ __rands[], ZZ __rands_mac[]);
    Keyholder(ContextScheme1 __c1, ZZ __key, ZZ __key_mac, ZZ __rands[], ZZ __rands_mac[]);
    Keyholder(ContextScheme2 __c2, ZZ __key, ZZ __key_mac, ZZ __rands[], ZZ __rands_mac[]);
	Keyholder(ContextScheme1 __c1);
	Keyholder(ContextScheme2 __c2);
    void initialize_context(ContextScheme1 __c1);
    void initialize_context(ContextScheme2 __c2);
	Scheme1_Round1_output Scheme1_Round1(ZZ __h_x_alpha, ZZ __g_alpha);
	void Scheme1_Round2(
        pcs_public_key *pk, int id,
        mpz_t __mpz_secret,
        mpz_t __mpz_mac,
        mpz_t* __mpz_coefficients,
        mpz_t* __mpz_mac_coefficients
    );

    void Scheme2_Round1(ZZ *secret_share_alpha, ZZ *mac_share_alpha, ContextScheme2 context, ZZ h_x_alpha, int idd);

};

#endif