#ifndef __DRAWABLE__
#define __DRAWABLE__

#include <vector>
#include "aabbox.h"
#include "PrimitiveSet.h"
#include "Node.h"
#include "GLHeader.h"
#include "Image.h"
#include "GLES3/gl3.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "_Vec3.h"
#include "_Vec2.h"

#ifdef __cplusplus
}
#endif

enum EN_CDrawableBind
{
    BIND_OFF = 0,
    BIND_OVERALL = 1,
    BIND_PER_PRIMITIVE_SET = 2,
    BIND_PER_VERTEX = 4
};

enum EN_CDrawableType
{
    CDrawableType_Default = 0,
    CDrawableType_Terrain,
    CDrawableType_Terrain_Border,
    CDrawableType_RunWay,
    CDrawableType_RunWayFrame,
    CDrawableType_2DTerrain,
    CDrawableType_2DBarrier,
    CDrawableType_3DBarrier,
    CDrawableType_HvWire,   //high voltage wire
    CDrawableType_HvTower,  //high voltage tower
    CDrawableType_FlightTubeWaypoint,
};// CDrawableType, *pCDrawableType;

class CDrawable
{
public:
    /*颜色绑定*/
    EN_CDrawableBind  _colorBind;
    /*法线绑定*/
    EN_CDrawableBind  _normalBind;
    /*纹理坐标绑定*/
    EN_CDrawableBind  _texcoordBind;

    /* 顶点 CVec3d   值类型Vector  */
    //pCVector _vertexs;
    std::vector<CVec3d> _vertexs2;
    //pCVector _vertexsF;//CVec3f版本的顶点坐标, shader only surpports float instead of double
    std::vector<CVec3f> _vertexsF2;

    /* 法线  CVec3f   值类型Vector    */
    //pCVector _normals;
    std::vector<CVec3f> _normals2;

    /*纹理坐标  CVec2f   值类型Vector   */
    //pCVector _texcoords;
    std::vector<CVec2f> _texcoords2;

    /* 颜色 CVec4f   值类型Vector   */
    //pCVector _colors;
    std::vector<CVec4f> _colors2;

    /* 顶点对应的海拔高度 GLfloat 值类型Vector */
    //pCVector _altitudes;
    std::vector<GLfloat> _altitudes2;

    //pCVector _indices;//indices for glDrawElements 
    std::vector<GLushort> _indices2;

    /*显示列表 Gluint   值类型Vector   */
    //pCVector _lists;
    std::vector<GLuint> _lists2;

    /*绘制集合   CPrimitiveSet   值类型Vector  */
    //pCVector _primitiveSets; 
    std::vector<CPrimitiveSet> _primitiveSets2;

    pCNode node;//本结构数据，附在此node节点上

    /*纹理*/
    pCImage texture_image;
    /*控制drawable是否绘制*/
    int is_display;
    unsigned int count;

    EN_CDrawableType drawType;
    CVec3d centerPos;
    CVec3f aftPosRel2CenterPos;//飞机相对于centerPos 的位置（相对位置xyz）

    pAabbox boundingbox;

    GLuint vao;

    int vboInitialized;//Whether the vbo buffer has been filled? 0: not filled , 1: been filled

    GLuint vtx_buffer;
    GLuint tcd_buffer;
    GLuint clr_buffer;
    GLuint nml_buffer;
    GLuint alt_buffer;//海拔高度buffer name
    GLuint idx_buffer;//glDrawElements indices

    GLfloat color[4];
    GLuint alarm_color;//告警颜色
    int alarm_state;//告警状态
    float alarm_distance;//单位米

    CDrawable(){
        _texcoordBind = BIND_OFF;
        node = 0;
        count = 0;
        drawType = CDrawableType_Default;
        centerPos = Vec_CreatVec3d(0, 0, 0);
        aftPosRel2CenterPos = Vec_CreatVec3f(0, 0, 0);
        vao = 0;
        vtx_buffer = 0;
        tcd_buffer = 0;
        clr_buffer = 0;
        nml_buffer = 0;
        alt_buffer = 0;
        idx_buffer = 0;
        color[0] = 0;
        color[1] = 0;
        color[2] = 0;
        color[3] = 0;
        alarm_distance = 0;

        _colorBind = BIND_OVERALL;
        _normalBind = BIND_OFF;
        /*默认绘制*/
        is_display = 1;
        texture_image = NULL;
        boundingbox = Aabbox_Create();
        //detailmap = 0;
        vboInitialized = 0;
        alarm_color = 0xFF0000FF;//RGBA
        alarm_state = 0;//0, no alarm
    }
#if TERRAIN3D_DETAILMAP
    int detailmap;
    int debug;
#endif
};
typedef CDrawable* pCDrawable;

pCDrawable CDrawable_Create();

//销毁Drawable 
bool CDrawable_Destory(pCDrawable pdraw);

bool CDrawable_DestoryPtr(pCDrawable *pdraw);

void CDrawable_Draw(pCDrawable p);

void DrawRectanglePlane(pCDrawable pdraw, const pCVec3d pos, const pCVec2f texCoord, const pCVec4f pColor, float* pAlt);
void DrawRectangleRingPlane(pCDrawable pdraw, const pCVec3d pos, const pCVec2f texCoord, const pCVec4f pColor, float* pAlt);
void DrawRectangleRingPlaneWithSideFrame(pCDrawable pdraw, const pCVec3d pos, const pCVec2f texCoord, const pCVec4f pColor, float* pAlt);
void drawRectangleToDrawableNode(pCDrawable geometry, CVec3d *corners, CVec4f *colors, CVec2f *texcoords);
void CDrawable_CreateGLBuffers(pCDrawable geometry);

void CDrawable_SetColorBind(pCDrawable p, EN_CDrawableBind b);

void CDrawable_SetNormalBind(pCDrawable p, EN_CDrawableBind b);

void CDrawable_registExtOpenglFunction();

void setGeometyVbo(pCDrawable pDrawable);

void DrawableNodeCreateVBO(pCDrawable geometry);

#endif
