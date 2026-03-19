///////////////////////////////////////////////////////////
//  EarthCoord2D.h
//  Implementation of the Class EarthCoord2D
//  Created on:      23-2月-2016 14:10:54
//  Original author: zs
///////////////////////////////////////////////////////////

#pragma once

#include <HmsAviPf.h>

struct CSimEarthCoord
{
	double dLongitude;		//经度
	double dLatitude;		//纬度

public:
	CSimEarthCoord(double dLong, double dLat);
	CSimEarthCoord(const HmsAviPf::Vec2 & point);
	CSimEarthCoord();

	inline operator HmsAviPf::Vec2(){ return HmsAviPf::Vec2(dLongitude, dLatitude); }

	void Clean();

	CSimEarthCoord GetCrossPointByVector(double dCurDirection, const CSimEarthCoord& vectorEc, double dVectorDirection, bool* pbResult = 0) const;
	CSimEarthCoord GetCrossPointByCircle(double dCurDirection, const CSimEarthCoord& originEc, double dRadiusNm, bool* pbResult = 0) const;
	CSimEarthCoord GetPoistion(const CSimEarthCoord& directiveEc, double dDistanceRate) const;
	CSimEarthCoord GetPoistion(double dAngle, double dDistanceNm) const;
	double GetDistanceNm(const CSimEarthCoord& destEc) const;
	double GetDistanceKm(const CSimEarthCoord& destEc) const;
	double GetDistanceM(const CSimEarthCoord& destEc) const;
	double GetRandian(const CSimEarthCoord& destEc) const;
	double GetAngle(const CSimEarthCoord& destEc) const;
	void SetData(double dLong, double dLat);

	double GetRapidDistanceKm(const CSimEarthCoord & pos) const;
	void GetRapidDistanceKmAndRadian(const CSimEarthCoord & ec, double & dDistance, double & dHeading) const;
	double GetDeltaDistanceKm(const CSimEarthCoord & delta) const;
	double GetDeltaRelateDirection(const CSimEarthCoord & delta) const;
	double GetDeltaKmRelateDirection(const CSimEarthCoord & delta) const;
	CSimEarthCoord GetRapidPos(double dDistance, double dHeadingRadain) const;

};

CSimEarthCoord operator-(const CSimEarthCoord& pos1, const CSimEarthCoord& pos2);