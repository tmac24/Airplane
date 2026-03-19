//
//  platform_ios.m
//  Hms_ios
//
//  Created by sunt on 2026/2/5.
//

#import <Foundation/Foundation.h>
#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES3/gl.h>

#include "platform.h"

extern "C" {

// ================= 回调注册 =================

void ESUTIL_API esRegisterDrawFunc(
    ESContext *esContext,
    void (*drawFunc)(ESContext *, float))
{
    if (esContext)
        esContext->drawFunc = drawFunc;
}

void ESUTIL_API esRegisterUpdateFunc(
    ESContext *esContext,
    void (*updateFunc)(ESContext *, float))
{
    if (esContext)
        esContext->updateFunc = updateFunc;
}

void ESUTIL_API esRegisterShutdownFunc(
    ESContext *esContext,
    void (*shutdownFunc)(ESContext *))
{
    if (esContext)
        esContext->shutdownFunc = shutdownFunc;
}

// ================= Buffer Swap =================

void ESUTIL_API esSwapBuffers(ESContext *esContext)
{
    // iOS 上 swap 由 View / EngineView 控制
    // 这里故意留空
}

// ================= Loop 控制 =================

GLboolean esUserInterrupt(ESContext *esContext)
{
    // iOS 生命周期由 UIKit 管
    return GL_FALSE;
}

}
