#ifndef __KEYHOLDER_H__
#define __KEYHOLDER_H__

#include "../global/psi_utils.h"

using namespace std;
using namespace NTL;

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
	Keyholder(string str);
    void initialize_context(ContextScheme1 __c1);
    void initialize_context(ContextScheme2 __c2);
    string toString();

    Scheme1_Round2_receive Scheme1_Round2(Scheme1_Round2_send payload);
	Scheme1_Round1_receive Scheme1_Round1(Scheme1_Round1_send payload);

    void Scheme2_Round1(ZZ *secret_share_alpha, ZZ *mac_share_alpha, ContextScheme2 context, ZZ h_x_alpha, int idd);

};

#endif