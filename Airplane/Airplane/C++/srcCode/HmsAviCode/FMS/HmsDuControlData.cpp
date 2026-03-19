#include "HmsDuControlData.h"

CHmsDuControlData::CHmsDuControlData()
{
	m_curCourse = 0.0f;
	m_SelectHeading = 0.0f;
	m_selWptIndex = 0;
	m_isOBSMode = true;
	m_metricEnable = false;
	m_hpaUnitEnable = false;
	m_ePfdCompassMode = PFD_COMPASS_DATA_MODE::magneticHeading;
}

CHmsDuControlData::~CHmsDuControlData()
{

}

void CHmsDuControlData::SetCurCourse(float course)
{
	m_curCourse = course;
	MemberChangedHandle(MEMBER_CUR_COURSE);
}

void CHmsDuControlData::SetIsOBSMode(bool isObs)
{
	if (m_isOBSMode != isObs)
	{
		m_isOBSMode = isObs;
		MemberChangedHandle(MEMBER_IS_OBS_MODE);
	}
}

void CHmsDuControlData::SetSelWptIndex(int index)
{
	//if (m_selWptIndex != index)
	{
		m_selWptIndex = index;
		MemberChangedHandle(MEMBER_SEL_WPT_INDEX);
	}
}

void CHmsDuControlData::SetEnroute(std::vector<WptNodeStu2D> enroute)
{
	m_enroute = enroute;
	MemberChangedHandle(MEMBER_WAY_POINTS);
}

void CHmsDuControlData::SetMetricEnable(bool isMetric)
{
	m_metricEnable = isMetric;
	MemberChangedHandle(MEMBER_METRIC_ENABLE);
}

void CHmsDuControlData::SetHpaUnitEnable(bool isHpa)
{
	m_hpaUnitEnable = isHpa;
	MemberChangedHandle(MEMBER_HPA_UNIT_ENABLE);
}

void CHmsDuControlData::SetSelectHeading(float fHeading)
{
	m_SelectHeading = fHeading;
}

float CHmsDuControlData::GetCurCourse()
{
	return m_curCourse;
}

float CHmsDuControlData::GetSelectHeading()
{
	return m_SelectHeading;
}

bool CHmsDuControlData::IsObsMode()
{
	return m_isOBSMode;
}

int CHmsDuControlData::GetSelWptIndex()
{
	return m_selWptIndex;
}

std::vector<WptNodeStu2D> CHmsDuControlData::GetEnroute()
{
	return m_enroute;
}

bool CHmsDuControlData::IsMetricEnable()
{
	return m_metricEnable;
}

bool CHmsDuControlData::IsHpaUnitEnable()
{
	return m_hpaUnitEnable;
}

void CHmsDuControlData::SubscribeMemberChanged(OnMemberChange callback, DUControlDataMember member)
{
	SubscribeMap::iterator iter = m_subscribeMap.find(member);
	if (iter != m_subscribeMap.end())
	{
		m_subscribeMap[member].push_back(callback);
	}
	else
	{
		vector<OnMemberChange> memberList;
		memberList.push_back(callback);
		m_subscribeMap[member] = memberList;
	}
}

void CHmsDuControlData::MemberChangedHandle(DUControlDataMember theChanged)
{
	SubscribeMap::iterator iter = m_subscribeMap.find(theChanged);
	if (iter != m_subscribeMap.end())
	{
		vector<OnMemberChange> memberList = iter->second;
		for (OnMemberChange callback : memberList)
		{
			callback(theChanged);
		}
	}
}

void CHmsDuControlData::UnSubscribeAll()
{
	m_subscribeMap.clear();
}






