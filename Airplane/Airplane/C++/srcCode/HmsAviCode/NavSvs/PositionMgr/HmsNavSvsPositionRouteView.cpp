#include "HmsNavSvsPositionRouteView.h"
#include "NavSvs/HmsNavSvsMathHelper.h"
#include "Calculate/CalculateZs.h"
#include "NavSvs/HmsNavSvsLoadDataThread.h"
#include "NavSvs/NavSvsObject/HmsNavSvsObjectMgr.h"
#include "NavSvs/NavSvsObject/HmsNavSvsObjectRouteTrack.h"

CHmsNavSvsPositionRouteView::CHmsNavSvsPositionRouteView()
{
    m_type = NavSvsPositionMgrType::POSMGR_ROUTE;
    m_yawActionTime = 0;
    m_curIndex = 0;
    m_lastIndex = -1;
    m_speed = 1;
    m_bRun = false;

    m_vecTmpInsert.reserve(50000);
    m_vecData.reserve(50000);
}

CHmsNavSvsPositionRouteView::~CHmsNavSvsPositionRouteView()
{

}

CHmsNavSvsPositionRouteView* CHmsNavSvsPositionRouteView::GetInstance()
{
    static CHmsNavSvsPositionRouteView *s_pIns = nullptr;
    if (!s_pIns)
    {
        s_pIns = new CHmsNavSvsPositionRouteView;
    }
    return s_pIns;
}

void CHmsNavSvsPositionRouteView::Update(float delta)
{
    if (m_vecData.empty() || m_curIndex < 0 || m_curIndex >= (int)m_vecData.size())
    {
        return;
    }
    auto &stu = m_vecData[m_curIndex];
    m_lon = stu.lon;
    m_lat = stu.lat;
    m_height = stu.height + 50;

    static float time = 0;
    if (m_bRun)
    {
        if (m_curIndex < (int)m_vecData.size() - 1)
        {
            auto &nextStu = m_vecData[m_curIndex + 1];

            float len = CalculateDistanceKM(stu.lon, stu.lat, nextStu.lon, nextStu.lat);
            float needTime = 0;
            int count = 0;
            //两个点相等
            if (len < 0.001)
            {
                count = 1;
            }
            else
            {
                needTime = len / (500.0 * m_speed / 3600);
                time += delta;
                count = time / needTime;
            }        
            if (count > 0)
            {
                time = 0;
                m_curIndex += count;

                m_lon = nextStu.lon;
                m_lat = nextStu.lat;
                m_height = nextStu.height + 50;
            }
            else
            {
                Vec2d lonLat1 = Vec2d(stu.lon, stu.lat);
                Vec2d lonLat2 = Vec2d(nextStu.lon, nextStu.lat);

                const double earthR = EarthRadiusM / 1000.0; //km
                double bR = MATH_DEG_TO_RAD_DOUBLE(90 - lonLat1.y);
                double aR = MATH_DEG_TO_RAD_DOUBLE(90 - lonLat2.y);
                double cR = HmsMath_acos(cos(bR) * cos(aR) + sin(bR) * sin(aR) * cos(MATH_DEG_TO_RAD_DOUBLE(lonLat2.x - lonLat1.x)));

                Vec3d pos1 = CHmsNavSvsMathHelper::LonLatToEarthPoint_Double(&lonLat1);
                Vec3d pos2 = CHmsNavSvsMathHelper::LonLatToEarthPoint_Double(&lonLat2);

                Vec3d axis;
                Vec3d::cross(pos1, pos2, &axis);
                axis.normalize();
                Quaterniond q = Quaterniond(axis, cR * (time / needTime));
                Vec3d pos3 = q * pos1;

                auto ll = CHmsNavSvsMathHelper::EarthPointToLonLat_Double(&pos3);

                m_lon = ll.x;
                m_lat = ll.y;
                m_height = stu.height + (nextStu.height - stu.height) * (time / needTime) + 50; 
            }
        } 
        if (m_curIndex < (int)m_vecData.size() - 1)
        {
            auto &curStu = m_vecData[m_curIndex];
            auto &nextStu = m_vecData[m_curIndex + 1];
            float len = CalculateDistanceKM(curStu.lon, curStu.lat, nextStu.lon, nextStu.lat);
            if (len > 0.01)
            {
                double angle = CalculateAngle(curStu.lon, curStu.lat, nextStu.lon, nextStu.lat);
                angle = MATH_RAD_TO_DEG_DOUBLE(angle);
                if (angle < 0)
                {
                    angle += 360;
                }
                double yaw = angle;

                if (m_lastIndex != m_curIndex)
                {
                    m_lastIndex = m_curIndex;

                    if (fabs(m_yaw - yaw) > 5)
                    {
                        if (!m_vecYaw.empty())
                        {
                            m_yaw = m_vecYaw.back();
                        }
                        m_vecYaw.clear();
                        m_vecYaw.push_back(yaw);
                    }
                    else
                    {
                        m_yaw = yaw;
                    }
                }
            }      
        }
        auto height = GetAltAboveGround(m_lon, m_lat, m_height);
        if (m_height < height)
        {
            m_height = height + 50;
        }

        if (!m_vecYaw.empty())
        {
            float actionTime = 2;
            m_yawActionTime += delta;
            if (m_yawActionTime > actionTime)
            {
                m_yawActionTime = 0;
                m_yaw = m_vecYaw.back();
                m_vecYaw.clear();
            }
            else
            {
                double nextYaw = m_vecYaw.back();
                double curYaw = m_yaw;
                if (fabs(nextYaw - curYaw) > 180)
                {
                    if (curYaw < nextYaw)
                    {
                        curYaw += 360;
                    }
                    else
                    {
                        nextYaw += 360;
                    }
                }
                curYaw = curYaw + (nextYaw - curYaw) / actionTime * m_yawActionTime;

                if (curYaw > 360)
                {
                    curYaw -= 360;
                }
                if (curYaw < 0)
                {
                    curYaw += 360;
                }
                m_yaw = curYaw;
            }
        }
    } 
}

void CHmsNavSvsPositionRouteView::SetRouteData(const std::vector<FPL2DNodeInfoStu> &vec)
{
    std::vector<PositionRouteViewData> vecData;
    bool bExit = false;
    for (auto &c : vec)
    {
        for (auto &e : c.vWayPoint)
        {
            if (e.brief.attr.searchShape.type == SHAPE_END
                || e.ctrlPoint.eFixType == FIX_TYPE_HOLD_TIME 
                || e.ctrlPoint.eFixType == FIX_TYPE_HOLD_DEST)
            {
                for (auto &pos : e.vecInsertEarthPos)
                {
                    Vec3d earthPos = Vec3d(pos.x, pos.y, pos.z);
                    auto d = CHmsNavSvsMathHelper::EarthPointToLonLat_Double(&earthPos);

                    PositionRouteViewData stu;
                    stu.lon = d.x;
                    stu.lat = d.y;
                    vecData.push_back(stu);
                }
                if (!e.vecInsertEarthPos.empty())
                {
                    vecData.back().ident = e.brief.ident;
                    vecData.back().bGetTerrainAlt = false;
                }
            }
            else
            {
                PositionRouteViewData stu;
                stu.lon = e.brief.lon;
                stu.lat = e.brief.lat;
                stu.height = FeetToMeter(e.brief.attr.altStu->nAltitude);
                stu.ident = e.brief.ident;
                vecData.push_back(stu);
            }
            if (e.ctrlPoint.eFixType == FIX_TYPE_HOLD_TIME
                || e.ctrlPoint.eFixType == FIX_TYPE_HOLD_DEST)
            {
                bExit = true;
            }
            if (bExit)
            {
                break;
            }
        }
        if (bExit)
        {
            break;
        }
    }

    //起点终点大于地形高度
    if (vecData.size() >= 2)
    {
        if (vecData.front().bGetTerrainAlt)
        {
            vecData.front().height = GetAltAboveGround(vecData.front().lon, vecData.front().lat, vecData.front().height);
        }
        if (vecData.back().bGetTerrainAlt)
        {
            vecData.back().height = GetAltAboveGround(vecData.back().lon, vecData.back().lat, vecData.back().height);
        }
    }

    m_vecData.clear();
    double curHeight = -1;
    double nextHeight = -1;
    for (int i = 0; i < (int)(vecData.size() - 1); ++i)
    {
        auto &curStu = vecData[i];
        auto &nextStu = vecData[i + 1];
        Vec2d lonlat1 = Vec2d(curStu.lon, curStu.lat);
        Vec2d lonlat2 = Vec2d(nextStu.lon, nextStu.lat);
        if (curStu.height > 0)
        {
            curHeight = curStu.height;
        }
        if (nextStu.height > 0)
        {
            nextHeight = nextStu.height;
        }
        if (curHeight < 0 && nextHeight > 0)
        {
            curHeight = nextHeight;
        }
        if (curHeight > 0 && nextHeight < 0)
        {
            nextHeight = curHeight;
        }
        if (curHeight < 0 && nextHeight < 0)
        {
            curHeight = 2000;
            nextHeight = 2000;
        }

        m_vecTmpInsert.clear();
        CHmsNavSvsMathHelper::InsertLonLat_Double(&lonlat1, &lonlat2, m_vecTmpInsert, 2);
        for (int k = 0; k < (int)m_vecTmpInsert.size(); ++k)
        {
            auto &c = m_vecTmpInsert[k];
            PositionRouteViewData stu;
            stu.lon = c.x;
            stu.lat = c.y;
            stu.height = curHeight + k * ((nextHeight - curHeight) / (m_vecTmpInsert.size() - 1));
            m_vecData.push_back(stu);
        }
        if (i == 0)
        {
            m_vecData.front().ident = curStu.ident;
            m_vecData.back().ident = nextStu.ident;
        }
        else
        {
            m_vecData.back().ident = nextStu.ident;
        }
    }
    m_curIndex = 0;
    m_lastIndex = -1;
    m_yawActionTime = 0;

    auto pBase = CHmsNavSvsObjectMgr::GetInstance()->GetNavSvsObject(NavSvsObjectType::OBJECTTYPE_ROUTETRACK);
    auto pRouteTrack = dynamic_cast<CHmsNavSvsObjectRouteTrack*>(pBase);
    if (pRouteTrack)
    {
        pRouteTrack->SetEnable(true);
        pRouteTrack->SetData(m_vecData);
    }
}

void CHmsNavSvsPositionRouteView::Play()
{
    m_bRun = true;

    if (m_vecData.size() > 0 && m_curIndex >= ((int)m_vecData.size() - 1))
    {
        m_curIndex = 0;
        m_lastIndex = -1;
        m_yawActionTime = 0;
    }
}

void CHmsNavSvsPositionRouteView::Pause()
{
    m_bRun = false;
}

int CHmsNavSvsPositionRouteView::ChangeSpeed()
{
    if (m_speed == 1)
    {
        m_speed = 5;
    }
    else
    {
        m_speed += 5;
    }
    if (m_speed > 15)
    {
        m_speed = 1;
    }
    return m_speed;
}
