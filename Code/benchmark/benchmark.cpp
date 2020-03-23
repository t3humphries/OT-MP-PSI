#include <sys/stat.h>
#include <unistd.h>
#include <cstdio>
#include <math.h>
#include <NTL/ZZ_p.h>
#include <NTL/ZZ.h>
#include "../global/nlohmann/json.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include <vector>
#include "../global/psi_utils.h"
#include "../client/Recon.h"
#include "../client/Elementholder.h"
#include <chrono>

using namespace NTL;
using namespace std;
using json = nlohmann::json;

inline bool exists(const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

string generate_benchmark_context(int m, int n, int t, int bitsize, bool force=false){

    string dirname = "benchmark_"+to_string(m)+to_string(n)+to_string(t)+to_string(bitsize);//TODO
    if (!force && exists(dirname)){
        return dirname;
    }
    else if(force & exists(dirname)){
        system(("rm -r " + dirname).c_str());        
    }
    system(("mkdir " + dirname).c_str());
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
        __num_elements__ = 1 + rand() % n;
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

    ZZ p = read_prime(bitsize);
    KeyholderContext keyholder_context;
    keyholder_context.initialize_context(ZZ(p)-1, t);
    keyholder_context.write_to_file(dirname + "/keyholder_context.json");

    return dirname;
}

//generally needed
vector<vector<Share>> generate_shares_of_id(
    Elementholder elementholder,
    int num_bins, int max_bin_size,
    Context public_context, client elem_holder, int scheme
    ){
    vector<vector<Share>> shares_bins;
    Share share_x;
    for (int i=0;i<num_bins;i++){
        shares_bins.push_back(vector<Share>(0));
    }

    for (int i = 0; i< elementholder.num_elements; i++){
        if (scheme==1)
            share_x = elementholder.get_share_1(public_context, elementholder.elements[i], elem_holder, num_bins); 
        else
            share_x = elementholder.get_share_2(public_context, elementholder.elements[i], elem_holder, num_bins);
        shares_bins[conv<int>(share_x.bin)].push_back(share_x);
    }
    //padding the bins
    for (int i=0;i<num_bins;i++){
        while(shares_bins[i].size() < max_bin_size){
            shares_bins[i].push_back(Share(ZZ(elementholder.id), ZZ(i), public_context.p));
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

void run_benchmark(int m, int n, int t, int bitsize, string dirname, int schemetype, bool force=false){

    ZZ p = read_prime(bitsize), g=read_generator(bitsize), q;
    q = (p-1)/2;

    ifstream config_file(dirname + "//benchmark_config.json");
    json config;
    config_file >> config;
    
    int num_bins=config["num_bins"], max_bin_size=config["max_bin_size"];
    Context context(p, q, g, t);

    //ShareGen
    vector<vector<Share>> bins_shares;
    vector<vector<Share>> bins_people_shares[num_bins];
    for (int i=0;i<num_bins;i++){
        bins_people_shares[i] = vector<vector<Share>>();
    }
    vector<int> elements;
    int idd;
    int sum_sharegen = 0;

    Keyholder keyholder;
    keyholder.initialize_from_file(context, dirname + "/keyholder_context.json");

    //Initialize connection to server
    client elem_holder("127.0.0.1");//TODO change this to an arg??
    elem_holder.send_to_server("INIT", keyholder.toString());

    cout << "Generating type " << schemetype << " shares for party ";
    for (int i=0;i<m;i++){
        idd=config["id_list"][i];
        elements = read_elements_to_vector(dirname + "/elements/"+ to_string(idd)+".txt");
        Elementholder elementholder(idd, elements.data(), (int)elements.size());
        cout << idd << ",";
        auto begin = chrono::high_resolution_clock::now();    
        //read the elements of this person
        bins_shares = generate_shares_of_id(elementholder, num_bins, max_bin_size, context, elem_holder,schemetype);
        auto end = chrono::high_resolution_clock::now();    
        auto dur = end - begin;
        auto ms = chrono::duration_cast<chrono::milliseconds>(dur).count();
        sum_sharegen += ms;
        for (int j=0; j<num_bins;j++){
            bins_people_shares[j].push_back(bins_shares[j]);
        }
    }

    cout << "\b" << endl << "Generating shares complete in " << sum_sharegen/m << " miliseconds on average for each party (including padding)" << endl;

    vector<ZZ> ans;
    int sum = 0;
    auto begin = chrono::high_resolution_clock::now();    
    for (int i=0;i<num_bins;i++){
        if (schemetype==1)
            ans = recon1_in_bin_x(bins_people_shares[i], context, m, max_bin_size);
        else
            ans = recon2_in_bin_x(bins_people_shares[i], context, m, max_bin_size);
        sum += ans.size();
    }
    auto end = chrono::high_resolution_clock::now();    
    auto dur = end - begin;
    auto ms = chrono::duration_cast<chrono::milliseconds>(dur).count();

    cout << "Reconstruction complete in " << ms << " miliseconds" << endl; 

    cout << "Found " << sum << " elements in t-threshold intersection" << endl;

   //write results to file

}

int main(int argc, char *argv[])  
{ 
    int m=10, n=10, t=2, bitsize=1024;
    // int m, n, t, bitsize=1024;
    bool force=false;
    int opt; 

    // put ':' in the starting of the 
    // string so that program can  
    //distinguish between '?' and ':'  
    while((opt = getopt(argc, argv, ":m:n:t:b:f")) != -1)  
    {  
        switch(opt)  
        {  
            case 'm':  
                m = stoi(optarg);
                break;
            case 'n':  
                n = stoi(optarg);
                break;
            case 't':  
                t = stoi(optarg);
                break;
            case 'b':  
                bitsize = stoi(optarg);
                break;
            case 'f':  
                force=true;
                break;  
            case ':':  
                printf("option needs a value\n");  
                break;
            case '?':
                printf("unknown option: %c\n", optopt); 
                break;  
        }  
    }  
      
    // optind is for the extra arguments 
    // which are not parsed 
    for(; optind < argc; optind++){      
        printf("extra arguments: %s\n", argv[optind]);  
    } 
    
    string dirname = generate_benchmark_context(m,n,t,bitsize);
    run_benchmark(m,n,t,bitsize,dirname,1,force);
    cout << endl;
    run_benchmark(m,n,t,bitsize,dirname,2,force);
    // run_benchmark_2(m,n,t,bitsize,dirname,false);

    return 0; 
} 