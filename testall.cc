#include<fstream>
#include<cstdlib>
#include<string>
#include<iostream>

using namespace std;
int main(){
    for(int i=5; i<=15; i++){
        #pragma omp num_threads(12) for
        for(int j=1; j<=3; j++){
            string exec = "./TSP ./data/n_" + to_string(i) + "/case_" + to_string(j) + ".txt ";
            string BB = "./data/n_" + to_string(i) + "/BB1_" + to_string(j) + ".txt ";
            string BF = "./data/n_" + to_string(i) + "/result/BF_" + to_string(j) + ".txt ";
            cout<<exec+BB<<'\n';
            system((exec+BB).c_str());
            //cout<<exec2+BF<<'\n';
            //system((exec2+BF).c_str());
        }
    }
}