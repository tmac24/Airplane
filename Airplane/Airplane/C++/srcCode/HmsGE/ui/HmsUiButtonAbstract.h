#pragma once
#include "display/HmsStretchImageNode.h"
#include "display/HmsLabel.h"
#include "base/RefPtr.h"
#include "HmsUiEventInterface.h"

NS_HMS_BEGIN

enum class HMS_UI_BUTTON_STATE
{
	btn_normal,			//the normal state
	btn_selected,		//the selected state
	btn_disable,		//the disable state
};

class CHmsImageNode;

class CHmsUiButtonAbstract : public HmsUiEventInterface
{
public:
	CHmsUiButtonAbstract(CHmsNode * node);
	~CHmsUiButtonAbstract();

	typedef std::function<void(CHmsUiButtonAbstract*, HMS_UI_BUTTON_STATE)> HmsUiBtnCallback;
	typedef std::function<void(CHmsUiButtonAbstract*)> OnClickedCallback;


	void SetCallbackFunc(const HmsUiBtnCallback & callback);

	void SetOnClickedFunc(const OnClickedCallback & callback);

	/**
	*Toggle button is the button that toggle state Press once 
	*param   
	*returns 
	*by [11/24/2017 song.zhang] 
	*/
	void SetToggleBtn(bool bToggle);

protected:
	virtual void SetState(HMS_UI_BUTTON_STATE eState);

	virtual void DispatchCallbackFunc();

public:
	void AddIcon(CHmsImageNode *icon);

	void AddIcon(const std::string & iconPath);

	void AddIcon(const std::string & iconPath, const Size &iconSize);

public:
	virtual void AddText(const char * strText, float size = 12.0f, const Color4B &color = Color4B::WHITE, bool bBorder = false);

	virtual void AddLabel(CHmsLabel *label);

	//attach the label to button state,and not use the button label,but you should attach each state
	void AttachLabelToState(CHmsLabel *label, HMS_UI_BUTTON_STATE eState);

	//attach the Icon to button state,and you should attach each state
	void AttachIconToState(CHmsImageNode *icon, HMS_UI_BUTTON_STATE eState);

	//get the button text,if no have the normal label, get the current state label
    virtual std::string GetText();

	//get the state text
	std::string GetTextByState(HMS_UI_BUTTON_STATE eState);

	//set the text for normal label
	void SetText(const char * strText);

	//set the text for particular state, if no have the state, set the normal label 
	void SetTextToState(const char * strText, HMS_UI_BUTTON_STATE eState);

	void SetTextColor(const Color4B &color);

	void SetTextColorToState(const Color4B &color, HMS_UI_BUTTON_STATE eState);

public:
	//get the state node, if the button has different state node
	virtual CHmsNode * GetStateNode(HMS_UI_BUTTON_STATE eState);
	void RegisterStateNode(CHmsNode * node, HMS_UI_BUTTON_STATE eState);

	//get the label, if the button have a label
	CHmsLabel * GetLabel();

	//get the label by state, if the button has different state node
	CHmsLabel * GetLabelByState(HMS_UI_BUTTON_STATE eState);

	//get the icon node, if the button have a icon node
	CHmsImageNode * GetIconNode();
	
	virtual void SetSelected(bool isSelected);

	virtual void Selected();

	virtual void UnSelected();

	virtual bool IsSelected();

	virtual bool OnPressed(const Vec2 & posOrigin);

	virtual void OnMove(const Vec2 & posOrigin);

	virtual void OnReleased(const Vec2 & posOrigin);

	virtual void EnableUi(bool bEnable);

    virtual void SetEnableCallback(bool b);

protected:

	HMS_UI_BUTTON_STATE				m_eBtnState;
	HmsUiBtnCallback				m_funcCallback;
	OnClickedCallback				m_onClickedCallback;
	bool							m_bToggleBtn;
    bool                            m_bEnableCallback;
};

NS_HMS_END