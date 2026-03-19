#pragma once

#include "ui/HmsUiEventInterface.h"
#include "ui/HmsUiStretchButton.h"
#include "base/HmsTimer.h"
#include "base/HmsActionTimeRef.h"
#include "display/HmsDrawNode.h"
#include "../HmsGlobal.h"
#include "ui/HmsUiImagePanel.h"
#include "HmsNavDataStruct.h"
#include "base/HmsScissorNode.h"
#include "Nav/ScratchPad/HmsScratchPadClipping.h"

using namespace HmsAviPf;

class CHmsFrame2DRoot;

class CHmsRouteElementsList : public HmsAviPf::CHmsNode, public HmsUiEventInterface
{
public:
	enum class RouteElementType
	{
		AIRPORT,
		AIRWAY,
		VOR,
		SID_STAR,
		WAYPOINT,
		NDB,
		TRAFFIC,
		_ERROR
	};

	struct RouteElementStu
	{
		WptNodeBrief nodeStu;

		RefPtr<CHmsUiStretchButton> buttonPtr;
		Vec2 nextPos;

		RouteElementStu() :buttonPtr(nullptr)
		{}
	};
public:
	CHmsRouteElementsList();
	~CHmsRouteElementsList();

    static CHmsRouteElementsList* Create(Size windowSize, float bottomToolHeight);
    virtual bool Init(const HmsAviPf::Size & windowSize, float bottomToolHeight);
	void UpdateListItems();
	void SetWptNodeData(const std::vector<FPL2DNodeInfoStu> &vec);
	void SetWptNodeData(const HistoryRouteStu &historyStu);
	void ClearAll();
	void SetEnableEvent(bool b);
	void SetToolBtnVisible(bool b);

protected:
	virtual bool OnPressed(const Vec2 & posOrigin);

	virtual void OnMove(const Vec2 & posOrigin);

	virtual void OnReleased(const Vec2 & posOrigin);

private:
	inline HmsAviPf::Color3B GetButtonColorByRouteType(WptPointType t);

	void LayoutRouteElement(const Vec2 & posOrigin);

	void CaculateNextPosition();

	void SetRouteElementsToNextPosition(int beginIndex);

	void DeleteRouteElement(int index, RouteElementStu &stu);
	void InsertRouteElement(int index, RouteElementStu &stu);

	void OnRouteNodeClick(CHmsUiButtonAbstract *pBtn, int index);

    void UpdateContainerPosition(float deltaY);
    void UpdateScrollbar();
private:
	std::vector<RouteElementStu> m_vecRouteElements;
	const float c_buttonHeight = 70;
	const float c_margin = 5;

    RefPtr<CHmsScratchPadClipping> m_pScissorNode;
    RefPtr<CHmsUiPanel> m_pItemsContainer;
    RefPtr<CHmsStretchImageNode> m_pScrollbar;
    bool m_bPressOnScrollbar;
    float m_containerYPressOn;

	CHmsTime m_time;
	Vec2 m_posPressStart;
	Vec2 m_posButtonPressStart;

	int m_curDragIndex;
	RouteElementStu m_curDragStu;
	bool m_bIsDragOut;
	const int c_maxLocalZ = 500;
 
	RefPtr<CHmsUiStretchButton> m_pGarbageButton;
	RefPtr<CHmsUiStretchButton> m_pReverseButton;
	RefPtr<CHmsUiStretchButton> m_pClearButton;
	RefPtr<CHmsUiStretchButton> m_pClearAllButton;

	bool m_bEnableEvent;
};
