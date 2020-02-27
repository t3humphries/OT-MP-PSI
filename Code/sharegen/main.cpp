#include "ShareGen.h"
#include "nlohmann/json.hpp"

using namespace NTL;
using namespace std;
using json = nlohmann::json;

// class Thing;

int main()
{
	// Thing b;
	int p = 1000000007;
	int q = 500000003;
	ZZ_p::init(ZZ(p));
	ZZ r1 [2] = {ZZ(1), ZZ(1)}; 
	ZZ r2 [2] = {ZZ(1), ZZ(1)};
	Share ans = ShareGen_1(ZZ(p), ZZ_p(3), ZZ(0), ZZ(2), 3, ZZ(4), ZZ(5), r1, r2);
	// Share ShareGen_1(ZZ p, ZZ_p g, ZZ id, ZZ X, int t, ZZ key, ZZ key_mac, ZZ randoms[], ZZ randoms_mac[]){

	// create an empty structure (null)
	// json j;
	// j["pi"] = 3.141;
	// j["happy"] = true;

	// cout << j << endl;

}

// int main(){
// 	int p = 1000000007;
// 	int q = 500000003;
// 	ZZ_p::init(ZZ(p));
// 	ZZ r1 [2] = {ZZ(1), ZZ(1)}; 
// 	ZZ r2 [2] = {ZZ(1), ZZ(1)};
// 	// Share ans = ShareGen_1(ZZ(p), ZZ_p(3), ZZ(0), ZZ(2), 3, ZZ(4), ZZ(5), r1, r2);
// 	Share ans = ShareGen_2(ZZ(p), ZZ(q), ZZ(0), ZZ(2), 3, ZZ(4), ZZ(5), r1, r2);
// 	cout << ans.id << endl << ans.bin << endl << ans.SS << endl << ans.SS_mac << endl;

// 	cout << NTL::power(ans.SS, 5) << endl;
// }