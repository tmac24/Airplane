#pragma once
#include "HmsUiButton.h"
#include "display/HmsImageNode.h"

NS_HMS_BEGIN

class CHmsUiImage2StateButton : public CHmsImageNode, public HmsUiEventInterface
{
public:
	CHmsUiImage2StateButton();
	~CHmsUiImage2StateButton();

	typedef std::function<void(CHmsUiImage2StateButton*, HMS_UI_BUTTON_STATE)> HmsUiBtnCallback;

	static CHmsUiImage2StateButton* CreateWithImage(const char * strImageNormal, const char * strImageSelected, const HmsUiBtnCallback& callback = nullptr);

	void SetCallbackFunc(const HmsUiBtnCallback & callback);

	/**
	*Toggle button is the button that toggle state Press once 
	*param   
	*returns 
	*by [11/24/2017 song.zhang] 
	*/
	void SetToggleBtn(bool bToggle);

	void SetSelected(bool bSelected);

protected:
	virtual bool Init() override;
	bool InitWithTexture(Texture2D * textureNormal, Texture2D * textureSelected);

	void SetState(HMS_UI_BUTTON_STATE eState);

public:
	void AddText(const char * strText, float size = 12.0f, const Color4B &color = Color4B::WHITE, bool bBorder = false);

	void AddLabel(CHmsLabel *label);

	std::string GetText();

	void SetText(const char * strText);


	void SetTextColor(const Color4B &color);

	CHmsLabel * GetLabel();

	void Selected();

	void UnSelected();

	bool IsSelected();

	virtual bool OnPressed(const Vec2 & posOrigin);

	virtual void OnMove(const Vec2 & posOrigin);

	virtual void OnReleased(const Vec2 & posOrigin);

	virtual void EnableUi(bool bEnable);

protected:
	HMS_UI_BUTTON_STATE				m_eBtnState;
	HmsUiBtnCallback				m_funcCallback;
	bool							m_bToggleBtn;

	RefPtr<Texture2D>				m_textureNormal;
	RefPtr<Texture2D>				m_textureSelected;
};

NS_HMS_END