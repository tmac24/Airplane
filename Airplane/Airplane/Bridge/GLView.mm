//
//  GLView.m
//  Airplane
//
//  Created by sunt on 2026/3/18.
//

#define GLES_SILENCE_DEPRECATION

#import "GLView.h"
#import <QuartzCore/CAEAGLLayer.h>
#include "GLES3/gl3.h"

@interface GLView () {

    // ========================
    // 🔹 OpenGL ES 核心对象
    // ========================

    EAGLContext *context;
    // OpenGL ES 上下文（类似 Android 的 EGLContext）
    // 所有 GL 操作都必须在这个上下文中执行

    CAEAGLLayer *eaglLayer;
    // 渲染层（类似 Surface）
    // 用于把 OpenGL 渲染结果显示到屏幕

    GLuint renderBuffer;
    // 渲染缓冲区（存储最终像素数据）

    GLuint frameBuffer;
    // 帧缓冲区（组织渲染流程，把 renderBuffer 绑定进去）

    GLuint program;
    // Shader 程序（顶点着色器 + 片段着色器）

    GLuint VBO;
    // 顶点缓冲对象（存储三角形顶点数据）
}
@end

@implementation GLView

#pragma mark - Layer 类型

+ (Class)layerClass {
    // 指定当前 View 使用 CAEAGLLayer（否则无法使用 OpenGL）
    return [CAEAGLLayer class];
}

#pragma mark - 初始化

- (instancetype)initWithFrame:(CGRect)frame {
    if (self = [super initWithFrame:frame]) {

        // 初始化流程（顺序非常重要）
        [self setupLayer];     // 1. 创建显示层
        [self setupContext];   // 2. 创建 OpenGL 上下文
        [self setupBuffers];   // 3. 创建缓冲区
        [self setupProgram];   // 4. 创建 shader 程序
        [self setupVertex];    // 5. 上传顶点数据
    }
    return self;
}

#pragma mark - 初始化 Layer

- (void)setupLayer {
    eaglLayer = (CAEAGLLayer *)self.layer;

    // 设置为不透明（提高性能）
    eaglLayer.opaque = YES;
}

#pragma mark - 初始化 Context

- (void)setupContext {
    // 创建 OpenGL ES 2.0 上下文
    context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];

    // 设置为当前上下文（非常重要）
    [EAGLContext setCurrentContext:context];
}

#pragma mark - 初始化缓冲区

- (void)setupBuffers {

    // ---------- RenderBuffer ----------
    glGenRenderbuffers(1, &renderBuffer);
    // 生成一个 RenderBuffer

    glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
    // 绑定 RenderBuffer

    [context renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer];
    // 把 Layer 绑定到 RenderBuffer（让它能显示到屏幕）


    // ---------- FrameBuffer ----------
    glGenFramebuffers(1, &frameBuffer);
    // 生成 FrameBuffer

    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    // 绑定 FrameBuffer

    glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                              GL_COLOR_ATTACHMENT0,
                              GL_RENDERBUFFER,
                              renderBuffer);
    // 把 RenderBuffer 挂载到 FrameBuffer 上
}

#pragma mark - 编译 Shader

- (GLuint)compileShader:(const char *)source type:(GLenum)type {
    GLuint shader = glCreateShader(type);
    // 创建 shader 对象

    glShaderSource(shader, 1, &source, NULL);
    // 传入 shader 代码

    glCompileShader(shader);
    // 编译 shader

    return shader;
}

#pragma mark - 创建 Shader 程序

- (void)setupProgram {

    // ---------- 顶点着色器 ----------
    const char *vs =
    "attribute vec3 position;\n"
    "void main() {\n"
    "    gl_Position = vec4(position, 1.0);\n"
    "}";

    // ---------- 片段着色器 ----------
    const char *fs =
    "precision mediump float;\n"
    "void main() {\n"
    "    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
    "}";

    GLuint vertexShader = [self compileShader:vs type:GL_VERTEX_SHADER];
    GLuint fragmentShader = [self compileShader:fs type:GL_FRAGMENT_SHADER];

    program = glCreateProgram();
    // 创建 shader 程序

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    // 把两个 shader 挂载到程序上

    glLinkProgram(program);
    // 链接程序

    glUseProgram(program);
    // 使用该程序（后续绘制都会用它）
}

#pragma mark - 顶点数据

- (void)setupVertex {

    // 三角形顶点（x, y, z）
    GLfloat vertices[] = {
         0.0f,  0.5f, 0.0f,   // 顶点1
        -0.5f, -0.5f, 0.0f,   // 顶点2
         0.5f, -0.5f, 0.0f    // 顶点3
    };

    glGenBuffers(1, &VBO);
    // 创建 VBO

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // 绑定 VBO

    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(vertices),
                 vertices,
                 GL_STATIC_DRAW);
    // 把顶点数据上传到 GPU

    GLuint pos = glGetAttribLocation(program, "position");
    // 获取 shader 中 position 变量的位置

    glEnableVertexAttribArray(pos);
    // 启用顶点属性

    glVertexAttribPointer(pos,
                          3,              // 每个顶点 3 个值
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          0);
    // 告诉 GPU 如何读取顶点数据
}

#pragma mark - 渲染

- (void)render {

    // 设置视口（决定绘制区域大小）
    glViewport(0, 0,
               self.frame.size.width,
               self.frame.size.height);

    // 清屏（设置背景色）
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // 绘制三角形
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // 把 RenderBuffer 显示到屏幕（非常关键）
    [context presentRenderbuffer:GL_RENDERBUFFER];
}

#pragma mark - 启动渲染循环

- (void)startRender {

    CADisplayLink *link =
    [CADisplayLink displayLinkWithTarget:self
                                selector:@selector(render)];

    // 每一帧调用 render（类似游戏循环）
    [link addToRunLoop:[NSRunLoop mainRunLoop]
              forMode:NSDefaultRunLoopMode];
}

@end
