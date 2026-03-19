#include "HmsNavMathHelper.h"
#include "HmsLog.h"

CHmsNavMathHelper::CHmsNavMathHelper()
{
}

CHmsNavMathHelper::~CHmsNavMathHelper()
{
}

HmsAviPf::Vec2 CHmsNavMathHelper::EarthPointToLonLat(const Vec3 &pos)
{
    auto lat = HmsMath_asin(0 - pos.y);
	auto lon = atan2f(pos.x, pos.z);
	Vec2 v;
	v.x = MATH_RAD_TO_DEG(lon);
	v.y = MATH_RAD_TO_DEG(lat);

	return v;
}

Vec3 CHmsNavMathHelper::LonLatToEarthPoint(const Vec2 &lonLat)
{
	Vec3 ret;
	auto lon = MATH_DEG_TO_RAD(lonLat.x);
	auto lat = MATH_DEG_TO_RAD(lonLat.y);

	ret.x = sinf(lon) * cos(lat);
	ret.y = -sin(lat);
	ret.z = cos(lon) * cos(lat);

	return ret;
}

Vec2 CHmsNavMathHelper::NextPointLonLatByBearing(Vec2 curPos, float dis, float bearing)
{
	const float earthR = 6371.393f; //km
	auto cRadian = dis / earthR;
	auto tempRadian = MATH_DEG_TO_RAD(90 - curPos.y);
	auto bearingRadian = MATH_DEG_TO_RAD(bearing);
    auto aRadian = HmsMath_acos(cos(tempRadian) * cos(cRadian) + sin(tempRadian) * sin(cRadian) * cos(bearingRadian));

    auto c = HmsMath_asin(sin(cRadian) * sin(bearingRadian) / sin(aRadian));

	Vec2 ret;
	ret.x = curPos.x + MATH_RAD_TO_DEG(c);
	if (ret.x > 180) ret.x -= 360;
	ret.y = 90 - MATH_RAD_TO_DEG(aRadian);
	return ret;
}

void CHmsNavMathHelper::InsertLonLat(const Vec2 &lonLat1, const Vec2 &lonLat2, std::vector<Vec2> &vecLonLatSerials, float dis)
{
	std::vector<HmsAviPf::Vec3> vecEarth;
	InsertEarthPos(lonLat1, lonLat2, vecEarth, dis);
	for (auto &c : vecEarth)
	{
		vecLonLatSerials.push_back(EarthPointToLonLat(c));
	}
}

void CHmsNavMathHelper::InsertEarthPos(const Vec2 &lonLat1, const Vec2 &lonLat2, std::vector<HmsAviPf::Vec3> &vecEarthSerials, float dis)
{
	const float earthR = 6371.393f; //km
	auto bR = MATH_DEG_TO_RAD(90 - lonLat1.y);
	auto aR = MATH_DEG_TO_RAD(90 - lonLat2.y);
    auto cR = HmsMath_acos(cos(bR) * cos(aR) + sin(bR) * sin(aR) * cos(MATH_DEG_TO_RAD(lonLat2.x - lonLat1.x)));

	int count = earthR * cR / dis;

	Vec3 pos1 = LonLatToEarthPoint(lonLat1);
	Vec3 pos2 = LonLatToEarthPoint(lonLat2);

	Vec3 axis;
	Vec3::cross(pos1, pos2, &axis);
	axis.normalize();

	vecEarthSerials.push_back(pos1);
	for (int i = 1; i < count; ++i)
	{
		Quaternion q(axis, cR / count * i);
		vecEarthSerials.push_back(q * pos1);
	}
	vecEarthSerials.push_back(pos2);
}

void CHmsNavMathHelper::StrSplit(std::string& s, const std::string& delim, std::vector<std::string> &vecRet)
{
	size_t last = 0;
	size_t index = s.find_first_of(delim, last);
	while (index != std::string::npos)
	{
		vecRet.push_back(s.substr(last, index - last));
		last = index + 1;
		index = s.find_first_of(delim, last);
	}
	if (index - last > 0)
	{
		vecRet.push_back(s.substr(last, index - last));
	}
}

bool CHmsNavMathHelper::StrLonLatToFloat(float &lon, float &lat, std::string str)
{
    if(str.find('_') == std::string::npos)
    {
        bool bRet = false;
        std::vector<std::string> listStr;
        CHmsNavMathHelper::StrSplit(str, "/", listStr);
        if (listStr.size() == 2)
        {
            std::string first = listStr[1];
            std::string second = listStr[0];
            if (first.size() >= 2 && second.size() >= 2)
            {
                char fc = first.at(first.size() - 1);
                char sc = second.at(second.size() - 1);
                first = first.substr(0, first.size() - 1);
                second = second.substr(0, second.size() - 1);
                if (fc == 'N' && sc == 'E')
                {
                    lon = atof(second.c_str());
                    lat = atof(first.c_str());
                    bRet = true;
                }
                else if (fc == 'N' && sc == 'W')
                {
                    lon = 0 - atof(second.c_str());
                    lat = atof(first.c_str());
                    bRet = true;
                }
                else if (fc == 'S' && sc == 'E')
                {
                    lon = atof(second.c_str());
                    lat = 0 - atof(first.c_str());
                    bRet = true;
                }
                else if (fc == 'S' && sc == 'W')
                {
                    lon = 0 - atof(second.c_str());
                    lat = 0 - atof(first.c_str());
                    bRet = true;
                }
                if (bRet)
                {
                    if (std::isnan(lon) || std::isnan(lat))
                    {
                        bRet = false;
                    }
                    if (fabsf(lon) > 180 || fabsf(lat) > 90)
                    {
                        bRet = false;
                    }
                }
            }
        }
        return bRet;
    }
    else
    {
        auto GetDegreeMinSec = [](std::string &str, int &degree, int &min, int &sec, char &type)
        {
            return sscanf(str.c_str(), "%d_%d_%d%c", &degree, &min, &sec, &type);
        };
        auto GetLonLat = [GetDegreeMinSec](std::string &strLon, float &lon, std::string &strLat, float &lat)
        {
            bool bRet = true;
            int degree;
            int min;
            int sec;
            char type;
            if (4 == GetDegreeMinSec(strLon, degree, min, sec, type))
            {
                lon = degree + min / 60.0 + sec / 3600.0;
                if ('W' == type)
                {
                    lon = 0 - lon;
                }
            }
            else
            {
                bRet = false;
            }
            if (4 == GetDegreeMinSec(strLat, degree, min, sec, type))
            {
                lat = degree + min / 60.0 + sec / 3600.0;
                if ('S' == type)
                {
                    lat = 0 - lat;
                }
            }
            else
            {
                bRet = false;
            }
            if (fabsf(lon) > 180 || fabsf(lat) > 90)
            {
                bRet = false;
            }
            return bRet;
        };

        bool bRet = false;
        std::vector<std::string> listStr;
        CHmsNavMathHelper::StrSplit(str, "/", listStr);
        if (listStr.size() == 2)
        {
            bRet = GetLonLat(listStr[0], lon, listStr[1], lat);
        }
        return bRet;
    }
}

std::string CHmsNavMathHelper::LonLatToString(float lon, float lat)
{
#if 0
	char tmp[128] = { 0 };
	if (lon < 0 && lat < 0)
	{
		sprintf(tmp, "%.5fS/%.5fW", fabsf(lat), fabsf(lon));
	}
	else if (lon < 0 && lat >= 0)
	{
		sprintf(tmp, "%.5fN/%.5fW", fabsf(lat), fabsf(lon));
	}
	else if (lon >= 0 && lat < 0)
	{
		sprintf(tmp, "%.5fS/%.5fE", fabsf(lat), fabsf(lon));
	}
	else if (lon >= 0 && lat >= 0)
	{
		sprintf(tmp, "%.5fN/%.5fE", fabsf(lat), fabsf(lon));
	}
	std::string str = tmp;
	return str;
#else
	double dTemp = abs(lon);
	int nLonDeg = dTemp;
	dTemp -= nLonDeg;
	dTemp *= 60;
	int nLonMin = (int)dTemp;
	int nLonSec = (dTemp - nLonMin) * 60;
	char cLon = (lon > 0.0f) ? 'E': 'W';

	dTemp = abs(lat);
	int nLatDeg = dTemp;
	dTemp -= nLatDeg;
	dTemp *= 60;
	int nLatMin = (int)dTemp;
	int nLatSec = (dTemp - nLatMin) * 60;
	char cLat = (lat > 0.0f) ? 'N':'S';
	char strLonLat[200];
	int nStrRetLen = sprintf(strLonLat, "%03d\302\260%02d'%02d%c/%02d\302\260%02d'%02d%c", nLonDeg, nLonMin, nLonSec, cLon, nLatDeg, nLatMin, nLatSec, cLat);
    if (nStrRetLen >= HMS_ARRAYSIZE(strLonLat))
    {
        HMSLOGERROR("strLen=%d", nStrRetLen);
    }
	return strLonLat;
#endif
}

void CHmsNavMathHelper::InsertLonLatNotEqual(const Vec2 &lonLat1, const Vec2 &lonLat2, std::vector<HmsAviPf::Vec2> &vecLonLatSerials)
{
	std::vector<HmsAviPf::Vec3> vecEarth;
	InsertEarthPosNotEqual(lonLat1, lonLat2, vecEarth);
	for (auto &c : vecEarth)
	{
		vecLonLatSerials.push_back(EarthPointToLonLat(c));
	}
}

void CHmsNavMathHelper::InsertEarthPosNotEqual(const Vec2 &lonLat1, const Vec2 &lonLat2, std::vector<HmsAviPf::Vec3> &vecEarthSerials)
{
	const float earthR = 6371.393f; //km
	auto bR = MATH_DEG_TO_RAD(90 - lonLat1.y);
	auto aR = MATH_DEG_TO_RAD(90 - lonLat2.y);
    auto cR = HmsMath_acos(cos(bR) * cos(aR) + sin(bR) * sin(aR) * cos(MATH_DEG_TO_RAD(lonLat2.x - lonLat1.x)));

	//涓嶇瓑鍒嗗姬搴?
	std::vector<float> vecRadian;
	float radianLen = 0.2f;
	float first = 0;
	float last = 0;
	int index = 0;
	for (int i = 0; i < 150; ++i)
	{
		first += radianLen / earthR;
		last = cR - first;
		radianLen *= 1.3f;
		if (first >= last)
		{
			break;
		}
		vecRadian.insert(vecRadian.begin() + index, first);
		vecRadian.insert(vecRadian.begin() + index + 1, last);
		++index;
	}

	Vec3 pos1 = LonLatToEarthPoint(lonLat1);
	Vec3 pos2 = LonLatToEarthPoint(lonLat2);
	Vec3 axis;
	Vec3::cross(pos1, pos2, &axis);
	axis.normalize();
	vecEarthSerials.push_back(pos1);
    for (int i = 0; i < (int)vecRadian.size(); ++i)
	{
		Quaternion q(axis, vecRadian[i]);
		vecEarthSerials.push_back(q * pos1);
	}
	vecEarthSerials.push_back(pos2);
}
