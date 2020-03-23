#ifndef __SHAREGEN__
#define __SHAREGEN__

#include <NTL/ZZ_p.h>
#include <iostream>
#include <gmp.h>    // gmp is included implicitly
#include <libhcs.h> // master header includes everything
#include<fstream>
#include <sstream>
#include <string>
#include "../global/psi_utils.h"


Share ShareGen_1(
    Context public_context, KeyholderContext keyholder_context, NTL::ZZ id, NTL::ZZ X, int num_bins
);
Share ShareGen_2(
    Context public_context, KeyholderContext keyholder_context, NTL::ZZ id, NTL::ZZ X, int num_bins
);

#endif