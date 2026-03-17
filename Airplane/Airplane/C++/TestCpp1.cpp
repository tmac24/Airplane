//
//  TestCpp1.cpp
//  Airplane
//
//  Created by sunt on 2026/3/4.
//

#include "TestCpp1.h"
#include <iostream>

int TestCpp1::add1(int a, int b) {
    return a + b;
}

void Person::sayHello() {
    printf("hello\n");
    printf("我的名字是 %s，年龄是 %d\n", name.c_str(), age);
    std::cout << "我的名字是 " << name << "，年龄是 " << age << std::endl;
}

void Person::setAge(int age) {
    this->age = age;
}

void Student::study() {
    printf("我是 %s,我正在学习\n",name.c_str());
}


void Animal::speak() {
    std::cout << "动物在叫" << std::endl;
}

void Dog::speak() {
    printf("狗在叫\n");
}

void Cat::speak() {
    printf("猫在叫\n");
}



Superman::Superman(std::string name) {
    this->name = name;
    std::cout << "构造函数执行，name = " << this->name << std::endl;
}

Superman::~Superman() {
    std::cout << "析构函数执行，name = " << this->name<< std::endl;
}

void Superman::speakName() {
    printf("说出我的名字：%s\n",name.c_str());
}


void MyCompany::Person::sayHello() {
    std::cout << "Hello, I'm " << name << ", age " << age << std::endl;
}

void MyCompany::greet() {
    std::cout << "Welcome to MyCompany!" << std::endl;
}
