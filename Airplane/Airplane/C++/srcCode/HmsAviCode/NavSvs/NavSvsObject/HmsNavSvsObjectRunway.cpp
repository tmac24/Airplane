#include "HmsNavSvsObjectRunway.h"
#include "NavSvs/HmsNavSvsMgr.h"
#include "base/HmsImage.h"
#include "render/HmsGLStateCache.h"
#include "Calculate/CalculateZs.h"
#include "HmsNavSvsObjectMgr.h"

CHmsNavSvsObjectRunway::CHmsNavSvsObjectRunway()
{
    m_objectType = NavSvsObjectType::OBJECTTYPE_RUNWAY;

    glGenBuffers(1, &m_nVBO);
    glGenBuffers(1, &m_nVBOIndex);

    m_nVertexCount = 0;
    m_nIndexCount = 0;
    m_pVertexData = NULL;
    m_pIndexData = NULL;

    m_nUnitVertexCount = 4;
    m_pUnitVertexData = (V3F_T2F*)malloc(m_nUnitVertexCount * sizeof(V3F_T2F));
    m_nUnitIndexCount = 6;
    m_pUnitIndexData = (GLuint*)malloc(m_nUnitIndexCount * sizeof(GLuint));

    m_pRunwayData = NULL;
    m_nRunwayDataSize = 0;
    m_bRunwayDataDirty = false;

    Image *pImage = new Image();
    pImage->initWithImageFile("res/NavSvs/_rway_line.jpg");
    pImage->GenerateMipmaps();
    m_pTexture = new Texture2D();
    m_pTexture->initWithImage(pImage);
    Texture2D::TexParams texparam = { GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT };
    m_pTexture->setTexParameters(texparam);
    HMS_SAFE_RELEASE(pImage);
}

CHmsNavSvsObjectRunway::~CHmsNavSvsObjectRunway()
{

}

void CHmsNavSvsObjectRunway::InitUnitData(const SvsRunwayDataStu *pRunwayData)
{
    double widthRatio = CHmsNavSvsMathHelper::MeterToWorldUnit(pRunwayData->width * 0.5);
    double lengthRatio = CHmsNavSvsMathHelper::MeterToWorldUnit(pRunwayData->length * 0.5);

    int index = 0;
    m_pUnitVertexData[index++].vertices = Vec3(1 * widthRatio, 0, -2 * lengthRatio);
    m_pUnitVertexData[index++].vertices = Vec3(-1 * widthRatio, 0, -2 * lengthRatio);
    m_pUnitVertexData[index++].vertices = Vec3(-1 * widthRatio, 0, 0 * lengthRatio);
    m_pUnitVertexData[index++].vertices = Vec3(1 * widthRatio, 0, 0 * lengthRatio);

    for (int i = 0; i < 1; ++i)
    {
        m_pUnitVertexData[0 + i * 4].texCoords.u = 1;
        m_pUnitVertexData[0 + i * 4].texCoords.v = 0;

        m_pUnitVertexData[1 + i * 4].texCoords.u = 0;
        m_pUnitVertexData[1 + i * 4].texCoords.v = 0;

        m_pUnitVertexData[2 + i * 4].texCoords.u = 0;
        m_pUnitVertexData[2 + i * 4].texCoords.v = pRunwayData->length / 50;

        m_pUnitVertexData[3 + i * 4].texCoords.u = 1;
        m_pUnitVertexData[3 + i * 4].texCoords.v = pRunwayData->length / 50;
    }
    index = 0;
    for (int i = 0; i < 1; ++i)
    {
        m_pUnitIndexData[index++] = 0 + i * 4;
        m_pUnitIndexData[index++] = 1 + i * 4;
        m_pUnitIndexData[index++] = 2 + i * 4;

        m_pUnitIndexData[index++] = 0 + i * 4;
        m_pUnitIndexData[index++] = 2 + i * 4;
        m_pUnitIndexData[index++] = 3 + i * 4;
    }
}

void CHmsNavSvsObjectRunway::SetData(SvsRunwayDataStu *pData, int dataSize)
{
    if (m_pRunwayData)
    {
        free(m_pRunwayData);
        m_pRunwayData = NULL;
        m_nRunwayDataSize = 0;
    }
    if (pData && dataSize > 0)
    {
        m_nRunwayDataSize = dataSize;
        m_pRunwayData = (SvsRunwayDataStu*)malloc(dataSize * sizeof(SvsRunwayDataStu));
        memcpy(m_pRunwayData, pData, dataSize * sizeof(SvsRunwayDataStu));
    }

    m_bRunwayDataDirty = true;
}

void CHmsNavSvsObjectRunway::UpdateVertexData()
{
#define VEC3D_TO_VEC3F(a,b) a.x = b.x;a.y=b.y;a.z=b.z

    if (m_pVertexData)
    {
        free(m_pVertexData);
        m_pVertexData = NULL;
        m_nVertexCount = 0;
    }
    if (m_pIndexData)
    {
        free(m_pIndexData);
        m_pIndexData = NULL;
        m_nIndexCount = 0;
    }
    if (m_nRunwayDataSize <= 0 || m_pRunwayData == NULL)
    {
        return;
    }
    m_nVertexCount = m_nUnitVertexCount * m_nRunwayDataSize;
    m_nIndexCount = m_nUnitIndexCount * m_nRunwayDataSize;
    m_pVertexData = (V3F_T2F*)malloc(m_nVertexCount * sizeof(V3F_T2F));
    m_pIndexData = (GLuint*)malloc(m_nIndexCount * sizeof(GLuint));

    for (int i = 0; i < m_nRunwayDataSize; ++i)
    {
        SvsRunwayDataStu *pStu = &m_pRunwayData[i];

        Vec2d lonlat2 = CHmsNavSvsMathHelper::NextPointLonLatByBearing_Double(&pStu->runwayOne.lonLat, pStu->width / 2000, pStu->runwayOne.bearing - 90);
        Vec2d lonlat3 = CHmsNavSvsMathHelper::NextPointLonLatByBearing_Double(&pStu->runwayOne.lonLat, pStu->width / 2000, pStu->runwayOne.bearing + 90);

        Vec2d lonlat0 = CHmsNavSvsMathHelper::NextPointLonLatByBearing_Double(&pStu->runwayTwo.lonLat, pStu->width / 2000, pStu->runwayTwo.bearing - 90);
        Vec2d lonlat1 = CHmsNavSvsMathHelper::NextPointLonLatByBearing_Double(&pStu->runwayTwo.lonLat, pStu->width / 2000, pStu->runwayTwo.bearing + 90);

        int start = i * m_nUnitVertexCount;

        Vec3d pos;
        pos = CHmsNavSvsMathHelper::LonLatAltToXYZ_Double(&lonlat0, pStu->runwayTwo.height);
        VEC3D_TO_VEC3F(m_pVertexData[0 + start].vertices, pos);

        pos = CHmsNavSvsMathHelper::LonLatAltToXYZ_Double(&lonlat1, pStu->runwayTwo.height);
        VEC3D_TO_VEC3F(m_pVertexData[1 + start].vertices, pos);

        pos = CHmsNavSvsMathHelper::LonLatAltToXYZ_Double(&lonlat2, pStu->runwayOne.height);
        VEC3D_TO_VEC3F(m_pVertexData[2 + start].vertices, pos);

        pos = CHmsNavSvsMathHelper::LonLatAltToXYZ_Double(&lonlat3, pStu->runwayOne.height);
        VEC3D_TO_VEC3F(m_pVertexData[3 + start].vertices, pos);

        InitUnitData(pStu);

        //int start = i * m_nUnitVertexCount;
        for (int j = start; j < start + m_nUnitVertexCount; ++j)
        {
            m_pVertexData[j].texCoords = m_pUnitVertexData[j - start].texCoords;
        }
      
        start = i * m_nUnitIndexCount;
        pStu->indexOffsetInVbo = start;
        for (int j = start; j < start + m_nUnitIndexCount; ++j)
        {
            m_pIndexData[j] = m_pUnitIndexData[j - start] + i * m_nUnitVertexCount;
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_nVBO);
    glBufferData(GL_ARRAY_BUFFER, m_nVertexCount * sizeof(V3F_T2F), m_pVertexData, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_nVBOIndex);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_nIndexCount * sizeof(GLuint), m_pIndexData, GL_STATIC_DRAW);
}

void CHmsNavSvsObjectRunway::CalRunwayToDraw()
{
    memset(m_indexRunwayToDraw, -1, sizeof(m_indexRunwayToDraw));
    
    int index = 0;
    auto planeLonLat = s_pSvsMgr->GetAttitudeStu()->LngLat;
    for (int i = 0; i < m_nRunwayDataSize; ++i)
    {
        Vec2d *pPos = &m_pRunwayData[i].runwayOne.lonLat;      
        double dis = CalculateDistanceKM(pPos->x, pPos->y, planeLonLat.x, planeLonLat.y);
        if (dis < 100)
        {
            m_indexRunwayToDraw[index++] = i;
        }
    }
}

void CHmsNavSvsObjectRunway::Update(float delta)
{
    if (m_bRunwayDataDirty)
    {
        m_bRunwayDataDirty = false;
        UpdateVertexData();
        CalRunwayToDraw();
    }
    {
        static float runwayElapse = 0;
        runwayElapse += delta;
        if (runwayElapse > 5)
        {
            runwayElapse = 0;
            CalRunwayToDraw();
        }
    }
}

void CHmsNavSvsObjectRunway::OnDraw()
{
    if (m_nVertexCount <= 0 || m_nIndexCount <= 0)
    {
        return;
    }
    Mat4 mat;
    CHmsNavSvsMathHelper::GetMatFromMat4d(&mat, s_pSvsMgr->GetMat4dPVRelative());

    auto pGLprogram = s_pSvsObjectMgr->m_pGlProgram;
    pGLprogram->setUniformLocationWithMatrix4fv(s_pSvsObjectMgr->m_locUniformMVP, mat.m, 1);
    pGLprogram->setUniformLocationWith4f(s_pSvsObjectMgr->m_locUniformColor, 1.0, 1.0, 1.0, 1.0);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glBindBuffer(GL_ARRAY_BUFFER, m_nVBO);
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(V3F_T2F), (GLvoid *)offsetof(V3F_T2F, vertices));
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(V3F_T2F), (GLvoid *)offsetof(V3F_T2F, texCoords));
    glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_POSITION);
    glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_TEX_COORD);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_nVBOIndex);
    glBindTexture(GL_TEXTURE_2D, m_pTexture->getName());

    for (int i = 0; i < 50; ++i)
    {
        int index = m_indexRunwayToDraw[i];
        if (index < 0 || index >= m_nRunwayDataSize)
        {
            break;
        }
        int offset = m_pRunwayData[index].indexOffsetInVbo * sizeof(GLuint);
        glDrawElements(GL_TRIANGLES, m_nUnitIndexCount, GL_UNSIGNED_INT, (void*)offset);
    }
}
