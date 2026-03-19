#ifndef _CAMERA__H_
#define _CAMERA__H_

#include "Node.h" 
#include "Matrix.h"

#ifdef __cplusplus
extern "C" {
#endif


#include "_Vec3.h"

#ifdef __cplusplus
}
#endif


typedef struct _CCamera
{
    /*节点信息*/
    pCNode root;
    /*眼点*/
    CVec3d eye, center, up;
    /**/
    /*视图矩阵*/
    CMatrix viewmatrix;
    CMatrix viewmatrixCurrTerrCenter;//origin coordinate is center of current terrain tile.
    /*投影矩阵*/
    CMatrix projectmatrix;
    /*视口用CVec4存储的int
    unsigned int	viewport[4];*/
    CVec4f  	viewport;
    /*是否需要初始化*/
    unsigned char iInit;

    float fovY;
    /**/
    unsigned int iClearMask;
    void(*ResizeOpenGL)(struct _CCamera* camera);
    void(*InitOpenGL)(struct _CCamera* camera);
    void(*PaintOpenGL)(struct _CCamera* camera);
}CCamera, *pCCamera;

pCCamera CCamera_Create();
void CCamera_InitOpenGL(pCCamera camera);
void CCamera_ResizeOpenGL(pCCamera camera);
void CCamera_PaintOpenGL(pCCamera camera);
CVec3d CCamera_GetPosition(pCCamera camera);

typedef struct _Frustum
{
    int i;
}Frustum, *pFrustum;


#endif