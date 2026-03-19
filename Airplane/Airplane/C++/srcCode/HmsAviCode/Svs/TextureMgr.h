#ifndef __TEXTURE_MGR__H_
#define __TEXTURE_MGR__H_

#include "Image.h"

typedef enum _TextureType
{
	TextureType_RunWay0 = 0,
	TextureType_RunWay1,
	TextureType_RunWay2,
	TextureType_RunWay3,
	TextureType_RunWay4,
	TextureType_RunWay5,
	TextureType_RunWay6,
	TextureType_RunWay7,
	TextureType_RunWay8,
	TextureType_RunWay9,
	TextureType_RunWay10,
	TextureType_RunWay11,
	TextureType_RunWay12,
	TextureType_RunWay13,
	TextureType_RunWay14,
	TextureType_RunWay15,
	TextureType_RunWay16,
	TextureType_RunWay17,
	TextureType_RunWay18,
	TextureType_RunWay19,
	TextureType_RunWay20,
	TextureType_RunWay21,
	TextureType_RunWay22,
	TextureType_RunWay23,
	TextureType_RunWay24,
	TextureType_RunWay25,
	TextureType_RunWay26,
	TextureType_RunWay27,
	TextureType_RunWay28,
	TextureType_RunWay29,
	TextureType_RunWay30,
	TextureType_RunWay31,
	TextureType_RunWay32,
	TextureType_RunWay33,
	TextureType_RunWay34,
	TextureType_RunWay35,
	TextureType_RunWay36,
	TextureType_RunWayThr=TextureType_RunWay36+1,
	TextureType_RunWayLeft=TextureType_RunWayThr+1,
	TextureType_RunWayRight=TextureType_RunWayLeft+1, 
	TextureType_RunWayLine=TextureType_RunWayRight+1,
	TextureType_3DTerrain,
	TextureType_3DTerrain_NEW,
	TextureType_2DTerrain,
	TextureType_Warning,
	TextureType_Obstacle,
	TextureType_Obstacle_with_Light,
	TextureType_Tall_Obstacle,
	TextureType_Tall_Obstacle_with_Light,  
	TextureType_Tall_FlightPath,
	TextureType_Aircraft, 
	TextureType_Demo,
	TextureType_WarningPoint_Red,
	TextureType_WarningPoint_Yellow,
	TextureType_DetailMap,
	TextureType_PLANE,
	TextureType_Airport,
	MAX_IMAGE_NUMBER,
}TextureType;   

pCImage TextureMgr_GetImage(enum _TextureType type);

pCImage TextureMgr_GetFlightPathImage();

void TextureMgr_SetImage(enum _TextureType type,pCImage p);

void TextureMgr_Init();

pCImage TextureMgr_Get2DTerrainImage(char* name);

#endif //  __TEXTURE_MGR__H_