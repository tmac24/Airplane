#pragma once

#include "base/HmsNode.h"
#include "display/HmsImageNode.h"
#include "base/HmsCamera.h"
#include "ui/HmsUiEventInterface.h"
#include "../HmsGlobal.h"

using namespace HmsAviPf;

class CHmsNavMathHelper
{
public:
	CHmsNavMathHelper();
	~CHmsNavMathHelper();

	static Vec2 EarthPointToLonLat(const Vec3 &pos);
	static Vec3 LonLatToEarthPoint(const Vec2 &lonLat);

	static Vec2 NextPointLonLatByBearing(Vec2 curPos, float dis, float bearing);
	static void InsertLonLat(const Vec2 &lonLat1, const Vec2 &lonLat2, std::vector<HmsAviPf::Vec2> &vecLonLatSerials, float dis);
	static void InsertEarthPos(const Vec2 &lonLat1, const Vec2 &lonLat2, std::vector<HmsAviPf::Vec3> &vecEarthSerials, float dis);

	//不等分
	static void InsertLonLatNotEqual(const Vec2 &lonLat1, const Vec2 &lonLat2, std::vector<HmsAviPf::Vec2> &vecLonLatSerials);
	static void InsertEarthPosNotEqual(const Vec2 &lonLat1, const Vec2 &lonLat2, std::vector<HmsAviPf::Vec3> &vecEarthSerials);

	static void StrSplit(std::string& s, const std::string& delim, std::vector<std::string> &vecRet);
	static bool StrLonLatToFloat(float &lon, float &lat, std::string str);
	static std::string LonLatToString(float lon, float lat);
};