#pragma once

#include "HmsRouteElementsList.h"
#include "HmsNavDataStruct.h"

class CHmsFrame2DRoot;

class CHmsRouteElementsEdit : public HmsAviPf::CHmsNode, public HmsUiEventInterface
{
public:

public:
	CHmsRouteElementsEdit();
	~CHmsRouteElementsEdit();

	CREATE_FUNC_BY_SIZE(CHmsRouteElementsEdit)

	virtual bool Init(const HmsAviPf::Size & windowSize);

	void SetWptNodeData(const std::vector<FPL2DNodeInfoStu> &vec);

private:
	RefPtr<CHmsRouteElementsList> m_pRouteList;
};

