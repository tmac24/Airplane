#include "SimAircraft.h"
#include "Calculate/CalculateZs.h"
#include "math/Vec2.h"
#ifndef _WIN32
#include <sys/time.h>
#endif

#ifdef _WIN32
void InitZsTime();
extern double GetZsTime();
#endif

USING_NS_HMS;

bool SimAircraft::Init()
{
	m_aircraftState.ResetValue();
	return true;
}

//SimAircraft* SimAircraft::Create(const InitParam& initState)
SimAircraft* SimAircraft::Create(const InitParam& initState)
{
	SimAircraft *simaircraft = new STD_NOTHROW SimAircraft();
	if (simaircraft && simaircraft->InitWithState(initState))
	{
		simaircraft->autorelease();
		return simaircraft;
	}
	HMS_SAFE_DELETE(simaircraft);
	return nullptr;
}

double InitSysTime()
{
	double currtime = 0.0;
#ifdef _WIN32
	InitZsTime();
#endif
#ifdef _linux
	//todo
#endif 
#ifdef _vxworks
	//todo
#endif
	return currtime;
}

double GetCurrSysTime()
{
	double currtime = 0.0;
#ifdef _WIN32
	currtime = GetZsTime();
#endif
#ifdef _linux
	//to be test
	static struct timeval curtime;
	gettimeofday(&curtime, NULL);
	currtime = ((curtime.tv_sec * 1000) + (curtime.tv_usec / 1000))/1000.0;
#endif 
#ifdef __vxworks
	//to be test
	static struct timeval curtime;
	gettimeofday(&curtime, NULL);
	currtime = ((curtime.tv_sec * 1000) + (curtime.tv_usec / 1000)) / 1000.0;
#endif
	return currtime;
}

void SimAircraft::Update(double delta)
{
	//m_aircraftState.fCurAlt += 1;
	
	auto distanceNM = delta * m_aircraftState.groundSpeedKnot / 3600;
	double newLon;
	double newLat;
	CalculatePositionByAngle(newLat, newLon, m_aircraftState.curLat, m_aircraftState.curLon, distanceNM, m_aircraftState.trackdir);
	m_aircraftState.curLon = newLon;
	m_aircraftState.curLat = newLat;

	m_aircraftState.fCurAlt += FeetToMeter(m_aircraftState.curVSpeed / 60) * delta;

	//std::cout << "sim aircraft param pos " << Vec2(m_aircraftState.curLon, m_aircraftState.curLat) << std::endl;
}

void SimAircraft::Run()
{	
	InitSysTime();
	double currTime = GetCurrSysTime();
	double lastTime = currTime - 1.0;
	double deltaTime = 0.0;

	while (m_running)
	{
		currTime = GetCurrSysTime();
		deltaTime = currTime - lastTime;
		lastTime = currTime;

		Update(deltaTime);
		
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}


bool SimAircraft::Start()
{
	if (m_running)
	{
		std::cout << " simAircraft is already running." << std::endl;
		return false;
	}

	m_running = true;
	m_pthread = new std::thread(&SimAircraft::Run, this);

    return true;
}

bool SimAircraft::Stop()
{
	m_running = false;
	return true;
}

bool SimAircraft::InitWithState(const InitParam& initState)
{
	m_aircraftState.ResetValue();
	m_aircraftState.curLon = initState.lon;
	m_aircraftState.curLat = initState.lat;
	m_aircraftState.fCurAlt = initState.alt;
	m_aircraftState.curVSpeed = initState.vs;
	m_aircraftState.groundSpeedKnot = initState.gs;
	m_aircraftState.curHeading = initState.hdg;
	m_aircraftState.trackdir = initState.trk;

	return true;
}

SimAircraft::SimAircraft()
: m_running(false)
, m_pthread(nullptr)
{

}

SimAircraft::~SimAircraft()
{	
	Stop();
	if (m_pthread)
	{
		m_pthread->join();
	}	 
}


const AircraftState& SimAircraft::GetAircraftState()
{
	return m_aircraftState;
}

void AircraftState::ResetValue()
{
	fCurAlt = 0.0f;
	groundSpeedKnot = 0.0f;
	curLon = 0;
	curLat = 0;
	curHeading = 0.0f;
	curVSpeed = 0.0f;
	trackdir = 0.0f;

	AltitudeRadio = 0;	
	MagnetismCourse = 0.0f;	
	Time = 0.0;	
	EastSpeed = 0.0f;
	NorthSpeed = 0.0f;
	EastNorthSkySpeed = Vec3(0.0f, 0.0f, 0.0f);
}



