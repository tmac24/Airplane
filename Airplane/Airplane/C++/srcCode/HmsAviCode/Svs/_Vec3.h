#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef __VEC3__
#define __VEC3__

typedef struct _CVec3
{
    float _v[3];
}CVec3f, CVec3, *pCVec3, *pCVec3f;

typedef struct _CVec3d
{
    double _v[3];
}CVec3d, *pCVec3d;

CVec3f Vec_CreatVec3f(float x, float y, float z);

CVec3d Vec_CreatVec3d(double x, double y, double z);

void Vec_Set3d(pCVec3d v, double x, double y, double z);

CVec3d Vec_Vec3fToVec3d(CVec3 v);
CVec3f Vec_Vec3dToVec3f(CVec3d v);

CVec3d Vec_pVec3fToVec3d(pCVec3 v);
CVec3f Vec_pVec3dToVec3f(pCVec3d v);

/*
求点积vec->_v[0]*vec->_v[0] + vec->_v[1]*vec->_v[1] + vec->_v[2]*vec->_v[2]
*/
double Vec_Length2Vec3f(const CVec3f *vec);
/*
求点积vec->_v[0]*vec->_v[0] + vec->_v[1]*vec->_v[1] + vec->_v[2]*vec->_v[2]
*/
double Vec_Length2Vec3d(const CVec3d *vec);
double Vec_LengthVec3f(const CVec3 *vec3f);;
double Vec_LengthVec3d(const CVec3d *vec3d);
double Vec_NormalizeVec3f(CVec3 *vec3f);
double Vec_NormalizeVec3d(CVec3d *vec3d);

CVec3 Vec_Vec3fAddVec3f(const CVec3 *v0, const CVec3 *v1);
CVec3 Vec_Vec3fSubVec3f(const CVec3 *v0, const CVec3 *v1);

CVec3d Vec_Vec3dAddVec3d(const CVec3d *v0, const CVec3d *v1);

CVec3d Vec_Vec3dSubVec3d(const CVec3d *v0, const CVec3d *v1);
double Vec_Vec3dDistance(const CVec3d *v0, const CVec3d *v1);


void Vec_Vec3MulDouble(CVec3 *v0, double v1);

void Vec_Vec3fDivDouble(CVec3 *v0, double v1);

/*
//乘法
*/
void Vec_Vec3dMulDouble(CVec3d *v0, double v1);
/*
//除法
*/
CVec3d Vec_Vec3dDivDouble(CVec3d *v0, double v1);


double Vec_DotProductVec3f(const CVec3f *vec, const CVec3f *vec1);

double Vec_DotProductVec3d(const CVec3d *vec, const CVec3d *vec1);

/*
求叉积  返回垂直向量
a × b = [a2b3 -a3b2, a3b1 - a1b3, a1b2- a2b1]
*/
CVec3f Vec_CrossProductVec3f(const CVec3f *vec, const CVec3f *vec1);
/*
求叉积  返回垂直向量
a × b = [a2b3 -a3b2, a3b1 - a1b3, a1b2- a2b1]
*/
CVec3d Vec_CrossProductVec3d(const CVec3d *vec, const CVec3d *vec1);
void Vec_PrintVec3d(const CVec3d *vec);
void Vec_PrintVec3f(const CVec3f *vec);

pCVec3d Vec_CreatpCVec3d(CVec3d v);
int Vec_Vec3dCompareVec3d(const CVec3d *aVec1, const CVec3d *aVec2);

void Vec_CalcVec3dNormal(pCVec3d result, const pCVec3d point1, const pCVec3d point2, const pCVec3d point3);
void Vec_CalcVec3fNormal(pCVec3f result, const pCVec3f point1, const pCVec3f point2, const pCVec3f point3);

CVec3d Vec_CalcVec3dDirNormal(const pCVec3d v1, const pCVec3d v2);


/*******************************************************************************
名称：Vec_CosValueVec3f
功能：求向量之间的cos值
参数：无
时间:  16:42:32 8/1/2015
用户 : xiafei
版本：1.0
注意：
返回值：
*******************************************************************************/
double Vec_CosValueVec3f(const CVec3f *vec, const CVec3f *vec1);

#endif

