//
//  Triangle.cpp
//  Airplane
//
//  Created by sunt on 2026/3/17.
//

#include "Triangle.h"
#include <iostream>

// 顶点着色器 GLSL
const char* vertexShaderSource = R"(
attribute vec3 aPos;

uniform float angle;

void main() {
    float s = sin(angle);
    float c = cos(angle);

    mat4 rotation = mat4(
        c,  s,  0.0, 0.0,
       -s,  c,  0.0, 0.0,
        0.0,0.0,1.0, 0.0,
        0.0,0.0,0.0, 1.0
    );

    gl_Position = rotation * vec4(aPos, 1.0);
}
)";

// 片段着色器 GLSL
const char* fragmentShaderSource = R"(
precision mediump float;
void main() {
    gl_FragColor = vec4(0.5, 0.5, 0.5, 1.0); // 灰色
}
)";

// 三角形顶点
GLfloat vertices[] = {
     0.0f,  0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f
};

Triangle::Triangle() : VBO(0), shaderProgram(0), angle(0.0f) {}

Triangle::~Triangle() {
    cleanup();
}

void Triangle::init() {
    // 创建 VBO
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 编译顶点/片段着色器
    GLuint vertexShader = compileShader(vertexShaderSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

    shaderProgram = linkProgram(vertexShader, fragmentShader);

    // 删除临时 shader
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Triangle::render() {
    
    angle += 0.02f;   // 每帧增加一点

    glUseProgram(shaderProgram);
    
    GLint angleLoc = glGetUniformLocation(shaderProgram, "angle");
    glUniform1f(angleLoc, angle);

    // 绑定 VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    GLint posAttrib = glGetAttribLocation(shaderProgram, "aPos");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);

    // 绘制三角形
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(posAttrib);
}

void Triangle::cleanup() {
    if (VBO) glDeleteBuffers(1, &VBO);
    if (shaderProgram) glDeleteProgram(shaderProgram);
}

GLuint Triangle::compileShader(const std::string& source, GLenum type) {
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    return shader;
}

GLuint Triangle::linkProgram(GLuint vertexShader, GLuint fragmentShader) {
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    return program;
}


/**
 
 #include "Triangle.h"
 #include <GLES2/gl2.h>
 #include <EGL/egl.h>  // 平台初始化示例，实际根据 iOS/Android 设置

 int main() {
     // 初始化 OpenGL ES 上下文 (EGL / iOS 或 Android 视图)
     // 这里略，通常由平台提供

     Triangle triangle;
     triangle.init();

     // 渲染循环
     while (true) {
         glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
         glClear(GL_COLOR_BUFFER_BIT);

         triangle.render();

         // 刷新显示缓冲区 (SwapBuffers)
         // 平台相关函数，例如 eglSwapBuffers(display, surface);
     }

     triangle.cleanup();
     return 0;
 }
 
 */
