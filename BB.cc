#include<fstream>
#include<vector>
#include<algorithm>
#include<ctime>
#include<iostream>

using namespace std;

//return (row,col), cost
pair<pair<int,int>,int> select_arc(vector<vector<int>> weight){
    
    int row,col;
    int max_cost = INT32_MIN;
    int n = weight.size();
    for(int i=0; i<n; i++){
        for(int j=0; j<n; j++){
            if(weight[i][j]==-1)    continue;
            
            int min_row = INT32_MAX, min_col = INT32_MAX;
            //Find the minimum element of the same row of (i,j)
            for(int c=0; c<n; c++){
                if(weight[i][c]==-1 || c==j)    continue;
                min_row = min(min_row, weight[i][c]);
            }
            //Find the minimum element of the same column of (i,j)
            for(int r=0; r<n; r++){
                if(weight[r][j]==-1 || r==i)    continue;
                min_col = min(min_col, weight[r][j]);
            }
            if(min_row+min_col > max_cost){
                row = i, col = j;
                max_cost = min_row + min_col;
            }
        }
    }
    return pair<pair<int,int>,int>(pair<int,int>(row, col), max_cost);
}

bool cmp(pair<int,int> a, pair<int,int> b){
    return a.first<b.first;
}

vector<int> arc_to_array(vector<pair<int,int>> &edges, vector<vector<int>> &weight){
    sort(edges.begin(), edges.end(), cmp);
    vector<int> res;

    int w=0;
    int it=0;
    for(int i=0; i<edges.size(); i++){
        res.push_back(edges[it].first);
        w += weight[edges[it].first][edges[it].second];
        it = edges[it].second;
    }

    //w += weight[edges[it].first][edges[it].second];
    res.push_back(edges[it].first);
    res.push_back(w);
    return res;
}

int main(int argc, char** argv){
    clock_t start,end;
    ifstream input;
    ofstream BB;
    //input.open("text_book.txt");
    //BB.open("BB.txt");
    input.open(argv[1]);
    BB.open(argv[2]);

    int n;
    input>>n;

    vector<vector<int>> weight(n, vector<int>(n, 0));
    for(auto &i: weight)
        for(auto &j : i){
            input>>j;
        }


    //BB
    start = clock();
    //reduction
    vector<vector<int>> reducted = weight;
    int opt_lo_bound = 0;
    vector<pair<int,int>> edges;
    
    for(int a=0; a<n; a++){
        //by row
        for(int i=0; i<n; i++){
            int min_row = INT32_MAX;
            for(int j=0; j<n; j++){
                if(reducted[i][j]==-1)    continue;
                min_row = min(min_row, reducted[i][j]);
            }
            for(int j=0; j<n; j++){
                if(reducted[i][j]==-1){
                    reducted[i][j] = -1;
                    continue;
                }
                reducted[i][j] = reducted[i][j] - min_row;
            }
            opt_lo_bound += min_row;
        }
        //by col
        for(int i=0; i<n; i++){
            int min_col = INT32_MAX;
            for(int j=0; j<n; j++){
                if(reducted[j][i]==-1)    continue;
                if(min_col==0)  break;
                min_col = min(min_col, reducted[j][i]);
            }
            if(min_col==0)  continue;
            for(int j=0; j<n; j++){
                if(reducted[j][i]==-1)    continue;
                reducted[j][i] = reducted[j][i] - min_col;
            }
            opt_lo_bound += min_col;
        }


        // for(auto &i: reducted){
        //     for(auto &j : i){
        //         if(j>(INT32_MAX)/2)
        //             cout<<-1;
        //         else 
        //             cout<<j;
        //         cout<<' ';
        //     }
        //     cout<<'\n';
        // }
        // cout<<opt_lo_bound<<'\n';
        
        pair<pair<int,int>,int> arc = select_arc(reducted);

        // cout<<arc.first.first<<" "<<arc.first.second<<" "<<arc.second<<'\n';
        reducted[arc.first.second][arc.first.first] = -1;

        for(int i=0; i<n; i++){
            reducted[arc.first.first][i] = -1;
            reducted[i][arc.first.second] = -1;
        }
        edges.push_back(arc.first);
    }

    // for(auto i: edges){
    //     cout<<i.first<<' '<<i.second<<'\n';
    // }
    vector<int> seq = arc_to_array(edges, weight);
    
    end = clock();

    BB<<"Path: "<< seq[0]+1;
    for(int i=1; i<=n; i++)
        BB<<"->"<<seq[i]+1;
    BB<<"\nCost: "<<seq[n+1]<<'\n';
    BB<<"Execution Time: "<<(double)(end - start)/CLOCKS_PER_SEC<<" sec\n";
}