#ifndef _HEADER_ZS_CALCULATE_H____
#define _HEADER_ZS_CALCULATE_H____

#include <math.h>
#include <stdlib.h>

#define EarthRadiusNM		3440.0647948164
#define EarthRadiusKM		6371
#define EarthRadiusM		6378137.0f
#define SIM_Pi				3.14159265358979323846
#define SIM_Pi_x2			(SIM_Pi*2)
#define MeterFeet			3.2808399
#define KnotKMH				1.852f
#define MeterSFeetM			196.85032f
#define KilometerNM			0.5399568
#define KnotMeterS			0.51444444
#define FuelDensity			775		//º½¿ÕÈ¼ÓÍÃÜ¶È Ç§¿Ë/Á¢·½Ã×
#define KGHUKGALH			1 / FuelDensity * 219.9691573   //Ç§¿ËÃ¿Ð¡Ê±×ªÓ¢ÖÆ¼ÓÂØÃ¿Ð¡Ê±
#define KGHUSGALH			1 / FuelDensity * 264.1720524	//Ç§¿ËÃ¿Ð¡Ê±×ªÃÀÖÆ¼ÓÂØÃ¿Ð¡Ê±

#define EquatorRadiusMeter	6378137
#define PolarRadiusMeter	6356725

inline double ToRadian(double dAngle)
{
	return dAngle * SIM_Pi / 180.0;
}

inline double ToAngle(double dRadian)
{
	return fmod(dRadian * 180 / SIM_Pi + 360, 360.0);
}

inline double ToAngleOrg(double dRadian)
{
	return dRadian * 180 / SIM_Pi;
}

inline bool Equal(float f1, float f2) 
{
	return (abs(f1 - f2) < 1e-7f);
}

inline bool Equal(double d1, double d2)
{
	return (abs(d1 - d2) < 1e-7);
}

//Ç§Ã××ªº£Àï
inline double KilometerToNM(double dKM)
{
	return dKM * KilometerNM;
}

//º£Àï×ªÇ§Ã×
inline double NmToKilometer(double dNM)
{
	return dNM / KilometerNM;
}

//Ã××ªÓ¢³ß
inline double MeterToFeet(double dMeter)
{
	return dMeter * MeterFeet;
}
//Ó¢³ß×ªÃ×
inline double FeetToMeter(double dFeet)
{
	return dFeet / MeterFeet;
}
//½Ú×ªÇ§Ã×Ã¿Ð¡Ê±
inline double KnotToKMH(double dKnot)
{
	return dKnot * KnotKMH;
}
//Ç§Ã×Ã¿Ð¡Ê±×ª½Ú
inline double KMHToKnot(double dKMH)
{
	return dKMH / KnotKMH;
}
//Ã×Ã¿Ãë×ª½Ú
inline double MeterSToKnot(double dMeterS)
{
	return dMeterS * KnotMeterS;
}
//½Ú×ªÃ×Ã¿Ãë
inline double KnotToMeterS(double dKnot)
{
	return dKnot / KnotMeterS;
}
//Ã×Ã¿Ãë×ªÓ¢³ßÃ¿·ÖÖÓ
inline double MeterSToFeetM(double dMeterS)
{
	return dMeterS * MeterSFeetM;
}
//Ó¢³ßÃ¿·ÖÖÓ×ªÃ×Ã¿Ãë
inline double FeetMToMeterS(double dFeetM)
{
	return dFeetM / MeterSFeetM;
}
//Ç§¿ËÃ¿Ð¡Ê±×ªÓ¢ÖÆ¼ÓÂØÃ¿Ð¡Ê±
inline double KGHToUkGalH(double dKgH)
{
	return dKgH * KGHUKGALH;
}

//Êý¾ÝÓ³Éä ½«Êý×Ö´ÓÒ»¸ö·¶Î§ÄÚÓ³ÉäÖÁÁíÒ»·¶Î§ÄÚ
inline double NumberMap(double dTarget, double dRawMin, double dRawMax, double dNewMin, double dNewMax)
{
	if (dTarget - dRawMax > 1e-7)
	{
		return dNewMax;
	}
	else if (dTarget - dRawMin < 1e-7)
	{
		return dNewMin;
	}

	return (dNewMax - dNewMin) * dTarget / (dRawMax - dRawMin);
}
/************************************************************************
º¯ÊýÃû³Æ£º	double CalculateAngle(double dLong1, double dLat1, double dLong2, double dLat2);
º¯Êý¹¦ÄÜ£º	¼ÆËãÁ½¸ö¾­Î³¶ÈÖ®¼äµÄ·½Ïò¼Ð½Ç»¡¶È
º¯Êý²ÎÊý£º	dLong1
			dLat1
			dLong2
			dLat2
·µ»ØÖµ£º	·½Ïò(»¡¶È)
×÷Õß£º		ZS	
ÐÞ¸ÄÈÕÆÚ£º	<2013:1:10 12>   
************************************************************************/
double  CalculateAngle(double dLong1, double dLat1, double dLong2, double dLat2);

/************************************************************************
º¯ÊýÃû³Æ£º	double CalculateAngle(double dLong1, double dLat1, double dLong2, double dLat2);
º¯Êý¹¦ÄÜ£º	¼ÆËãÁ½¸ö¾­Î³¶ÈÖ®¼äµÄ¾àÀë£¨º£Àï£©
º¯Êý²ÎÊý£º	dLong1
			dLat1
			dLong2
			dLat2
·µ»ØÖµ£º	¾àÀë
×÷Õß£º		ZS	
ÐÞ¸ÄÈÕÆÚ£º	<2013:1:10 12>   
************************************************************************/
double  CalculateDistanceNM(double dLong1, double dLat1, double dLong2, double dLat2);

/************************************************************************
º¯ÊýÃû³Æ£º	double CalculateAngle(double dLong1, double dLat1, double dLong2, double dLat2);
º¯Êý¹¦ÄÜ£º	¼ÆËãÁ½¸ö¾­Î³¶ÈÖ®¼äµÄ·½ÏòµÄ¾àÀë£¨km£©
º¯Êý²ÎÊý£º	dLong1
			dLat1
			dLong2
			dLat2
·µ»ØÖµ£º	¾àÀë
×÷Õß£º		ZS	
ÐÞ¸ÄÈÕÆÚ£º	<2013:1:10 12>   
************************************************************************/
double  CalculateDistanceKM(double dLong1, double dLat1, double dLong2, double dLat2);

double  CalculateDistanceM(double dLong1, double dLat1, double dLong2, double dLat2);

/************************************************************************
º¯ÊýÃû³Æ£º	void CalculatePositionByAngle(double & DestLat, double & DestLon, double orgLat, double orgLon,
			double distance, double angle)
º¯Êý¹¦ÄÜ£º	Í¨¹ýÒÑ¾­ÖªµÀµÄ¾­Î³¶ÈµÄ ¾àÀëºÍ¼Ð½Ç£¬Çó¾­Î³
º¯Êý²ÎÊý£º	DestLat			Ä¿±êÎ³
			DestLon			Ä¿±ê¾­
			orgLat			Ô­Î³
			orgLon			Ô­¾­
			distance		¾àÀë£¬º£Àï
			angle			¼Ð½Ç£¬½Ç¶È
·µ»ØÖµ£º	
×÷Õß£º		ZS	
ÐÞ¸ÄÈÕÆÚ£º	<2013:6:19 13>   
************************************************************************/
void  CalculatePositionByAngle(double & DestLat, double & DestLon, double orgLat, double orgLon,
	double distNM, double angle);


#endif