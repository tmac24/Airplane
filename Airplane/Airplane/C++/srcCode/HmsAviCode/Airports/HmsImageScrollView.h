#pragma once

#include "base/HmsClippingNode.h"
#include "display/HmsDrawNode.h"
#include "ui/HmsUiEventInterface.h"
#include "ui/HmsUiImagePanel.h"
#include "base/HmsTimer.h"
NS_HMS_BEGIN

class CHmsImageScrollView : public CHmsClippingNode, public HmsUiEventInterface
{
public:
	CHmsImageScrollView();
	~CHmsImageScrollView();

	static CHmsImageScrollView* Create(const Size & sizeShow);

	void AttachImageNode(CHmsImageNode *pNode);

	void SetImageToCenter();

	void ImageMoveDistance(int x, int y);

protected:
	virtual bool Init(const Size & sizeShow);

	virtual bool OnPressed(const Vec2 & posOrigin);

	virtual void OnMove(const Vec2 & posOrigin);

	virtual void OnReleased(const Vec2 & posOrigin);

	void EdgeDetection();

	RefPtr<CHmsImageNode> m_pImageNode;
	RefPtr<CHmsDrawNode> m_nodeMask;					//the mask of the node
	
	CHmsTime m_time;
	Vec2 m_posPressStart;
	Vec2 m_posPanelPressStart;
};

NS_HMS_END