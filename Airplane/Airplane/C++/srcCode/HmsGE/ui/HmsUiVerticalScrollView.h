#pragma once

#include "base/HmsClippingNode.h"
#include "display/HmsDrawNode.h"
#include "ui/HmsUiEventInterface.h"
#include "ui/HmsUiImagePanel.h"
#include "base/HmsTimer.h"
NS_HMS_BEGIN

class CHmsUiVerticalScrollView : public CHmsClippingNode, public HmsUiEventInterface
{
public:
	CHmsUiVerticalScrollView();
	~CHmsUiVerticalScrollView();

	static CHmsUiVerticalScrollView* Create(const Size & sizeShow);

	void AddWindowItem(CHmsNode * node);
	void RemoveAllItem();

	void SetWindowShowRect(const Rect & rect);

	void SetWindowShowRectWithBoder(const Rect & rect, float fBoderLeft, float fBoderRight, float fBoderUp, float fBoderDown);

	CHmsUiPanel * GetUiPanel();

	void RegisterAllChildUiEvent();

	void SetMinimumShowHeight(float fMinimunShowHeight);

	HmsAviPf::Size GetPanelSize();

	void SetItemVSpace(float fSpace){ m_fItemSpace = fSpace;}

	//use the child order
	void CheckAndSetIndexWasShow(int nIndex);
	void CheckAndSetItemWasShow(CHmsNode * nodeItem);

	void ScrollToTop();

protected:
	virtual bool Init(const Size & sizeShow);

	virtual bool OnPressed(const Vec2 & posOrigin);

	virtual void OnMove(const Vec2 & posOrigin);

	virtual void OnReleased(const Vec2 & posOrigin);

	RefPtr<CHmsDrawNode>			m_nodeMask;					//the mask of the node
	RefPtr<CHmsUiPanel>				m_nodePanel;				//the panel of the node
	float							m_fPanelContentHeight;	
	CHmsTime						m_time;
	Vec2							m_posPressStart;
	Vec2							m_posPanelPressStart;
	float							m_fMinimumShowHeight;
	float							m_fItemSpace;
};

NS_HMS_END