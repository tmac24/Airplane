#include "_Vec2.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#pragma warning(disable:4244) 
CVec2 Vec_CreatVec2(float x, float y)
{
	CVec2 n;
	n._v[0] = x;
	n._v[1] = y;
	return n;
}

CVec2 Vec_CreatVec2f(float x, float y)
{
	return Vec_CreatVec2(x,y);
}

CVec2d Vec_CreatVec2d(double x, double y)
{
	CVec2d n;
	n._v[0] = x;
	n._v[1] = y;
	return n;
}


double Vec_LengthVec2(CVec2 *vec2f)
{
	return sqrt(vec2f->_v[0]*vec2f->_v[0] + vec2f->_v[1]*vec2f->_v[1]);
}

double Vec_NormalizeVec2(CVec2 *vec2f)
{
	double leng = Vec_LengthVec2(vec2f);
	vec2f->_v[0] /= leng;
	vec2f->_v[1] /= leng;
	return leng;
}

double Vec_LengthVec2d(CVec2d *vec2f)
{
	return sqrt(vec2f->_v[0]*vec2f->_v[0] + vec2f->_v[1]*vec2f->_v[1]);
}

double Vec_NormalizeVec2d(CVec2d *vec2f)
{
	double leng = Vec_LengthVec2d(vec2f);
	vec2f->_v[0] /= leng;
	vec2f->_v[1] /= leng;
	return leng;
}


CVec2 Vec_AddVec2(const CVec2 *v0, const CVec2 *v1)
{
	CVec2 n;
	n._v[0] = v0->_v[0] + v1->_v[0];
	n._v[1] = v0->_v[1] + v1->_v[1];
	return n;
}

CVec2 Vec_SubVec2(const CVec2 *v0, const CVec2 *v1)
{
	CVec2 n;
	n._v[0] = v0->_v[0] - v1->_v[0];
	n._v[1] = v0->_v[1] - v1->_v[1];
	return n;
}

/*
//加法
*/
CVec2d Vec_AddVec2d(const CVec2d *v0, const CVec2d *v1)
{
	CVec2d n;
	n._v[0] = v0->_v[0] + v1->_v[0];
	n._v[1] = v0->_v[1] + v1->_v[1];
	return n;
};
/*
//减法
*/
CVec2d Vec_SubVec2d(const CVec2d *v0, const CVec2d *v1)
{

	CVec2d n;
	n._v[0] = v0->_v[0] - v1->_v[0];
	n._v[1] = v0->_v[1] - v1->_v[1];
	return n;
}


void Vec_Vec2fMulFloat(CVec2 *v0, double v1)
{
	v0->_v[0] *= v1;
	v0->_v[1] *= v1;
}

void Vec_Vec2fDivDouble(CVec2 *v0, double v1)
{
	v0->_v[0] /= v1;
	v0->_v[1] /= v1;
}

/*
//乘法
*/
void Vec_Vec2dMulDouble(CVec2d *v0, double v1)
{
	v0->_v[0] *= v1;
	v0->_v[1] *= v1;
}
/*
//除法
*/
void Vec_Vec2dDivDouble(CVec2d *v0, double v1)
{
	v0->_v[0] /= v1;
	v0->_v[1] /= v1;
}

void Vec_PrintVec2f(const CVec2f *vec)
{
	printf("CVec2f: %f,%f\n", vec->_v[0], vec->_v[1]);
}
void Vec_PrintVec2d(const CVec2d *vec)
{
	printf("CVec2d: %lf,%lf\n", vec->_v[0], vec->_v[1]);
}

pCVec2i Vec_CreatVec2iPtr(int x, int y)
{
	pCVec2i vec = (pCVec2i)malloc(sizeof(CVec2i)); 
	if (vec)
	{
		vec->_v[0] = x; 
		vec->_v[1] = y;
	}
	
	return vec;
}
void Vec_DestoryVec2iPtr(pCVec2i *vec)
{
	if (vec)
	{
		free( *vec);
		*vec = 0;
	}
}

double Vec_DotProduct2d(const CVec2d *v0, const CVec2d *v1)
{
	return (v0->_v[0]*v1->_v[0]+v0->_v[1]*v1->_v[1]);
}