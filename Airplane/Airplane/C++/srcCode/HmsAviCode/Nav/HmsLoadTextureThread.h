#pragma once

#include <thread>
#include <mutex>
#include <functional>
#include <queue>

#include "render/HmsTexture2D.h"
#include "base/HmsImage.h"
#include "HmsMapTileSql.h"

class CHmsLoadTextureThread
{
public:
	static CHmsLoadTextureThread* GetInstance();

	typedef std::function<void(HmsAviPf::Image *pImage, int nLayer, int nX, int nY)> CallbackFunc;
	struct taskStu
	{
		long id;
		int nLayer;
		int nX;
		int nY;
		CallbackFunc func;
#if _NAV_SVS_LOGIC_JUDGE
		taskStu()
		{
			id = 0;
			nLayer = 0;
			nX = 0;
			nY = 0;
			func = nullptr;
		}
#endif
	};

	void AddLoadTask(int nLayer, int nX, int nY, CallbackFunc func, long id);

private:
	CHmsLoadTextureThread();
	~CHmsLoadTextureThread();

	void LoadTexture();

	HmsAviPf::Image * GetTileImage(int nLayer, int nX, int nY, CHmsMapTileSql &mapTileSql);

private:
	std::queue<taskStu> m_taskQuene;
	std::thread *m_pThread;
	std::mutex m_mutex;

	bool m_bRunning;

	std::map<int, int> m_mapTaskCount;
};


