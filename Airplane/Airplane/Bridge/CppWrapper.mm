//
//  CppWrapper.m
//  Airplane
//
//  Created by sunt on 2026/3/4.
//

#import "CppWrapper.h"
#include "TestCpp.hpp"
//#import "TestCpp1.h"
#include "TestCpp1.h"

@interface CppWrapper()
{
    TestCpp _test;
    
    TestCpp1 _test1;
}
@end

@implementation CppWrapper


- (int)addWithA:(int)a b:(int)b {
    return _test.add(a, b);
}

- (int)addWithA1:(int)a b:(int)b {
    return _test1.add1(a, b);
}

- (void)testC {
    Person p;
    p.name = "hms";
    p.age = 18;
    p.sayHello();
    
    Person *f = new Person();
    f->name = "Foug";
    f->age = 20;
    f->sayHello();
    delete f;
    /**
     1.对象与指针
     Person p
     创建对象
     使用 .
     自动管理内存

     Person *p
     创建指针
     使用 ->
     指向对象地址
     */
    
    
    Person P1;
    P1.name = "james";
    P1.age = 10;
    
    MyPoint p2;
    p2.x = 3;
    p2.y = 5;
    /**
     2.类与结构体
     class 面向对象 默认访问权限：private
     
     struct 数据结构           public
     都可继承、定义构造函数、多态。
     */
    
    
    
    Animal *an1 = new Dog();
    an1->speak();
    
    Animal *an2 = new Cat();
    an2->speak();
    
    Student *student = new Student();
    student->name = "爱因斯坦";
    student->age = 98;
    student->sayHello();
    student->study();
    
    /**
     3.多态
     1️⃣ 有继承关系
     2️⃣ 父类函数必须是 virtual
     3️⃣ 子类要重写（override）该函数
     核心原理：对象内部有一个函数表指针，运行时决定调用哪个函数；用父类指针，调用子类实现
     
     继承
     继承 = 复用代码 + 建立父子关系;多态 = 同一接口，不同对象表现不同行为
     */
    
    
    
    Person *ming = new Person();
    ming->setAge(18);
    ming->name = "小明";
    ming->sayHello();
    /**
     4.this关键字
     this 就是“当前对象的地址（指针）“
     */
    

    
    
    Superman *superMan = new Superman("奥特曼");
    superMan->speakName();
    delete superMan;
        
    Superman superman2 = Superman("葫芦娃");
    superman2.speakName();
    /**
     4.生命周期全过程:
     .h写声明  .cpp写实现
        1.创建对象
            a.分配内存
            b.调用构造函数
            c.this= &p
        2.调用方法
        3.销毁对象，自动调用析构函数
            a.栈对象 自动创建，自动销毁
            b.堆对象 手动创建，必须delete
     */
    
    
    
    
    MyCompany::Person p11;
    p11.name = "kobe";
    p11.sayHello();
    MyCompany::greet();
    /**
     5.namespace
     命名空间 = 给代码（变量、函数、类等）加上一个“名字标签”，避免名字冲突
     */
    
    
    
    
    /**
     OpenGL ES = 移动端专用的轻量级 GPU 图形 API

     用于渲染 2D/3D 图形

     基于 OpenGL，但去掉桌面不适合的特性

     现代移动开发通常配合着色器和 GPU 编程使用
     
     */
}

@end
