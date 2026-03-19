#include "HmsAviAudioEngine.h"
//#include "HmsAudio/include/SimpleAudioEngine.h"

//using namespace CocosDenshion;

CHmsAviAudioEngine * CHmsAviAudioEngine::s_pInstance = nullptr;

CHmsAviAudioEngine* CHmsAviAudioEngine::getInstance()
{
	if (nullptr == s_pInstance)
	{
		s_pInstance = new CHmsAviAudioEngine();
		if (s_pInstance)
		{
			s_pInstance->PreloadAudio();
		}		
	}
	return s_pInstance;
}

CHmsAviAudioEngine::~CHmsAviAudioEngine()
{

}

void CHmsAviAudioEngine::PreloadAudio()
{
	Init();
	PreloadCurrentUsedAudio();
}

void CHmsAviAudioEngine::PlayBgMusic()
{
	if (m_bActive)
	{
		//SimpleAudioEngine::getInstance()->playBackgroundMusic("AviSound/Test/Bandari.mp3", true);
	}	
}

void CHmsAviAudioEngine::PlayEffectByName(std::string name, bool bLoop)
{
	if (m_bActive)
	{
		auto itFind = m_mapEffectAudio.find(name);
		if (itFind != m_mapEffectAudio.end())
		{
//			SimpleAudioEngine::getInstance()->playEffect(itFind->second.c_str(), bLoop);
		}
	}
}

void CHmsAviAudioEngine::playEffectByPath(std::string strEffectPath, bool bLoop)
{
	if (m_bActive)
	{
//		SimpleAudioEngine::getInstance()->playEffect(strEffectPath.c_str(), bLoop);
	}
}

void CHmsAviAudioEngine::PlayTerrainTerrain()
{
	PlayEffectByName("terrainTerrain", true);
}

void CHmsAviAudioEngine::SetMusicVol(float vol)
{
//	SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(vol);
}

void CHmsAviAudioEngine::SetEffectVol(float vol)
{
//	SimpleAudioEngine::getInstance()->setEffectsVolume(vol);

}

void CHmsAviAudioEngine::StopAllSound()
{
//	SimpleAudioEngine::getInstance()->stopAllEffects();
//	SimpleAudioEngine::getInstance()->stopBackgroundMusic();
}

void CHmsAviAudioEngine::SetActive(bool bActive)
{
	m_bActive = bActive;
	if (!bActive)
	{
		StopAllSound();
	}
}

void CHmsAviAudioEngine::PreloadAllAudio()
{
//	SimpleAudioEngine::getInstance()->preloadEffect("AviSound/EGPWS/10.wav");
//	SimpleAudioEngine::getInstance()->preloadEffect("AviSound/EGPWS/100.wav");
//	SimpleAudioEngine::getInstance()->preloadEffect("AviSound/EGPWS/1000.wav");
//	SimpleAudioEngine::getInstance()->preloadEffect("AviSound/EGPWS/20.wav");
//	SimpleAudioEngine::getInstance()->preloadEffect("AviSound/EGPWS/200.wav");
//	SimpleAudioEngine::getInstance()->preloadEffect("AviSound/EGPWS/2500.wav");
//	SimpleAudioEngine::getInstance()->preloadEffect("AviSound/EGPWS/30.wav");
//	SimpleAudioEngine::getInstance()->preloadEffect("AviSound/EGPWS/300.wav");
//	SimpleAudioEngine::getInstance()->preloadEffect("AviSound/EGPWS/40.wav");
//	SimpleAudioEngine::getInstance()->preloadEffect("AviSound/EGPWS/400.wav");
//	SimpleAudioEngine::getInstance()->preloadEffect("AviSound/EGPWS/50.wav");
//	SimpleAudioEngine::getInstance()->preloadEffect("AviSound/EGPWS/500.wav");
//	SimpleAudioEngine::getInstance()->preloadEffect("AviSound/EGPWS/Approach minimums.wav");
//	SimpleAudioEngine::getInstance()->preloadEffect("AviSound/EGPWS/bank angle.wav");
//	SimpleAudioEngine::getInstance()->preloadEffect("AviSound/EGPWS/dontsink.wav");
//	SimpleAudioEngine::getInstance()->preloadEffect("AviSound/EGPWS/glideslope.wav");
//	SimpleAudioEngine::getInstance()->preloadEffect("AviSound/EGPWS/minimums.wav");
//	SimpleAudioEngine::getInstance()->preloadEffect("AviSound/EGPWS/pullup.wav");
//	SimpleAudioEngine::getInstance()->preloadEffect("AviSound/EGPWS/sinkrate.wav");
//	SimpleAudioEngine::getInstance()->preloadEffect("AviSound/EGPWS/terrain.wav");
//	SimpleAudioEngine::getInstance()->preloadEffect("AviSound/EGPWS/terrainTerrain.wav");
//	SimpleAudioEngine::getInstance()->preloadEffect("AviSound/EGPWS/toolowflaps.wav");
//	SimpleAudioEngine::getInstance()->preloadEffect("AviSound/EGPWS/toolowgear.wav");
//	SimpleAudioEngine::getInstance()->preloadEffect("AviSound/EGPWS/toolowterrain.wav");
//	SimpleAudioEngine::getInstance()->preloadEffect("AviSound/EGPWS/windshear.wav");
//	SimpleAudioEngine::getInstance()->preloadEffect("AviSound/TCAS/Adjust vertical speed,adjust.wav");
//	SimpleAudioEngine::getInstance()->preloadEffect("AviSound/TCAS/Clear of conflict.wav");
//	SimpleAudioEngine::getInstance()->preloadEffect("AviSound/TCAS/Climb,climb now.wav");
//	SimpleAudioEngine::getInstance()->preloadEffect("AviSound/TCAS/Climb,Crossing,climb.wav");
//	SimpleAudioEngine::getInstance()->preloadEffect("AviSound/TCAS/climb.wav");
//	SimpleAudioEngine::getInstance()->preloadEffect("AviSound/TCAS/descend,Crossing,descend.wav");
//	SimpleAudioEngine::getInstance()->preloadEffect("AviSound/TCAS/descend.wav");
//	SimpleAudioEngine::getInstance()->preloadEffect("AviSound/TCAS/Descend; descend now.wav");
//	SimpleAudioEngine::getInstance()->preloadEffect("AviSound/TCAS/Increase climb.wav");
//	SimpleAudioEngine::getInstance()->preloadEffect("AviSound/TCAS/Increase descent.wav");
//	SimpleAudioEngine::getInstance()->preloadEffect("AviSound/TCAS/Maintain vertical speed,Crossing,maintain.wav");
//	SimpleAudioEngine::getInstance()->preloadEffect("AviSound/TCAS/Maintain vertical speed,maintain.wav");
//	SimpleAudioEngine::getInstance()->preloadEffect("AviSound/TCAS/Monitor vertical speed.wav");
//	SimpleAudioEngine::getInstance()->preloadEffect("AviSound/TCAS/TCAS system test fail.wav");
//	SimpleAudioEngine::getInstance()->preloadEffect("AviSound/TCAS/TCAS system test OK.wav");
//	SimpleAudioEngine::getInstance()->preloadEffect("AviSound/TCAS/Traffic Traffic.wav");
}

void CHmsAviAudioEngine::PreloadCurrentUsedAudio()
{
//	SimpleAudioEngine::getInstance()->preloadEffect("AviSound/EGPWS/minimums.wav");
//	SimpleAudioEngine::getInstance()->preloadEffect("AviSound/EGPWS/pullup.wav");
//	SimpleAudioEngine::getInstance()->preloadEffect("AviSound/EGPWS/sinkrate.wav");
//	SimpleAudioEngine::getInstance()->preloadEffect("AviSound/EGPWS/terrain.wav");
//	SimpleAudioEngine::getInstance()->preloadEffect("AviSound/EGPWS/terrainTerrain.wav");
//	SimpleAudioEngine::getInstance()->preloadEffect("AviSound/EGPWS/toolowterrain.wav");
}

void CHmsAviAudioEngine::Init()
{
//	m_mapEffectAudio["100"] = ("AviSound/EGPWS/10.wav");
//	m_mapEffectAudio["100"] = ("AviSound/EGPWS/100.wav");
//	m_mapEffectAudio["1000"] = ("AviSound/EGPWS/1000.wav");
//	m_mapEffectAudio["20"] = ("AviSound/EGPWS/20.wav");
//	m_mapEffectAudio["200"] = ("AviSound/EGPWS/200.wav");
//	m_mapEffectAudio["2500"] = ("AviSound/EGPWS/2500.wav");
//	m_mapEffectAudio["30"] = ("AviSound/EGPWS/30.wav");
//	m_mapEffectAudio["300"] = ("AviSound/EGPWS/300.wav");
//	m_mapEffectAudio["40"] = ("AviSound/EGPWS/40.wav");
//	m_mapEffectAudio["400"] = ("AviSound/EGPWS/400.wav");
//	m_mapEffectAudio["50"] = ("AviSound/EGPWS/50.wav");
//	m_mapEffectAudio["500"] = ("AviSound/EGPWS/500.wav");
//	m_mapEffectAudio["Approach minimums"] = ("AviSound/EGPWS/Approach minimums.wav");
//	m_mapEffectAudio["bank angle"] = ("AviSound/EGPWS/bank angle.wav");
//	m_mapEffectAudio["dontsink"] = ("AviSound/EGPWS/dontsink.wav");
//	m_mapEffectAudio["glideslope"] = ("AviSound/EGPWS/glideslope.wav");
//	m_mapEffectAudio["minimums"] = ("AviSound/EGPWS/minimums.wav");
//	m_mapEffectAudio["pullup"] = ("AviSound/EGPWS/pullup.wav");
//	m_mapEffectAudio["sinkrate"] = ("AviSound/EGPWS/sinkrate.wav");
//	m_mapEffectAudio["terrain"] = ("AviSound/EGPWS/terrain.wav");
//	m_mapEffectAudio["terrainTerrain"] = ("AviSound/EGPWS/terrainTerrain.wav");
//	m_mapEffectAudio["toolowflaps"] = ("AviSound/EGPWS/toolowflaps.wav");
//	m_mapEffectAudio["toolowgear"] = ("AviSound/EGPWS/toolowgear.wav");
//	m_mapEffectAudio["toolowterrain"] = ("AviSound/EGPWS/toolowterrain.wav");
//	m_mapEffectAudio["windshear"] = ("AviSound/EGPWS/windshear.wav");
//	m_mapEffectAudio["Adjust vertical speed,adjust"] = ("AviSound/TCAS/Adjust vertical speed,adjust.wav");
//	m_mapEffectAudio["Clear of conflict"] = ("AviSound/TCAS/Clear of conflict.wav");
//	m_mapEffectAudio["Climb,climb now"] = ("AviSound/TCAS/Climb,climb now.wav");
//	m_mapEffectAudio["Climb,Crossing,climb"] = ("AviSound/TCAS/Climb,Crossing,climb.wav");
//	m_mapEffectAudio["climb"] = ("AviSound/TCAS/climb.wav");
//	m_mapEffectAudio["descend,Crossing,descend"] = ("AviSound/TCAS/descend,Crossing,descend.wav");
//	m_mapEffectAudio["descend"] = ("AviSound/TCAS/descend.wav");
//	m_mapEffectAudio["Descend; descend now"] = ("AviSound/TCAS/Descend; descend now.wav");
//	m_mapEffectAudio["Increase climb"] = ("AviSound/TCAS/Increase climb.wav");
//	m_mapEffectAudio["Increase descent"] = ("AviSound/TCAS/Increase descent.wav");
//	m_mapEffectAudio["Maintain vertical speed,Crossing,maintain"] = ("AviSound/TCAS/Maintain vertical speed,Crossing,maintain.wav");
//	m_mapEffectAudio["Maintain vertical speed,maintain"] = ("AviSound/TCAS/Maintain vertical speed,maintain.wav");
//	m_mapEffectAudio["Monitor vertical speed"] = ("AviSound/TCAS/Monitor vertical speed.wav");
//	m_mapEffectAudio["TCAS system test fail"] = ("AviSound/TCAS/TCAS system test fail.wav");
//	m_mapEffectAudio["TCAS system test OK"] = ("AviSound/TCAS/TCAS system test OK.wav");
//	m_mapEffectAudio["Traffic Traffic"] = ("AviSound/TCAS/Traffic Traffic.wav");
}

