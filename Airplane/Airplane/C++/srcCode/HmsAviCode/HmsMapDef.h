#pragma once

enum class HmsMapCenterMode
{
	CurAftCenter,				//中心点模式
	MoveAbleLayer,			//移动模式
};

enum class HmsSitMapMode
{
	none,				//无地图
	terrian,			//地形
	roadmap,			//街道
	satellite,			//卫星
};

struct HmsMapDescribe
{
public:
	HmsMapCenterMode						eCenterMode;
	HmsSitMapMode							eSitMapMode;		//地图的展现形式
	float									fRange;			//地图的范围
	float									fRangePix;		//地图范围的像素个数
	HmsAviPf::Size							sizeMap;
	HmsAviPf::Vec2							mapCenter;		//地图显示的中心点
	float									mapHeading;		//地图显示的航向
	double									mapLngPerTile;
};

