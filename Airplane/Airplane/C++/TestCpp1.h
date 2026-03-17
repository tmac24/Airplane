//
//  TestCpp1.hpp
//  Airplane
//
//  Created by sunt on 2026/3/4.
//

#ifndef TestCpp1_hpp
#define TestCpp1_hpp

#include <stdio.h>
#include <string>

#include <iostream>
using namespace std; // 告诉编译器，“std 命名空间里的内容我都要用”

class TestCpp1 {
public:
    int add1(int a, int b);
};


/// 类
class Person { //父类
public:
    int age;
    std::string name;
    void sayHello();
    
    void setAge(int age);
};

class Student: public Person { //子类
public:
    void study();
};

/// 结构体
struct MyPoint {
    int x;
    int y;
};


/// 父类
class Animal {
public:
    virtual void speak();
};

/// 子类
class Dog: public Animal {
public:
    void speak() override;
};

class Cat: public Animal {
public:
    void speak() override;
};



class Superman {
public:
    std::string name;
    
    // 构造函数（声明）
    Superman(std::string name);
    
    // 析构函数（声明）
    ~Superman();
    
    void speakName();
};




namespace MyCompany {   // 定义命名空间
    class Person {
    public:
        std::string name;
        int age;

        void sayHello();
    };

    void greet();
}

#endif /* TestCpp1_hpp */
