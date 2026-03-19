#pragma once

#include "base/HmsImage.h"

struct TerrainTiffIndex
{
	union
	{
		struct
		{
			short nLong;
			short nLat;
		};
		int nIndex;
	};

	TerrainTiffIndex();

	bool IsValid() const;

	TerrainTiffIndex GetLeftIndex() const;
	TerrainTiffIndex GetRightIndex() const;
	TerrainTiffIndex GetUpIndex() const;
	TerrainTiffIndex GetDownIndex() const;

	bool operator==(const TerrainTiffIndex & _Right) const;
	bool operator>(const TerrainTiffIndex & _Right) const;
	bool operator<(const TerrainTiffIndex & _Right) const;

	HmsAviPf::Vec2 Center() const;
	double Left() const;
	double Right() const;
	double Top() const;
	double Bottom() const;

	static TerrainTiffIndex GetIndex(const HmsAviPf::Vec2 & wgsPos);
	static TerrainTiffIndex GetIndex(float lon, float lat);

	static HmsAviPf::Vec2 wgsLonLatNormalize(const HmsAviPf::Vec2 &wgsPos);
};

enum class TerrainTiffStatus
{
	Init,			//³õÊ¼×´Ì¬
	Loaded,			//¼ÓÔØ³É¹¦
	Failed,			//¼ÓÔØÊ§°Ü
};

struct HmsTerrainRawData
{
	TerrainTiffIndex			gtIndex;
	TerrainTiffStatus			eLoadStatus;			//¼ÓÔØ×´Ì¬
	short					*pData;
	HmsAviPf::Vec2			upperLeft;
	HmsAviPf::Vec2			lowerLeft;
	HmsAviPf::Vec2			upperRight;
	HmsAviPf::Vec2			lowerRight;
	HmsAviPf::Vec2			center;
	HmsAviPf::Size			sizeGeo;
	unsigned int			nDataWidth;
	unsigned int			nDataHeight;

	HmsTerrainRawData()
	{
		pData = nullptr;
		eLoadStatus = TerrainTiffStatus::Init;
		nDataWidth = 0;
		nDataHeight = 0;
	}

	~HmsTerrainRawData()
	{
		Release();
	}

	void Release()
	{
		if (pData)
		{
			delete[] pData;
			pData = nullptr;
		}
	}

	short GetAltitude(const HmsAviPf::Vec2 & pos);
};

union HmsColorCell
{
	unsigned int nColor;
	unsigned int rgb : 24;
	struct
	{
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;
	};
	struct
	{
		unsigned short rg;
		unsigned short ba;
	};

	HmsColorCell();
};