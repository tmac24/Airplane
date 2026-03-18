//
//  RenderObjects.cpp
//  Airplane
//
//  Created by sunt on 2026/3/18.
//

#include "RenderObjects.h"
#include <iostream>
#include <cmath>

// 顶点着色器
const char* vertex1ShaderSource = R"(
attribute vec3 aPos;
uniform float angle;
uniform vec4 ourColor;
varying vec4 vColor;

void main() {
    float s = sin(angle);
    float c = cos(angle);

    mat4 rotation = mat4(
        c,  s, 0.0, 0.0,
       -s,  c, 0.0, 0.0,
        0.0,0.0,1.0, 0.0,
        0.0,0.0,0.0, 1.0
    );

    gl_Position = rotation * vec4(aPos,1.0);
    vColor = ourColor;
}
)";

// 片段着色器
const char* fragment1ShaderSource = R"(
precision mediump float;
varying vec4 vColor;
void main() {
    gl_FragColor = vColor;
}
)";

RenderObjects::RenderObjects() : triVBO(0), rectVBO(0), shaderProgram(0), angle(0.0f) {}
RenderObjects::~RenderObjects() { cleanup(); }

GLuint RenderObjects::compileShader(const char* source, GLenum type) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success){
        char info[512];
        glGetShaderInfoLog(shader,512,nullptr,info);
        std::cerr << "Shader compile error: " << info << std::endl;
    }
    return shader;
}

GLuint RenderObjects::linkProgram(GLuint vertexShader, GLuint fragmentShader){
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success){
        char info[512];
        glGetProgramInfoLog(program,512,nullptr,info);
        std::cerr << "Program link error: " << info << std::endl;
    }
    return program;
}

void RenderObjects::init() {
    // ---------- 顶点缓冲 ----------

    GLfloat triVertices[] = {
        0.0f,  0.5f, 0.0f,
       -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f
    };

    GLfloat rectVertices[] = {
       -0.5f, -0.8f, 0.0f,
        0.5f, -0.8f, 0.0f,
       -0.5f, -0.5f, 0.0f,
        0.5f, -0.8f, 0.0f,
        0.5f, -0.5f, 0.0f,
       -0.5f, -0.5f, 0.0f
    };

    glGenBuffers(1,&triVBO);
    glBindBuffer(GL_ARRAY_BUFFER,triVBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(triVertices),triVertices,GL_STATIC_DRAW);

    glGenBuffers(1,&rectVBO);
    glBindBuffer(GL_ARRAY_BUFFER,rectVBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(rectVertices),rectVertices,GL_STATIC_DRAW);

    // ---------- Shader Program ----------
    GLuint vs = compileShader(vertex1ShaderSource, GL_VERTEX_SHADER);
    GLuint fs = compileShader(fragment1ShaderSource, GL_FRAGMENT_SHADER);

    shaderProgram = linkProgram(vs, fs);

    glDeleteShader(vs);
    glDeleteShader(fs);
}

void RenderObjects::render() {
    glUseProgram(shaderProgram);

    angle += 0.02f; // 每帧旋转

    GLint angleLoc = glGetUniformLocation(shaderProgram,"angle");
    glUniform1f(angleLoc, angle);

    GLint colorLoc = glGetUniformLocation(shaderProgram,"ourColor");

    GLint posAttrib = glGetAttribLocation(shaderProgram,"aPos");
    glEnableVertexAttribArray(posAttrib);

    // ===== 1️⃣ 绘制矩形（模板区域，不旋转） =====
    glUniform1f(angleLoc,0.0f); // 矩形不旋转
    glUniform4f(colorLoc,0.5f,0.5f,0.5f,1.0f);

    glBindBuffer(GL_ARRAY_BUFFER,rectVBO);
    glVertexAttribPointer(posAttrib,3,GL_FLOAT,GL_FALSE,3*sizeof(GLfloat),0);
    glDrawArrays(GL_TRIANGLES,0,6);

    // ===== 2️⃣ 绘制旋转三角形 =====
    glUniform1f(angleLoc,angle);
    glUniform4f(colorLoc,1.0f,0.0f,0.0f,1.0f);

    glBindBuffer(GL_ARRAY_BUFFER,triVBO);
    glVertexAttribPointer(posAttrib,3,GL_FLOAT,GL_FALSE,3*sizeof(GLfloat),0);
    glDrawArrays(GL_TRIANGLES,0,3);

    glDisableVertexAttribArray(posAttrib);
}

void RenderObjects::cleanup() {
    if(triVBO) glDeleteBuffers(1,&triVBO);
    if(rectVBO) glDeleteBuffers(1,&rectVBO);
    if(shaderProgram) glDeleteProgram(shaderProgram);
}
