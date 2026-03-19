#pragma once

#include <string>
#include <vector>
#include "HmsAviPf.h"

struct HmsGeomColumns
{
	std::string		strIsoName;
	std::string		strTableName;
	std::string		strFieldName;
	int				nLayerIndex;
	int				nShapeType;
	double			dExtentLeft;
	double			dExtentBottom;
	double			dExtentRight;
	double			dExtentTop;
	double			dIdxOriginX;
	double			dIdxOriginY;
	double			dIdxGridSize;
};

#pragma pack(push)
#pragma pack(1)

struct HmsRawGeomHeader
{
	int nGeoType;		//OrSize

	double dMinX;
	double dMinY;

	double dMaxX;
	double dMaxY;

	int  nIndexSize;
	int  nPositionSize;
};

struct GeomPosition
{
	double dX;
	double dY;
};

#pragma pack(pop)

struct HmsGeomData
{
	int							id;
	HmsRawGeomHeader			header;
	std::string					strName;
	std::vector<int>			vIndex;
	std::vector<GeomPosition>	vPosition;
    GeomPosition                centralPos;
};


struct HmsGeomBoundary
{
	double dLeft;
	double dRight;
	double dTop;
	double dBottom;
};

struct HmsAdminAreaDescribe
{
	std::string					strName;
	std::string					strDbPath;
	HmsGeomBoundary				boundary;
	HmsAviPf::Rect				boundaryRect;
	int							nLayerCnt;		//the total layer count
	std::vector<HmsGeomColumns>	vLayerInfo;
};