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

Keyholder::Keyholder(Context __c1, ZZ __key, ZZ __rands[]){
    public_context = __c1;
    key = __key;
    //key_mac = __key_mac;
    randoms = __rands;
    //randoms_mac = __rands_mac;
}

// Keyholder::Keyholder(Context __c2, ZZ __key, ZZ __key_mac, ZZ __rands[], ZZ __rands_mac[]){
//     context2 = __c2;
//     key = __key;
//     key_mac = __key_mac;
//     randoms = __rands;
//     randoms_mac = __rands_mac;
// }

Keyholder::Keyholder(Context __c1){
    initialize_context(__c1);
}

// Keyholder::Keyholder(Context __c2){
//     initialize_context(__c2);
// }

void Keyholder::initialize_context(Context __c1){
    public_context = __c1;
    ZZ_p::init(public_context.p-1);
    key = rep(random_ZZ_p());
    //key_mac = rep(random_ZZ_p());
    randoms = new ZZ[public_context.t]; //TODO this is wrong should be t-1??
    //randoms_mac = new ZZ[public_context.t];
    for (int i=0;i<public_context.t;i++){
        randoms[i] = rep(random_ZZ_p());
        //randoms_mac[i] = rep(random_ZZ_p());
    }
}

void Keyholder::initialize_from_file(Context context, string filename){
    public_context = context;
    std::ifstream inputFile(filename);
    json jsonFile;
    inputFile >> jsonFile;
    if(!inputFile.good()){
        cout<< "Could not open:"<<filename<<endl;
    }
    inputFile.close();
    // t = jsonFile["t"] ;
    int t = public_context.t;
    key = ZZ(INIT_VAL, jsonFile["key"].get<string>().c_str());
    //key_mac = ZZ(INIT_VAL, jsonFile["key_mac"].get<string>().c_str());
    randoms = new NTL::ZZ[t-1];
    //randoms_mac = new NTL::ZZ[t-1];
    for(int i = 0 ; i < t-1 ; i++){
        randoms[i] = ZZ(INIT_VAL, jsonFile["randoms"][i].get<string>().c_str());
       // randoms_mac[i] = ZZ(INIT_VAL, jsonFile["randoms_mac"][i].get<string>().c_str());
    }
}

Scheme1_Round1_receive Keyholder::Scheme1_Round1(Scheme1_Round1_send payload){
	
    ZZ p = public_context.p, g = public_context.g;
	int t = public_context.t;
    Scheme1_Round1_receive output;
	ZZ_p::init(p);
	ZZ_p g_p, h_x_alpha, g_alpha;
	conv(g_p, g);
    conv(h_x_alpha, payload.h_x_alpha);
    conv(g_alpha, payload.g_alpha);        
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
    output.masked_secret_alpha = ZZ(0);//TODO remove double check this rep(R_alpha * NTL::power(h_x_alpha, key));
	for (int i = 0; i < t-1; i++){
		output.masked_coefficients_alpha.push_back(rep(R_alpha * NTL::power(h_x_alpha, randoms[i])));
	}

 //    output.masked_mac_alpha = rep(R_alpha * NTL::power(h_x_alpha, key));
	// for (int i = 0; i < t-1; i++){
	// 	output.masked_mac_coefficients_alpha.push_back(rep(R_alpha * NTL::power(h_x_alpha, randoms_mac[i])));
	// }

    return output;
}

Scheme1_Round2_receive Keyholder::Scheme1_Round2(Scheme1_Round2_send payload){
	mpz_t __mpz_temp;
	mpz_init(__mpz_temp);

    Scheme1_Round2_receive output;
	
	ZZ_to_mpz_t(__mpz_temp, __R_inverse);
    //mpz_set_ui(output.mpz_secret,1);
	pcs_ep_mul(payload.pk, output.mpz_secret, payload.mpz_secret, __mpz_temp);//TODO do we need this?
	//pcs_ep_mul(payload.pk, output.mpz_mac, payload.mpz_mac, __mpz_temp);
	
    ZZ_p::init(public_context.p);
	ZZ_p R_inv_id_pows = to_ZZ_p(payload.id) * conv<ZZ_p>(__R_inverse);
	for (int i=0;i<public_context.t-1;i++){
		ZZ_p_to_mpz_t(__mpz_temp, R_inv_id_pows);
		pcs_ep_mul(payload.pk, (payload.mpz_coefficients)[i], (payload.mpz_coefficients)[i], __mpz_temp);
		//pcs_ep_mul(payload.pk, (payload.mpz_mac_coefficients)[i], (payload.mpz_mac_coefficients)[i], __mpz_temp);
		R_inv_id_pows = R_inv_id_pows * to_ZZ_p(payload.id);

		pcs_ee_add(payload.pk, output.mpz_secret, output.mpz_secret, (payload.mpz_coefficients)[i]);
		//pcs_ee_add(payload.pk, output.mpz_mac, output.mpz_mac, (payload.mpz_mac_coefficients)[i]);
	}

    return output;

}

Scheme2_receive Keyholder::Scheme2_Round1(Scheme2_send payload){
    ZZ p = public_context.p;
    int t = public_context.t;
    ZZ secret_exp, mac_exp;
    ZZ_p::init(p);
	{
		ZZ_pPush push(p-1);
		// ZZ_p::init(q);
		ZZ_p __secret_exp, __mac_exp, id_pows, __temp, __temp_id;
		__secret_exp = ZZ_p(0);//TODO Double check conv(__secret_exp, key);
		//conv(__mac_exp, key);
		conv(id_pows, payload.id);
        conv(__temp_id, payload.id);
		for (int i = 0; i < t-1; i++){
			conv(__temp, randoms[i]);
			__secret_exp += id_pows * __temp;
			//conv(__temp, randoms_mac[i]);
			//__mac_exp += id_pows * __temp;
			id_pows *= __temp_id;
		}
		secret_exp = rep(__secret_exp);
		//mac_exp = rep(__mac_exp);
	}

    Scheme2_receive output;

	output.secret_share_alpha = rep(NTL::power(conv<ZZ_p>(payload.h_x_alpha), secret_exp));
	//output.mac_share_alpha= rep(NTL::power(conv<ZZ_p>(payload.h_x_alpha), mac_exp));

    return output;
}

string Keyholder::toString()  //TODO these are likely much larger than needed.
{
    string delim = ",";
    string toReturn = "";
    toReturn += ZZ_to_str(public_context.p);
    toReturn += delim;
    toReturn += ZZ_to_str(public_context.q);
    toReturn += delim;
    toReturn += ZZ_to_str(public_context.g);
    toReturn += delim;
    toReturn += to_string(public_context.t);
    toReturn += delim;

    // toReturn += ZZ_to_str(context2.p);
    // toReturn += delim;
    // toReturn += ZZ_to_str(context2.q);
    // toReturn += delim;
    // toReturn += to_string(context2.t);
    // toReturn += delim;

    toReturn += ZZ_to_str(key);
    toReturn += delim;
    //toReturn += ZZ_to_str(key_mac);
    //toReturn += delim;

    int size = public_context.t-1;

    for(int i = 0 ; i<size ; i++)
    {
        toReturn += ZZ_to_str(randoms[i]);
        toReturn += delim;
    }
    // for(int i = 0 ; i<size ; i++)
    // {
    //     toReturn += ZZ_to_str(randoms_mac[i]);
    //     toReturn += delim;
    // }

    toReturn += ZZ_to_str(r);
    toReturn += delim;
    toReturn += ZZ_to_str(__R);
    toReturn += delim;
    toReturn += ZZ_to_str(__R_inverse);
    
    return toReturn;
}

Keyholder::Keyholder(string str)
{
    stringstream ss(str);
    string token;
    char delim = ',';
    std::getline(ss, token, delim);
    public_context.p = str_to_ZZ(token);
    std::getline(ss, token, delim);
    public_context.q = str_to_ZZ(token);
    std::getline(ss, token, delim);
    public_context.g = str_to_ZZ(token);
    std::getline(ss, token, delim);
    public_context.t = stoi(token);

    // std::getline(ss, token, delim);
    // context2.p = str_to_ZZ(token);
    // std::getline(ss, token, delim);
    // context2.q = str_to_ZZ(token);
    // std::getline(ss, token, delim);
    // context2.t = stoi(token);

    std::getline(ss, token, delim);
    key = str_to_ZZ(token);
    //std::getline(ss, token, delim);
    //key_mac = str_to_ZZ(token);

    int size = public_context.t-1;
    randoms = new ZZ[size];
    for(int i = 0 ; i<size ; i++)
    {
        std::getline(ss, token, delim); 
        randoms[i] = str_to_ZZ(token);
    }
    // randoms_mac = new ZZ[size];
    // for(int i = 0 ; i<size ; i++)
    // {
    //     std::getline(ss, token, delim); 
    //     randoms_mac[i] = str_to_ZZ(token);
    // }

    std::getline(ss, token, delim);
    r = str_to_ZZ(token);
    std::getline(ss, token, delim);
    __R = str_to_ZZ(token);
    std::getline(ss, token, delim);
    __R_inverse = str_to_ZZ(token);

}