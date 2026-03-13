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
