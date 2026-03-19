#include "HmsNavSvsObjectRouteTrack.h"
#include "NavSvs/HmsNavSvsMgr.h"
#include "base/HmsImage.h"
#include "Calculate/CalculateZs.h"
#include "render/HmsGLStateCache.h"
#include "HmsNavSvsObjectMgr.h"

using namespace HmsAviPf;

CHmsNavSvsObjectRouteTrack::CHmsNavSvsObjectRouteTrack()
{
    m_objectType = NavSvsObjectType::OBJECTTYPE_ROUTETRACK;

    glGenBuffers(1, &m_nVBO);
    glGenBuffers(1, &m_nVBOIndex);

    m_vecUnitVertexData.resize(4);
    m_vecUnitIndexData.resize(6);

    m_vecVertexData.reserve(1000);
    m_vecIndexData.reserve(1500);

    m_bDataDirty = false;

    Image *pImage = new Image();
    pImage->initWithImageFile("res/NavSvs/routeTrack.png");
    m_pTexture = new Texture2D();
    m_pTexture->initWithImage(pImage);
    Texture2D::TexParams texparam = { GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE };
    m_pTexture->setTexParameters(texparam);
    HMS_SAFE_RELEASE(pImage);
}

CHmsNavSvsObjectRouteTrack::~CHmsNavSvsObjectRouteTrack()
{

}

/*
*2   1
*3   0
*/
void CHmsNavSvsObjectRouteTrack::InitUnitData(float length)
{
    int index = 0;

    for (int i = 0; i < 1; ++i)
    {
        m_vecUnitVertexData[0 + i * 4].texCoords.u = 1;
        m_vecUnitVertexData[0 + i * 4].texCoords.v = 0;

        m_vecUnitVertexData[1 + i * 4].texCoords.u = 0;
        m_vecUnitVertexData[1 + i * 4].texCoords.v = 0;

        m_vecUnitVertexData[2 + i * 4].texCoords.u = 0;
        m_vecUnitVertexData[2 + i * 4].texCoords.v = length / 50;

        m_vecUnitVertexData[3 + i * 4].texCoords.u = 1;
        m_vecUnitVertexData[3 + i * 4].texCoords.v = length / 50;
    }
    index = 0;
    for (int i = 0; i < 1; ++i)
    {
        m_vecUnitIndexData[index++] = 0 + i * 4;
        m_vecUnitIndexData[index++] = 1 + i * 4;
        m_vecUnitIndexData[index++] = 2 + i * 4;

        m_vecUnitIndexData[index++] = 0 + i * 4;
        m_vecUnitIndexData[index++] = 2 + i * 4;
        m_vecUnitIndexData[index++] = 3 + i * 4;
    }
}

void CHmsNavSvsObjectRouteTrack::UpdateVertexData()
{
#define VEC3D_TO_VEC3F(a,b) a.x = b.x;a.y=b.y;a.z=b.z

    m_vecVertexData.clear();
    m_vecIndexData.clear();

    if (m_vecData.empty())
    {
        return;
    }

    int index = 0;
    for (int i = 0; i < (int)m_vecData.size() - 1; ++i)
    {
        const auto &curStu = m_vecData[i];
        const auto &nextStu = m_vecData[i + 1];

        if (curStu.lon == nextStu.lon && curStu.lat == nextStu.lat)
        {
            continue;
        }
        auto angle = MATH_RAD_TO_DEG_DOUBLE(CalculateAngle(curStu.lon, curStu.lat, nextStu.lon, nextStu.lat));
        auto length = CalculateDistanceM(curStu.lon, curStu.lat, nextStu.lon, nextStu.lat);
        Vec2d tmpPos;

        float width = 0.01f;
        tmpPos = Vec2d(curStu.lon, curStu.lat);
        Vec2d lonlat2 = CHmsNavSvsMathHelper::NextPointLonLatByBearing_Double(&tmpPos, width, angle - 90);
        Vec2d lonlat3 = CHmsNavSvsMathHelper::NextPointLonLatByBearing_Double(&tmpPos, width, angle + 90);

        tmpPos = Vec2d(nextStu.lon, nextStu.lat);
        Vec2d lonlat0 = CHmsNavSvsMathHelper::NextPointLonLatByBearing_Double(&tmpPos, width, angle + 90);
        Vec2d lonlat1 = CHmsNavSvsMathHelper::NextPointLonLatByBearing_Double(&tmpPos, width, angle - 90);

        int start = index * m_vecUnitVertexData.size();

        m_vecVertexData.insert(m_vecVertexData.end(), m_vecUnitVertexData.begin(), m_vecUnitVertexData.end());
        m_vecIndexData.insert(m_vecIndexData.end(), m_vecUnitIndexData.begin(), m_vecUnitIndexData.end());

        Vec3d pos;
        pos = CHmsNavSvsMathHelper::LonLatAltToXYZ_Double(&lonlat0, nextStu.height);
        VEC3D_TO_VEC3F(m_vecVertexData[0 + start].vertices, pos);

        pos = CHmsNavSvsMathHelper::LonLatAltToXYZ_Double(&lonlat1, nextStu.height);
        VEC3D_TO_VEC3F(m_vecVertexData[1 + start].vertices, pos);

        pos = CHmsNavSvsMathHelper::LonLatAltToXYZ_Double(&lonlat2, curStu.height);
        VEC3D_TO_VEC3F(m_vecVertexData[2 + start].vertices, pos);

        pos = CHmsNavSvsMathHelper::LonLatAltToXYZ_Double(&lonlat3, curStu.height);
        VEC3D_TO_VEC3F(m_vecVertexData[3 + start].vertices, pos);

        InitUnitData(length);

        for (int j = start; j < start + (int)m_vecUnitVertexData.size(); ++j)
        {
            m_vecVertexData[j].texCoords = m_vecUnitVertexData[j - start].texCoords;
        }

        start = index * m_vecUnitIndexData.size();
        for (int j = start; j < start + (int)m_vecUnitIndexData.size(); ++j)
        {
            m_vecIndexData[j] = m_vecUnitIndexData[j - start] + index * m_vecUnitVertexData.size();
        }

        ++index;
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_nVBO);
    glBufferData(GL_ARRAY_BUFFER, m_vecVertexData.size() * sizeof(V3F_T2F), m_vecVertexData.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_nVBOIndex);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_vecIndexData.size() * sizeof(GLuint), m_vecIndexData.data(), GL_STATIC_DRAW);
}

void CHmsNavSvsObjectRouteTrack::SetData(std::vector<PositionRouteViewData> &vecLonlat)
{
    m_vecData = vecLonlat;

    m_bDataDirty = true;
}

void CHmsNavSvsObjectRouteTrack::Update(float delta)
{
    if (m_bDataDirty)
    {
        m_bDataDirty = false;
        UpdateVertexData();
    }
}

void CHmsNavSvsObjectRouteTrack::OnDraw()
{
    if (m_vecVertexData.empty() || m_vecIndexData.empty())
    {
        return;
    }
    Mat4 mat;
    CHmsNavSvsMathHelper::GetMatFromMat4d(&mat, s_pSvsMgr->GetMat4dPVRelative());

    auto pGLprogram = s_pSvsObjectMgr->m_pGlProgram;
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

    glDrawElements(GL_TRIANGLES, m_vecIndexData.size(), GL_UNSIGNED_INT, 0);
}
