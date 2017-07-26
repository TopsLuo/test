/*
    智能指针头文件:<memory>，它是c++11里的特性，使用智能指针需要注意几点：
    1.  不能将一个原始指针直接赋值给一个智能指针，原因：一个是类，一个是指针
    2.  不要用一个原始指针初始化多个智能指针，原因：会造成多次销毁。办法：用智能指针初始化另外一个智能指针
    3.  不要在函数中创建智能指针，因为C++的函数参数的计算顺序在不同的编译器下是不同的。正确的做法是：先创建好，然后再传入
    4.  禁止通过shared_from_this()返回this指针，原因：可能造成二次析构
    5.  避免循环引用。智能指针最大的一个陷阱是循环引用。循环引用会导致内在泄漏。解决方法是：AStruct或者BStruct改为weak_ptr
*/
#include <iostream>
#include <memory>
#include <unistd.h>
#include <list>
using namespace std;

class Person{
public:
    Person(int v)
    {
        value = v;
        cout << "Cons: " << value << endl;
    }
    ~Person()
    {
        cout << "Des: " << value << endl;
    }
    void Helloworld()
    {
        cout << "Hello world" << endl;
    }

    int value;
};

void FreeShared(shared_ptr<Person> & person)
{
    cout << "user count: " << person.use_count() << endl;
}
int main()
{
    list<shared_ptr<Person>> person_list;

    Person *p1 = new Person(1);

    shared_ptr<Person> p2(p1);
    cout << "p2 use count:" << p2.use_count() << endl;
    shared_ptr<Person> p3(p2);
    cout << "p3 use count:" << p3.use_count() << endl;
    shared_ptr<Person> p4(p3);
    cout << "p4 use count:" << p4.use_count() << endl;

    person_list.push_back(p2);
    cout << "p2 use count:" << p2.use_count() << endl;
    person_list.push_back(p3);
    cout << "p2 use count:" << p2.use_count() << endl;
    person_list.push_back(p4);
    cout << "p2 use count:" << p2.use_count() << endl;

    //FreeShared(p4);

    for(list<shared_ptr<Person>>::iterator p = person_list.begin(); p != person_list.end(); p++)
    {
        //(*p)->Helloworld();
        p->reset();
        //cout << "p2 use count:" << p2.use_count() << endl;
    }

    p2.reset();
    cout << "p2 use count:" << p2.use_count() << endl;
    cout << "p3 use count:" << p3.use_count() << endl;
    cout << "p4 use count:" << p4.use_count() << endl;
    p3.reset();
    cout << "p2 use count:" << p2.use_count() << endl;
    cout << "p3 use count:" << p3.use_count() << endl;
    cout << "p4 use count:" << p4.use_count() << endl;
    p4.reset();
    cout << "p2 use count:" << p2.use_count() << endl;
    cout << "p3 use count:" << p3.use_count() << endl;
    cout << "p4 use count:" << p4.use_count() << endl;

    cout << "end" << endl;
    return 0;
}
//g++ shared_ptr.cxx -std=c++11
