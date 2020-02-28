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
	json j;
	Share ans;
	ZZ r1[t-1], r2[t-1];
	stringstream ssa;
	ofstream o1("../test_cases/ss1_match.json");
	ofstream o2("../test_cases/ss2_match.json");
	
	for (int i = 0; i < t-1; i++){
		r1[i] = rand() % 1000 +1;
		r2[i] = rand() % 1000 +1;
	}
	o1 << "[" << endl;
	o2 << "[" << endl;
	for (int i = 0; i < t; i++){
		id = rand() % 100 + 1;
		ans = ShareGen_1(ZZ(p), ZZ_p(g), ZZ(id), ZZ(X), t, ZZ(key), ZZ(key_mac), r1, r2);
		j["id"] = id;
		j["bin"] = ZZ_to_str(ans.bin);
		j["SS"] = ZZ_to_str(ans.SS);
		j["SS_MAC"] = ZZ_to_str(ans.SS_mac);
		o1 << j;
		if (i != t-1){o1 << ",";}
		o1 << endl;

		ans = ShareGen_2(ZZ(p), ZZ(q), ZZ(id), ZZ(X), t, ZZ(key), ZZ(key_mac), r1, r2);
		j["id"] = id;
		j["bin"] = ZZ_to_str(ans.bin);
		j["SS"] = ZZ_to_str(ans.SS);
		j["SS_MAC"] = ZZ_to_str(ans.SS_mac);
		cout << j << endl;
		o2 << j;
		if (i != t-1){o2 << ",";}
		o2 << endl;

	}
	o1 << "]";
	o2 << "]";

}