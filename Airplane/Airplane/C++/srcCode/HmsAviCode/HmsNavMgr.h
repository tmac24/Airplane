#pragma once
#include "base/HmsMap.h"
#include "base/HmsNode.h"
#include "HmsLayerNav.h"
#include "ui/HmsUiButtonsManger.h"

enum class HmsNavType
{
	NavNone,
	NavMap,						//地图导航页面
	NavAirport,					//机场导航主页
	NavAirportProgram,			//机场导航程序页
    NavAirportProgramPreview
};

class CHmsNavMgr
{
public:
	CHmsNavMgr();
	~CHmsNavMgr();

	static CHmsNavMgr * GetInstance();

	void Init(const HmsAviPf::Size & size);

	//nLevel 1=1/4Screen  2=1/2Screen   3=3/4Screen 
	void Resize(const HmsAviPf::Size & size, int nLevel = 2);

	void SetNavLayer(CHmsLayerNav * nav);
	HmsAviPf::CHmsNode * ShowNavType(HmsNavType eType);

	HmsAviPf::CHmsNode * GetNavType(HmsNavType eType);

    void ShowLastNavType();

	HmsAviPf::CHmsUiButtonsManger * GetBtnMgr(){ return &m_btnMgr; }

protected:
	
	void AddNavType(HmsNavType eType, HmsAviPf::CHmsNode * node);

private:
	HmsNavType											m_eType;
	HmsAviPf::Map<int, HmsAviPf::CHmsNode*>		m_mapNavNode;
	HmsAviPf::RefPtr<CHmsLayerNav>						m_navLayer;
	HmsAviPf::CHmsUiButtonsManger							m_btnMgr;

    HmsNavType m_lastNavType;
};