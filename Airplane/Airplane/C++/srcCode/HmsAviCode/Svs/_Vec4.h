#ifndef __VEC4__
#define __VEC4__

#include <stdlib.h>
#include <math.h>
#ifndef WIN32
#include "math.h"
#endif 

typedef struct _CVec4
{
	float _v[4];
}CVec4f, CVec4, *pCVec4, *pCVec4f; 

typedef struct  _CVec4d
{
	double _v[4];
}CVec4d, *pCVec4d;


 CVec4 Vec_CreatVec4f(double x, double y,double z,double w);

 CVec4d Vec_CreatVec4d(double x, double y,double z,double w);

 CVec4d Vec_Vec4dAddVec4d(const CVec4d *v0, const CVec4d *v1);
 CVec4d Vec_Vec4dSubVec4d(const CVec4d *v0, const CVec4d *v1);

 void Vec_Vec4dMulDouble(CVec4d *v0, double v1);
  pCVec4d Vec_CreatpCVec4d(CVec4d v);
/*
//求长度
*/
 double Vec_LengthVec4f(const CVec4 *vec2f);

/*
//求长度
*/
 double Vec_LengthVec4d(const CVec4d *vec2f);


 void Vec_PrintVec4d(const CVec4d *vec);

 void Vec_PrintVec4f(const CVec4f *vec);

 int Vec_EqualVec4d(const CVec4d *vec, const CVec4d *vec1);

#endif