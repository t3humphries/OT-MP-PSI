#include "Keyholder.h"

using namespace std;
using namespace NTL;

// void ZZ_to_mpz_t(mpz_t __out, ZZ __temp_ZZ){
// 	std::stringstream ssa;
// 	ssa << __temp_ZZ;
// 	mpz_set_str( __out, ssa.str().c_str(), 10);
// }

// void ZZ_p_to_mpz_t(mpz_t __out, ZZ_p& num){
// 	ZZ __temp_ZZ;
// 	__temp_ZZ = rep(num);
// 	std::stringstream ssa;
// 	ssa << __temp_ZZ;
// 	mpz_set_str( __out, ssa.str().c_str(), 10);
// }

// void mpz_t_to_ZZ(ZZ& __out, mpz_t num){
// 	// ZZ __temp_ZZ;
// 	std::stringstream __ssa;
// 	char __temp[1000];
// 	mpz_get_str(__temp, 10, num);
// 	__ssa << __temp;
// 	__ssa >> __out;
// }

// void mpz_t_to_ZZ_p(ZZ_p& __out, mpz_t num){
// 	ZZ __temp_ZZ;
// 	std::stringstream __ssa;
// 	char __temp[1000];
// 	mpz_get_str(__temp, 10, num);
// 	__ssa << __temp;
// 	__ssa >> __temp_ZZ;
// 	conv(__out, __temp_ZZ);
// }

Keyholder::Keyholder(ContextScheme1 __c1, int __key, int __key_mac, ZZ __rands[], ZZ __rands_mac[]){
    public_context = __c1;
    key = ZZ(__key);
    key_mac = ZZ(__key_mac);
    randoms = __rands;
    randoms_mac = __rands_mac;
}

Keyholder::Keyholder(ContextScheme1 __c1, ZZ __key, ZZ __key_mac, ZZ __rands[], ZZ __rands_mac[]){
    public_context = __c1;
    key = __key;
    key_mac = __key_mac;
    randoms = __rands;
    randoms_mac = __rands_mac;
}

Keyholder::Keyholder(ContextScheme1 __c1){
    initialize_context(__c1);
}

void Keyholder::initialize_context(ContextScheme1 __c1){
    public_context = __c1;
    ZZ_p::init(public_context.p-1);
    key = rep(random_ZZ_p());
    key_mac = rep(random_ZZ_p());
    randoms = new ZZ[public_context.t];
    randoms_mac = new ZZ[public_context.t];
    for (int i=0;i<public_context.t;i++){
        randoms[i] = rep(random_ZZ_p());
        randoms_mac[i] = rep(random_ZZ_p());
    }
}

Scheme1_Round1_output Keyholder::Scheme1_Round1(ZZ __h_x_alpha, ZZ __g_alpha){
	
    ZZ p = public_context.p, g = public_context.g;
	int t = public_context.t;
    Scheme1_Round1_output output;
	ZZ_p::init(p);
	ZZ_p g_p, h_x_alpha, g_alpha;
	conv(g_p, g);
    conv(h_x_alpha, __h_x_alpha);
    conv(g_alpha, __g_alpha);        

	ZZ_p R, R_inverse, R_alpha;
    {
		ZZ_pPush push(p-1);
		ZZ_p __temp = random_ZZ_p();
        while  (GCD(rep(__temp), public_context.p-1) > 1) {
            __temp = random_ZZ_p();
        }
		r = rep(__temp);
	}

	R = NTL::power(g_p, r);
	// R_inverse = NTL::power(g, -r);
	R_inverse = 1 / R;

    __R = rep(R);
    __R_inverse = rep(R_inverse);

    // conv(R, __R);
    // conv(R_inverse, __R_inverse);

	R_alpha = NTL::power(g_alpha, r);
    output.masked_secret_alpha = rep(R_alpha * NTL::power(h_x_alpha, key));
    // ZZ_p masked_coefficients_alpha[t-1];
	for (int i = 0; i < t-1; i++){
        // cout << "1: " << R_alpha * NTL::power(h_x_alpha, randoms[i]) << endl;
        // cout << "2: " << R_alpha * NTL::power(h_x_alpha, randoms[i]) << endl;
		output.masked_coefficients_alpha.push_back(rep(R_alpha * NTL::power(h_x_alpha, randoms[i])));
	}

    output.masked_mac_alpha = rep(R_alpha * NTL::power(h_x_alpha, key * key_mac));
    // ZZ_p masked_mac_coefficients_alpha[t-1];
	for (int i = 0; i < t-1; i++){
		output.masked_mac_coefficients_alpha.push_back(rep(R_alpha * NTL::power(h_x_alpha, randoms_mac[i])));
	}

    return output;
    // return Scheme1_Round1_output(
    //     masked_secret_alpha,
    //     masked_coefficients_alpha,
    //     masked_mac_alpha,
    //     masked_mac_coefficients_alpha
    // );
}

void Keyholder::Scheme1_Round2(
    pcs_public_key *pk, int id,
    mpz_t __mpz_secret, mpz_t __mpz_mac,
    mpz_t* __mpz_coefficients, mpz_t* __mpz_mac_coefficients
){
	mpz_t __mpz_temp;
	mpz_init(__mpz_temp);
	
	ZZ_to_mpz_t(__mpz_temp, __R_inverse);
	pcs_ep_mul(pk, __mpz_secret, __mpz_secret, __mpz_temp);
	pcs_ep_mul(pk, __mpz_mac, __mpz_mac, __mpz_temp);
	
    ZZ_p::init(public_context.p);

	ZZ_p R_inv_id_pows = to_ZZ_p(id) * conv<ZZ_p>(__R_inverse);
	for (int i=0;i<public_context.t-1;i++){
		ZZ_p_to_mpz_t(__mpz_temp, R_inv_id_pows);
		pcs_ep_mul(pk, __mpz_coefficients[i], __mpz_coefficients[i], __mpz_temp);
		pcs_ep_mul(pk, __mpz_mac_coefficients[i], __mpz_mac_coefficients[i], __mpz_temp);
		R_inv_id_pows = R_inv_id_pows * to_ZZ_p(id);

		pcs_ee_add(pk, __mpz_secret, __mpz_secret, __mpz_coefficients[i]);
		pcs_ee_add(pk, __mpz_mac, __mpz_mac, __mpz_mac_coefficients[i]);
	}

}
