#pragma once
#include "base/HmsNode.h"
#include "base/RefPtr.h"

NS_HMS_BEGIN

typedef std::function<void(CHmsNode*)> HmsUiCallback;

class CHmsLabel;
class Touch;

struct UiEventPoint
{
	int		Id;
	Touch *	touch;
	Vec2	uiPoint;
	bool	bActive;

	UiEventPoint()
	{
		Id = 0;
		touch = nullptr;
		bActive = false;
	}
};

enum class HMS_UI_EVENT_TYPE
{
	none_ui_event,
	single_ui_event,
	multi_ui_event,
};

class HmsUiEventInterface
{
public:
	HmsUiEventInterface(CHmsNode * node);
	virtual ~HmsUiEventInterface();
	/*
	* OnPressed
	* param x in 相对于父节点的x位置
	* param y in 相对于父节点的x位置
	* return
	*by [10/31/2017 zhenhua.wu]
	*/
	virtual bool OnPressed(const Vec2 & posOrigin);
	virtual void OnMove(const Vec2 & posOrigin);
	virtual void OnReleased(const Vec2 & posOrigin);
	virtual void OnCancel(const Vec2 & posOrigin);
	virtual void OnClick(){};

	virtual bool OnMutiPressed(const std::vector<UiEventPoint> &pos);
	virtual void OnMutiMove(const std::vector<UiEventPoint> &pos);
	virtual void OnMutiReleased(const std::vector<UiEventPoint> &pos);
	virtual void OnMutiEnd();

	virtual void EnableUi(bool bEnable);

	virtual void RegisterTouchEvent(int nEventOrder = 0);

	virtual void RegisterMutiTouchEvent(int nEventOrder = 0);

	int RegisterAllNodeChild();

	void AddChildInterface(HmsUiEventInterface * child);
	void RemoveChildInterface(HmsUiEventInterface * child);
	void RemoveAllChildInterface();
	void RemoveFromParentInterface();

	//获取锚点相对于父节点所在的位置
	Vec2 GetPositionAr();

	//获取绘图原点相对于父节点的位置
	Vec2 GetPositionOrigin();

	CHmsLabel * CreateLabel(const char * strText, float fSize, bool border = false);

	CHmsNode * GetNode(){ return m_node.get(); }

	Vec2 GetUiTopCenter();
	Vec2 GetUiTopLeft();
	Vec2 GetUiTopRight();
	Vec2 GetUiBottomRight();
	Vec2 GetUiBottomLeft();
	Vec2 GetUiBottomCenter();
	Vec2 GetUiCenter();
	Vec2 GetUiCenterLeft();
	Vec2 GetUiCenterRight();

protected:
	/**
	*
	*param   
	*returns 
	*by [11/23/2017 song.zhang] 
	*/
	bool IsActive();
	Vec2 GetUiArPostion(const Vec2 & posParent, HmsUiEventInterface * pNode);
	Vec2 GetUiOriginPostion(const Vec2 & posParent, HmsUiEventInterface * pNode);

	void ShortChildInterface();

	void SetParentUiInterface(HmsUiEventInterface * parent){ m_pParentUiInterface = parent; }

	static bool UiEventComparisonGreater(HmsUiEventInterface* ei1, HmsUiEventInterface* ei2);

	Vec2 Touch2UiPosition(Touch * touch);

	virtual Rect GetRectByOriginUI();
    virtual Rect GetRectByOriginUI(CHmsNode * node);

private:
	void FillMultiTouch(const std::vector<Touch*> touchs);
	//return true if touch is updata and check
	bool FillMultiTouchByCheck(const std::vector<Touch*> touchs);
	std::vector<UiEventPoint> FillMultiTouchAndReturnNewFill(const std::vector<Touch*> touchs);

	UiEventPoint * GetFilledUiEventPoint(int nID);
	void RemoveFilledUiEventPoint(int nID);

protected:
	bool								m_bDisable;	//you can disable it
	bool								m_bReorderNeeded;
	RefPtr<CHmsNode>					m_node;		//the node of current node
	std::vector<HmsUiEventInterface*>	m_vUiEventInterface;
	HmsUiEventInterface *				m_pParentUiInterface;
	std::vector<UiEventPoint>	*		m_pVectorMutiPoint;
	HMS_UI_EVENT_TYPE					m_eUiEventType;
};

NS_HMS_END