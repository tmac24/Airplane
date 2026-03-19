#include "HmsGauge.h"
#include "display/HmsDrawNode.h"

USING_NS_HMS;

CHmsGauge::CHmsGauge():
m_curGaugeMode(GaugeMode::GAUGE_MODE_ERROR)
{
	failRectColor = Color4F(Color4B(0x8C, 0x24, 0x13, 0x96));
	failRectBorderColor = Color4F(Color4B(0xC3, 0xC1, 0xC1, 0xFF));
	failCrossLineColor = Color4B(0xC8, 0x24, 0x08, 0xFF);
}


CHmsGauge::~CHmsGauge()
{
}

bool CHmsGauge::Init()
{
	if (!CHmsNode::Init())
	{
		return false;
	}

	return true;
}

void CHmsGauge::DrawFailWin(CHmsDrawNode *failWindNode, float leftTopX, float leftTopY, float rightBottomX, float rightBottomY)
{
	failWindNode->DrawSolidRect(leftTopX, leftTopY, rightBottomX, rightBottomY, failRectColor);
	std::vector<Vec2> vPos;
	vPos.push_back(Vec2(leftTopX, leftTopY));
	vPos.push_back(Vec2(rightBottomX, rightBottomY));
	failWindNode->DrawLineStrip(vPos, 2.0f, failCrossLineColor);
	vPos.clear();
	vPos.push_back(Vec2(rightBottomX, leftTopY));
	vPos.push_back(Vec2(leftTopX, rightBottomY));
	failWindNode->DrawLineStrip(vPos, 2.0f, failCrossLineColor);
	failWindNode->DrawInnerRect(leftTopX, leftTopY, rightBottomX, rightBottomY, 1.0f, failRectBorderColor);
}
