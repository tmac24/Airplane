#pragma once
#include "HmsGlobal.h"
#include "HmsMapLayer.h"
#include "HmsMapLayerInterface.h"
#include "HmsMapDef.h"
#include "base/HmsClippingNode.h"

typedef std::function<bool(float fNewRange, float fOldRange)> LPFuncHmsMapRangeCheck;

// enum class HmsMapCenterMode
// {
// 	CurAftCenter,				//中心点模式
// 	MoveAbleLayer,			//移动模式
// };

class CHmsMapNode : public HmsAviPf::CHmsNode
{
public:
	CHmsMapNode();
	~CHmsMapNode();

	CREATE_FUNC(CHmsMapNode);

	virtual bool Init() override;

	/*设置地图中心点，和向上的方向*/
	void SetMapData(const HmsAviPf::Vec2 & posCenter, float fHeading);

	void SetMapMode(HmsSitMapMode mapMode);
	HmsSitMapMode GetMapMode(){ return m_eSitMapMode; }

	void SetRange(float fRange, bool bUpdate = true);
	float GetRange(){ return m_fRange; }
	void SetRangePix(float fRangePix);

	void SetMapSize(const HmsAviPf::Size & size);

	void SetMapRangeCheck(const LPFuncHmsMapRangeCheck & funCheck);

	void SetClipRect(const HmsAviPf::Rect & rect);

	void SetUseClipRect(bool bUse){ m_bClipMap = bUse; }

	void SetMapMove(const HmsAviPf::Vec2 & delta);

	void SetCenterMode(HmsMapCenterMode eMode);

	bool IsCenterMode(){ return m_eCenterMode == HmsMapCenterMode::CurAftCenter; }

	void AddMapLayer(const std::string & strLayerName, CHmsMapLayerInterface * pInterface, int nLayerID = 1);

	void UpdateMap();

	void SetLayerVisible(const std::string & strName, bool bVisible);

	void SetNorthMode(bool bNorth);

protected:
	void InitEventDispatcher();
	void OnMapScale(float fScale);
	void OnMapMove(const HmsAviPf::Vec2 & delta);
	void OnMapRangeChange(float fRange);

	void AddMapLayer(HmsAviPf::CHmsNode * node);
	void RemoveMapLayer(HmsAviPf::CHmsNode * node);
	void UpdateDataForMoveableLayer();

private:
	bool									m_bClipMap;			//是否剪切地图
	HmsAviPf::RefPtr<HmsAviPf::CHmsClippingNode>	m_nodeClip;			//剪切节点
	HmsAviPf::Rect							m_rectClip;

	HmsMapCenterMode						m_eCenterMode;

	HmsAviPf::RefPtr<CHmsMapLayer>			m_layerMap;			//地图图层
	//HmsAviPf::RefPtr<CHmsGeomDrawNode>		m_layerGeomDrawNode;		//行政区域绘制图层
	//HmsAviPf::RefPtr<CHmsRouteLayer>			m_layerRouteLayer;
	std::map<std::string, CHmsMapLayerInterface*>	m_mapLayer;

	HmsSitMapMode							m_eSitMapMode;		//地图的展现形式
	float									m_fRange;			//地图的范围
	float									m_fRangePix;		//地图范围的像素个数

	HmsAviPf::Vec2							m_mapCenter;		//地图显示的中心点
	float									m_mapHeading;		//地图显示的航向

	double									m_mapLngPerTile;

	HmsAviPf::Size							m_sizeMap;

	LPFuncHmsMapRangeCheck					m_funcMapRangeCheck;		//检查地图范围判断是否修改


	int										m_nLayer;
	float									m_fLayerScale;

public:
	static float							s_nMaxMapLayer;
};

