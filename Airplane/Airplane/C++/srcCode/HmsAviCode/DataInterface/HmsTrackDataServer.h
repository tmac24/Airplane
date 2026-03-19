#pragma once

#include "Calculate/CalculateZs.h"
#include "Calculate/PositionZs.h"
#include <list>
#include "math/HmsMath.h"
#include "trackData/HmsTrackDataRecord.h"
#include <vector>

struct HmsTrackPosition
{
    EarthCoord2D        pos;        //the position of WGS84
    float               altFt;      //the altitude of feet
};

struct HmsTrackCell
{
    EarthCoord2D        pos;        //the position of WGS84
    HmsAviPf::Vec3      pos3D;      //the position of 3d earth
    bool                bUsed;

    HmsTrackCell()
    {
        bUsed = false;
    }

    void UpdatePos3D();
};

class CHmsTrackData
{
public:
	bool IsDataDirty(){ return m_bListDirty; }
    int UpdatePostion(const HmsTrackPosition & htp);
	void SwallowDirty(){ m_bListDirty = false; }
    std::list<HmsTrackCell> & GetTrackPosList(){ return m_listTrackPos; }
private:
    std::list<HmsTrackCell>		    m_listTrackPos;
	bool					        m_bListDirty;				//显示列表是否更新
};

class CHmsTrackDataServer
{
public:
    CHmsTrackDataServer();

    //check current position every second, 
    bool Update();

    void SetRecordCmd(bool bRecord);

    void FlushCurrentRecord();

    CHmsTrackData * GetCurrentTrackData(){ return &m_data; }
    std::vector<HmsTrackPosition> GetHistoryTrackPos(const std::string & strName);

    static HmsAviPf::Vec3 Get3DPostion(const  EarthCoord2D & ec);

private:
    CHmsTime                m_timeUpdate;
    CHmsTrackData           m_data;
    CHmsTrackDataRecord     m_dataRecord;
    EarthCoord2D            m_ecCurrent;

    std::string             m_strHistoryName;
    std::vector<HmsTrackPosition>   m_vHistoryTrack;
};
