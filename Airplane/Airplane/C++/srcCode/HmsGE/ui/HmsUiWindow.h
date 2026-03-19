#pragma once

#include "base/HmsClippingNode.h"
#include "display/HmsDrawNode.h"
#include "ui/HmsUiEventInterface.h"
#include "ui/HmsUiImagePanel.h"
#include "base/HmsTimer.h"
NS_HMS_BEGIN

class CHmsUiWindow : public CHmsClippingNode, public HmsUiEventInterface
{
public:
	CHmsUiWindow();
	~CHmsUiWindow();

	static CHmsUiWindow* Create(const Size & sizeShow);

	void AddWindowItem(CHmsNode * node);

	void SetWindowShowRect(const Rect & rect);

	CHmsUiPanel * GetUiPanel();

	void RegisterAllChildUiEvent();

	void SetMinimumShowHeight(float fMinimunShowHeight);

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
	const float						c_fItemSpace = 20.0f;
};

NS_HMS_END