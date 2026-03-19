#include "HmsPeriodicCalcTask.h"
#include "NavSvs/HmsNavSvsLoadDataThread.h"
#include "Calculate/CalculateZs.h"
#include "TAWS/CollisionManager.h"
#include "TAWS/RunwayTileDataMgr.h"
#include "Svs/HmsSvsFlightTube.h"


//************************************
// Method:    SimGroundSpeed
// FullName:  SimGroundSpeed
// Access:    public 
// Desc:      
// Returns:   float
// Qualifier:
// Parameter: const float & delta
// Parameter: const double & Lng
// Parameter: const double & Lat
// Author:    yan.jiang
//************************************
float SimGroundSpeed(const float& delta, const double& Lng, const double& Lat)
{
    static CHmsAlphaFilter filter1 = CHmsAlphaFilter(0.65);
    static float s_groundspd = 0.0F;

    static double s_lon = Lng;
    static double s_lat = Lat;

    auto distDeltaNm = CalculateDistanceNM(s_lon, s_lat, Lng, Lat);
    s_groundspd = (float)distDeltaNm *3600.0F / delta;// distDeltaNm / (s_delta_cum1 / 3600.0f);

    s_lon = Lng;
    s_lat = Lat;

    s_groundspd = filter1.GetFilterValue(s_groundspd);

    return s_groundspd;
}

//************************************
// Method:    ConvertGsVsDirToEastNorthSky
// FullName:  CHmsLayerTaws::ConvertGsVsDirToEastNorthSky
// Access:    public 
// Desc:      Metric
// Returns:   HmsAviPf::Vec3
// Qualifier:
// Parameter: const float & groundSpeedMps		Meter/sec
// Parameter: const float & verticalSpeedMps	Meter/sec
// Parameter: const float & fMovingDirection	Degree (0~360)
// Author:    yan.jiang
//************************************
HmsAviPf::Vec3d ConvertGsVsDirToEastNorthSky(const float& groundSpeedMps, const float& verticalSpeedMps, const float& fMovingDirection)
{
    Vec3d velocityOfEastNorthSky;
    velocityOfEastNorthSky.x = sin(ToRadian(fMovingDirection)) * groundSpeedMps;
    velocityOfEastNorthSky.y = cos(ToRadian(fMovingDirection)) * groundSpeedMps;
    velocityOfEastNorthSky.z = verticalSpeedMps;
    return velocityOfEastNorthSky;
}

//************************************
// Method:    ConvertGsVsDirToEastNorthSkyImperial
// FullName:  CHmsLayerTaws::ConvertGsVsDirToEastNorthSkyImperial
// Access:    public 
// Desc:      Imperial
// Returns:   HmsAviPf::Vec3
// Qualifier:
// Parameter: const float & groundSpeedKnot	Knot
// Parameter: const float & verticalSpeedFpm	Feet/Min
// Parameter: const float & fMovingDirection	Degree (0~360)
// Author:    yan.jiang
//************************************
HmsAviPf::Vec3d ConvertGsVsDirToEastNorthSkyImperial(const float& groundSpeedKnot, const float& verticalSpeedFpm, const float& fMovingDirection)
{
    auto groundSpeedMps = KnotToKMH(groundSpeedKnot) * 1000 / 3600;
    auto verticalSpeedMps = FeetToMeter(verticalSpeedFpm) / 60;
    return ConvertGsVsDirToEastNorthSky((float)groundSpeedMps, (float)verticalSpeedMps, fMovingDirection);
}

float CalcSimTrackDir(const float& delta, const double& Lng, const double& Lat)
{
    static double s_lon = Lng;
    static double s_lat = Lat;
    static float s_simTrackDir = 0;

    s_simTrackDir = (float)ToAngle(CalculateAngle(s_lon, s_lat, Lng, Lat));
    s_lon = Lng;
    s_lat = Lat;

    return s_simTrackDir;
}

Vec3 CalcEastNorthSkySpeed(const float& groundSpeedKnot, const float& verticalSpeedFpm, const float& track)
{
    static Vec3d s_ENS;
    s_ENS = ConvertGsVsDirToEastNorthSkyImperial(groundSpeedKnot, verticalSpeedFpm, track);
    Vec3 ens;
    ens.x = s_ENS.x;
    ens.y = s_ENS.y;
    ens.z = s_ENS.z;

    return ens;
}

CHmsPeriodicCalcTask::CHmsPeriodicCalcTask()
{
    //离地高的计算
    PeriodicJob::PeriodicJobFunc jobCalcSetHeith = [](float delta)-> void{
        CHmsDataBus *dataBus = CHmsGlobal::GetInstance()->GetDataBus();
		if (dataBus)
		{
			auto aftData = dataBus->GetData();
			if (aftData)
			{
        		Vec2 pos = Vec2(aftData->longitude, aftData->latitude);
        		auto alt_ground_surface = CHmsGlobal::GetInstance()->GetTerrainServer()->GetAltitude(&pos);
				auto height_from_gnd = dataBus->GetMslAltitudeMeter() - alt_ground_surface;

				dataBus->SetHeight(height_from_gnd);
			}
		}       
    };

    //地速、东北天速度，模拟值计算
    PeriodicJob::PeriodicJobFunc jobCalcGroundSpeedTrackSpeedOfEastNorthSkyup = [](float delta)->void{
        CHmsDataBus *dataBus = CHmsGlobal::GetInstance()->GetDataBus();
		if (dataBus)
		{
			auto aftData = dataBus->GetData();
			if (aftData)//modefy by MK
			{
				//get sim groundSpeed
				auto simGs = SimGroundSpeed(delta, aftData->longitude, aftData->latitude);

				//get sim Track
				auto simTrack = CalcSimTrackDir(delta, aftData->longitude, aftData->latitude);

				//get East/North/Sky speed
				auto ens = CalcEastNorthSkySpeed(aftData->groundSpeed, aftData->verticalSpeed, simTrack);//东北天目前使用模拟track计算

				aftData->simGroundSpeed = simGs;
				aftData->simTrack = simTrack;
				aftData->speedEast = ens.x;
				aftData->speedNorth = ens.y;
				aftData->speedSkyUp = ens.z;
			}
		}        
    };

    //TAWS告警计算 之 碰撞信息
    PeriodicJob::PeriodicJobFunc jobCalcCollitionInfo = [](float delta)->void{
        CHmsDataBus *dataBus = CHmsGlobal::GetInstance()->GetDataBus();
		if (dataBus)// modefy by MK
		{
			auto aftData = dataBus->GetData();
			if (aftData)
			{
				//calculate terrain collision
				HmsAviPf::Vec3d EastNorthSkySpeed(aftData->speedEast, aftData->speedNorth, aftData->speedSkyUp);

				//与地形碰撞、与地表障碍物碰撞 的告警计算
				CollisionManager::getInstance()->CalcCollisionInfo(delta,
					aftData->longitude,
					aftData->latitude,
					dataBus->GetMslAltitudeMeter(),
					aftData->trueHeading,
					aftData->trueTrack,
					EastNorthSkySpeed);
			}			
		}        
    };

    //TAWS告警计算 之 下降率过大
    PeriodicJob::PeriodicJobFunc jobCalcSinkRateAlarm = [](float delta)->void{
        CHmsDataBus *dataBus = CHmsGlobal::GetInstance()->GetDataBus();
		if (dataBus)
		{
			auto aftData = dataBus->GetData();
			if (aftData)
			{
				auto colliMgr = CollisionManager::getInstance();

				if (colliMgr)
				{
					colliMgr->CalcSinkRateAlarmState(delta,
						aftData->heightAboveTerrainMeter);
				}				
			}			
		}       
    };

    //计算最近的机场信息
    PeriodicJob::PeriodicJobFunc jobCalcNearestAirport = [](float delta)->void{
        CHmsDataBus *dataBus = CHmsGlobal::GetInstance()->GetDataBus();
		if (dataBus)
		{
			auto aftData = dataBus->GetData();
			if (aftData)
			{
				auto nearestRunwayInfo = RunwayTileDataMgr::getInstance()->CalcNearestRunwayInfo(aftData->longitude, aftData->latitude);
			}
		}       
    };

    //飞行管道 通过碰撞检测来决定管道是否需要显示
    PeriodicJob::PeriodicJobFunc jobCalcFlightTubeShowHide = [](float delta)->void{
        CHmsDataBus *dataBus = CHmsGlobal::GetInstance()->GetDataBus();
		if (dataBus)
		{
			auto aftData = dataBus->GetData();
			if (aftData)
			{
				auto gnssData_Alt = dataBus->GetMslAltitudeMeter();
				Vec3d aftLLA = Vec3d(aftData->longitude, aftData->latitude, gnssData_Alt);

				HmsSvsFlightTubeMgr::getInstance()->_mtx_vecTubeSegRef.lock();
				auto tubeSegRef = HmsSvsFlightTubeMgr::getInstance()->VecTubeSegRef();
				for (auto pseg : tubeSegRef)
				{
					if (pseg)
					{
						pseg->updateShowHide(delta, aftLLA);
					}
				}
				HmsSvsFlightTubeMgr::getInstance()->_mtx_vecTubeSegRef.unlock();
			}			
		}        
    };

    //飞行管道 检测飞机在管道的内外
    PeriodicJob::PeriodicJobFunc jobCalcFlightTubeInOut = [](float delta)->void{
        CHmsDataBus *dataBus = CHmsGlobal::GetInstance()->GetDataBus();
		if (dataBus)
		{
			auto aftData = dataBus->GetData();
			if (aftData)
			{
				auto gnssData_Alt = dataBus->GetMslAltitudeMeter();
				Vec3d aftLLA = Vec3d(aftData->longitude, aftData->latitude, gnssData_Alt);

				HmsSvsFlightTubeMgr::getInstance()->_mtx_vecTubeSegRef.lock();
				auto tubeSegRef = HmsSvsFlightTubeMgr::getInstance()->VecTubeSegRef();
				for (auto pseg : tubeSegRef)
				{
					if (pseg)
					{
						pseg->updateInOut(delta, aftLLA);
					}
				}
				HmsSvsFlightTubeMgr::getInstance()->_mtx_vecTubeSegRef.unlock();
			}			
		}       
    };
#define GPS_REPORT_PERIOD 1.0//SECOND
    m_vecPj.push_back(PeriodicJob(1.0f, jobCalcSetHeith));//per 1.0 seconds
    m_vecPj.push_back(PeriodicJob((float)(GPS_REPORT_PERIOD + 0.1f), jobCalcGroundSpeedTrackSpeedOfEastNorthSkyup));//大于gps的报告周期，避免出现计算出来的航迹无意义的情况
    m_vecPj.push_back(PeriodicJob((float)(GPS_REPORT_PERIOD + 0.1f), jobCalcCollitionInfo));//per 1.1 seconds
    m_vecPj.push_back(PeriodicJob(0.9F, jobCalcSinkRateAlarm));//per 0.9 seconds
    m_vecPj.push_back(PeriodicJob(4.0F, jobCalcNearestAirport));//per 4.0 seconds
    m_vecPj.push_back(PeriodicJob(10.0F, jobCalcFlightTubeShowHide));//per 10.0 seconds
    m_vecPj.push_back(PeriodicJob(2.5F, jobCalcFlightTubeInOut));//per 2.5 seconds
}

//************************************
// Method:    excute
// FullName:  CHmsAltTask::excute
// Access:    virtual public 
// Desc:      run some computing Periodic Job
// Returns:   void
// Qualifier:
// Author:    yan.jiang
//************************************
void CHmsPeriodicCalcTask::excute(float delta)
{
    for (auto& pj : m_vecPj)
    {
        pj.m_stopwatch += delta;
        if (pj.m_stopwatch > pj.m_period)
        {
            pj.m_job(pj.m_stopwatch);
            pj.m_stopwatch = 0.0F;
        }
    }
}

void CHmsPeriodicCalcTask::OnDuControlDataChanged(DUControlDataMember theChnaged)
{
    return;
}

PeriodicJob::PeriodicJob(float period, PeriodicJob::PeriodicJobFunc job)
: m_period(period)
, m_job(job)
, m_stopwatch(0.0F)
{

}
