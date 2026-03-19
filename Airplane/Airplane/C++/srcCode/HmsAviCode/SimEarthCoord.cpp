///////////////////////////////////////////////////////////
//  EarthCoord2D.cpp
//  Implementation of the Class EarthCoord2D
//  Created on:      23-2月-2016 14:10:54
//  Original author: zs
///////////////////////////////////////////////////////////

#include "SimEarthCoord.h"
#include "Calculate/CalculateZs.h"
#include <math.h>
#include "HmsGlobal.h"


CSimEarthCoord::CSimEarthCoord(double dLong, double dLat)
{
	dLatitude = dLat;
	dLongitude = dLong;
}


CSimEarthCoord::CSimEarthCoord()
	: dLongitude(0)
	, dLatitude(0)
{

}

CSimEarthCoord::CSimEarthCoord(const HmsAviPf::Vec2 & point)
	: dLatitude(point.y)
	, dLongitude(point.x)
{

}


void CSimEarthCoord::Clean()
{
	dLatitude = 0.0;
	dLongitude = 0.0;
}


CSimEarthCoord CSimEarthCoord::GetCrossPointByVector(double dCurDirection, const CSimEarthCoord& vectorEc, double dVectorDirection, bool* pbResult) const 
{
	CSimEarthCoord cross;
	bool bExistCross = true;

	if (FuzzyCompare(fmod(dCurDirection + 270, 90), 0))	//tan(dStarHeading) is not exist.
	{
		if (FuzzyCompare(fmod(dVectorDirection + 270, 90), 0))	//tan(dDestHeading) is not exist	
		{
			//两直线平行,为得到数据，取终点
			if (!FuzzyCompare(vectorEc.dLatitude - dLatitude, 0))
			{
				//two point is not at the same line
				bExistCross = false;
			}
			cross.dLatitude = vectorEc.dLatitude;
			cross.dLongitude = vectorEc.dLongitude;
		}
		else
		{
			cross.dLatitude = dLatitude;
			cross.dLongitude = vectorEc.dLongitude + (dLongitude - vectorEc.dLongitude) * tan(ToRadian(dVectorDirection));
		}
	}
	else
	{
		if (FuzzyCompare(fmod(dVectorDirection + 270, 90), 0))	//tan(dDestHeading) is not Exist;
		{
			cross.dLatitude = vectorEc.dLatitude;
			cross.dLongitude = dLongitude + (vectorEc.dLongitude - dLongitude) * tan(ToRadian(dCurDirection));
		}
		else
		{
			double temp = ToRadian(dVectorDirection);
			double dCosDestHeading = cos(temp);
			double dSinDestHeading = sin(temp);
			double dTanStarHeading = tan(ToRadian(dCurDirection));

			temp = dTanStarHeading * dCosDestHeading - dSinDestHeading;
			if (FuzzyCompare(temp, 0))
			{
				//oh i can't calculate the position there,the using the dest pos
				bExistCross = false;
			}
			else
			{
				double t = (vectorEc.dLongitude - dLongitude + dTanStarHeading * (dLatitude - vectorEc.dLatitude)) / temp;
				cross.dLatitude = vectorEc.dLatitude + t * dCosDestHeading;
				cross.dLongitude = vectorEc.dLongitude + t * dSinDestHeading;
			}
		}
	}

	if (nullptr != pbResult)
	{
		*pbResult = bExistCross;
	}
	return cross;
}


CSimEarthCoord CSimEarthCoord::GetCrossPointByCircle(double dCurDirection, const CSimEarthCoord& originEc, double dRadiusNm, bool* pbResult) const 
{
	double earthNm2Rate = (1.0 / 59.935205183585) * (1.0 / 59.935205183585) * sin(ToRadian(originEc.dLatitude));

	CSimEarthCoord dest;
	bool bResultSuitable = true;
	double dRadian = ToRadian(dCurDirection);
	double dCosRadian = cos(dRadian);
	double dSinRadian = sin(dRadian);

	double dA = dLongitude - originEc.dLongitude;
	double dB = dSinRadian;
	double dC = dLatitude - originEc.dLatitude;
	double dD = dCosRadian;


	double AX2 = 1;	//dSinRadian * dSinRadian + dCosRadian * dCosRadian;
	double BX = 2 * dA * dB + 2 * dC * dD;
	double C = dA * dA + dC * dC - dRadiusNm * dRadiusNm * earthNm2Rate;

	double delta = BX*BX - 4 * AX2 * C;
	double t;
	if (FuzzyCompare(delta, 0))
	{
		t = -BX / (2 * AX2);
		dest.dLatitude = dLatitude + t * dCosRadian;
		dest.dLongitude = dLongitude + t * dSinRadian;
	}
	else if (delta > 0.0)
	{
		t = (-BX + sqrt(delta)) / (2 * AX2);
		dest.dLatitude = dLatitude + t * dCosRadian;
		dest.dLongitude = dLongitude + t * dSinRadian;
	}
	else
	{
		bResultSuitable = false;
		//没有交点时，设定为指定方向上的交点
		dest = GetPoistion(dCurDirection, dRadiusNm);
	}

	if (nullptr != pbResult)
	{
		*pbResult = bResultSuitable;
	}
	return dest;
}


CSimEarthCoord CSimEarthCoord::GetPoistion(const CSimEarthCoord& directiveEc, double dDistanceRate) const 
{
	CSimEarthCoord destPos;
	destPos.dLongitude = dLongitude + (directiveEc.dLongitude - dLongitude) * dDistanceRate;
	destPos.dLatitude = dLatitude + (directiveEc.dLatitude - dLatitude) * dDistanceRate;
	return destPos;
}


CSimEarthCoord CSimEarthCoord::GetPoistion(double dAngle, double dDistanceNm) const 
{
	CSimEarthCoord destPos;
	CalculatePositionByAngle(destPos.dLatitude, destPos.dLongitude,
		dLatitude, dLongitude, dDistanceNm, dAngle);
	return destPos;
}


double CSimEarthCoord::GetDistanceNm(const CSimEarthCoord& destEc) const 
{
	return CalculateDistanceNM(dLongitude, dLatitude, destEc.dLongitude, destEc.dLatitude);
}


double CSimEarthCoord::GetDistanceKm(const CSimEarthCoord& destEc) const 
{
	return CalculateDistanceKM(dLongitude, dLatitude, destEc.dLongitude, destEc.dLatitude);
}


double CSimEarthCoord::GetRandian(const CSimEarthCoord& destEc) const 
{
	return CalculateAngle(dLongitude, dLatitude, destEc.dLongitude, destEc.dLatitude);
}


double CSimEarthCoord::GetAngle(const CSimEarthCoord& destEc) const 
{
	return ToAngle(CalculateAngle(dLongitude, dLatitude, destEc.dLongitude, destEc.dLatitude));
}


void CSimEarthCoord::SetData(double dLong, double dLat)
{
	dLatitude = dLat;
	dLongitude = dLong;
}

double CSimEarthCoord::GetDistanceM(const CSimEarthCoord& destEc) const
{
	return GetDistanceKm(destEc) * 1000;
}

double CSimEarthCoord::GetRapidDistanceKm(const CSimEarthCoord & pos) const
{
	auto temp = pos - *this;
	temp.dLongitude *= CHmsGlobal::GetKmPerLongitude(pos.dLatitude);
	temp.dLatitude *= CHmsGlobal::GetKmPerLatitude();
	auto d = sqrt(temp.dLongitude*temp.dLongitude + temp.dLatitude*temp.dLatitude);
	return d;
}

void CSimEarthCoord::GetRapidDistanceKmAndRadian(const CSimEarthCoord & ec, double & dDistance, double & dHeading) const
{

	auto temp = ec - *this;
	temp.dLongitude *= CHmsGlobal::GetKmPerLongitude(ec.dLatitude);
	temp.dLatitude *= CHmsGlobal::GetKmPerLatitude();
	dDistance = sqrt(temp.dLongitude*temp.dLongitude + temp.dLatitude*temp.dLatitude);
	dHeading = GetDeltaKmRelateDirection(temp);
}

double CSimEarthCoord::GetDeltaDistanceKm(const CSimEarthCoord & delta) const
{
	auto temp = delta;
	temp.dLongitude *= CHmsGlobal::GetKmPerLongitude(this->dLatitude);
	temp.dLatitude *= CHmsGlobal::GetKmPerLatitude();
	return sqrt(temp.dLongitude*temp.dLongitude + temp.dLatitude*temp.dLatitude);
}

double CSimEarthCoord::GetDeltaRelateDirection(const CSimEarthCoord & delta) const
{
	return atan2(delta.dLongitude, delta.dLatitude);
}

double CSimEarthCoord::GetDeltaKmRelateDirection(const CSimEarthCoord & delta) const
{
	return atan2(delta.dLongitude*CHmsGlobal::GetKmPerLongitude(this->dLatitude), delta.dLatitude*CHmsGlobal::GetKmPerLatitude());
}

CSimEarthCoord CSimEarthCoord::GetRapidPos(double dDistance, double dHeadingRadain) const
{
	CSimEarthCoord posRet = *this;
	posRet.dLongitude += dDistance * sin(dHeadingRadain) / CHmsGlobal::GetKmPerLongitude(this->dLatitude);
	posRet.dLatitude += dDistance * cos(dHeadingRadain) / CHmsGlobal::GetKmPerLatitude();
	return posRet;
}

CSimEarthCoord operator-(const CSimEarthCoord& pos1, const CSimEarthCoord& pos2)
{
	return CSimEarthCoord(
		pos1.dLongitude - pos2.dLongitude,
		pos1.dLatitude - pos2.dLatitude);
}