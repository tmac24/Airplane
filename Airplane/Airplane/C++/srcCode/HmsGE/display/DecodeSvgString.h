#pragma once
#include <string>
#include "HmsPainterPath.h"

NS_HMS_BEGIN

class CDecodeSvgString
{
public:
	CDecodeSvgString();
	~CDecodeSvgString();

	bool GetSvgPath(const std::string & str, CHmsPainterPath &path);

	bool GetSvgPolygon(const std::string & str, std::vector<Vec2> & vOut);
 
protected:

};

NS_HMS_END