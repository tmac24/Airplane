#pragma once
#include "HmsMapLoadManager.h"
//#include "CCOrderMap.h"
#include <map>

class CHmsMapLayer : public HmsAviPf::CHmsNode
{
public:
	CHmsMapLayer();
	~CHmsMapLayer();

	CREATE_FUNC(CHmsMapLayer);

	bool Init();

	void OnLoadMapTile(HmsMapTile * pMapTile);

	void OnRemoveMapTile(HmsMapTile * pMapTile);

	void OnChangeLayer(int nLayer);

	void OnChangeMapSource();

	void SetCurrentAftData(const HmsAviPf::Vec2 & pos, float fHeading);
	void SetCurrentAftData(float fHeading);

	HmsAviPf::Vec2 GetLngLatByOffset(const HmsAviPf::Vec2 & deltaScreen);
	static HmsAviPf::Vec2 GetLngLatByOffset(const HmsAviPf::Vec2 & posCenter, float fHeading, int nLayer, float fLayerScale, const HmsAviPf::Vec2 & deltaScreen);

	virtual void update(float delta);

	void SetCurrentRange(float fRange);

	void SetLayerAndScaled(int nLayer, float fLayerScale);

	CHmsMapLoadManager * GetMapLoadManager(){ return m_pLoadManager; }

protected:
	void SetMapCenter(const HmsAviPf::Vec2 & pos);
	void SetCurrentHeading(float fHeading);

	void InitEventDispatcher();

	void OnScaleLayer(float fScale);

	HmsAviPf::CHmsNode * GetOrCreateMapTileNode(int nLayer);
	HmsAviPf::CHmsNode * GetCurrentNode();

private:
	//获取低一级别的层级
	int GetLowLayer();

	//获取高一级别的层级
	int GetUpLayer();

private:
	CHmsMapLoadManager *					m_pLoadManager;

	float									m_fScaleLayer;		//当前级别地图的缩放倍数
	float									m_fScaleLayerDown;

	HmsAviPf::CHmsNode	*						m_pNodeRoot;		//地图跟节点
	//HmsAviPf::OrderMap<int, HmsAviPf::CHmsNode*>		m_mapNodeMapTile;
	std::map<int, HmsAviPf::CHmsNode*>		m_mapNodeMapTile;
	int										m_nLayer;				//当前图层
	int										m_nLayerDown;			//当前图层的底层图层
	int										m_nTileCnt;
	double									m_dLngPerTile;
	double									m_dLatPerTile;
	HmsAviPf::Vec2							m_scaleOrgShowTile;			//原始地图放缩倍数


	HmsAviPf::Vec2							m_posCenter;
};

