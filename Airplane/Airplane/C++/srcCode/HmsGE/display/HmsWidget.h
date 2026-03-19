#pragma once
#include "base/HmsNode.h"
#include "base/RefPtr.h"

NS_HMS_BEGIN

class TouchInterface
{
public:
	TouchInterface(){ m_bPressed = false; }
	/*
	* OnPressed
	* param x in 相对于父节点的x位置
	* param y in 相对于父节点的x位置
	* return
	*by [10/31/2017 zhenhua.wu]
	*/
	virtual bool OnPressed(const Vec2 & posParent){ return false; }
	virtual void OnMove(const Vec2 & posParent){}
	virtual void OnReleased(const Vec2 & posParent){}

protected:
	bool m_bPressed;

	friend class CHmsWidget;
};

class CHmsDrawNode;

class CHmsWidget : public CHmsNode, TouchInterface
{
public:
	typedef std::function<void(CHmsWidget*)> onClickCallback;

	CHmsWidget();
	~CHmsWidget();

	CREATE_FUNC(CHmsWidget);

	virtual bool Init() override;

	virtual void RegisterTouchEvent();

	//给当前控件设置焦点
	virtual void Focus();

	//取消当前控件焦点
	virtual void Blur();

	bool HasFocus();

	virtual void SetEnabel(bool enable);

	virtual void SetSize(const Size &size);

	void SetBackgroundColor(const Color4B &color);

	void SetBorderWidth(const float &width);

	void SetBorderColor(const Color4B &color);

	void SetClickedCallback(onClickCallback onClicked);

	// callback
	onClickCallback onClicked;

	virtual void SetContentSize(const Size & size) override;

protected:
	bool m_focus;
	bool m_enabled;
	float	m_borderWidth;//边框宽度
	Color4B m_borderColor;//边框颜色
	Color4B m_backgroundColor;//背景颜色
	RefPtr<CHmsDrawNode>	m_pBackgroundNode;

	virtual void UpdateBackground();

	virtual void EnableChanged(){};
	virtual void FocusChanged(){};
	
};

NS_HMS_END