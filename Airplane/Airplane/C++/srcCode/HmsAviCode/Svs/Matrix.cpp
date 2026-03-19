#include "Matrix.h"
#include "MathCommon.h"


struct FlightParameter
{
    double mLon; //经度
    double mLat; //纬度
    double mAlt; //高度RNP 导航仿真数据库系统接口控制文件
    double mPsai; //航向角
    double mTheta; //俯仰角
    double mGama; //横滚角
    double mElevator; //升降舵
    double mRightAileron; //右副翼（度）
    double mLeftAileron;//左副翼
    double mRudder; //方向舵
    double mFlap; //襟翼
    double mLandgear; //起落架
    double mRNP; //RNP
    double mANP; //ANP
    double mSpoiler; //减速板
    double mIAS; //指示空速
    double mTime; //飞行时间
    double mFree[10]; //保留字节
};



void CMatrixd_BindFunc(pCMatrixd p)
{
    p->GetRotate = CMatrix_GetRotate;
    p->GetTrans = CMatrix_GetTrans;
    p->GetScale = CMatrix_GetScale;

};

void CMatrix_SETROW(CMatrixd* p, unsigned int row, double_type v0, double_type v1, double_type v2, double_type v3)
{
    p->_mat[row][0] = v0;
    p->_mat[row][1] = v1;
    p->_mat[row][2] = v2;
    p->_mat[row][3] = v3;
}

CMatrix CMatrix_Identity()
{
    CMatrixd n;
    n._mat[0][0] = (1);	n._mat[0][1] = (0);	n._mat[0][2] = (0);	n._mat[0][3] = (0);
    n._mat[1][0] = (0);	n._mat[1][1] = (1);	n._mat[1][2] = (0);	n._mat[1][3] = (0);
    n._mat[2][0] = (0);	n._mat[2][1] = (0);	n._mat[2][2] = (1);	n._mat[2][3] = (0);
    n._mat[3][0] = (0);	n._mat[3][1] = (0);	n._mat[3][2] = (0);	n._mat[3][3] = (1);

    CMatrixd_BindFunc(&n);

    return n;
}

CMatrix CMatrix_Zero()
{
    CMatrixd n;
    n._mat[0][0] = 0;	n._mat[0][1] = 0;	n._mat[0][2] = 0;	n._mat[0][3] = 0;
    n._mat[1][0] = 0;	n._mat[1][1] = 0;	n._mat[1][2] = 0;	n._mat[1][3] = 0;
    n._mat[2][0] = 0;	n._mat[2][1] = 0;	n._mat[2][2] = 0;	n._mat[2][3] = 0;
    n._mat[3][0] = 0;	n._mat[3][1] = 0;	n._mat[3][2] = 0;	n._mat[3][3] = 0;

    CMatrixd_BindFunc(&n);

    return n;
}

CMatrix CMatrix_Create(double_type a00, double_type a01, double_type a02, double_type a03,
    double_type a10, double_type a11, double_type a12, double_type a13,
    double_type a20, double_type a21, double_type a22, double_type a23,
    double_type a30, double_type a31, double_type a32, double_type a33)
{
    CMatrix p;
    CMatrix_SETROW(&p, 0, a00, a01, a02, a03);
    CMatrix_SETROW(&p, 1, a10, a11, a12, a13);
    CMatrix_SETROW(&p, 2, a20, a21, a22, a23);
    CMatrix_SETROW(&p, 3, a30, a31, a32, a33);
    CMatrixd_BindFunc(&p);
    return p;
}

void CMatrix_Set(CMatrix *p, double_type a00, double_type a01, double_type a02, double_type a03,
    double_type a10, double_type a11, double_type a12, double_type a13,
    double_type a20, double_type a21, double_type a22, double_type a23,
    double_type a30, double_type a31, double_type a32, double_type a33)
{
    CMatrix_SETROW(p, 0, a00, a01, a02, a03);
    CMatrix_SETROW(p, 1, a10, a11, a12, a13);
    CMatrix_SETROW(p, 2, a20, a21, a22, a23);
    CMatrix_SETROW(p, 3, a30, a31, a32, a33);
}

void CMatrix_SetTransXYZ(CMatrix*p, double_type tx, double_type ty, double_type tz)
{
    p->_mat[3][0] = tx;
    p->_mat[3][1] = ty;
    p->_mat[3][2] = tz;
}
void CMatrix_SetTransVec3f(CMatrix*p, const CVec3f v)
{
    p->_mat[3][0] = v._v[0];
    p->_mat[3][1] = v._v[1];
    p->_mat[3][2] = v._v[2];
}
void CMatrix_SetTransVec3d(CMatrix*p, const CVec3d v)
{
    p->_mat[3][0] = v._v[0];
    p->_mat[3][1] = v._v[1];
    p->_mat[3][2] = v._v[2];
}

void CMatrix_PreMultTranslateVec3f(CMatrix* p, const CVec3f v)
{
    float tmpvec[3], tmp;
    unsigned int i;
    tmpvec[0] = v._v[0]; tmpvec[1] = v._v[1]; tmpvec[2] = v._v[2];
    for (i = 0; i < 3; ++i)
    {
        tmp = tmpvec[i];
        if (tmp == 0)
            continue;
        p->_mat[3][0] += tmp*p->_mat[i][0];
        p->_mat[3][1] += tmp*p->_mat[i][1];
        p->_mat[3][2] += tmp*p->_mat[i][2];
        p->_mat[3][3] += tmp*p->_mat[i][3];
    }
}

void CMatrix_PreMultTranslateVec3d(CMatrix* p, const CVec3d v)
{
    double tmpvec[3], tmp;
    unsigned int i;
    tmpvec[0] = v._v[0]; tmpvec[1] = v._v[1]; tmpvec[2] = v._v[2];
    for (i = 0; i < 3; ++i)
    {
        tmp = tmpvec[i];
        if (tmp == 0)
            continue;
        p->_mat[3][0] += tmp*p->_mat[i][0];
        p->_mat[3][1] += tmp*p->_mat[i][1];
        p->_mat[3][2] += tmp*p->_mat[i][2];
        p->_mat[3][3] += tmp*p->_mat[i][3];
    }
}


CMatrixd CMatrix_LookAtVec3f(CVec3f eye, CVec3f center, CVec3f up)
{
    CMatrixd m = CMatrix_Identity();
    CMatrix_MakeLookAtVec3f(&m, eye, center, up);
    return m;
}
CMatrixd CMatrix_LookAtVec3d(CVec3d* eye, CVec3d* center, CVec3d *up)
{
    CMatrixd m = CMatrix_Identity();
    CMatrix_MakeLookAtVec3d(&m, *eye, *center, *up);
    return m;
}

void CMatrix_MakeLookAtVec3d(CMatrix* p, const CVec3d eye, const CVec3d center, const CVec3d up)
{
    CVec3d f;
    CVec3d s;
    CVec3d u;
    CVec3d posEye;

    f = Vec_Vec3dSubVec3d(&center, &eye);
    Vec_NormalizeVec3d(&f);
    s = Vec_CrossProductVec3d(&f, &up);
    Vec_NormalizeVec3d(&s);

    u = Vec_CrossProductVec3d(&s, &f);

    Vec_NormalizeVec3d(&u);


    CMatrix_Set(p,
        s._v[0], u._v[0], -f._v[0], 0.0,
        s._v[1], u._v[1], -f._v[1], 0.0,
        s._v[2], u._v[2], -f._v[2], 0.0,
        0.0, 0.0, 0.0, 1.0);

    posEye = Vec_CreatVec3d(-eye._v[0], -eye._v[1], -eye._v[2]);
    CMatrix_PreMultTranslateVec3d(p, posEye);
}

CMatrixd CMatrix_LookAtXYZ(const double eye_x, const double eye_y, const double eye_z,
    const double center_x, const double center_y, const double center_z,
    const double up_x, const double up_y, const double up_z)
{
    CMatrixd m = CMatrix_Identity();
    CMatrix_MakeLookAtXYZ(&m,
        eye_x, eye_y, eye_z,
        center_x, center_y, center_z,
        up_x, up_y, up_z);
    return m;
}
void CMatrix_MakeLookAtXYZ(CMatrix* p, const double eye_x, const double eye_y, const double eye_z,
    const double center_x, const double center_y, const double center_z,
    const double up_x, const double up_y, const double up_z)
{
    CVec3d f;
    CVec3d s;
    CVec3d u;
    CVec3d posEye;

    f._v[0] = center_x - eye_x;
    f._v[1] = center_y - eye_y;
    f._v[2] = center_z - eye_z;
    Vec_NormalizeVec3d(&f);
    s = Vec_CreatVec3d(
        f._v[1] * up_z - f._v[2] * up_y,
        f._v[2] * up_x - f._v[0] * up_z,
        f._v[0] * up_y - f._v[1] * up_x);

    Vec_NormalizeVec3d(&s);

    u = Vec_CrossProductVec3d(&s, &f);

    Vec_NormalizeVec3d(&u);


    CMatrix_Set(p,
        s._v[0], u._v[0], -f._v[0], 0.0,
        s._v[1], u._v[1], -f._v[1], 0.0,
        s._v[2], u._v[2], -f._v[2], 0.0,
        0.0, 0.0, 0.0, 1.0);

    posEye = Vec_CreatVec3d(-eye_x, -eye_y, -eye_z);
    CMatrix_PreMultTranslateVec3d(p, posEye);
}

void CMatrix_MakeLookAtVec3f(CMatrix* p, const CVec3f eye, const CVec3f center, const CVec3f up)
{
    CVec3f f;
    CVec3f s;
    CVec3f u;
    CVec3f posEye;

    f = Vec_Vec3fSubVec3f(&center, &eye);
    Vec_NormalizeVec3f(&f);
    s = Vec_CrossProductVec3f(&f, &up);
    Vec_NormalizeVec3f(&s);

    u = Vec_CrossProductVec3f(&s, &f);

    Vec_NormalizeVec3f(&u);


    CMatrix_Set(p,
        s._v[0], u._v[0], -f._v[0], 0.0,
        s._v[1], u._v[1], -f._v[1], 0.0,
        s._v[2], u._v[2], -f._v[2], 0.0,
        0.0, 0.0, 0.0, 1.0);

    posEye = Vec_CreatVec3f(-eye._v[0], -eye._v[1], -eye._v[2]);
    CMatrix_PreMultTranslateVec3f(p, posEye);
}



double CMatrix_INNER_PRODUCT(const CMatrix *a, const CMatrix *b, unsigned int r, unsigned int c)
{
    return
        ((a)->_mat[r][0] * (b)->_mat[0][c]) + ((a)->_mat[r][1] * (b)->_mat[1][c]) + ((a)->_mat[r][2] * (b)->_mat[2][c]) + ((a)->_mat[r][3] * (b)->_mat[3][c]);
}


void CMatrix_PreMultMatrix(CMatrix *p, const CMatrixd *other)
{
    double_type t[4];
    int col = 0;
    for (col = 0; col < 4; ++col)
    {
        t[0] = CMatrix_INNER_PRODUCT(other, p, 0, col);
        t[1] = CMatrix_INNER_PRODUCT(other, p, 1, col);
        t[2] = CMatrix_INNER_PRODUCT(other, p, 2, col);
        t[3] = CMatrix_INNER_PRODUCT(other, p, 3, col);
        p->_mat[0][col] = t[0];
        p->_mat[1][col] = t[1];
        p->_mat[2][col] = t[2];
        p->_mat[3][col] = t[3];
    }

}

void CMatrix_PostMultMatrix(CMatrix *p, const CMatrixd *other)
{
    double_type t[4];
    int row = 0;
    for (row = 0; row < 4; ++row)
    {
        t[0] = CMatrix_INNER_PRODUCT(p, other, row, 0);
        t[1] = CMatrix_INNER_PRODUCT(p, other, row, 1);
        t[2] = CMatrix_INNER_PRODUCT(p, other, row, 2);
        t[3] = CMatrix_INNER_PRODUCT(p, other, row, 3);
        CMatrix_SETROW(p, row, t[0], t[1], t[2], t[3]);
    }
}


CVec3f CMatrix_PreMultVec3f(CMatrix *p, const CVec3f* v)
{
    double_type d = 1.0f / (p->_mat[0][3] * v->_v[0] + p->_mat[1][3] * v->_v[1] + p->_mat[2][3] * v->_v[2] + p->_mat[3][3]);
    return  Vec_CreatVec3f(
        (p->_mat[0][0] * v->_v[0] + p->_mat[1][0] * v->_v[1] + p->_mat[2][0] * v->_v[2] + p->_mat[3][0])*d,
        (p->_mat[0][1] * v->_v[0] + p->_mat[1][1] * v->_v[1] + p->_mat[2][1] * v->_v[2] + p->_mat[3][1])*d,
        (p->_mat[0][2] * v->_v[0] + p->_mat[1][2] * v->_v[1] + p->_mat[2][2] * v->_v[2] + p->_mat[3][2])*d);

};
CVec3d CMatrix_PreMultVec3d(CMatrix *p, const CVec3d* v)
{
    double_type d = 1.0f / (p->_mat[0][3] * v->_v[0] + p->_mat[1][3] * v->_v[1] + p->_mat[2][3] * v->_v[2] + p->_mat[3][3]);
    double x = (p->_mat[0][0] * v->_v[0] + p->_mat[1][0] * v->_v[1] + p->_mat[2][0] * v->_v[2] + p->_mat[3][0])*d;
    double y = (p->_mat[0][1] * v->_v[0] + p->_mat[1][1] * v->_v[1] + p->_mat[2][1] * v->_v[2] + p->_mat[3][1])*d;
    double z = (p->_mat[0][2] * v->_v[0] + p->_mat[1][2] * v->_v[1] + p->_mat[2][2] * v->_v[2] + p->_mat[3][2])*d;

    return  Vec_CreatVec3d(x, y, z);
};
CVec3f CMatrix_PostMultVec3f(CMatrix *p, const CVec3f* v)
{
    double_type d = 1.0f / (p->_mat[3][0] * v->_v[0] + p->_mat[3][1] * v->_v[1] + p->_mat[3][2] * v->_v[2] + p->_mat[3][3]);
    return Vec_CreatVec3f(
        (p->_mat[0][0] * v->_v[0] + p->_mat[0][1] * v->_v[1] + p->_mat[0][2] * v->_v[2] + p->_mat[0][3])*d,
        (p->_mat[1][0] * v->_v[0] + p->_mat[1][1] * v->_v[1] + p->_mat[1][2] * v->_v[2] + p->_mat[1][3])*d,
        (p->_mat[2][0] * v->_v[0] + p->_mat[2][1] * v->_v[1] + p->_mat[2][2] * v->_v[2] + p->_mat[2][3])*d);
};
CVec3d CMatrix_PostMultVec3d(CMatrix *p, const CVec3d* v)
{
    double_type d = 1.0f / (p->_mat[3][0] * v->_v[0] + p->_mat[3][1] * v->_v[1] + p->_mat[3][2] * v->_v[2] + p->_mat[3][3]);
    return Vec_CreatVec3d(
        (p->_mat[0][0] * v->_v[0] + p->_mat[0][1] * v->_v[1] + p->_mat[0][2] * v->_v[2] + p->_mat[0][3])*d,
        (p->_mat[1][0] * v->_v[0] + p->_mat[1][1] * v->_v[1] + p->_mat[1][2] * v->_v[2] + p->_mat[1][3])*d,
        (p->_mat[2][0] * v->_v[0] + p->_mat[2][1] * v->_v[1] + p->_mat[2][2] * v->_v[2] + p->_mat[2][3])*d);
};

CVec4f CMatrix_preMultVec4f(CMatrix *p, const CVec4f* v)
{
    float x = (p->_mat[0][0] * v->_v[0] + p->_mat[1][0] * v->_v[1] + p->_mat[2][0] * v->_v[2] + p->_mat[3][0] * v->_v[3]);
    float y = (p->_mat[0][1] * v->_v[0] + p->_mat[1][1] * v->_v[1] + p->_mat[2][1] * v->_v[2] + p->_mat[3][1] * v->_v[3]);
    float z = (p->_mat[0][2] * v->_v[0] + p->_mat[1][2] * v->_v[1] + p->_mat[2][2] * v->_v[2] + p->_mat[3][2] * v->_v[3]);
    float w = (p->_mat[0][3] * v->_v[0] + p->_mat[1][3] * v->_v[1] + p->_mat[2][3] * v->_v[2] + p->_mat[3][3] * v->_v[3]);

    return Vec_CreatVec4f(x, y, z, w);
};
CVec4d CMatrix_preMultVec4d(CMatrix *p, const CVec4d* v)
{
    double x = (p->_mat[0][0] * v->_v[0] + p->_mat[1][0] * v->_v[1] + p->_mat[2][0] * v->_v[2] + p->_mat[3][0] * v->_v[3]);
    double y = (p->_mat[0][1] * v->_v[0] + p->_mat[1][1] * v->_v[1] + p->_mat[2][1] * v->_v[2] + p->_mat[3][1] * v->_v[3]);
    double z = (p->_mat[0][2] * v->_v[0] + p->_mat[1][2] * v->_v[1] + p->_mat[2][2] * v->_v[2] + p->_mat[3][2] * v->_v[3]);
    double w = (p->_mat[0][3] * v->_v[0] + p->_mat[1][3] * v->_v[1] + p->_mat[2][3] * v->_v[2] + p->_mat[3][3] * v->_v[3]);

    return Vec_CreatVec4d(x, y, z, w);
};
CVec4f CMatrix_postMultVec4f(CMatrix *p, const CVec4f* v)
{
    float x = (p->_mat[0][0] * v->_v[0] + p->_mat[0][1] * v->_v[1] + p->_mat[0][2] * v->_v[2] + p->_mat[0][3] * v->_v[3]);
    float y = (p->_mat[1][0] * v->_v[0] + p->_mat[1][1] * v->_v[1] + p->_mat[1][2] * v->_v[2] + p->_mat[1][3] * v->_v[3]);
    float w = (p->_mat[2][0] * v->_v[0] + p->_mat[2][1] * v->_v[1] + p->_mat[2][2] * v->_v[2] + p->_mat[2][3] * v->_v[3]);
    float z = (p->_mat[3][0] * v->_v[0] + p->_mat[3][1] * v->_v[1] + p->_mat[3][2] * v->_v[2] + p->_mat[3][3] * v->_v[3]);

    return Vec_CreatVec4f(x, y, z, w);
};
CVec4d CMatrix_postMultVec4d(CMatrix *p, const CVec4d* v)
{
    double x = (p->_mat[0][0] * v->_v[0] + p->_mat[0][1] * v->_v[1] + p->_mat[0][2] * v->_v[2] + p->_mat[0][3] * v->_v[3]);
    double y = (p->_mat[1][0] * v->_v[0] + p->_mat[1][1] * v->_v[1] + p->_mat[1][2] * v->_v[2] + p->_mat[1][3] * v->_v[3]);
    double z = (p->_mat[2][0] * v->_v[0] + p->_mat[2][1] * v->_v[1] + p->_mat[2][2] * v->_v[2] + p->_mat[2][3] * v->_v[3]);
    double w = (p->_mat[3][0] * v->_v[0] + p->_mat[3][1] * v->_v[1] + p->_mat[3][2] * v->_v[2] + p->_mat[3][3] * v->_v[3]);

    return Vec_CreatVec4d(x, y, z, w);
};


void CMatrix_mult(CMatrix *p, const CMatrix* lhs, const CMatrix* rhs)
{
    if (lhs == p)
    {
        CMatrix_PostMultMatrix(p, rhs);
        return;
    }
    if (rhs == p)
    {
        CMatrix_PreMultMatrix(p, lhs);
        return;
    }

    p->_mat[0][0] = CMatrix_INNER_PRODUCT(lhs, rhs, 0, 0);
    p->_mat[0][1] = CMatrix_INNER_PRODUCT(lhs, rhs, 0, 1);
    p->_mat[0][2] = CMatrix_INNER_PRODUCT(lhs, rhs, 0, 2);
    p->_mat[0][3] = CMatrix_INNER_PRODUCT(lhs, rhs, 0, 3);
    p->_mat[1][0] = CMatrix_INNER_PRODUCT(lhs, rhs, 1, 0);
    p->_mat[1][1] = CMatrix_INNER_PRODUCT(lhs, rhs, 1, 1);
    p->_mat[1][2] = CMatrix_INNER_PRODUCT(lhs, rhs, 1, 2);
    p->_mat[1][3] = CMatrix_INNER_PRODUCT(lhs, rhs, 1, 3);
    p->_mat[2][0] = CMatrix_INNER_PRODUCT(lhs, rhs, 2, 0);
    p->_mat[2][1] = CMatrix_INNER_PRODUCT(lhs, rhs, 2, 1);
    p->_mat[2][2] = CMatrix_INNER_PRODUCT(lhs, rhs, 2, 2);
    p->_mat[2][3] = CMatrix_INNER_PRODUCT(lhs, rhs, 2, 3);
    p->_mat[3][0] = CMatrix_INNER_PRODUCT(lhs, rhs, 3, 0);
    p->_mat[3][1] = CMatrix_INNER_PRODUCT(lhs, rhs, 3, 1);
    p->_mat[3][2] = CMatrix_INNER_PRODUCT(lhs, rhs, 3, 2);
    p->_mat[3][3] = CMatrix_INNER_PRODUCT(lhs, rhs, 3, 3);
}



int CMatrix_invert_4x3(CMatrix* p, const CMatrixd* mat)
{
    CMatrixd TPinv = CMatrix_Identity();
    double_type r00, r01, r02,
        r10, r11, r12,
        r20, r21, r22;
    double_type one_over_det = 1.0;

    CMatrixd tm = CMatrix_Identity();

    if (mat == p)
    {
        tm = (*mat);
        return CMatrix_invert_4x3(p, &tm);
    }



    r00 = mat->_mat[0][0]; r01 = mat->_mat[0][1]; r02 = mat->_mat[0][2];
    r10 = mat->_mat[1][0]; r11 = mat->_mat[1][1]; r12 = mat->_mat[1][2];
    r20 = mat->_mat[2][0]; r21 = mat->_mat[2][1]; r22 = mat->_mat[2][2];


    p->_mat[0][0] = r11*r22 - r12*r21;
    p->_mat[0][1] = r02*r21 - r01*r22;
    p->_mat[0][2] = r01*r12 - r02*r11;


    one_over_det = 1.0 / (r00*p->_mat[0][0] + r10*p->_mat[0][1] + r20*p->_mat[0][2]);
    r00 *= one_over_det; r10 *= one_over_det; r20 *= one_over_det;



    p->_mat[0][0] *= one_over_det;
    p->_mat[0][1] *= one_over_det;
    p->_mat[0][2] *= one_over_det;
    p->_mat[0][3] = 0.0;
    p->_mat[1][0] = r12*r20 - r10*r22;
    p->_mat[1][1] = r00*r22 - r02*r20;

    p->_mat[1][2] = r02*r10 - r00*r12;

    p->_mat[1][3] = 0.0;
    p->_mat[2][0] = r10*r21 - r11*r20;

    p->_mat[2][1] = r01*r20 - r00*r21;
    p->_mat[2][2] = r00*r11 - r01*r10;
    p->_mat[2][3] = 0.0;
    p->_mat[3][3] = 1.0;


#define d r22
    d = mat->_mat[3][3];

    if ((d - 1.0)*(d - 1.0) > 1.0e-6)
    {


        p->_mat[3][0] = p->_mat[3][1] = p->_mat[3][2] = 0.0;

#define px r00
#define py r01
#define pz r02
#define one_over_s  one_over_det
#define a  r10
#define b  r11
#define c  r12

        a = mat->_mat[0][3]; b = mat->_mat[1][3]; c = mat->_mat[2][3];
        px = p->_mat[0][0] * a + p->_mat[0][1] * b + p->_mat[0][2] * c;
        py = p->_mat[1][0] * a + p->_mat[1][1] * b + p->_mat[1][2] * c;
        pz = p->_mat[2][0] * a + p->_mat[2][1] * b + p->_mat[2][2] * c;

#undef a
#undef b
#undef c
#define tx r10
#define ty r11
#define tz r12

        tx = mat->_mat[3][0]; ty = mat->_mat[3][1]; tz = mat->_mat[3][2];
        one_over_s = 1.0 / (d - (tx*px + ty*py + tz*pz));

        tx *= one_over_s; ty *= one_over_s; tz *= one_over_s;

        TPinv._mat[0][0] = tx*px + 1.0;
        TPinv._mat[0][1] = ty*px;
        TPinv._mat[0][2] = tz*px;
        TPinv._mat[0][3] = -px * one_over_s;
        TPinv._mat[1][0] = tx*py;
        TPinv._mat[1][1] = ty*py + 1.0;
        TPinv._mat[1][2] = tz*py;
        TPinv._mat[1][3] = -py * one_over_s;
        TPinv._mat[2][0] = tx*pz;
        TPinv._mat[2][1] = ty*pz;
        TPinv._mat[2][2] = tz*pz + 1.0;
        TPinv._mat[2][3] = -pz * one_over_s;
        TPinv._mat[3][0] = -tx;
        TPinv._mat[3][1] = -ty;
        TPinv._mat[3][2] = -tz;
        TPinv._mat[3][3] = one_over_s;

        CMatrix_PreMultMatrix(p, &TPinv);

#undef px
#undef py
#undef pz
#undef one_over_s
#undef d
    }
    else
    {
        tx = mat->_mat[3][0]; ty = mat->_mat[3][1]; tz = mat->_mat[3][2];

        p->_mat[3][0] = -(tx*p->_mat[0][0] + ty*p->_mat[1][0] + tz*p->_mat[2][0]);
        p->_mat[3][1] = -(tx*p->_mat[0][1] + ty*p->_mat[1][1] + tz*p->_mat[2][1]);
        p->_mat[3][2] = -(tx*p->_mat[0][2] + ty*p->_mat[1][2] + tz*p->_mat[2][2]);

#undef tx
#undef ty
#undef tz
    }

    return 1;
}



#ifndef SGL_ABS
#define SGL_ABS(A)  ((A >= 0) ? (A) : (-A))
#endif

#ifndef SGL_SWAP
#define SGL_SWAP(a,b,temp) ((temp)=(a),(a)=(b),(b)=(temp))
#endif

int CMatrix_invert_4x4(CMatrix* p, const CMatrixd* mat)
{
    unsigned int indxc[4], indxr[4], ipiv[4];
    unsigned int i, j, k, l, ll, lx;
    unsigned int icol = 0;
    unsigned int irow = 0;
    double temp, pivinv, dum, big;

    CMatrixd tm = CMatrix_Identity();
    if (mat == p) {
        tm = (*mat);
        return CMatrix_invert_4x4(p, &tm);
    }



    *p = *mat;

    for (j = 0; j < 4; j++) ipiv[j] = 0;

    for (i = 0; i < 4; i++)
    {
        big = 0.0;
        for (j = 0; j < 4; j++)
        if (ipiv[j] != 1)
        for (k = 0; k < 4; k++)
        {
            if (ipiv[k] == 0)
            {
                if (SGL_ABS(p->_mat[j][k]) >= big)
                {
                    big = SGL_ABS(p->_mat[j][k]);
                    irow = j;
                    icol = k;
                }
            }
            else if (ipiv[k] > 1)
                return 0;
        }
        ++(ipiv[icol]);
        if (irow != icol)
        for (l = 0; l < 4; l++) SGL_SWAP(p->_mat[irow][l],
            p->_mat[icol][l],
            temp);

        indxr[i] = irow;
        indxc[i] = icol;
        if (p->_mat[icol, icol] == 0)
            return 0;

        pivinv = 1.0 / p->_mat[icol][icol];
        p->_mat[icol][icol] = 1;
        for (l = 0; l < 4; l++) p->_mat[icol][l] *= pivinv;
        for (ll = 0; ll < 4; ll++)
        if (ll != icol)
        {
            dum = p->_mat[ll][icol];
            p->_mat[ll][icol] = 0;
            for (l = 0; l<4; l++) p->_mat[ll][l] -= p->_mat[icol][l] * dum;
        }
    }
    for (lx = 4; lx>0; --lx)
    {
        if (indxr[lx - 1] != indxc[lx - 1])
        for (k = 0; k < 4; k++) SGL_SWAP(p->_mat[k][indxr[lx - 1]],
            p->_mat[k][indxc[lx - 1]], temp);
    }
    return 1;
}

int CMatrix_invert(CMatrix* p, const CMatrix* rhs)
{
    int is_4x3 = (rhs->_mat[0][3] == 0.0 && rhs->_mat[1][3] == 0.0 &&  rhs->_mat[2][3] == 0.0 && rhs->_mat[3][3] == 1.0);
    return is_4x3 ? CMatrix_invert_4x3(p, rhs) : CMatrix_invert_4x4(p, rhs);
}
CMatrixd CMatrix_Inverse(const CMatrixd* matrix)
{
    CMatrixd m;
    CMatrixd_BindFunc(&m);
    CMatrix_invert(&m, matrix);
    return m;
}

CVec3f CMatrix_Transform3x3Vec3fMatrix(const CVec3f* v, const CMatrixd* m)
{
    float x = (m->_mat[0][0] * v->_v[0] + m->_mat[1][0] * v->_v[1] + m->_mat[2][0] * v->_v[2]);
    float y = (m->_mat[0][1] * v->_v[0] + m->_mat[1][1] * v->_v[1] + m->_mat[2][1] * v->_v[2]);
    float z = (m->_mat[0][2] * v->_v[0] + m->_mat[1][2] * v->_v[1] + m->_mat[2][2] * v->_v[2]);
    return Vec_CreatVec3f(x, y, z);
}
CVec3d CMatrix_Transform3x3Vec3dMatrix(const CVec3d* v, const CMatrixd* m)
{
    double x = (m->_mat[0][0] * v->_v[0] + m->_mat[1][0] * v->_v[1] + m->_mat[2][0] * v->_v[2]);
    double y = (m->_mat[0][1] * v->_v[0] + m->_mat[1][1] * v->_v[1] + m->_mat[2][1] * v->_v[2]);
    double z = (m->_mat[0][2] * v->_v[0] + m->_mat[1][2] * v->_v[1] + m->_mat[2][2] * v->_v[2]);
    return Vec_CreatVec3d(x, y, z);
}

CVec3f CMatrix_Transform3x3MatrixVec3f(const CMatrixd* m, const CVec3f* v)
{
    float x = (m->_mat[0][0] * v->_v[0] + m->_mat[0][1] * v->_v[1] + m->_mat[0][2] * v->_v[2]);
    float y = (m->_mat[1][0] * v->_v[0] + m->_mat[1][1] * v->_v[1] + m->_mat[1][2] * v->_v[2]);
    float z = (m->_mat[2][0] * v->_v[0] + m->_mat[2][1] * v->_v[1] + m->_mat[2][2] * v->_v[2]);
    return Vec_CreatVec3f(x, y, z);
}
CVec3d CMatrix_Transform3x3MatrixVec3d(const CMatrixd* m, const CVec3d* v)
{
    double x = (m->_mat[0][0] * v->_v[0] + m->_mat[0][1] * v->_v[1] + m->_mat[0][2] * v->_v[2]);
    double y = (m->_mat[1][0] * v->_v[0] + m->_mat[1][1] * v->_v[1] + m->_mat[1][2] * v->_v[2]);
    double z = (m->_mat[2][0] * v->_v[0] + m->_mat[2][1] * v->_v[1] + m->_mat[2][2] * v->_v[2]);
    return Vec_CreatVec3d(x, y, z);
}


void CMatrix_GetLookAtVec3f(CMatrix* p, CVec3f* eye, CVec3f* center, CVec3f* up, double_type lookDistance)
{
    CMatrixd inv = CMatrix_Identity();
    CVec3f  orgin = { .0, 0.0, 0.0 };
    CVec3f e;
    CVec3f ey1;
    CVec3f ez1;
    CVec3f c;

    CMatrix_invert(&inv, p);

    e = CMatrix_PreMultVec3f(&inv, &orgin);
    ey1 = Vec_CreatVec3f(0.0, 1.0, 0.0);
    ez1 = Vec_CreatVec3f(0.0, 0.0, -1);
    c = CMatrix_Transform3x3MatrixVec3f(p, &ez1);

    *up = CMatrix_Transform3x3MatrixVec3f(p, &ey1);

    Vec_NormalizeVec3f(&c);



    c._v[0] = e._v[0] + c._v[0] * lookDistance;
    c._v[1] = e._v[1] + c._v[1] * lookDistance;
    c._v[2] = e._v[2] + c._v[2] * lookDistance;


    *eye = e;
    *center = c;
}

void CMatrix_GetLookAtVec3d(CMatrix* p, CVec3d* eye, CVec3d* center, CVec3d* up, double_type lookDistance)
{
    CMatrixd inv = CMatrix_Identity();
    CVec3d  orgin = { .0, 0.0, 0.0 };
    CVec3d e;
    CVec3d ey1;
    CVec3d ez1;
    CVec3d c;


    CMatrix_invert(&inv, p);

    e = CMatrix_PreMultVec3d(&inv, &orgin);
    ey1 = Vec_CreatVec3d(0.0, 1.0, 0.0);
    ez1 = Vec_CreatVec3d(0.0, 0.0, -1);
    c = CMatrix_Transform3x3MatrixVec3d(p, &ez1);

    *up = CMatrix_Transform3x3MatrixVec3d(p, &ey1);
    Vec_NormalizeVec3d(&c);



    c._v[0] = e._v[0] + c._v[0] * lookDistance;
    c._v[1] = e._v[1] + c._v[1] * lookDistance;
    c._v[2] = e._v[2] + c._v[2] * lookDistance;


    *eye = e;
    *center = c;

    /*

    Matrix_implementation inv;
    inv.invert(*this);

    // note: e and c variables must be used inside this method instead of eye and center
    // because eye and center are references and they may point to the same variable.
    Vec3f e = hms::Vec3f(0.0,0.0,0.0)*inv;
    up = transform3x3(*this,hms::Vec3f(0.0,1.0,0.0));
    Vec3f c = transform3x3(*this,hms::Vec3f(0.0,0.0,-1));
    c.normalize();
    c = e + c*lookDistance;

    // assign the results
    eye = e;
    center = c;
    */
}





/*modified by yuanli */
int CMatrix_IsIdentity(const CMatrixd* matrix)
{
    return matrix->_mat[0][0] == 1.0 && matrix->_mat[0][1] == 0.0 && matrix->_mat[0][2] == 0.0 &&  matrix->_mat[0][3] == 0.0 &&
        matrix->_mat[1][0] == 0.0 && matrix->_mat[1][1] == 1.0 && matrix->_mat[1][2] == 0.0 &&  matrix->_mat[1][3] == 0.0 &&
        matrix->_mat[2][0] == 0.0 && matrix->_mat[2][1] == 0.0 && matrix->_mat[2][2] == 1.0 &&  matrix->_mat[2][3] == 0.0 &&
        matrix->_mat[3][0] == 0.0 && matrix->_mat[3][1] == 0.0 && matrix->_mat[3][2] == 0.0 &&  matrix->_mat[3][3] == 1.0;
}

void CMatrix_MakeIdentity(CMatrixd* matrix)
{
    CMatrix_SetMakeScaleXYZ(matrix, 1, 1, 1);
}

void CMatrix_SetMakeScaleVec3f(CMatrixd* matrix, const CVec3f* v)
{
    CMatrix_SetMakeScaleXYZ(matrix, v->_v[0], v->_v[1], v->_v[2]);
}

void CMatrix_SetMakeScaleVec3d(CMatrixd* matrix, const CVec3d* v)
{
    CMatrix_SetMakeScaleXYZ(matrix, v->_v[0], v->_v[1], v->_v[2]);
}

void CMatrix_SetMakeScaleXYZ(CMatrixd* matrix, double_type x, double_type y, double_type z)
{
    CMatrix_SETROW(matrix, 0, x, 0, 0, 0);
    CMatrix_SETROW(matrix, 1, 0, y, 0, 0);
    CMatrix_SETROW(matrix, 2, 0, 0, z, 0);
    CMatrix_SETROW(matrix, 3, 0, 0, 0, 1);
    CMatrixd_BindFunc(matrix);
}

void CMatrix_SetMakeTranslateVec3f(CMatrixd* matrix, const CVec3f* v)
{
    CMatrix_SetMakeTranslateXYZ(matrix, v->_v[0], v->_v[1], v->_v[2]);
}

void CMatrix_SetMakeTranslateVec3d(CMatrixd* matrix, const CVec3d* v)
{
    CMatrix_SetMakeTranslateXYZ(matrix, v->_v[0], v->_v[1], v->_v[2]);
}

void CMatrix_SetMakeTranslateXYZ(CMatrixd* matrix, double_type x, double_type y, double_type z)
{
    CMatrix_SETROW(matrix, 0, 1, 0, 0, 0);
    CMatrix_SETROW(matrix, 1, 0, 1, 0, 0);
    CMatrix_SETROW(matrix, 2, 0, 0, 1, 0);
    CMatrix_SETROW(matrix, 3, x, y, z, 1);
    CMatrixd_BindFunc(matrix);
}

CMatrixd CMatrix_ScaleVec3f(const CVec3f* v)
{
    CMatrixd m;
    CMatrix_SetMakeScaleVec3f(&m, v);
    return m;
}

CMatrixd CMatrix_ScaleVec3d(const CVec3d* v)
{
    CMatrixd m;
    CMatrix_SetMakeScaleVec3d(&m, v);
    return m;
}

CMatrixd CMatrix_ScaleXYZ(double_type sx, double_type sy, double_type sz)
{
    CMatrixd m;
    CMatrix_SetMakeScaleXYZ(&m, sx, sy, sz);
    return m;
}

CMatrixd CMatrix_TranslateVec3f(const CVec3f v)
{
    CMatrixd m;
    CMatrix_SetMakeTranslateVec3f(&m, &v);
    return m;
}

CMatrixd CMatrix_TranslateVec3d(const CVec3d v)
{
    CMatrixd m;
    CMatrix_SetMakeTranslateVec3d(&m, &v);
    return m;
}

CMatrixd CMatrix_TranslateXYZ(double_type tx, double_type ty, double_type tz)
{
    CMatrixd m;
    CMatrix_SetMakeTranslateXYZ(&m, tx, ty, tz);
    return m;
}

//************************************
// Method:    CMatrix_d2f
// FullName:  CMatrix_d2f
// Access:    public 
// Desc:      double to float
// Returns:   CMatrixf
// Qualifier:
// Parameter: CMatrixd md
//************************************
CMatrixf CMatrix_d2f(CMatrixd md)
{
    CMatrixf mf;
    int i, j;
    for (i = 0; i < 4; ++i)
    {
        for (j = 0; j < 4; ++j)
        {
            mf._mat[i][j] = md._mat[i][j];
        }
    }
    return mf;
}

void CMatrix_SetRotate(CMatrixd* m, const pCQuat q)
{
    double length2;
    double rlength2;
    double wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

    length2 = q->_v[0] * q->_v[0] + q->_v[1] * q->_v[1] + q->_v[2] * q->_v[2] + q->_v[3] * q->_v[3];
    if (fabs(length2) <= DBL_MIN)
    {
        m->_mat[0][0] = 0.0; m->_mat[1][0] = 0.0; m->_mat[2][0] = 0.0;
        m->_mat[0][1] = 0.0; m->_mat[1][1] = 0.0; m->_mat[2][1] = 0.0;
        m->_mat[0][2] = 0.0; m->_mat[1][2] = 0.0; m->_mat[2][2] = 0.0;
    }
    else
    {
        /*
         normalize quat if required.
         We can avoid the expensive sqrt in this case since all 'coefficients' below are products of two q components.
         That is a square of a square root, so it is possible to avoid that
         */
        if (length2 != 1.0)
        {
            rlength2 = 2.0 / length2;
        }
        else
        {
            rlength2 = 2.0;
        }


        /* calculate coefficients*/
        x2 = rlength2*q->_v[0];
        y2 = rlength2*q->_v[1];
        z2 = rlength2*q->_v[2];

        xx = q->_v[0] * x2;
        xy = q->_v[0] * y2;
        xz = q->_v[0] * z2;

        yy = q->_v[1] * y2;
        yz = q->_v[1] * z2;
        zz = q->_v[2] * z2;

        wx = q->_v[3] * x2;
        wy = q->_v[3] * y2;
        wz = q->_v[3] * z2;

        /*
         Note.  Gamasutra gets the matrix assignments inverted, resulting
         in left-handed rotations, which is contrary to OpenGL and OSG's
         methodology.  The matrix assignment has been altered in the next
         few lines of code to do the right thing.
         */

        m->_mat[0][0] = 1.0 - (yy + zz);
        m->_mat[1][0] = xy - wz;
        m->_mat[2][0] = xz + wy;


        m->_mat[0][1] = xy + wz;
        m->_mat[1][1] = 1.0 - (xx + zz);
        m->_mat[2][1] = yz - wx;

        m->_mat[0][2] = xz - wy;
        m->_mat[1][2] = yz + wx;
        m->_mat[2][2] = 1.0 - (xx + yy);
    }
}

void CMatrix_MakeRotateVec3fVec3f(CMatrixd* m, const CVec3f* from, const CVec3f* to)
{
    CQuat quat;
    CMatrix_MakeIdentity(m);
    CQuat_MakeRotateFromVec3fVec3f(&quat, from, to);
    CMatrix_SetRotate(m, &quat);
}

void CMatrix_MakeRotateVec3dVec3d(CMatrixd* m, const CVec3d* from, const CVec3d* to)
{
    CQuat quat;
    CMatrix_MakeIdentity(m);
    CQuat_MakeRotateFromVec3dVec3d(&quat, *from, *to);
    CMatrix_SetRotate(m, &quat);
}

void CMatrix_MakeRotateAVec3f(CMatrixd* m, double_type angle, CVec3f* axis)
{
    CQuat quat;
    CMatrix_MakeIdentity(m);
    CQuat_MakeRotateAvec3f(&quat, angle, axis);
    CMatrix_SetRotate(m, &quat);
}

void CMatrix_MakeRotateAVec3d(CMatrixd* m, double_type angle, CVec3d* axis)
{
    CQuat quat;
    CMatrix_MakeIdentity(m);


    CQuat_MakeRotateAvec3d(&quat, angle, *axis);
    CMatrix_SetRotate(m, &quat);
}

void CMatrix_MakeRotateAXYZ(CMatrixd* m, double_type angle, double_type x, double_type y, double_type z)
{
    CQuat quat;
    CMatrix_MakeIdentity(m);
    CQuat_MakeRotateAxyz(&quat, angle, x, y, z);
    CMatrix_SetRotate(m, &quat);
}





void CMatrix_MakeRotateQuat(CMatrixd* m, pCQuat q)
{
    CMatrix_MakeIdentity(m);
    CMatrix_SetRotate(m, q);
}

void CMatrix_MakeRotateAVec3fAVec3fAVec3f(CMatrixd* m, double_type angle1, const CVec3f axis1,
    double_type angle2, const CVec3f axis2,
    double_type angle3, const CVec3f axis3)
{
    CQuat quat;
    CMatrix_MakeIdentity(m);
    CQuat_MakeRotateAvec3fAvec3fAvec3f(&quat, angle1, axis1,
        angle2, axis2,
        angle3, axis3);
    CMatrix_SetRotate(m, &quat);
}

void CMatrix_MakeRotateAVec3dAVec3dAVec3d(CMatrixd* m, double_type angle1, CVec3d axis1,
    double_type angle2, CVec3d axis2,
    double_type angle3, CVec3d axis3)
{
    CQuat quat;
    CMatrix_MakeIdentity(m);


    CQuat_MakeRotateAvec3dAvec3dAvec3d(&quat, angle1, axis1,
        angle2, axis2,
        angle3, axis3);
    CMatrix_SetRotate(m, &quat);
}


CMatrixd CMatrix_RotateVec3fVec3f(const CVec3f* from, const CVec3f* to)
{
    CMatrixd m;
    CMatrix_MakeRotateVec3fVec3f(&m, from, to);
    return m;
}

CMatrixd CMatrix_RotateVec3dVec3d(const CVec3d* from, const CVec3d* to)
{
    CMatrixd m;
    CMatrix_MakeRotateVec3dVec3d(&m, from, to);
    return m;
}

CMatrixd CMatrix_RotateAXYZ(double_type angle, double_type x, double_type y, double_type z)
{
    CMatrixd m;
    CMatrix_MakeRotateAXYZ(&m, angle, x, y, z);
    return m;
}

CMatrixd CMatrix_RotateAVec3f(double_type angle, CVec3f axis)
{
    CMatrixd m;
    CMatrix_MakeRotateAVec3f(&m, angle, &axis);
    return m;
}

CMatrixd CMatrix_RotateAVec3d(double_type angle, CVec3d axis)
{
    CMatrixd m;
    CMatrix_MakeRotateAVec3d(&m, angle, &axis);
    return m;
}

CMatrixd CMatrix_RotateAVec3fAVec3fAVec3f(double_type angle1, const CVec3f axis1,
    double_type angle2, const CVec3f axis2,
    double_type angle3, const CVec3f axis3)
{
    CMatrixd m;
    CMatrix_MakeRotateAVec3fAVec3fAVec3f(&m, angle1, axis1,
        angle2, axis2,
        angle3, axis3);
    return m;
}

CMatrixd CMatrix_RotateAVec3dAVec3dAVec3d(double_type angle1, CVec3d axis1,
    double_type angle2, CVec3d axis2,
    double_type angle3, CVec3d axis3)
{
    CMatrixd m;
    CMatrix_MakeRotateAVec3dAVec3dAVec3d(&m, angle1, axis1,
        angle2, axis2,
        angle3, axis3);
    return m;
}

CMatrixd CMatrix_RotateQuat(pCQuat quat)
{
    CMatrixd m;
    CMatrix_MakeRotateQuat(&m, quat);
    return m;
}

CQuat CMatrix_GetRotate(CMatrixd* m)
{
    CQuat q = CQuat_CreatDefault();

    double_type s;
    double_type tq[4];
    int    i, j;


    tq[0] = 1 + m->_mat[0][0] + m->_mat[1][1] + m->_mat[2][2];
    tq[1] = 1 + m->_mat[0][0] - m->_mat[1][1] - m->_mat[2][2];
    tq[2] = 1 - m->_mat[0][0] + m->_mat[1][1] - m->_mat[2][2];
    tq[3] = 1 - m->_mat[0][0] - m->_mat[1][1] + m->_mat[2][2];


    j = 0;
    for (i = 1; i<4; i++) j = (tq[i]>tq[j]) ? i : j;


    if (j == 0)
    {
        /* perform instant calculation */
        q._v[3] = tq[0];
        q._v[0] = m->_mat[1][2] - m->_mat[2][1];
        q._v[1] = m->_mat[2][0] - m->_mat[0][2];
        q._v[2] = m->_mat[0][1] - m->_mat[1][0];
    }
    else if (j == 1)
    {
        q._v[3] = m->_mat[1][2] - m->_mat[2][1];
        q._v[0] = tq[1];
        q._v[1] = m->_mat[0][1] + m->_mat[1][0];
        q._v[2] = m->_mat[2][0] + m->_mat[0][2];
    }
    else if (j == 2)
    {
        q._v[3] = m->_mat[2][0] - m->_mat[0][2];
        q._v[0] = m->_mat[0][1] + m->_mat[1][0];
        q._v[1] = tq[2];
        q._v[2] = m->_mat[1][2] + m->_mat[2][1];
    }
    else /* if (j==3) */
    {
        q._v[3] = m->_mat[0][1] - m->_mat[1][0];
        q._v[0] = m->_mat[2][0] + m->_mat[0][2];
        q._v[1] = m->_mat[1][2] + m->_mat[2][1];
        q._v[2] = tq[3];
    }

    s = sqrt(0.25 / tq[j]);
    q._v[3] *= s;
    q._v[0] *= s;
    q._v[1] *= s;
    q._v[2] *= s;

    return q;
}

CVec3d CMatrix_GetTrans(CMatrixd* m)
{
    CVec3d v;
    v._v[0] = m->_mat[3][0];
    v._v[1] = m->_mat[3][1];
    v._v[2] = m->_mat[3][2];

    return v;
}

CVec3d CMatrix_GetScale(CMatrixd* m)
{
    CVec3d v, x_vec, y_vec, z_vec;
    x_vec = Vec_CreatVec3d(m->_mat[0][0], m->_mat[1][0], m->_mat[2][0]);
    y_vec = Vec_CreatVec3d(m->_mat[0][1], m->_mat[1][1], m->_mat[2][1]);
    z_vec = Vec_CreatVec3d(m->_mat[0][2], m->_mat[1][2], m->_mat[2][2]);

    v._v[0] = sqrt(x_vec._v[0] * x_vec._v[0] + x_vec._v[1] * x_vec._v[1] + x_vec._v[2] * x_vec._v[2]);
    v._v[1] = sqrt(y_vec._v[0] * y_vec._v[0] + y_vec._v[1] * y_vec._v[1] + y_vec._v[2] * y_vec._v[2]);
    v._v[2] = sqrt(z_vec._v[0] * z_vec._v[0] + z_vec._v[1] * z_vec._v[1] + z_vec._v[2] * z_vec._v[2]);

    return  v;
}




CMatrixd CMatrix_MatrixMultMatrix(const pCMatrix matL, const pCMatrix matR)
{
    CMatrixd mat = CMatrix_Identity();
    CMatrix_mult(&mat, matL, matR);
    return mat;
}
CVec3f CMatrix_Vec3fMultMatrixToVec3f(CVec3f vec, const pCMatrix matR)
{
    return CMatrix_PreMultVec3f(matR, &vec);
}
CVec3d CMatrix_Vec3fMultMatrixToVec3d(CVec3f vec, const pCMatrix matR)
{
    CVec3d xx;
    xx._v[0] = vec._v[0]; xx._v[1] = vec._v[1]; xx._v[2] = vec._v[2];
    return CMatrix_PreMultVec3d(matR, &xx);
}
CVec3f  CMatrix_Vec3dMultMatrixToVec3f(CVec3d vec, const pCMatrix matR)
{
    CVec3f xx;
    vec = CMatrix_PreMultVec3d(matR, &vec);
    xx._v[0] = vec._v[0];
    xx._v[1] = vec._v[1];
    xx._v[2] = vec._v[2];
    return xx;
}
CVec3d CMatrix_Vec3dMultMatrixToVec3d(CVec3d vec, const pCMatrix matR)
{
    return CMatrix_PreMultVec3d(matR, &vec);
}

CVec4f CMatrix_Vec4fMultMatrixToVec4f(pCMatrix p, const CVec4f v)
{
    return CMatrix_preMultVec4f(p, &v);
}
CVec4d CMatrix_Vec4fMultMatrixToVec4d(pCMatrix p, const CVec4d v)
{
    return CMatrix_preMultVec4d(p, &v);
}


CMatrix CMatrix_GetRotateMatrix(pCMatrix p)
{
    CQuat q = CMatrix_GetRotate(p);
    CMatrixd m;
    CMatrix_MakeRotateQuat(&m, &q);
    return m;
}
CMatrix CMatrix_GetTransMatrix(pCMatrix p)
{
    CMatrix mat = CMatrix_Identity();
    mat._mat[3][0] = p->_mat[3][0];
    mat._mat[3][1] = p->_mat[3][1];
    mat._mat[3][2] = p->_mat[3][2];
    return mat;
}


void  CMatrix_MakeOrtho(pCMatrix p, double left, double right,
    double bottom, double top,
    double zNear, double zFar)
{
    /*note transpose of Matrix_implementation wr.t OpenGL documentation
    since the OSG use post multiplication rather than pre.*/
    double tx = -(right + left) / (right - left);
    double ty = -(top + bottom) / (top - bottom);
    double tz = -(zFar + zNear) / (zFar - zNear);
    CMatrix_SETROW(p, 0, 2.0 / (right - left), 0.0, 0.0, 0.0);
    CMatrix_SETROW(p, 1, 0.0, 2.0 / (top - bottom), 0.0, 0.0);
    CMatrix_SETROW(p, 2, 0.0, 0.0, -2.0 / (zFar - zNear), 0.0);
    CMatrix_SETROW(p, 3, tx, ty, tz, 1.0);
}

CMatrix  CMatrix_Ortho(double left, double right,
    double bottom, double top,
    double zNear, double zFar)
{
    CMatrix mat = CMatrix_Identity();
    CMatrix_MakeOrtho(&mat, left, right, bottom, top, zNear, zFar);
    return mat;
}

void CMatrix_MakeOrtho2D(pCMatrix p, double left, double right, double bottom, double top)
{
    CMatrix_MakeOrtho(p, left, right, bottom, top, -1.0, 1.0);
}
CMatrix CMatrix_Ortho2D(double left, double right, double bottom, double top)
{
    CMatrix p = CMatrix_Identity();
    CMatrix_MakeOrtho2D(&p, left, right, bottom, top);
    return p;
}


int CMatrix_GetOrtho(pCMatrix p, double * left, double * right, double * bottom, double * top, double * zNear, double * zFar)
{
    if (p->_mat[0][3] != 0.0 || p->_mat[1][3] != 0.0 || p->_mat[2][3] != 0.0 || p->_mat[3][3] != 1.0)
        return 0;

    *zNear = (p->_mat[3][2] + 1.0) / p->_mat[2][2];
    *zFar = (p->_mat[3][2] - 1.0) / p->_mat[2][2];

    *left = -(1.0 + p->_mat[3][0]) / p->_mat[0][0];
    *right = (1.0 - p->_mat[3][0]) / p->_mat[0][0];

    *bottom = -(1.0 + p->_mat[3][1]) / p->_mat[1][1];
    *top = (1.0 - p->_mat[3][1]) / p->_mat[1][1];
    return 1;
}

int CMatrix_GetFrustum(pCMatrix p, double* left, double* right, double* bottom, double* top, double* zNear, double* zFar)
{
    if (p->_mat[0][3] != 0.0 || p->_mat[1][3] != 0.0 || p->_mat[2][3] != -1.0 || p->_mat[3][3] != 0.0)
        return 0;

    /* note: near and far must be used inside this method instead of zNear and zFar
    because zNear and zFar are references and they may point to the same variable.*/
    {
        double temp_near = p->_mat[3][2] / (p->_mat[2][2] - 1.0);
        double temp_far = p->_mat[3][2] / (1.0 + p->_mat[2][2]);

        *left = temp_near * (p->_mat[2][0] - 1.0) / p->_mat[0][0];
        *right = temp_near * (1.0 + p->_mat[2][0]) / p->_mat[0][0];

        *top = temp_near * (1.0 + p->_mat[2][1]) / p->_mat[1][1];
        *bottom = temp_near * (p->_mat[2][1] - 1.0) / p->_mat[1][1];

        *zNear = temp_near;
        *zFar = temp_far;
    }

    return 1;
}

int CMatrix_GetPerspective(pCMatrix p, double * fovy, double * aspectRatio, double * zNear, double * zFar)
{
    double  right;
    double  left;
    double  top;
    double  bottom;
    /*note: near and far must be used inside this method instead of zNear and zFar
      because zNear and zFar are references and they may point to the same variable.*/
    double  temp_near;
    double  temp_far;
    int  r = 0;
    right = 0.0;
    left = 0.0;
    top = 0.0;
    bottom = 0.0;
    temp_near = 0.0;
    temp_far = 0.0;

    /* get frustum and compute results*/
    r = CMatrix_GetFrustum(p, &left, &right, &bottom, &top, &temp_near, &temp_far);
    if (r)
    {
        *fovy = RadiansToDegrees*(atan(top / temp_near) - atan(bottom / temp_near));
        *aspectRatio = (right - left) / (top - bottom);
    }
    *zNear = temp_near;
    *zFar = temp_far;
    return r;
}


void CMatrix_MakeFrustum(pCMatrix p, double left, double right, double bottom, double top, double zNear, double zFar)
{
    /* note transpose of Matrix_implementation wr.t OpenGL documentation, since the OSG use post multiplication rather than pre.*/
    double A = (right + left) / (right - left);
    double B = (top + bottom) / (top - bottom);
    double C = (fabs(zFar) > DBL_MAX) ? -1. : -(zFar + zNear) / (zFar - zNear);
    double D = (fabs(zFar) > DBL_MAX) ? -2.*zNear : -2.0*zFar*zNear / (zFar - zNear);
    CMatrix_SETROW(p, 0, 2.0*zNear / (right - left), 0.0, 0.0, 0.0);
    CMatrix_SETROW(p, 1, 0.0, 2.0*zNear / (top - bottom), 0.0, 0.0);
    CMatrix_SETROW(p, 2, A, B, C, -1.0);
    CMatrix_SETROW(p, 3, 0.0, 0.0, D, 0.0);
}

void CMatrix_MakePerspective(pCMatrix p, double fovy, double aspectRatio, double zNear, double zFar)
{
    /* calculate the appropriate left, right etc.*/
    double tan_fovy = tan(DegreesToRadians*(fovy*0.5));
    double right = tan_fovy * aspectRatio * zNear;
    double left = -right;
    double top = tan_fovy * zNear;
    double bottom = -top;
    CMatrix_MakeFrustum(p, left, right, bottom, top, zNear, zFar);
}

CMatrix CMatrix_Perspective(double fovy, double aspectRatio, double zNear, double zFar)
{
    CMatrix pro = CMatrix_Identity();
    CMatrix_MakePerspective(&pro, fovy, aspectRatio, zNear, zFar);
    return pro;
}
CMatrix CMatrix_Frustum(pCMatrix p, double left, double right, double bottom, double top, double zNear, double zFar)
{
    CMatrix pro = CMatrix_Identity();
    CMatrix_MakeFrustum(&pro, left, right, bottom, top, zNear, zFar);
    return pro;
}
