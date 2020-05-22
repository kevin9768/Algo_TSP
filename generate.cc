#include<fstream>
#include<string>
#include<ctime>
#include<cstdlib>
#include<iostream>

using namespace std;
int main(){
    srand(time(NULL));
    ofstream f;
    for(int i=5;i<=15;i++){
        for(int j=1;j<=3;j++){
            if(j==1){
                string dir = "mkdir .\\data\\n_" + to_string(i);
                system(dir.c_str());
            }
            string s=to_string(i), c=to_string(j);
            
            string out = ".\\data\\n_" + s + "\\case_" + c +".txt";
            cout<<out<<endl;
            f.open(out.c_str(), ios_base::trunc | ios_base::in);

            f<<i<<'\n';
            for(int k=0; k<i; k++){
                for(int l=0; l<i; l++){
                    if(k==l)
                        f<<-1;
                    else{
                        f<<(1 + rand()%99);
                    }
                    if(l==i-1)
                        f<<'\n';
                    else
                        f<<' ';
                }
            } 
            f.close();
        }
    }
}