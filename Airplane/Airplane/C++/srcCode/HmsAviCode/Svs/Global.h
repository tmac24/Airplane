
#ifndef _GLOBAL__H_
#define _GLOBAL__H_

#include "Log.h"
#include "GLHeader.h"
#include "Common.h"

#ifdef __cplusplus
extern "C" {
#endif


#include "_Vec2.h"
#include "_Vec3.h"
#include "_Vec4.h"
#include "ManipulatorType.h"

#ifdef __cplusplus
}
#endif

/*alt data source
0: CHmsGlobal::GetInstance()->GetTerrainServer()->GetAltitude();
1: CAltManager_GetAltByLonLat()
*/
#define GLOBAL_ALT_DATA_SOURCE 1

#define GLOBAL_WRITE_FAIL 0
#define GLOBAL_WRITE_SUCCESS 1

#define GLOBAL_READ_FAIL 0
#define GLOBAL_READ_SUCCESS 1

#pragma warning(disable:4996)

typedef struct _GLOBAL
{
    int iPosX, iPosY;
    /*窗口宽*/
    unsigned int iWindowWidth;
    /*窗口高*/
    unsigned int iWindowHeight;
    /*开启雾*/
    int bFogFlag;
    CVec4f vFogColor;
    CVec2f vFogRange;
    CVec4f vBackColor;
    int bUseJoyStick;
    int bLoadBuilt;
    unsigned int iLoadBuiltTileSize;
    unsigned char bShowTerrain;
    int bSetPos;
    CVec3d vLonLatAlt;
    int bIsAnyRunwayNearby;
    CVec3d vYawPitchRoll;
    int drawMode;//画线模式，0为triangle_strip 或 1为line_strip
    int glPolygonModeFlag;
    float fStep;
    enum ViewType eViewType;
    //enum MainView eMainView;
    float fMainViewHawkeyeHeight;
    unsigned char bLight;
    /*量程环*/
    unsigned int ucRange;
    unsigned char b2DRotate;
    unsigned char bWarning;
    /*地理长度*/
    float pro_Distance;
    /*夹角*/
    float pro_angle;
    /*地理宽度*/
    float pro_width;
    /*分辨率*/
    void(*Save)();
    void(*Reset)();
    double _2DLatHeight;
    double _2DLonWidth;
    double  m_rou;
    double m_rangeHeight;
    unsigned int m_2DTerrainNum;
    CVec4f m_offset;
    unsigned char m_terrainSingleColor;
    unsigned char m_use3DWarning;
    unsigned char m_use2DWarning;
    //显示跑道边线
    unsigned char m_showRuwFrame;
    //设置2d地形告警模式
    unsigned char m_terrain2DMode;
    //障碍物渲染
    unsigned char m_barrierRender;
    //高压线渲染
    unsigned char  m_hvtRender;
    //航路点渲染
    unsigned char m_waypointRender;
    //飞行管道渲染
    unsigned char m_flighttubeRender;
    //机场渲染
    unsigned char m_airportRender;
    //飞机图标渲染
    unsigned char m_craftRender;
    //经纬网格
    unsigned char m_gridRender;
    //
    unsigned char enableNet;
    unsigned char terrain2D_color_mode;
    unsigned char terrain2DColorFlag;
    unsigned char only2DRender;
    unsigned char drawback;
    unsigned int warningNum;
    unsigned char pfd;
    unsigned int fps;
    int waringstate;	//告警级别
    int radioHeight;	//无线电高度
    int verticleSpeed; //空速
    int goundSpeed;	//地速
    int cullback;
    float fovX;		//jy: x方向的fov
    float fovY;
    int terrain2dmode;
    int FristPersonMode;
}GLOBAL, *pGLOBAL;

extern pGLOBAL  g_Global;

pGLOBAL GetGlobal();
void Global_Save();
void Global_Reset();

void  Global_BindFunc(pGLOBAL p);

static int Global_WriteFile(pGLOBAL p);

static int Global_ReadFile(pGLOBAL p);

static GLOBAL  Global_CreatDefault();

static CVec3d sScenceRelativePoint;
static CVec3d sScenceRelativeLonLatAlt;

CVec3d Global_GetScenceRelativeNegativePoint();
CVec3d Global_GetScenceRelativePoint();
void  Global_SetScenceRelativePoint(CVec3d point);
void Global_SetScenceRelativeLonLatAlt(CVec3d lonlatalt);
CVec3d Global_GetScenceRelativeLonLatAlt();
void Global_SetLonLatAlt(CVec3d lla);
void Global_SetYawPitchRoll(CVec3d ypr);

#endif
