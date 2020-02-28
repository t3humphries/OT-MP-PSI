#include <NTL/ZZ_p.h>
#include <iostream>
#include <gmp.h>    // gmp is included implicitly
#include <libhcs.h> // master header includes everything
#include<fstream>
#include <sstream>
#include <string>

// class Share;
struct Share {
	NTL::ZZ id;
	NTL::ZZ bin;
	NTL::ZZ SS;
	NTL::ZZ SS_mac;
};

std::string ZZ_to_str(NTL::ZZ zz);
NTL::ZZ_p hash_(NTL::ZZ_p x, NTL::ZZ p);
void ZZ_p_to_mpz_t(mpz_t __out, NTL::ZZ_p& num);
void mpz_t_to_ZZ_p(NTL::ZZ_p& __out, mpz_t num);
Share ShareGen_1(
    NTL::ZZ p, NTL::ZZ_p g,
    NTL::ZZ id, NTL::ZZ X, int t,
    NTL::ZZ key, NTL::ZZ key_mac,
    NTL::ZZ randoms[], NTL::ZZ randoms_mac[]
);
Share ShareGen_2(
    NTL::ZZ p, NTL::ZZ q,
    NTL::ZZ id, NTL::ZZ X, int t,
    NTL::ZZ key, NTL::ZZ key_mac,
    NTL::ZZ randoms[], NTL::ZZ randoms_mac[]
);