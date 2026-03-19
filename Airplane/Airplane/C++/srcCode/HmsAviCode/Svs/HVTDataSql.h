/*!
 * \file HVTDataSql.h
 *
 * \author yan.jiang
 * \date 一月 2019
 *
 * 提供渲染所需要的高压线以及电塔数据
 * 飞机附近一定距离范围内的高压线以及电塔才显示
 * 数据刷新，按照一定的规则，
 * 规则可以为：飞机距离上一次所在的位置超过一定的距离阈值后就触发数据刷新。
 * 本类只负责数据刷新（不包含vbo刷新）
 */
#pragma once
#include <vector>
#include <map>
#include <set>
extern "C"
{
#include "_Vec3.h"
}
#include <unordered_set>
#include "HVTDataMgr.h"
#include "HmsAviPf.h"
#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_ANDROID)
#include "sqlite3.h"
#elif (HMS_TARGET_PLATFORM == HMS_PLATFORM_LINUX)
#include "sqlite3.h"
#elif (HMS_TARGET_PLATFORM == HMS_PLATFORM_IOS)
#include "sqlite3.h"
#else
#include "sqlite3\include\sqlite3.h"
#endif


struct TowerEndData{
    std::vector<towerinfo> tinfos;
    std::vector<CVec3d> einfos;//end point infos
};

class  HVTDataSql
{
public:
    virtual ~HVTDataSql();
    static HVTDataSql* getInstance();
    TowerEndData GetTowerEndData(const CVec3d& centerLLA, const double& LonRange, const double& LatRange);
    std::vector<Tower> GetTowersData(const CVec3d& centerLLA, const double& LonRange, const double& LatRange);

private:
    HVTDataSql();
    static HVTDataSql* _instance;
    sqlite3* _pDb;

};
