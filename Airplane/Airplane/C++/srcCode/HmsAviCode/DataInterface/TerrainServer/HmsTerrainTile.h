#pragma once

#include "HmsTerrainTileDescribe.h"

class CHmsTerrainTile
{
public:
    CHmsTerrainTile();
    ~CHmsTerrainTile();

    void Clean();

    inline void SetLayer(int nLayer){ m_nLayer = nLayer; }
    inline int GetLayer(){ return m_nLayer; }

    short * GetData(){ return m_pTileData; }

    inline static int GetWidth(){ return c_nTileWidth; }
    inline static int GetHeight(){ return c_nTileHeight; }

    void SetTileDescribe(const CHmsTerrainTileDescribe & des){ m_tileDescribe = des; }
    CHmsTerrainTileDescribe & GetTileDescribe(){
        return m_tileDescribe;
    }

protected:
    int					m_nLayer;
    short *m_pTileData;

    CHmsTerrainTileDescribe	m_tileDescribe;

    const static int c_nTileWidth = 750;
    const static int c_nTileHeight = 750;
};

