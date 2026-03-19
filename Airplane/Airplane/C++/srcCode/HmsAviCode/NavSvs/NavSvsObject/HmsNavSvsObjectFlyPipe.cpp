#include "HmsNavSvsObjectFlyPipe.h"
#include "NavSvs/HmsNavSvsMgr.h"
#include "base/HmsImage.h"
#include "Calculate/CalculateZs.h"
#include "render/HmsGLStateCache.h"
#include "HmsNavSvsObjectMgr.h"

using namespace HmsAviPf;

CHmsNavSvsObjectFlyPipe::CHmsNavSvsObjectFlyPipe()
{
    m_objectType = NavSvsObjectType::OBJECTTYPE_FLYPIPE;

    glGenBuffers(1, &m_nVBO);
    glGenBuffers(1, &m_nVBOIndex);

    m_nVertexCount = 0;
    m_nIndexCount = 0;
    m_pVertexData = NULL;
    m_pIndexData = NULL;

    m_nUnitVertexCount = 4 * 4;
    m_pUnitVertexData = (V3F_T2F*)malloc(m_nUnitVertexCount * sizeof(V3F_T2F));
    m_nUnitIndexCount = 24;
    m_pUnitIndexData = (GLuint*)malloc(m_nUnitIndexCount * sizeof(GLuint));

    m_pPipeData = NULL;
    m_nPipeDataSize = 0;
    m_fRnp = 0.3f;
    m_bPipeDataDirty = false;
    m_nCurPipeDataIndex = -1;

    m_bDrawFlyPipe = true;
    m_bTwinkleFlyPipe = false;
    m_bPlaneOutFlyPipe = false;

    Image *pImage = new Image();
    pImage->initWithImageFile("res/NavSvs/flytube.png");
    pImage->GenerateMipmaps();
    m_pTexture = new Texture2D();
    m_pTexture->initWithImage(pImage);
    HMS_SAFE_RELEASE(pImage);
}

CHmsNavSvsObjectFlyPipe::~CHmsNavSvsObjectFlyPipe()
{

}

void CHmsNavSvsObjectFlyPipe::InitUnitData(double scale, double space)
{
    int index = 0;
    float xLen = 1.5;
    m_pUnitVertexData[index++].vertices = Vec3(-1 * xLen * scale + space,  1 * scale        , 0);
    m_pUnitVertexData[index++].vertices = Vec3(-1 * xLen * scale        ,  1 * scale        , 0);
    m_pUnitVertexData[index++].vertices = Vec3(-1 * xLen * scale        , -1 * scale        , 0);
    m_pUnitVertexData[index++].vertices = Vec3(-1 * xLen * scale + space, -1 * scale        , 0);
    m_pUnitVertexData[index++].vertices = Vec3( 1 * xLen * scale        , -1 * scale + space, 0);
    m_pUnitVertexData[index++].vertices = Vec3(-1 * xLen * scale        , -1 * scale + space, 0);
    m_pUnitVertexData[index++].vertices = Vec3(-1 * xLen * scale        , -1 * scale        , 0);
    m_pUnitVertexData[index++].vertices = Vec3( 1 * xLen * scale        , -1 * scale        , 0);
    m_pUnitVertexData[index++].vertices = Vec3( 1 * xLen * scale        ,  1 * scale        , 0);
    m_pUnitVertexData[index++].vertices = Vec3( 1 * xLen * scale - space,  1 * scale        , 0);
    m_pUnitVertexData[index++].vertices = Vec3( 1 * xLen * scale - space, -1 * scale        , 0);
    m_pUnitVertexData[index++].vertices = Vec3( 1 * xLen * scale        , -1 * scale        , 0);
    m_pUnitVertexData[index++].vertices = Vec3( 1 * xLen * scale        ,  1 * scale        , 0);
    m_pUnitVertexData[index++].vertices = Vec3(-1 * xLen * scale        ,  1 * scale        , 0);
    m_pUnitVertexData[index++].vertices = Vec3(-1 * xLen * scale        ,  1 * scale - space, 0);
    m_pUnitVertexData[index++].vertices = Vec3( 1 * xLen * scale        ,  1 * scale - space, 0);
    for (int i = 0; i < 4; ++i)
    {
        m_pUnitVertexData[0 + i * 4].texCoords.u = 1;
        m_pUnitVertexData[0 + i * 4].texCoords.v = 0;

        m_pUnitVertexData[1 + i * 4].texCoords.u = 0;
        m_pUnitVertexData[1 + i * 4].texCoords.v = 0;

        m_pUnitVertexData[2 + i * 4].texCoords.u = 0;
        m_pUnitVertexData[2 + i * 4].texCoords.v = 1;

        m_pUnitVertexData[3 + i * 4].texCoords.u = 1;
        m_pUnitVertexData[3 + i * 4].texCoords.v = 1;
    }
    index = 0;
    for (int i = 0; i < 4; ++i)
    {
        m_pUnitIndexData[index++] = 0 + i * 4;
        m_pUnitIndexData[index++] = 1 + i * 4;
        m_pUnitIndexData[index++] = 2 + i * 4;

        m_pUnitIndexData[index++] = 0 + i * 4;
        m_pUnitIndexData[index++] = 2 + i * 4;
        m_pUnitIndexData[index++] = 3 + i * 4;
    }
}

void CHmsNavSvsObjectFlyPipe::SetData(SvsFlyPipeDataStu *pData, int dataSize, float rnp)
{
    if (m_pPipeData)
    {
        free(m_pPipeData);
        m_pPipeData = NULL;
        m_nPipeDataSize = 0;
    }
    if (pData && dataSize > 0)
    {
        m_fRnp = rnp;
        m_nPipeDataSize = dataSize;
        m_pPipeData = (SvsFlyPipeDataStu*)malloc(dataSize * sizeof(SvsFlyPipeDataStu));
        memcpy(m_pPipeData, pData, dataSize * sizeof(SvsFlyPipeDataStu));
    }

    m_bPipeDataDirty = true;
}

void CHmsNavSvsObjectFlyPipe::UpdateVertexData()
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
    if (m_nPipeDataSize <= 0 || m_pPipeData == NULL)
    {
        return;
    }
    m_nVertexCount = m_nUnitVertexCount * m_nPipeDataSize;
    m_nIndexCount = m_nUnitIndexCount * m_nPipeDataSize;
    m_pVertexData = (V3F_T2F*)malloc(m_nVertexCount * sizeof(V3F_T2F));
    m_pIndexData = (GLuint*)malloc(m_nIndexCount * sizeof(GLuint));

    double scale = CHmsNavSvsMathHelper::MeterToWorldUnit(NmToKilometer(m_fRnp) * 1000.0);
    double space = CHmsNavSvsMathHelper::MeterToWorldUnit(m_fRnp * 40);
    InitUnitData(scale, space);

    CalAttitudeStu attiStu;
    Mat4d mat4dPipeModel;
    for (int i = 0; i < m_nPipeDataSize; ++i)
    {
        SvsFlyPipeDataStu *pStu = &m_pPipeData[i];
        attiStu.LngLat.x = pStu->lonLat.x;
        attiStu.LngLat.y = pStu->lonLat.y;
        attiStu.Height = pStu->height;
        attiStu.pitch = 0;
        attiStu.roll = 0;
        //calculate yaw
        {
            Vec2 *pLonlat1 = nullptr;
            Vec2 *pLonlat2 = nullptr;
            if (i == m_nPipeDataSize - 1)
            {
                pLonlat1 = &m_pPipeData[i - 1].lonLat;
                pLonlat2 = &m_pPipeData[i].lonLat;
            }
            else
            {
                pLonlat1 = &m_pPipeData[i].lonLat;
                pLonlat2 = &m_pPipeData[i + 1].lonLat;
            }
            double angle = CalculateAngle(pLonlat1->x, pLonlat1->y, pLonlat2->x, pLonlat2->y);
            angle = MATH_RAD_TO_DEG(angle);
            attiStu.yaw = angle;
        }
        CHmsNavSvsMathHelper::CaculateAttitude(&attiStu, &mat4dPipeModel, NULL, NULL, NULL, NULL);

        int start = i * m_nUnitVertexCount;
        for (int j = start; j < start + m_nUnitVertexCount; ++j)
        {
            m_pVertexData[j] = m_pUnitVertexData[j - start];
            Vec3d pos;
            VEC3D_TO_VEC3F(pos, m_pVertexData[j].vertices);
            mat4dPipeModel.transformPoint(pos, &pos);
            VEC3D_TO_VEC3F(m_pVertexData[j].vertices, pos);
        }
        start = i * m_nUnitIndexCount;
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

void CHmsNavSvsObjectFlyPipe::SetRnp(double rnp)
{
    m_fRnp = rnp;

    m_bPipeDataDirty = true;
}

void CHmsNavSvsObjectFlyPipe::CalCurPipeDataIndex()
{
    m_nCurPipeDataIndex = -1;
    double minValue = 999999;
    int minIndex = -1;
    for (int i = 0; i < m_nPipeDataSize; ++i)
    {
        Vec2 *pPos = &m_pPipeData[i].lonLat;
        auto ll = s_pSvsMgr->GetAttitudeStu()->LngLat;
        double dis = CalculateDistanceNM(pPos->x, pPos->y, ll.x, ll.y);
        if (dis < minValue)
        {
            minValue = dis;
            minIndex = i;
        }
    }
    m_nCurPipeDataIndex = minIndex;

    if (m_nCurPipeDataIndex == -1)
    {
        m_nCurPipeDataIndex = m_nPipeDataSize - 1;
    } 

    //printf(" \n  m_nCurPipeDataIndex : %d \n ", m_nCurPipeDataIndex);
}

void CHmsNavSvsObjectFlyPipe::CalCurOffsetDistance()
{
    m_bTwinkleFlyPipe = false;
    m_bPlaneOutFlyPipe = false;
    m_bDrawFlyPipe = true;

    if (m_nCurPipeDataIndex >= 0 && m_nCurPipeDataIndex < m_nPipeDataSize)
    {
        SvsFlyPipeDataStu *pPipeData1 = &m_pPipeData[m_nCurPipeDataIndex];
        SvsFlyPipeDataStu *pPipeData2;
        if (m_nCurPipeDataIndex == m_nPipeDataSize - 1)
        {
            pPipeData2 = &m_pPipeData[m_nCurPipeDataIndex - 1];
        }
        else
        {
            pPipeData2 = &m_pPipeData[m_nCurPipeDataIndex + 1];
        }
        Vec2d lonlat1 = { pPipeData1->lonLat.x, pPipeData1->lonLat.y };
        Vec2d lonlat2 = { pPipeData2->lonLat.x, pPipeData2->lonLat.y };
        auto pAttitudeStu = s_pSvsMgr->GetAttitudeStu();
        Vec3d p0 = CHmsNavSvsMathHelper::LonLatAltToXYZ_Double(&pAttitudeStu->LngLat, pAttitudeStu->Height);
        Vec3d p1 = CHmsNavSvsMathHelper::LonLatAltToXYZ_Double(&lonlat1, pPipeData1->height);
        Vec3d p2 = CHmsNavSvsMathHelper::LonLatAltToXYZ_Double(&lonlat2, pPipeData2->height);
        Vec3d p10 = p0 - p1;
        Vec3d p12 = p2 - p1;
        Vec3d::cross(p10, p12, &p10);
        float distance = p10.length() / p12.length();
        distance = KilometerToNM(CHmsNavSvsMathHelper::WorldUnitToMeter(distance) / 1000.0);
        if (distance > m_fRnp)
        {
            //m_bTwinkleFlyPipe = true;
            m_bPlaneOutFlyPipe = true;
        }
        //printf(" \n  distance : %f \n ", distance);
    }
}

void CHmsNavSvsObjectFlyPipe::SetFlyPipeData(std::vector<PositionRouteViewData> &vecLonlat)
{
    int vecSize = vecLonlat.size();
    SvsFlyPipeDataStu *pStu = (SvsFlyPipeDataStu*)malloc(vecSize * sizeof(SvsFlyPipeDataStu));
    for (int i = 0; i < vecSize; ++i)
    {
        PositionRouteViewData *pPos = &vecLonlat[i];
        pStu[i].lonLat.x = pPos->lon;
        pStu[i].lonLat.y = pPos->lat;
        pStu[i].height = pPos->height;
    }
    SetData(pStu, vecSize, 0.3f);

    free(pStu);
}

void CHmsNavSvsObjectFlyPipe::Update(float delta)
{
    if (m_bPipeDataDirty)
    {
        m_bPipeDataDirty = false;
        UpdateVertexData();
    }

    {
        static float flyPipeElapse = 0;
        flyPipeElapse += delta;
        if (flyPipeElapse > 1)
        {
            flyPipeElapse = 0;
            CalCurPipeDataIndex();
            CalCurOffsetDistance();
        }
    }

    if (m_bTwinkleFlyPipe)
    {
        static float flypipeTime = 0;
        flypipeTime += delta;
        if (flypipeTime >= 1)
        {
            flypipeTime = 0;
            m_bDrawFlyPipe = m_bDrawFlyPipe ? false : true;
        }
    }
}

#define FLYPIPE_OFFSET_START 2
#define FLYPIPE_OFFSET_END 5

void CHmsNavSvsObjectFlyPipe::OnDraw()
{
    if (!m_bDrawFlyPipe)
    {
        return;
    }
    if (m_nVertexCount <= 0 || m_nIndexCount <= 0)
    {
        return;
    }
    Mat4 mat;
    CHmsNavSvsMathHelper::GetMatFromMat4d(&mat, s_pSvsMgr->GetMat4dPVRelative());

    auto pGLprogram = s_pSvsObjectMgr->m_pGlProgram;
    pGLprogram->setUniformLocationWithMatrix4fv(s_pSvsObjectMgr->m_locUniformMVP, mat.m, 1);
    if (m_bPlaneOutFlyPipe)
    {
        pGLprogram->setUniformLocationWith4f(s_pSvsObjectMgr->m_locUniformColor, 1.0f, 150.0f / 255.0f, 85.0f / 255.0f, 1.0f);
    }
    else
    {
        pGLprogram->setUniformLocationWith4f(s_pSvsObjectMgr->m_locUniformColor, 0.0f, 1.0f, 0.0f, 1.0f);
    }
    
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glBindBuffer(GL_ARRAY_BUFFER, m_nVBO);
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(V3F_T2F), (GLvoid *)offsetof(V3F_T2F, vertices));
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(V3F_T2F), (GLvoid *)offsetof(V3F_T2F, texCoords));
    glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_POSITION);
    glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_TEX_COORD);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_nVBOIndex);
    glBindTexture(GL_TEXTURE_2D, m_pTexture->getName());

#if 0
    if (m_nCurPipeDataIndex >= 0 && m_nCurPipeDataIndex < m_nPipeDataSize)
    {
        for (int i = 0; i < 2; ++i)
        {
            int start = m_nCurPipeDataIndex + FLYPIPE_OFFSET_START - i * ((FLYPIPE_OFFSET_END - FLYPIPE_OFFSET_START) + FLYPIPE_OFFSET_START * 2);
            start = SVS_BETWEEN_VALUE(start, 0, m_nPipeDataSize - 1);
            int end = m_nCurPipeDataIndex + FLYPIPE_OFFSET_END - i * ((FLYPIPE_OFFSET_END - FLYPIPE_OFFSET_START) + FLYPIPE_OFFSET_START * 2);
            end = SVS_BETWEEN_VALUE(end, 0, m_nPipeDataSize - 1);
            if (end > start)
            {
                int count = end - start + 1;
                int offset = start * m_nUnitIndexCount * sizeof(GLuint);
                glDrawElements(GL_TRIANGLES, count * m_nUnitIndexCount, GL_UNSIGNED_INT, (void*)offset);
            }   
        }    
    }
#else
    glDrawElements(GL_TRIANGLES, m_nIndexCount, GL_UNSIGNED_INT, 0);
#endif
}
