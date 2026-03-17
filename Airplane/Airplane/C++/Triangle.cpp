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
void main() {
    gl_Position = vec4(aPos, 1.0);
}
)";

// 片段着色器 GLSL
const char* fragmentShaderSource = R"(
precision mediump float;
void main() {
    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0); // 红色
}
)";

// 三角形顶点
GLfloat vertices[] = {
     0.0f,  0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f
};

Triangle::Triangle() : VBO(0), shaderProgram(0) {}

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
    glUseProgram(shaderProgram);

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
