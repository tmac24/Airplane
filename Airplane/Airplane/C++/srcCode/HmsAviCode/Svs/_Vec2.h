#ifndef __VEC2__
#define __VEC2__


#ifndef WIN32
#include "math.h"
#else
#include <math.h>
#endif  


typedef struct _CVec2f
{ 
	float _v[2];
}CVec2f, CVec2, *pCVec2,*pCVec2f;

typedef struct  _CVec2d
{
	double _v[2]; 
}CVec2d, *pCVec2d;

typedef struct  _CVec2i
{
	int _v[2]; 
}CVec2i, *pCVec2i;



CVec2 Vec_CreatVec2(float x, float y);

pCVec2i Vec_CreatVec2iPtr(int x, int y);

void Vec_DestoryVec2iPtr(pCVec2i *vec);

CVec2 Vec_CreatVec2f(float x, float y);

CVec2d Vec_CreatVec2d(double x, double y);


double Vec_LengthVec2(CVec2 *vec2f);

double Vec_NormalizeVec2(CVec2 *vec2f);

double Vec_LengthVec2d(CVec2d *vec2f);

double Vec_NormalizeVec2d(CVec2d *vec2f);


CVec2 Vec_AddVec2(const CVec2 *v0, const CVec2 *v1);

CVec2 Vec_SubVec2(const CVec2 *v0, const CVec2 *v1);

/*
//加法
*/
CVec2d Vec_AddVec2d(const CVec2d *v0, const CVec2d *v1);
/*
//减法
*/
CVec2d Vec_SubVec2d(const CVec2d *v0, const CVec2d *v1);


void Vec_Vec2fMulFloat(CVec2 *v0, double v1);

void Vec_Vec2fDivDouble(CVec2 *v0, double v1);

/*
//乘法
*/
void Vec_Vec2dMulDouble(CVec2d *v0, double v1);
/*
//除法
*/
void Vec_Vec2dDivDouble(CVec2d *v0, double v1);

void Vec_PrintVec2f(const CVec2f *v0);

void Vec_PrintVec2d(const CVec2d *v0);

double Vec_DotProduct2d(const CVec2d *v0, const CVec2d *v1);

#endif