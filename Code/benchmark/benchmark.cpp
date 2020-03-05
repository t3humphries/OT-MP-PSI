#include <NTL/ZZ_p.h>
#include <NTL/ZZ.h>
#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include <vector>
#include "../sharegen/psi_utils.h"
#include "../sharegen/ShareGen.h"

using namespace NTL;
using namespace std;
using json = nlohmann::json;

//Just for benchmark
void generate_benchmark_context(string dirname){
    //make a directory
    std::ifstream config_file(dirname + "/public_config.json");
    json config;
	config_file >> config;
    
    long m = config["m"];
    long n = config["n"];
    long t = config["t"];
    string __temp = config["p"];
    ZZ p = ZZ(atol(__temp.c_str()));

    // cout << m << endl << n << endl <<  t << endl << p << endl;

    ZZ_p::init(p);
    //create the config file
    int __num_elements__;
    ofstream party_file;
    for (int i =0; i< m; i++){
        __num_elements__ = rand() % (n+1);
        party_file.open(dirname + "//elements//" + to_string(i+1) + ".txt");
        if (!party_file.is_open()){
            cout << "Something wrong for party " + to_string(i) << endl;
        } else{
            for (int j =0;j<__num_elements__;j++){
                // cout << rep(random_ZZ_p()) << endl;
                party_file << rep(random_ZZ_p()) << endl;
            }
        }
        party_file.close();
    }

    //TODO: generate keyholder context

}

//generally needed
vector<Share>* generate_shares_1(
    long elements_list[], int idd, int num_bins, int max_bin_size,
    ContextScheme1 public_context, KeyholderContext keyholder_context
    ){
    vector<Share> shares_bins[num_bins];
    int size_of_set = sizeof(elements_list);
    Share share_x;
    for (int i=0;i<num_bins;i++){
        shares_bins[i] = vector<Share>(0);
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

void run_benchmark(string benchmark_code){
    //get the directory with that code
    //read the config
    int m = 10;
    //ShareGen
    time_t current_time;
    double share_gen_time;
    for (int i=0;i<m;i++){
        current_time = time(NULL);
        //TODO: generate the shares for this person, put then in a list of lists
        share_gen_time = difftime(time(NULL), current_time);
        cout << share_gen_time << endl;
        //distribute the elements into bins
        //pad bins
    }

    //Reconstruct

    //write results to file

}

int main(){
    // generate_benchmark_context("benchmark_0000");
    run_benchmark("benchmark_0000");
}