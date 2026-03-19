#include "HmsNavSvsObjectPlane.h"
#include "NavSvs/HmsNavSvsMgr.h"
#include "base/HmsImage.h"
#include "render/HmsGLStateCache.h"
#include "NavSvs/HmsNavSvsModuleMgr.h"
#include "HmsNavSvsObjectMgr.h"

using namespace HmsAviPf;

void CHmsNavSvsObjectPlane::ObjectPlaneInitVertex(int t, float xratio, float yratio, float zratio)
{
    float ratio = CHmsNavSvsMathHelper::MeterToWorldUnit(5);

    static const float coords[6][4][3] = {
        { { +1, -1, -1 }, { -1, -1, -1 }, { -1, +1, -1 }, { +1, +1, -1 } },
        { { +1, +1, -1 }, { -1, +1, -1 }, { -1, +1, +1 }, { +1, +1, +1 } },
        { { +1, -1, +1 }, { +1, -1, -1 }, { +1, +1, -1 }, { +1, +1, +1 } },
        { { -1, -1, -1 }, { -1, -1, +1 }, { -1, +1, +1 }, { -1, +1, -1 } },
        { { +1, -1, +1 }, { -1, -1, +1 }, { -1, -1, -1 }, { +1, -1, -1 } },
        { { -1, -1, +1 }, { +1, -1, +1 }, { +1, +1, +1 }, { -1, +1, +1 } }
    };

    static int index = 0;

    for (int j = 0; j < 4; ++j)
    {
        m_pVertexData[index].vertices.x = coords[t][j][0] * xratio * ratio;
        m_pVertexData[index].vertices.y = coords[t][j][1] * yratio * ratio;
        m_pVertexData[index].vertices.z = coords[t][j][2] * zratio * ratio;
        m_pVertexData[index].texCoords.u = (j == 0 || j == 3);
        m_pVertexData[index].texCoords.v = (j == 2 || j == 3);

        ++index;
    }
}

static void PushPlaneVertex(V3F_T2F *pData, float x, float y, float z, float u, float v)
{
    float ratio = CHmsNavSvsMathHelper::MeterToWorldUnit(34);
    pData->vertices.x = x * ratio;
    pData->vertices.y = y * ratio;
    pData->vertices.z = z * ratio;
    pData->texCoords.u = u;
    pData->texCoords.v = v;
}

CHmsNavSvsObjectPlane::CHmsNavSvsObjectPlane()
{
    m_objectType = NavSvsObjectType::OBJECTTYPE_PLANE;

    glGenBuffers(1, &m_nVBO);
    glGenBuffers(1, &m_nVBOIndex);

    m_nVertexCount = 8;
    m_nIndexCount = 12;
    m_pVertexData = (V3F_T2F*)malloc(m_nVertexCount * sizeof(V3F_T2F));
    m_pIndexData = (GLuint*)malloc(m_nIndexCount * sizeof(GLuint));

    {
        int index = 0;
        //纹理坐标以左下角为原点
        //后视
		PushPlaneVertex(&m_pVertexData[index++], +0.5f, -0.13f, 0.0f, 1.0f, 0.740234f);
		PushPlaneVertex(&m_pVertexData[index++], -0.5f, -0.13f, 0.0f, 0.0f, 0.740234f);
		PushPlaneVertex(&m_pVertexData[index++], -0.5f, +0.13f, 0.0f, 0.0f, 1.0f);
		PushPlaneVertex(&m_pVertexData[index++], +0.5f, +0.13f, 0.0f, 1.0f, 1.0f);
		//俯视									   						 
		PushPlaneVertex(&m_pVertexData[index++], +0.5f, 0.0f, +0.4267f, 0.75f, 0.0f);
		PushPlaneVertex(&m_pVertexData[index++], -0.5f, 0.0f, +0.4267f, 0.0f, 0.0f);
		PushPlaneVertex(&m_pVertexData[index++], -0.5f, 0.0f, -0.4267f, 0.0f, 0.64f);
		PushPlaneVertex(&m_pVertexData[index++], +0.5f, 0.0f, -0.4267f, 0.75f, 0.64f);
    }

    int index = 0;
    for (int i = 0; i < m_nIndexCount / 6; ++i)
    {
        m_pIndexData[index++] = 0 + i * 4;
        m_pIndexData[index++] = 1 + i * 4;
        m_pIndexData[index++] = 2 + i * 4;
        m_pIndexData[index++] = 0 + i * 4;
        m_pIndexData[index++] = 2 + i * 4;
        m_pIndexData[index++] = 3 + i * 4;
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_nVBO);
    glBufferData(GL_ARRAY_BUFFER, m_nVertexCount * sizeof(V3F_T2F), m_pVertexData, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_nVBOIndex);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_nIndexCount * sizeof(GLuint), m_pIndexData, GL_STATIC_DRAW);

    Image *pImage = new Image();
    pImage->initWithImageFile("res/NavSvs/y8_512.png");
    m_pTexture = new Texture2D();
    m_pTexture->initWithImage(pImage);
    HMS_SAFE_RELEASE(pImage);
}

CHmsNavSvsObjectPlane::~CHmsNavSvsObjectPlane()
{

}

    

void CHmsNavSvsObjectPlane::Update(float delta)
{

}

void CHmsNavSvsObjectPlane::OnDraw()
{
    auto viewType = CHmsNavSvsModuleMgr::GetInstance()->GetCurCameraCtrl()->GetCameraType();
    if (viewType != NavSvsCameraType::SVSCAMERATYPE_THIRD && viewType != NavSvsCameraType::SVSCAMERATYPE_OVERLOOK)
    {
        return;
    }

    Mat4d model = (*s_pSvsMgr->GetMat4dTrans()) * (*s_pSvsMgr->GetPlaneModel());
    auto pMat4dPV = s_pSvsMgr->GetMat4dPVRelative();
    model = (*pMat4dPV) * model;

    Mat4 mat;
    CHmsNavSvsMathHelper::GetMatFromMat4d(&mat, &model);

    auto pGLprogram = s_pSvsObjectMgr->m_pGlProgram;
    pGLprogram->setUniformLocationWith3f(s_pSvsObjectMgr->m_locUniformRelativeOrigin, 0, 0, 0);
    pGLprogram->setUniformLocationWithMatrix4fv(s_pSvsObjectMgr->m_locUniformMVP, mat.m, 1);
    pGLprogram->setUniformLocationWith4f(s_pSvsObjectMgr->m_locUniformColor, 1.0, 1.0, 1.0, 1.0);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    GL::blendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glBindBuffer(GL_ARRAY_BUFFER, m_nVBO);
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(V3F_T2F), (GLvoid *)offsetof(V3F_T2F, vertices));
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(V3F_T2F), (GLvoid *)offsetof(V3F_T2F, texCoords));
    glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_POSITION);
    glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_TEX_COORD);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_nVBOIndex);

    glBindTexture(GL_TEXTURE_2D, m_pTexture->getName());
    if (viewType == NavSvsCameraType::SVSCAMERATYPE_THIRD)
    {
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
    else
    {
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (const void*)(6 * sizeof(GLuint)));
    }
    
    glDisable(GL_BLEND);
}

void CHmsNavSvsObjectPlane::UpdateVertexData()
{

}
