
#include "AltManager.h"
#include "MathCommon.h"
#include "FPPCamera.h"
#include "Global.h"
#include "Coordinate.h"
#include "CoordinateSystemMgr.h"
#include <stdio.h>

#if HMS_TARGET_PLATFORM == HMS_PLATFORM_IOS
#include "OpenGLES/ES2/gl.h"
#endif


static  unsigned char glPolyModeFlag;
static  unsigned char bFogFlag;
static  unsigned char bLightFlag;
static CVec4f vFogColor;
static CVec2f vFogRange;

pCCamera fppcamera = NULL;

//CMatrix viewmatrix;
//CMatrixd matrixProjectd;

void FPPCamera_InitCamera()
{
}

pCCamera FPPCamera_GetCamera()
{
    if (fppcamera == NULL)
    {
        pCCamera camera = CCamera_Create();
        float vfov = 2.0 * RadiansToDegrees*(atan((float)(GetGlobal()->iWindowHeight) / (float)(GetGlobal()->iWindowWidth)	*	tan(DegreesToRadians*(90.0) / 2.0)));
        camera->fovY = vfov;
        camera->eye = Vec_CreatVec3d(0, -1, 0);
        camera->center = Vec_CreatVec3d(0, 0, 0);
        camera->up = Vec_CreatVec3d(0, 0, 1);
        camera->viewport = Vec_CreatVec4f(0, 0, GetGlobal()->iWindowWidth, GetGlobal()->iWindowHeight);
        camera->viewmatrix = CMatrix_LookAtVec3d(&(camera->eye), &(camera->center), &(camera->up));
        camera->projectmatrix = CMatrix_Perspective(camera->fovY, GetGlobal()->iWindowWidth / (float)(GetGlobal()->iWindowHeight), 100, 400000.0);
        FPPCamera_SetCamera(camera);
    }
    return fppcamera;
}
void FPPCamera_SetCamera(pCCamera p)
{
    fppcamera = p;
}

//输入当地时间，当地经纬度（转化为弧度），输出太阳高度（弧度），方向（弧度）
void sunPosition(int year, int month, int day, int hour, int minute, int second, float localLat, float locatLon, float* height, float* direction)
{
    float theta, t, N = 0.0f, NO;//N为积日  theta为赤纬角
    float lat;//赤纬角
    float Et;//时差
    float timeAngle;		//时角

    int monthDay[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    int count;
    for (count = 0; count < month - 1; count++)
    {
        N += monthDay[count];
    }
    N += day;
    if (((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) && (month >= 3))
        N++;
    NO = 79.6764 + 0.2422*(year - 1985) - (int)((year - 1985) / 4);
    t = N - NO;
    theta = 2 * 3.1415926*t / 365.2422;

    lat = 0.3723 + 23.2567*sin(theta) + 0.1149*sin(2 * theta) - 0.1712*sin(3 * theta) - 0.758*cos(theta) + 0.3656*cos(2 * theta) + 0.0201*cos(3 * theta);
    lat = lat*3.1415926f / 180.0f;

    Et = 0.0028 - 1.9857*sin(theta) + 9.9059*sin(2 * theta) - 7.0924*cos(theta) - 0.6882*cos(2 * theta);

    timeAngle = ((float)(hour - 12) + (float)(minute - (120.0f*3.14159f / 180.0f - locatLon) * 4 * 180 / 3.1415926 + Et) / 60 + (float)(second) / 3600)*15.0f*3.1415926 / 180.0f;
    *height = sin(localLat)*sin(lat) + cos(localLat)*cos(lat)*cos(timeAngle);
    *height = asin(*height);

    *direction = (sin(*height)*sin(localLat) - sin(lat)) / (cos(*height)*cos(localLat));
    if (*direction >= 0.9999999)
    {
        *direction = 0.9999999F;
    }
    else if (*direction <= -0.9999999)
    {
        *direction = -0.9999999F;
    }
    //	direction=clamp((sin(height)*sin(localLat)-sin(lat))/(cos(height)*cos(localLat)), -0.9999999, 0.9999999);
    *direction = acos(*direction);
    if (timeAngle < 0)
        *direction = -(*direction);
}

void FPPCamera_InitOpenGL(pCCamera camera)
{
    //glClear(GL_DEPTH_BUFFER_BIT);	

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    /*更新光照*/
    if (bLightFlag)
    {
        float mat_specular_par = 0.2f;
        float mat_shininess_par = 0.1f;
        float white_diffuse_par = 0.5f;
        float Light_Model_Ambient_par = 0.5f;

        GLfloat mat_specular[] = { mat_specular_par, mat_specular_par, mat_specular_par, 1.0 };

        GLfloat mat_shininess[] = { mat_shininess_par, mat_shininess_par, mat_shininess_par, 1.0 };

        GLfloat light_position[] = { 1, 1.0, 1.0, 0.0 };

        GLfloat white_diffuse[] = { white_diffuse_par, white_diffuse_par, white_diffuse_par, 1.0 };

        GLfloat Light_Model_Ambient[] = { Light_Model_Ambient_par, Light_Model_Ambient_par, Light_Model_Ambient_par, 1.0 }; // 
        {
            CVec3d sunDir = Vec_CreatVec3d(1, 1, 1);
            CMatrix inverview = CMatrix_GetRotateMatrix(&camera->viewmatrix);
            sunDir = CMatrix_Vec3dMultMatrixToVec3d(sunDir, &inverview);
            light_position[0] = sunDir._v[0];
            light_position[1] = sunDir._v[1];
            light_position[2] = sunDir._v[2];
        }
    }

    if (bFogFlag)
    {
        glClearColor(vFogColor._v[0], vFogColor._v[1], vFogColor._v[2], 1);
    }
    else
    {
        glClearColor(GetGlobal()->vBackColor._v[0], GetGlobal()->vBackColor._v[1], GetGlobal()->vBackColor._v[2], 1);
    }

    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_BLEND);
    glDisable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void FPPCamera_ResizeOpenGL(pCCamera camera)
{
    pGLOBAL pglobal = GetGlobal();
    if (camera == NULL)
    {
        printf("CCamera_ResizeOpenGL , FPP Camera ==NULL;\n");
        return;
    }
    else
    {
        int iPosX = GetGlobal()->iPosX;
        int iPosY = GetGlobal()->iPosY;
        float asp = (float)(GetGlobal()->iWindowWidth) / (float)(GetGlobal()->iWindowHeight);
#if 0
        //方式1 x方向fov为固定值
        float fovy = GetGlobal()->fovX / asp;//jy:得到y方向的fov
        camera->fovY = fovy;
#else
        //方式2 y方向fov为固定值
        camera->fovY = GetGlobal()->fovY;
#endif
        camera->projectmatrix = CMatrix_Perspective(camera->fovY, asp, 1.0, 400000.0);//400000.0		

        camera->viewport = Vec_CreatVec4f(iPosX, iPosY, GetGlobal()->iWindowWidth, GetGlobal()->iWindowHeight);
        glViewport(camera->viewport._v[0], camera->viewport._v[1], camera->viewport._v[2], camera->viewport._v[3]);

        CVec3d eye, center, up;

        //CMatrix viewmatrix = camera->viewmatrix;			
        CMatrix_GetLookAtVec3d(&(camera->viewmatrix), &eye, &center, &up, 10.0);
        eye = CCoordinateMgr_GetWGS84RelativePoint(eye);
        center = CCoordinateMgr_GetWGS84RelativePoint(center);

        //update view matrix of current-terrain-center version
        camera->viewmatrixCurrTerrCenter = CMatrix_LookAtVec3d(&eye, &center, &up);//修改view矩阵平移分量。view的逆矩阵：原来是从地心移动到眼前，现在是从当前位置1度整经度1度整纬度区块的中位置（海拔为0）移动到眼前。
    }
}

#if 0
void FPPCamera_PaintOpenGL(pCCamera camera)
{
    if (camera == NULL)
    {
        printf("CCamera_ResizeOpenGL , FPP Camera ==NULL;\n");
        return;
    }
    else
    {
        CNode_RenderChild(camera->root, TODO, TODO);
    }
}

void FPPCamera_RenderNode(pCNode node)
{
    if (FPPCamera_GetCamera() == NULL || node == NULL)
    {
        return;
    }

    //Draw3();
    CNode_RenderChild(node, TODO, TODO);
}
#endif

void FPPCamera_Render()
{
    FPPCamera_InitOpenGL(FPPCamera_GetCamera());
    FPPCamera_ResizeOpenGL(FPPCamera_GetCamera());
#if 0//jiangyan: 地形未挂在camera root 下，禁掉不影响地形显示
    FPPCamera_PaintOpenGL(FPPCamera_GetCamera());
#endif
}

void FPPCamera_SetViewMat(const pCMatrix viemat)
{
    FPPCamera_GetCamera()->viewmatrix = *viemat;
}

void FPPCamera_SetFogFlg(unsigned char bFog)
{
    bFogFlag = bFog;
}
void FPPCamera_SetFogColor(CVec4f vfogColor)
{
    vFogColor = vfogColor;
}
void FPPCamera_SetFogRange(CVec2f vfogRange)
{
    vFogRange = vfogRange;
}
void FPPCamera_SetPolyModeFlg(unsigned char bFlg)
{
    glPolyModeFlag = bFlg;
}
void FPPCamera_SetLightFlg(unsigned char bLight)
{
    bLightFlag = bLight;
}

void FPPCamera_MinViewport()
{
    pCCamera camera;
    if (camera = FPPCamera_GetCamera())
    {
        int iPosX = GetGlobal()->iPosX;
        int iPosY = GetGlobal()->iPosY;
        unsigned int iWindowWidth = 1, iWindowHeight = 1;
        camera->projectmatrix = CMatrix_Perspective(camera->fovY, (float)iWindowWidth / (float)(iWindowHeight), 1, 400000.0);
        camera->viewport = Vec_CreatVec4f(iPosX, iPosY, iWindowWidth, iWindowHeight);
    }
}

void FPPCamera_MaxViewport()
{
    pCCamera camera;
    if (camera = FPPCamera_GetCamera())
    {
        int iPosX = GetGlobal()->iPosX;
        int iPosY = GetGlobal()->iPosY;
        unsigned int iWindowWidth = GetGlobal()->iWindowWidth, iWindowHeight = GetGlobal()->iWindowHeight;
        camera->projectmatrix = CMatrix_Perspective(camera->fovY, (float)iWindowWidth / (float)(iWindowHeight), 1, 400000.0);
        camera->viewport = Vec_CreatVec4f(iPosX, iPosY, iWindowWidth, iWindowHeight);
    }
}
