#include "HmsTerrainLegend.h"
#include "display/HmsDrawNode.h"
#include "math/HmsGeometry.h"
#include "HmsGlobal.h"
#include <string>

CHmsTerrainLegend::CHmsTerrainLegend()
{

}


CHmsTerrainLegend::~CHmsTerrainLegend()
{

}

bool CHmsTerrainLegend::Init(const HmsAviPf::Size & size)
{
    if (!CHmsNode::Init())
    {
        return false;
    }
    SetContentSize(size);

    const float marginLeft = 2;
    const float marginBottom = 2;

    auto pBgNode = HmsAviPf::CHmsDrawNode::Create();
    pBgNode->SetAnchorPoint(HmsAviPf::Vec2(0, 0));
    pBgNode->SetPosition(0, 0);
    this->AddChild(pBgNode);
    pBgNode->DrawSolidRect(
        HmsAviPf::Rect(0, 0, size.width, size.height),
        HmsAviPf::Color4F(HmsAviPf::Color3B(0xFE, 0xFE, 0xFE)));
    pBgNode->DrawSolidRect(
        HmsAviPf::Rect(marginLeft, marginBottom, size.width - marginLeft * 2, size.height - marginBottom * 2),
        HmsAviPf::Color4F(HmsAviPf::Color3B(0x33, 0x33, 0x33)));

    _nodeContent = CHmsNode::Create();
    AddChild(_nodeContent);

    return true;
}

bool CHmsTerrainLegend::ReGenerate(const warningColorStyle& wcs)
{
    _nodeContent->RemoveAllChildrenWithCleanup(true);

    auto sz = GetContentSize();
    auto margin = HmsAviPf::Size(4, 4);//最外的四边
    auto margin2 = HmsAviPf::Size(4, 4);//
    auto margin3 = HmsAviPf::Size(2, 2);//colorbox 的四边

    auto cntThresholds = wcs.ThresholdCnt();
    auto cntColors = cntThresholds + 1;

    auto horizontalPos1 = sz.height * 0.75F;
    auto verticalPos1 = sz.width * 0.70F;
    auto colorboxwidth = sz.width * 0.20F;

    auto h2 = horizontalPos1 - margin.height - margin2.height;

    auto colorRect = HmsAviPf::Rect(
        verticalPos1 + margin3.width,
        margin.height + margin3.height + margin.height,
        colorboxwidth,
        h2 - margin.height - margin2.height - 2 * margin3.height);

    auto colorboxH = colorRect.size.height / cntColors;

    auto colorRectWithMargin = HmsAviPf::Rect(
        colorRect.origin.x - margin3.width,
        colorRect.origin.y - margin3.height,
        colorRect.size.width + margin3.width * 2,
        colorRect.size.height + margin3.height * 2
        );

    auto pColorBoxFrame = HmsAviPf::CHmsDrawNode::Create();
    pColorBoxFrame->SetAnchorPoint(HmsAviPf::Vec2(0, 0.0));
    pColorBoxFrame->SetPosition(0, 0);
    _nodeContent->AddChild(pColorBoxFrame);
    pColorBoxFrame->DrawSolidRect(
        colorRectWithMargin,
        HmsAviPf::Color4F(HmsAviPf::Color3B(0xFE, 0xFE, 0xFE)));

    pColorBoxFrame->DrawHorizontalLine(margin.width * 2, sz.width - margin.width * 2, horizontalPos1, 1.5F);

    auto lbTerrain = CHmsGlobal::CreateLanguageLabel("TERRAIN", 26);
	if (lbTerrain)
	{
		lbTerrain->SetAnchorPoint(HmsAviPf::Vec2(0.5F, 0.5F));
		lbTerrain->SetPosition(HmsAviPf::Vec2(sz.width / 2, sz.height*(0.75F + 0.25F / 2)));

		_nodeContent->AddChild(lbTerrain);
	}

    for (int i = 0; i < cntThresholds; ++i)
    {
        auto pats = wcs.AltThresholds();
        auto at = std::to_string((int)(*pats)[i]) + " M";
        auto lbth = CHmsGlobal::CreateLanguageLabel(at.c_str(), 22);
        lbth->SetAnchorPoint(HmsAviPf::Vec2(1.0F, 0.5F));
        lbth->SetPosition(HmsAviPf::Vec2(verticalPos1 - margin2.width, colorRect.origin.y + colorRect.size.height - colorboxH * (i + 1)));
        _nodeContent->AddChild(lbth);
    }

    for (int i = 0; i < cntColors; ++i)
    {
        auto box = HmsAviPf::Rect(colorRect.origin.x, colorRect.origin.y + colorRect.size.height - colorboxH * (i + 1), colorRect.size.width, colorboxH);
        auto pColorBoxNode1color = HmsAviPf::CHmsDrawNode::Create();

        auto clr = (*wcs.Colors())[i];
        pColorBoxNode1color->DrawSolidRect(
            box,
            HmsAviPf::Color4F(clr.x, clr.y, clr.z, clr.w));
        _nodeContent->AddChild(pColorBoxNode1color);
    }

    return true;
}
