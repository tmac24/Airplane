//
//  RenderObjects.hpp
//  Airplane
//
//  Created by sunt on 2026/3/18.
//

#ifndef RenderObjects_hpp
#define RenderObjects_hpp

#include <stdio.h>

#pragma once
#include "GLES3/gl3.h"

class RenderObjects {
public:
    RenderObjects();
    ~RenderObjects();

    void init();     // 初始化 VBO/Shader
    void render();   // 绘制（旋转三角形+底部矩形）
    void cleanup();  // 释放资源

    GLuint shaderProgram; // shader 程序

private:
    // 顶点缓冲
    GLuint triVBO;
    GLuint rectVBO;

    float angle;  // 三角形旋转角度

    GLuint compileShader(const char* source, GLenum type);
    GLuint linkProgram(GLuint vertexShader, GLuint fragmentShader);
};

#endif /* RenderObjects_hpp */


/**
 | 特性     | Objective-C                 | C++                            |
 | ------ | --------------------------- | ------------------------------ |
 | 声明对象方法 | `- (void)render`            | `void RenderObjects::render()` |
 | 类方法    | `+ (void)foo`               | `static void foo()`            |
 | 构造函数   | `- init`                    | `RenderObjects()`              |
 | 析构函数   | `- dealloc`                 | `~RenderObjects()`             |
 | 访问成员   | `self->member` 或直接 `member` | `this->member` 或直接 `member`    |

 
 */
