#pragma once
#include "base/HmsNode.h"
#include "ui/HmsUiButtonAbstract.h"

class CHmsToolBarCombItem : public HmsAviPf::CHmsNode, public HmsAviPf::CHmsUiButtonAbstract
{
public:
	CHmsToolBarCombItem();
	~CHmsToolBarCombItem();

	static CHmsToolBarCombItem* CreateWithImage(const char * strImagePath, const char * strTitle, const HmsAviPf::Size & size, const char * strBgPath = nullptr);
	static CHmsToolBarCombItem* CreateWithString(const char * strTitle, const HmsAviPf::Size & size, const char * strBgPath = nullptr);
	
	virtual bool Init(const char * strImagePath, const char * strTitle, const HmsAviPf::Size & size, const char * strBgPath = nullptr);
	virtual bool Init(const char * strTitle, const HmsAviPf::Size & size, const char * strBgPath = nullptr);

	virtual void SetState(HmsAviPf::HMS_UI_BUTTON_STATE eState) override;

	virtual bool OnPressed(const HmsAviPf::Vec2 & posOrigin) override;

	virtual void Selected() override;

	void SetClick2Normal(bool bOn){ m_bClick2Normal = bOn; }

	void SetText(const char * strText);

	std::string GetText(){ return m_strText; }

	void SetBgOpacity(GLubyte opacity);

protected:
	void SetButtonColor(const HmsAviPf::Color3B & color);

private:
	HmsAviPf::RefPtr<HmsAviPf::CHmsStretchImageNode>			m_bgNode;
	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>						m_labelText;
	std::string													m_strText;
	bool														m_bClick2Normal;
};