
#include "Quat.h"
#pragma warning(disable:4244) 

void CQuat_BindFunc(pCQuatd p)
{
/*	p->getRotate = CQuat_GetRotateAxyz;*/
}

CQuat CQuat_CreatDefault()
{
	CQuat quat;
	quat._v[0] = 0.0; quat._v[1] = 0.0; quat._v[2] = 0.0; quat._v[3] = 1.0;
	CQuat_BindFunc(&quat);
	return quat;
}
CQuat CQuat_Create(double x, double y, double z, double w)
{
	CQuat nnd;
	nnd._v[0] = x;
	nnd._v[1] = y;
	nnd._v[2] = z;
	nnd._v[3] = w;
	CQuat_BindFunc(&nnd);
	return nnd;
}

CQuat CQuat_MultiCQuat(pCQuat orl, pCQuat rhs)
{
	
    double x= 	rhs->_v[3] * orl->_v[0] + rhs->_v[0] * orl->_v[3] + rhs->_v[1] * orl->_v[2] - rhs->_v[2] * orl->_v[1] ;
    double y= 	rhs->_v[3] * orl->_v[1] - rhs->_v[0] * orl->_v[2] + rhs->_v[1] * orl->_v[3] + rhs->_v[2] * orl->_v[0] ;
    double z= 	rhs->_v[3] * orl->_v[2] + rhs->_v[0] * orl->_v[1] - rhs->_v[1] * orl->_v[0] + rhs->_v[2] * orl->_v[3] ;
    double w= 	rhs->_v[3] * orl->_v[3] - rhs->_v[0] * orl->_v[0] - rhs->_v[1] * orl->_v[1] - rhs->_v[2] * orl->_v[2] ;
	return CQuat_Create(x,y,z,w);
}

void CQuat_MakeRotateAxyz(pCQuat quat, double angle, double x, double y, double z)
{
	if (quat)
	{
		const double epsilon = 0.0000001;
		double inversenorm;
		double coshalfangle;
		double sinhalfangle;
		double length = sqrt(x*x + y*y + z*z);
		if (length < epsilon)
		{
			*quat = CQuat_CreatDefault();
			return;
		}
		inversenorm = 1.0 / length;
		coshalfangle = cos(0.5*angle);
		sinhalfangle = sin(0.5*angle);

		quat->_v[0] = x * sinhalfangle * inversenorm;
		quat->_v[1] = y * sinhalfangle * inversenorm;
		quat->_v[2] = z * sinhalfangle * inversenorm;
		quat->_v[3] = coshalfangle;
	}
}


void CQuat_MakeRotateAvec3f(pCQuat quat, double angle, CVec3f* vec)
{
	CQuat_MakeRotateAxyz(quat, angle, vec->_v[0], vec->_v[1], vec->_v[2]);
}
void CQuat_MakeRotateAvec3d(pCQuat quat, double angle, CVec3d vec)
{
	CQuat_MakeRotateAxyz(quat, angle, vec._v[0], vec._v[1], vec._v[2]);
}


void CQuat_MakeRotateAvec3dAvec3dAvec3d(pCQuat quat, double angle1, CVec3d axis1,
	double angle2, CVec3d axis2,
	double angle3,  CVec3d axis3)
{
	if (quat)
	{
		CQuat q1 = CQuat_CreatDefault(), q2 = CQuat_CreatDefault(), q3 = CQuat_CreatDefault();
		CQuat_MakeRotateAvec3d(&q1, angle1, axis1);
		CQuat_MakeRotateAvec3d(&q2, angle2, axis2);
		CQuat_MakeRotateAvec3d(&q3, angle3, axis3);

		*quat = CQuat_MultiCQuat(&q1, &q2);
		*quat = CQuat_MultiCQuat(quat, &q3);

		/**this = q1*q2*q3;*/
	}
}

void CQuat_MakeRotateAvec3fAvec3fAvec3f(pCQuat quat,
	double angle1, const CVec3f axis1,
	double angle2, const CVec3f axis2,
	double angle3, const CVec3f axis3)
{ 
	CQuat_MakeRotateAvec3dAvec3dAvec3d(quat,
		angle1, Vec_Vec3fToVec3d(axis1),
		angle2, Vec_Vec3fToVec3d(axis2),
		angle3, Vec_Vec3fToVec3d(axis3));
}






void CQuat_MakeRotateFromVec3dVec3d(pCQuat quat, CVec3d from, CVec3d to)
{
	CVec3d sourceVector = from;
	CVec3d targetVector = to;

	double fromLen2 = Vec_Length2Vec3d(&from);
	double fromLen;
	double toLen2;


	double dotProdPlus1;
	/*
	// normalize only when necessary, epsilon test
	*/
	if ((fromLen2 < 1.0 - 1e-7) || (fromLen2 > 1.0 + 1e-7))
	{
		fromLen = sqrt(fromLen2);

		Vec_Vec3dDivDouble(&sourceVector, fromLen);
		/*
		//sourceVector /= fromLen;
		*/
	}
	else fromLen = 1.0;

	toLen2 = Vec_Length2Vec3d(&to);
	/*
	//to.length2();
	// normalize only when necessary, epsilon test
	*/
	if ((toLen2 < 1.0 - 1e-7) || (toLen2 > 1.0 + 1e-7))
	{
		double toLen;
		/*
		// re-use fromLen for case of mapping 2 vectors of the same length
		*/
		if ((toLen2 > fromLen2 - 1e-7) && (toLen2 < fromLen2 + 1e-7)) {
			toLen = fromLen;
		}
		else toLen = sqrt(toLen2);
		Vec_Vec3dDivDouble(&targetVector, toLen);
		/*
		//	targetVector /= toLen;
		*/
	}


	/*Now let's get into the real stuff
	// Use "dot product plus one" as test as it can be re-used later on*/
	dotProdPlus1 = 1.0 + Vec_DotProductVec3d(&sourceVector, &targetVector);
	/*
// sourceVector * targetVector;
// Check for degenerate case of full u-turn. Use epsilon for detection
*/

	if (dotProdPlus1 < 1e-7) {

		/*
		// Get an orthogonal vector of the given vector
		// in a plane with maximum vector coordinates.
		// Then use it as quaternion axis with pi angle
		// Trick is to realize one value at least is >0.6 for a normalized vector.
		*/
		if (fabs(sourceVector._v[0]) < 0.6) {
			const double norm = sqrt(1.0 - sourceVector._v[0]  * sourceVector._v[0]);
			quat->_v[0] = 0.0;
			quat->_v[1] = sourceVector._v[2] / norm;
			quat->_v[2] = -sourceVector._v[1] / norm;
			quat->_v[3] = 0.0;
		}
		else if (fabs(sourceVector._v[1]) < 0.6) {
			const double norm = sqrt(1.0 - sourceVector._v[1] * sourceVector._v[1]);
			quat->_v[0] = -sourceVector._v[2] / norm;
			quat->_v[1] = 0.0;
			quat->_v[2] = sourceVector._v[0] / norm;
			quat->_v[3] = 0.0;
		}
		else {
			const double norm = sqrt(1.0 - sourceVector._v[2] * sourceVector._v[2]);
			quat->_v[0] = sourceVector._v[1] / norm;
			quat->_v[1] = -sourceVector._v[0] / norm;
			quat->_v[2] = 0.0;
			quat->_v[3] = 0.0;
		}
	}

	else {
		/*
// Find the shortest angle quaternion that transforms normalized vectors
// into one other. Formula is still valid when vectors are colinear
*/
		const double s = sqrt(0.5 * dotProdPlus1);
		CVec3d tmp0 = Vec_CrossProductVec3d(&sourceVector, &targetVector);
		const CVec3d tmp = Vec_Vec3dDivDouble(&tmp0, 2.0*s);
		/*
		// sourceVector ^ targetVector / (2.0*s);
		*/
		quat->_v[0] = tmp._v[0];
		quat->_v[1] = tmp._v[1];
		quat->_v[2] = tmp._v[2];
		quat->_v[3] = s;
	}
}

void  CQuat_MakeRotateFromVec3fVec3f(pCQuat quat, const CVec3f* from, const CVec3f* to)
{
	CQuat_MakeRotateFromVec3dVec3d(quat, Vec_Vec3fToVec3d(*from), Vec_Vec3fToVec3d(*to));
}

void  CQuat_GetRotateAxyz(pCQuat quat, double* angle, double* x, double* y, double* z)
{
	if (quat)
	{
		double sinhalfangle = sqrt(quat->_v[0] * quat->_v[0] + quat->_v[1] * quat->_v[1] + quat->_v[2] * quat->_v[2]);

		*angle = 2.0 * atan2(sinhalfangle, quat->_v[3]);
		if (sinhalfangle)
		{
			*x = quat->_v[0] / sinhalfangle;
			*y = quat->_v[1] / sinhalfangle;
			*z = quat->_v[2] / sinhalfangle;
		}
		else
		{
			*x = 0.0;
			*y = 0.0;
			*z = 1.0;
		}
	}
}


void  CQuat_GetRotateAvec3f(pCQuat quat, double* angle, CVec3f* vec)
{
	if (quat && angle && vec)
	{
		double x, y, z;
		CQuat_GetRotateAxyz(quat, angle, &x, &y, &z);
		vec->_v[0] = x;
		vec->_v[1] = y;
		vec->_v[2] = z;
	}
}
void  CQuat_GetRotateAvec3d(pCQuat quat, double* angle, CVec3d* vec)
{
	if (quat && angle && vec)
	{
		double x, y, z;
		CQuat_GetRotateAxyz(quat, angle, &x, &y, &z);
		vec->_v[0] = x;
		vec->_v[1] = y;
		vec->_v[2] = z;
	}
}


void CQuat_Print(pCQuatd p)
{
	printf("四元数:%f,%f,%f,%f\n",p->_v[0],p->_v[1],p->_v[2],p->_v[3]);
}

	


/*

//
//CQuat(const CVec4f& v)
//{
//	_v[0] = v._v[0]();
//	_v[1] = v._v[1]();
//	_v[2] = v._v[2]();
//	_v[3] = v._v[3]();
//}
//
//inline CQuat(const CVec4d& v)
//{
//	_v[0] = v._v[0]();
//	_v[1] = v._v[1]();
//	_v[2] = v._v[2]();
//	_v[3] = v._v[3]();
//}
//
//inline CQuat(double angle, const CVec3f& axis)
//{
//	makeRotate(angle, axis);
//}
//inline CQuat(double angle, const CVec3d& axis)
//{
//	makeRotate(angle, axis);
//}
//
//inline CQuat(double angle1, const CVec3f& axis1,
//	double angle2, const CVec3f& axis2,
//	double angle3, const CVec3f& axis3)
//{
//	makeRotate(angle1, axis1, angle2, axis2, angle3, axis3);
//}
//
//inline CQuat(double angle1, const CVec3d& axis1,
//	double angle2, const CVec3d& axis2,
//	double angle3, const CVec3d& axis3)
//{
//	makeRotate(angle1, axis1, angle2, axis2, angle3, axis3);
//}
//inline void set(double x, double y, double z, double w)
//{
//	_v[0] = x;
//	_v[1] = y;
//	_v[2] = z;
//	_v[3] = w;
//}
//
//inline void set(const CVec4f& v)
//{
//	_v[0] = v._v[0]();
//	_v[1] = v._v[1]();
//	_v[2] = v._v[2]();
//	_v[3] = v._v[3]();
//}
//
//inline void set(const CVec4d& v)
//{
//	_v[0] = v._v[0]();
//	_v[1] = v._v[1]();
//	_v[2] = v._v[2]();
//	_v[3] = v._v[3]();
//}
//inline double & operator [] (int i) { return _v[i]; }
//inline double   operator [] (int i) const { return _v[i]; }
//
//inline double & x() { return _v[0]; }
//inline double & y() { return _v[1]; }
//inline double & z() { return _v[2]; }
//inline double & w() { return _v[3]; }
//
//inline double x() const { return _v[0]; }
//inline double y() const { return _v[1]; }
//inline double z() const { return _v[2]; }
//inline double w() const { return _v[3]; }
//
//inline const CQuat operator * (double rhs) const
//{
//	return CQuat(_v[0] * rhs, _v[1] * rhs, _v[2] * rhs, _v[3] * rhs);
//}
//
//inline CQuat& operator *= (double rhs)
//{
//	_v[0] *= rhs;
//	_v[1] *= rhs;
//	_v[2] *= rhs;
//	_v[3] *= rhs;
//	return *this;        // enable nesting
//}
//inline const CQuat operator*(const CQuat& rhs) const
//{
//	return CQuat(rhs._v[3] * _v[0] + rhs._v[0] * _v[3] + rhs._v[1] * _v[2] - rhs._v[2] * _v[1],
//		rhs._v[3] * _v[1] - rhs._v[0] * _v[2] + rhs._v[1] * _v[3] + rhs._v[2] * _v[0],
//		rhs._v[3] * _v[2] + rhs._v[0] * _v[1] - rhs._v[1] * _v[0] + rhs._v[2] * _v[3],
//		rhs._v[3] * _v[3] - rhs._v[0] * _v[0] - rhs._v[1] * _v[1] - rhs._v[2] * _v[2]);
//}
//
///// Unary multiply
//inline CQuat& operator*=(const CQuat& rhs)
//{
//	double x = rhs._v[3] * _v[0] + rhs._v[0] * _v[3] + rhs._v[1] * _v[2] - rhs._v[2] * _v[1];
//	double y = rhs._v[3] * _v[1] - rhs._v[0] * _v[2] + rhs._v[1] * _v[3] + rhs._v[2] * _v[0];
//	double z = rhs._v[3] * _v[2] + rhs._v[0] * _v[1] - rhs._v[1] * _v[0] + rhs._v[2] * _v[3];
//	_v[3] = rhs._v[3] * _v[3] - rhs._v[0] * _v[0] - rhs._v[1] * _v[1] - rhs._v[2] * _v[2];
//
//	_v[2] = z;
//	_v[1] = y;
//	_v[0] = x;
//
//	return (*this);            // enable nesting
//}
//inline const CQuat operator - () const
//{
//	return CQuat(-_v[0], -_v[1], -_v[2], -_v[3]);
//}
//
//double length() const
//{
//	return sqrt(_v[0] * _v[0] + _v[1] * _v[1] + _v[2] * _v[2] + _v[3] * _v[3]);
//}
//
//double length2() const
//{
//	return _v[0] * _v[0] + _v[1] * _v[1] + _v[2] * _v[2] + _v[3] * _v[3];
//}
//
//void makeRotate(double  angle,
//	double  x, double  y, double  z);
//void makeRotate(double  angle, const CVec3f& vec);
//void makeRotate(double  angle, const CVec3d& vec);
//
//void makeRotate(double  angle1, const CVec3f& axis1,
//	double  angle2, const CVec3f& axis2,
//	double  angle3, const CVec3f& axis3);
//void makeRotate(double  angle1, const CVec3d& axis1,
//	double  angle2, const CVec3d& axis2,
//	double  angle3, const CVec3d& axis3);
//
//void makeRotate(const CVec3f& vec1, const CVec3f& vec2);
//void makeRotate(const CVec3d& vec1, const CVec3d& vec2);
//
////void makeRotate_original(const CVec3d& vec1, const CVec3d& vec2);
//
//void getRotate(double& angle, CVec3f& vec) const;
//void getRotate(double& angle, CVec3d& vec) const;
//void getRotate(double& angle, double& x, double& y, double& z) const;
*/