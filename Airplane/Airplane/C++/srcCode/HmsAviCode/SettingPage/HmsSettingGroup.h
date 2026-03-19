#pragma once

#include "ui/HmsUiImageButton.h"
#include "ui/HmsUiStretchButton.h"
#include "ui/HmsUiImagePanel.h"
#include "display/HmsDrawNode.h"
#include "Language/HmsLanguageLabel.h"

class CHmsSettingGroup : public HmsAviPf::CHmsNode, public HmsAviPf::HmsUiEventInterface
{
public:
	CHmsSettingGroup();
	~CHmsSettingGroup();

	static CHmsSettingGroup * Create(const std::string & strGroupName, float fWidth);

	virtual bool Init(const std::string & strGroupName, float fWidth);

	HmsAviPf::CHmsLabel * AddItem(const std::string & strItemName, HmsAviPf::CHmsNode * nodeBtn);
	void AddRadioItem(const std::string & name1, const std::string & name2, bool bSelect1, const std::function<void(int)> & cb);

private:
	void AddAndUpdateNodeBtns(HmsAviPf::CHmsNode * nodeBtn);
	void AddAndUpdateRadioBtns(HmsAviPf::CHmsNode * node1, HmsAviPf::CHmsNode * node2);

private:
	HmsAviPf::RefPtr<HmsAviPf::CHmsDrawNode>				m_nodeDraw;
	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>					m_labelTitle;
	float													m_fWidth;
	int														m_nItemCnt;
	HmsAviPf::Vector<HmsAviPf::CHmsNode*>					m_vNodeBtn;
};

