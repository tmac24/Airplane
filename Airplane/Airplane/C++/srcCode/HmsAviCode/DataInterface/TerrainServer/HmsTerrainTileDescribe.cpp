#include "HmsTerrainTileDescribe.h"


CHmsTerrainTileDescribe::CHmsTerrainTileDescribe()
{
	m_nIndex = 0;
	m_nLayer = 0;
	m_nBindCol = 0;
	m_nBindRow = 0;
}


CHmsTerrainTileDescribe::~CHmsTerrainTileDescribe()
{
}

bool CHmsTerrainTileDescribe::IsValid() const
{
	if (!m_indexTif.IsValid())
	{
		return false;
	}

	unsigned int nMaxTile = GetTileCnt();
	if (m_nIndex < nMaxTile * nMaxTile)
	{
		return true;
	}
	return false;
}

CHmsTerrainTileDescribe CHmsTerrainTileDescribe::GetLeftDescribe() const
{
	unsigned int nMaxTile = GetTileCnt();
	CHmsTerrainTileDescribe out = *this;

	unsigned int nCol = m_nIndex%nMaxTile;
	if (nCol == 0)
	{
		out.m_nIndex += nMaxTile - 1;
		out.m_indexTif = m_indexTif.GetLeftIndex();
	}
	else
	{
		out.m_nIndex -= 1;
	}
	out.m_nBindCol--;
	return out;
}

CHmsTerrainTileDescribe CHmsTerrainTileDescribe::GetRightDescribe() const
{
	unsigned int nMaxTile = GetTileCnt();
	CHmsTerrainTileDescribe out = *this;

	unsigned int nCol = m_nIndex%nMaxTile;
	if (nCol == nMaxTile - 1)
	{
		out.m_nIndex -= nMaxTile - 1;
		out.m_indexTif = m_indexTif.GetRightIndex();
	}
	else
	{
		out.m_nIndex += 1;
	}
	out.m_nBindCol++;
	return out;
}

CHmsTerrainTileDescribe CHmsTerrainTileDescribe::GetUpDescribe() const
{
	unsigned int nMaxTile = GetTileCnt();
	CHmsTerrainTileDescribe out = *this;

	unsigned int nRow = m_nIndex/nMaxTile;
	if (nRow == 0)
	{
		unsigned int nTotalTile = nMaxTile * nMaxTile;
		out.m_nIndex = (m_nIndex + nTotalTile - nMaxTile)%nTotalTile;
		out.m_indexTif = m_indexTif.GetUpIndex();
	}
	else
	{
		out.m_nIndex -= nMaxTile;
	}
	out.m_nBindRow--;
	return out;
}

CHmsTerrainTileDescribe CHmsTerrainTileDescribe::GetDownDescribe() const
{
	unsigned int nMaxTile = GetTileCnt();
	CHmsTerrainTileDescribe out = *this;

	unsigned int nRow = m_nIndex / nMaxTile;
	if (nRow == nMaxTile - 1)
	{
		unsigned int nTotalTile = nMaxTile * nMaxTile;
		out.m_nIndex = (m_nIndex + nMaxTile) % nTotalTile;
		out.m_indexTif = m_indexTif.GetDownIndex();
	}
	else
	{
		out.m_nIndex += nMaxTile;
	}
	out.m_nBindRow++;
	return out;
}

HmsAviPf::Vec2 CHmsTerrainTileDescribe::Center() const
{
	unsigned int nMaxTile = GetTileCnt();
	auto posX = m_indexTif.Left() + 5.0 / nMaxTile * (m_nIndex%nMaxTile + 0.5);
	auto posY = m_indexTif.Top() - 5.0 / nMaxTile * (m_nIndex / nMaxTile + 0.5);
	return HmsAviPf::Vec2(posX, posY);
}

double CHmsTerrainTileDescribe::Left() const
{
	unsigned int nMaxTile = GetTileCnt();

	auto pos = m_indexTif.Left() + 5.0 / nMaxTile * (m_nIndex%nMaxTile);
	return pos;
}

double CHmsTerrainTileDescribe::Right() const
{
	unsigned int nMaxTile = GetTileCnt();

	auto pos = m_indexTif.Left() + 5.0 / nMaxTile * (m_nIndex%nMaxTile+1);
	return pos;
}

double CHmsTerrainTileDescribe::Top() const
{
	unsigned int nMaxTile = GetTileCnt();

	auto pos = m_indexTif.Top() - 5.0 / nMaxTile * (m_nIndex/nMaxTile);
	return pos;
}

double CHmsTerrainTileDescribe::Bottom() const
{
	unsigned int nMaxTile = GetTileCnt();

	auto pos = m_indexTif.Top() - 5.0 / nMaxTile * (m_nIndex / nMaxTile + 1);
	return pos;
}

bool CHmsTerrainTileDescribe::operator==(const CHmsTerrainTileDescribe & _Right) const
{
	if (m_nIndex == _Right.m_nIndex &&
		m_nLayer == _Right.m_nLayer &&
		m_indexTif == _Right.m_indexTif)
	{
		return true;
	}

	return false;
}

bool CHmsTerrainTileDescribe::operator>(const CHmsTerrainTileDescribe & _Right) const
{
	if (m_nLayer > _Right.m_nLayer)
	{
		return true;
	}
	if (m_indexTif > _Right.m_indexTif)
	{
		return true;
	}
	else if (m_nIndex > _Right.m_nIndex)
	{
		return true;
	}
	return false;
}

bool CHmsTerrainTileDescribe::operator<(const CHmsTerrainTileDescribe & _Right) const
{
	if (m_nLayer < _Right.m_nLayer)
	{
		return true;
	}
	if (m_indexTif < _Right.m_indexTif)
	{
		return true;
	}
	else if (m_nIndex < _Right.m_nIndex)
	{
		return true;
	}
	return false;
}

CHmsTerrainTileDescribe CHmsTerrainTileDescribe::GetMapTile(const HmsAviPf::Vec2 & wgsPos, unsigned int nLayer)
{
	CHmsTerrainTileDescribe mtd;
	mtd.m_indexTif = TerrainTiffIndex::GetIndex(wgsPos);
	mtd.m_nLayer = nLayer;

	unsigned int nMaxTile = 1 << nLayer;
	double dSegLen = 5.0 / nMaxTile;

	double left = mtd.m_indexTif.Left();
	double bottom = mtd.m_indexTif.Bottom();

	auto deltaX = wgsPos.x - left;
	auto deltaY = wgsPos.y - bottom;

	unsigned int nCol = (unsigned int)(deltaX / dSegLen);
	unsigned int nRow = nMaxTile - 1 - (unsigned int)(deltaY / dSegLen);

	if (nRow >= nMaxTile)
	{
		nRow = nMaxTile - 1;
	}

	if (nCol >= nMaxTile)
	{
		nCol = nMaxTile - 1;
	}

	mtd.m_nIndex = nRow * nMaxTile + nCol;

	return mtd;
}

double CHmsTerrainTileDescribe::GetOffset() const
{
	unsigned int nMaxTile = GetTileCnt();
	double dSegLen = 5.0 / nMaxTile;

	return dSegLen;
}
