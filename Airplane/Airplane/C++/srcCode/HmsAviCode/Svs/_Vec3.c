#include "_Vec3.h"

#pragma warning(disable:4244) 

 CVec3f Vec_CreatVec3f(float x, float y, float z)
{
	CVec3f n;
	n._v[0] = x;
	n._v[1] = y;
	n._v[2] = z;
	return n;
} 


 CVec3d Vec_CreatVec3d(double x, double y, double z)
{
	CVec3d n;
	n._v[0] = x;
	n._v[1] = y;
	n._v[2] = z;
	return n;
}

 void Vec_Set3d(pCVec3d v, double x, double y, double z)
 {
	 if(v)	 v->_v[0] = x, v->_v[1] = y, v->_v[2] = z;

 }

 CVec3d Vec_Vec3fToVec3d(CVec3 v)
 {
	 return Vec_CreatVec3d(v._v[0],v._v[1],v._v[2]);
 }
 CVec3f Vec_Vec3dToVec3f(CVec3d v)
 {
	 return Vec_CreatVec3f(v._v[0],v._v[1],v._v[2]);
 }

 CVec3d Vec_pVec3fToVec3d(pCVec3 v)
 {
	 return Vec_CreatVec3d(v->_v[0],v->_v[1],v->_v[2]);
 }
 CVec3f Vec_pVec3dToVec3f(pCVec3d v)
 {
	 return Vec_CreatVec3f(v->_v[0],v->_v[1],v->_v[2]);
 }


/*
求点积vec->_v[0]*vec->_v[0] + vec->_v[1]*vec->_v[1] + vec->_v[2]*vec->_v[2]
*/
 double Vec_Length2Vec3f(const CVec3f *vec)
{
	return vec->_v[0]*vec->_v[0] + vec->_v[1]*vec->_v[1] + vec->_v[2]*vec->_v[2];
}
/*
求点积vec->_v[0]*vec->_v[0] + vec->_v[1]*vec->_v[1] + vec->_v[2]*vec->_v[2]
*/
 double Vec_Length2Vec3d(const CVec3d *vec)
{
	return vec->_v[0]*vec->_v[0] + vec->_v[1]*vec->_v[1] + vec->_v[2]*vec->_v[2];
} 

 double Vec_LengthVec3f(const CVec3 *vec2f)
{
	return sqrt(Vec_Length2Vec3f(vec2f));
}

 double Vec_NormalizeVec3f(CVec3 *vec2f)
{
	double leng = Vec_LengthVec3f(vec2f);
	Vec_Vec3fDivDouble(vec2f,leng);
	return leng;
}

 double Vec_LengthVec3d(const CVec3d *vec2f)
{
	return sqrt(Vec_Length2Vec3d(vec2f));
}

 double Vec_NormalizeVec3d(CVec3d *vec3d)
{
	double leng = Vec_LengthVec3d(vec3d);
	Vec_Vec3dDivDouble(vec3d,leng);
	return leng;
}

 CVec3 Vec_Vec3fAddVec3f(const CVec3 *v0, const CVec3 *v1)
{
	CVec3 n;
	n._v[0] = v0->_v[0] + v1->_v[0];
	n._v[1] = v0->_v[1] + v1->_v[1];
	n._v[2] = v0->_v[2] + v1->_v[2];
	return n;
}

 CVec3 Vec_Vec3fSubVec3f(const CVec3 *v0, const CVec3 *v1)
{
	CVec3 n;
	n._v[0] = v0->_v[0] - v1->_v[0];
	n._v[1] = v0->_v[1] - v1->_v[1];
	n._v[2] = v0->_v[2] - v1->_v[2];
	return n;
}

 CVec3d Vec_Vec3dAddVec3d(const CVec3d *v0, const CVec3d *v1)
{
	CVec3d n;
	n._v[0] = v0->_v[0] + v1->_v[0];
	n._v[1] = v0->_v[1] + v1->_v[1];
	n._v[2] = v0->_v[2] + v1->_v[2];
	return n;
}

 CVec3d Vec_Vec3dSubVec3d(const CVec3d *v0, const CVec3d *v1)
{
	CVec3d n;
	n._v[0] = v0->_v[0] - v1->_v[0];
	n._v[1] = v0->_v[1] - v1->_v[1];
	n._v[2] = v0->_v[2] - v1->_v[2];
	return n;
}

 
 double Vec_Vec3dDistance(const CVec3d *v0, const CVec3d *v1)
 {
     CVec3d delta = Vec_Vec3dSubVec3d(v0, v1);
     return Vec_LengthVec3d(&delta);
 }

 void Vec_Vec3MulDouble(CVec3 *v0, double v1)
{
	v0->_v[0] *= v1;
	v0->_v[1] *= v1;
	v0->_v[2] *= v1;
}

 void Vec_Vec3fDivDouble(CVec3 *v0, double v1)
{
	v0->_v[0] /= v1;
	v0->_v[1] /= v1;
	v0->_v[2] /= v1;
}

/*
//乘法
*/
 void Vec_Vec3dMulDouble(CVec3d *v0, double v1)
{
	v0->_v[0] *= v1;
	v0->_v[1] *= v1;
	v0->_v[2] *= v1;
}
/*
//除法
*/
 CVec3d Vec_Vec3dDivDouble(CVec3d *v0, double v1)
{
	 if (0 == v0 || fabsf(v1) < 1e-10){
		 CVec3d temp = { 0.0, 0.0, 0.0 };
		 return temp;
	 }

	 v0->_v[0] /= v1;
	 v0->_v[1] /= v1;
	 v0->_v[2] /= v1;
	 return *v0;
}


 double Vec_DotProductVec3f(const CVec3f *vec, const CVec3f *vec1)
{
	return vec->_v[0]*vec1->_v[0] + vec->_v[1]*vec1->_v[1] + vec->_v[2]*vec1->_v[2];
}

 double Vec_CosValueVec3f(const CVec3f *vec, const CVec3f *vec1)
 {
	 double dotv = Vec_DotProductVec3f(vec,vec1);
	 double len1= Vec_LengthVec3f(vec),len2= Vec_LengthVec3f(vec1);

	 return dotv/(len1*len2) ;
 }

 double Vec_DotProductVec3d(const CVec3d *vec, const CVec3d *vec1)
{
	return vec->_v[0]*vec1->_v[0] + vec->_v[1]*vec1->_v[1] + vec->_v[2]*vec1->_v[2];
}


/*
求叉积  返回垂直向量
a × b = [a2b3 -a3b2, a3b1 - a1b3, a1b2- a2b1]
*/
 CVec3f Vec_CrossProductVec3f(const CVec3f *vec, const CVec3f *vec1)
{
	return Vec_CreatVec3f(
		vec->_v[1]*vec1->_v[2]-vec->_v[2]*vec1->_v[1],
		vec->_v[2]*vec1->_v[0]-vec->_v[0]*vec1->_v[2] ,
		vec->_v[0]*vec1->_v[1]-vec->_v[1]*vec1->_v[0]); 
}

 CVec3d Vec_CrossProductVec3d(const CVec3d *vec, const CVec3d *vec1)
{
	return Vec_CreatVec3d(
		vec->_v[1]*vec1->_v[2]-vec->_v[2]*vec1->_v[1],
		vec->_v[2]*vec1->_v[0]-vec->_v[0]*vec1->_v[2] ,
		vec->_v[0]*vec1->_v[1]-vec->_v[1]*vec1->_v[0]
	); 
}

 void Vec_PrintVec3d(const CVec3d *vec)
{
	printf("CVec3d: %lf,%lf,%lf\n", vec->_v[0], vec->_v[1], vec->_v[2]);
}
 void Vec_PrintVec3f(const CVec3f *vec)
{
	printf("CVec3f: %f,%f,%f\n", vec->_v[0], vec->_v[1], vec->_v[2]);
}

 int Vec_Vec3dCompareVec3d(const CVec3d *aVec1, const CVec3d *aVec2)
 {
	 if (aVec1->_v[0] <aVec2->_v[0])
	 {
		 return 1;
	 }
	 else if (aVec1->_v[0]  == aVec2->_v[0] )
	 {
		 if (aVec1->_v[1]  <aVec2->_v[1])
		 {
			 return 1;
		 }
		 else if (aVec1->_v[1] == aVec2->_v[1])
		 {
			 return (aVec1->_v[2] <aVec2->_v[2]);
		 }
		 else
		 {
			 return 0;
		 }
	 }
	 else
	 {
		 return 0;
	 }
 }

 pCVec3d Vec_CreatpCVec3d(CVec3d v)
 {
	 pCVec3d pVec3d = (pCVec3d)malloc(sizeof(CVec3d));
	 if (pVec3d)
	 {
		 pVec3d->_v[0] = v._v[0];
		 pVec3d->_v[1] = v._v[1];
		 pVec3d->_v[2] = v._v[2];
	 }
	 	
	 return pVec3d;
 }





 void Vec_CalcVec3dNormal(pCVec3d result, const pCVec3d point1, const pCVec3d point2, 	 const pCVec3d point3)
 {
	 CVec3d v1,v2;		  
	 v1._v[0] = point1->_v[0] - point2->_v[0];
	 v1._v[1] = point1->_v[1] - point2->_v[1];
	 v1._v[2] = point1->_v[2] - point2->_v[2];
	 v2._v[0] = point2->_v[0] - point3->_v[0];
	 v2._v[1] = point2->_v[1] - point3->_v[1];
	 v2._v[2] = point2->_v[2] - point3->_v[2];
	 *result = Vec_CrossProductVec3d(&v1, &v2);
	 Vec_NormalizeVec3d(result);
 }

 CVec3d Vec_CalcVec3dDirNormal(const pCVec3d v1, const pCVec3d v2)
 {
	 CVec3d result = Vec_CrossProductVec3d(v1,v2);
	 Vec_NormalizeVec3d(&result);
	 return result;
 }

 void Vec_CalcVec3fNormal(pCVec3f result, const pCVec3f point1, const pCVec3f point2, const pCVec3f point3)
 {
	 CVec3f v1,v2;		  
	 v1._v[0] = point1->_v[0] - point2->_v[0];
	 v1._v[1] = point1->_v[1] - point2->_v[1];
	 v1._v[2] = point1->_v[2] - point2->_v[2];
	 v2._v[0] = point2->_v[0] - point3->_v[0];
	 v2._v[1] = point2->_v[1] - point3->_v[1];
	 v2._v[2] = point2->_v[2] - point3->_v[2];
	 *result = Vec_CrossProductVec3f(&v1, &v2);
	 Vec_NormalizeVec3f(result);
 }