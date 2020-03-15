#include "Keyholder.h"

using namespace std;
using namespace NTL;

// Keyholder::Keyholder(ContextScheme1 __c1, int __key, int __key_mac, ZZ __rands[], ZZ __rands_mac[]){
//     public_context = __c1;
//     key = ZZ(__key);
//     key_mac = ZZ(__key_mac);
//     randoms = __rands;
//     randoms_mac = __rands_mac;
// }

Keyholder::Keyholder(ContextScheme1 __c1, ZZ __key, ZZ __key_mac, ZZ __rands[], ZZ __rands_mac[]){
    public_context = __c1;
    key = __key;
    key_mac = __key_mac;
    randoms = __rands;
    randoms_mac = __rands_mac;
}

Keyholder::Keyholder(ContextScheme2 __c2, ZZ __key, ZZ __key_mac, ZZ __rands[], ZZ __rands_mac[]){
    context2 = __c2;
    key = __key;
    key_mac = __key_mac;
    randoms = __rands;
    randoms_mac = __rands_mac;
}

Keyholder::Keyholder(ContextScheme1 __c1){
    initialize_context(__c1);
}

Keyholder::Keyholder(ContextScheme2 __c2){
    initialize_context(__c2);
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

void Keyholder::initialize_context(ContextScheme2 __c2){
    context2 = __c2;
    ZZ_p::init(context2.p-1);
    key = rep(random_ZZ_p());
    key_mac = rep(random_ZZ_p());
    randoms = new ZZ[context2.t];
    randoms_mac = new ZZ[context2.t];
    for (int i=0;i<context2.t;i++){
        randoms[i] = rep(random_ZZ_p());
        randoms_mac[i] = rep(random_ZZ_p());
    }
}

Scheme1_Round1_receive Keyholder::Scheme1_Round1(ZZ __h_x_alpha, ZZ __g_alpha){
	
    ZZ p = public_context.p, g = public_context.g;
	int t = public_context.t;
    Scheme1_Round1_receive output;
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
    // return Scheme1_Round1_receive(
    //     masked_secret_alpha,
    //     masked_coefficients_alpha,
    //     masked_mac_alpha,
    //     masked_mac_coefficients_alpha
    // );
}

// void Keyholder::Scheme1_Round2(
//     pcs_public_key *pk, int id,
//     mpz_t __mpz_secret, mpz_t __mpz_mac,
//     mpz_t* __mpz_coefficients, mpz_t* __mpz_mac_coefficients
// ){
// 	mpz_t __mpz_temp;
// 	mpz_init(__mpz_temp);
	
// 	ZZ_to_mpz_t(__mpz_temp, __R_inverse);
// 	pcs_ep_mul(pk, __mpz_secret, __mpz_secret, __mpz_temp);
// 	pcs_ep_mul(pk, __mpz_mac, __mpz_mac, __mpz_temp);
	
//     ZZ_p::init(public_context.p);

// 	ZZ_p R_inv_id_pows = to_ZZ_p(id) * conv<ZZ_p>(__R_inverse);
// 	for (int i=0;i<public_context.t-1;i++){
// 		ZZ_p_to_mpz_t(__mpz_temp, R_inv_id_pows);
// 		pcs_ep_mul(pk, __mpz_coefficients[i], __mpz_coefficients[i], __mpz_temp);
// 		pcs_ep_mul(pk, __mpz_mac_coefficients[i], __mpz_mac_coefficients[i], __mpz_temp);
// 		R_inv_id_pows = R_inv_id_pows * to_ZZ_p(id);

// 		pcs_ee_add(pk, __mpz_secret, __mpz_secret, __mpz_coefficients[i]);
// 		pcs_ee_add(pk, __mpz_mac, __mpz_mac, __mpz_mac_coefficients[i]);
// 	}

// }

void Keyholder::Scheme1_Round2(Scheme1_Round2_send payload){
	mpz_t __mpz_temp;
	mpz_init(__mpz_temp);
	
	ZZ_to_mpz_t(__mpz_temp, __R_inverse);
	pcs_ep_mul(payload.pk, payload.mpz_secret, payload.mpz_secret, __mpz_temp);
	pcs_ep_mul(payload.pk, payload.mpz_mac, payload.mpz_mac, __mpz_temp);
	
    ZZ_p::init(public_context.p);
	ZZ_p R_inv_id_pows = to_ZZ_p(payload.id) * conv<ZZ_p>(__R_inverse);
	for (int i=0;i<public_context.t-1;i++){
		ZZ_p_to_mpz_t(__mpz_temp, R_inv_id_pows);
		pcs_ep_mul(payload.pk, payload.mpz_coefficients[i], payload.mpz_coefficients[i], __mpz_temp);
		pcs_ep_mul(payload.pk, payload.mpz_mac_coefficients[i], payload.mpz_mac_coefficients[i], __mpz_temp);
		R_inv_id_pows = R_inv_id_pows * to_ZZ_p(payload.id);

		pcs_ee_add(payload.pk, payload.mpz_secret, payload.mpz_secret, payload.mpz_coefficients[i]);
		pcs_ee_add(payload.pk, payload.mpz_mac, payload.mpz_mac, payload.mpz_mac_coefficients[i]);
	}

}

void Keyholder::Scheme2_Round1(ZZ *secret_share_alpha, ZZ *mac_share_alpha, ContextScheme2 context, ZZ h_x_alpha, int idd){
    ZZ p = context.p;
    int t = context.t;
    ZZ secret_exp, mac_exp;
    ZZ_p::init(p);
	{
		ZZ_pPush push(p-1);
		// ZZ_p::init(q);
		ZZ_p __secret_exp, __mac_exp, id_pows, __temp;
		conv(__secret_exp, key);
		conv(__mac_exp, key * key_mac);
		conv(id_pows, idd);
		for (int i = 0; i < t-1; i++){
			conv(__temp, randoms[i]);
			__secret_exp += id_pows * __temp;
			conv(__temp, randoms_mac[i]);
			__mac_exp += id_pows * __temp;
			conv(__temp, idd);
			id_pows *= __temp;
		}
		secret_exp = rep(__secret_exp);
		mac_exp = rep(__mac_exp);
	}

	*secret_share_alpha = rep(NTL::power(conv<ZZ_p>(h_x_alpha), secret_exp));
	*mac_share_alpha= rep(NTL::power(conv<ZZ_p>(h_x_alpha), mac_exp));
}