#ifndef _MODULE__H_
#define _MODULE__H_

#include "Drawable.h"
#include "Node.h" 
#include "MatrixNode.h"
#include "FPPManipulator.h"
#include "Global.h"
#include "Log.h"
#include "GLHeader.h" 
#include "Coordinate.h"
#include "FileManager.h"  
#include "Matrix.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "Quat.h"

#ifdef __cplusplus
}
#endif

typedef struct data
{
	int a;
	int b;
}Data,*pData;

void CModule_InitScence();
void CModule_InitScence2();

bool CHmodule_SetAircraftModel(const char * modeName);
void CModule_PaintOpenGL();
void CModule_ResizeOpenGL(int startx,int starty,int width,int height);
 
//void CModule_SetMainView();

void CModule_Paint2DOpenGL_OLD();

void CModule_DrawAll();
void CModule_Update(float delta);
void CModule_Render(float delta);
void CModule_Render3D();

void SvsTerrainDataService();


#endif
