#include "HmsNavTask.h"
#include "../Calculate/CalculateZs.h"
#include "DataInterface/HmsDataBus.h"
#include "../HmsGlobal.h"
#include "DataInterface/SmartAviDataDef.h"
#include "HmsFmsDataDef.h"

#define WPT_ENTER_RADIUS 1.0   //航路点进入半径 1.0海里

CHmsNavTask::CHmsNavTask()
{
	m_isEntered = false;
	m_isChangeCourse = true;
	m_activeIndex = -1;
	m_dataBus = CHmsGlobal::GetInstance()->GetDataBus();
}

CHmsNavTask::~CHmsNavTask()
{

}

void CHmsNavTask::excute(float delta)
{
	int wayPointsCount = m_wayPoints.size();
	auto gpsData = m_dataBus->GetData();
	if (wayPointsCount == 0 || gpsData->bLatLonValid == false)
	{
		FmsCalData data;
		data.valid = false;
		m_dataBus->SetFmsData(data);
		return;
	}

	if (m_activeIndex >= 0)
	{
		auto ctrlData = m_dataBus->GetDuControlData();

		if (ctrlData)
		{
			double curLon = gpsData->longitude;
			double curLat = gpsData->latitude;

			FmsCalData data;
			data.valid = true;
			data.phaseOfFlight = "ENR";
			float cdiRatio = 0.0f;			//偏离比例
			float course = 0.0f;			//航道
			double xtk = 0.0f;				//航道偏离距离

			double disToActiveWpt = 0.0f;	//当前点至激活航点距离
			float calCourseValue = ctrlData->GetCurCourse() + 180;
			calCourseValue = calCourseValue > 360.0f ? calCourseValue - 360.0f : calCourseValue;
			double courseRadian = ToRadian(calCourseValue);		//航道弧度
			double flyRadian = 0.0f;		//当前点与激活点连线的弧度
			if (ctrlData->IsObsMode())
			{
				if (m_activeIndex >= 0 && m_activeIndex <= wayPointsCount - 1)
				{
					WptNodeStu2D activePnt = m_wayPoints.at(m_activeIndex);

					flyRadian = CalculateAngle(activePnt.brief.lon, activePnt.brief.lat, curLon, curLat);
					disToActiveWpt = CalculateDistanceNM(activePnt.brief.lon, activePnt.brief.lat, curLon, curLat);
				}
			}
			else
			{
				if (IsUseNextWpt(curLon, curLat))
				{
					m_activeIndex = m_activeIndex + 1;
					m_isChangeCourse = true;
					m_isEntered = false;
				}

				if (m_activeIndex > 0 && m_activeIndex <= wayPointsCount - 1)
				{
					WptNodeStu2D frontPnt = m_wayPoints.at(m_activeIndex - 1);
					WptNodeStu2D activePnt = m_wayPoints.at(m_activeIndex);

					if (m_isChangeCourse)
						courseRadian = CalculateAngle(activePnt.brief.lon, activePnt.brief.lat, frontPnt.brief.lon, frontPnt.brief.lat);
					flyRadian = CalculateAngle(activePnt.brief.lon, activePnt.brief.lat, curLon, curLat);
					disToActiveWpt = CalculateDistanceNM(activePnt.brief.lon, activePnt.brief.lat, curLon, curLat);
				}
				else if (m_activeIndex == 0 && wayPointsCount > 1)
				{
					WptNodeStu2D activePnt = m_wayPoints.at(m_activeIndex);
					WptNodeStu2D nextPnt = m_wayPoints.at(m_activeIndex + 1);

					if (m_isChangeCourse)
						courseRadian = CalculateAngle(nextPnt.brief.lon, nextPnt.brief.lat, activePnt.brief.lon, activePnt.brief.lat);
					flyRadian = CalculateAngle(activePnt.brief.lon, activePnt.brief.lat, curLon, curLat);
					disToActiveWpt = CalculateDistanceNM(activePnt.brief.lon, activePnt.brief.lat, curLon, curLat);
				}
			}

			xtk = std::sin(flyRadian - courseRadian) * disToActiveWpt;
			cdiRatio = NumberMap(xtk, -5.0f, 5.0f, -2, 2);

			course = ToAngle(courseRadian) + 180;
			course = course > 360.0f ? course - 360.0f : course;

			data.cdiRatio = cdiRatio;
			data.changeCourse = m_isChangeCourse;
			if (m_isChangeCourse) data.course = course;
			data.flySegment = m_activeIndex;
			m_dataBus->SetFmsData(data);

			//m_isChangeCourse = false;
		}		
	}
}

bool CHmsNavTask::IsUseNextWpt(double lon, double lat)
{
	if (m_activeIndex >= 0 && m_activeIndex < (int)m_wayPoints.size() - 1)
	{
		WptNodeStu2D useWpt = m_wayPoints.at(m_activeIndex);

		if (!m_isEntered)
		{
			double distance_cur_use = CalculateDistanceNM(lon, lat, useWpt.brief.lon, useWpt.brief.lat);
			if ((distance_cur_use - WPT_ENTER_RADIUS) < 0.000001)
			{
				m_isEntered = true;
			}
		}

		if (m_isEntered)
		{
			WptNodeStu2D nextWpt = m_wayPoints.at(m_activeIndex + 1);
			double distance = CalToNextPointDistance(lon, lat, useWpt.brief.lon, useWpt.brief.lat, nextWpt.brief.lon, nextWpt.brief.lat);
			double p2p3 = CalculateDistanceNM(useWpt.brief.lon, useWpt.brief.lat, nextWpt.brief.lon, nextWpt.brief.lat);
			if (distance - p2p3 < 0.000001)
			{
				return true;
			}
		}
	}

	return false;
}

int CHmsNavTask::CalUseWaypointIndex(double lon, double lat)
{
	int nearestIndex = CalNearestPointIndex(lon, lat);
	int useIndex = nearestIndex;
	if (nearestIndex < (int)m_wayPoints.size() - 1)
	{
		WptNodeStu2D p2 = m_wayPoints.at(nearestIndex);
		WptNodeStu2D p3 = m_wayPoints.at(nearestIndex + 1);
		double distance = CalToNextPointDistance(lon, lat, p2.brief.lon, p2.brief.lat, p3.brief.lon, p3.brief.lat);
		double p2p3 = CalculateDistanceNM(p2.brief.lon, p2.brief.lat, p3.brief.lon, p3.brief.lat);
		if (distance - p2p3 < 0.000001)
		{
			useIndex = nearestIndex + 1;
		}
	}
	return useIndex;
}

int CHmsNavTask::CalNearestPointIndex(double longitude, double latitude)
{
	int count = m_wayPoints.size();
	if (count == 0) return -1;

	if (count == 1) return 0;

	int index = 0;
	WptNodeStu2D first = m_wayPoints.at(0);
	double distance = CalculateDistanceNM(first.brief.lon, first.brief.lat, longitude, latitude);
	for (int i = 1; i < (int)m_wayPoints.size(); i++)
	{
		WptNodeStu2D pnt = m_wayPoints.at(i);
		double temp = CalculateDistanceNM(pnt.brief.lon, pnt.brief.lat, longitude, latitude);
		if (temp - distance < 0.000001)
		{
			distance = temp;
			index = i;
		}
	}

	return index;
}

double CHmsNavTask::CalToNextPointDistance(double p1Long, double p1Lat, double p2Long, double p2Lat, double p3Long, double p3Lat)
{
	double angle1 = CalculateAngle(p1Long, p1Lat, p3Long, p3Lat);
	double angle2 = CalculateAngle(p2Long, p2Lat, p3Long, p3Lat);
	double angle = fabs(angle1 - angle2);
	double p1p3 = CalculateDistanceNM(p1Long, p1Lat, p3Long, p3Lat);
	return std::cos(angle) * p1p3;
}

void CHmsNavTask::OnDuControlDataChanged(DUControlDataMember theChanged)
{
	if (theChanged == MEMBER_SEL_WPT_INDEX)
	{
		SetDirectToWpt(m_dataBus->GetDuControlData()->GetSelWptIndex());
	}
	else if (theChanged == MEMBER_WAY_POINTS)
	{
		UpdateEnroute(m_dataBus->GetDuControlData()->GetEnroute());
	}
}

void CHmsNavTask::SetDirectToWpt(int wptIndex)
{
	if (wptIndex >= 0 && wptIndex < (int)m_wayPoints.size())
	{
		m_activeIndex = wptIndex;
		m_isEntered = false;
		m_isChangeCourse = true;
	}
}

void CHmsNavTask::UpdateEnroute(std::vector<WptNodeStu2D> wayPoint)
{
	m_wayPoints = wayPoint;
	if (m_wayPoints.size() == 0)
	{
		m_activeIndex = -1;
	}
	else if (m_wayPoints.size() == 1)
	{
		m_activeIndex = 0;
	}
	else
	{
		m_dataBus = CHmsGlobal::GetInstance()->GetDataBus();
		auto gpsData = m_dataBus->GetData();
		m_activeIndex = CalNearestPointIndex(gpsData->longitude, gpsData->latitude);
		m_isChangeCourse = true;
	}
}