#pragma once
#include "HmsUiButtonAbstract.h"
#include <map>
NS_HMS_BEGIN

class UiButtonNode : public HmsAviPf::Ref
{
private:
	std::string								m_strButton;
	HmsAviPf::Vector<UiButtonNode*>			m_vChildButtons;
};

class CHmsUiButtonsTree
{
public:
	CHmsUiButtonsTree();
	~CHmsUiButtonsTree();

	void CreateMode(const std::string & strModeName);

	void CreateOrSelectGroup(CHmsNode * node);
	void CreateOrSelectGroup(CHmsNode * node, bool bShow);

	void AddBtn(const std::string & strBtn, CHmsUiButtonAbstract * pBtnAbs);
	CHmsUiButtonAbstract * GetBtn(const std::string & strBtn);

protected:
	std::map<std::string, CHmsUiButtonAbstract*>	*	m_pBtnsMgr;
};

NS_HMS_END