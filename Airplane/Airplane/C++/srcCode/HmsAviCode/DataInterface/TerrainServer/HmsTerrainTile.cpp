#include "HmsTerrainTile.h"
#include "HmsAviPf.h"

CHmsTerrainTile::CHmsTerrainTile()
{
	m_pTileData = new short[c_nTileWidth * c_nTileHeight];
	if (m_pTileData)
	{
		memset(m_pTileData, 0, sizeof(short)* c_nTileHeight * c_nTileWidth);
	}	
}


CHmsTerrainTile::~CHmsTerrainTile()
{
	if (m_pTileData)
	{
		delete[] m_pTileData;
		m_pTileData = nullptr;
	}
}

void CHmsTerrainTile::Clean()
{
	if (m_pTileData)
	{
		memset(m_pTileData, 0, sizeof(short) * c_nTileHeight * c_nTileWidth);
	}
}
