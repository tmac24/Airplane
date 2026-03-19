#ifndef __QUAT__
#define __QUAT__
 
#include "_Vec3.h" 

typedef struct _CQuat
{
	double  _v[4]; 
}CQuatd, CQuat,*pCQuatd,*pCQuat ;


void CQuat_BindFunc(pCQuatd p);
CQuat CQuat_CreatDefault();
CQuat CQuat_Create(double x, double y, double z, double w);

CQuat CQuat_MultiCQuat(pCQuat orl, pCQuat rhs);
void CQuat_MakeRotateAxyz(pCQuat quat, double angle, double x, double y, double z);

void CQuat_MakeRotateAvec3f(pCQuat quat, double angle, CVec3f* vec);
void CQuat_MakeRotateAvec3d(pCQuat quat, double angle, CVec3d vec);

void CQuat_MakeRotateAvec3dAvec3dAvec3d(pCQuat quat, double angle1, CVec3d axis1,
	double angle2, CVec3d axis2,
	double angle3,  CVec3d  axis3);

void CQuat_MakeRotateAvec3fAvec3fAvec3f(pCQuat quat,
	double angle1, const CVec3f axis1,
	double angle2, const CVec3f axis2,
	double angle3, const CVec3f axis3);



void CQuat_MakeRotateFromVec3dVec3d(pCQuat quat, CVec3d from, CVec3d to);
void  CQuat_MakeRotateFromVec3fVec3f(pCQuat quat, const CVec3f* from, const CVec3f* to);

void  CQuat_GetRotateAxyz(pCQuat quat, double* angle, double* x, double* y, double* z);

void  CQuat_GetRotateAvec3f(pCQuat quat, double* angle, CVec3f* vec);
void  CQuat_GetRotateAvec3d(pCQuat quat, double* angle, CVec3d* vec);



void CQuat_Print(pCQuatd p);


#endif