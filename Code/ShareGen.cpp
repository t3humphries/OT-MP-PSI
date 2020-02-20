#include <NTL/ZZ_p.h>
#include <iostream>
#include <gmp.h>    // gmp is included implicitly
#include <libhcs.h> // master header includes everything
#include <sstream>
#include <string>

// #include "ShareGen.h"

using namespace std;
using namespace NTL;

struct Share {
	ZZ id;
	ZZ bin;
	ZZ_p SS;
	ZZ_p SS_mac;
};

ZZ_p hash_(ZZ x, ZZ p){
	ZZ_p::init(p);
	hash<string> ptr_hash;
	std::stringstream ssa;
	ssa << x;
	return ZZ_p(ptr_hash(ssa.str()));
}

void ZZ_p_to_mpz_t(mpz_t __out, ZZ_p& num){
	ZZ __temp_ZZ;
	__temp_ZZ = rep(num);
	std::stringstream ssa;
	ssa << __temp_ZZ;
	mpz_set_str( __out, ssa.str().c_str(), 10);
}

void mpz_t_to_ZZ_p(ZZ_p& __out, mpz_t num){
	ZZ __temp_ZZ;
	std::stringstream __ssa;
	char __temp[1000];
	mpz_get_str(__temp, 10, num);
	__ssa << __temp;
	__ssa >> __temp_ZZ;
	conv(__out, __temp_ZZ);
}


Share ShareGen_1(ZZ p, ZZ_p g, ZZ id, ZZ X, int t, ZZ key, ZZ key_mac, ZZ randoms[], ZZ randoms_mac[]){

	ZZ_p::init(p);
	// Element Holder

	ZZ_p h_x = hash_(X, p);
	ZZ alpha, alpha_inv;
	{
		ZZ_pPush push(p-1);
		ZZ_p __temp = random_ZZ_p();
		while  (GCD(rep(__temp), ZZ(p-1)) > 1) {
			__temp = random_ZZ_p();
		}
		alpha = rep(__temp);
		__temp = 1 / __temp;
		alpha_inv = rep(__temp);
	}

	ZZ_p h_x_alpha = NTL::power(h_x, alpha);
	ZZ_p g_alpha = NTL::power(g, alpha);
	
	// Key Holder

	ZZ r;
	ZZ_p R, R_inverse, R_alpha;
	{
		ZZ_pPush push(p-1);
		ZZ_p __temp = random_ZZ_p();
		r = rep(__temp);
	}

	R = NTL::power(g, r);
	// R_inverse = NTL::power(g, -r);
	R_inverse = 1 / R;
	R_alpha = NTL::power(g_alpha, r);

    ZZ_p masked_secret_alpha = R_alpha * NTL::power(h_x_alpha, key);
    ZZ_p masked_coefficients_alpha[t-1];
	for (int i = 0; i < t-1; i++){
		masked_coefficients_alpha[i] = R_alpha * NTL::power(h_x_alpha, randoms[i]);
	}

    ZZ_p masked_mac_alpha = R_alpha * NTL::power(h_x_alpha, key * key_mac);
    ZZ_p masked_mac_coefficients_alpha[t-1];
	for (int i = 0; i < t-1; i++){
		masked_mac_coefficients_alpha[i] = R_alpha * NTL::power(h_x_alpha, randoms_mac[i]);
	}

	// Element Holder

	ZZ_p masked_secret = NTL::power(masked_secret_alpha, alpha_inv);
	ZZ_p masked_coefficients[t-1];
	for (int i=0; i < t-1; i++){
		masked_coefficients[i] = NTL::power(masked_coefficients_alpha[i], alpha_inv);
	}

	ZZ_p masked_mac = NTL::power(masked_mac_alpha, alpha_inv);
	ZZ_p masked_mac_coefficients[t-1];
	for (int i=0; i < t-1; i++){
		masked_mac_coefficients[i] = NTL::power(masked_mac_coefficients_alpha[i], alpha_inv);
	}

	mpz_t __mpz_secret;
	mpz_t __mpz_coefficients[t-1];
	mpz_init(__mpz_secret);
	ZZ_p_to_mpz_t(__mpz_secret, masked_secret);
	for (int i=0;i<t-1;i++){
		mpz_init(__mpz_coefficients[i]);
		ZZ_p_to_mpz_t(__mpz_coefficients[i], masked_coefficients[i]);
	}

	mpz_t __mpz_mac;
	mpz_init(__mpz_mac);
	mpz_t __mpz_mac_coefficients[t-1];
	ZZ_p_to_mpz_t(__mpz_mac, masked_mac);
	for (int i=0;i<t-1;i++){
		mpz_init(__mpz_mac_coefficients[i]);
		ZZ_p_to_mpz_t(__mpz_mac_coefficients[i], masked_mac_coefficients[i]);
	}

	// initialize data structures
    pcs_public_key *pk = pcs_init_public_key();
    pcs_private_key *vk = pcs_init_private_key();
    hcs_random *hr = hcs_init_random();

    // Generate a key pair with modulus of size 2048 bits
    pcs_generate_key_pair(pk, vk, hr, 2048 + 80);

    pcs_encrypt(pk, hr, __mpz_secret, __mpz_secret);
	pcs_encrypt(pk, hr, __mpz_mac, __mpz_mac);
	for (int i=0;i<t-1;i++){
		pcs_encrypt(pk, hr, __mpz_coefficients[i], __mpz_coefficients[i]);
		pcs_encrypt(pk, hr, __mpz_mac_coefficients[i], __mpz_mac_coefficients[i]);
	}

	// Key Holder

	mpz_t __mpz_temp;
	mpz_init(__mpz_temp);
	
	ZZ_p_to_mpz_t(__mpz_temp, R_inverse);
	pcs_ep_mul(pk, __mpz_secret, __mpz_secret, __mpz_temp);
	pcs_ep_mul(pk, __mpz_mac, __mpz_mac, __mpz_temp);
	
	ZZ_p R_inv_id_pows = to_ZZ_p(id) * R_inverse;
	for (int i=0;i<t-1;i++){
		ZZ_p_to_mpz_t(__mpz_temp, R_inv_id_pows);
		pcs_ep_mul(pk, __mpz_coefficients[i], __mpz_coefficients[i], __mpz_temp);
		pcs_ep_mul(pk, __mpz_mac_coefficients[i], __mpz_mac_coefficients[i], __mpz_temp);
		R_inv_id_pows = R_inv_id_pows * to_ZZ_p(id);

		pcs_ee_add(pk, __mpz_secret, __mpz_secret, __mpz_coefficients[i]);
		pcs_ee_add(pk, __mpz_mac, __mpz_mac, __mpz_mac_coefficients[i]);
	}

	// Element Holder

	pcs_decrypt(vk, __mpz_secret, __mpz_secret);	
	pcs_decrypt(vk, __mpz_mac, __mpz_mac);

	ZZ_p secret_share, mac_share;
	mpz_t_to_ZZ_p(secret_share, __mpz_secret);
	mpz_t_to_ZZ_p(mac_share, __mpz_mac);

	Share ans = {
		.id = id,
		.bin = ZZ(1),
		.SS = secret_share,
		.SS_mac = mac_share
	};
	return ans;
}

