#include "HmsUiButtonsManger.h"

NS_HMS_BEGIN

CHmsUiButtonsManger::CHmsUiButtonsManger()
{
	m_pBtnsMgr = nullptr;
}

CHmsUiButtonsManger::~CHmsUiButtonsManger()
{
}

void CHmsUiButtonsManger::AddBtn(int nIndex, CHmsUiButtonAbstract * pBtnAbs)
{
	if (nullptr == m_pBtnsMgr)
	{
		m_pBtnsMgr = &m_mapGroup[nullptr];
	}

	if (m_pBtnsMgr)
	{
		auto & mgr = *m_pBtnsMgr;
		mgr[nIndex] = pBtnAbs;
	}
}

void CHmsUiButtonsManger::AddBtn(CHmsUiButtonAbstract * pBtnAbs)
{
	int nIndex = 0;
	if (m_pBtnsMgr)
	{
		nIndex = m_pBtnsMgr->size();
	}
	AddBtn(nIndex, pBtnAbs);
}

void CHmsUiButtonsManger::OnPress(int nIndex)
{
	auto btn = GetIndexBtn(nIndex);
	if (btn)
	{
		btn->Selected();
	}
}

void CHmsUiButtonsManger::OnRelease(int nIndex)
{
	auto btn = GetIndexBtn(nIndex);
	if (btn)
	{
		btn->UnSelected();
	}
}

CHmsUiButtonAbstract * CHmsUiButtonsManger::GetIndexBtn(int nIndex)
{
	CHmsUiButtonAbstract * pBut = nullptr;
	if (m_pBtnsMgr)
	{
		auto itFind = m_pBtnsMgr->find(nIndex);
		if (itFind != m_pBtnsMgr->end())
		{
			pBut = itFind->second;
		}
	}
	return pBut;
}

void CHmsUiButtonsManger::OnBtnChange(int nIndex, int nBtnState)
{
	if (0 == nBtnState)
		OnRelease(nIndex);
	else
		OnPress(nIndex);
}

void CHmsUiButtonsManger::CreateOrSelectGroup(CHmsNode * node)
{
	m_pBtnsMgr = &m_mapGroup[node];
}

void CHmsUiButtonsManger::CreateOrSelectGroup(CHmsNode * node, bool bShow)
{
	m_pBtnsMgr = &m_mapGroup[node];
	SetShow(bShow);
}

void CHmsUiButtonsManger::SetShow(bool bShow)
{
	if (m_pBtnsMgr)
	{
		for (auto & c : *m_pBtnsMgr)
		{
			if (c.second)
			{
				c.second->GetNode()->SetVisible(bShow);
				if (bShow)
				{
					c.second->GetNode()->ForceUpdateAllChild();
				}
			}
		}
	}

}



NS_HMS_END


