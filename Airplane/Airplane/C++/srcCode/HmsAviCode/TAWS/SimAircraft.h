#pragma once
#include "HmsAviMacros.h"
#include <iostream>
#include <thread>
#include "base/Ref.h"
#include "math/Vec3.h"

#include "TawsInitParam.h"
#include "PlaneState.h"

class AircraftState{
public:
	float  fCurAlt;
	float  groundSpeedKnot;
	double curLon;
	double curLat;
	float curHeading;
	double curVSpeed;
	double trackdir;
	HmsAviPf::Vec3   EastNorthSkySpeed;//ENS

	//unsigned short AltitudeRdio;
	double AltitudeRadio;	
	float MagnetismCourse;
	double Time;	
	float EastSpeed;
	float NorthSpeed;

	void ResetValue();
};


class SimAircraft : public HmsAviPf::Ref
{
public:
	CREATE_FUNC(SimAircraft);
	
	static SimAircraft* Create(const HmsAviPf::InitParam& initState);	
	bool Start();
	bool Stop();
	SimAircraft();
	virtual ~SimAircraft();
	const AircraftState& GetAircraftState();
	bool InitWithState(const HmsAviPf::InitParam& initState);
protected:
	void Run();
	void Update(double delta);
	bool Init();
	
	bool m_running;
private:
	AircraftState m_aircraftState;
	std::thread * m_pthread;
};
