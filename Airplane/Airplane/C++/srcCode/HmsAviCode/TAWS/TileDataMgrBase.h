#pragma once
#include "math/Vec3.h"
#include "math/Vec2.h"
#include <vector>
#include <map>
#include "base/Ref.h"
#include <set>

NS_HMS_BEGIN

typedef std::pair<int, int> TileKey;
class DataBlock :public Ref
{
public:
	DataBlock(int lon, int lat);
protected:
	int m_lon;//��block�ľ��ȣ����ͣ�
	int m_lat;//��block��γ��
	int LoadBinaryFile(const char* file, unsigned char**data);
};


class TileDataMgrBase
{
public:	
	TileDataMgrBase();
	virtual ~TileDataMgrBase();
	
	void UpdateBlocks(const Vec2& currLonLat);
	int GetHeartBeat();
	
private:
    int m_heartbeat;

protected:
	std::map<TileKey, DataBlock*> m_renderChunks;
	virtual DataBlock* CreateDataBlock(const TileKey& tkey) = 0;	
};


NS_HMS_END