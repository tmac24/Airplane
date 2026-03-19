#pragma once
#include "../HmsGlobal.h"
#include "base/HmsTimer.h"
#include "display/HmsDrawNode.h"
#include "ui/HmsUiEventInterface.h"

class CHmsHSIBox : public HmsAviPf::CHmsNode, public HmsAviPf::HmsUiEventInterface
{
public:
	typedef std::function<void(int)> OnValueChangedCallback;
	CHmsHSIBox();
	~CHmsHSIBox();

	CREATE_FUNC(CHmsHSIBox);

	virtual bool Init() override;

	void SetValue(const int &iValue);

	int GetValue();

	void SetTitle(const std::string &title);

	virtual bool OnPressed(const HmsAviPf::Vec2 & posOrigin) override;
	virtual void OnReleased(const HmsAviPf::Vec2 & posOrigin) override;

	virtual HmsAviPf::Rect GetRectByOrigin();

	OnValueChangedCallback OnValueChanged;
private:
	float m_boxWidth;
	float m_boxHeight;
	HmsAviPf::Color4B m_boxColor;
	HmsAviPf::Color4B m_defaultColor;//默认边框颜色
	HmsAviPf::Color4B m_slectedColor;//选中时的边框颜色
	bool m_selected;

	HmsAviPf::RefPtr<HmsAviPf::CHmsDrawNode>	m_pBoxNode;
	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>		m_pValue;
	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>		m_pTitle;
	HmsAviPf::RefPtr<HmsAviPf::CHmsDrawNode>	m_pMinusNode;
	HmsAviPf::RefPtr<HmsAviPf::CHmsDrawNode>	m_pPlusNode;

	CHmsTime							m_timeLast;

	int									m_value;
	std::string							m_strTitle;

	void SelectedBox(bool sel);
};

