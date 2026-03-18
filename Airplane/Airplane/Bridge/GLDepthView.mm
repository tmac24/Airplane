//
//  GLDepthView.m
//  Airplane
//
//  Created by sunt on 2026/3/18.
//

#define GLES_SILENCE_DEPRECATION

#import "GLDepthView.h"
#import <QuartzCore/CAEAGLLayer.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#import <math.h>

@interface GLDepthView ()
{
    EAGLContext *context;
    CAEAGLLayer *eaglLayer;

    GLuint frameBuffer;
    GLuint colorBuffer;
    GLuint depthBuffer;

    GLuint program;
    GLuint VBO;
    GLuint EBO;

    float angle;
}
@end

@implementation GLDepthView

+ (Class)layerClass {
    return [CAEAGLLayer class];
}

- (instancetype)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        [self setupLayer];
        [self setupContext];
        [self setupBuffers];
        [self setupProgram];
        [self setupVertex];
    }
    return self;
}

#pragma mark - Layer

- (void)setupLayer {
    eaglLayer = (CAEAGLLayer *)self.layer;
    eaglLayer.opaque = YES;
}

#pragma mark - Context

- (void)setupContext {
    context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    [EAGLContext setCurrentContext:context];
}

#pragma mark - Buffers

- (void)setupBuffers {
    // Color buffer
    glGenRenderbuffers(1, &colorBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, colorBuffer);
    [context renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer];

    // Frame buffer
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorBuffer);

    // Depth buffer
    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);

    CGFloat scale = [UIScreen mainScreen].scale;
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16,
                          self.frame.size.width * scale,
                          self.frame.size.height * scale);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        NSLog(@"❌ FrameBuffer 不完整");
    }
}

#pragma mark - Shader

- (GLuint)compileShader:(const char *)source type:(GLenum)type {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    return shader;
}

- (void)setupProgram {
    const char *vs =
    "attribute vec3 position;\n"
    "uniform mat4 mvp;\n"
    "void main(){\n"
    "  gl_Position = mvp * vec4(position,1.0);\n"
    "}";

    const char *fs =
    "precision mediump float;\n"
    "void main(){\n"
    "  gl_FragColor = vec4(1.0,0.5,0.2,1.0);\n"
    "}";

    GLuint vertexShader = [self compileShader:vs type:GL_VERTEX_SHADER];
    GLuint fragmentShader = [self compileShader:fs type:GL_FRAGMENT_SHADER];

    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glUseProgram(program);
}

#pragma mark - Vertex

- (void)setupVertex {
    GLfloat vertices[] = {
        -0.5,-0.5,-0.5,  0.5,-0.5,-0.5,  0.5,0.5,-0.5,  -0.5,0.5,-0.5,
        -0.5,-0.5,0.5,   0.5,-0.5,0.5,   0.5,0.5,0.5,   -0.5,0.5,0.5
    };

    GLushort indices[] = {
        0,1,2, 2,3,0,
        4,5,6, 6,7,4,
        0,1,5, 5,4,0,
        2,3,7, 7,6,2,
        1,2,6, 6,5,1,
        0,3,7, 7,4,0
    };

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    GLuint pos = glGetAttribLocation(program, "position");
    glEnableVertexAttribArray(pos);
    glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

#pragma mark - Matrix

void multiplyMatrix(const float *a, const float *b, float *result) {
    for (int i=0;i<4;i++){
        for (int j=0;j<4;j++){
            result[i*4+j]=0;
            for (int k=0;k<4;k++){
                result[i*4+j] += a[i*4+k]*b[k*4+j];
            }
        }
    }
}

void makeRotationY(float angle, float *m) {
    float c = cosf(angle);
    float s = sinf(angle);
    m[0]=c; m[1]=0; m[2]=s; m[3]=0;
    m[4]=0; m[5]=1; m[6]=0; m[7]=0;
    m[8]=-s; m[9]=0; m[10]=c; m[11]=0;
    m[12]=0; m[13]=0; m[14]=0; m[15]=1;
}

void makePerspective(float fovy, float aspect, float near, float far, float *m) {
    float f = 1.0f / tanf(fovy * 0.5f);
    m[0] = f/aspect; m[1]=0; m[2]=0; m[3]=0;
    m[4]=0; m[5]=f; m[6]=0; m[7]=0;
    m[8]=0; m[9]=0; m[10]=(far+near)/(near-far); m[11]=-1;
    m[12]=0; m[13]=0; m[14]=2*far*near/(near-far); m[15]=0;
}

#pragma mark - Render

- (void)render {
    
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, colorBuffer);
    

    angle += 0.02f;
    CGFloat scale = [UIScreen mainScreen].scale;
    glViewport(0,0,self.frame.size.width*scale,self.frame.size.height*scale);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2,0.3,0.3,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 透视矩阵
    float proj[16];
    makePerspective(M_PI/4.0f, self.frame.size.width/self.frame.size.height, 0.1f, 100.0f, proj);

    // 旋转矩阵
    float rot[16];
    makeRotationY(angle, rot);

    // MVP = Projection * Rotation
    float mvp[16];
    multiplyMatrix(proj, rot, mvp);

    GLuint mvpLoc = glGetUniformLocation(program,"mvp");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, mvp);

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);

    [context presentRenderbuffer:GL_RENDERBUFFER];
}

- (void)layoutSubviews {
    [super layoutSubviews];
    [EAGLContext setCurrentContext:context];

    if(frameBuffer) glDeleteFramebuffers(1,&frameBuffer);
    if(colorBuffer) glDeleteRenderbuffers(1,&colorBuffer);
    if(depthBuffer) glDeleteRenderbuffers(1,&depthBuffer);

    // Color buffer
    glGenRenderbuffers(1,&colorBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER,colorBuffer);
    [context renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer *)self.layer];

    // Depth buffer
    glGenRenderbuffers(1,&depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER,depthBuffer);
    CGFloat scale = [UIScreen mainScreen].scale;
    glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT16,
                          self.bounds.size.width*scale,
                          self.bounds.size.height*scale);

    // Frame buffer
    glGenFramebuffers(1,&frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER,frameBuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorBuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        NSLog(@"❌ FrameBuffer 不完整");
    }
}

#pragma mark - Start

- (void)startRender {
    CADisplayLink *link = [CADisplayLink displayLinkWithTarget:self selector:@selector(render)];
    [link addToRunLoop:[NSRunLoop mainRunLoop] forMode:NSDefaultRunLoopMode];
}

@end
