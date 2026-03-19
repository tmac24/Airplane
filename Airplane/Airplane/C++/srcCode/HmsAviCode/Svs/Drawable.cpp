#include <initializer_list>
#include <chrono>
#include <stdio.h>
#include "HmsLog.h"

extern bool g_supportVAO;

#include "Drawable.h"
#include "AltManager.h"
#include "MathCommon.h"
#include "FPPManipulator.h"
#include "Global.h"
#include "Log.h"
#include "GLHeader.h"
#include "TextureMgr.h"
#include "TerrainHead.h"


#define USE_VA 1

#if 0//jiangyan: 无用，注释掉
#ifndef M96_PLATFORM
PFNGLBINDBUFFERPROC    glBindbuffer;
PFNGLGENBUFFERSPROC    glGenBuffers;
PFNGLBUFFERDATAPROC	   glBufferData;
PFNGLDELETEBUFFERSPROC glDeleteBuffer;
#endif
#endif

#if TERRAIN3D_DETAILMAP
PFNGLACTIVETEXTUREARBPROC glActiveTexture;
PFNGLMULTITEXCOORD2FVARBPROC glMultiTexCoord2fv;

void CDrawable_registExtOpenglFunction()
{
    glActiveTexture =  (PFNGLBLENDEQUATIONPROC)wglGetProcAddress("glActiveTextureARB");
    glMultiTexCoord2fv = (PFNGLMULTITEXCOORD2FVPROC)wglGetProcAddress("glMultiTexCoord2fvARB");
}
#else


void CDrawable_registExtOpenglFunction()
{

}
#endif

#pragma pack(1)
typedef struct stVec3f
{
    float x, y, z;
}vec3f;
#pragma pack()

extern GLuint shaderProgramOnDraw;

//extern CMatrix viewmatrix;
//extern CMatrixd matrixProjectd;

pCDrawable CDrawable_Create()
{
    //pCDrawable ptr = (pCDrawable)malloc(sizeof(CDrawable));
    //if (ptr == NULL)
    //{
    //    printf("pCDrawable CDrawable_Creat() malloc failed!\n");
    //    exit(1);
    //}
    //memset(ptr, 0, sizeof(CDrawable));
    pCDrawable ptr = NULL;
    try
    {
        ptr = new CDrawable;
    }
    catch (std::bad_alloc& e1)
    {
        HMSLOG("pCDrawable CDrawable_Create() new failed!, %s\n", e1.what());
        return NULL;
    }

    //ptr->_vertexs = CVector_Create(sizeof(CVec3d));
    //ptr->_vertexsF = CVector_Create(sizeof(CVec3f));
    //ptr->_normals = CVector_Create(sizeof(CVec3f));
    //ptr->_texcoords = CVector_Create(sizeof(CVec2f));
    //ptr->_colors = CVector_Create(sizeof(CVec4f));
    //ptr->_altitudes = CVector_Create(sizeof(GLfloat));
    //ptr->_indices = CVector_Create(sizeof(GLushort));
    //ptr->_lists = CVector_Create(sizeof(GLuint));
    //ptr->_primitiveSets = CVector_Create(sizeof(CPrimitiveSet));

    return ptr;
}

bool CDrawable_Destory(pCDrawable pdraw)
{
    if (pdraw)
    {
        // 		if ( p->drawType == CDrawableType_2DBarrier)
        // 		{
        // 			printf("删除2D障碍物\n");
        // 		}

        //LogWrite(LOG_LEVEL_DEBUG_FP,"/*清空  %s 显示列表*/ start : %s,%d\n", p->node->name,__FILE__, __LINE__);
        /*清空显示列表*/
        //CDrawable_DeleteLists(pdraw);
        //pdraw->_lists = NULL;
        //CVector_Destroy(pdraw->_lists);
        //pdraw->_lists = NULL;
        /*释放顶点*/
        //LogWrite(LOG_LEVEL_DEBUG_FP,"	/*释放顶点*/   %s  start : %s,%d\n", p->node->name, __FILE__, __LINE__);
        //CVector_Destroy(pdraw->_vertexs);
        //pdraw->_vertexs = NULL;
        //CVector_Destroy(pdraw->_vertexsF);
        //pdraw->_vertexsF = NULL;
        /*释放法线*/
        //LogWrite(LOG_LEVEL_DEBUG_FP,"	/*释放法线*/   %s    start : %s,%d\n", p->node->name, __FILE__, __LINE__);
        //CVector_Destroy(pdraw->_normals);
        //pdraw->_normals = NULL;
        /*释放文理坐标*/
        //LogWrite(LOG_LEVEL_DEBUG_FP,"	/*释放文理坐标*/   %s  start : %s,%d\n", p->node->name, __FILE__, __LINE__);
        //CVector_Destroy(pdraw->_texcoords);
        //pdraw->_texcoords = NULL;
        /*释放颜色*/
        //LogWrite(LOG_LEVEL_DEBUG_FP,"	/*释放颜色*/   %s    start : %s,%d\n", p->node->name, __FILE__, __LINE__);
        //CVector_Destroy(pdraw->_colors);
        //pdraw->_colors = NULL;
        //CVector_Destroy(pdraw->_altitudes);
        //pdraw->_altitudes = NULL;
        //CVector_Destroy(pdraw->_indices);
        //pdraw->_indices = NULL;
        /*释放图元*/
        //LogWrite(LOG_LEVEL_DEBUG_FP,"	/*释放图元*/   %s    start : %s,%d\n", p->node->name, __FILE__, __LINE__);
        //CVector_Destroy(pdraw->_primitiveSets);
        //pdraw->_primitiveSets = NULL;
        //LogWrite(LOG_LEVEL_DEBUG_FP,"/*清空显示列表*/ over : %s,%d\n", __FILE__, __LINE__);
        Aabbox_Destory(pdraw->boundingbox);

        //free GL buffers
        glDeleteBuffers(1, &pdraw->vtx_buffer);
        glDeleteBuffers(1, &pdraw->tcd_buffer);
        glDeleteBuffers(1, &pdraw->clr_buffer);
        glDeleteBuffers(1, &pdraw->nml_buffer);
        glDeleteBuffers(1, &pdraw->alt_buffer);//海拔高度buffer name
        glDeleteBuffers(1, &pdraw->idx_buffer);
        glDeleteVertexArrays(1, &pdraw->vao);

        /*释放结构体*/
        CImage_Destory(&(pdraw->texture_image));
        pdraw->texture_image = NULL;
        //free(pdraw);
        delete pdraw;
    }
    return true;
}

bool CDrawable_DestoryPtr(pCDrawable *pdraw)
{
    pCDrawable p = *pdraw;

    if (p != NULL)
    {
        //p->_lists = NULL;

        //LogWrite(LOG_LEVEL_DEBUG_FP,"/*清空  %s 显示列表*/ start : %s,%d\n", p->node->name,__FILE__, __LINE__);
        //p->_lists = NULL;
        /*释放顶点*/
        //LogWrite(LOG_LEVEL_DEBUG_FP,"	/*释放顶点*/   %s  start : %s,%d\n", p->node->name, __FILE__, __LINE__);
        //CVector_Destroy(p->_vertexs);
        //p->_vertexs = NULL;
        /*释放法线*/
        //LogWrite(LOG_LEVEL_DEBUG_FP,"	/*释放法线*/   %s    start : %s,%d\n", p->node->name, __FILE__, __LINE__);
        //CVector_Destroy(p->_normals); p->_normals = NULL;
        /*释放文理坐标*/
        //LogWrite(LOG_LEVEL_DEBUG_FP,"	/*释放文理坐标*/   %s  start : %s,%d\n", p->node->name, __FILE__, __LINE__);
        //CVector_Destroy(p->_texcoords); p->_texcoords = NULL;
        /*释放颜色*/
        //LogWrite(LOG_LEVEL_DEBUG_FP,"	/*释放颜色*/   %s    start : %s,%d\n", p->node->name, __FILE__, __LINE__);
        //CVector_Destroy(p->_colors); p->_colors = NULL;
        /*释放图元*/
        //LogWrite(LOG_LEVEL_DEBUG_FP,"	/*释放图元*/   %s    start : %s,%d\n", p->node->name, __FILE__, __LINE__);
        //CVector_Destroy(p->_primitiveSets); p->_primitiveSets = NULL;
        //LogWrite(LOG_LEVEL_DEBUG_FP,"/*清空显示列表*/ over : %s,%d\n", __FILE__, __LINE__);

        Aabbox_Destory(p->boundingbox);
        /*释放结构体*/
        CImage_Destory(&(p->texture_image));
        p->texture_image = NULL;
        //free(p);
        delete p;
        p = NULL;
        *pdraw = 0;
        return true;
    }
    return false;
}

//************************************
// Method:    DrawRectanglePlane
// FullName:  DrawRectanglePlane
// Access:    public 
// Desc:      
// Returns:   void
// Qualifier:
// Parameter: const pCDrawable pdraw
// Parameter: const pCVec3d pos
// Parameter: const pCVec2f texCoord
// Parameter: const pCVec4f pColor
// Parameter: GLfloat * pAlt
// Author:    yan.jiang
//************************************
void DrawRectanglePlane(pCDrawable pdraw, const pCVec3d pos, const pCVec2f texCoord, const pCVec4f pColor, float* pAlt)
{
    CVec3f vertexTempf;

    for (auto i : { 0, 1, 2, 3 })
    {
        //CVector_Pushback(pdraw->_vertexs, &pos[i]);
        pdraw->_vertexs2.push_back(pos[i]);
        vertexTempf = Vec_pVec3dToVec3f(&pos[i]);
        //CVector_Pushback(pdraw->_vertexsF, &vertexTempf);
        pdraw->_vertexsF2.push_back(vertexTempf);
        //CVector_Pushback(pdraw->_texcoords, &texCoord[i]);
        //CVector_Pushback(pdraw->_colors, &pColor[i]);
        //CVector_Pushback(pdraw->_altitudes, &pAlt[i]);
        pdraw->_texcoords2.push_back(texCoord[i]);
        pdraw->_colors2.push_back(pColor[i]);
        pdraw->_altitudes2.push_back(pAlt[i]);
    }

    auto last = static_cast<unsigned short>(pdraw->_indices2.size() * 4 / 6);
    unsigned short RecIdxPatern[6] = {
        static_cast<unsigned short>(last + 0),
        static_cast<unsigned short>(last + 1),
        static_cast<unsigned short>(last + 2),
        static_cast<unsigned short>(last + 0),
        static_cast<unsigned short>(last + 2),
        static_cast<unsigned short>(last + 3) };

    for (unsigned short i : RecIdxPatern)
    {
        //CVector_Pushback(pdraw->_indices, &i);
        pdraw->_indices2.push_back(i);
    }
}

//************************************
// Method:    DrawRectangleRingPlane
// FullName:  DrawRectangleRingPlane
// Access:    public 
// Desc:      绘制矩形环
// Returns:   void
// Qualifier:
// Parameter: pCDrawable pdraw
// Parameter: const pCVec3d pos
// Parameter: const pCVec2f texCoord
// Parameter: const pCVec4f pColor
// Parameter: float * pAlt
// Author:    yan.jiang
//************************************
void DrawRectangleRingPlane(pCDrawable pdraw, const pCVec3d pos, const pCVec2f texCoord, const pCVec4f pColor, float* pAlt)
{
    /*
    3-----------------2
    | \             / |
    |  7-----------6  |
    |  |           |  |
    |  |           |  |
    |  |           |  |
    |  4-----------5  |
    | /             \ |
    0-----------------1
    */
    CVec3f vertexTempf;
    CPrimitiveSet primitiveSet;

    for (auto i : { 0, 1, 2, 3, 4, 5, 6, 7 })
    {
        //CVector_Pushback(pdraw->_vertexs, &pos[i]);
        pdraw->_vertexs2.push_back(pos[i]);
        vertexTempf = Vec_pVec3dToVec3f(&pos[i]);
        //CVector_Pushback(pdraw->_vertexsF, &vertexTempf);
        pdraw->_vertexsF2.push_back(vertexTempf);
        //CVector_Pushback(pdraw->_texcoords, &texCoord[i]);
        //CVector_Pushback(pdraw->_colors, &pColor[i]);
        //CVector_Pushback(pdraw->_altitudes, &pAlt[i]);
        pdraw->_texcoords2.push_back(texCoord[i]);
        pdraw->_colors2.push_back(pColor[i]);
        pdraw->_altitudes2.push_back(pAlt[i]);
    }

    auto lastInd = pdraw->_indices2.size();
    auto last = static_cast<unsigned short>(lastInd * 8 / 12);

    unsigned short RecIdxPatern[6];
    RecIdxPatern[0] = static_cast<unsigned short>(last + 4);
    RecIdxPatern[1] = static_cast<unsigned short>(last + 7);
    RecIdxPatern[2] = static_cast<unsigned short>(last + 3);
    RecIdxPatern[3] = static_cast<unsigned short>(last + 0);
    RecIdxPatern[4] = static_cast<unsigned short>(last + 1);
    RecIdxPatern[5] = static_cast<unsigned short>(last + 5);

    for (unsigned short i : RecIdxPatern)
    {
        //CVector_Pushback(pdraw->_indices, &i);
        pdraw->_indices2.push_back(i);
    }
    //add primitiveSet
    primitiveSet = CPrimitiveSet_CreatDefaultVisible(GL_TRIANGLE_FAN, lastInd, 6); primitiveSet.lineWidth = 7896;
    //CVector_Pushback(pdraw->_primitiveSets, &primitiveSet);
    pdraw->_primitiveSets2.push_back(primitiveSet);

    //add primset 
    RecIdxPatern[0] = static_cast<unsigned short>(last + 6);
    RecIdxPatern[1] = static_cast<unsigned short>(last + 5);
    RecIdxPatern[2] = static_cast<unsigned short>(last + 1);
    RecIdxPatern[3] = static_cast<unsigned short>(last + 2);
    RecIdxPatern[4] = static_cast<unsigned short>(last + 3);
    RecIdxPatern[5] = static_cast<unsigned short>(last + 7);

    for (unsigned short i : RecIdxPatern)
    {
        //CVector_Pushback(pdraw->_indices, &i);
        pdraw->_indices2.push_back(i);
    }
    //add primitiveSet
    primitiveSet = CPrimitiveSet_CreatDefaultVisible(GL_TRIANGLE_FAN, lastInd + 6, 6); primitiveSet.lineWidth = 7897;
    //CVector_Pushback(pdraw->_primitiveSets, &primitiveSet);
    pdraw->_primitiveSets2.push_back(primitiveSet);
}

//************************************
// Method:    DrawRectangleRingPlaneWithSideFrame2
// FullName:  DrawRectangleRingPlaneWithSideFrame2
// Access:    public 
// Desc:      
// Returns:   void
// Qualifier:
// Parameter: pCDrawable pdraw
// Parameter: const pCVec3 pos
// Parameter: const pCVec2f texCoord
// Parameter: const pCVec4f pColor
// Parameter: float * pAlt
// Author:    yan.jiang
//************************************
void DrawRectangleRingPlaneWithSideFrame(pCDrawable pdraw, const pCVec3d pos, const pCVec2f texCoord, const pCVec4f pColor, float* pAlt)
{
    //Ring Plane with Side Frame 
    /*
    level 1:
    3-----------------2
    | \             / |
    |  7-----------6  |
    |  |           |  |
    |  |           |  |
    |  |           |  |
    |  4-----------5  |
    | /             \ |
    0-----------------1

    level 2:
    11---------------10
    |                 |
    |                 |
    |                 |
    |                 |
    |                 |
    |                 |
    |                 |
    8-----------------9
    */
    CVec3f vertexTempf;
    CPrimitiveSet primitiveSet;

    for (auto i : { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 })
    {
        //CVector_Pushback(pdraw->_vertexs, &pos[i]);
        pdraw->_vertexs2.push_back(pos[i]);
        vertexTempf = Vec_pVec3dToVec3f(&pos[i]);
        //CVector_Pushback(pdraw->_vertexsF, &vertexTempf);
        pdraw->_vertexsF2.push_back(vertexTempf);
        //CVector_Pushback(pdraw->_texcoords, &texCoord[i]);
        //CVector_Pushback(pdraw->_colors, &pColor[i]);
        //CVector_Pushback(pdraw->_altitudes, &pAlt[i]);
        pdraw->_texcoords2.push_back(texCoord[i]);
        pdraw->_colors2.push_back(pColor[i]);
        pdraw->_altitudes2.push_back(pAlt[i]);
    }

    auto lastInd = pdraw->_indices2.size();
    auto last = static_cast<unsigned short>(lastInd * 12 / 24);

    unsigned short RecIdxPatern[6];

    RecIdxPatern[0] = static_cast<unsigned short>(last + 4);
    RecIdxPatern[1] = static_cast<unsigned short>(last + 7);
    RecIdxPatern[2] = static_cast<unsigned short>(last + 3);
    RecIdxPatern[3] = static_cast<unsigned short>(last + 0);
    RecIdxPatern[4] = static_cast<unsigned short>(last + 1);
    RecIdxPatern[5] = static_cast<unsigned short>(last + 5);

    for (unsigned short i : RecIdxPatern)
    {
        //CVector_Pushback(pdraw->_indices, &i);
        pdraw->_indices2.push_back(i);
    }
    //add primitiveSet
    primitiveSet = CPrimitiveSet_CreatDefaultVisible(GL_TRIANGLE_FAN, lastInd, 6); primitiveSet.lineWidth = 1111;
    //CVector_Pushback(pdraw->_primitiveSets, &primitiveSet);
    pdraw->_primitiveSets2.push_back(primitiveSet);

    //add primset 
    RecIdxPatern[0] = static_cast<unsigned short>(last + 6);
    RecIdxPatern[1] = static_cast<unsigned short>(last + 5);
    RecIdxPatern[2] = static_cast<unsigned short>(last + 1);
    RecIdxPatern[3] = static_cast<unsigned short>(last + 2);
    RecIdxPatern[4] = static_cast<unsigned short>(last + 3);
    RecIdxPatern[5] = static_cast<unsigned short>(last + 7);

    for (unsigned short i : RecIdxPatern)
    {
        //CVector_Pushback(pdraw->_indices, &i);
        pdraw->_indices2.push_back(i);
    }
    //add primitiveSet
    primitiveSet = CPrimitiveSet_CreatDefaultVisible(GL_TRIANGLE_FAN, lastInd + 6, 6); primitiveSet.lineWidth = 1112;
    //CVector_Pushback(pdraw->_primitiveSets, &primitiveSet);
    pdraw->_primitiveSets2.push_back(primitiveSet);

    //add primset side frame up&right
    RecIdxPatern[0] = static_cast<unsigned short>(last + 0);
    RecIdxPatern[1] = static_cast<unsigned short>(last + 3);
    RecIdxPatern[2] = static_cast<unsigned short>(last + 11);
    RecIdxPatern[3] = static_cast<unsigned short>(last + 8);
    RecIdxPatern[4] = static_cast<unsigned short>(last + 9);
    RecIdxPatern[5] = static_cast<unsigned short>(last + 1);

    for (unsigned short i : RecIdxPatern)
    {
        //CVector_Pushback(pdraw->_indices, &i);
        pdraw->_indices2.push_back(i);
    }
    //add primitiveSet
    primitiveSet = CPrimitiveSet_CreatDefaultVisible(GL_TRIANGLE_FAN, lastInd + 6 * 2, 6); primitiveSet.lineWidth = 1113;
    //CVector_Pushback(pdraw->_primitiveSets, &primitiveSet);
    pdraw->_primitiveSets2.push_back(primitiveSet);

    //add primset side frame down&left
    RecIdxPatern[0] = static_cast<unsigned short>(last + 2);
    RecIdxPatern[1] = static_cast<unsigned short>(last + 1);
    RecIdxPatern[2] = static_cast<unsigned short>(last + 9);
    RecIdxPatern[3] = static_cast<unsigned short>(last + 10);
    RecIdxPatern[4] = static_cast<unsigned short>(last + 11);
    RecIdxPatern[5] = static_cast<unsigned short>(last + 3);

    for (unsigned short i : RecIdxPatern)
    {
        //CVector_Pushback(pdraw->_indices, &i);
        pdraw->_indices2.push_back(i);
    }
    //add primitiveSet
    primitiveSet = CPrimitiveSet_CreatDefaultVisible(GL_TRIANGLE_FAN, lastInd + 6 * 3, 6); primitiveSet.lineWidth = 1114;
    //CVector_Pushback(pdraw->_primitiveSets, &primitiveSet);
    pdraw->_primitiveSets2.push_back(primitiveSet);
}

void drawRectangleToDrawableNode(pCDrawable geometry, CVec3d *corners, CVec4f *colors, CVec2f *texcoords)
{
    float alts[4] = { 0, 0, 0, 0 };
    DrawRectanglePlane(geometry, corners, texcoords, colors, alts);

    CPrimitiveSet primitiveSet;
    primitiveSet.beg = geometry->count;//jy:geometry->count represent the total vertex count of the geometry(drawable node);
    primitiveSet.count = 6;//two triangles
    primitiveSet.type = GL_TRIANGLES;
    //CVector_Pushback(geometry->_primitiveSets, &primitiveSet);
    geometry->_primitiveSets2.push_back(primitiveSet);
    geometry->count += primitiveSet.count;
}


//************************************
// Method:    CDrawable_CreateGLBuffers
// FullName:  CDrawable_CreateGLBuffers
// Access:    public 
// Desc:      
// Returns:   void
// Qualifier:
// Parameter: pCDrawable geometry
// Author:    yan.jiang
//************************************
void CDrawable_CreateGLBuffers(pCDrawable geometry)
{
    switch (geometry->drawType)
    {
    case CDrawableType_Terrain:
    case CDrawableType_Terrain_Border:

        if (g_supportVAO)
        {
            glGenVertexArrays(1, &geometry->vao);
            glBindVertexArray(geometry->vao);
        }

        glGenBuffers(1, &geometry->vtx_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, geometry->vtx_buffer);
        //glBufferData(GL_ARRAY_BUFFER, sizeof(CVec3f)* geometry->_vertexsF->size, geometry->_vertexsF->data, GL_STATIC_DRAW);
        glBufferData(GL_ARRAY_BUFFER, sizeof(CVec3f)* geometry->_vertexsF2.size(), geometry->_vertexsF2.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &geometry->clr_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, geometry->clr_buffer);
        //glBufferData(GL_ARRAY_BUFFER, sizeof(CVec4f)*geometry->_colors->size, geometry->_colors->data, GL_STATIC_DRAW);
        glBufferData(GL_ARRAY_BUFFER, sizeof(CVec4f)*geometry->_colors2.size(), geometry->_colors2.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &geometry->tcd_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, geometry->tcd_buffer);
        //glBufferData(GL_ARRAY_BUFFER, sizeof(CVec2f)*geometry->_texcoords->size, geometry->_texcoords->data, GL_STATIC_DRAW);
        glBufferData(GL_ARRAY_BUFFER, sizeof(CVec2f)*geometry->_texcoords2.size(), geometry->_texcoords2.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &geometry->alt_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, geometry->alt_buffer);
        //glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*geometry->_altitudes->size, geometry->_altitudes->data, GL_STATIC_DRAW);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*geometry->_altitudes2.size(), geometry->_altitudes2.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &geometry->idx_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry->idx_buffer);
        //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*geometry->_indices->size, geometry->_indices->data, GL_STATIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*geometry->_indices2.size(), geometry->_indices2.data(), GL_STATIC_DRAW);

        if (g_supportVAO)
        {
            setGeometyVbo(geometry);
            glBindVertexArray(0);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        break;
    default:
        break;
    }

    geometry->texture_image = TextureMgr_GetImage(TextureType_3DTerrain_NEW);
    geometry->vboInitialized = 1;
}

//************************************
// Method:    CDrawable_DrawWithVBO
// FullName:  CDrawable_DrawWithVBO
// Access:    public 
// Desc:      use vbo to draw
// Returns:   void
// Qualifier:
// Parameter: pCDrawable pdraw
//************************************
void CDrawable_DrawWithVBO(pCDrawable pdraw)
{
    if (g_supportVAO)
    {
        glBindVertexArray(pdraw->vao);
    }
    else
    {
        setGeometyVbo(pdraw);
    }

    //The position of the eye at the moment.  longitude latitude altitude
    GLuint EyePosID = glGetUniformLocation(shaderProgramOnDraw, "EyePosition_worldspace");
    if (EyePosID != -1)
    {
        CVec3d vLonLatAlt = GetGlobal()->vLonLatAlt;
        glUniform3f(EyePosID, vLonLatAlt._v[0], vLonLatAlt._v[1], vLonLatAlt._v[2]);
    }

    if (pdraw->texture_image && pdraw->texture_image->m_ID != 0)
    {
        GLuint samplerLoc = glGetUniformLocation(shaderProgramOnDraw, "s_texture");
        if (samplerLoc != -1)
        {
            // Bind the texture
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, pdraw->texture_image->m_ID);
            // Set the sampler texture unit to 0
            glUniform1i(samplerLoc, 0);

            GLint texture_base_fromat = glGetUniformLocation(shaderProgramOnDraw,
                "u_texture_base_format");
            glUniform1i(texture_base_fromat, pdraw->texture_image->m_iType);
        }
    }
    if (pdraw->alarm_state == 1)
    {
        GLuint AlarmID = glGetUniformLocation(shaderProgramOnDraw, "u_alarm");
        if (AlarmID != -1)
        {
            glUniform1i(AlarmID, 1);
        }
        //时间
        GLuint TimeID = glGetUniformLocation(shaderProgramOnDraw, "u_time");
        if (TimeID != -1)
        {
            auto time_now = std::chrono::system_clock::now();
            auto duration_in_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_now.time_since_epoch());
            double time_in_sec = duration_in_ms.count() / 1000.0;
            float tm = (float)(time_in_sec - (int)time_in_sec);
            glUniform1f(TimeID, tm);
        }
    }
    else
    {
        GLuint AlarmID = glGetUniformLocation(shaderProgramOnDraw, "u_alarm");
        if (AlarmID != -1)
        {
            glUniform1i(AlarmID, 0);
        }
    }

    if (pdraw->drawType == EN_CDrawableType::CDrawableType_HvWire)
    {
        GLuint AftRelPos2WireCenterID = glGetUniformLocation(shaderProgramOnDraw, "u_AftPosition_basedOnWireCenterPos");
        if (AftRelPos2WireCenterID != -1)
        {
            glUniform3fv(AftRelPos2WireCenterID, 1, &(pdraw->aftPosRel2CenterPos._v[0]));
        }
        GLuint AlarmDistanceID = glGetUniformLocation(shaderProgramOnDraw, "u_alarmDistance");
        if (AlarmDistanceID != -1)
        {
            glUniform1f(AlarmDistanceID, pdraw->alarm_distance);
        }
        //时间
        GLuint TimeID = glGetUniformLocation(shaderProgramOnDraw, "u_time");
        if (TimeID != -1)
        {
            auto time_now = std::chrono::system_clock::now();
            auto duration_in_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_now.time_since_epoch());
            double time_in_sec = duration_in_ms.count() / 1000.0;
            float tm = (float)(time_in_sec - (int)time_in_sec);
            glUniform1f(TimeID, tm);
        }
    }

    if (pdraw->drawType == EN_CDrawableType::CDrawableType_FlightTubeWaypoint)
    {
        GLint texture_base_fromat = glGetUniformLocation(shaderProgramOnDraw,
            "u_texture_base_format");
        glUniform1i(texture_base_fromat, pdraw->texture_image->m_iType);
        GLuint tubecolor = glGetUniformLocation(shaderProgramOnDraw, "u_color2");
        if (tubecolor != -1)
        {
            glUniform4fv(tubecolor, 1, &(pdraw->color[0]));
        }
    }

    unsigned int beg;
    unsigned int count;
    unsigned int drawtype;

    auto sz = pdraw->_primitiveSets2.size();
    //for (unsigned int i = 0; i < sz; ++i)
    //{
    //    auto kkk = pdraw->_primitiveSets2[i];
    //    printf("%d\n",kkk.beg);
    //}

    //for (auto& curPrimit : pdraw->_primitiveSets2)
    for (unsigned int i = 0; i < sz; ++i)
    {
        auto curPrimit = pdraw->_primitiveSets2[i];
        if (curPrimit.bVisable == 0)
        {
            continue;
        }

        beg = curPrimit.beg * sizeof(unsigned short);
        count = curPrimit.count;
        drawtype = curPrimit.type;

        /*begin to draw. one primitive one drawCall */
        if (GetGlobal()->drawMode == 1)
        {
            glDrawElements(GL_LINE_STRIP, count, GL_UNSIGNED_SHORT, (void*)beg);
        }
        else
        {
            glDrawElements(drawtype, count, GL_UNSIGNED_SHORT, (void*)beg);
        }
    }

    if (g_supportVAO)
    {
        glBindVertexArray(0);
    }
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}

//************************************
// Method:    CDrawable_DrawTerrainWithVBO
// FullName:  CDrawable_DrawTerrainWithVBO
// Access:    public 
// Desc:      use vbo to draw
// Returns:   void
// Qualifier:
// Parameter: pCDrawable pdraw
//************************************
void CDrawable_DrawTerrainWithVBO(pCDrawable pdraw)
{
    if (pdraw->drawType != CDrawableType_Terrain
        &&pdraw->drawType != CDrawableType_Terrain_Border)
    {
        return;
    }

    //unsigned int _vertexsSize = pdraw->_vertexs->size;/*顶点个数*/
    unsigned int _vertexsSize = pdraw->_vertexs2.size();/*顶点个数*/

    if (g_supportVAO)
    {
        glBindVertexArray(pdraw->vao);
    }
    else
    {
        setGeometyVbo(pdraw);
    }

    //The position of the eye at the moment.  longitude latitude altitude
    GLuint EyePosID = glGetUniformLocation(shaderProgramOnDraw, "EyePosition_worldspace");
    if (EyePosID != -1)
    {
        CVec3d vLonLatAlt = GetGlobal()->vLonLatAlt;
        glUniform4f(EyePosID,
            vLonLatAlt._v[0],
            vLonLatAlt._v[1],
            vLonLatAlt._v[2],
            (float)GetGlobal()->bIsAnyRunwayNearby
            );
    }

    if (pdraw->texture_image && pdraw->texture_image->m_ID != 0)
    {
        GLuint samplerLoc = glGetUniformLocation(shaderProgramOnDraw, "s_texture");
        if (samplerLoc != -1)
        {
            // Bind the texture
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, pdraw->texture_image->m_ID);
            // Set the sampler texture unit to 0
            glUniform1i(samplerLoc, 0);
        }
    }

    unsigned int count;
    unsigned int curPrimitNum = 0;
    unsigned int primitiveSetsNumber = 0;

    auto sz = pdraw->_primitiveSets2.size();

    //for (auto& curPrimit : pdraw->_primitiveSets2)
    for (unsigned int i = 0; i < sz; ++i)
    {
        auto curPrimit = pdraw->_primitiveSets2[i];
        count = curPrimit.count;
        curPrimitNum += 1;

        /*begin to draw. one primitive one drawCall */
        if (GetGlobal()->drawMode == 1)
        {
            glDrawElements(GL_LINE_STRIP, count, GL_UNSIGNED_SHORT, 0);
        }
        else if (GetGlobal()->drawMode == 2)
        {
            glDrawElements(GL_LINE_LOOP, count, GL_UNSIGNED_SHORT, 0);
        }
        else
        {
            glDrawElements(curPrimit.type, count, GL_UNSIGNED_SHORT, 0);
        }
        primitiveSetsNumber++;
    }

    if (g_supportVAO)
    {
        glBindVertexArray(0);
    }
    else
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    //	glDisableVertexAttribArray(0);
    //	glDisableVertexAttribArray(1);
    //	glDisableVertexAttribArray(2);
}


void CDrawable_Draw(pCDrawable pdraw)
{
    if (pdraw == nullptr)
    {
        return;
    }

    if (pdraw->drawType == CDrawableType_RunWayFrame)
    {
        if (GetGlobal()->m_showRuwFrame)
        {
            pdraw->is_display = 1;
        }
        else
        {
            pdraw->is_display = 0;
        }
    }
    /*没得绘制啊*/
    if (!pdraw->is_display)
    {
        return;
    }

    if (pdraw->_primitiveSets2.size() <= 0)
    {
        return;
    }

    switch (pdraw->drawType)
    {
    case CDrawableType_Terrain:
        CDrawable_DrawTerrainWithVBO(pdraw);
        break;
    case CDrawableType_Terrain_Border:
        CDrawable_DrawTerrainWithVBO(pdraw);
        break;
    case CDrawableType_RunWay:
        CDrawable_DrawWithVBO(pdraw);
        break;
    case CDrawableType_3DBarrier:
        CDrawable_DrawWithVBO(pdraw);
        break;
    case CDrawableType_FlightTubeWaypoint:
        CDrawable_DrawWithVBO(pdraw);
        break;
    case CDrawableType_HvWire:
        CDrawable_DrawWithVBO(pdraw);
        break;
    case CDrawableType_HvTower:
        CDrawable_DrawWithVBO(pdraw);
        break;
    case CDrawableType_Default:
        CDrawable_DrawWithVBO(pdraw);
        break;
    default:
        //assert(false);
        break;
    }
}

void CDrawable_SetColorBind(pCDrawable p, EN_CDrawableBind b)
{
    p->_colorBind = b;
}

void CDrawable_SetNormalBind(pCDrawable p, EN_CDrawableBind b)
{
    p->_normalBind = b;
}

void setGeometyVbo(pCDrawable pdraw)
{
    //	GLuint LocVertex = glGetAttribLocation(shaderprogram, "a_vertexPosition_modelspace");
    //	GLuint LocColor = glGetAttribLocation(shaderprogram, "a_color");
    //	GLuint LocTexcoord = glGetAttribLocation(shaderprogram, "a_texCoord");
    //	GLuint LocAltitude = glGetAttribLocation(shaderprogram, "a_altitude_float");

    GLuint LocVertex = 0;
    GLuint LocColor = 1;
    GLuint LocTexcoord = 2;
    GLuint LocAltitude = 3;

    glEnableVertexAttribArray(LocVertex);
    glBindBuffer(GL_ARRAY_BUFFER, pdraw->vtx_buffer);
    glVertexAttribPointer(
        LocVertex,          // attribute
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
        );

    glEnableVertexAttribArray(LocColor);
    glBindBuffer(GL_ARRAY_BUFFER, pdraw->clr_buffer);
    glVertexAttribPointer(
        LocColor,           // attribute
        4,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
        );

    glEnableVertexAttribArray(LocTexcoord);
    glBindBuffer(GL_ARRAY_BUFFER, pdraw->tcd_buffer);
    glVertexAttribPointer(
        LocTexcoord,        // attribute
        2,                  // size
        GL_FLOAT,           // type
        GL_FALSE,
        0,                  // stride
        (void*)0            // array buffer offset
        );

    glEnableVertexAttribArray(LocAltitude);
    glBindBuffer(GL_ARRAY_BUFFER, pdraw->alt_buffer);
    glVertexAttribPointer(
        LocAltitude,        // attribute
        1,                  // size
        GL_FLOAT,           // type
        GL_FALSE,
        0,                  // stride
        (void*)0            // array buffer offset
        );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pdraw->idx_buffer);
}


//************************************
// Method:    DrawableNodeCreateVBO
// FullName:  DrawableNodeCreateVBO
// Access:    public 
// Desc:      前提是 _vertexsF, _colors,_texcoords,_altitudes,_altitudes,_indices 都是准备好的状态
// Returns:   void
// Qualifier:
// Parameter: pCDrawable geometry
// Author:    yan.jiang
//************************************
void DrawableNodeCreateVBO(pCDrawable geometry)
{
    if (g_supportVAO)
    {
        glGenVertexArrays(1, &geometry->vao);
        glBindVertexArray(geometry->vao);
    }

    //create vbo
    glGenBuffers(1, &geometry->vtx_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, geometry->vtx_buffer);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(CVec3f)* geometry->_vertexsF->size, geometry->_vertexsF->data, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CVec3f)* geometry->_vertexsF2.size(), geometry->_vertexsF2.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &geometry->clr_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, geometry->clr_buffer);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(CVec4f)*geometry->_colors->size, geometry->_colors->data, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CVec4f)*geometry->_colors2.size(), geometry->_colors2.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &geometry->tcd_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, geometry->tcd_buffer);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(CVec2f)*geometry->_texcoords->size, geometry->_texcoords->data, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CVec2f)*geometry->_texcoords2.size(), geometry->_texcoords2.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &geometry->alt_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, geometry->alt_buffer);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*geometry->_altitudes->size, geometry->_altitudes->data, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*geometry->_altitudes2.size(), geometry->_altitudes2.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &geometry->idx_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry->idx_buffer);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*geometry->_indices->size, geometry->_indices->data, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*geometry->_indices2.size(), geometry->_indices2.data(), GL_STATIC_DRAW);

    if (g_supportVAO)
    {
        setGeometyVbo(geometry);
        glBindVertexArray(0);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
