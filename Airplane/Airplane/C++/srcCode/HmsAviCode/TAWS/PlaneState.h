#pragma once
#include "NavSvs/mathd/Vec3d.h"

class PlaneState{
public:
    float  fCurAlt;
    float  groundSpeedKnot;
    double curLon;
    double curLat;
    float  curTrueHeading;
    float  curMagHeading;
    double curVSpeed;
    double trackdir;
    HmsAviPf::Vec3d   EastNorthSkySpeed;//ENS, x=E, y=N z=S

    //unsigned short AltitudeRdio;
    double AltitudeRadio;
    unsigned int DispCommond;
    float MagnetismCourse;
    unsigned short RedArlarmTimeSet;
    unsigned short YellowArlarmTimeSet;
    double Time;
    unsigned char VideoContor;
    unsigned int Spare1;
    unsigned char isCustomLowColliPointHeight;//1 default, 2 use custom height
    short customLowColliPointHeight;
    unsigned int Spare2;

    PlaneState()
    {
        ResetValue();
    };

    void ResetValue(){
        fCurAlt = 0.0f;
        groundSpeedKnot = 0.0f;
        curLon = 0;
        curLat = 0;
        curTrueHeading = 0.0f;
        curMagHeading = 0.0f;
        curVSpeed = 0.0f;
        trackdir = 0.0f;
        EastNorthSkySpeed = HmsAviPf::Vec3d(0.0f, 0.0f, 0.0f);

        AltitudeRadio = 0;
        DispCommond = 0;
        MagnetismCourse = 0.0f;
        RedArlarmTimeSet = 0x1;
        YellowArlarmTimeSet = 0;
        Time = 0.0;
        VideoContor = 0;
        //Spare1 = 0;
        isCustomLowColliPointHeight = 0;
        customLowColliPointHeight = 45;
        Spare2 = 0;
    };
};

