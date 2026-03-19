#include <cmath>
#include "TileDataMgrBase.h"
#include "Svs/FileManager.h"
#include "Svs/AltManager.h"
//#include "..\DataInterface\FsxDataServer\SimFsxDataDef.h"

NS_HMS_BEGIN

TileDataMgrBase::TileDataMgrBase()
: m_heartbeat(0)
{
    for (auto c : m_renderChunks)
    {
        delete c.second;
    }
    m_renderChunks.clear();
}


TileDataMgrBase::~TileDataMgrBase()
{
}

void TileDataMgrBase::UpdateBlocks(const Vec2& currLonLat)
{
    Vec2 lb(currLonLat.x - 0.5, currLonLat.y - 0.5);//left bottom
    Vec2 rb(currLonLat.x + 0.5, currLonLat.y - 0.5);
    Vec2 rt(currLonLat.x + 0.5, currLonLat.y + 0.5);
    Vec2 lt(currLonLat.x - 0.5, currLonLat.y + 0.5);
    if (lb.x < -180.0) lb.x += 360.0;
    if (lt.x < -180.0) lt.x += 360.0;
    if (rb.x > 180.0) rb.x -= 360.0;
    if (rt.x > 180.0) rt.x -= 360.0;

    std::set<TileKey> currNeededTile;
    currNeededTile.insert(TileKey(floorf(lb.x), floorf(lb.y)));
    currNeededTile.insert(TileKey(floorf(lt.x), floorf(lt.y)));
    currNeededTile.insert(TileKey(floorf(rb.x), floorf(rb.y)));
    currNeededTile.insert(TileKey(floorf(rt.x), floorf(rt.y)));

    std::set<TileKey> addChunks;
    std::set<TileKey> delChunks;

    for (auto c : currNeededTile)
    {//if the tile is needed and not in renderChunks, insert it into renderChunk
        auto result = m_renderChunks.find(c);
        if (result == m_renderChunks.end())
        {
            addChunks.insert(c);
        }
    }

    for (auto c : m_renderChunks)
    {//insert the needless tile into delChunks
        auto result = currNeededTile.find(c.first);
        if (result == currNeededTile.end())
        {
            delChunks.insert(c.first);
        }
    }

    //delete needless barrier tile
    for (auto c : delChunks)
    {
        auto rubbishChunk = m_renderChunks.find(c);
        HMS_SAFE_RELEASE_NULL(rubbishChunk->second);
        m_renderChunks.erase(c);
        m_heartbeat++;
    }

    //create new barrier tile 
    for (auto c : addChunks)
    {
        DataBlock* newblock = CreateDataBlock(c);
        m_renderChunks.insert(make_pair(c, newblock));
        m_heartbeat++;
    }

    addChunks.clear();
    delChunks.clear();

    //now, the renderChunks is ready

}


int TileDataMgrBase::GetHeartBeat()
{
    return m_heartbeat;
}

DataBlock::DataBlock(int lon, int lat)
: m_lon(lon)
, m_lat(lat)
{

}

int DataBlock::LoadBinaryFile(const char* file, unsigned char**data)
{
    FILE* pFile = nullptr;

    long len = 0;

#ifdef WIN32
    fopen_s(&pFile, file, "rb");
#else
    pFile = fopen(file, "rb");
#endif 
    if (pFile)
    {
        unsigned char* orgData = nullptr;
        int res = 0;
        fpos_t pos;
        fseek(pFile, 0, SEEK_END);
        len = ftell(pFile);
        fgetpos(pFile, &pos);

#if defined(ANDROID)
        printf("LoadBinaryFile %s  :%ld,pos:%d\n", file, len, (int)pos);
#elif defined(__linux)
        printf("LoadBinaryFile %s  :%ld,pos:%d\n", file, len, pos.__pos);
#elif defined(__vxworks)
        printf("LoadBinaryFile %s  :%ld,pos:%d\n", file, len, pos._Off);
#elif defined(_WIN32)
        printf("LoadBinaryFile %s  :%ld,pos:%d\n", file, len, (int)pos);
#else

#endif

        orgData = new unsigned char[len];
        fseek(pFile, 0, SEEK_SET);
        res = fread(orgData, len, 1, pFile);
        //	res = fwrite(orgData, len, 1, pFile);
        *data = orgData;
        fclose(pFile);
    }
    return len;
}

NS_HMS_END



