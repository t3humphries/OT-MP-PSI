#include "ShareGen.h"
#include "nlohmann/json.hpp"

using namespace NTL;
using namespace std;
using json = nlohmann::json;


int main()
{
	int p = 1000000007;
	int q = 500000003;
	int g = 3;
	int id, X=7, Y=8, t=10, key=4, key_mac=5;
	ZZ_p::init(ZZ(p));
	Share ans;
	ZZ r1[t-1], r2[t-1];
	stringstream ssa;
	ofstream o1("../test_cases/ss1_match.json");
	ofstream o2("../test_cases/ss2_match.json");

	for (int i = 0; i < t-1; i++){
		r1[i] = rand() % 1000 +1;
		r2[i] = rand() % 1000 +1;
	}
	json jsons_1, jsons_2;
	for (int i = 0; i < t; i++){
		id = rand() % 100 + 1;

		ans = ShareGen_1(ZZ(p), ZZ_p(g), ZZ(id), ZZ(X), t, ZZ(key), ZZ(key_mac), r1, r2);
		jsons_1[i]["id"] = id;
		jsons_1[i]["bin"] = ZZ_to_str(ans.bin);
		jsons_1[i]["SS"] = ZZ_to_str(ans.SS);
		jsons_1[i]["SS_MAC"] = ZZ_to_str(ans.SS_mac);

		ans = ShareGen_2(ZZ(p), ZZ(q), ZZ(id), ZZ(X), t, ZZ(key), ZZ(key_mac), r1, r2);
		jsons_2[i]["id"] = id;
		jsons_2[i]["bin"] = ZZ_to_str(ans.bin);
		jsons_2[i]["SS"] = ZZ_to_str(ans.SS);
		jsons_2[i]["SS_MAC"] = ZZ_to_str(ans.SS_mac);

	}
	o1 << jsons_1;
	o2 << jsons_2;
}