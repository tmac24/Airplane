#pragma once
#include "HmsUiButtonAbstract.h"
#include <map>
NS_HMS_BEGIN

typedef std::map<int, CHmsUiButtonAbstract*> MapBtnMgr;

class CHmsUiButtonsManger
{
public:
	CHmsUiButtonsManger();
	~CHmsUiButtonsManger();

	void CreateOrSelectGroup(CHmsNode * node);
	void CreateOrSelectGroup(CHmsNode * node, bool bShow);

	void OnBtnChange(int nIndex, int nBtnState);
	void OnPress(int nIndex);
	void OnRelease(int nIndex);

	void AddBtn(int nIndex, CHmsUiButtonAbstract * pBtnAbs);
	void AddBtn(CHmsUiButtonAbstract * pBtnAbs);
	CHmsUiButtonAbstract * GetIndexBtn(int nIndex);

	void SetShow(bool bShow);

protected:
	std::map<CHmsNode*, MapBtnMgr>				m_mapGroup;
	std::map<int, CHmsUiButtonAbstract*>	*	m_pBtnsMgr;
};

NS_HMS_END