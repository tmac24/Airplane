#include "PositionZs.h"
#include "CalculateZs.h"

EarthCoord2D::EarthCoord2D()
{
	dLatitude = 0.0;
	dLongitude = 0.0;
}

EarthCoord2D::EarthCoord2D(double dLat, double dLong)
{
	dLatitude = dLat;
	dLongitude = dLong;
}

void EarthCoord2D::SetData(double dLat, double dLong)
{
	dLatitude = dLat;
	dLongitude = dLong;
}

void EarthCoord2D::Clean()
{
	dLatitude = 0.0;
	dLongitude = 0.0;
}

double EarthCoord2D::GetAngle( const EarthCoord2D & destEc ) const
{
	return ToAngle(CalculateAngle(dLongitude, dLatitude, destEc.dLongitude, destEc.dLatitude));
}

double EarthCoord2D::GetRandian( const EarthCoord2D & destEc ) const
{
	return CalculateAngle(dLongitude, dLatitude, destEc.dLongitude, destEc.dLatitude);
}

double EarthCoord2D::GetDistanceKm( const EarthCoord2D & destEc ) const
{
	return CalculateDistanceKM(dLongitude, dLatitude, destEc.dLongitude, destEc.dLatitude);
}

double EarthCoord2D::GetDistanceNm( const EarthCoord2D & destEc ) const
{
	return CalculateDistanceNM(dLongitude, dLatitude, destEc.dLongitude, destEc.dLatitude);
}

EarthCoord2D EarthCoord2D::GetPoistion( double dAngle, double dDistanceNm ) const
{
	EarthCoord2D destPos;
	CalculatePositionByAngle(destPos.dLatitude, destPos.dLongitude, 
		dLatitude, dLongitude, dDistanceNm, dAngle);
	return destPos;
}

EarthCoord2D EarthCoord2D::GetPoistion( const EarthCoord2D & directiveEc, double dDistanceRate ) const
{
	EarthCoord2D destPos;
	destPos.dLongitude = dLongitude + (directiveEc.dLongitude - dLongitude) * dDistanceRate;
	destPos.dLatitude = dLatitude + (directiveEc.dLatitude - dLatitude) * dDistanceRate;
	return destPos;
}

EarthCoord2D EarthCoord2D::GetCrossPointByCircle( double dCurDirection, const EarthCoord2D & originEc, double dRadiusNm, bool * pbResult /*= NULL*/ ) const
{
	double earthNm2Rate = (1.0/59.935205183585) * (1.0/59.935205183585) * sin(ToRadian(originEc.dLatitude));

	EarthCoord2D dest; 
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
	if (Equal(delta, 0))
	{
		t = -BX/(2*AX2);
		dest.dLatitude = dLatitude + t * dCosRadian;
		dest.dLongitude = dLongitude + t * dSinRadian;
	}
	else if (delta > 0.0)
	{
		t = (-BX + sqrt(delta)) / (2*AX2);
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

EarthCoord2D EarthCoord2D::GetCrossPointByVector( double dCurDirection, const EarthCoord2D & vectorEc, double dVectorDirection, bool * pbResult /*= NULL*/ ) const
{
	EarthCoord2D cross;
	bool bExistCross = true;

	if (Equal(fmod(dCurDirection+270, 90), 0))	//tan(dStarHeading) is not exist.
	{
		if (Equal(fmod(dVectorDirection+270, 90), 0))	//tan(dDestHeading) is not exist	nu
		{
			//两直线平行,为得到数据，取终点
			if (!Equal(vectorEc.dLatitude - dLatitude, 0))
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
		if (Equal(fmod(dVectorDirection+270, 90), 0))	//tan(dDestHeading) is not Exist;
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
			if (Equal(temp, 0))
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

EarthCoord3D::EarthCoord3D()
{
	dLatitude = 0.0;
	dLongitude = 0.0;
	dAltitude = 0.0;
}

EarthCoord3D::EarthCoord3D(double dLat, double dLong, double dAlt)
{
	dLatitude = dLat;
	dLongitude = dLong;
	dAltitude = dAlt;
}

void EarthCoord3D::Clean()
{
	dLatitude = 0.0;
	dLongitude = 0.0;
	dAltitude = 0.0;
}