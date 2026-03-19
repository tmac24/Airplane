#include "FPPManipulator.h"
#include "MatrixNode.h"
#include "MathCommon.h"
#include "Global.h"
#include "Coordinate.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 


pCCameraManipulator g_pCameraManipulator = NULL;

void CameraManipulator_Create()
{
    if (g_pCameraManipulator == NULL)
    {
        g_pCameraManipulator = (pCCameraManipulator)malloc(sizeof(CCameraManipulator));
        if (g_pCameraManipulator)
        {
            memset(g_pCameraManipulator, 0, sizeof(CCameraManipulator));
            CameraManipulator_BinFunc(g_pCameraManipulator);

            g_pCameraManipulator->mDefaultMat = CMatrix_RotateAVec3f(PI_2, Vec_CreatVec3f(1, 0, 0));
            g_pCameraManipulator->fMouseSpeed = 0.05F;
            g_pCameraManipulator->lastMousePosx = GetGlobal()->iWindowWidth / 2.0;
            g_pCameraManipulator->lastMousePosy = GetGlobal()->iWindowHeight / 2.0;
        }
    }
}

void CameraManipulator_Init()
{
    CameraManipulator_Create();
    g_pCameraManipulator->vLonLatAlt = GetGlobal()->vLonLatAlt;
    CameraManipulator_BinFunc(g_pCameraManipulator);
}

pCCameraManipulator GetFPPManipulator()
{
    if (g_pCameraManipulator == NULL)
    {
        CameraManipulator_Init();
    }
    return g_pCameraManipulator;
}

void CameraManipulator_BinFunc(pCCameraManipulator p)
{
    p->Update = CameraManipulator_Update;
}

//interface
double g_camyaw;
double g_campitch;
int g_camMode;//1st 3rd person mode

void interface_updateCam()
{
    GetGlobal()->FristPersonMode = g_camMode;
}

void CameraManipulator_Update(void)
{
    pCCameraManipulator pman = GetFPPManipulator();

    interface_updateCam();


    CMatrix matPlaneAttitude = Coordinate_YawPitchRollVec3dDegreeToMatrix(pman->vYawPitchRoll);

    CMatrix matPlaneTrans = Coordinate_Vec3dDegreeLonlatToMatrix(pman->vLonLatAlt);
    CMatrix matLonlatRotate = CMatrix_GetRotateMatrix(&matPlaneTrans);
    matPlaneTrans = CMatrix_GetTransMatrix(&matPlaneTrans);
    pman->vPosition = CMatrix_GetTrans(&matPlaneTrans);//up direction , Perpendicular to the ground

    //////////////////////////////////////////////////////////////////////////
    CMatrix matCameraAttitude = CMatrix_Identity();
    CMatrix matCamTrans = CMatrix_Identity();
    CMatrix matCamYaw = CMatrix_Identity();
    CMatrix matCamPitch = CMatrix_Identity();
    if (GetGlobal()->FristPersonMode == 1)//if 3rd person view
    {//rotate based on object croodinate
        CMatrix_SetTransXYZ(&matCamTrans, 0, -12, 4);
    }
    else//1st person view
    {//rotate based on world croodinate
    }
    CMatrix_MakeRotateAXYZ(&matCamYaw, -g_camyaw, 0, 0, 1);//model coordinate, Z is up, X is right, Y is forward
    CMatrix_MakeRotateAXYZ(&matCamPitch, g_campitch, 1, 0, 0);
    matCameraAttitude = CMatrix_MatrixMultMatrix(&matCameraAttitude, &matCamTrans);
    matCameraAttitude = CMatrix_MatrixMultMatrix(&matCameraAttitude, &matCamPitch);
    matCameraAttitude = CMatrix_MatrixMultMatrix(&matCameraAttitude, &matCamYaw);

    //////////////////////////////////////////////////////////////////////////

    if (GetGlobal()->FristPersonMode == 1)//if 3rd person view
    {
        // 1st person view , The camera attitude is superimposed on the airplane attitude
        matCameraAttitude = CMatrix_MatrixMultMatrix(&matCameraAttitude, &matPlaneAttitude);
    }
    else
    {
        // 1st person view , The camera attitude is superimposed on the airplane attitude
        matCameraAttitude = CMatrix_MatrixMultMatrix(&matCameraAttitude, &matPlaneAttitude);
    }

    CMatrix viewMat = CMatrix_MatrixMultMatrix(&matCameraAttitude, &matLonlatRotate);

    viewMat = CMatrix_MatrixMultMatrix(&pman->mDefaultMat, &viewMat);
    viewMat = CMatrix_MatrixMultMatrix(&viewMat, &matPlaneTrans);
    pman->mViewMatrix = CMatrix_Inverse(&viewMat);//jy:why inverse? 相机相对于物体的移动转化为物体相对于相机的移动。送入shader。手动生成VIEW矩阵就要这样，最终都是移动物体。
}

