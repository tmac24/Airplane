#include "Drawable.h"
#include "MathCommon.h"
#include "FPPManipulator.h"
#include "AirportCommon.h"
#include "Log.h"
#include "FileManager.h"
#include "Coordinate.h"
#include "CoordinateSystemMgr.h"
#include "Matrix.h"
#include "TerrainHead.h"


void LittleEndianToBigEndianRunway(CRunway *info)
{
    info->lat = LittleEndianToBigEndianDouble((info->lat));
    info->lon = LittleEndianToBigEndianDouble((info->lon));
    info->len = LittleEndianToBigEndianInt((info->len));
    info->width = LittleEndianToBigEndianInt((info->width));
    info->altitude = LittleEndianToBigEndianInt((info->altitude));
    info->direction = LittleEndianToBigEndianDouble((info->direction));
    info->gradient = LittleEndianToBigEndianDouble((info->gradient));
}

void PrintRunway(CRunway *info)
{
    printf("CRunway info ICAO_id :%s\n", info->ICAO_id);
    printf("CRunway info ICAO_code :%s\n", info->ICAO_code);
    printf("CRunway info runway_id :%s\n", info->runway_id);

    printf("CRunway info lon :%lf\n", info->lon);
    printf("CRunway info lat :%lf\n", info->lat);

    printf("CRunway info len :%f\n", info->len*0.3048f);
    printf("CRunway info Lele :%f\n", info->altitude*0.3048f);
    printf("CRunway info width :%f\n", info->width*0.3048f);

    printf("CRunway info direction :%lf\n", info->direction);
    printf("CRunway info gradient :%lf\n", info->gradient);
}

