//
//  Triangle.hpp
//  Airplane
//
//  Created by sunt on 2026/3/17.
//

// $(SRCROOT)/Airplane/C++/inc
// $(SRCROOT)/Hms_ios/ResourceC++

#ifndef Triangle_hpp
#define Triangle_hpp

#include <stdio.h>

#include <GLES2/gl2.h>
#include <string>

class Triangle {
public:
    Triangle();
    ~Triangle();

    void init();       // 初始化顶点和着色器
    void render();     // 绘制三角形
    void cleanup();    // 释放资源

private:
    GLuint VBO;        // 顶点缓冲对象
    GLuint shaderProgram;
    float angle;

    GLuint compileShader(const std::string& source, GLenum type);
    GLuint linkProgram(GLuint vertexShader, GLuint fragmentShader);
};

#endif /* Triangle_hpp */
