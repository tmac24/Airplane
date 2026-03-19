#pragma once

#include "ui/HmsUiButton.h"
#include "HmsNavDataStruct.h"

using namespace HmsAviPf;
class CHmsFrame2DRoot;

class CHmsFrame2DToolbox : public HmsAviPf::CHmsNode, public HmsUiEventInterface
{
public:
	CHmsFrame2DToolbox();
	~CHmsFrame2DToolbox();

	CREATE_FUNC_BY_SIZE(CHmsFrame2DToolbox)
	virtual bool Init(const HmsAviPf::Size & windowSize);
	void SetCurSelectIndex(FPLNodeIndex index);
	void SetFrame2DRoot(CHmsFrame2DRoot *p);

private:
	CHmsFrame2DRoot *m_pFrame2DRoot;
	FPLNodeIndex m_curIndex;
};
