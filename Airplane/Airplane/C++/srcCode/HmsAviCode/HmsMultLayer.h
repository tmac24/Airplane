#pragma once
#include "HmsScreenLayout.h"

class CHmsMultLayer : public CHmsScreenLayout
{
public:
	CHmsMultLayer();
	~CHmsMultLayer();

	virtual bool init(Hms_Screen_Layout_Type eType);

	static CHmsMultLayer * create(Hms_Screen_Layout_Type eType);

	void AddPage(CHmsScreenLayout * pLayout);

	void SortPageAndShowIndex(int nIndex = 0);

	void OnSelectedTagNode(HmsAviPf::CHmsNode * tag);

protected:
	HmsAviPf::CHmsNode * CreateTag(CHmsScreenLayout * pLayout);
	void SetRecvTouchEvent();

private:
	HmsAviPf::Vector<CHmsScreenLayout*>				m_vPageInfo;
	HmsAviPf::Vector<HmsAviPf::CHmsNode*>					m_vPageTag;
	HmsAviPf::RefPtr<HmsAviPf::CHmsImageNode>				m_curPushTag;
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>					m_curUsedPageInfo;
};

