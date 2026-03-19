#pragma once
#include "ui/HmsUiButtonAbstract.h"
#include "display/HmsStretchImageNode.h"
#include "display/HmsLabel.h"
#include "base/RefPtr.h"
#include "HmsUiEventInterface.h"

NS_HMS_BEGIN

class CHmsImageNode;

class CHmsUiHotAreaButton : public CHmsNode, public CHmsUiButtonAbstract
{
public:
	CHmsUiHotAreaButton();
	virtual ~CHmsUiHotAreaButton();


	static CHmsUiHotAreaButton* Create( const Size & size, const HmsUiBtnCallback& callback = nullptr);

	void AddContentAlignLeftCenter(CHmsNode * node, const Vec2 & offset);
	void AddContentAlignRightCenter(CHmsNode * node, const Vec2 & offset);
	void AddContentAlignCenter(CHmsNode * node, const Vec2 & offset);
protected:

};

NS_HMS_END