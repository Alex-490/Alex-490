#include <iostream>
#include <algorithm>
#include<string>
#include<vector>	//存储数组
#include<map>		//存储对象
#include<sstream>	//流对象
#include<fstream>	//文件操作
 
using namespace std;
#define INT 0	//整形
#define DOU 1	//浮点型
#define STR 2	//字符串
#define BOOL 3	//布尔
#define LIST 4	//数组
#define DICT 5	//对象
class Json {
public:
	int INT_VAL;
	double DOU_VAL;
	string STR_VAL;
	bool BOOL_VAL;
	vector<Json>List;//用vector表示数组
	map<Json, Json>dict;//用map表示对象
 
 
	int type;//表示解析的类型
 
	void del() //del用于在更换变量类型的时候清除原来的变量
	{
		if (type == STR)STR_VAL.clear();
		if (type == LIST)List.clear();
		if (type == DICT)dict.clear();
	}
 
	Json() {}
	~Json()
	{
		del();//析构时释放内存
	}
 
	//各类构造函数
	Json(const int& x)
	{
		del();//类型改变前需要释放之前的
		INT_VAL = x;
		type = INT;//0
	}
	Json(const double& x) {
		del();
		DOU_VAL = x;
		type = DOU;//1
	}
	Json(const string& x) {
		del();
		STR_VAL = x;
		type = STR;//2
	}
	Json(const char* x) {
		del();
		type = STR;//3
		STR_VAL = string(x);
	}
	Json(const bool& x) {
		del();
		BOOL_VAL = x;
		type = BOOL;//4
	}
 
	//用列表初始化来初始化 动态类型
	Json(initializer_list<Json> l) //访问 const T 类型对象数组的轻量代理对象
	{
		del();
		type = LIST;//5
		for (Json x : l)
			List.push_back(x);
	}
 
	void add(Json x) //添加函数，将内容放入vector
	{
		if (type == LIST)
			List.push_back(x);
	}
 
	void put(Json key, Json val) //将内容放入map
	{
		if (type == DICT)
			dict[key] = val;
	}
 
	Json& operator [](Json i) {
		if (type == LIST)
			return List[i.INT_VAL];
		else 
			return dict[i];
	}
 
	string str();
};
 
 
//重载运算符，方便输出
ostream& operator << (ostream& out, const Json& v)
{
	if (v.type == INT)
		out << v.INT_VAL;
	if (v.type == DOU)
		out << v.DOU_VAL;
	if (v.type == STR)
		out << "\"" << v.STR_VAL << "\"";
	if (v.type == BOOL) 
	{
		if (v.BOOL_VAL)
			out << "true";
		else 
			out << "false";
	}
	if (v.type == LIST) //重载输出，格式要尽可能符合JSON的语法
	{
		out << "[";
		for (int i = 0; i < v.List.size(); i++) 
		{
			if (i)//第一个元素前面不用逗号
				out << ",";
			out << v.List[i];
		}
		out << "]";
	}
	if (v.type == DICT)
	{
		out << "{";
		for (auto it = v.dict.begin(); it != v.dict.end(); it++) 
		{
			if (it != v.dict.begin())//第一个元素前面不用逗号
				out << ",";
			out << it->first << ":" << it->second;//map以first和second表示键值对
		}
		out << "}";
	}
	return out;
}
 
string Json::str() 
{
	stringstream ss; 
	ss << (*this);//使用重载过后的<<
	return ss.str();
}
 
bool operator < (const Json& a, const Json& b) //重载 < 运算符，C++里的vector和map都已经重载了小于号
{
	if (a.type != b.type)
		return a.type < b.type;
	else 
	{
		if (a.type == INT)
			return a.INT_VAL < b.INT_VAL;
		if (a.type == DOU)
			return a.DOU_VAL < b.DOU_VAL;
		if (a.type == STR)
			return a.STR_VAL < b.STR_VAL;
		if (a.type == LIST)
			return a.List < b.List;
		if (a.type == DICT)
			return a.dict < b.dict;
		return true;
	}
}
 
stringstream ss;
//流类，将字符串转换为数字、将数字转换为字符串等
//peek() 返回当前下标所指向的字符
//get() 返回当前下标所指向的字符,并令下标加1，返回值为 -1 说明遍历完了
Json parser_val();//解析未知的类型
Json parser_num();
Json parser_str();
Json parser_bool();
Json parser_arr();
Json parser_map();
 
Json parser_val()
{
	while (ss.peek() != -1)//判断Json的类型
	{
		if (ss.peek() == ' ' || ss.peek() == '\n' || ss.peek() == '\t')
			ss.get();//吃掉
		else if (ss.peek() == '"')//字符串
		{
			return parser_str();
		}
		else if (ss.peek() == 'f' || ss.peek() == 't')//布尔
		{
			return parser_bool();
		}
		else if (ss.peek() == '[')//数组
		{
			return  parser_arr();
		}
		else if (ss.peek() == '{')//对象
		{
			return parser_map();
		}
		else
		{
			return parser_num();//数字
		}
	}
	return 0;
}
 
Json parser_num()//解析数字
{
	string s;
	//isdigit 判断一个字符是否为数字
	while (isdigit(ss.peek()) || ss.peek() == 'e' || ss.peek() == '-' || ss.peek() == '+')//判断数字
		s.push_back(ss.get());//std::string::pushi_back()
	if (count(s.begin(), s.end(), '.') || count(s.begin(), s.end(), 'e'))//是否是浮点数
	{
		return stof(s);//string转float
	}
	else 
	{
		return stoi(s);//string转int
	}
}
 
Json parser_str()//解析字符串
{
	ss.get();
	string s;
	while (ss.peek() != '"')
		s.push_back(ss.get());
	ss.get();//去掉引号
	return Json(s);
}
 
Json parser_bool()//解析布尔
{
	if (ss.peek() == 'f') 
	{
		ss.get();ss.get();ss.get(); ss.get();ss.get();//吃掉false
		return Json(false);
	}
	else 
	{
		ss.get(); ss.get(); ss.get(); ss.get();//吃掉true
		return Json(true);
	}
}
 
Json parser_arr()//解析数组
{
	ss.get();//吃掉 [
	Json vec; 
	vec.type = LIST;
	while (ss.peek() != ']') 
	{
		vec.add(parser_val());//不确定每个数组元素的类型
		while (ss.peek() != ']' && (ss.peek() == ' ' || ss.peek() == '\t' || ss.peek() == '\n' || ss.peek() == ','))
			ss.get();//切换到下一个元素
	}
	ss.get();
	return vec;
}
 
Json parser_map() 
{
	ss.get();//吃掉 {
	Json dict; 
	dict.type = DICT;
	while (ss.peek() != '}') 
	{
		Json key = parser_val();
		while (ss.peek() == ' ' || ss.peek() == ':')//键与值的分割
			ss.get();
		Json val = parser_val();
		dict.put(key, val);
		while (ss.peek() != '}' && (ss.peek() == ' ' || ss.peek() == '\t' || ss.peek() == '\n' || ss.peek() == ','))
			ss.get();//切换到下一个对象
	}
	ss.get();
	return dict;
}
 
Json parser(string s) 
{
	ss.clear();//释放内存
	ss << s;
	return parser_val();
 
}
 
int main()
{
	ifstream fin("test.txt");
	stringstream ss; 
	ss << fin.rdbuf();
	Json x = parser(ss.str());
 
	cout << "test 1.1------------------------------" << endl;
	cout << x << endl;
 
	cout << "\n\ntest 1.2------------------------------" << endl;
	cout << x["person"] << endl;
 
	cout << "\n\ntest 1.3------------------------------" << endl;
	cout << x["person"]["name"] << endl;
	cout << x["person"]["age"] << endl;
	cout << x["person"]["hobby"][0] << endl;
	cout << x["person"]["hobby"][1] << endl;
	cout << x["person"]["hobby"][2] << endl;
	cout << x["person"]["hobby"][3] << endl;
	
 
	//修改内容
	Json y = parser(x.str());
	y["person"]["name"] = "july";
	y["person"]["age"] = 28;
	y["person"]["hobby"][0] = "6.78";
	y["person"]["hobby"][1] = "play GAMES";
	y["person"]["hobby"][2] = "sing and dance";
	x = parser(y.str());
	
	cout << "\n\ntest 2.1------------------------------" << endl;
	cout << x << endl;
 
	cout << "\n\ntest 2.2------------------------------" << endl;
	cout << x["person"]["name"] << endl;
	cout << x["person"]["age"] << endl;
	cout << x["person"]["hobby"][0] << endl;
	cout << x["person"]["hobby"][1] << endl;
	cout << x["person"]["hobby"][2] << endl;
	cout << x["person"]["hobby"][3] << endl;
 
	return 0;
}