#ifndef _HMS_AVI_AUDIO_ENGINE_H____
#define _HMS_AVI_AUDIO_ENGINE_H____

#include <map>
#include <string>

class CHmsAviAudioEngine
{
public:
	static CHmsAviAudioEngine* getInstance();

	~CHmsAviAudioEngine();

	//预处理音频
	void PreloadAudio();

	//播放背景音乐
	void PlayBgMusic();

	void PlayEffectByName(std::string name, bool bLoop);
	void playEffectByPath(std::string strEffectPath, bool bLoop);

	void PlayTerrainTerrain();

	//设置音乐大小
	void SetMusicVol(float vol);
	//设置音效大小
	void SetEffectVol(float vol);

	//停止所有音频播放
	void StopAllSound();

	void SetActive(bool bActive);

protected:
	void PreloadAllAudio();
	void PreloadCurrentUsedAudio();
	void Init();

private:
	float m_fVomMusicVom;
	float m_fVomEffectVom;
	bool  m_bActive;

	std::map<std::string, std::string>  m_mapEffectAudio;		//name,path

private:
	static CHmsAviAudioEngine * s_pInstance;
};

#endif