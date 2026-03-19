#include "Global.h"
#include "Coordinate.h"
#include "math/HmsMath.h"

pGLOBAL g_Global = NULL;

pGLOBAL GetGlobal()
{
    if (g_Global == NULL)
    {
        /*目前创建它，没有存储为配置文件*/
        g_Global = (pGLOBAL)malloc(sizeof(GLOBAL));

        if (g_Global == NULL)
        {
            //printf("g_Global = (pGLOBAL)malloc(sizeof(GLOBAL)) failed\n");
            LogWrite(LOG_LEVEL_FATAL, "g_Global = (pGLOBAL)malloc(sizeof(GLOBAL)) failed.");//xyh 退出了打印无用写一行日志.
            exit(1);
        }
        Global_BindFunc(g_Global);
#ifdef WIN32
        /*	if (GLOBAL_READ_SUCCESS != Global_ReadFile(g_Global))*/
        {
            *g_Global = Global_CreatDefault();
            Global_WriteFile(g_Global);
        }
#else
        *g_Global = Global_CreatDefault();
#endif

    }
    return g_Global;
}

void Global_BindFunc(pGLOBAL p)
{
    p->Reset = Global_Reset;
    p->Save = Global_Save;
}
int Global_WriteFile(pGLOBAL p)
{
    char* file = "../Config.ini";
    FILE* fp = NULL;
    /*二进制写取的形式打开 */
    fp = fopen(file, "wb");
    if (NULL == fp)
    {
        printf("Global_WriteFile write config failed\n");
        return GLOBAL_WRITE_FAIL;
    }
    fwrite(p, sizeof(GLOBAL), 1, fp);
    fclose(fp);
    return GLOBAL_WRITE_SUCCESS;
}

int Global_ReadFile(pGLOBAL p)
{
    char* file = "../Config.ini";
    GLOBAL config;

    FILE* fp = NULL;
    /*二进制读取的形式打开 */
    fp = fopen(file, "rb");
    if (NULL == fp)
    {
        return GLOBAL_READ_FAIL;
    }
    /*整个读进来*/
    fread((&config), sizeof(config), 1, fp);
    *p = config;
    fclose(fp);
    return GLOBAL_READ_SUCCESS;
}
static GLOBAL  Global_CreatDefault()
{
    GLOBAL g;
    memset(&g, 0, sizeof(g));
    g.iPosX = 0;
    g.iPosY = 0;
    g.iWindowWidth = 800;
    g.iWindowHeight = 450;
    g.iWindowWidth = 1920;
    g.iWindowHeight = 1080;
    g.iWindowWidth = 1024;
    g.iWindowHeight = 768;
    g.iWindowWidth = 1272;
    g.iWindowHeight = 892;
    g.m_rou = 2.0;
    //g.bFogFlag = 1;
    g.bFogFlag = 0;
    g.vFogColor = Vec_CreatVec4f(0.68039223, 0.88627458, 1.0, 0.5);
    g.vBackColor = Vec_CreatVec4f(24 / 255.0, 70 / 255.0, 251 / 255.0, 1.0);
    g.vBackColor = g.vFogColor;// Vec_CreatVec4f(24/255.0,70/255.0,251/255.0,1.0);
    g.vFogRange = Vec_CreatVec2f(0.0, 30000.0);
    g.bUseJoyStick = 0;
    g.bLoadBuilt = 0;
    g.iLoadBuiltTileSize = 3;
    g.bSetPos = 0;
    /*g.vLonLatAlt=
    Vec_CreatVec3d(94.338108, 29.306496, 5000.0);*/
    g.vLonLatAlt = Vec_CreatVec3d(103.946944, 30.578333, 1000);
    //g.vLonLatAlt=		Vec_CreatVec3d(103.9394140	,30.5637337, 	511);
    //g.vLonLatAlt=	Vec_CreatVec3d(103.94838583	,30.57446041, 	1000); 
    //双流山中测试点
    //g.vLonLatAlt= 		Vec_CreatVec3d(103.48526678	,31.10966467, 	1575); 
    //g.vYawPitchRoll=Vec_CreatVec3d(30,-30,0);
    //g.vLonLatAlt=Vec_CreatVec3d(104.0739877,30.6497181,607.0);  
    //g.vYawPitchRoll=Vec_CreatVec3d(21.8,1.7,0);
    //双流山中测试点
    //g.vLonLatAlt=Vec_CreatVec3d(103.3681287,30.9472734,4370.0);  
    //西安咸阳机场
    //g.vLonLatAlt=	Vec_CreatVec3d(108.7652216	,34.455256, 	506.0);
    //g.vLonLatAlt = Vec_CreatVec3d(108.7652216, 34.455256, 706.0);//jiangyan: assend
    g.vLonLatAlt = Vec_CreatVec3d(103.000, 29.675, 2500);//jiangyan: 雅安
    //g.vLonLatAlt = Vec_CreatVec3d(103.100, 29.100, 2500);//jiangyan: 雅安附近
    g.vYawPitchRoll = Vec_CreatVec3d(0.0, 0.0, 0);
    g.bIsAnyRunwayNearby = 0;//0, no runway nearby ;  1, a runway nearby (judging by a threshold distance)
    g.drawMode = 0;//0:triangle_strip, 1:line_strip, 2:line_loop
    g.eViewType = ViewType_FirstPerson;
    //g.eMainView = MainView_3DAnd2D;
    //	g.eMainView = MainView_3D;
    // 	g.eMainView = MainView_2D;
    g.fMainViewHawkeyeHeight = 100;
    g.glPolygonModeFlag = 0;
    //g.fStep = 100;//jiangyan: mod
    g.fStep = 10;
#if TAWS_USE_LIGHT
    g.bLight = 1;
#else
    g.bLight = 0;
#endif
    g.b2DRotate = 1;
    g.ucRange = 30;
    /*地理长度*/
    g.pro_Distance = 10000;
    /*夹角*/
    g.pro_angle = 5;
    /*地理宽度*/
    g.pro_width = 100.0;
    g._2DLonWidth = 1.0;
    g._2DLatHeight = 1.0;
    g.m_rangeHeight = 10000.0;
    g.m_offset = Vec_CreatVec4f(0, -1000, 0, 0);
    g.m_terrainSingleColor = 0;
    //显示跑道边线
    g.m_showRuwFrame = 0;
    //设置2d地形告警模式
    g.m_terrain2DMode = 0;
    //Terran render
    g.bShowTerrain = 1;
    //机场渲染
    g.m_airportRender = 1;
    //障碍物渲染
    g.m_barrierRender = 1;
    //高压线渲染
    g.m_hvtRender = 1;
    //飞机图标渲染
    g.m_craftRender = 0;
    g.m_waypointRender = 0;
    g.m_flighttubeRender = 0;
    g.m_gridRender = 0;
    g.enableNet = 1;
    g.terrain2D_color_mode = 0;
    g.fovX = 82.0;
    g.fovY = 65.0;
    g.terrain2DColorFlag = 1;
    g.only2DRender = 1;
    g.waringstate = 0;
    g.verticleSpeed = 0;
    g.radioHeight = 0;
    g.cullback = 1;
    g.terrain2dmode = Terrain2DMode_CenterCraft;
    g.FristPersonMode = 0;//0 = firet person view, 1 = 3rd person view
    g.pfd = 1;
    return g;
}

void Global_Save()
{
    Global_WriteFile(GetGlobal());
}
void Global_Reset()
{
    if (GLOBAL_READ_SUCCESS != Global_ReadFile(GetGlobal()))
    {
        printf("g_Global reset failed\n");
    }
}

CVec3d Global_GetScenceRelativeNegativePoint()
{
    return Vec_CreatVec3d(-sScenceRelativePoint._v[0], -sScenceRelativePoint._v[1], -sScenceRelativePoint._v[2]);
}
CVec3d Global_GetScenceRelativePoint()
{
    return sScenceRelativePoint;
}

void Global_SetScenceRelativePoint(CVec3d point)
{
    sScenceRelativePoint = point;
}

void Global_SetScenceRelativeLonLatAlt(CVec3d lonlatalt)
{
    sScenceRelativeLonLatAlt = lonlatalt;
    Global_SetScenceRelativePoint(Coordinate_DegreeVec3dLonLatAltToVec3d(sScenceRelativeLonLatAlt));
}

CVec3d Global_GetScenceRelativeLonLatAlt()
{
    return sScenceRelativeLonLatAlt;
}

void Global_SetLonLatAlt(CVec3d lla)
{
    //经度规范化到 [-180,+180)
    if (lla._v[0] < -180)
    {
        lla._v[0] = fmod(lla._v[0] - 180, 360.0) + 180;
    }
    else//>=0
    {
        lla._v[0] = fmod(lla._v[0] + 180, 360.0) - 180;
    }

    //纬度限制到[-89.99, +89.99]
    lla._v[1] = HMS_MAX(lla._v[1], -89.99);
    lla._v[1] = HMS_MIN(lla._v[1], +89.99);

    GetGlobal()->vLonLatAlt = lla;
}

void Global_SetYawPitchRoll(CVec3d ypr)
{
    GetGlobal()->vYawPitchRoll = ypr;
}