#include "HmsNavSvsObjectADSBMgr.h"
#include "NavSvs/HmsNavSvsMgr.h"
#include "base/HmsImage.h"
#include "Calculate/CalculateZs.h"
#include "render/HmsGLStateCache.h"
#include "HmsNavSvsObjectMgr.h"
#include "NavSvs/HmsNavSvsModuleMgr.h"
#include "base/HmsFileUtils.h"
#include "HmsGlobal.h"
#include "DataInterface/HmsDataBus.h"

using namespace HmsAviPf;

CHmsNavSvsObjectADSBMgr::CHmsNavSvsObjectADSBMgr()
{
    m_objectType = NavSvsObjectType::OBJECTTYPE_ADSB;

    glGenBuffers(1, &m_nVBO);
    glGenBuffers(1, &m_nVBOIndex);

    m_vecUnitVertexData.resize(4);
    m_vecUnitIndexData.resize(6);

    m_bDataDirty = false;

    Image *pImage = new Image();
    pImage->initWithImageFile("res/NavSvs/y8_512.png");
    m_pTexture = new Texture2D();
    m_pTexture->initWithImage(pImage);
    Texture2D::TexParams texparam = { GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE };
    m_pTexture->setTexParameters(texparam);
    HMS_SAFE_RELEASE(pImage);

    InitUnitData();
}

CHmsNavSvsObjectADSBMgr::~CHmsNavSvsObjectADSBMgr()
{

}

void CHmsNavSvsObjectADSBMgr::UpdateVertexData()
{
    if (m_vecData.empty())
    {
        return;
    }

    if (m_vecLabel.size() < m_vecData.size())
    {
        int len = m_vecData.size() - m_vecLabel.size();
        for (int i = 0; i < len; ++i)
        {
            auto pLabel = CHmsNavSvsLabel::Create("1");
            m_vecLabel.push_back(pLabel);
        }
    }
    int index = 0;
    for (int i = 0; i < (int)m_vecData.size(); ++i)
    {
        auto &stu = m_vecData[i];
        stu.pLabel = m_vecLabel[i];
    }
}

static void PushVertex(V3F_T2F *pData, float x, float y, float z, float u, float v)
{
    float ratio = 64;
    pData->vertices.x = x * ratio;
    pData->vertices.y = y * ratio;
    pData->vertices.z = z * ratio;
    pData->texCoords.u = u;
    pData->texCoords.v = v;
}

void CHmsNavSvsObjectADSBMgr::InitUnitData()
{
    int index = 0;

    PushVertex(&m_vecUnitVertexData[index++], 0.0f, 0.0f, 0.0f, 0.75f, 0.0f);
    PushVertex(&m_vecUnitVertexData[index++], -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    PushVertex(&m_vecUnitVertexData[index++], -1.0f, 1.0f, 0.0f, 0.0f, 0.64f);
    PushVertex(&m_vecUnitVertexData[index++], 0.0f, 1.0f, 0.0f, 0.75f, 0.64f);

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

    glBindBuffer(GL_ARRAY_BUFFER, m_nVBO);
    glBufferData(GL_ARRAY_BUFFER, m_vecUnitVertexData.size() * sizeof(V3F_T2F), m_vecUnitVertexData.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_nVBOIndex);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_vecUnitIndexData.size() * sizeof(GLuint), m_vecUnitIndexData.data(), GL_DYNAMIC_DRAW);
}

void CHmsNavSvsObjectADSBMgr::SetData(std::vector<NavSvsObjectADSBStu> &vec)
{
    m_vecData = vec;
    m_bDataDirty = true;
}

void CHmsNavSvsObjectADSBMgr::Update(float delta)
{
    //=========add test data ========begin
#if 0
    auto pPositionMgr = CHmsNavSvsModuleMgr::GetInstance()->GetPositionMgr(NavSvsPositionMgrType::POSMGR_DATABUS);
    if (pPositionMgr)
    {
        if (pPositionMgr != CHmsNavSvsModuleMgr::GetInstance()->GetCurPositionMgr())
        {
            pPositionMgr->Update(delta);
        }     

        std::vector<NavSvsObjectADSBStu> vecTest;
        {
            NavSvsObjectADSBStu stu;
            stu.ident = "zuuu01";
            stu.lon = pPositionMgr->GetLon();
            stu.lat = pPositionMgr->GetLat();
            stu.height = pPositionMgr->GetHeight();
            vecTest.push_back(stu);
        }
        {
            NavSvsObjectADSBStu stu;
            stu.ident = "zuuu03";
            stu.lon = pPositionMgr->GetLon();
            stu.lat = pPositionMgr->GetLat();
            stu.height = pPositionMgr->GetHeight() + 1000;
            vecTest.push_back(stu);
        }
        SetData(vecTest);
    }
#else
    auto pMap = CHmsGlobal::GetInstance()->GetDataBus()->GetADSBData();
    std::vector<NavSvsObjectADSBStu> vecTest;
    for (auto iter : *pMap)
    {
        NavSvsObjectADSBStu stu;
        stu.ident = iter.second.callsign;
        stu.lon = iter.second.longitude;
        stu.lat = iter.second.latitude;
        stu.height = FeetToMeter(iter.second.altitude);
        vecTest.push_back(stu);
    }
    SetData(vecTest);
#endif
    //=========add test data ========end

    if (m_bDataDirty)
    {
        m_bDataDirty = false;
        UpdateVertexData();
    }

    auto pCurPositionMgr = CHmsNavSvsModuleMgr::GetInstance()->GetCurPositionMgr();
    CalAttitudeStu cameraAttiStu;
    cameraAttiStu.LngLat.x = pCurPositionMgr->GetLon();
    cameraAttiStu.LngLat.y = pCurPositionMgr->GetLat();
    cameraAttiStu.Height = pCurPositionMgr->GetHeight();
    cameraAttiStu.yaw = pCurPositionMgr->GetYaw();
    cameraAttiStu.pitch = pCurPositionMgr->GetPitch();
    cameraAttiStu.roll = pCurPositionMgr->GetRoll();

    for (int i = 0; i < (int)m_vecData.size(); ++i)
    {
        auto &stu = m_vecData[i];
        CalAttitudeStu attiStu;
        attiStu.LngLat.x = stu.lon;
        attiStu.LngLat.y = stu.lat;
        attiStu.Height = stu.height;
        attiStu.yaw = 180;
        attiStu.pitch = 0;
        attiStu.roll = 0;
        stu.pLabel->SetAttitudeStu(attiStu);

        char tmp[128];
        sprintf(tmp, "%s %.6f %.6f %.0fm", stu.ident.c_str(), attiStu.LngLat.x, attiStu.LngLat.y, attiStu.Height);
        stu.pLabel->SetString(tmp);
        stu.pLabel->SetCameraAttitudeStu(cameraAttiStu);
        stu.pLabel->SetRotateWithCamera(true);
        stu.pLabel->Update(delta);
    }
}

void CHmsNavSvsObjectADSBMgr::OnDraw()
{
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

    for (int i = 0; i < (int)m_vecData.size(); ++i)
    {
        auto &stu = m_vecData[i];
        if (stu.pLabel == nullptr)
        {
            continue;
        }

        auto model = (*s_pSvsMgr->GetMat4dTrans()) * stu.pLabel->m_mat4dM;
        auto pMat4dPV = CHmsNavSvsMgr::GetInstance()->GetMat4dPVRelative();
        model = (*pMat4dPV) * model;

        Mat4 mat;
        CHmsNavSvsMathHelper::GetMatFromMat4d(&mat, &model);
        auto pGLprogram = s_pSvsObjectMgr->m_pGlProgram;
        pGLprogram->setUniformLocationWith3f(s_pSvsObjectMgr->m_locUniformRelativeOrigin, 0, 0, 0);
        pGLprogram->setUniformLocationWithMatrix4fv(s_pSvsObjectMgr->m_locUniformMVP, mat.m, 1);
        pGLprogram->setUniformLocationWith4f(s_pSvsObjectMgr->m_locUniformColor, 1.0, 1.0, 1.0, 1.0);

        glDrawElements(GL_TRIANGLES, m_vecUnitIndexData.size(), GL_UNSIGNED_INT, (void*)0);
    }
    for (int i = 0; i < (int)m_vecData.size(); ++i)
    {
        auto &stu = m_vecData[i];
        if (stu.pLabel)
        {
            stu.pLabel->OnDraw();
        }
    }
}
