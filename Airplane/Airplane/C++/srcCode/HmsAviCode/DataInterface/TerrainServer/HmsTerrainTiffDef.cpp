#include <HmsAviPf.h>
#include "HmsTerrainTiffDef.h"

USING_NS_HMS;

TerrainTiffIndex::TerrainTiffIndex()
{
	nLong = 0;
	nLat = 0;
}


bool TerrainTiffIndex::IsValid() const
{
#if 0
	return nLat > 0 && nLat <= 24 && nLong > 0 && nLat <= 72;
#else//xiaoyh.
	return nLat > 0 && nLat <= 24 && nLong > 0 && nLong <= 72;
#endif
}

TerrainTiffIndex TerrainTiffIndex::GetLeftIndex() const
{
	TerrainTiffIndex index = *this;
	index.nLong -= 1;
	if (index.nLong <= 0)
	{
		index.nLong = 72;
	}
	return index;
}

TerrainTiffIndex TerrainTiffIndex::GetRightIndex() const
{
	TerrainTiffIndex index = *this;
	index.nLong += 1;
	if (index.nLong > 72)
	{
		index.nLong = 1;
	}
	return index;
}

TerrainTiffIndex TerrainTiffIndex::GetUpIndex() const
{
	TerrainTiffIndex index = *this;
	index.nLat -= 1;
	if (index.nLat < 1-6)
	{
		index.nLat = 24 + 6;
	}
	return index;
}

TerrainTiffIndex TerrainTiffIndex::GetDownIndex() const
{
	TerrainTiffIndex index = *this;
	index.nLat += 1;
	if (index.nLat > 24 + 6)
	{
		index.nLat = 1 - 6;
	}
	return index;
}

bool TerrainTiffIndex::operator==(const TerrainTiffIndex & _Right) const
{
	return (this->nIndex == _Right.nIndex);
}

bool TerrainTiffIndex::operator>(const TerrainTiffIndex & _Right) const
{
	return (this->nIndex > _Right.nIndex);
}

bool TerrainTiffIndex::operator<(const TerrainTiffIndex & _Right) const
{
	return (this->nIndex < _Right.nIndex);
}

HmsAviPf::Vec2 TerrainTiffIndex::Center() const
{
	HmsAviPf::Vec2 center;
	center.x = -180 + nLong * 5 - 2.5;
	center.y = 60 + 2.5 - nLat * 5;
	return center;
}

double TerrainTiffIndex::Left() const
{
	return -180 + (nLong - 1) * 5;
}

double TerrainTiffIndex::Right() const
{
	return -180 + nLong * 5;
}

double TerrainTiffIndex::Top() const
{
	return  60 - (nLat - 1) * 5;
}

double TerrainTiffIndex::Bottom() const
{
	return 60 - nLat * 5;
}

TerrainTiffIndex TerrainTiffIndex::GetIndex(const HmsAviPf::Vec2 & wgsPos)
{
	TerrainTiffIndex index;

	Vec2 wgsPosNormalized = wgsLonLatNormalize(wgsPos);

	index.nLong = short(int((wgsPosNormalized.x + 180.0f) / 5)) + 1;
	index.nLat = (short)floor((60 - wgsPosNormalized.y) / 5) + 1;

	return index;
}

TerrainTiffIndex TerrainTiffIndex::GetIndex(float lon, float lat)
{
	return GetIndex(Vec2(lon, lat));
}

//************************************
// Method:    wgsLonLatNormalize
// FullName:  TerrainTiffIndex::wgsLonLatNormalize
// Access:    public static 
// Returns:   HmsAviPf::Vec2
// Qualifier:
// Parameter: const HmsAviPf::Vec2 & wgsPos
//************************************
HmsAviPf::Vec2 TerrainTiffIndex::wgsLonLatNormalize(const HmsAviPf::Vec2 &wgsPos)
{
	Vec2 wgsPosNormalized;
	if (wgsPos.x >= -180 && wgsPos.x <= 180)
	{
		wgsPosNormalized.x = wgsPos.x;
	}
	else
	{
		auto temp = fmod(wgsPos.x, 360.0f);
		if (temp > 180)
		{
			temp -= 360;
		}
		else if (temp < -180)
		{
			temp += 360;
		}
		wgsPosNormalized.x = temp;
	}

	wgsPosNormalized.y = wgsPos.y;

	return wgsPosNormalized;
}

HmsColorCell::HmsColorCell()
{
	nColor = 0XFF000000;
}

short HmsTerrainRawData::GetAltitude(const HmsAviPf::Vec2 & pos)
{
	if (eLoadStatus == TerrainTiffStatus::Loaded)
	{
		Vec2 posNormalized = TerrainTiffIndex::wgsLonLatNormalize(pos);//avoid crossing
		auto posx = posNormalized.x - upperLeft.x;
		auto posy = upperLeft.y - posNormalized.y;
		//auto delta = pos - lowerLeft;		

		int nCol = nDataWidth*(posx / sizeGeo.width);
		int nRow = nDataHeight*(posy / sizeGeo.height);
		//int nIndex = nCol*nDataWidth + nRow;
		int nIndex = nRow*nDataWidth + nCol;
		
		if (pData[nIndex] < 0)
		{
			return 0;
		}
		return pData[nIndex];
	}
	return 0;
}
