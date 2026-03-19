#include "../HmsGlobal.h"
#include "HmsProfileView.h"
#include "../DataInterface/TerrainServer/HmsGeoMapLoadManager.h"
#include "../Calculate/CalculateZs.h"
#include "../DataInterface/HmsDataBus.h"
#include "../Nav/HmsNavMathHelper.h"
#include "../DataInterface/TerrainServer/HmsTerrainServer.h"
#include "../DataInterface/TerrainServer/HmsTerrainTiffDef.h"

#define PROFILE_SUBDIVSION_CNT 300
#define PROFILE_SUBDIVSION_CNT_BEGIN (-PROFILE_SUBDIVSION_CNT/10)
#define PROFILE_SUBDIVSION_CNT_END (PROFILE_SUBDIVSION_CNT + PROFILE_SUBDIVSION_CNT_BEGIN)

USING_NS_HMS;

CHmsProfileView::CHmsProfileView()
{
    m_distanceKm = 25.0f;
    m_heading = 0.0f;
    m_bUpdateData = true;
    m_bUpdateChart = false;
    m_fLonLatOffset = 0.0f;
    m_nHeightEstTerrain = 0;
    m_fAltMeter = 0.0f;
	m_imageTerrain = nullptr;
}

CHmsProfileView* CHmsProfileView::Create(float width, float height)
{
    CHmsProfileView *pRet = new CHmsProfileView();
    if (pRet && pRet->Init(width, height))
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}

bool CHmsProfileView::Init(float width, float height)
{
    if (!CHmsNode::Init())
    {
        return false;
    }

    SetContentSize(Size(width, height));

    m_pNodeBg = CHmsDrawNode::Create();
    m_pNodeBg->DrawSolidRect(0, 0, width, height, Color4F::BLACK);
    this->AddChild(m_pNodeBg);

    m_pProfileChart = CHmsProfileChart::Create(width - 10, height - 10);
    m_pProfileChart->SetPosition(0, 0);
    m_pProfileChart->SetBorder(true, true, false, false);
    m_pProfileChart->SetBorderColor(Color4B::WHITE);
    this->AddChild(m_pProfileChart);

    return true;
}

bool CHmsProfileView::GetVSDData(const Vec2 & pos, std::vector<HmsAviPf::Vec2> & vPos)
{
    bool bRet = false;

    if ((nullptr == m_imageTerrain) || (m_imageTerrain->getBitPerPixel() != 32))
    {
        return bRet;
    }

    HmsColorCell * data = (HmsColorCell *)(m_imageTerrain->getData());
    int nWidth = m_imageTerrain->getWidth();
    int nHeight = m_imageTerrain->getHeight();
    int nMaxLenth = m_imageTerrain->getDataLen() / 4;
    int nDestX = 0;
    int nDestY = 0;
    int nDestOrgX = nWidth * pos.x;
    int nDestOrgY = nHeight * pos.y;
    m_nHeightEstTerrain = 0;

    auto GetAlt = [=](int nX, int nY, float & fAlt)
    {
        if ((nX >= 0 && nX < nWidth) && (nY >= 0 && nY < nHeight))
        {
            int nIndex = ((nWidth * nY) + nX);
            if (nIndex < nMaxLenth)
            {
                int nData = data[nIndex].rgb;
                if (nData >= 0x8000)
                    fAlt = 0;
                else
                {
                    if (m_nHeightEstTerrain < nData)
                        m_nHeightEstTerrain = nData;
                    fAlt = nData;
                }

                return true;
            }
        }
        return false;
    };

    Vec2 tempPos;
    // 	if (GetAlt(nDestOrgX, nDestOrgY, tempPos.y))
    // 	{
    // 		vPos.push_back(tempPos);
    // 	}

    float yScale = m_mapDstOffset.y / m_mapDstOffset.x;
    float lengthKm = m_distanceKm * 1.25f;
    float fHeadingRad = ToRadian(m_heading);
    float lengthXKm = lengthKm * sin(fHeadingRad);
    float lengthYKm = lengthKm * cos(fHeadingRad) / yScale;
    float lengthXKmDelta = lengthXKm / PROFILE_SUBDIVSION_CNT;
    float lenghtYKmDelta = lengthYKm / PROFILE_SUBDIVSION_CNT;
    int nIndex = 0;

    for (int i = PROFILE_SUBDIVSION_CNT_BEGIN; i <= PROFILE_SUBDIVSION_CNT_END; i++)
    {
        tempPos.x = nIndex++;
        nDestX = nDestOrgX + (int)((i*lengthXKmDelta*nWidth) / m_mapDstOffset.x);
        nDestY = nDestOrgY - (int)((i*lenghtYKmDelta*nHeight) / m_mapDstOffset.y);

        if (GetAlt(nDestX, nDestY, tempPos.y))
            vPos.push_back(tempPos);
        else
            break;
    }

    if (vPos.size())
    {
        return true;
    }

    return false;
}

void CHmsProfileView::Update(float delta)
{
    if (m_bUpdateData && m_imageTerrain && (m_mapRect.size.width > 0))
    {
        auto delta = m_curCenter - m_mapCenter;
        delta = delta / m_fLonLatOffset;

        Vec2 m_vCenterPos;
        m_vCenterPos = Vec2(0.5, 0.5) + Vec2(delta.x, -delta.y);

        std::vector<HmsAviPf::Vec2> vData;
        vData.reserve(302);

        if (m_pProfileChart && GetVSDData(m_vCenterPos, vData))
        {
            m_pProfileChart->SetData(vData, m_nHeightEstTerrain);
        }
        else
        {
            m_pProfileChart->SetDataClear();
        }
    }
}

void CHmsProfileView::OnEnter()
{
    CHmsNode::OnEnter();
    SetScheduleUpdate();
    CHmsGlobal::GetInstance()->GetGeoMapLoadManager()->SetUsingLoading();
}

void CHmsProfileView::OnExit()
{
    CHmsNode::OnExit();
    SetUnSchedule();
    CHmsGlobal::GetInstance()->GetGeoMapLoadManager()->SetUnusedLoading();
}

void CHmsProfileView::SetBackGroundColor(const HmsAviPf::Color4B &color)
{
    m_pNodeBg->clear();
    m_pNodeBg->DrawSolidRect(0, 0, m_sizeContent.width, m_sizeContent.height, Color4F(color));
}

void CHmsProfileView::SetDistanceKm(const float &fDistanceKm)
{
    m_distanceKm = fDistanceKm;
    if (m_pProfileChart)
    {
        m_pProfileChart->SetRange(fDistanceKm);
    }
    m_bUpdateData = true;
}

void CHmsProfileView::SetHeading(const float &fHeading)
{
    if (fabs(m_heading - fHeading) < 0.1)
    {
        return;
    }

    m_heading = fHeading;
    m_bUpdateData = true;
}

void CHmsProfileView::SetAftAltMeter(float fAltMeter)
{
    if (m_pProfileChart)
    {
        m_pProfileChart->SetAftAltMeter(fAltMeter);
    }

    if (fabs(m_fAltMeter - fAltMeter) < 20)
    {
        return;
    }
    m_fAltMeter = m_fAltMeter;
    m_bUpdateData = true;
}

void CHmsProfileView::SetLocation(const double &lon, const double &lat)
{
    if (fabs(m_longitude - lon) < 0.00001 && fabs(m_latitude - lat) < 0.00001)
    {
        return;
    }

    m_longitude = lon;
    m_latitude = lat;
    m_curCenter.setPoint(m_longitude, m_latitude);
    m_bUpdateData = true;
}

void CHmsProfileView::SetTerrainData(HmsAviPf::Image * pTerrainImage, const HmsAviPf::Vec2 & mapCenter, const HmsAviPf::Rect & mapRect, const HmsAviPf::Vec2 & mapDstOffset, float fLonLatOffset)
{
    m_imageTerrain = pTerrainImage;
    m_mapCenter = mapCenter;
    m_mapRect = mapRect;
    m_mapDstOffset = mapDstOffset;
    m_fLonLatOffset = fLonLatOffset * 3;

    //pTerrainImage->saveToFile("D:\\zs123.png");

    auto m_vMapPicRange = Vec2(m_distanceKm / m_mapDstOffset.x, m_distanceKm / m_mapDstOffset.y);
}

void CHmsProfileView::UpdateData()
{
    m_bUpdateData = false;
    m_bUpdateChart = false;
    m_vecDataSource.clear();
    double destLng;
    double destLat;
    CalculatePositionByAngle(destLat, destLng, m_latitude, m_longitude, m_distanceKm, m_heading);

    //0.5nm插值
    float sepDis = 0.5; //nm
    std::vector<Vec2> vecLonLat;
    CHmsNavMathHelper::InsertLonLat(Vec2(m_longitude, m_latitude), Vec2(destLng, destLat), vecLonLat, sepDis * 1.852);//NM TO KM

    float distance = 0;
	for (std::vector<Vec2>::size_type k = 0; k < vecLonLat.size(); k++)
    {
        AltitudeDataStu ads;
        ads.lon = vecLonLat.at(k).x;
        ads.lat = vecLonLat.at(k).y;
        ads.distance = distance;
        distance += sepDis;
        m_vecDataSource.push_back(ads);
    }

    //从地形数据中获取高度信息
    auto server = CHmsGlobal::GetInstance()->GetTerrainServer();
    if (server && m_vecDataSource.size() > 0)
    {
        std::vector<AltitudeDataStu> *vAltData = new std::vector<AltitudeDataStu>;
        if (vAltData)
        {
            vAltData->assign(m_vecDataSource.begin(), m_vecDataSource.end());

            server->GetAltDataByAsyn(vAltData, [=](bool bRet, std::vector<AltitudeDataStu> * vAltData)
            {
                m_vecDislpayData.clear();
                for (int i = 0; i < (int)vAltData->size(); i++)
                {
                    Vec2 item(vAltData->at(i).distance, vAltData->at(i).altitude);
                    //vAltData->at(i).altitude *= 3.2808399f;
                    m_vecDislpayData.push_back(item);
                }

                delete vAltData;
                vAltData = nullptr;
                m_bUpdateChart = true;
            });
        }
    }
}


