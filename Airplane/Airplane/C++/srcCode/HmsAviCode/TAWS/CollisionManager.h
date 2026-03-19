#pragma once
#include <mutex>
#include "HmsAviMacros.h"
#include "math/Vec2.h"
#include "math/Vec3.h"
#include <vector>
#include "NavSvs/mathd/Vec2d.h"
#include "NavSvs/mathd/Vec3d.h"
#include <unordered_set>

enum class HMS_COLLISION_TARGET_TYPE
{
    TERRAIN = 0,	//0x00
    BARRIER,		//0x01
    INVALID			//0x02
};

#define BARRIER_DEFAULT_HEIGHT 30	//Meter
#define BARRIER_RADIUS_KM 0.015		//barrier radius = 0.015km, 15meter
#define BARRIER_ALARM_HEIGHT_THESHOLD 30//Meter

class CollideInfo{
public:
    bool bExistCollision;//exist collision in a threshold range(not vertical downward) or threshold seconds(vertical downward)
    HmsAviPf::Vec2d vecCollidePos;//longitude & latitude, the location of collision.
    float fCountDownSeconds;//expect a collision in seconds.
    float fCollideDistance;//collide distance meter
    HMS_COLLISION_TARGET_TYPE enCollisionTargetType;

    CollideInfo()
    {
        ResetValue();
    };
    CollideInfo(const CollideInfo& ot)
    {
        bExistCollision = ot.bExistCollision;
        vecCollidePos = ot.vecCollidePos;
        fCountDownSeconds = ot.fCountDownSeconds;
        fCollideDistance = ot.fCollideDistance;
        enCollisionTargetType = ot.enCollisionTargetType;
    }

    CollideInfo& operator=(const CollideInfo& other);

    void ResetValue(){
        bExistCollision = false;
        vecCollidePos = HmsAviPf::Vec2d(0, 0);
        fCountDownSeconds = 3600;
        fCollideDistance = 99999;
        enCollisionTargetType = HMS_COLLISION_TARGET_TYPE::TERRAIN;
    };
};

class CollisionManager
{
public:
    static CollisionManager* getInstance();
    
    void SetSafeDistance(double lowerSafeDistance, double upperSafeDistance, double LeftRightSafeDistance);
    void SetLowerSafeDistance(double lowerSafeDistance);
    void SetDebug(bool dbg);
    void CalcCollisionInfo(const float& delta, const double& curLon, const double& curLat, const float& curAlt, const float& heading, const float& track, const HmsAviPf::Vec3d& speedOfEastNorthSky);
    void CalcSinkRateAlarmState(const double& deltaTime, const double& RadioAltitude);
    CollideInfo CalcBarrierCollisionInfo(const float& delta, const double& longitude, const double& latitude, const double& altitude,
        const double& track, const HmsAviPf::Vec3d& speedOfEastNorthSky);
    void registerColliInfoRefreshCallback(const std::function<void(const CollideInfo& ci)>& callbackCollideGot);
private:
    CollideInfo ColliInfoAnalyse(void);
    CollisionManager(double lowerSafeDistance, double upperSafeDistance, double LeftRightSafeDistance);
    CollisionManager();
    static CollisionManager* _pCollisionMgr;
    double _lowerSafeDistance;		 //down��meter��
    double _upperSafeDistance;		 //up
    double _LeftRightSafeDistance;	 //left and right, �ɻ��ʵ㵽�ɻ���ߵ���ײ����ľ��룬�ұ�Ҳ��˶Գƣ��˾��벢�Ǵ���ߵ㵽�ұߵľ��롣
    bool _bDebug;
    std::mutex _mtx_callbacks_modifying;
    std::vector<std::function<void(const CollideInfo& ci)>> _colliInfoRefreshCallbacks;
    CollideInfo _collideInfoTerr;
    CollideInfo _collideInfoBarr;
};

inline CollideInfo& CollideInfo::operator=(const CollideInfo& other)
{
    this->bExistCollision = other.bExistCollision;
    this->vecCollidePos.x = other.vecCollidePos.x;
    this->vecCollidePos.y = other.vecCollidePos.y;
    this->fCountDownSeconds = other.fCountDownSeconds;
    this->fCollideDistance = other.fCollideDistance;
    this->enCollisionTargetType = other.enCollisionTargetType;
    return *this;
}
