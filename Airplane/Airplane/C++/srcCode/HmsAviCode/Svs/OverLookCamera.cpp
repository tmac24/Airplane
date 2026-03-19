#include "OverLookCamera.h"
#include "MathCommon.h"
#include "FPPCamera.h"
#include "Global.h"
#include "Coordinate.h"
#include "CoordinateSystemMgr.h"
#include <stdio.h>


/*
static pCCamera fppcamera;
static  unsigned char bSetUpCamera;
*/
static  unsigned char glPolyModeFlag;
static  unsigned char bFogFlag;
static CVec4f vFogColor;
static CVec2f vFogRange;

pCCamera overLookCamera = NULL;


void OverLookCamera_SetViewport(int x, int y, unsigned int w, unsigned int h)
{
    pCCamera camera;
    if (camera = OverLookCamera_GetCamera())
    {
        float viewport_w = w;
        float viewport_h = h;
        float viewport_x = x;
        float viewport_y = y;
        OverLookCamera_GetCamera()->viewport = Vec_CreatVec4f(viewport_x, viewport_y, viewport_w, viewport_h);

#if TAWS_TERRAIN_SHADING_MAP_ORTH2D
        {
            float width, height;
            width = height = GetGlobal()->ucRange*1000.0*0.8;
            viewport_w = 1.5*viewport_h / viewport_w;
            height = width*viewport_w;
            camera->projectmatrix = CMatrix_Ortho(-width / 2.0, width / 2.0, -height / 2.0, height / 2.0, 1, 400000.0);
            //OverLookCamera_GetCamera()->viewport=Vec_CreatVec4f(0,0,viewport_w,viewport_h); 
        }
#else
        camera->projectmatrix = CMatrix_Perspective(camera->fovY,viewport_w/viewport_h,1,400000.0);
#endif

    }
}
void OverLookCamera_DefaultViewport()
{
    pCCamera camera = 0;
    if (camera = OverLookCamera_GetCamera())
    {
        int iPosX = GetGlobal()->iPosX;
        int iPosY = GetGlobal()->iPosY;
        unsigned int w = GetGlobal()->iWindowWidth, h = GetGlobal()->iWindowHeight;
        float asp = (float)h / (float)w;
        float viewport_h = h / 3.0;
        float viewport_w = viewport_h / asp;
        float viewport_x = w - viewport_w + iPosX;
        float viewport_y = 0 + iPosY;
        OverLookCamera_SetViewport(viewport_x, viewport_y, viewport_w, viewport_h);
    }
}

void OverLookCamera_SetCamera(pCCamera p)
{
    overLookCamera = p;
}

pCCamera OverLookCamera_GetCamera()
{
    if (overLookCamera == NULL)
    {
        float viewport_x, viewport_y, viewport_w, viewport_h;
        pCCamera camera = CCamera_Create();
        camera->eye = Vec_CreatVec3d(0, -1, 0);
        camera->center = Vec_CreatVec3d(0, 0, 0);
        camera->up = Vec_CreatVec3d(0, 0, 1);

        viewport_w = GetGlobal()->iWindowWidth / 4.0;
        viewport_h = GetGlobal()->iWindowHeight / 4.0;
        viewport_x = GetGlobal()->iWindowWidth - viewport_w;
        viewport_y = 0;//GetGlobal()->iWindowHeight - viewport_w; 
        camera->fovY = 2.0 * RadiansToDegrees*(atan((float)(viewport_h) / (float)(viewport_w)*	tan(DegreesToRadians*(90.0) / 2.0)));
        camera->viewmatrix = CMatrix_LookAtVec3d(&(camera->eye), &(camera->center), &(camera->up));

#if TAWS_TERRAIN_SHADING_MAP_ORTH2D
        {
            float width, height;
            width = height = GetGlobal()->ucRange*1000.0;
            camera->projectmatrix = CMatrix_Ortho(-width / 2.0, width / 2.0, -height / 2.0, height / 2.0, 1, 400000.0);
        }
#else
        camera->projectmatrix = CMatrix_Perspective(camera->fovY, viewport_w / viewport_h, 1, 400000.0);
#endif		
        camera->viewport = Vec_CreatVec4f(viewport_x, viewport_y, viewport_w, viewport_h);
        OverLookCamera_SetCamera(camera);
    }
    /* lock */
    return overLookCamera;
    /* unlock */
}

