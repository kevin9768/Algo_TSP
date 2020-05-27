#include<fstream>
#include<vector>
#include<algorithm>
#include<ctime>
#include<iostream>

using namespace std;

int BB_UB = INT32_MAX;
int N;

//return (row,col), cost
pair<pair<int,int>,int> select_arc(vector<vector<int> >& weight){
    
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

vector<int> arc_to_array(vector<pair<int,int> > &edges, vector<vector<int> > weight){
    edges.begin()++;
    sort(edges.begin(), edges.end(), cmp);
    vector<int> res;

    int w=0;
    int it=0;
    for(int i=0; i<edges.size(); i++){
        //if(edges[i].first>=edges.size())    continue;  
        res.push_back(edges[it].first);
        w += weight[edges[it].first][edges[it].second];
        it = edges[it].second;
    }

    res.push_back(edges[it].first);
    res.push_back(w);
    return res;
}


//lower bound added to optimal solution
int reduction(vector<vector<int> > &weight){
    int low_bound = 0;
    int n = weight.size();

    //by row
    for(int i=0; i<n; i++){
        int min_row = INT32_MAX;
        for(int j=0; j<n; j++){
            if(weight[i][j]==-1)    continue;
            if(min_row==0)  break;
            min_row = min(min_row, weight[i][j]);
        }
        if(min_row==0||min_row==INT32_MAX)  continue;
        //cout<<"reduct row i: "<<i<<" by "<<min_row<<"\n";
        for(int j=0; j<n; j++){
            if(weight[i][j]==-1)    continue;
            weight[i][j] -= min_row;
        }
        low_bound += min_row;
    }

    //by col
    for(int i=0; i<n; i++){
        int min_col = INT32_MAX;
        for(int j=0; j<n; j++){
            if(weight[j][i]==-1)    continue;
            if(min_col==0)  break;
            min_col = min(min_col, weight[j][i]);
        }
        if(min_col==0||min_col==INT32_MAX)  continue;
        //cout<<"reduct col i: "<<i<<" by "<<min_col<<"\n";
        for(int j=0; j<n; j++){
            if(weight[j][i]==-1)    continue;
            weight[j][i] -= min_col;
        }
        low_bound += min_col;
    }

    return low_bound;
}

bool feasible(vector<pair<int, int> > edges){
    if(edges.size()==0 || edges.size()==1) return true;
    vector<int> res;

    sort(edges.begin(), edges.end(), cmp);
    int last = N;

    vector<pair<int,int> > reconstruct;
    int it=0;
    for(int i=0; i<last; i++){
        if(edges[it].first!=i)
            reconstruct.push_back(pair<int,int>(i, -1));
        else
            reconstruct.push_back(edges[it++]);
        
    }
    int cycle_count = 0;
    bool idle = false;
    for(int i=0; i<last; i++){
        it=i;
        while(1){
            it = reconstruct[it].second;
            if(it==-1){
                idle = true;
                break;
            }
            if(it==i){
                cycle_count++;
                break;
            }
        }
    }
    if(cycle_count>=1 && idle)
        return false;

    return true;
}

vector<pair<int,int> > BB_recur(vector<pair<int, int> > taken, vector<vector<int> > l_weight, int lo, int level){
    vector<pair<int,int> > res, res_l, res_r;
    if(lo>BB_UB)    return res;

    pair<pair<int,int>,int> arc = select_arc(l_weight);

    if(level==N-1){
        if(lo<BB_UB){
            BB_UB = lo;
            res.push_back(arc.first);
        }
        return res;
    }

    vector<vector<int> > r_weight = l_weight;
    
    bool left = false;
    //left
    taken.push_back(arc.first);
    if(feasible(taken)){
        left = true;
        for(int i=0; i<N; i++){
            l_weight[arc.first.first][i] = -1;
            l_weight[i][arc.first.second] = -1;
        }
        l_weight[arc.first.second][arc.first.first] = -1;
        int l_lo = lo + reduction(l_weight);
        res_l = BB_recur(taken, l_weight, l_lo, level+1);
    }

    //right
    taken.pop_back();
    r_weight[arc.first.first][arc.first.second] = -1;
    int r_lo = arc.second + lo;
    if(r_lo<BB_UB)
        res_r = BB_recur(taken, r_weight, r_lo, level);

    if(res_l.size() > res_r.size()){
        res = res_l;
    }
    else{
        res = res_r;
    }

    if(left)
        res.push_back(arc.first);

    if(level==0)
        for(auto i:res)
            cout<< i.first<<", "<<i.second<<'\n';

    return res;

}

vector<pair<int, int> > solve_BB(vector<vector<int> > weight){
    int low_bound = reduction(weight);
    vector<pair<int, int> > taken;


    return BB_recur(taken, weight, low_bound, 0);

}

int main(int argc, char** argv){
    clock_t start,end;
    ifstream input;
    ofstream BB;
    //input.open("text_book.txt");
    //BB.open("BB.txt");
    input.open(argv[1]);
    BB.open(argv[2]);

    input>>N;
    int n = N;
    vector<vector<int> > weight(n, vector<int>(n, 0));
    for(auto &i: weight)
        for(auto &j : i)
            input>>j;


    //BB
    start = clock();
    //reduction
    //vector<vector<int> > weight;
    int opt_lo_bound;
    vector<pair<int,int> > edges = solve_BB(weight);
    
    vector<int> seq = arc_to_array(edges, weight);
    
    end = clock();

    
    opt_lo_bound = seq[n+1];
    BB<<"Path: "<< seq[0];
    for(int i=1; i<=n; i++)
        BB<<"->"<<seq[i];
    BB<<"\nCost: "<<opt_lo_bound<<'\n';
    BB<<"Execution Time: "<<(double)(end - start)/CLOCKS_PER_SEC<<" sec\n";
}