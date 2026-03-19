#pragma once

#include <map>

#include "base/HmsNode.h"
#include "display/HmsDrawNode.h"
#include "ui/HmsUiEventInterface.h"
#include "ui/HmsUiVerticalScrollView.h"
#include "base/HmsActionTimeRef.h"
#include "HmsStandardDef.h"

using namespace HmsAviPf;

enum class ClippingEventMode
{
	E_TRANSMIT,
	E_DRAG,
	E_DRAG_CLICK
};

class CHmsScratchPadClipping : public CHmsClippingNode, public HmsUiEventInterface
{
public:
	CHmsScratchPadClipping();
	~CHmsScratchPadClipping();

	CREATE_FUNC(CHmsScratchPadClipping);

	void AddChildWithEvent(CHmsNode * child, bool bEvent);

	void EdgeDetection();

	void SetEventMode(ClippingEventMode mode);

	void ChildMoveDistance(int x, int y);

	void SetEdgeDetection_0_0(bool b){ m_bEdgeBy00 = b; }

protected:
	virtual bool Init();

	virtual bool OnPressed(const Vec2 & posOrigin);

	virtual void OnMove(const Vec2 & posOrigin);

	virtual void OnReleased(const Vec2 & posOrigin);

	void EdgeDetection_0_0();
	void EdgeDetection_05_05();

private:
	RefPtr<CHmsNode> m_pChildNode;

	Vec2 m_posPressStart;
	Vec2 m_posChildPressStart;

	ClippingEventMode m_eventMode;

	bool m_bEdgeBy00;
};

