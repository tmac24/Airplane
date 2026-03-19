
#include "CollisionManager.h"
#include <vector>
#include "HmsGlobal.h"
#include "Calculate/CalculateZs.h"
#include "DataInterface/TerrainServer/HmsTerrainServer.h"
#include "BarrierTileDataMgr.h"
#include "NavSvs/mathd/Vec3d.h"
#include <assert.h>

USING_NS_HMS;


#define THRESHOLD_SPEED 1.0 //Meter/Second
#define THRESHOLD_TEST_RANGE 10 //KM. collision test range
#define THRESHOLD_TEST_VERTICAL_SECONDS (5*60)//Second. vertical collision test time range

CollisionManager::CollisionManager(double lowerSafeDistance, double upperSafeDistance, double LeftRightSafeDistance)
{
    SetSafeDistance(lowerSafeDistance, upperSafeDistance, LeftRightSafeDistance);
}

CollisionManager::CollisionManager()
{
    SetSafeDistance(0.0, 0.0, 0.0);
    SetDebug(false);
}

CollisionManager* CollisionManager::_pCollisionMgr = nullptr;

CollisionManager* CollisionManager::getInstance()
{
    if (nullptr == _pCollisionMgr)
    {
        _pCollisionMgr = new CollisionManager();
    }
    return _pCollisionMgr;
}

//************************************
// Method:    ColliInfoAnlanyse
// FullName:  CollisionManager::ColliInfoAnlanyse
// Access:    public 
// Desc:      融合多种碰撞信息，计算出眼下最危险的碰撞信息
// Returns:   CollideInfo
// Qualifier:
// Parameter: void
// Author:    yan.jiang
//************************************
CollideInfo CollisionManager::ColliInfoAnalyse(void)
{
    CollideInfo output;

    if (!_collideInfoTerr.bExistCollision && !_collideInfoBarr.bExistCollision)
    {
        return output;
    }

    if (_collideInfoTerr.bExistCollision && !_collideInfoBarr.bExistCollision)
    {
        output = _collideInfoTerr;
    }
    else if (!_collideInfoTerr.bExistCollision && _collideInfoBarr.bExistCollision)
    {
        output = _collideInfoBarr;
    }
    else if (_collideInfoTerr.bExistCollision && _collideInfoBarr.bExistCollision)
    {
        //get the closer one
        output = (_collideInfoTerr.fCountDownSeconds < _collideInfoBarr.fCountDownSeconds) ? _collideInfoTerr : _collideInfoBarr;
    }
    return output;
}

void CollisionManager::SetSafeDistance(double lowerSafeDistance, double upperSafeDistance, double LeftRightSafeDistance)
{
    _lowerSafeDistance = MAX(0, lowerSafeDistance);
    _upperSafeDistance = MAX(0, upperSafeDistance);
    _LeftRightSafeDistance = MAX(0, LeftRightSafeDistance);
}

void CollisionManager::SetLowerSafeDistance(double lowerSafeDistance)
{
    _lowerSafeDistance = MAX(0.0, lowerSafeDistance);
}

void CollisionManager::SetDebug(bool dbg)
{
    _bDebug = dbg;
}


#define SIDE_LINE_DEGREE 1//碰撞测试左右边线，向左右两边发散，一边偏离heading 1度，左右共 2度，左边线的出发点为left点，右边线的出发点为right点
//************************************
// Method:    CalcCollisionInfo
// FullName:  CollisionManager::CalcCollisionInfo
// Access:    private 
// Desc:      
// Returns:   void
// Qualifier:
// Parameter: const double & curLon
// Parameter: const double & curLat
// Parameter: const float & curAlt			(meter)
// Parameter: const Vec3 & speedOfEastNorthSky
//					x east speed (m/s)
//					y nroth speed (m/s)
//					z sky speed (m/s)
// Author:    yan.jiang
//************************************
void CollisionManager::CalcCollisionInfo(const float& delta,
    const double& curLon, const double& curLat, const float& curAlt,
    const float& heading, const float& track,
    const HmsAviPf::Vec3d& speedOfEastNorthSky)
{
    //计算出机身包围点
    //下、上、左、右，四个点。（机身质点可以不用了）
    //当飞机水平速度小于阈值时，飞机左右两点，依照heading确定；当飞机水平速度大于等于阈值时，飞机左右两点按照飞行轨迹方向确定。

    double d3k = KMHToKnot(_LeftRightSafeDistance / 1000);

    Vec3d t1LLA = Vec3d(curLon, curLat, curAlt - _lowerSafeDistance);
    Vec3d t2LLA = Vec3d(curLon, curLat, curAlt + _upperSafeDistance);

    Vec3d t3LLA;//left
    Vec3d t4LLA;//right

    std::vector<Vec3d> colLLAs;//collision test point

    colLLAs.push_back(t1LLA);//bottom       0
    colLLAs.push_back(t2LLA);//up           1

    static double horizontalRange = THRESHOLD_TEST_RANGE;

    Vec2d horizontalVelocity(speedOfEastNorthSky.x, speedOfEastNorthSky.y);
    auto hSpeed = horizontalVelocity.getLength();
    auto velocity = speedOfEastNorthSky.length();

    //计算与障碍物的碰撞
    _collideInfoBarr = CalcBarrierCollisionInfo(delta,
        curLon, curLat, curAlt,
        track,
        speedOfEastNorthSky);

    //计算  与地形的碰撞
    if (hSpeed < THRESHOLD_SPEED)//avoid to divide by zero.
    {
        //这种垂直下降的情况下，只考虑下、左、右，三个点，不考虑，左特殊和右特殊点（左右两条发散碰撞线的起点）
        CalculatePositionByAngle(t3LLA.y, t3LLA.x, curLat, curLon, d3k, heading - 90);//left
        CalculatePositionByAngle(t4LLA.y, t4LLA.x, curLat, curLon, d3k, heading + 90);//right
        t3LLA.z = curAlt;
        t4LLA.z = curAlt;
        colLLAs.push_back(t3LLA);//left         2
        colLLAs.push_back(t4LLA);//right        3

        std::vector<double> endPointAltitudes;//检测范围末端各点海拔
        for (const auto &cl : colLLAs)
        {
            double endalt = cl.z + speedOfEastNorthSky.z * THRESHOLD_TEST_VERTICAL_SECONDS;
            endPointAltitudes.push_back(endalt);
        }

        std::vector<AltitudeDataStu> *vAltData = new std::vector<AltitudeDataStu>;
        //vAltData->push_back({ curLon, curLat, 0, 0 });

        for (auto i : { 0, 2, 3 })//bottom, left and right
        {
            AltitudeDataStu ads;
            ads.lon = float(colLLAs[i].x);
            ads.lat = float(colLLAs[i].y);
            ads.distance = 0;
            ads.altitude = 0;
            vAltData->push_back(ads);
        }

        auto server = CHmsGlobal::GetInstance()->GetTerrainServer();
        if (server)
        {
            server->GetAltDataByAsyn(vAltData,
                [=](bool bRet, std::vector<AltitudeDataStu> *vecAltData)
            {
                std::vector<CollideInfo> colInfos;//碰撞点集合
                int indices[3] = { 0, 2, 3 };//bottom, left and right
                int idx = 0;
                for (const auto &groundPointLLA : *vecAltData)
                {
                    auto tpIdx = indices[idx % 3];
                    auto testPointLLA = colLLAs[tpIdx];
                    if (groundPointLLA.altitude > endPointAltitudes[tpIdx])//测试范围末端，比地底。测试范围内有碰撞点，需要进一步判断测试点
                    {
                        CollideInfo ci;
                        ci.bExistCollision = true;
                        ci.vecCollidePos = Vec2d(groundPointLLA.lon, groundPointLLA.lat);

                        auto collidistVec = groundPointLLA.altitude - testPointLLA.z;//collidistVec为正表示已经碰撞
                        ci.fCollideDistance = HMS_MAX(0, -collidistVec);                            //碰撞距离
                        ci.fCountDownSeconds = HMS_MAX(0, (collidistVec / speedOfEastNorthSky.z));  //碰撞倒计时

                        ci.enCollisionTargetType = HMS_COLLISION_TARGET_TYPE::TERRAIN;
                        if (_bDebug)
                        {
                            std::cout << "Vcoli pos:" << ci.vecCollidePos << "\tcoli ct:" << ci.fCountDownSeconds
                                << "\tcoli cd:" << ci.fCollideDistance << "\tp=" << tpIdx << std::endl;
                        }
                        colInfos.push_back(ci);
                    }
                    idx++;
                }

                //choose the nearest collision point
                CollideInfo ci;

                if (colInfos.size())
                {//测试范围内存在碰撞点
                    auto nearestci = min_element(colInfos.begin(), colInfos.end(),
                        [](const CollideInfo& c1, const CollideInfo& c2){
                        return c1.fCountDownSeconds < c2.fCountDownSeconds;
                    });
                    ci = *nearestci;
                }
                else
                {//无碰撞点
                    ci.bExistCollision = false;
                }

                colInfos.clear();

                _collideInfoTerr = ci;

                auto output = ColliInfoAnalyse();

                _mtx_callbacks_modifying.lock();
                for (auto& cb : _colliInfoRefreshCallbacks)
                {
                    cb(output);
                }
                _mtx_callbacks_modifying.unlock();
                HMS_SAFE_DELETE(vecAltData);
            });
        }
    }
    else
    {
        double horizontalAngle = 0.0;//coordinate is the same with heading(north is 0, east is 90, south 180, west is 270)
        horizontalAngle = 90 - ToAngle(atan2(speedOfEastNorthSky.y, speedOfEastNorthSky.x));

        CalculatePositionByAngle(t3LLA.y, t3LLA.x, curLat, curLon, d3k, horizontalAngle - 90);//left
        CalculatePositionByAngle(t4LLA.y, t4LLA.x, curLat, curLon, d3k, horizontalAngle + 90);//right
        t3LLA.z = curAlt;
        t4LLA.z = curAlt;
        colLLAs.push_back(t3LLA);//left         2
        colLLAs.push_back(t4LLA);//right        3
        colLLAs.push_back(t3LLA);//left 边线的出发点与left点相同（有点浪费性能）          4
        colLLAs.push_back(t4LLA);//right 边线的出发点与rigth点相同（有点浪费性能）        5

        auto timeSpend = horizontalRange * 1000 / hSpeed;//seconds

        double endlon;//检测范围末端的位置
        double endlat;//检测范围末端的位置
        CalculatePositionByAngle(endlat, endlon, curLat, curLon, KMHToKnot(horizontalRange), horizontalAngle);

        static int sampleNumber = 512;

        auto horizontalStep = (Vec2d(endlon, endlat) - Vec2d(curLon, curLat)) / sampleNumber;//质点的运动方向即所有测试点的运动方向。
        auto verticalStep = timeSpend * speedOfEastNorthSky.z / sampleNumber;

        //左右两条发散的碰撞边线单独计算
        //left special
        CalculatePositionByAngle(endlat, endlon, curLat, curLon, KMHToKnot(horizontalRange), horizontalAngle - SIDE_LINE_DEGREE);
        auto horizontalStepLeft = (Vec2d(endlon, endlat) - Vec2d(curLon, curLat)) / sampleNumber;
        //rigth special
        CalculatePositionByAngle(endlat, endlon, curLat, curLon, KMHToKnot(horizontalRange), horizontalAngle + SIDE_LINE_DEGREE);
        auto horizontalStepRight = (Vec2d(endlon, endlat) - Vec2d(curLon, curLat)) / sampleNumber;
        //左右两特殊碰撞线的verticalStep 和普通线的一致

        std::vector<AltitudeDataStu> *vAltData = new std::vector<AltitudeDataStu>;

        for (int i = 0; i < sampleNumber; i++)
        {
            for (auto j : { 0, 2, 3 })//下、左、右，下、左、右，下、左、右，下、左、右，下、左、右，....
            {
                auto p = colLLAs[j];
                auto testPos = Vec2d(p.x, p.y) + i * horizontalStep;
                vAltData->push_back({ float(testPos.x), float(testPos.y), 0, 0 });
            }

            Vec2d testPos;
            //特殊左            
            testPos = Vec2d(colLLAs[4].x, colLLAs[4].y) + i * horizontalStepLeft;
            vAltData->push_back({ (float)testPos.x, (float)testPos.y, 0, 0 });
            //特殊右            
            testPos = Vec2d(colLLAs[5].x, colLLAs[5].y) + i * horizontalStepRight;
            vAltData->push_back({ (float)testPos.x, (float)testPos.y, 0, 0 });
        }

        auto server = CHmsGlobal::GetInstance()->GetTerrainServer();
        if (server)
        {
            server->GetAltDataByAsyn(vAltData,
                [=](bool bRet, std::vector<AltitudeDataStu> *vecAltData)
            {
                int indices[5] = { 0, 2, 3, 4, 5 };
                CollideInfo ci;
                int idx = 0;
                ci.bExistCollision = false;
                for (const auto &groundPointLLA : *vecAltData)
                {
                    auto testPointLLA = colLLAs[indices[idx % 5]];
                    auto alt = testPointLLA.z + verticalStep * idx / 5;
                    if (groundPointLLA.altitude > alt)
                    {
                        ci.bExistCollision = true;
                        ci.vecCollidePos = Vec2d(groundPointLLA.lon, groundPointLLA.lat);
                        auto distHoriz = CalculateDistanceM(testPointLLA.x, testPointLLA.y, groundPointLLA.lon, groundPointLLA.lat);// 碰撞距离;
                        auto dist3D = Vec2d(distHoriz, curAlt - groundPointLLA.altitude);
                        ci.fCollideDistance = dist3D.getLength();
                        ci.fCountDownSeconds = ci.fCollideDistance / velocity;//seconds
                        ci.enCollisionTargetType = HMS_COLLISION_TARGET_TYPE::TERRAIN;
                        if (_bDebug)
                        {
                            std::cout << "coli pos:" << ci.vecCollidePos << "\tcoli ct:" << ci.fCountDownSeconds
                                << "\tcoli cd:" << ci.fCollideDistance << "\tp=" << indices[idx % 5]
                                << "\tga=" << groundPointLLA.altitude << " pa=" << alt << " ca=" << curAlt
                                << std::endl;
                        }
                        break;
                    }
                    idx++;
                }

                _collideInfoTerr = ci;

                auto output = ColliInfoAnalyse();
                _mtx_callbacks_modifying.lock();
                for (auto& cb : _colliInfoRefreshCallbacks)
                {
                    cb(output);
                }
                _mtx_callbacks_modifying.unlock();
                HMS_SAFE_DELETE(vecAltData);
            });
        }
    }
}

void CollisionManager::CalcSinkRateAlarmState(const double& deltaTime, const double& RadioAltitude)
{
    static double s_radioAltimeter = RadioAltitude;
    static double s_sinkrate = 0;
    if (MeterToFeet(RadioAltitude) < 2450
        && MeterToFeet(RadioAltitude > 30)
        )
    {
        s_sinkrate = (RadioAltitude - s_radioAltimeter) * 60 / deltaTime;//feet per min
        if (s_sinkrate > (MeterToFeet(RadioAltitude) - 30) * (5000 - 1000) / (2450 - 30) + 1000)
        {
            //Sink Rate Alarm!!!
            if (_bDebug)
            {
                std::cout << "ALARM: SINK RATE! RA(" << MeterToFeet(RadioAltitude) << ") sink rate(" << s_sinkrate << ")" << std::endl;
            }
        }
        s_radioAltimeter = RadioAltitude;
    }
}


//************************************
// Method:    CalcBarrierCollisionInfo
// FullName:  CollisionManager::CalcBarrierCollisionInfo
// Access:    public 
// Desc:      飞行轨迹与障碍物碰撞检测，障碍物几何形状等效为圆柱体（无底）
// Returns:   CollideInfo
// Qualifier:
// Parameter: const float & delta
// Parameter: const double & longitude
// Parameter: const double & latitude
// Parameter: const double & altitude
// Parameter: const double & track
// Parameter: const double & vs
// Parameter: const double & gs
// Author:    yan.jiang
//************************************
CollideInfo CollisionManager::CalcBarrierCollisionInfo(const float& delta, const double& longitude, const double& latitude, const double& altitude,
    const double& track, const HmsAviPf::Vec3d& speedOfEastNorthSky)
{
    Vec2d horizontalVelocity(speedOfEastNorthSky.x, speedOfEastNorthSky.y);
    auto hSpeed = horizontalVelocity.getLength();

    auto gsMps = hSpeed;
    auto verticalSpeedMps = speedOfEastNorthSky.z;//convert to meter per second
    auto alt_meter = altitude;//convert to meter per second

    static float cum_delta = 0;//second
    static std::vector<BarrierParameters*> s_bps = BarrierTileDataMgr::getInstance()->GetNearestBarrierInfors(Vec2(longitude, latitude));

    //飞机测试点由一质点改为添加下、上、左、右四个质点。等效成修改障碍物几何尺寸。

    CollideInfo ci;

    cum_delta += delta;
    if (cum_delta > 10.0)
    {
        cum_delta = 0;
        s_bps = BarrierTileDataMgr::getInstance()->GetNearestBarrierInfors(Vec2(longitude, latitude));
    }

    unsigned int maxCalcNum = 200;//if more than this number, skip and no more caculating.
    Vec2d p0(longitude, latitude);

    auto KmPerLat = CHmsGlobal::GetKmPerLatitude();
    //double r = 8.993E-5;
    double r = (BARRIER_RADIUS_KM + _LeftRightSafeDistance / 1000.0) / KmPerLat;//degree,  barrier radius

    double destlon;
    double destlat;

    //CalculatePositionByAngle(destlat, destlon, latitude, longitude, 5, track);
    //原来是：以飞机质心为原点，向飞机航迹方向（track）发射出射线。计算此射线与障碍物圆柱体的相交关系。
    //现在改为两条射线，分别为向航迹的左边偏离1度，以及向航迹方向右边偏离1度。这两条射线，计算其与障碍物圆柱体的相交关系。
    CalculatePositionByAngle(destlat, destlon, latitude, longitude, 5, track - 1);
    Vec2d dirLeft = Vec2d(destlon - longitude, destlat - latitude);
    CalculatePositionByAngle(destlat, destlon, latitude, longitude, 5, track + 1);
    Vec2d dirRight = Vec2d(destlon - longitude, destlat - latitude);

    dirLeft.normalize();
    dirRight.normalize();

    for (const auto &bp : s_bps)
    {
        if (0 == maxCalcNum--)
        {
            break;
        }
        Vec2d c(bp->longitude, bp->latitude);

        auto e = c - p0;

        auto len = e.getLength();//degree approixmate

        auto aLeft = e.dot(dirLeft);
        auto aRight = e.dot(dirRight);

        //射线与圆相交
        if (
            (
            aLeft > 0 //圆在dirLeft方向的前方
            &&
            r*r - e.dot(e) + aLeft*aLeft >= 0 //左线与圆相交
            )
            ||
            (
            aRight > 0//圆在dirRight方向的前方
            &&
            r*r - e.dot(e) + aRight*aRight >= 0 //右线与圆相交
            )
            )
        {
            //horizontal collision (xy flat without considering z, the altitude )

            //now get the altitude of the pos 
            double barrier_bottom_alt = 0;
#if 0//当障碍物数据中的障碍物海拔，与，障碍物经纬通过高程数据查询到的地表海拔不一致时，目前使用障碍物数据的海拔作为障碍物的底部海拔。
            auto pRawTerrainData = CHmsGlobal::GetInstance()->GetTerrainServer()->GetTerrainRawData(c);
            alt = pRawTerrainData->GetAltitude(c);
#else
            barrier_bottom_alt = bp->altitude;
#endif

            auto horizontalDistance = CalculateDistanceM(bp->longitude, bp->latitude, longitude, latitude);

            auto ETA_sec = horizontalDistance / gsMps;//sec

            auto EstimatedAltitudeOfArrival = alt_meter + ETA_sec * verticalSpeedMps;

            auto top = barrier_bottom_alt + ((bp->height) ? bp->height : BARRIER_DEFAULT_HEIGHT);
            if ((EstimatedAltitudeOfArrival < top + _lowerSafeDistance)
                //&& (EstimatedAltitudeOfArrival > alt)
                )
            {
                //the collisino will happen.
                //generate collision infomation

                //todo
                ci.bExistCollision = true;
                ci.enCollisionTargetType = HMS_COLLISION_TARGET_TYPE::BARRIER;
                ci.vecCollidePos = c;
                ci.fCollideDistance = horizontalDistance;
                ci.fCountDownSeconds = ETA_sec;

                //no need to caculate the latter point
                break;
            }
        }
    }

    if (_bDebug && ci.bExistCollision)
    {
        std::cout << "barrier colli" << Vec2(ci.vecCollidePos.x, ci.vecCollidePos.y) << " ctd=" << ci.fCountDownSeconds << std::endl;
    }

    return ci;
}

void CollisionManager::registerColliInfoRefreshCallback(const std::function<void(const CollideInfo& ci)>& callbackCollideGot)
{
    _mtx_callbacks_modifying.lock();
    _colliInfoRefreshCallbacks.push_back(callbackCollideGot);//todo: 去重
    _mtx_callbacks_modifying.unlock();
}
