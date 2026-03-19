#pragma once
/*#include <HmsAviPf.h>*/
#include <HmsAviPf.h>
#include <map>
#include "HmsScreenAvionics.h"

class CHmsScreenMgr
{
public:
	CHmsScreenMgr();
	~CHmsScreenMgr();

	//get the scene for the avionics
	HmsAviPf::CHmsScreen * GetSceneAvionics();

	//get the welcome scene
	HmsAviPf::CHmsScreen * GetSceneWelcome(const std::function<void()> & callback);

	//get the welcome scene which has the NaveInfo
	HmsAviPf::CHmsScreen * GetSceneNavInfo();

	CHmsScreenAvionics * GetScreenAvionics(){ return m_pScreenAvionics; }

private:
	HmsAviPf::CHmsScreen			* m_pScene;			//the scene for avionics
	HmsAviPf::CHmsScreen			* m_pSceneWelcome;	//the scene for welcome
	HmsAviPf::CHmsScreen			* m_pSceneNavInfo;

	CHmsScreenAvionics				* m_pScreenAvionics;
};

