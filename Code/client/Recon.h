#ifndef __RECON_H__
#define __RECON_H__

#include <NTL/ZZ_p.h>
#include <NTL/ZZ.h>
#include "../global/nlohmann/json.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include "../global/psi_utils.h"

NTL::ZZ_p reconScheme1(std::vector<Share> shares, Context context, int mac); //mac=1 to recon SS_mac and mac=0 to recon SS
NTL::ZZ_p reconScheme2(std::vector<Share> shares, Context context, int mac);
std::vector<NTL::ZZ> recon_in_bin_x(std::vector<std::vector<Share>> shares, Context context, int m, int max_bin_size, int scheme);

#endif