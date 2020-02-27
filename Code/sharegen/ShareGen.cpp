#include "ShareGen.h"
#include "nlohmann/json.hpp"

using namespace NTL;
using namespace std;
using json = nlohmann::json;

ZZ_p hash_(ZZ x, ZZ p){
	if (x == ZZ(7))
		return ZZ_p(3123);
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
	ZZ_p g_alpha = NTL::power(g, alpha);
	
	//////////////////// Key Holder ////////////////////

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
    pcs_generate_key_pair(pk, vk, hr, 2048 + 80);

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

	Share ans = {
		.id = id,
		.bin = ZZ(1),
		.SS = secret_share,
		.SS_mac = mac_share
	};
	return ans;
}

Share ShareGen_2(ZZ p, ZZ q, ZZ id, ZZ X, int t, ZZ key, ZZ key_mac, ZZ randoms[], ZZ randoms_mac[]){

	//////////////////// Element Holder ////////////////////
	ZZ_p::init(p);
	ZZ_p h_x = hash_(X, p);
	// h_x = h_x * h_x;
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
		conv(__secret_exp, key);
		conv(__mac_exp, key * key_mac);
		conv(id_pows, id);
		for (int i = 0; i < t-1; i++){
			conv(__temp, randoms[i]);
			__secret_exp += id_pows * __temp;
			conv(__temp, randoms_mac[i]);
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

	Share ans = {
		.id = id,
		.bin = ZZ(1),
		.SS = secret,
		.SS_mac = mac,
	};
	return ans;
}


int main()
{
	int p = 1000000007;
	int q = 500000003;
	int g = 3;
	int id, X=7, Y=8, t=10, key=4, key_mac=5;
	ZZ_p::init(ZZ(p));
	json j;
	Share ans;
	ZZ r1[t-1], r2[t-1];
	stringstream ssa;
	ofstream o1("../test_cases/ss1_match.json");
	ofstream o2("../test_cases/ss2_match.json");
	
	for (int i = 0; i < t-1; i++){
		r1[i] = rand() % 1000 +1;
		r2[i] = rand() % 1000 +1;
	}
	o1 << "[" << endl;
	o2 << "[" << endl;
	for (int i = 0; i < t; i++){
		id = rand() % 100 + 1;
		ans = ShareGen_1(ZZ(p), ZZ_p(g), ZZ(id), ZZ(X), t, ZZ(key), ZZ(key_mac), r1, r2);
		j["id"] = id;
		ssa << ans.bin;
		j["bin"] = ssa.str().c_str(); ssa.str("");
		ssa << ans.SS;
		j["SS"] = ssa.str().c_str(); ssa.str("");
		ssa << ans.SS_mac;
		j["SS_MAC"] = ssa.str().c_str(); ssa.str("");
		o1 << j;
		if (i != t-1){o1 << ",";}
		o1 << endl;

		ans = ShareGen_2(ZZ(p), ZZ(q), ZZ(id), ZZ(X), t, ZZ(key), ZZ(key_mac), r1, r2);
		j["id"] = id;
		ssa << ans.bin;
		j["bin"] = ssa.str().c_str(); ssa.str("");
		ssa << ans.SS;
		j["SS"] = ssa.str().c_str(); ssa.str("");
		ssa << ans.SS_mac;
		j["SS_MAC"] = ssa.str().c_str(); ssa.str("");
		cout << j << endl;
		o2 << j;
		if (i != t-1){o2 << ",";}
		o2 << endl;

	}
	o1 << "]";
	o2 << "]";

}