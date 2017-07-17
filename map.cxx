//map复习
#include <string>
#include <map>
#include <iostream>
using namespace std;

int main()
{
    map<int, string> mapStudent;
    //插入方法一
    mapStudent.insert(pair<int,string>(1, "student_1"));
    mapStudent.insert(pair<int,string>(2, "student_2"));
    mapStudent.insert(pair<int,string>(3, "student_3"));

    //插入方法二
    mapStudent.insert(map<int,string>::value_type(4, "student_4"));
    mapStudent.insert(map<int,string>::value_type(5, "student_5")); 
    mapStudent.insert(map<int,string>::value_type(6, "student_6")); 

    //插入方法三
    mapStudent[7] = "student_7";
    mapStudent[8] = "student_8";
    mapStudent[9] = "student_9";

    //插入方法四,可以判断是否插入成功
    pair<map<int, string>::iterator, bool> Insert_Par;
    Insert_Par = mapStudent.insert(pair<int, string>(10,"student_10"));
    if(Insert_Par.second == true)
        cout << "Insert Successfully" <<endl;
    else
        cout << "Insert failed" <<endl;

    //前向迭代器
    map<int, string>::iterator iter;
    for(iter = mapStudent.begin(); iter != mapStudent.end();iter++)
        cout << iter->first << ' '<< iter->second<<endl;

    //逆向迭代器
    map<int, string>::reverse_iterator iter1;
    for(iter1 = mapStudent.rbegin(); iter1 != mapStudent.rend();iter1++)
        cout << iter1->first << ' '<< iter1->second<<endl;

    //用数组的形式
    int nSize = mapStudent.size();
    for(int i = 1; i <= nSize; i++)
        cout << mapStudent[i]<<endl;

    //查找
    iter = mapStudent.find(7);
    if(iter != mapStudent.end())
        cout << "Find, the value is" << iter->second <<endl;

    //删除
    //通过key删除
    int res = mapStudent.erase(1);

    // 通过iterator删除
    iter = mapStudent.find(2);
    mapStudent.erase(iter);

    //再次遍历
    for(iter = mapStudent.begin(); iter != mapStudent.end();iter++)
        cout << iter->first << ' '<< iter->second<<endl;

    return 0;
}
