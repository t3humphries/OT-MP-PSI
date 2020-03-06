#include <sys/stat.h>
#include <unistd.h>
#include <cstdio>
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
#include <chrono>

using namespace NTL;
using namespace std;
using json = nlohmann::json;

inline bool exists(const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

void generate_benchmark_context(int m, int n, int t, string dirname, bool force=false){
    if (!force && exists(dirname + "/benchmark_config.json")){
        cout << "Benchmark config exist, proceeding..." << endl;
        return;
    } else if (force){

        if (exists(dirname + "//benchmark_config.json")){
            remove((dirname + "//benchmark_config.json").c_str());
        }
        if (exists(dirname + "//elements")){
            // rmdir((dirname + "//elements").c_str());
            system(("rm -r " + dirname + "//elements").c_str());
        }
    }

    srand(time(0));
    
    ofstream config_file(dirname + "//benchmark_config.json");
    json config;
    config["m"] = m;
    config["n"] = n;
    config["num_bins"] = n / (int)log(n);
    config["max_bin_size"] = 4 * (int)log(n);
    config["t"] = t;
    int id_list[m];
    for (int i = 0;i < m;i++){
        id_list[i] = rand()%1000+1;
        config["id_list"][i] = id_list[i];
    }

    config_file << config;

    // generate elements
    system(("mkdir " + dirname + "//elements").c_str());//TODO: must be better way of doing this
    int __num_elements__;
    ofstream element_file;
    for (int i =0; i< m; i++){
        __num_elements__ = rand() % (n+1);
        element_file.open(dirname + "//elements//" + to_string(id_list[i]) + ".txt");
        if (!element_file.is_open()){
            cout << "Something wrong for party " + to_string(id_list[i]) << endl;
        } else{
            for (int j =0;j<__num_elements__;j++){
                element_file << rand() % 10000000 << endl; //TODO: generate the elements accordingly
            }
        }
        element_file.close();
    }
    cout << "Benchmark config created successfully" << endl;
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
    int __index;
    for (int i = 0; i< size_of_set; i++){
        share_x = ShareGen_1(public_context, keyholder_context, ZZ(idd), ZZ(elements_list[i]), num_bins);
        shares_bins[conv<int>(share_x.bin)].push_back(share_x);
    }
    //padding the bins
    for (int i=0;i<num_bins;i++){
        while(shares_bins[i].size() < max_bin_size){
            shares_bins[i].push_back(Share(ZZ(idd), ZZ(i), public_context.p));
        }
    }
    return shares_bins;
}

vector<vector<Share>> generate_shares_2(
    vector<int> elements_list, int idd, int num_bins, int max_bin_size,
    ContextScheme2 public_context, KeyholderContext keyholder_context
    ){
    vector<vector<Share>> shares_bins;
    int size_of_set = elements_list.size();
    Share share_x;
    for (int i=0;i<num_bins;i++){
        shares_bins.push_back(vector<Share>(0));
    }
    int __index;
    for (int i = 0; i< size_of_set; i++){
        share_x = ShareGen_2(public_context, keyholder_context, ZZ(idd), ZZ(elements_list[i]), num_bins);
        shares_bins[conv<int>(share_x.bin)].push_back(share_x);
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

    fstream inputFile(filename, std::ios_base::in);
    if(!inputFile.good())
    {
        cout<<"Could not open file: "<<filename<<endl;
    }

    vector<int> toReturn = vector<int>(0);
    int currentLine = 0;

    while(inputFile >> currentLine)
    {
        toReturn.push_back(currentLine);
    }

    inputFile.close();
    return toReturn;
}

void run_benchmark_1(int m, int n, int t, int bitsize, bool force=false){

    int p = 1000000007, g=3;
    string dirname = "benchmark_"+to_string(m)+to_string(n)+to_string(t)+to_string(bitsize);//TODO




   

    if(force)
    {
        if (exists(dirname))
        {
                rmdir((dirname).c_str());
        }
        system(("mkdir " + dirname).c_str());        
        generate_benchmark_context(m,n,t,dirname);         

    }
    else
    {
        if (exists(dirname)){
                ;
                }
                else{        
                cout<<"\n\n\nwe arrived here\n";  
                system(("mkdir " + dirname).c_str());      
                generate_benchmark_context(m,n,t,dirname);
        }

    }
    //Read the config from the file which is named benchmark_0000/benchmark_config.json

     ifstream config_file(dirname + "//benchmark_config.json");
    json config;
    config_file >> config;
    
    int num_bins=config["num_bins"], max_bin_size=config["max_bin_size"]; //Read these from file
    ContextScheme1 c1(p, g, t);

    KeyholderContext keyholder_context;
    keyholder_context.initialize_context(ZZ(p)-1, t);
    keyholder_context.write_to_file(dirname + "/keyholder_context.json");

    //ShareGen
    vector<vector<Share>> bins_shares;
    vector<vector<Share>> bins_people_shares[num_bins];
    for (int i=0;i<num_bins;i++){
        bins_people_shares[i] = vector<vector<Share>>();
    }
    vector<int> elements;
    int idd;
    int sum_sharegen = 0;

    //reading the config from file

   


    cout << "generating shares for party "; 
    for (int i=0;i<m;i++){
        
        idd=config["id_list"][i];     
        elements = read_elements_to_vector(dirname + "/elements/"+ to_string(idd)+".txt");
        cout << idd << ",";
        auto begin = chrono::high_resolution_clock::now();    
        //read the elements of this person
        bins_shares = generate_shares_1(elements, i+1, num_bins, max_bin_size, c1, keyholder_context);
        auto end = chrono::high_resolution_clock::now();    
        auto dur = end - begin;
        auto ms = chrono::duration_cast<chrono::milliseconds>(dur).count();
        // cout << ms << " miliseconds" << endl;
        sum_sharegen += ms;
        for (int j=0; j<num_bins;j++){
            bins_people_shares[j].push_back(bins_shares[j]);
        }
    }

    cout << endl << "Generating shares complete in " << sum_sharegen/m << " miliseconds on average for each party (including padding)" << endl;

    vector<ZZ> ans;
    int sum = 0;
    auto begin = chrono::high_resolution_clock::now();    
    for (int i=0;i<num_bins;i++){
        ans = recon1_in_bin_x(bins_people_shares[i], c1, keyholder_context.key_mac, m, max_bin_size);
        sum += ans.size();
    }
    auto end = chrono::high_resolution_clock::now();    
    auto dur = end - begin;
    auto ms = chrono::duration_cast<chrono::milliseconds>(dur).count();

    cout << "Reconstruction complete in " << ms << " miliseconds" << endl; 

    cout << "Found " << sum << " elements in t-threshold intersection" << endl;

   //write results to file

}

void run_benchmark_2(int m, int n, int t, int bitsize, bool force=false){

    int p = 1000000007, q=500000003;
    string dirname = "benchmark_0000";//TODO
    generate_benchmark_context(m,n,t,"benchmark_0000",false); //TODO
    //TODO: Read the config from the file which is named benchmark_0000/benchmark_config.json
    
    int num_bins=5, max_bin_size=8; //Read these from file
    ContextScheme2 c2(p, q, t);

    KeyholderContext keyholder_context;
    keyholder_context.initialize_context(ZZ(q), t);
    keyholder_context.write_to_file(dirname + "/keyholder_context2.json");

    //ShareGen
    vector<vector<Share>> bins_shares;
    vector<vector<Share>> bins_people_shares[num_bins];
    for (int i=0;i<num_bins;i++){
        bins_people_shares[i] = vector<vector<Share>>();
    }
    vector<int> elements;
    int idd;
    int sum_sharegen = 0;

    //reading the config from file

    ifstream config_file(dirname + "//benchmark_config.json");
    json config;
    config_file >> config;

    // for(int i = 0;i<size;i++)
    // {
    //     shares[i].id=temp[i]["id"];
    //     std::string str = temp[i]["SS"];
    //     shares[i].SS= atol(str.c_str());
    //     str = temp[i]["SS_MAC"];
    //     shares[i].SS_mac= atol(str.c_str());
    // }

    cout << "generating shares for party "; 
    for (int i=0;i<m;i++){
        //Read the elements of person i from file    
        idd=config["id_list"][i];     
        cout << idd << ", ";
        elements = read_elements_to_vector(dirname + "/elements/"+ to_string(idd)+".txt");
        auto begin = chrono::high_resolution_clock::now();    
        //read the elements of this person
        bins_shares = generate_shares_2(elements, i+1, num_bins, max_bin_size, c2, keyholder_context);
        auto end = chrono::high_resolution_clock::now();    
        auto dur = end - begin;
        auto ms = chrono::duration_cast<chrono::milliseconds>(dur).count();
        // cout << ms << " miliseconds" << endl;
        sum_sharegen += ms;
        for (int j=0; j<num_bins;j++){
            bins_people_shares[j].push_back(bins_shares[j]);
        }
    }

    cout << endl << "Generating shares 2 complete in " << sum_sharegen/m << " miliseconds on average for each party (including padding)" << endl;

    vector<ZZ> ans;
    int sum = 0;
    auto begin = chrono::high_resolution_clock::now();    
    for (int i=0;i<num_bins;i++){
        ans = recon2_in_bin_x(bins_people_shares[i], c2, keyholder_context.key_mac, m, max_bin_size);
        sum += ans.size();
    }
    auto end = chrono::high_resolution_clock::now();    
    auto dur = end - begin;
    auto ms = chrono::duration_cast<chrono::milliseconds>(dur).count();

    cout << "Reconstruction 2 complete in " << ms << " miliseconds" << endl; 

    cout << "Found " << sum << " elements in t-threshold intersection" << endl;

    //write results to file

}

int main(){
    int m=10, n=10, t=2;
    run_benchmark_1(m,n,t,80, false);
    cout << endl;
    //run_benchmark_2(m,n,t,80);

}