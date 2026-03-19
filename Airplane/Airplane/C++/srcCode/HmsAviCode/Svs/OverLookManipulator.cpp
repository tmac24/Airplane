#include "OverLookManipulator.h"
#include "MatrixNode.h"
#include "MathCommon.h"
#include "Global.h"
#include "Coordinate.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 


pCOverLookManipulator g_pOverLookManipulator = NULL;

void OverLookManipulator_Create()
{
    if (g_pOverLookManipulator == NULL)
    {
        g_pOverLookManipulator = (pCOverLookManipulator)malloc(sizeof(COverLookManipulator));
        memset(g_pOverLookManipulator, 0, sizeof(COverLookManipulator));
        OverLookManipulator_BinFunc(g_pOverLookManipulator);
        g_pOverLookManipulator->viewMatrix = CMatrix_LookAtVec3f(Vec_CreatVec3f(0, 0, 0), Vec_CreatVec3f(0, 0, -1), Vec_CreatVec3f(0, 1, 0));
        g_pOverLookManipulator->mDefaultMat = CMatrix_RotateAVec3f(PI_2, Vec_CreatVec3f(1, 0, 0));
        g_pOverLookManipulator->fMouseSpeed = 0.1F;
        g_pOverLookManipulator->lastMousePosx = GetGlobal()->iWindowWidth / 2.0;
        g_pOverLookManipulator->lastMousePosy = GetGlobal()->iWindowHeight / 2.0;
    }
}

void OverLookManipulator_Init()
{
    OverLookManipulator_Create();
    g_pOverLookManipulator->vLonLatAlt = GetGlobal()->vLonLatAlt;
    g_pOverLookManipulator->center =
        g_pOverLookManipulator->up =
        g_pOverLookManipulator->eye = Coordinate_DegreeVec3dLonLatAltToVec3d(g_pOverLookManipulator->vLonLatAlt);
    OverLookManipulator_BinFunc(g_pOverLookManipulator);

}

pCOverLookManipulator GetOverLookManipulator()
{
    if (g_pOverLookManipulator == NULL)
    {
        OverLookManipulator_Init();
    }
    return g_pOverLookManipulator;
}

void OverLookManipulator_BinFunc(pCOverLookManipulator p)
{
    p->Update = OverLookManipulator_Update;
}

void OverLookManipulator_Update(void)
{
    pCOverLookManipulator pman = GetOverLookManipulator();
    CMatrix attitudeMat = pman->viewMatrix = Coordinate_YawPitchRollVec3dDegreeToMatrix(pman->vYawPitchRoll);
    {
        CMatrix trans = Coordinate_Vec3dDegreeLonlatToMatrix(pman->vLonLatAlt);
        CMatrix lonlatRotate = CMatrix_GetRotateMatrix(&trans);
        trans = CMatrix_GetTransMatrix(&trans);
        pman->vPosition = CMatrix_GetTrans(&trans);
        {
            CMatrix viewMat = CMatrix_MatrixMultMatrix(&attitudeMat, &lonlatRotate);
            OverLookManipulator_CalcLook();
            viewMat = CMatrix_MatrixMultMatrix(&pman->mDefaultMat, &viewMat);
            viewMat = CMatrix_MatrixMultMatrix(&viewMat, &trans);
            attitudeMat = viewMat;
        }
        attitudeMat = CMatrix_Inverse(&attitudeMat);
        pman->mViewMatrix = attitudeMat;
        /*pman->viewMatrix = attitudeMat;*/
        /*¸üÐÂµ½eye,center,up*/
        {
            CVec3d eye, center, up;
            CMatrix_GetLookAtVec3d(&attitudeMat, &eye, &center, &up, 1);
            pman->eye = eye;
            pman->center = center;
            pman->up = up;
        }
    }
    OverLookManipulator_CalcLightMatrix();
}

void OverLookManipulator_CalcLightMatrix(void)
{
    pCOverLookManipulator pman = GetOverLookManipulator();
    CMatrix attitudeMat = pman->viewMatrix = Coordinate_YawPitchRollVec3dDegreeToMatrix(Vec_CreatVec3d(0, -90, 0));
    CMatrix trans = Coordinate_Vec3dDegreeLonlatToMatrix(pman->vLonLatAlt);
    CMatrix lonlatRotate = CMatrix_GetRotateMatrix(&trans);
    {
        trans = CMatrix_GetTransMatrix(&trans);
        {
            CMatrix viewMat = CMatrix_MatrixMultMatrix(&attitudeMat, &lonlatRotate);
            viewMat = CMatrix_MatrixMultMatrix(&pman->mDefaultMat, &viewMat);
            viewMat = CMatrix_MatrixMultMatrix(&viewMat, &trans);
            attitudeMat = CMatrix_Inverse(&viewMat);
            pman->LightMatrix = attitudeMat;
        }

    }
}

void OverLookManipulator_CalcLook()
{
    pCOverLookManipulator pman = GetOverLookManipulator();
    pman->vLookAtDir = CMatrix_Vec3fMultMatrixToVec3d(Vec_CreatVec3f(0, 1, 0), &pman->viewMatrix);
    pman->vLookAtRig = CMatrix_Vec3fMultMatrixToVec3d(Vec_CreatVec3f(1, 0, 0), &pman->viewMatrix);
    pman->vLookAtUp = CMatrix_Vec3fMultMatrixToVec3d(Vec_CreatVec3f(0, 0, 1), &pman->viewMatrix);
    Vec_NormalizeVec3d(&pman->vLookAtDir);
    Vec_NormalizeVec3d(&pman->vLookAtRig);
    Vec_NormalizeVec3d(&pman->vLookAtUp);
}

