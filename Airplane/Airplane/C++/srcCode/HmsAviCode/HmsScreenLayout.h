#pragma once
#include "HmsGlobal.h"

enum  class Hms_Screen_Layout_Type
{
	Layout_Small,		//最小
	Layout_Middle,		//半屏
	Layout_Normal,		//全屏
	Layout_Large,		//跨屏3/2屏幕
	Layout_Menu,		//菜单 全屏，上部分
	Layout_Menu_Full,	//菜单 完整菜单
    Layout_Full
};

class CHmsScreenLayout : public HmsAviPf::CHmsNode
{
public:
	CHmsScreenLayout();
	~CHmsScreenLayout();

	CREATE_FUNC(CHmsScreenLayout);

	virtual bool Init() override;

	//设置布局格式
	void SetLayoutType(Hms_Screen_Layout_Type eType);
	Hms_Screen_Layout_Type GetLayoutType(){ return m_eScreenLayoutType; }

	HmsAviPf::Size GetScreenLayoutSize();
	HmsAviPf::Size GetScreenLayoutSize(Hms_Screen_Layout_Type eType);
	HmsAviPf::Size GetSize();
	HmsAviPf::Vec2 GetTopCenter(); 
	HmsAviPf::Vec2 GetTopLeft();
	HmsAviPf::Vec2 GetTopRight();
	HmsAviPf::Vec2 GetBottomRight();
	HmsAviPf::Vec2 GetBottomLeft();
	HmsAviPf::Vec2 GetBottomCenter();
	HmsAviPf::Vec2 GetCenter();
	HmsAviPf::Vec2 GetCenterLeft();
	HmsAviPf::Vec2 GetCenterRight();
	HmsAviPf::Rect GetRect();

	HmsAviPf::CHmsNode * GetCentreNode(){ return m_pNodeCentre; }

	//更新显示
	virtual void ResetViewer(){ ForceUpdateAllChild(); }
	virtual void OnLayoutBtnChange(int nLayoutBtnIndex, int nBtnState){ ; }

	HmsAviPf::CHmsLabel * GetPageLabel();

	virtual void SetPosition(float x, float y) override;
	virtual void SetPosition(const HmsAviPf::Vec2 & pos) override;
	virtual void SetPositionX(float x) override;

	bool DispatchScreenButtonChange(int nBtnIndex, int nBtnState);

protected:
	void InitLayoutType(Hms_Screen_Layout_Type eType);

	bool TouchInLayout(HmsAviPf::Touch * touch);

	//bool MouseInLayout(HmsAviPf::CHmsEventMouse * mouse);

	virtual bool onTouchBegan(HmsAviPf::Touch* touch, HmsAviPf::CHmsEvent* event);
	virtual void onTouchMoved(HmsAviPf::Touch* touch, HmsAviPf::CHmsEvent* event){}
	virtual void onTouchEnded(HmsAviPf::Touch* touch, HmsAviPf::CHmsEvent* event){}

	void SetRecvTouchEvent();

	HmsAviPf::CHmsLabel * CreateOrSetPageLabel(const char * strTitle);

	void ChangePageLabelByLayoutType(Hms_Screen_Layout_Type tType);

	void UpdateScreenPosIndex();

	CHmsNode * GetLayoutBg();

public:
	virtual void OnScreenLayoutChanged(Hms_Screen_Layout_Type eOld, Hms_Screen_Layout_Type eNew){};

private:
	Hms_Screen_Layout_Type			m_eScreenLayoutType;	//屏幕显示类型
	HmsAviPf::Size					m_sizeLayout;
	HmsAviPf::CHmsNode	*			m_pNodeCentre;
	int								m_nScreenPosIndex;		//0:left, 1:left mid, 2:right mid, 3:right
};

