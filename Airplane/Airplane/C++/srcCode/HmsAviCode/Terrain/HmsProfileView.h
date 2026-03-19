#pragma once

#include "base/HmsNode.h"
#include "base/HmsClippingNode.h"
#include "display/HmsStretchImageNode.h"
#include "HmsProfileChart.h"

class CHmsProfileView : public HmsAviPf::CHmsNode
{
public:
    CHmsProfileView();

    static CHmsProfileView* Create(float width, float height);

    virtual bool Init(float width, float height);

    virtual void OnEnter() override;

    virtual void OnExit() override;

    virtual void Update(float delta) override;

    void SetBackGroundColor(const HmsAviPf::Color4B &color);

    void SetDistanceKm(const float &fDistanceKm);

    void SetHeading(const float &fHeading);

    void SetAftAltMeter(float fAltMeter);

    void SetLocation(const double &lon, const double &lat);

    void SetTerrainData(HmsAviPf::Image * pTerrainImage, const HmsAviPf::Vec2 & mapCenter, const HmsAviPf::Rect & mapRect, const HmsAviPf::Vec2 & mapDstOffset, float fLonLatOffset);

    void UpdateData();

protected:
    bool GetVSDData(const HmsAviPf::Vec2 & pos, std::vector<HmsAviPf::Vec2> & vPos);

private:
    HmsAviPf::RefPtr<CHmsProfileChart>	        m_pProfileChart;
    HmsAviPf::RefPtr<HmsAviPf::CHmsDrawNode>	m_pNodeBg;

    float m_distanceKm;
    float m_heading;
    float m_fAltMeter;
    double m_longitude;
    double m_latitude;
    bool m_bUpdateData;
    bool m_bUpdateChart;
    std::vector<AltitudeDataStu>                    m_vecDataSource;
    std::vector<HmsAviPf::Vec2>                     m_vecDislpayData;

    HmsAviPf::Vec2									m_curCenter;
    HmsAviPf::RefPtr<HmsAviPf::Image>				m_imageTerrain;
    HmsAviPf::Vec2									m_mapCenter;
    HmsAviPf::Rect									m_mapRect;
    HmsAviPf::Vec2									m_mapDstOffset;
    float											m_fLonLatOffset;
    int												m_nHeightEstTerrain;
};
