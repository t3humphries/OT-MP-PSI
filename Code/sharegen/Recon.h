#ifndef __RECON_H__
#define __RECON_H__

#include <NTL/ZZ_p.h>
#include <NTL/ZZ.h>
#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include "psi_utils.h"

NTL::ZZ_p reconScheme1(std::vector<Share> shares, ContextScheme1 context, int mac); //mac=1 to recon SS_mac and mac=0 to recon SS
NTL::ZZ_p reconScheme2(std::vector<Share> shares, ContextScheme2 context, int mac);
std::vector<NTL::ZZ> recon1_in_bin_x(std::vector<std::vector<Share>> shares, ContextScheme1 context, NTL::ZZ k2, int m, int max_bin_size);
std::vector<NTL::ZZ> recon2_in_bin_x(std::vector<std::vector<Share>> shares, ContextScheme2 context, NTL::ZZ k2, int m, int max_bin_size);

#endif