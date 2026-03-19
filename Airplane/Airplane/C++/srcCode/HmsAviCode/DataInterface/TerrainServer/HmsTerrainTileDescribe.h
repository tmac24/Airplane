#pragma once
#include "HmsAviPf.h"
#include "HmsTerrainTiffDef.h"

class CHmsTerrainTileDescribe
{
public:
	TerrainTiffIndex	m_indexTif;
	unsigned int	m_nLayer;
	unsigned int	m_nIndex;
	int				m_nBindRow;		//绑定的行
	int				m_nBindCol;		//绑定的列

	CHmsTerrainTileDescribe();
	~CHmsTerrainTileDescribe();

	bool IsValid() const;

	CHmsTerrainTileDescribe GetLeftDescribe() const;
	CHmsTerrainTileDescribe GetRightDescribe() const;
	CHmsTerrainTileDescribe GetUpDescribe() const;
	CHmsTerrainTileDescribe GetDownDescribe() const;

	bool operator==(const CHmsTerrainTileDescribe & _Right) const;
	bool operator>(const CHmsTerrainTileDescribe & _Right) const;
	bool operator<(const CHmsTerrainTileDescribe & _Right) const;

	HmsAviPf::Vec2 Center() const;
	double Left() const;
	double Right() const;
	double Top() const;
	double Bottom() const;

	double GetOffset() const;

	static CHmsTerrainTileDescribe GetMapTile(const HmsAviPf::Vec2 & wgsPos, unsigned int nLayer);

protected:
	inline unsigned int GetTileCnt() const { return 1 << m_nLayer; }
};

