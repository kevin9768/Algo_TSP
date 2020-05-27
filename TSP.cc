#include<fstream>
#include<vector>
#include<algorithm>
#include<ctime>
#include<queue>
#include<iostream>

using namespace std;

struct Node{
    vector<pair<int, int> > path;
    vector<vector<int> > weight;
    int bound;
    int level;
};

Node* newNode(vector<pair<int,int> > _path, vector<vector<int> > _weight, int _bound, int _level){
    Node* n = new Node;
    n->path = _path;
    n->weight = _weight;
    n->bound = _bound;
    n->level = _level;
    return n;
}
Node* newNode(vector<vector<int> > _weight, int _bound, int _level){
    Node* n = new Node;
    n->weight = _weight;
    n->bound = _bound;
    n->level = _level;
    return n;
}

bool cmp(pair<int,int> a, pair<int,int> b){
    return a.first<b.first;
}

bool feasible(vector<pair<int, int> > edges, int N){
    if(edges.size()==0 || edges.size()==1) return true;
    vector<int> res;

    sort(edges.begin(), edges.end(), cmp);
    int last = N;

    vector<pair<int,int> > reconstruct(N+10, pair<int,int>(-1,-1));
    int it=0;
    for(int i=0; i<last; i++){
        if(edges[it].first==i){
            reconstruct[i] = edges[it++];
        }
    }
    int cycle_count = 0;
    bool idle = false;
    for(int i=0; i<last; i++){
        it=i;
        while(1){
            if(it>=last)    break;
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
int reduction(vector<vector<int> > &weight);
vector<int> arc_to_array(vector<pair<int,int> > &edges){
    sort(edges.begin(), edges.end(), cmp);
    vector<int> res;

    int it=0;
    for(int i=0; i<edges.size(); i++){  
        res.push_back(edges[it].first);
        it = edges[it].second;
    }

    res.push_back(edges[it].first);
    return res;
}

//return (row,col), cost
pair<pair<int,int>,int> select_arc(vector<vector<int> >& weight){
    
    int row=-1,col=-1;
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
            if(min_row==INT32_MAX || min_col==INT32_MAX)    continue;
            if(min_row+min_col > max_cost){
                row = i, col = j;
                max_cost = min_row + min_col;
            }
        }
    }


    if(max_cost==INT32_MIN){
        int min=INT32_MAX;
        for(int i=0; i<n; i++){
            for(int j=0; j<n; j++){
                if(weight[i][j]!=-1){
                    if(weight[i][j]<min){
                        row = i, col = j;
                        min = weight[i][j];
                    }
                }
            }
        }
        max_cost = min;
    }

    return pair<pair<int,int>,int>(pair<int,int>(row, col), max_cost);
}

int reduction(vector<vector<int> > &weight){
    int low_bound = 0;
    int n = weight.size();

    for(int i=0; i<n; i++){
        int min_row = INT32_MAX;
        for(int j=0; j<n; j++){
            if(weight[i][j]==-1)    continue;
            if(min_row==0)  break;
            min_row = min(min_row, weight[i][j]);
        }
        if(min_row==INT32_MAX)  continue;

        for(int j=0; j<n; j++){
            if(weight[i][j]==-1)            continue;
            if(weight[i][j]==INT32_MAX)     continue;
            weight[i][j] -= min_row;
        }
        low_bound += min_row;
    }

    for(int i=0; i<n; i++){
        int min_col = INT32_MAX;
        for(int j=0; j<n; j++){
            if(weight[j][i]==-1)            continue;
            if(min_col==0)  break;
            min_col = min(min_col, weight[j][i]);
        }
        if(min_col==INT32_MAX)  continue;

        for(int j=0; j<n; j++){
            if(weight[j][i]==-1)            continue;
            if(weight[j][i]==INT32_MAX)     continue;
            weight[j][i] -= min_col;
        }
        low_bound += min_col;
    }
    return low_bound;
}


void BranchNBound(vector<vector<int> >& weight, ofstream &BB){
    clock_t start,end;

    start = clock();
    int n = weight.size();
    int upper_bound = INT32_MAX;
    vector<pair<int,int> > opt_path;
    vector<vector<int> > reduced = weight;
    int low_bound = reduction(reduced);

    stack<Node*> s;
    Node* root = newNode(reduced, low_bound, 0);
    s.push(root);

    while(!s.empty()){
        Node* cur = s.top();
        s.pop();

        if(cur->level == n){
            upper_bound = cur->bound;
            opt_path = cur->path;
            continue;
        }

        
        pair<pair<int,int>,int> arc = select_arc(cur->weight);
        if(arc.first.first==-1 || arc.first.second==-1 ||arc.second==INT32_MAX) continue;

        vector<pair<int,int> > l_path, r_path;
        vector<vector<int> > l_weight, r_weight;
        int l_bound = cur->bound, r_bound = cur->bound;
        l_weight = cur->weight;
        r_weight = cur->weight;

        //l_sub
        l_path = cur->path;
        l_path.push_back(arc.first);
        l_weight[arc.first.second][arc.first.first] = -1;
        for(int i=0; i<n; i++){
            l_weight[arc.first.first][i] = -1;
            l_weight[i][arc.first.second] = -1;
        }
        l_bound += reduction(l_weight);
        int ll = cur->level + 1;
        Node* left = newNode(l_path, l_weight, l_bound, ll);

        //r_sub
        r_path = cur->path;
        r_weight[arc.first.first][arc.first.second] = -1;
        r_bound += arc.second;
        reduction(r_weight);

        Node* right = newNode(r_path, r_weight, r_bound, cur->level);

        
        if(r_bound<upper_bound)
            s.push(right);
        if(feasible(l_path, n))
            if(l_bound<upper_bound)
                s.push(left);

    }

    vector<int> order = arc_to_array(opt_path);

    BB<<"Path: "<<order[0];
    for(int i=1; i<order.size(); i++)
        BB<<"->"<<order[i];
    BB<<'\n';
    BB<<"Cost: "<<upper_bound<<'\n';
    BB<<"Execution Time: "<<(double)(end - start)/CLOCKS_PER_SEC<<" sec\n";

}

void BruteForce(vector<vector<int> > weight, ofstream &BF){
    clock_t start,end;

    start = clock();
    int n = weight.size();
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
    }while(next_permutation(vertex.begin()+1, vertex.end()));
    end = clock();

    BF<<"Path: "<<min_path[0]+1;
    for(int i=1; i<n; i++)
        BF<<"->"<<min_path[i]+1;
    BF<<"->"<<min_path[0]+1<<'\n';
    BF<<"Cost: "<<min<<'\n';
    BF<<"Execution Time: "<<(double)(end - start)/CLOCKS_PER_SEC<<" sec\n";
}

int main(int argc, char** argv){
    ifstream input;
    ofstream BF, BB;
    //input.open("input.txt");
    //BF.open("BF.txt");
    input.open(argv[1]);
    //BF.open(argv[2]);
    BB.open(argv[2]);

    int n;
    input>>n;

    vector<vector<int> > weight(n, vector<int>(n, 0));
    for(auto &i: weight)
        for(auto &j : i)
            input>>j;


    //Brute Force
    //BruteForce(weight, BF);

    //Branch and Bound
    BranchNBound(weight, BB);
}