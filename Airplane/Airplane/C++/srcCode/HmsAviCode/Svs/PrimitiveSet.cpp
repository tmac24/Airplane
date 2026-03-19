#include "PrimitiveSet.h"


CPrimitiveSet CPrimitiveSet_Create(GLenum t, unsigned int b, unsigned int c, unsigned int bVisible)
{
    CPrimitiveSet nnd;
    nnd.type = t;
    nnd.beg = b;
    nnd.count = c;
    nnd.bVisable = bVisible;
    return nnd;
}

CPrimitiveSet CPrimitiveSet_CreatDefaultVisible(GLenum t, unsigned int b, unsigned int c)
{
    CPrimitiveSet nnd;
    nnd.type = t;
    nnd.beg = b;
    nnd.count = c;
    nnd.bVisable = 1;
    return nnd;
}

