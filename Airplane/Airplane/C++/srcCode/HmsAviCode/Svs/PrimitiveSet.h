#ifndef _PRIMITIVESET__H_
#define _PRIMITIVESET__H_

#include "GLHeader.h"
#include "GLES3/gl3.h"


typedef struct _CPrimitiveSet
{
    GLenum type;
    unsigned int beg;
    unsigned int count;
    unsigned int lineWidth;
    unsigned int bVisable;//1: visible  , 0: not visible, 
}CPrimitiveSet, *pCPrimitiveSet;


CPrimitiveSet CPrimitiveSet_Create(GLenum t, unsigned int b, unsigned int c, unsigned int bVisible);
CPrimitiveSet CPrimitiveSet_CreatDefaultVisible(GLenum t, unsigned int b, unsigned int c);



#endif
