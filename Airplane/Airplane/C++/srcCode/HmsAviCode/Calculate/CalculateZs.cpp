#include "CalculateZs.h"

double CalculateAngle(double dLong1, double dLat1, double dLong2, double dLat2)
{
	double dLat = ToRadian(dLat2-dLat1);
	double dLong = ToRadian(dLong2-dLong1);
	double lat1 = ToRadian(dLat1);
	double lat2 = ToRadian(dLat2);

	return atan2(sin(dLong)*cos(lat2), cos(lat1)*sin(lat2) - sin(lat1)*cos(lat2) * cos(dLong));
}

double CalculateDistanceNM(double dLong1, double dLat1, double dLong2, double dLat2)
{
	double dLat = ToRadian(dLat2-dLat1);
	double dLong = ToRadian(dLong2-dLong1);
	double lat1 = ToRadian(dLat1);
	double lat2 = ToRadian(dLat2);

	double a = pow(sin(dLat/2), 2) +
		pow(sin(dLong/2), 2)  * cos(lat1) * cos(lat2);
	return EarthRadiusNM * 2 * atan2(sqrt(a), sqrt(1-a));
}

double CalculateDistanceKM(double dLong1, double dLat1, double dLong2, double dLat2)
{
	double dLat = ToRadian(dLat2-dLat1);
	double dLong = ToRadian(dLong2-dLong1);
	double lat1 = ToRadian(dLat1);
	double lat2 = ToRadian(dLat2);

	double a = pow(sin(dLat/2), 2) +
		pow(sin(dLong/2), 2)  * cos(lat1) * cos(lat2);
	return EarthRadiusKM * 2 * atan2(sqrt(a), sqrt(1-a));
}

double CalculateDistanceM(double dLong1, double dLat1, double dLong2, double dLat2)
{
	return CalculateDistanceKM(dLong1, dLat1, dLong2, dLat2) * 1000;
}

void CalculatePositionByAngle(double & DestLat, double & DestLon, double orgLat, double orgLon,
	double distNM, double angle)
{
	double rad_lati  = ToRadian(orgLat);
	double rad_longi = ToRadian(orgLon);
	double Ec = PolarRadiusMeter + (EquatorRadiusMeter - PolarRadiusMeter)*(90.0-orgLat)/90.0;
	double Ed = EquatorRadiusMeter * cos(rad_lati);

	double difx = distNM * 1852.0 * cos(ToRadian(90-angle));
	double dify = distNM * 1852.0 * sin(ToRadian(90-angle));

	DestLat = ToAngle(1.0*dify/Ec + rad_lati);
	DestLon = ToAngle(1.0*difx/Ed + rad_longi);

	if (DestLon > 180)
	{
		DestLon = DestLon - 360;
	}
	if (DestLat > 180)
	{
		DestLat = DestLat - 360;
	}
}
