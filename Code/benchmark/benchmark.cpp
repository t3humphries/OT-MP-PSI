#include <math.h>
#include <NTL/ZZ_p.h>
#include <NTL/ZZ.h>
#include "../sharegen/nlohmann/json.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include <vector>
#include "../sharegen/psi_utils.h"
#include "../sharegen/ShareGen.h"
#include "../sharegen/Recon.h"

using namespace NTL;
using namespace std;
using json = nlohmann::json;

void generate_benchmark_context(int m, int n, int t, string dirname){
    ofstream config_file(dirname + "/benchmark_config.json");
    json config;
    config["m"] = m;
    config["n"] = n;
    config["num_bins"] = n / log(n);
    config["max_bin_size"] = 4 * log(n);
    config["t"] = t;
    int id_list[m];
    for (int i = 0;i < m;i++) config["id_list"][i] = rand()%1000+1;

    config_file << config;

    // generate elements
    int __num_elements__;
    ofstream element_file;
    for (int i =0; i< m; i++){
        __num_elements__ = rand() % (n+1);
        element_file.open(dirname + "//elements//" + to_string(i+1) + ".txt");
        if (!element_file.is_open()){
            cout << "Something wrong for party " + to_string(i) << endl;
        } else{
            for (int j =0;j<__num_elements__;j++){
                // cout << rep(random_ZZ_p()) << endl;
                // element_file << rep(random_ZZ_p()) << endl;
                element_file << rand() % 10000000 << endl;
                //TODO: generate the elements accordingly
            }
        }
        element_file.close();
    }

}

//generally needed
vector<vector<Share>> generate_shares_1(
    vector<int> elements_list, int idd, int num_bins, int max_bin_size,
    ContextScheme1 public_context, KeyholderContext keyholder_context
    ){
    vector<vector<Share>> shares_bins;
    int size_of_set = elements_list.size();
    Share share_x;
    for (int i=0;i<num_bins;i++){
        shares_bins.push_back(vector<Share>(0));
    }
    for (int i = 0; i< size_of_set; i++){
        share_x = ShareGen_1(public_context, keyholder_context, ZZ(idd), ZZ(elements_list[i]), num_bins);
    }
    //padding the bins
    for (int i=0;i<num_bins;i++){
        while(shares_bins[i].size() < max_bin_size){
            shares_bins[i].push_back(Share(ZZ(idd), ZZ(i), public_context.p));
        }
    }
    return shares_bins;
}

vector<int> read_elements_to_vector(string filename){
    //TODO:return the vector of elements in the file
    vector<int> ans = vector<int>(7);
    for (int i=0;i<7;i++) ans[i]=i;
    // rand()%10000;
    return ans;
}

void run_benchmark(string dirname){

    int p = 1000000007, g=3, m=10, n=100, t=2;
    generate_benchmark_context(m,n,t,"benchmark_0000");

    ContextScheme1 c1(p, g, t);

    KeyholderContext keyholder_context;
    keyholder_context.initialize_context(ZZ(p)-1, t);
    keyholder_context.write_to_file(dirname + "/keyholder_context.json");

    //ShareGen
    time_t current_time;
    double share_gen_time;
    vector<vector<Share>> bins_shares;
    vector<int> xx = read_elements_to_vector("fake"); //TODO

    vector<vector<Share>> __bin_0_shares(0);
    vector<vector<Share>> __bin_1_shares(0);
    vector<vector<Share>> __bin_2_shares(0);
    vector<vector<Share>> __bin_3_shares(0);
    for (int i=0;i<m;i++){
        cout << "generating for person " << i << endl;
        current_time = time(NULL);
        //read the elements of this person
        bins_shares = generate_shares_1(xx, rand()%1000+1, 4, 7, c1, keyholder_context);
        // for (int j=0;j<7;j++){
        //     cout << bins_shares[0][j].SS << endl;
        // }
        __bin_0_shares.push_back(bins_shares[0]);
        __bin_1_shares.push_back(bins_shares[1]);
        __bin_2_shares.push_back(bins_shares[2]);
        __bin_3_shares.push_back(bins_shares[3]);
        //TODO: generate the shares for this person, put then in a list of lists
        share_gen_time = difftime(time(NULL), current_time);
        cout << share_gen_time << endl;
        //distribute the elements into bins
    }

    for(int i = 0;i<7;i++){
        cout << __bin_0_shares[0][i].SS << endl;
    }

    cout << "generating complete" << endl;

    vector<ZZ> ans;
    ans = recon1_in_bin_x(__bin_0_shares, c1, 5, m, 7);
        cout << ans.size() << endl;
    ans = recon1_in_bin_x(__bin_1_shares, c1, 5, m, 7);
        cout << ans.size() << endl;
    ans = recon1_in_bin_x(__bin_2_shares, c1, 5, m, 7);
        cout << ans.size() << endl;
    ans = recon1_in_bin_x(__bin_3_shares, c1, 5, m, 7);
        cout << ans.size() << endl;

    //cout<<recon1_in_bin_x(__bin_0_shares, c1, 5, m, 7).size()<<endl;

    //Reconstruct

    //write results to file

}

int main(){
    run_benchmark("benchmark_0000");
}