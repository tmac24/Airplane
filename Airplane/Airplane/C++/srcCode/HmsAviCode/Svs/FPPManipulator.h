#ifndef _CAMERA_MANIPULATOR__H_
#define _CAMERA_MANIPULATOR__H_

#include "Matrix.h"


typedef struct _CameraManipulator
{
    /*经纬度矩阵*/
    CMatrixd m_lonlatMat;
    /*世界坐标矩阵*/
    CMatrixd m_worldMat;
    /*姿态旋转矩阵*/
    CMatrixd m_attitude;
    /* 经纬度旋转矩阵*/
    CMatrixd m_lolatRotate;
    /*模型默认方向旋转*/
    CMatrixd m_nosDirMat;

    /*经纬高转换得到的xyz*/
    CVec3d m_lonlatPos;
    int isActive;
    
    int lastMousePosx;
    int lastMousePosy;

    int leftButtonDown;
    int rightButtonDown;

    CVec3f vFrom;
    CVec3f vTo;
    /*默认的旋转，相机通过这个旋转可以把坐标系转换为OSG的坐标系*/
    CMatrix mDefaultMat;
    /**************第一人称****************/
    /*鼠标速度 类似于CS的鼠标灵敏度*/
    float fMouseSpeed;
    /*前后左右移动速度*/
    float fMoveSpeed;
    /*视点前方*/
    CVec3d vLookAtDir;
    /*视点右方*/
    CVec3d vLookAtRig;
    /*视点上方*/
    CVec3d vLookAtUp;
    /*视点位置*/
    CVec3d vPosition;
    /*视角姿态*/
    CVec3d vYawPitchRoll;
    /*航向 顺时针旋转为正 欧拉度
    俯仰 向上为正  欧拉度
    滚转 左滚为正 欧拉度
    姿态head pitch roll*/
    CVec3d m_hpr;
    /*最终的视图矩阵*/
    CMatrix mViewMatrix;
    /*经纬高 弧度*/
    CVec3d vLonLatAlt;
    /***************************************/

    void(*Update)(void);
}CCameraManipulator, *pCCameraManipulator;


extern pCCameraManipulator  g_pCameraManipulator;

void  CameraManipulator_Create();
void  CameraManipulator_Init();
void  CameraManipulator_BinFunc(pCCameraManipulator p);

void  CCameraManipulator_CalcLook();

void CameraManipulator_Update(void);

pCCameraManipulator GetFPPManipulator();

extern double g_camyaw;
extern double g_campitch;
extern int g_camMode;//1st 3rd person mode


#endif