#pragma once

#include <string>

struct HmsADSBShortTraffic
{
    std::string callsign;
    float longitude;
    float latitude;
    int	 altitude;
    float trackHeading;
    double updateTime;

    HmsADSBShortTraffic()
    {
        longitude = 0;
        latitude = 0;
        altitude = 0;
        trackHeading = 0;
        updateTime = 0;
    }
};