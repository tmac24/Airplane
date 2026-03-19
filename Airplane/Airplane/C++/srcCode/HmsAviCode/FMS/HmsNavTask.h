#pragma once
#include "HmsFMSTask.h"

class CHmsDataBus;
class CHmsNavTask : public CHmsFMSTask
{
public:
	CHmsNavTask();
	~CHmsNavTask();

	virtual void excute(float delta) override;

	virtual void OnDuControlDataChanged(DUControlDataMember theChanged);
private:
	CHmsDataBus			*m_dataBus;
	int					m_activeIndex;
	bool				m_isEntered;	//is entered to the select wpt index zone
	bool				m_isChangeCourse;	//是否改变航道 航线变化和非OBS模式下激活航点发生变化时为true，否则false
	std::vector<WptNodeStu2D> m_wayPoints;

	//根据当前位置确定使用的航路点索引 lon lat为飞机当前经纬度坐标
	int CalUseWaypointIndex(double lon, double lat);
	//计算离所给经纬度最近的航路点索引
	int CalNearestPointIndex(double longitude, double latitude);
	//已知当前所处位置p1，当前索引航路点p2，下一个航路点p3，计算p1到过p3点并垂直于p2p3的直线距离
	double CalToNextPointDistance(double p1Long, double p1Lat,
		double p2Long, double p2Lat,
		double p3Long, double p3Lat);
	//根据当前位置判断是否使用下一个航路点
	bool IsUseNextWpt(double lon, double lat);

	void UpdateEnroute(std::vector<WptNodeStu2D> wayPoint);
	void SetDirectToWpt(int wptIndex);
};

