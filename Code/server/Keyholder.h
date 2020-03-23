#ifndef __KEYHOLDER_H__
#define __KEYHOLDER_H__

#include "../global/psi_utils.h"

using namespace std;
using namespace NTL;

class Keyholder{
	public:
	Context public_context;
	ZZ key;
	ZZ key_mac;
	ZZ* randoms;
	ZZ* randoms_mac;
	ZZ r, __R, __R_inverse;

    Keyholder(Context __c1, ZZ __key, ZZ __key_mac, ZZ __rands[], ZZ __rands_mac[]);
	Keyholder(Context __c1);
	Keyholder(string str);
	Keyholder(){}
    void initialize_context(Context __c1);
    string toString();

	Scheme1_Round1_receive Scheme1_Round1(Scheme1_Round1_send payload);
    Scheme1_Round2_receive Scheme1_Round2(Scheme1_Round2_send payload);

    Scheme2_receive Scheme2_Round1(Scheme2_send payload);

};

#endif