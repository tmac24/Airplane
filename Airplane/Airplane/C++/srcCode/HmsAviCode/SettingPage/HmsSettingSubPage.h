#pragma once
#include <HmsAviPf.h>
#include "ui/HmsUiEventInterface.h"
#include "ui/HmsUiImagePanel.h"
#include "ui/HmsUiVerticalScrollView.h"
#include "ui/HmsUiButtonAbstract.h"
#include "ui/HmsUiImage2StateButton.h"
#include "../HmsParallelLayer.h"
#include "ui/HmsUiRadio.h"

typedef std::function<void(const std::string &, int)>	FuncSettingPageCallBack;

struct SettingItemContent
{
	std::string												strName;
	int														nParam;
	bool													bSelected;
};

class CHmsSettingSubPage : public HmsAviPf::CHmsNode, public HmsAviPf::HmsUiEventInterface
{
public:
	CHmsSettingSubPage();
	~CHmsSettingSubPage();

	static CHmsSettingSubPage * Create(const HmsAviPf::Size & size);

	virtual bool init(const HmsAviPf::Size & size);

	void SetSubPageTitle(const std::string & title);

	void SetContent(const std::vector<SettingItemContent> & vItemContent);
	void SetContent(SettingItemContent * pItems, int nArrayCnt);

	void SetParallelLayer(CHmsParallelLayer * layer){ m_layerParallel = layer; };
	void SetItemMutex(bool bMutex){ m_bMutex = bMutex; }

	void SetClickCallback(const FuncSettingPageCallBack & callback){ m_callback = callback; }

	virtual bool OnPressed(const HmsAviPf::Vec2 & posOrigin) override;

protected:
	void OnRadioClick(HmsAviPf::CHmsUiButtonAbstract *sender, HmsAviPf::HMS_UI_BUTTON_STATE state, const std::string & strName, int nParam);
	virtual void AddContentNode(HmsAviPf::CHmsNode * node);
	virtual void RemoveAllContentNode();

protected:
	HmsAviPf::RefPtr<HmsAviPf::CHmsDrawNode>					m_nodeDraw;
	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>						m_labelTitle;
	float													m_fContentBegin;
	int														m_nItemCnt;
	HmsAviPf::RefPtr<CHmsParallelLayer>						m_layerParallel;
	bool													m_bMutex;
	HmsAviPf::Map<std::string, HmsAviPf::CHmsUiRadio*>		m_mapRadioGroup;
	HmsAviPf::Vector<HmsAviPf::CHmsNode *>					m_vContectNode;
	FuncSettingPageCallBack									m_callback;
};
