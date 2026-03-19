#include "HmsNavComm.h"
#include "FMS/HmsDuControlData.h"
#include "HmsGlobal.h"
#include "DataInterface/HmsDataBus.h"
#include "Calculate/CalculateZs.h"
#include "HmsConfigXMLReader.h"
#include <stdio.h>
#include <stdarg.h>

CHmsNavComm* CHmsNavComm::GetInstance()
{
    static CHmsNavComm* s_pIns = nullptr;
    if (!s_pIns)
    {
        s_pIns = new CHmsNavComm;
    }
    return s_pIns;
}


static std::string NavInfoFormatTime(double hour)
{
    int sec = hour * 3600;
    char tmpTime[64] = { 0 };
    sprintf(tmpTime, "%02d:%02d:%02d"
        , sec / 3600
        , sec % 3600 / 60
        , sec % 60);

    std::string ret = tmpTime;
    return ret;
}

int CHmsNavComm::UpdateTableETA(float groundSpeed, std::vector<NavLogTableStu> &vecNavLogTable)
{
    int curIndex = 0;
#if _NAV_SVS_LOGIC_JUDGE
	if (!CHmsGlobal::GetInstance()->GetDataBus()
		|| !CHmsGlobal::GetInstance()->GetDataBus()->GetDuControlData())
		return 0;
#endif
    auto vecWptNodeStu = CHmsGlobal::GetInstance()->GetDataBus()->GetDuControlData()->GetEnroute();

    vecNavLogTable.clear();
    if (vecWptNodeStu.empty())
    {
        return curIndex;
    }
    double totalNM = 0;
    for (int i = 0; i < (int)vecWptNodeStu.size() - 1; ++i)
    {
        const WptNodeBrief &curStu = vecWptNodeStu.at(i).brief;
        const WptNodeBrief &nextStu = vecWptNodeStu.at(i + 1).brief;

        double angle = CalculateAngle(curStu.lon, curStu.lat, nextStu.lon, nextStu.lat) / SIM_Pi * 180;
        if (angle < 0)
        {
            angle += 360;
        }

        double dis = CalculateDistanceNM(curStu.lon, curStu.lat, nextStu.lon, nextStu.lat);
        totalNM += dis;

        NavLogTableStu stu;
        stu.angle = angle;
        stu.totalNM = totalNM;
        stu.legNM = dis;
        if (groundSpeed > 1)
        {
            stu.totalTime = stu.totalNM / groundSpeed;
            stu.totalTimeStr = NavInfoFormatTime(stu.totalTime);
            stu.legTime = dis / groundSpeed;
            stu.legTimeStr = NavInfoFormatTime(stu.legTime);
        }
        vecNavLogTable.push_back(stu);
    }
    if (groundSpeed > 1 && vecNavLogTable.size() > 0)
    {
        FmsCalData &data = CHmsGlobal::GetInstance()->GetDataBus()->GetFmsData();
        curIndex = data.flySegment - 1;

        double totalNM = vecNavLogTable.back().totalNM;
        float remainNM = 0;
        for (int i = 0; i < (int)vecNavLogTable.size(); ++i)
        {
            NavLogTableStu &stu = vecNavLogTable[i];
            if (i >= curIndex)
            {
                float legNM = stu.legNM;
                if (i == curIndex)
                {
                    CHmsDataBus *dataBus = CHmsGlobal::GetInstance()->GetDataBus();
#if _NAV_SVS_LOGIC_JUDGE
					if (dataBus)
#endif
					{
						auto aftData = dataBus->GetData();
						Vec2 plane = Vec2(aftData->longitude, aftData->latitude);
						const WptNodeBrief &nextStu = vecWptNodeStu.at(i + 1).brief;
						legNM = CalculateDistanceNM(plane.x, plane.y, nextStu.lon, nextStu.lat);
					}
                }
                remainNM += legNM;

                stu.remainNM = remainNM;
                if (IsMetric())
                {
                    stu.remainNMStr = CHmsGlobal::FormatString("%.0fkm", KnotToKMH(stu.remainNM));
                }
                else
                {
                    stu.remainNMStr = CHmsGlobal::FormatString("%.0fnm", stu.remainNM);
                }
                
                stu.remainTime = stu.remainNM / groundSpeed;
                stu.remainTimeStr = NavInfoFormatTime(stu.remainTime);

                time_t	NowTime;	// 当前时间数
                struct tm *nowtm;	// 当前时间结构体
                NowTime = time(NULL);
                NowTime += stu.remainTime * 60 * 60;
                nowtm = localtime(&NowTime);
				if (nowtm)
				{
					char tmpTime[64] = { 0 };
					sprintf(tmpTime, "%04d-%02d-%02d %02d:%02d:%02d"
						, nowtm->tm_year + 1900
						, nowtm->tm_mon + 1
						, nowtm->tm_mday
						, nowtm->tm_hour
						, nowtm->tm_min
						, nowtm->tm_sec);
					stu.etaStr = tmpTime;
				}
				else
				{
					stu.etaStr = "---";
				}
            }
        }
    }
    return curIndex;
}

bool CHmsNavComm::IsMetric()
{
    return m_bMetricEnable;
}

double CHmsNavComm::GetNMOrKM(double value)
{
    if (IsMetric())
    {
        return value;
    }
    else
    {
        return KnotToKMH(value);
    }
}

std::string CHmsNavComm::AppendNMOrKMUnit(const char *format, ...)
{
    std::string strFormat = format;
    if (IsMetric())
    {
        strFormat += "km";
    }
    else
    {
        strFormat += "nm";
    }

    va_list _ArgList;
    va_start(_ArgList, format);
    char strTemp[256] = { 0 };
    vsprintf(strTemp, strFormat.c_str(), _ArgList);

    return strTemp;
}

CHmsNavComm::CHmsNavComm()
: m_pFrame2DRoot(nullptr)
, m_pNavFplLayer(nullptr)
, m_bMetricEnable(true)
, m_bEnableTileLoad(true)
{
    if (CHmsConfigXMLReader::GetSingleConfig()->GetConfigInfo("FunctionConfig/MetricEnable", 1))
    {
        m_bMetricEnable = true;
    }
    else
    {
        m_bMetricEnable = false;
    }
}

CHmsNavComm::~CHmsNavComm()
{

}
