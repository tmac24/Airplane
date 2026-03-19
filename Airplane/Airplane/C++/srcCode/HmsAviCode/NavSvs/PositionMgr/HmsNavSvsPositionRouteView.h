#pragma once

#include "HmsNavSvsPosition.h"
#include <vector>
#include "Nav/HmsNavDataStruct.h"
#include "NavSvs/mathd/Vec2d.h"
#include "NavSvs/HmsNavSvsCommDef.h"

class CHmsNavSvsPositionRouteView : public CHmsNavSvsPositionBase
{
public:
    static CHmsNavSvsPositionRouteView* GetInstance();
    virtual void Update(float delta) override;

    void SetRouteData(const std::vector<FPL2DNodeInfoStu> &vec);

    void Play();
    void Pause();
    int ChangeSpeed();

private:
    CHmsNavSvsPositionRouteView();
    ~CHmsNavSvsPositionRouteView();

public:
    std::vector<HmsAviPf::Vec2d> m_vecTmpInsert;
    std::vector<PositionRouteViewData> m_vecData;
    std::vector<double> m_vecYaw;
    float m_yawActionTime;
    int m_curIndex;
    int m_lastIndex;
    int m_speed;
    bool m_bRun;
};