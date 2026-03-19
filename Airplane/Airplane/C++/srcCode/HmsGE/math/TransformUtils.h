/****************************************************************************
Copyright (c) 2009      Valentin Milea
Copyright (c) 2010-2012 HmsAviPf-x.org
Copyright (c) 2013-2014 Chukong Technologies Inc.

http://www.HmsAviPf-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#ifndef __SUPPORT_TRANSFORM_UTILS_H__
#define __SUPPORT_TRANSFORM_UTILS_H__

// TODO: when in MAC or windows, it includes <OpenGL/gl.h>
#include "HmsGL.h"
#include "HmsAviMacros.h"

/**
 * @addtogroup base
 * @{
 */

namespace   HmsAviPf {

struct AffineTransform;
/**@{
 Conversion between mat4*4 and AffineTransform.
 @param m The Mat4*4 pointer.
 @param t Affine transform.
 */
HMS_DLL void CGAffineToGL(const AffineTransform &t, GLfloat *m);
HMS_DLL void GLToCGAffine(const GLfloat *m, AffineTransform *t);
/**@}*/
}//namespace   HmsAviPf 
/**
 end of base group
 @}
 */
#endif // __SUPPORT_TRANSFORM_UTILS_H__
