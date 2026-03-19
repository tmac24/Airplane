#pragma once

#include <thread>
#include <mutex>
#include <functional>
#include <queue>

#include "render/HmsTexture2D.h"
#include "base/HmsImage.h"
#include "HmsMapTileSql.h"
#include "base/RefPtr.h"

class CHmsTextureFactory
{
public:
	static CHmsTextureFactory* GetInstance();

	typedef std::function<void(HmsAviPf::Texture2D *pTexture, int nLayer, int nX, int nY)> CallbackFunc;
	struct taskStu
	{
		HmsAviPf::Image *pImage;
		int nLayer;
		int nX;
		int nY;
		CallbackFunc func;
	};

	void AddTask(HmsAviPf::Image *pImage, int nLayer, int nX, int nY, CallbackFunc func);

	void LoadTexture();

private:
	CHmsTextureFactory();
	~CHmsTextureFactory();

private:
	std::queue<taskStu> m_taskQuene;
	std::mutex m_mutex;
};

