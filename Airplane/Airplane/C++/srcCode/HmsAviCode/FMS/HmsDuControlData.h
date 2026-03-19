#pragma once
#include <string>
#include <vector>
#include <map>
#include <functional>
#include "../Nav/HmsNavDataStruct.h"

enum DUControlDataMember
{
    MEMBER_CUR_COURSE,
    MEMBER_IS_OBS_MODE,
    MEMBER_SEL_WPT_INDEX,
    MEMBER_WAY_POINTS,
    MEMBER_METRIC_ENABLE,
    MEMBER_HPA_UNIT_ENABLE
};

enum class PFD_COMPASS_DATA_MODE
{
    trueTrack,
    trueHeading,
    magneticTrack,
    magneticHeading,
};

using namespace std;
class CHmsDuControlData
{
public:
    typedef std::function<void(DUControlDataMember)> OnMemberChange;

    CHmsDuControlData();
    ~CHmsDuControlData();

    void SetCurCourse(float course);
    void SetIsOBSMode(bool isObs);
    void SetSelWptIndex(int index);
    void SetEnroute(std::vector<WptNodeStu2D> enroute);
    void SetMetricEnable(bool isMetric);
    void SetHpaUnitEnable(bool isHpa);
    void SetSelectHeading(float fHeading);
    void SetPfdCompassDataMode(PFD_COMPASS_DATA_MODE eMode){ m_ePfdCompassMode = eMode; }

    float GetCurCourse();
    float GetSelectHeading();
    bool IsObsMode();
    int GetSelWptIndex();
    vector<WptNodeStu2D> GetEnroute();
    bool IsMetricEnable();
    bool IsHpaUnitEnable();
    PFD_COMPASS_DATA_MODE GetPfdCompassDataMode(){ return m_ePfdCompassMode; }

    void SubscribeMemberChanged(OnMemberChange callback, DUControlDataMember);
    void UnSubscribeAll();//cancel all subscribe
private:
    float m_curCourse;
    float m_SelectHeading;
    bool m_isOBSMode;
    int m_selWptIndex;
    vector<WptNodeStu2D> m_enroute;
    bool m_metricEnable;				//米制激活
    bool m_hpaUnitEnable;				//HPA单位
    PFD_COMPASS_DATA_MODE	m_ePfdCompassMode;

    typedef map<DUControlDataMember, vector<OnMemberChange>> SubscribeMap;
    SubscribeMap m_subscribeMap;
    void MemberChangedHandle(DUControlDataMember theChanged);
};