#include<fstream>
#include<vector>
#include<algorithm>
#include<ctime>
#include<iostream>

using namespace std;
int main(int argc, char** argv){
    clock_t start,end;
    ifstream input;
    ofstream BF;
    //input.open("input.txt");
    //BF.open("BF.txt");
    input.open(argv[1]);
    BF.open(argv[2]);

    int n;
    input>>n;

    vector<vector<int>> weight(n, vector<int>(n, 0));
    for(auto &i: weight)
        for(auto &j : i)
            input>>j;


    //Brute Force
    start = clock();
    vector<int> vertex(n,0);

    for(int i=0; i<n; i++)
        vertex[i] = i;
    int min = INT32_MAX;
    vector<int> min_path;

    do{
        int cur_weight = 0;
        int k = vertex[0];
        for(int i=1; i<n; i++){
            cur_weight += weight[k][vertex[i]];
            k=vertex[i];
        }
        cur_weight += weight[k][vertex[0]];

        if(cur_weight<min){
            min = cur_weight;
            min_path = vertex;
        }
    }while(next_permutation(vertex.begin(), vertex.end()));
    end = clock();

    BF<<"Path: "<<min_path[0]+1;
    for(int i=1; i<n; i++)
        BF<<"->"<<min_path[i]+1;
    BF<<"->"<<min_path[0]+1<<'\n';
    BF<<"Cost: "<<min<<'\n';
    BF<<"Execution Time: "<<(double)(end - start)/CLOCKS_PER_SEC<<" sec\n";

}