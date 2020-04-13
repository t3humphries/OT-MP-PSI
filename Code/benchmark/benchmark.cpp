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
#include <chrono>
#include "../global/psi_utils.h"
#include "../client/Recon.h"
#include "../client/Elementholder.h"
#include "../client/client.h"

using namespace NTL;
using namespace std;
using json = nlohmann::json;

inline bool exists(const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

string get_dirname(int m, int n, int t, int bitsize){
    return "benchmark_"+to_string(m)+to_string(n)+to_string(t)+to_string(bitsize);//TODO
}

string generate_benchmark_context(int m, int n, int t, int bitsize, bool force=false){

    string dirname = get_dirname(m,n,t,bitsize);
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

Share get_fast_share_1(Context context, int X, int num_bins, Keyholder keyholder, int id)
{
    ZZ_p::init(context.p);
    ZZ_p share = ZZ_p(0);
    for(int i=1; i<context.t; i++)
    {
        share = share + (ZZ_p(NTL::power(hash_XX(ZZ(X) ,ZZ(context.p)),keyholder.randoms[i-1]))* ZZ_p(NTL::power(ZZ_p (id), ZZ(i)))); 
    }
    ZZ final = rep(share);
    return Share(
        ZZ(id),
        rep(hash_XX(ZZ(X), ZZ(num_bins))),
        ZZ(final)
    );

}

Share get_fast_share_2(Context context, int X, int num_bins, Keyholder keyholder, int id)
{
    ZZ_p::init(context.p);
    ZZ_p temp = ZZ_p(0), share;

    for(int i=1; i<context.t; i++)
    {
        ZZ_pPush push(context.p-1);
        temp = temp + (conv<ZZ_p>(keyholder.randoms[i-1]) * ZZ_p(NTL::power(ZZ_p(id), ZZ(i)))); 
    }

    share = ZZ_p(NTL::power(hash_XX(ZZ(X) ,ZZ(context.p)),rep(temp)));
    ZZ final = rep(share);
    return Share(
        ZZ(id),
        rep(hash_XX(ZZ(X), ZZ(num_bins))),
        ZZ(final)
    );
}


//generally needed
vector<vector<Share>> generate_shares_of_id(
    Elementholder elementholder,  Keyholder keyholder,
    int num_bins, int max_bin_size,
    Context context, client* elem_holder, int scheme,
    bool fast_sharegen
    ){
    vector<vector<Share>> shares_bins;
    Share share_x;
    for (int i=0;i<num_bins;i++){
        shares_bins.push_back(vector<Share>(0));
    }
    if(!fast_sharegen)
    {
        for (int i = 0; i< elementholder.num_elements; i++){
            if (scheme==1)
                share_x = elementholder.get_share_1(context, elementholder.elements[i], elem_holder, num_bins);    
            else
                share_x = elementholder.get_share_2(context, elementholder.elements[i], elem_holder, num_bins);
                shares_bins[conv<int>(share_x.bin)].push_back(share_x);
    }
}
    else
    {

        for (int i = 0; i< elementholder.num_elements; i++){
            if (scheme==1)
                share_x = get_fast_share_1(context, elementholder.elements[i], num_bins, keyholder, elementholder.id);
            else
                share_x = get_fast_share_2(context, elementholder.elements[i], num_bins, keyholder, elementholder.id);
                shares_bins[conv<int>(share_x.bin)].push_back(share_x);      
    }
}
    //padding the bins
    for (int i=0;i<num_bins;i++){
        while(shares_bins[i].size() < max_bin_size){
            shares_bins[i].push_back(Share(elementholder.id, i, context.p));
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

void write_shares_to_file(vector<vector<Share>> *bins_people_shares, string dirname, int schemetype, int num_bins, int m, int max_bin_size){
    string share_dirname=dirname + "//sharesofscheme"+ to_string(schemetype);
    if(exists(share_dirname)){
        system(("rm -r " + share_dirname).c_str());        
    }
    system(("mkdir " + share_dirname).c_str());//Generate the shares directory
    ofstream shares_file;
    for (int i =0; i<num_bins; i++)
    {
        system(("mkdir " + dirname + "//sharesofscheme"+ to_string(schemetype) +"//bin" + to_string(i)).c_str());
        for(int j=0; j<m; j++)
        {
            shares_file.open(dirname + "//sharesofscheme"+ to_string(schemetype) +"//bin" + to_string(i) +"//" + to_string(j) + ".json");
            if (!shares_file.is_open())
            {
                cout << "Something wrong for the shares of user " + to_string(j) << endl;
            } else
            {
                json temp;
                for (int k =0;k<max_bin_size;k++){
                    temp[k]["id"] = ZZ_to_str(bins_people_shares[i][j][k].id);
                    temp[k]["bin"] = ZZ_to_str(bins_people_shares[i][j][k].bin);
                    temp[k]["SS"] = ZZ_to_str(bins_people_shares[i][j][k].SS);
                }
                shares_file << temp;
            }
            shares_file.close();
        }
    }
}

void read_shares_from_file(vector<vector<Share>> *bins_people_shares, string dirname, int schemetype, int num_bins, int m, int max_bin_size){
    ifstream share_file;

    for(int i=0; i<num_bins; i++)
    {
        bins_people_shares[i] = vector<vector<Share>>();
        for(int j=0; j<m; j++)
        {
            bins_people_shares[i].push_back(vector<Share>());
            share_file.open(dirname + "//sharesofscheme"+ to_string(schemetype) +"//bin" + to_string(i) +"//" + to_string(j) + ".json");
            json temp;
            share_file>>temp;
            for (int k=0;k<max_bin_size;k++)
            {
                string str1;
                str1 = temp[k]["id"] ;
                string str2 ;
                str2= temp[k]["bin"];
                string str3;
                str3=temp[k]["SS"];

                Share __share(str_to_ZZ(str1),str_to_ZZ(str2),str_to_ZZ(str3));
                bins_people_shares[i][j].push_back(__share);

            }
            share_file.close();
        }
    }
}

void run_benchmark(int m, int n, int t, int bitsize, int schemetype, bool force=false, string server_address="127.0.0.1", bool log=false, bool only_sharegen=false, bool fast_sharegen=false){

    string dirname = generate_benchmark_context(m,n,t,bitsize);
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
   
    client *elem_holder = NULL;
    if(!fast_sharegen)
    {
        elem_holder = new client(server_address, log);
        elem_holder->send_to_server("INIT", keyholder.toString());
        cout << "Connected to server" << endl;
    } else {
        cout << "Proceeding without server (fast)" << endl;
    }
    
    cout << "-------------------- Scheme " << schemetype << " -------------------- " << endl;
    cout << "---------- Share Generation ---------- " << endl;
    for (int i=0;i<m;i++){
        idd=config["id_list"][i];
        elements = read_elements_to_vector(dirname + "/elements/"+ to_string(idd)+".txt");
        Elementholder elementholder(idd, elements.data(), (int)elements.size(), bitsize);
        auto begin = chrono::high_resolution_clock::now();            
        bins_shares = generate_shares_of_id(elementholder, keyholder, num_bins, max_bin_size, context, elem_holder,schemetype, fast_sharegen);     
        auto end = chrono::high_resolution_clock::now();    
        auto dur = end - begin;
        auto ms = chrono::duration_cast<chrono::milliseconds>(dur).count();
        sum_sharegen += ms;
        for (int j=0; j<num_bins;j++){
            bins_people_shares[j].push_back(bins_shares[j]);
        }
    }

    cout << "---------- Share Genration Complete  ----------" << endl;
    cout << "Average Share Generation time for each party: " << sum_sharegen/m << " miliseconds (including padding)" << endl;
    write_shares_to_file(bins_people_shares,dirname,schemetype,num_bins,m,max_bin_size);


    if(log)
    {
        ofstream log_file;
        log_file.open(dirname + "//logfile.txt",std::ofstream::out | std::ofstream::app);
        log_file << "-------------------- Scheme " << schemetype << " -------------------- ";
        log_file << "---------- Share Generation ---------- " << endl;
        log_file << "---------- Share Genration Complete  ----------" << endl;
        log_file << "\tAverage Share Generation time for each party: " << sum_sharegen/m << " miliseconds (including padding)" << endl;
        //log_file << elem_holder.get_message_sizes() << endl;
        log_file.close();
    }

    if (!only_sharegen){
        cout << "---------- Reconstruction ---------- " << endl;

        vector<vector<vector<int>>> ans;//3D binary vector where bin<user<is element in intersection>>>
        int sum = 0;
        auto begin = chrono::high_resolution_clock::now();    
        for (int i=0;i<num_bins;i++){
            ans.push_back(recon_in_bin_x(bins_people_shares[i], context, m, max_bin_size, schemetype, &sum));
        }
        auto end = chrono::high_resolution_clock::now();    
        auto dur = end - begin;
        auto ms = chrono::duration_cast<chrono::milliseconds>(dur).count();
        cout << "---------- Reconstruction complete ----------" << endl; 
        cout << "\tTotal time: " << ms << " miliseconds" << endl;
        //cout<< elem_holder.get_message_sizes() << endl;
        cout << "\tFound " << sum << " elements in t-threshold intersection" << endl;
   
        // if(log)
        // {
        //     ofstream log_file;
        //     log_file.open(dirname + "//logfile.txt",std::ofstream::out | std::ofstream::app);
        //     // log_file << "-------------------- Scheme " << schemetype << " -------------------- ";
        //     // log_file << "---------- Share Generation ---------- " << endl;
        //     // log_file << "---------- Share Genration Complete  ----------" << endl;
        //     // log_file << "\tAverage Share Generation time for each party: " << sum_sharegen/m << " miliseconds (including padding)" << endl;
        //     log_file << "---------- Reconstruction ---------- ";
        //     log_file << "---------- Reconstruction complete ----------" << endl; 
        //     log_file << "\tTotal time: " << ms << " miliseconds" << endl;        log_file << "\tFound " << sum << " elements in t-threshold intersection" << endl;
        //     //log_file << elem_holder.get_message_sizes() << endl;
        //     log_file.close();
        // }
    }
   //write results to file
}

void benchmark_generate_share(int t, int bitsize, int scheme, string server_ip="127.0.0.1", int repeat=100, bool log = false){

    cout << "---------- Generating Share with Scheme " << scheme << " ----------" << endl
            << "\tt=" << t
            << "\tbitsize=" << bitsize
            << "\trepeat=" << repeat << endl;

    ZZ p = read_prime(bitsize), g=read_generator(bitsize), q;
    q = (p-1)/2;

    Context context(p, q, g, t);

    Keyholder keyholder(context);
    Elementholder elementholder(rand()%1000 + 1, bitsize);
    client elem_holder(server_ip,log);//TODO change this to an arg??
    elem_holder.send_to_server("INIT", keyholder.toString());

    cout << "Connected to server" << endl;

    Share share_x;
    int num_bins=10;
    float duration[repeat] = {0.0};
    float time_avg, time_min, time_max, time_std, time_total;
    time_total = (float)0;
    time_max = time_total;
    time_min = (float)99999;
    
    for (int i = 0; i< repeat; i++){
        auto begin = chrono::high_resolution_clock::now();
        if (scheme==1)
            share_x = elementholder.get_share_1(context, rand()%10000, &elem_holder, num_bins); 
        else
            share_x = elementholder.get_share_2(context, rand()%10000, &elem_holder, num_bins);
        auto end = chrono::high_resolution_clock::now();    
    auto dur = end - begin;
    auto ms = chrono::duration_cast<chrono::milliseconds>(dur).count();
    time_total+=(float)ms;
    duration[i]=(float)ms;
    if(duration[i]>time_max)
        time_max=duration[i];
    if(duration[i]<time_min)
        time_min=duration[i];
    }
    
    time_avg = (float)time_total/repeat;
    
    float temp = (float)0;
    for (int i=0; i< repeat; i++)
    {
        temp+=(duration[i] - time_avg)*(duration[i] - time_avg);
    }
    temp  = (float)temp/repeat;
    time_std = sqrt(temp);

    cout << "Average time: " << (float)time_total/repeat << " ms" << endl;
    vector<int> comm=elem_holder.get_message_sizes(); 

    if (log){
        // ofstream log_file;
        ofstream log_file2;
        string dirname="benchmark_sharegen_"+to_string(t)+to_string(bitsize);
        if(!exists(dirname))
        {
            system(("mkdir " + dirname).c_str());
        }
        // log_file.open(dirname+"//logfile.txt",std::ofstream::out | std::ofstream::app);
        // log_file << "---------- Generating Share with Scheme " << scheme << " ----------" << endl
        //         << "\tt=" << t
        //         << "\tbitsize=" << bitsize
        //         << "\trepeat=" << repeat << endl;

        // log_file << "Average time=" << (float)time_total/repeat << " ms" << endl;
        // //log_file << elem_holder.get_message_sizes() << endl;
        // log_file.close();


        log_file2.open(dirname+"//Sharegen"+to_string(t)+to_string(scheme)+".json");
        json outputlog;
        outputlog["scheme"] = scheme;
        outputlog["repeat"] = repeat;
        outputlog["t"]=t;
        if(scheme==1){
            outputlog["s1time_avg"]= time_avg;
            outputlog["s1time_max"] = time_max;
            outputlog["s1time_min"] = time_min;
            outputlog["s1time_std"] = time_std;
            outputlog["s1r1sent"]= comm[0];
            outputlog["s1r1recv"] = comm[1];
            outputlog["s1r2sent"] = comm[2];
            outputlog["s1r2recv"] = comm[3];
            outputlog["s1comm_total"] = comm[0]+comm[1]+comm[2]+comm[3];
        }
        else{
            outputlog["s2time_avg"]= time_avg;
            outputlog["s2time_max"] = time_max;
            outputlog["s2time_min"] = time_min;
            outputlog["s2time_std"] = time_std;
            outputlog["s2r1sent"]= comm[0];
            outputlog["s2r1recv"] = comm[1];
            outputlog["s2comm_total"] = comm[0]+comm[1];
        }
        log_file2 << outputlog;
        log_file2.close();

        cout << "Results written at " + dirname + "//Sharegen"+to_string(t)+to_string(scheme)+".json" << endl;
    }

    return;
}

void benchmark_reconstruction_single_bin(int m, int n, int t, int bitsize, int schemetype, int repeat=1,bool log=false){
    cout << "---------- Reconstructing in single bin with Scheme " << schemetype << " ----------" << endl
            << "\tm=" << m
            << "\tn=" << n
            << "\tt=" << t
            << "\tbitsize=" << bitsize
            << "\trepeat=" << repeat << endl;

    ZZ p = read_prime(bitsize), g=read_generator(bitsize), q;
    q = (p-1)/2;

    string dirname=get_dirname(m,n,t,bitsize);

    if (!exists(dirname)){
        cout << "Neccessary shares for running this benchmark doesn't exist." << endl
            << "run \"./benchmark all\" first" << endl;
        return;
    }

    ifstream config_file(dirname + "//benchmark_config.json");
    json config;
    config_file >> config;
    int num_bins=config["num_bins"], max_bin_size=config["max_bin_size"];

    Context context(p, q, g, t);

    vector<vector<Share>> bins_people_shares[num_bins];
    read_shares_from_file(bins_people_shares,dirname,schemetype,num_bins,m,max_bin_size);
    
    vector<vector<int>> ans;//2D binary vector where user<is element in intersection>>
    int sum = 0;//Todo is this not outputted??
    float duration[repeat] = {0.0};
    float time_avg, time_min, time_max, time_std, time_total;
    time_total = (float)0;
    time_max = time_total;
    time_min = (float)99999;
    for (int i=0;i<repeat;i++){
        auto begin = chrono::high_resolution_clock::now();
        int random_bin = rand()%num_bins;
        ans = recon_in_bin_x(bins_people_shares[random_bin], context, m, max_bin_size,schemetype, &sum);
        auto end = chrono::high_resolution_clock::now();    
    auto dur = end - begin;
    auto ms = chrono::duration_cast<chrono::milliseconds>(dur).count();
    time_total+=(float)ms;
    duration[i]=(float)ms;
    if(duration[i]>time_max)
        time_max=duration[i];
    if(duration[i]<time_min)
        time_min=duration[i];
    }
   
    cout << "\tReconstruction complete in " << (float)time_total/repeat << " miliseconds" << endl;
    time_avg = (float)time_total/repeat;
    float temp = (float)0;
    for (int i=0; i< repeat; i++)
    {
        temp+=(duration[i] - time_avg)*(duration[i] - time_avg);
    }
    temp  = (float)temp/repeat;
    time_std = sqrt(temp);

    if (log){
        // ofstream log_file;
        ofstream log_file2;
        // log_file.open(dirname+"//recon_logfile.txt",std::ofstream::out | std::ofstream::app);
        // log_file << "---------- Reconstructing in single bin with Scheme " << schemetype << " ----------" << endl
        //         << "\tm=" << m
        //         << "\tn=" << n
        //         << "\tt=" << t
        //         << "\tbitsize=" << bitsize
        //         << "\trepeat=" << repeat << endl;
        // log_file << "\tReconstruction complete in " << (float)time_total/repeat << " miliseconds" << endl;
        // log_file.close();
        log_file2.open(dirname+"//Recon"+to_string(m)+to_string(n)+to_string(t)+to_string(schemetype)+".json");

        json outputlog;
        
            outputlog["scheme"] = schemetype;
            outputlog["repeat"] = repeat;
            outputlog["t"]=t;
            outputlog["m"]=m;
            outputlog["n"]=n;
            if (schemetype==1){
                outputlog["s1time_avg"]= time_avg;
                outputlog["s1time_max"] = time_max;
                outputlog["s1time_min"] = time_min;
                outputlog["s1time_std"] = time_std;
            } else {
                outputlog["s2time_avg"]= time_avg;
                outputlog["s2time_max"] = time_max;
                outputlog["s2time_min"] = time_min;
                outputlog["s2time_std"] = time_std;
            }
            log_file2<<outputlog;
        log_file2.close();
        cout << "Results written at " + dirname+"//Recon"+to_string(m)+to_string(n)+to_string(t)+to_string(schemetype)+".json" << endl;
    }
}

void show_usage(){
    cerr << "Usage:\n"
            << "Commands:\n"
            << "\tall: run the entire benchmarking process, generate all share, reconstruct in all bins\n"
            << "\tsharegen: run a single share generation process, micro-benchmark\n"
            << "\trecon: run the reconstruction process in only one bin, micro-benchmark\n"
            << "Options:\n"
            << "\t-h\tShow this help message\n"
            << "\t-m\tNumber of parties (default=10)\n"
            << "\t-n\tMax number of elements (default=10)\n"
            << "\t-t\tThreshold (default=2)\n"
            << "\t-b\tPrime bit length (default=2048)\n"
            << "\t-f\tRegenerate elements for benchmarking instance (default=false)\n"
            << "\t-k\tAddress of keyholder server (default 127.0.0.1)\n"
            << "\t-r\tNumber of times to repeat experiment (has defaults)\n"
            << "\t-s\tChoice of Scheme 0=both,1,2 (default 0) \n"
            << "\t-l\tLog results in file (default false) \n"
            << "\t-x\tDon't run the reconstruction in the \"all\" command \n"
            << "\t-y\tFast Share Generation in the \"all\" command, for generating shares fast, without the actual protocol"
            << endl;
}


int main(int argc, char *argv[]){ 
    int m=10, n=10, t=2, bitsize=2048, repeat=1, scheme=0;
    bool force=false,log=false, only_sharegen=false, fast_sharegen=false;
    string server_address="127.0.0.1";
    int opt;
    if (argc < 2){
        show_usage();
        return 0;
    }
    if (strcmp(argv[1],"help")==0){
        show_usage();
        return 0;
    }else if(strcmp(argv[1],"all")==0){
        std::cout << "Running entire protocol" << endl;
        while((opt = getopt(argc, argv, ":hm:n:t:b:fk:s:lxy")) != -1)  
        {  
            switch(opt)  
            {  
                case 'h':
                    show_usage();
                    return 0;
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
                case 'k':
                    server_address=optarg;
                    break;
                case 's':
                    scheme=stoi(optarg);
                    break;
                case 'l':
                    log=true;
                    break;
                case 'x':
                    only_sharegen=true;
                    break;
                case 'y':
                    fast_sharegen=true;
                    break;
                case ':':
                    cout << optarg << endl;
                    printf("option needs a value\n");  
                    break;
                case '?':
                    printf("unknown option: %c\n", optopt); 
                    break;  
            }
        }
        if (scheme==0){
            run_benchmark(m,n,t,bitsize,1,force,server_address,log,only_sharegen,fast_sharegen);
            cout << endl;
            run_benchmark(m,n,t,bitsize,2,force,server_address,log,only_sharegen,fast_sharegen);
        }else{
            run_benchmark(m,n,t,bitsize,scheme,force,server_address,log,only_sharegen,fast_sharegen);
        }
        return 0;
    }else if(strcmp(argv[1],"sharegen")==0){
        std::cout << "Running Share Generation" << endl;
        repeat=10;
        while((opt = getopt(argc, argv, ":ht:b:k:s:r:l")) != -1)  
        {  
            switch(opt)  
            {  
                case 'h':
                    show_usage();
                    return 0;
                case 't':  
                    t = stoi(optarg);
                    break;
                case 'b':  
                    bitsize = stoi(optarg);
                    break;
                case 'k':
                    server_address=optarg;
                    break;  
                case 's':
                    scheme=stoi(optarg);
                    break;
                case 'r':
                    repeat=stoi(optarg);
                    break;
                case 'l':
                    log=true;
                    break;
                case ':':  
                    printf("option needs a value\n");  
                    break;
                case '?':
                    printf("unknown option: %c\n", optopt); 
                    break;  
            }
        }
        if (scheme==0){
            benchmark_generate_share(t,bitsize,1,server_address,repeat,log);
            benchmark_generate_share(t,bitsize,2,server_address,repeat,log);
        }else{
            benchmark_generate_share(t,bitsize,scheme,server_address,repeat,log);
        }
        return 0;
    }else if(strcmp(argv[1],"recon")==0){
        std::cout << "Running Reconstruction" << endl;
        while((opt = getopt(argc, argv, ":hm:n:t:b:s:r:l")) != -1)  
        {  
            switch(opt)  
            {  
                case 'h':
                    show_usage();
                    return 0;
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
                case 's':
                    scheme=stoi(optarg);
                    break;
                case 'r':
                    repeat=stoi(optarg);
                    break;
                case 'l':
                    log=true;
                    break;
                case ':':  
                    printf("option needs a value\n");  
                    break;
                case '?':
                    printf("unknown option: %c\n", optopt); 
                    break;  
            }
        }
        if (scheme==0){
            benchmark_reconstruction_single_bin(m,n,t,bitsize,1,repeat,log);
            benchmark_reconstruction_single_bin(m,n,t,bitsize,2,repeat,log);
        }else{
            benchmark_reconstruction_single_bin(m,n,t,bitsize,scheme,repeat,log);
        }        
        return 0;
    }else{
        cerr << argv[1] << " is not a command" << endl;
        return 0;
    }
    return 0; 
} 