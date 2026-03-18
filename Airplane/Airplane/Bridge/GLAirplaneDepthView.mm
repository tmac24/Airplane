//
//  GLAirplaneDepthView.m
//  Airplane
//
//  Created by sunt on 2026/3/18.
//

#define GLES_SILENCE_DEPRECATION

#import "GLAirplaneDepthView.h"
#import <QuartzCore/CAEAGLLayer.h>
#include "GLES3/gl3.h"
#include "RenderObjects.h"

@interface GLAirplaneDepthView () {

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

    GLuint depthStencilBuffer;
    // 深度缓冲+模板缓冲

    RenderObjects renderObjects; // C++ 绘制对象
}
@end

@implementation GLAirplaneDepthView

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
        
        renderObjects.init();
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
    
    CGFloat scale = [UIScreen mainScreen].scale;
    CGSize size = self.bounds.size;

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
    
    
    // Depth buffer
    glGenRenderbuffers(1,&depthStencilBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER,depthStencilBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER,
                          GL_DEPTH_COMPONENT16,
                          size.width*scale,
                          size.height*scale);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,depthStencilBuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_STENCIL_ATTACHMENT,GL_RENDERBUFFER,depthStencilBuffer);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE)
        NSLog(@"Framebuffer incomplete!");
}

#pragma mark - 编译 Shader



#pragma mark - 渲染

- (void)render {
    
    // ❗关键：重新绑定
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);

    // 设置视口（决定绘制区域大小）
    glViewport(0, 0,
               self.frame.size.width,
               self.frame.size.height);

    // 清屏（设置背景色）
    glClearColor(0.8f, 0.8f, 0.8f, 1.0f); // 浅灰背景
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);

    renderObjects.render(); // 调用 C++ 绘制

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


- (void)dealloc {
    renderObjects.cleanup();
}

@end
