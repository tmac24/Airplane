#pragma once
#include "base/HmsNode.h"
#include "ui/HmsUiEventInterface.h"
#include "Warning/HmsWarningColorStyle.h"


class CHmsTerrainLegend : public HmsAviPf::CHmsNode
{
public:
	CHmsTerrainLegend();
	virtual ~CHmsTerrainLegend();

	CREATE_FUNC_BY_SIZE(CHmsTerrainLegend)

	virtual bool Init(const HmsAviPf::Size & size);
    bool ReGenerate(const warningColorStyle& wcs);
private:
    HmsAviPf::RefPtr<HmsAviPf::CHmsNode>    _nodeContent;
};