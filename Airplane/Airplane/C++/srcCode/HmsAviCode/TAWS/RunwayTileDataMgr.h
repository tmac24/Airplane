#pragma once
#include "math/Vec3.h"
#include "math/Vec2.h"
#include <vector>
#include <map>
#include "base/Ref.h"
#include <set>
#include "TileDataMgrBase.h"

NS_HMS_BEGIN

#pragma pack(1)
typedef struct _RunwayParameters
{
    char ICAO_id[5];/*����������*/
    char ICAO_code[3];/*�������*/
    char runway_id[6];/*�ܵ����*/
    double lat;/*�ܵ���γ��*/
    double lon;
    int len;/*�ܵ���*/
    int width;	/*�ܵ���*/
    int altitude;/*�߶�*/
    double direction;/*�ܵ�����*/
    double gradient;/*�ܵ��ݶ�*/
}RunwayParameters, *pRunwayParameters;

#pragma pack() 


class RunwayBlock : public DataBlock
{
public:
    RunwayBlock(int lon, int lat);
    RunwayBlock(const Vec2&);
    RunwayBlock(const TileKey&);
    ~RunwayBlock();

    const std::vector<RunwayParameters*>& GetRunwayInfos(void);
private:
    std::vector<RunwayParameters*> m_RunwayInfos;/*�����ϰ�������  <AirportParameters>*/
    void InitRunwayBlockData();
};


class OutputInfo{
public:
    unsigned char alarmInfo;//0x00 none, 0x01 yellow alarm, 0x02 red alarm

    char nearestICAO_id[5];/*����������*/
    char nearestRunway_id[6];/*�ܵ����*/

    float nearestAptDist;//meter
    float nearestRunwayDist;//meter
    float nearestRunwayAlt;//meter
    float nearestRunwayAngle;//degree
    float collideDist;//collide distance , if no collide point , it is  65535.00
    OutputInfo(){
        ResetValue();
    }
    void ResetValue(){
        alarmInfo = 0;
        memset(nearestICAO_id, 0, sizeof(nearestICAO_id));
        memset(nearestRunway_id, 0, sizeof(nearestRunway_id));
        nearestAptDist = 99999.9f;
        nearestRunwayDist = 99999.9f;
        nearestRunwayAlt = 0.0f;
        nearestRunwayAngle = 0.0f;
        collideDist = 65535.00;
    }
};


class RunwayTileDataMgr : public TileDataMgrBase
{
public:
    static RunwayTileDataMgr* getInstance();
    const std::set<RunwayBlock*>& GetRenderBlocks();
    OutputInfo CalcNearestRunwayInfo(const double& lon, const double& lat);
private:
    RunwayTileDataMgr();
    static RunwayTileDataMgr* m_pInstance;
    std::set<RunwayBlock*> m_renderBlockBuffer;
    RunwayBlock* CreateDataBlock(const TileKey& tkey) override;
};

NS_HMS_END