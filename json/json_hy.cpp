#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <sstream>
using namespace std;//std包含stl

#define INT 0
#define DOU 1
#define STR 2
#define BOOL 3
#define VET 4
#define MAP 5

class Json{
public:
    int int_val;
    double dou_val;
    string str_val;
    bool bool_val;

    vector<Json> vet;
    map<Json,Json> map;

    int type;


    //被析构函数调用
    void del(){
        if(type == STR) str_val.clear();
        if(type == VET) vet.clear();
        if(type == MAP) map.clear();  
    }

    Json() {}
    ~Json(){
        del();
    }

    Json(const int& x){
        del();//类型改变之前先释放
        int_val=x;
        type=INT;
    }

    Json(const double& x){
        del();//类型改变之前先释放
        dou_val=x;
        type=DOU;
    }

    Json(const string& x){
        del();//类型改变之前先释放
        str_val=x;
        type=STR;
    }

    Json(const char& x){
        del();//类型改变之前先释放
        type=STR;
        str_val=string(x);
    }

    Json(const bool& x){
        del();//类型改变之前先释放
        bool_val=x;
        type=BOOL;
    }

    void add(Json x){
        if(type == VET){
            vet.push_back(x);
        }
    }

    void put(Json key,Json val){
        if(type == MAP){
            map[key]=val;       
        }
    }

};

ostream& operator <<(ostream& out,const Json& v){
    if( v.type == INT){
        out<<v.int_val;
    }
    if(v.type == DOU){
        out<<v.dou_val;
    }
    if(v.type == STR){
        out<<"\""<<v.str_val<<"\"";
    }
    if(v.type ==BOOL){
        if(v.bool_val){
            out<<"true";
        }
        else{
            out<<"false";
        }
    }

    if(v.type == VET){
        out<<"[";
        for(int i=0;i<v.vet.size();i++){
            if(i){
                out<<",";
            }
            out<<v.vet[i];
        }
        out<<"]";
    }
    if(v.type == MAP){
        out<<"{";
        for(auto it=v.map.begin();it!=v.map.end();it++){
            if(it!=v.map.begin()){
                out<<",";
            }
            out<<it->first<<":"<<it->second;
        }
        out<<"}";
    }
    return out;
}

bool operator < (const Json& a,const Json& b){
    if(a.type!=b.type){
        return a.type<b.type;
    }
    if(a.type ==INT)    return a.int_val<b.int_val;
    if(a.type ==DOU)    return a.dou_val<b.dou_val;
    if(a.type ==STR)    return a.str_val<b.str_val;
    if(a.type ==VET)    return a.vet<b.vet;
    if(a.type ==MAP)    return a.map<b.map;
}

stringstream ss;

Json parser_val(){
    while(ss.peek()!=-1){
        if(ss.peek()==' '||ss.peek()=='\n'||ss.peek()=='\t')    ss.get();
        else if(ss.peek()=='"') return parser_str();
        else if(ss.peek()=='f'||ss.peek()=='t') return parser_bool();
        else if(ss.peek()=='[') return parser_arr();
        else if(ss.peek()=='{') return parser_map();
        else return parser_num();
    }

    return 0;
}

//解析数字
Json parser_num(){
    string s;
    while(isdigit(ss.peek())||ss.peek()=='e'||ss.peek()=='-'||ss.peek()=='+'){
        s.push_back(ss.get());
    }

    if(count(s.begin(),s.end(),'.')||count(s.begin(),s.end(),'e'))
        return stof(s);//string转float
    else
        return stoi(s);//string转int
}

Json parser_str(){
    ss.get();
    string s;
    while(ss.peek()!='"')
        s.push_back(ss.get());
    ss.get();
    return Json(s);
}

Json parser_bool(){
    if(ss.peek()=='f'){
        ss.get();ss.get();ss.get();ss.get();ss.get();
        return Json(false);
    }
    else{
        ss.get();ss.get();ss.get();ss.get();
        return Json(true);
    }
}

Json parser_arr(){
    ss.get();
    Json vec;
    vec.type = VET;
    while(ss.peek()!=']'){
        vec.add(parser_val());
        while(ss.peek()!=']'&&(ss.peek()==' '||ss.peek()=='\t'||ss.peek()=='\n'||ss.peek()==',')){
            ss.get();
        }
    }

    ss.get();
    return vec;
}

Json parser_map(){
    ss.get();
    Json dict;
    dict.type = MAP;
    while(ss.peek()!='}'){
        Json key=parser_val();
        while(ss.peek()==' '||ss.peek()==':')
            ss.get();
        Json val=parser_val();
        dict.put(key,val);
        while(ss.peek()!='}'&&(ss.peek()!=' '||ss.peek()!='\t'||ss.peek()!='\n'||ss.peek()!=',')){
            ss.get();
        }
    }

    ss.get();
    return dict;
}

Json parser(string s){
    ss.clear();
    ss<<s;
    return parser_val();
}

int main(){
    
    return 0;
}