#ifndef _GL_HEADER__H_
#define  _GL_HEADER__H_
#ifdef WIN32
#include <Windows.h> //jiangyan： add
#endif
#if defined(__APPLE__) || defined(MACOSX)
//#include <GLUT/glut.h>
#else

//#include "myEsUtil.h"//jiangyan:
#include "esUtil.h"
//#include <GL/gl.h>


//#ifdef _VS2010
//#include "GL/glext.h"
#include "GLES2/gl2ext.h"


#include <stdio.h>
#include <stdlib.h>
#ifdef __vxworks
//jy: vx version stdlib lack of max min
#ifndef __cplusplus
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif  /* __cplusplus */
#endif

//#include <string.h>
#endif

#include "Log.h"		
#include <time.h>



///* 法线计算*/
//#ifndef _VS2010
//#define  VxWorks_JINGJIA_5400 1
//#define  TAWS_USE_VxUDP 1
//#else
/*VxWorks景嘉5400显卡 */
#define  VxWorks_JINGJIA_5400 0
#define  TAWS_USE_VxUDP 0
//#endif


//#define TRUE  1
//#define  FALSE 0
#define TERRAIN2D_OPTMIZITION 1
#define TERRAIN3D_DETAILMAP   0

#if _WIN32
#define TAWS_GUI 1
#endif


#define TAWS_FREE_FAILED 0
#define TAWS_FREE_SUCCCESS 1
#if TAWS_GUI
/*调试窗口*/
#define _USE_ANTTWEAKBAR_ 0
/*Input*/
#define _USE_INPUT_ 1
#else


#if VxWorks_JINGJIA_5400
/*调试窗口*/
#define _USE_ANTTWEAKBAR_ 0
/*Input*/
#define _USE_INPUT_ 0
#else
/*调试窗口*/
#define _USE_ANTTWEAKBAR_ 0
/*Input*/
#define _USE_INPUT_ 1
#endif



#endif




#define  _DISABLE_DISLAY_LIST_ 0

/*2D地形晕渲图使用3D地形LOD节点方式*/
#define TAWS_TERRAIN_SHADING_MAP_LOD 0
/*使用JEP地形数据或者SRTM地形数据*/
#define TAWS_TERRAIN_JEP_DATA 0
/*三维地形是否按照顶点着色*/
#define  TAWS_PER_VERTEX_COLOR 1
/*2D视图是否使用ORTH正交投影*/
#define  TAWS_TERRAIN_SHADING_MAP_ORTH2D 1

/*2D视图是否使用经纬度*/
#define  TAWS_MAP_2D_LONLAT 1

/*3D视图地图分层告警*/
#define  TAWS_MAP_3D_WARNING 0
/*3D视图地图分层告警*/
#define  TAWS_MAP_2D_WARNING 1

/*障碍物分层告警*/

/*障碍物查询地形高度*/
#define  QUERY_TERRAIN_HEIGHT 1

/* 机场调整*/
#define  TAWS_AIRPORT_ZUUU 1
#define  TAWS_AIRPORT_ZSSS 1
#define   TAWS_AIRPORT_ZUCK 1

/* 使用2D还是3D做2D图*/
#define  TAWS_2D_USE3D 1

#define TAWS_MAP_TEXTURE_SIZE_SAME 1

#define  TAWS_USE_CSTL_VECTOR 1

#define  TAWS_USE_LARGE_RANGE 0

#define  TAWS_USE_VERTEX_GRID   0

#define  TAWS_USE_NALG 1
//#define  _VS2010 1

//#ifdef _VS2010
/*是否使用libpng*/
#define  TAWS_USE_LIGHT  0
/*是否使用libpng*/
#define  TAWS_USE_PNG  0
/*模拟光照计算 */
#define  TAWS_SIM_LIGHT 1
/*是否传递法线*/
#define  TAWS_PASS_NORMAL 0
/* 法线计算*/
#define  TAWS_CALC_NORMAL 1
//#else
//#define  TAWS_USE_LIGHT  0
///*是否使用libpng*/
//#define  TAWS_USE_PNG  0
//
///*模拟光照计算 */
//#define  TAWS_SIM_LIGHT 1
///*是否传递法线*/
//#define  TAWS_PASS_NORMAL 0
///* 法线计算*/
//#define  TAWS_CALC_NORMAL 1
//#endif

#include "Common.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "_Vec4.h"

#ifdef __cplusplus
}
#endif

void GLHeader_PrintGlError();


//static float TerColor[4] = { 0.5, 1.0, 0.5, 0.0 };
static float TerColor[4] = { 181 / 255.0f, 117 / 255.0f, 70 / 255.0f, 0.3f };
static float SkyColor[4] = { 0.5f, 0.6f, 0.95f, 1.0f };
static float ProColor[4] = { 6.0f / 255.0f, 110.0f / 255.0f, 70.0f / 255.0f, 0.8f };
static float FogColor[4] = { 0.5f, 0.6f, 0.85f, 1.0f };


#define  ColorCount 16
static CVec4f m_AltColorVec[ColorCount] =
{
    0.02f, 0.24f, 0.62f, 1.0f,
    0.02f, 0.44f, 0.33f, 1.0f,
    0.04f, 0.62f, 0.46f, 1.0f,
    0.04f, 0.69f, 0.46f, 1.0f,
    0.04f, 0.81f, 0.46f, 1.0f,
    0.24f, 0.60f, 0.14f, 1.0f,
    0.41f, 0.62f, 0.16f, 1.0f,
    0.43f, 0.64f, 0.18f, 1.0f,
    0.64f, 0.66f, 0.18f, 1.0f,
    0.85f, 0.79f, 0.21f, 1.0f,
    0.70f, 0.42f, 0.17f, 1.0f,
    0.72f, 0.44f, 0.19f, 1.0f,
    0.73f, 0.45f, 0.20f, 1.0f,
    0.89f, 0.38f, 0.22f, 1.0f,
    0.91f, 0.38f, 0.24f, 1.0f,
    0.01f, 0.01f, 0.60f, 1.0f
};
static int m_AltVec[ColorCount] =
{
    0,
    10,
    20,
    30,
    50,
    70,
    90,
    200,
    400,
    600,
    800,
    1000,
    2000,
    4000,
    6000,
    9000
};

#define LEVEL  0

typedef enum _Layer2D
{
    Layer2D_Terrain = -15,

    Layer2D_TerrainWarning2D = 5,
    Layer2D_PointWarning = 10,
    Layer2D_Airport = LEVEL + 28,
    Layer2D_Barrier = LEVEL + 20,
    Layer2D_HVTL = LEVEL + 25,
    Layer2D_Aircraft = LEVEL + 30,
    Layer2D_Text = LEVEL + 35,
    Layer2D_Warning = LEVEL + 40
}Layer2D;

#define PROFILE_HEAD   (long f,l;)
#define PROFILE_BEG    (f = clock();)
#define PROFILE_END    (l = clock();)
#define PROFILE_PRINT(name)  (printf("%s, use time: %d", name, l-f);)


double getd_demical(double num);
float getf_demical(float num);

void memcpy_op(void* src, void* dst, int n);

#endif
