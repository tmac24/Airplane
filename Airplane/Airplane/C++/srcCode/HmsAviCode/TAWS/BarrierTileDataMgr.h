#pragma once
#include "math/Vec3.h"
#include "math/Vec2.h"
#include <vector>
#include <map>
#include "base/Ref.h"
#include <set>
#include "TileDataMgrBase.h"


#define METET_PER_FEET    0.3048f
#define KNOTS_TO_KMH 1.852f
#define NM_TO_M 1852.0f
#define NM_TO_FOOT 6076.1154855643

NS_HMS_BEGIN

#pragma pack(1)
struct BarrierParameters
{	
	long long record_id;//����ID��	
	char obsType[51];/*�ϰ�������*/
	char lightingType[2];	
	double latitude;//γ��	
	double longitude;/*����*/	
	int altitude;//�ϰ�����ߵ�ƽ�����θ�	
	int height;//�ϰ�����ߵ���ظ߶�
};
#pragma pack() 


class BarrierBlock: public DataBlock
{
public:
	BarrierBlock(int lon, int lat);
	BarrierBlock(const Vec2&);
	BarrierBlock(const TileKey&);
	~BarrierBlock();
	
	const std::vector<BarrierParameters*>& GetBarrierInfos(void);
private:
	std::vector<BarrierParameters*> m_BarrierInfos;/*�����ϰ�������  <BarrierParameters>*/	
	void InitBarrierBlockData();
};


class BarrierTileDataMgr : public TileDataMgrBase
{
public:
	static BarrierTileDataMgr* getInstance();
	const std::set<BarrierBlock*>& GetRenderBlocks();
	const std::vector<BarrierParameters*>& GetNearestBarrierInfors(const Vec2& LonLat);
private:
	BarrierTileDataMgr();
	static BarrierTileDataMgr* m_pInstance;
	std::set<BarrierBlock*> m_renderBlockBuffer;
	std::vector<BarrierParameters*> m_nearrestBarrierBuffer;
	BarrierBlock* CreateDataBlock(const TileKey& tkey) override;
};

NS_HMS_END