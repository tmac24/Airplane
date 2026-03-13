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
     Person p

     创建对象
     使用 .
     自动管理内存

     Person *p
     创建指针
     使用 ->

     指向对象地址
     */
    
}

@end
