#include <bits/stdc++.h>
using namespace std;

int main(){
    stringstream ss;
    string s="12124123213";
    ss<<s;
    while(ss.get()!=-1){
        cout<<ss.get()<<endl;
    }
    //cout<<ss.peek()<<endl;
    //cout<<ss.str()<<endl;

    return 0;
}