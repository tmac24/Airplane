#pragma once
#include "HmsAviMacros.h"

NS_HMS_BEGIN

enum enDataInputType{
	DataInputType_Sim = 0,
	DataInputType_UDP,//1
	DataInputType_Serial,//2
	InvalidValue
};


class InitParam{
public:
	enDataInputType taws_data_input_mode;
	bool show_compass;
	bool show_DebugMsg;
	bool show_ComFps;
	bool sim_trackDir_EastNorstSkySpeed;

	double lon;
	double lat;
	double alt;
	double vs;
	double gs;
	double hdg;
	double trk;

	double lowerSafeDistance;
	double upperSafeDistance;
	double leftRightSafeDistance;

	InitParam(){
		ResetValue();
	}
	void ResetValue(){
        taws_data_input_mode = enDataInputType::DataInputType_UDP;
		show_compass = false;
		show_DebugMsg = false;
		sim_trackDir_EastNorstSkySpeed = true;
		lon = 0.0;
		lat = 0.0;
		alt = 0.0;
		vs = 0.0;
		gs = 0.0;
		hdg = 0.0;
		trk = 0.0;

		lowerSafeDistance = 0.0;
		upperSafeDistance = 0.0;
		leftRightSafeDistance = 0.0;
	}
};

NS_HMS_END

