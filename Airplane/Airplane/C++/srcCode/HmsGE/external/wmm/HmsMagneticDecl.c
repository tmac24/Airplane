#include <stdlib.h>

#include "coremag.h"
#include "constants.h"

double HmsGetMagneticDeclination(double lon, double lat, double heightKM, int year, int month, int day)
{
    double lon_deg = lon;
    double lat_deg = lat;
    double h = heightKM;
    int yy = year;
    int mm = month;
    int dd = day;

    double field[6];
    double var = calc_magvar(SGD_DEGREES_TO_RADIANS * lat_deg, SGD_DEGREES_TO_RADIANS * lon_deg, h,
        yymmdd_to_julian_days(yy, mm, dd), field);

    var = SGD_RADIANS_TO_DEGREES * var;

    return var;
}

double HmsMagneticHeadingToTrueHeading(double lon, double lat, double heightKM, double mag)
{
    double decl = HmsGetMagneticDeclination(lon, lat, heightKM, 18, 01, 01);
    return mag + decl;
}