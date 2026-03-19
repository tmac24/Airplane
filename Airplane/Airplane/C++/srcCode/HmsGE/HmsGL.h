#pragma once

#include <GLES3/gl3.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include "HmsPlatformConfig.h"

#define glClearDepth				glClearDepthf

#if HMS_TARGET_PLATFORM == HMS_PLATFORM_ANDROID
//#define glDeleteVertexArrays        glDeleteVertexArraysOES
//#define glGenVertexArrays           glGenVertexArraysOES
//#define glBindVertexArray           glBindVertexArrayOES
//#define glMapBuffer                 glMapBufferOES
//#define glUnmapBuffer               glUnmapBufferOES

//define GL_DEPTH24_STENCIL8         GL_DEPTH24_STENCIL8_OES
//#define GL_WRITE_ONLY               GL_WRITE_ONLY_OES
#endif
