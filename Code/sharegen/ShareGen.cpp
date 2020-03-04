#include "ShareGen.h"
#include "nlohmann/json.hpp"

using namespace NTL;
using namespace std;
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

ZZ_p hash_(ZZ x, ZZ p){
	ZZ_p::init(p);
	hash<string> ptr_hash;
	return ZZ_p(ptr_hash(ZZ_to_str(x)));
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

Share ShareGen_1(
    ContextScheme1 public_context, KeyholderContext keyholder_context, NTL::ZZ id, NTL::ZZ X, int num_bins){
	ZZ p = public_context.p, g = public_context.g;
	int t = public_context.t;
	ZZ_p::init(p);
	ZZ_p g_p;
	conv(g_p, g);
	//////////////////// Element Holder ////////////////////

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
	ZZ_p g_alpha = NTL::power(g_p, alpha);
	
	//////////////////// Key Holder ////////////////////

	ZZ r;
	ZZ_p R, R_inverse, R_alpha;
	{
		ZZ_pPush push(p-1);
		ZZ_p __temp = random_ZZ_p();
		r = rep(__temp);
	}

	R = NTL::power(g_p, r);
	// R_inverse = NTL::power(g, -r);
	R_inverse = 1 / R;
	R_alpha = NTL::power(g_alpha, r);

    ZZ_p masked_secret_alpha = R_alpha * NTL::power(h_x_alpha, keyholder_context.key);
    ZZ_p masked_coefficients_alpha[t-1];
	for (int i = 0; i < t-1; i++){
		masked_coefficients_alpha[i] = R_alpha * NTL::power(h_x_alpha, keyholder_context.randoms[i]);
	}

    ZZ_p masked_mac_alpha = R_alpha * NTL::power(h_x_alpha, keyholder_context.key * keyholder_context.key_mac);
    ZZ_p masked_mac_coefficients_alpha[t-1];
	for (int i = 0; i < t-1; i++){
		masked_mac_coefficients_alpha[i] = R_alpha * NTL::power(h_x_alpha, keyholder_context.randoms_mac[i]);
	}

	//////////////////// Element Holder ////////////////////

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
    pcs_generate_key_pair(pk, vk, hr, 2048 + 80); //TODO: 2 * bitsize(p) + 80

    pcs_encrypt(pk, hr, __mpz_secret, __mpz_secret);
	pcs_encrypt(pk, hr, __mpz_mac, __mpz_mac);
	for (int i=0;i<t-1;i++){
		pcs_encrypt(pk, hr, __mpz_coefficients[i], __mpz_coefficients[i]);
		pcs_encrypt(pk, hr, __mpz_mac_coefficients[i], __mpz_mac_coefficients[i]);
	}

	//////////////////// Key Holder ////////////////////

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

	//////////////////// Element Holder ////////////////////

	pcs_decrypt(vk, __mpz_secret, __mpz_secret);	
	pcs_decrypt(vk, __mpz_mac, __mpz_mac);

	ZZ_p secret_share, mac_share;
	mpz_t_to_ZZ_p(secret_share, __mpz_secret);
	mpz_t_to_ZZ_p(mac_share, __mpz_mac);

	return Share(id,rep(hash_(X, ZZ(num_bins))),rep(secret_share),rep(mac_share));
}

Share ShareGen_2(
    ContextScheme2 public_context, KeyholderContext keyholder_context, NTL::ZZ id, NTL::ZZ X, int num_bins
){
	//////////////////// Element Holder ////////////////////
	ZZ p = public_context.p, q = public_context.q;
	int t = public_context.t;
	ZZ_p::init(p);
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

	//////////////////// Key Holder ////////////////////
	ZZ secret_exp, mac_exp;
	{
		ZZ_pPush push(p-1);
		// ZZ_p::init(q);
		ZZ_p __secret_exp, __mac_exp, id_pows, __temp;
		conv(__secret_exp, keyholder_context.key);
		conv(__mac_exp, keyholder_context.key * keyholder_context.key_mac);
		conv(id_pows, id);
		for (int i = 0; i < t-1; i++){
			conv(__temp, keyholder_context.randoms[i]);
			__secret_exp += id_pows * __temp;
			conv(__temp, keyholder_context.randoms_mac[i]);
			__mac_exp += id_pows * __temp;
			conv(__temp, id);
			id_pows *= __temp;
		}
		secret_exp = rep(__secret_exp);
		mac_exp = rep(__mac_exp);
	}

	ZZ_p secret = NTL::power(h_x_alpha, secret_exp);
	ZZ_p mac= NTL::power(h_x_alpha, mac_exp);

	//////////////////// Element Holder ////////////////////

	secret = NTL::power(secret, alpha_inv);
	mac = NTL::power(mac, alpha_inv);

	return Share(id,rep(hash_(X, ZZ(num_bins))),rep(secret),rep(mac));
}
