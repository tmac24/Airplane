#ifndef _FIRST_PERSON__H_
#define _FIRST_PERSON__H_


#include "FPPManipulator.h"
#include "Camera.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "_Vec4.h"
#include "_Vec3.h"
#include "_Vec2.h"

#ifdef __cplusplus
}
#endif


/*
static pCCamera fppcamera;
static  unsigned char bSetUpCamera;
*/
//static  unsigned char glPolyModeFlag;
//static  unsigned char bFogFlag;
//static  unsigned char bLightFlag;
//static CVec4f vFogColor;
//static CVec2f vFogRange;

void FPPCamera_SetPolyModeFlg(unsigned char bFlg);
void FPPCamera_SetFogFlg(unsigned char bFog);
void FPPCamera_SetLightFlg(unsigned char bLight);
void FPPCamera_SetFogColor(CVec4f vFogColor);
void FPPCamera_SetFogRange(CVec2f vFogRange);

void FPPCamera_InitOpenGL(pCCamera camera);
void FPPCamera_ResizeOpenGL(pCCamera camera);
void FPPCamera_PaintOpenGL(pCCamera camera);

void FPPCamera_RenderNode(pCNode node);

void FPPCamera_SetCamera(pCCamera p);


void FPPCamera_InitCamera();
pCCamera FPPCamera_GetCamera();

void FPPCamera_InitCamera();
void FPPCamera_Render();

void FPPCamera_SetViewMat(const pCMatrix viemat);


void FPPCamera_MinViewport();

void FPPCamera_MaxViewport();


#endif