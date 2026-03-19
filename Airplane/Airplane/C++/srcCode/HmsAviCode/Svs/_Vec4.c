#include "_Vec4.h"
#include <stdio.h>
#pragma warning(disable:4244) 

CVec4 Vec_CreatVec4f(double x, double y,double z,double w)
{
	CVec4 n;
	n._v[0] = x;
	n._v[1] = y;
	n._v[2] = z;
	n._v[3] = w;
	return n;
}

CVec4d Vec_CreatVec4d(double x, double y,double z,double w)
{
	CVec4d n;
	n._v[0] = x;
	n._v[1] = y;
	n._v[2] = z;
	n._v[3] = w;
	return n;
}


/*
//求长度
*/
double Vec_LengthVec4f(const CVec4 *vec2f)
{
	return sqrt(vec2f->_v[0]*vec2f->_v[0] + vec2f->_v[1]*vec2f->_v[1] + vec2f->_v[2]*vec2f->_v[2] + vec2f->_v[3]*vec2f->_v[3]);
}

/*
//求长度
*/
double Vec_LengthVec4d(const CVec4d *vec2f)
{
	return sqrt(vec2f->_v[0]*vec2f->_v[0] + vec2f->_v[1]*vec2f->_v[1] + vec2f->_v[2]*vec2f->_v[2] + vec2f->_v[3]*vec2f->_v[3]);
}

CVec4d Vec_Vec4dAddVec4d(const CVec4d *v0, const CVec4d *v1)
{
	CVec4d n;
	n._v[0] = v0->_v[0] + v1->_v[0];
	n._v[1] = v0->_v[1] + v1->_v[1];
	n._v[2] = v0->_v[2] + v1->_v[2];
	n._v[3] = v0->_v[3] + v1->_v[3];
	return n;
}

void Vec_Vec4dMulDouble(CVec4d *v0, double v1)
{
	v0->_v[0] *= v1;
	v0->_v[1] *= v1;
	v0->_v[2] *= v1;
	v0->_v[3] *= v1;
}

pCVec4d Vec_CreatpCVec4d(CVec4d v)
{
	pCVec4d pVec4d = (pCVec4d)malloc(sizeof(CVec4d));
	pVec4d->_v[0] = v._v[0];
	pVec4d->_v[1] = v._v[1];
	pVec4d->_v[2] = v._v[2];
	pVec4d->_v[3] = v._v[3];
	return pVec4d;

}

void Vec_PrintVec4d(const CVec4d *vec)
{
	printf("CVec4d: %lf,%lf,%lf,%lf\n", vec->_v[0], vec->_v[1], vec->_v[2], vec->_v[3]);
}
void Vec_PrintVec4f(const CVec4f *vec)
{
	printf("CVec4d: %f,%f,%f,%f\n", vec->_v[0], vec->_v[1], vec->_v[2], vec->_v[3]);
}

CVec4d Vec_Vec4dSubVec4d(const CVec4d *v0, const CVec4d *v1)
{
	CVec4d n;
	n._v[0] = v0->_v[0] - v1->_v[0];
	n._v[1] = v0->_v[1] - v1->_v[1];
	n._v[2] = v0->_v[2] - v1->_v[2];
	n._v[3] = v0->_v[3] - v1->_v[3];
	return n;
}

int Vec_EqualVec4d(const CVec4d *vec, const CVec4d *vec1)
{
	return ((vec->_v[0] == vec1->_v[0]) && 
			(vec->_v[0] == vec1->_v[0]) && 
			(vec->_v[0] == vec1->_v[0]) && 
			(vec->_v[0] == vec1->_v[0]));
}
