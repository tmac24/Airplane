#ifndef __MATRIX__
#define __MATRIX__


#include "GLHeader.h"
#ifdef __cplusplus
extern "C" {
#endif

#include <float.h>
#include "_Vec3.h"
#include "_Vec4.h"
#include "Quat.h"

#ifdef __cplusplus
}
#endif

typedef double double_type;
typedef float other_value_type;

struct CMatrix;
//typedef struct _CMatrixd

struct CMatrix
{
	double_type _mat[4][4];
    CQuat(*GetRotate)(CMatrix* m);
    CVec3d(*GetTrans)(CMatrix* m);
    CVec3d(*GetScale)(CMatrix* m);
};// CMatrixd, CMatrix, *pCMatrixd, *pCMatrix;
typedef CMatrix CMatrixd;
typedef CMatrix* pCMatrix;
typedef CMatrixd* pCMatrixd;

//typedef struct _CMatrixf
struct CMatrixf
{
	float _mat[4][4];
};

void CMatrixd_BindFunc(pCMatrixd p);

void CMatrix_SETROW(CMatrix* p, unsigned int row, double_type v0, double_type v1, double_type v2, double_type v3);
CMatrix  CMatrix_Identity();
CMatrix CMatrix_Zero();
CMatrix CMatrix_Create(double_type a00, double_type a01, double_type a02, double_type a03,
	double_type a10, double_type a11, double_type a12, double_type a13,
	double_type a20, double_type a21, double_type a22, double_type a23,
	double_type a30, double_type a31, double_type a32, double_type a33);

void CMatrix_Set(CMatrix *p, double_type a00, double_type a01, double_type a02, double_type a03,
	double_type a10, double_type a11, double_type a12, double_type a13,
	double_type a20, double_type a21, double_type a22, double_type a23,
	double_type a30, double_type a31, double_type a32, double_type a33);

void CMatrix_SetTransXYZ(CMatrix*p, double_type tx, double_type ty, double_type tz);
void CMatrix_SetTransVec3f(CMatrix*p, const CVec3f v);
void CMatrix_SetTransVec3d(CMatrix*p, const CVec3d v);

void CMatrix_PreMultTranslateVec3f(CMatrix* p, const CVec3f v);
void CMatrix_PreMultTranslateVec3d(CMatrix* p, const CVec3d v);

void CMatrix_PreMultMatrix(CMatrix *p, const CMatrixd *other);
void CMatrix_PostMultMatrix(CMatrix *p, const CMatrixd *other);
CVec3f CMatrix_PreMultVec3f(CMatrix *p, const CVec3f* v);
static CVec3d CMatrix_PreMultVec3d(CMatrix *p, const CVec3d* v);
CVec3f CMatrix_PostMultVec3f(CMatrix *p, const CVec3f* v);
static CVec3d CMatrix_PostMultVec3d(CMatrix *p, const CVec3d* v);
static CVec4f CMatrix_preMultVec4f(CMatrix *p, const CVec4f* v);
static CVec4d CMatrix_preMultVec4d(CMatrix *p, const CVec4d* v);
static CVec4f CMatrix_postMultVec4f(CMatrix *p, const CVec4f* v);
static CVec4d CMatrix_postMultVec4d(CMatrix *p, const CVec4d* v);
double CMatrix_INNER_PRODUCT(const CMatrix *a, const CMatrix *b, unsigned int r, unsigned int c);
void CMatrix_mult(CMatrix *p, const CMatrix* lhs, const CMatrix* rhs);


CMatrixd CMatrix_LookAtXYZ( const double eye_x,const double eye_y,const double eye_z,
	const double center_x,const double center_y,const double center_z,
	const double up_x,const double up_y,const double up_z);
void CMatrix_MakeLookAtXYZ(CMatrix* p, const double eye_x,const double eye_y,const double eye_z,
	const double center_x,const double center_y,const double center_z,
	const double up_x,const double up_y,const double up_z);

void CMatrix_MakeLookAtVec3f(CMatrix* p, const CVec3f eye, const CVec3f center, const CVec3f up);
void CMatrix_MakeLookAtVec3d(CMatrix* p, const CVec3d eye, const CVec3d center, const CVec3d up);

int CMatrix_invert_4x3(CMatrix* p, const CMatrixd* rhs);

int CMatrix_invert_4x4(CMatrix* p, const CMatrixd* rhs);

int CMatrix_invert(CMatrix* p, const CMatrix* rhs);


CVec3f CMatrix_Transform3x3Vec3fMatrix(const CVec3f* v, const CMatrixd* m);
CVec3d CMatrix_Transform3x3Vec3dMatrix(const CVec3d* v, const CMatrixd* m);
CVec3f CMatrix_Transform3x3MatrixVec3f(const CMatrixd* m, const CVec3f* v);
CVec3d CMatrix_Transform3x3MatrixVec3d(const CMatrixd* m, const CVec3d* v);

void CMatrix_GetLookAtVec3f(CMatrix* p, CVec3f* eye, CVec3f* center, CVec3f* up, double_type lookDistance);
void CMatrix_GetLookAtVec3d(CMatrix* p, CVec3d* eye, CVec3d* center, CVec3d* up, double_type lookDistance);


CMatrixd CMatrix_LookAtVec3f(CVec3f eye, CVec3f center, CVec3f up);
CMatrixd CMatrix_LookAtVec3d(CVec3d* eye, CVec3d* center, CVec3d *up);

CMatrixd CMatrix_Inverse(const CMatrixd* matrix);
 
int CMatrix_IsIdentity(const CMatrixd* matrix);
void CMatrix_MakeIdentity(CMatrixd* matrix);

void CMatrix_SetMakeScaleVec3f(CMatrixd* matrix, const CVec3f* v);
void CMatrix_SetMakeScaleVec3d(CMatrixd* matrix, const CVec3d* v);
void CMatrix_SetMakeScaleXYZ(CMatrixd* matrix, double_type x, double_type y, double_type z);

void CMatrix_SetMakeTranslateVec3f(CMatrixd* matrix, const CVec3f* v);
void CMatrix_SetMakeTranslateVec3d(CMatrixd* matrix, const CVec3d* v);
void CMatrix_SetMakeTranslateXYZ(CMatrixd* matrix, double_type x, double_type y, double_type z);

CMatrixd CMatrix_ScaleVec3f(const CVec3f* v);
CMatrixd CMatrix_ScaleVec3d(const CVec3d* v);
CMatrixd CMatrix_ScaleXYZ(double_type sx, double_type sy, double_type sz);

CMatrixd CMatrix_TranslateVec3f(const CVec3f v);
CMatrixd CMatrix_TranslateVec3d(const CVec3d v);
CMatrixd CMatrix_TranslateXYZ(double_type tx, double_type ty, double_type tz);
CMatrixf CMatrix_d2f(CMatrixd md);

void CMatrix_SetRotate(CMatrixd* m, const pCQuat q);
void CMatrix_MakeRotateVec3fVec3f(CMatrixd* m, const CVec3f* from, const CVec3f* to);
void CMatrix_MakeRotateVec3dVec3d(CMatrixd* m, const CVec3d* from, const CVec3d* to);
void CMatrix_MakeRotateAVec3f(CMatrixd* m, double_type angle, CVec3f* axis);
void CMatrix_MakeRotateAVec3d(CMatrixd* m, double_type angle, CVec3d* axis);
void CMatrix_MakeRotateAXYZ(CMatrixd* m, double_type angle, double_type x, double_type y, double_type z);
void CMatrix_MakeRotateQuat(CMatrixd* m, pCQuat q);
void CMatrix_MakeRotateAVec3fAVec3fAVec3f(CMatrixd* m, double_type angle1, const CVec3f axis1,
	double_type angle2, const CVec3f axis2,
	double_type angle3, const CVec3f axis3);
void CMatrix_MakeRotateAVec3dAVec3dAVec3d(CMatrixd* m, double_type angle1, CVec3d axis1,
	double_type angle2, CVec3d axis2,
	double_type angle3, CVec3d axis3);

CMatrixd CMatrix_RotateVec3fVec3f(const CVec3f* from, const CVec3f* to);
CMatrixd CMatrix_RotateVec3dVec3d(const CVec3d* from, const CVec3d* to);
CMatrixd CMatrix_RotateAXYZ(double_type angle, double_type x, double_type y, double_type z);
CMatrixd CMatrix_RotateAVec3f(double_type angle, CVec3f axis);
CMatrixd CMatrix_RotateAVec3d(double_type angle, CVec3d axis);
CMatrixd CMatrix_RotateAVec3fAVec3fAVec3f(double_type angle1, const CVec3f axis1,
	double_type angle2, const CVec3f axis2,
	double_type angle3, const CVec3f axis3);
CMatrixd CMatrix_RotateAVec3dAVec3dAVec3d(double_type angle1, CVec3d axis1,
	double_type angle2, CVec3d axis2,
	double_type angle3, CVec3d axis3);
CMatrixd CMatrix_RotateQuat(pCQuat quat);

CMatrixd CMatrix_MatrixMultMatrix(const pCMatrix matL,const pCMatrix matR);
CVec3f CMatrix_Vec3fMultMatrixToVec3f(CVec3f vec,const pCMatrix matR);
CVec3d CMatrix_Vec3fMultMatrixToVec3d(CVec3f vec,const pCMatrix matR);
CVec3f  CMatrix_Vec3dMultMatrixToVec3f(CVec3d vec,const pCMatrix matR);
CVec3d CMatrix_Vec3dMultMatrixToVec3d(CVec3d vec,const pCMatrix matR);

CVec4f CMatrix_Vec4fMultMatrixToVec4f(pCMatrix p, const CVec4f v);
CVec4d CMatrix_Vec4fMultMatrixToVec4d(pCMatrix p, const CVec4d v);
CVec4f CMatrix_MatrixMultVec4f(pCMatrix p, const CVec4f v);
CVec4d CMatrix_MatrixMultVec4d(pCMatrix p, const CVec4d v);


CQuat CMatrix_GetRotate(CMatrixd* m);
CVec3d CMatrix_GetTrans(CMatrixd* m);
CVec3d CMatrix_GetScale(CMatrixd* m);

CMatrix CMatrix_GetRotateMatrix(pCMatrix p);
CMatrix CMatrix_GetTransMatrix(pCMatrix p);


void  CMatrix_MakeOrtho(pCMatrix p,double left, double right,	double bottom, double top,	double zNear, double zFar);
CMatrix  CMatrix_Ortho(double left, double right,	double bottom, double top,	double zNear, double zFar);
void CMatrix_MakeOrtho2D(pCMatrix p,double left,   double right,double bottom, double top);
CMatrix CMatrix_Ortho2D(double left,   double right,double bottom, double top);
void CMatrix_MakeFrustum(pCMatrix p,double left, double right,	double bottom, double top,	double zNear, double zFar);
void CMatrix_MakePerspective(pCMatrix p,double fovy,double aspectRatio,double zNear, double zFar);
void CMatrix_MakeFrustum(pCMatrix p,double left, double right,	double bottom, double top,	double zNear, double zFar);
CMatrix CMatrix_Perspective(double fovy,double aspectRatio,double zNear, double zFar);
int CMatrix_GetOrtho(pCMatrix p,double * left, double * right,	double * bottom, double * top,	double * zNear, double * zFar);
int CMatrix_GetFrustum(pCMatrix p,double* left, double* right,	double* bottom, double* top,	double* zNear, double* zFar);
int CMatrix_GetPerspective(pCMatrix p,double * fovy, double * aspectRatio,double * zNear, double * zFar);

#endif