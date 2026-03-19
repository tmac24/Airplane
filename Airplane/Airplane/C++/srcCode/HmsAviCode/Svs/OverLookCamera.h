#ifndef _OVERLOOK_CAMERA__H_
#define _OVERLOOK_CAMERA__H_

#include "Camera.h"
#include "OverLookManipulator.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "_Vec4.h"
#include "_Vec2.h"

#ifdef __cplusplus
}
#endif

pCCamera OverLookCamera_GetCamera();
void OverLookCamera_DefaultViewport();



#endif