#include <vector>
#include <iostream>

#include "HVTMgr.h"
#include "plane3d_.h"
#include "AircraftModels.h"
#include "Hangar.h"
#include "HmsSvsFlightTube.h"
#include "AltManager.h"
#include "Drawable.h"
#include "Module.h"
#include "3DAirportMgr.h"
#include "3DBarrierMgr.h"
#include "TerrainChunk.h"
#include "OverLookCamera.h"
#include "TerrainThread.h"
#include "FPPCamera.h"
//#include "TAWS.h"
#include "Global.h"
//#include "GlobalInterFace.h"
#include "TextureMgr.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "CoordinateSystemMgr.h"

#ifdef __cplusplus
}
#endif

int  yidong = 0;
static unsigned int  wangluo = 0;
static unsigned int pfd = 1;
extern GLuint shaderprogram;
extern GLuint shaderprogram2;
extern GLuint shaderprogram3;
extern GLuint shaderprogram4;
extern GLuint shaderprogramHvWire;
extern GLuint shaderprogramHvTower;
extern GLuint shaderprogramAircraft;
extern GLuint shaderProgramOnDraw;
GLuint MatrixM2ID;

void CModule_ResizeOpenGL(int startx, int starty, int width, int height)
{
    //printf("重置窗口位置大小:%d,%d,%d,%d\n",startx,starty,width,height);
    GetGlobal()->iWindowWidth = width;
    GetGlobal()->iWindowHeight = height;
    GetGlobal()->iPosX = startx;
    GetGlobal()->iPosY = starty;
}

#if 0
void CModule_SetMainView()
{
    /*设置主窗口*/
    pGLOBAL pglobal = 0;
    if (pglobal = GetGlobal())
    {
        switch (pglobal->eMainView)
        {
        case MainView_3DAnd2D:
            FPPCamera_MaxViewport();
            OverLookCamera_DefaultViewport();
            break;
        }
    }
}
#endif

void CModule_InitScence()
{
    static int initScenceFlg = 0;

    if (initScenceFlg != 0)
    {
        return;
    }

    LogWrite(LOG_LEVEL_ALWAYS, "init scence");

    GetFPPManipulator()->vLonLatAlt = GetGlobal()->vLonLatAlt;

    CCoordinateMgr_Update(GetFPPManipulator()->vLonLatAlt);//jy：每一帧的参考坐标原点不是地球球心，而是摄像机所在的整数经纬度地块的中心点，海拔为0
    LogWrite(LOG_LEVEL_ALWAYS, "init scence");
    GetFPPManipulator()->Update();

    initScenceFlg = 1;

    CDrawable_registExtOpenglFunction();
    LogWrite(LOG_LEVEL_ALWAYS, "init scence over\n");
}

Plane3D* g_pPlane3d = nullptr;//current selected aircraft model
Hangar* g_pHanger = nullptr;

void CModule_InitScence2()
{
    /*注册按回调事件*/
    LogSetLogLevel(LOG_LEVEL_ALWAYS);

    LogWrite(LOG_LEVEL_ALWAYS, "init scence");
    
    g_pHanger = new Hangar;
    if (0 == g_pHanger)
    {
        LogWrite(LOG_LEVEL_ALWAYS, "init scence new hanger failed.");
        return;
    }

    auto hcb = [&](bool bRes){
        if (bRes)
        {
            auto anl = g_pHanger->GetAircraftNameList();
            if (anl.size() > 2)
            {
                int initPlaneId = 1;
                CHmodule_SetAircraftModel(anl[initPlaneId].c_str());
            }
        }
    };

    static HangarInitThread hit(*g_pHanger, hcb);

    //todo: 初始化飞行管道初始化数据
    //if (GetGlobal()->m_waypointRender)
    if (0)
    {
        HmsSvsFlightTubeMgr::getInstance()->setFlightPlanData(nullptr, 10);
    }
}

bool CHmodule_SetAircraftModel(const char * modeName)
{
    Plane3D* pmd = g_pHanger->GetAircraft(modeName);
    if (nullptr == pmd)
    {
        return false;
    }
    g_pPlane3d = pmd;
    return true;
}

#if 0
CVec3d GetAutoPlus()
{
    CVec3d lonlat = GetGlobal()->vLonLatAlt;
    lonlat._v[0] -= 0.00021*0.050 * 20 * 10;
    lonlat._v[2] = CAltManager_GetAltByLonLat(lonlat._v[0], lonlat._v[1]) + 50;

    return lonlat;
}
#endif

void CModule_PaintOpenGL()
{
    CModule_DrawAll();
}

void CModule_DrawAll()
{
    static unsigned long ms1 = clock();
    static unsigned long ms2 = clock();
    static float deltaSec = 0.0f;

    if (ms2 < ms1)
    {
        ms1 = 0;//unsigned long flip
    }

    deltaSec = (ms2 - ms1) / 1000.0f;
    ms1 = clock();//ms
    CModule_Update(deltaSec);
    CModule_Render(deltaSec);
    ms2 = clock();
}

void CModule_Update(float delta)
{
    //C3DHVTL_Add(103, 30);
    //3D update
    pGLOBAL pglobal = GetGlobal();

    if (pglobal->m_hvtRender)
    {
        HVTMgr::getInstance()->Update(pglobal->vLonLatAlt);
    }

    if (pglobal->FristPersonMode)
    {
        if (g_pPlane3d)
        {
            g_pPlane3d->Update(pglobal->vLonLatAlt, pglobal->vYawPitchRoll);
        }
    }

    if (pglobal->m_waypointRender)
    {
        HmsSvsFlightTubeMgr::getInstance()->Update(delta);
    }

    //3d terrain
    if (pglobal->bShowTerrain)
    {
        LogWrite(LOG_LEVEL_FATAL, "C3DTerrainManager_SetLonLatAlt update  \n");
        C3DTerrainManager_SetLonLatAlt(pglobal->vLonLatAlt);
        //C3DTerrainManager_UpdateTerrain();
        C3DTerrainManager_UpdateTerrain_multiThread();
    }

    //C3DNAGLWarningMgr_SetHeight(pglobal->vLonLatAlt._v[2]);
    //C3DNAGLWarningMgr_UpdateData();
    LogWrite(LOG_LEVEL_FATAL, "C3DNAGLWarningMgr_UpdateData over  \n");
    //3d airport
    if (pglobal->m_airportRender)
    {
        LogWrite(LOG_LEVEL_FATAL, "C3DAirportMgrSetLonLat  start  \n");
        C3DAirportMgrSetLonLat(pglobal->vLonLatAlt, pglobal->vYawPitchRoll);
        C3DAirportMgrUpdate();
        LogWrite(LOG_LEVEL_FATAL, "C3DAirportMgrSetLonLat  over  \n");
    }

    if (pglobal->m_barrierRender)
    {
        C3DBarrierMgrSetLonLat(pglobal->vLonLatAlt);
        C3DBarrierMgrUpdate();
    }
}

void CModule_Render(float delta)
{
    CModule_Render3D();
}

void CModule_Render3D()
{
    //draw 3d
    //camera and init
    pGLOBAL pglobal = GetGlobal();

    GetFPPManipulator()->vLonLatAlt = pglobal->vLonLatAlt;
    GetFPPManipulator()->vYawPitchRoll = pglobal->vYawPitchRoll;
    GetFPPManipulator()->Update();
    LogWrite(LOG_LEVEL_FATAL, "FPPCamera update vLonLatAlt:%lf,%lf,%lf\n", pglobal->vLonLatAlt._v[0], pglobal->vLonLatAlt._v[1], pglobal->vLonLatAlt._v[2]);
    FPPCamera_SetPolyModeFlg(pglobal->glPolygonModeFlag);
    FPPCamera_SetFogFlg(GetGlobal()->bFogFlag);
    FPPCamera_SetFogColor(pglobal->vFogColor);
    FPPCamera_SetFogRange(GetGlobal()->vFogRange);
    FPPCamera_SetLightFlg(GetGlobal()->bLight);
    FPPCamera_SetViewMat(&(GetFPPManipulator()->mViewMatrix));
    LogWrite(LOG_LEVEL_FATAL, "CCoordinateMgr_Update update over\n");
    CCoordinateMgr_Update(GetFPPManipulator()->vLonLatAlt);//jy：每一帧的参考坐标原点不是地球球心，而是摄像机所在的整数经纬度地块的中心点，海拔为0
    LogWrite(LOG_LEVEL_FATAL, "camera update over\n");

    shaderProgramOnDraw = 0;

    FPPCamera_Render();

    if (pglobal->bShowTerrain)
    {
        shaderProgramOnDraw = shaderprogram;
        glUseProgram(shaderProgramOnDraw);//use terrain's own shader
        //setup projection matrix & view matrix for terrain shader
        GLuint MatrixIDP = glGetUniformLocation(shaderProgramOnDraw, "P");
        CMatrixf matrixProjectf = CMatrix_d2f(FPPCamera_GetCamera()->projectmatrix);
        glUniformMatrix4fv(MatrixIDP, 1, GL_FALSE, &matrixProjectf._mat[0][0]);// in the "P" uniform			
        GLuint MatrixIDV = glGetUniformLocation(shaderProgramOnDraw, "V");
        MatrixM2ID = glGetUniformLocation(shaderProgramOnDraw, "M");
        CMatrixf matrixViewf = CMatrix_d2f(FPPCamera_GetCamera()->viewmatrixCurrTerrCenter);
        glUniformMatrix4fv(MatrixIDV, 1, GL_FALSE, &matrixViewf._mat[0][0]);// in the "V" uniform

        CNode_RenderChild(C3DTerrainManager_GetTerrainMgr()->m_root, HmsAviPf::Mat4d::IDENTITY, false);
        //C3DTerrainManager_RenderFilledBuffer();				
    }

    if (pglobal->m_hvtRender)
    {//画高压线
        shaderProgramOnDraw = shaderprogramHvWire;
        glUseProgram(shaderProgramOnDraw);
        //setup projection matrix & view matrix for barrier shader			
        GLuint MatrixIDP = glGetUniformLocation(shaderProgramOnDraw, "P");
        CMatrixf matrixProjectf = CMatrix_d2f(FPPCamera_GetCamera()->projectmatrix);
        glUniformMatrix4fv(MatrixIDP, 1, GL_FALSE, &matrixProjectf._mat[0][0]);// in the "P" uniform
        GLuint MatrixIDV = glGetUniformLocation(shaderProgramOnDraw, "V");
        MatrixM2ID = glGetUniformLocation(shaderProgramOnDraw, "M");
        CMatrixf matrixViewf = CMatrix_d2f(FPPCamera_GetCamera()->viewmatrixCurrTerrCenter);
        glUniformMatrix4fv(MatrixIDV, 1, GL_FALSE, &matrixViewf._mat[0][0]);// in the "V" uniform

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        CNode_RenderChild(HVTMgr::getInstance()->RootWires(), HmsAviPf::Mat4d::IDENTITY, false);
    }

    if (pglobal->m_hvtRender)
    {//画高压电塔
        shaderProgramOnDraw = shaderprogramHvTower;
        glUseProgram(shaderProgramOnDraw);
        //setup projection matrix & view matrix for barrier shader			
        GLuint MatrixIDP = glGetUniformLocation(shaderProgramOnDraw, "P");
        CMatrixf matrixProjectf = CMatrix_d2f(FPPCamera_GetCamera()->projectmatrix);
        glUniformMatrix4fv(MatrixIDP, 1, GL_FALSE, &matrixProjectf._mat[0][0]);// in the "P" uniform
        GLuint MatrixIDV = glGetUniformLocation(shaderProgramOnDraw, "V");
        MatrixM2ID = glGetUniformLocation(shaderProgramOnDraw, "M");
        CMatrixf matrixViewf = CMatrix_d2f(FPPCamera_GetCamera()->viewmatrixCurrTerrCenter);
        glUniformMatrix4fv(MatrixIDV, 1, GL_FALSE, &matrixViewf._mat[0][0]);// in the "V" uniform

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        CNode_RenderChild(HVTMgr::getInstance()->RootTowers(), HmsAviPf::Mat4d::IDENTITY, false);
    }

    if (pglobal->m_airportRender)
    {
        shaderProgramOnDraw = shaderprogram2;
        glDisable(GL_DEPTH_TEST);
        glUseProgram(shaderProgramOnDraw);//use airport's own shader
        //setup projection matrix & view matrix for airport shader			
        GLuint MatrixIDP = glGetUniformLocation(shaderProgramOnDraw, "P");
        CMatrixf matrixProjectf = CMatrix_d2f(FPPCamera_GetCamera()->projectmatrix);
        glUniformMatrix4fv(MatrixIDP, 1, GL_FALSE, &matrixProjectf._mat[0][0]);// in the "P" uniform
        GLuint MatrixIDV = glGetUniformLocation(shaderProgramOnDraw, "V");
        MatrixM2ID = glGetUniformLocation(shaderProgramOnDraw, "M");
        CMatrixf matrixViewf = CMatrix_d2f(FPPCamera_GetCamera()->viewmatrixCurrTerrCenter);
        glUniformMatrix4fv(MatrixIDV, 1, GL_FALSE, &matrixViewf._mat[0][0]);// in the "V" uniform

        pC3DAirportMgr ptrmgr = GetC3DAirportMgr();
        if (ptrmgr)
        {
            CNode_RenderChild(ptrmgr->m_root, HmsAviPf::Mat4d::IDENTITY, false);
        }

        glEnable(GL_DEPTH_TEST);
    }

    if (pglobal->m_barrierRender)
    {
        shaderProgramOnDraw = shaderprogram3;
        glUseProgram(shaderProgramOnDraw);//use barrier's own shader
        //setup projection matrix & view matrix for barrier shader			
        GLuint MatrixIDP = glGetUniformLocation(shaderProgramOnDraw, "P");
        CMatrixf matrixProjectf = CMatrix_d2f(FPPCamera_GetCamera()->projectmatrix);
        glUniformMatrix4fv(MatrixIDP, 1, GL_FALSE, &matrixProjectf._mat[0][0]);// in the "P" uniform
        GLuint MatrixIDV = glGetUniformLocation(shaderProgramOnDraw, "V");
        MatrixM2ID = glGetUniformLocation(shaderProgramOnDraw, "M");
        CMatrixf matrixViewf = CMatrix_d2f(FPPCamera_GetCamera()->viewmatrixCurrTerrCenter);
        glUniformMatrix4fv(MatrixIDV, 1, GL_FALSE, &matrixViewf._mat[0][0]);// in the "V" uniform

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        pC3DBarrierMgr ptrmgr = GetC3DBarrierMgr();
        if (ptrmgr)
        {
            CNode_RenderChild(ptrmgr->m_root, HmsAviPf::Mat4d::IDENTITY, false);
        }
    }

    if (pglobal->m_waypointRender)
    {
        shaderProgramOnDraw = shaderprogram3;
        glUseProgram(shaderProgramOnDraw);//use barrier's own shader
        //setup projection matrix & view matrix for barrier shader			
        GLuint MatrixIDP = glGetUniformLocation(shaderProgramOnDraw, "P");
        CMatrixf matrixProjectf = CMatrix_d2f(FPPCamera_GetCamera()->projectmatrix);
        glUniformMatrix4fv(MatrixIDP, 1, GL_FALSE, &matrixProjectf._mat[0][0]);// in the "P" uniform
        GLuint MatrixIDV = glGetUniformLocation(shaderProgramOnDraw, "V");
        MatrixM2ID = glGetUniformLocation(shaderProgramOnDraw, "M");
        CMatrixf matrixViewf = CMatrix_d2f(FPPCamera_GetCamera()->viewmatrixCurrTerrCenter);
        glUniformMatrix4fv(MatrixIDV, 1, GL_FALSE, &matrixViewf._mat[0][0]);// in the "V" uniform

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        auto pRootWp = HmsSvsFlightTubeMgr::getInstance()->getRootWaypoints();
        if (pRootWp)
        {
            CNode_RenderChild(pRootWp, HmsAviPf::Mat4d::IDENTITY, false);
        }

        shaderProgramOnDraw = shaderprogram4;
        glUseProgram(shaderProgramOnDraw);//use barrier's own shader
        //setup projection matrix & view matrix for barrier shader			
        MatrixIDP = glGetUniformLocation(shaderProgramOnDraw, "P");
        matrixProjectf = CMatrix_d2f(FPPCamera_GetCamera()->projectmatrix);
        glUniformMatrix4fv(MatrixIDP, 1, GL_FALSE, &matrixProjectf._mat[0][0]);// in the "P" uniform
        MatrixIDV = glGetUniformLocation(shaderProgramOnDraw, "V");
        MatrixM2ID = glGetUniformLocation(shaderProgramOnDraw, "M");
        matrixViewf = CMatrix_d2f(FPPCamera_GetCamera()->viewmatrixCurrTerrCenter);
        glUniformMatrix4fv(MatrixIDV, 1, GL_FALSE, &matrixViewf._mat[0][0]);// in the "V" uniform
        glDisable(GL_CULL_FACE);
        auto pRootFT = HmsSvsFlightTubeMgr::getInstance()->getRootFlightTubes();
        if (pRootFT)
        {
            CNode_RenderChild(pRootFT, HmsAviPf::Mat4d::IDENTITY, false);
        }
        glEnable(GL_CULL_FACE);
    }

    if (pglobal->FristPersonMode)//0:1st person mode, 1:3rd person mode
    {
        shaderProgramOnDraw = shaderprogramAircraft;
        glUseProgram(shaderProgramOnDraw);
        //setup projection matrix & view matrix for barrier shader			
        GLuint MatrixIDP = glGetUniformLocation(shaderProgramOnDraw, "P");
        CMatrixf matrixProjectf = CMatrix_d2f(FPPCamera_GetCamera()->projectmatrix);
        glUniformMatrix4fv(MatrixIDP, 1, GL_FALSE, &matrixProjectf._mat[0][0]);// in the "P" uniform
        GLuint MatrixIDV = glGetUniformLocation(shaderProgramOnDraw, "V");
        MatrixM2ID = glGetUniformLocation(shaderProgramOnDraw, "M");
        CMatrixf matrixViewf = CMatrix_d2f(FPPCamera_GetCamera()->viewmatrixCurrTerrCenter);
        glUniformMatrix4fv(MatrixIDV, 1, GL_FALSE, &matrixViewf._mat[0][0]);// in the "V" uniform

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        if (g_pPlane3d)
        {
            g_pPlane3d->Render();
        }
    }

}
