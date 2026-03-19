#include "HmsLoadTextureThread.h"
#ifdef __vxworks
#include <tasklib.h>
#endif

CHmsLoadTextureThread* CHmsLoadTextureThread::GetInstance()
{
	static CHmsLoadTextureThread temp;
	return &temp;
}

CHmsLoadTextureThread::CHmsLoadTextureThread()
: m_pThread(nullptr)
{
	m_bRunning = true;

	m_pThread = new std::thread(&CHmsLoadTextureThread::LoadTexture, this);
}

CHmsLoadTextureThread::~CHmsLoadTextureThread()
{
	m_bRunning = false;

	if (m_pThread)
	{
		m_pThread->join();
		delete m_pThread;
		m_pThread = nullptr;
	}
}

void CHmsLoadTextureThread::AddLoadTask(int nLayer, int nX, int nY, CallbackFunc func, long id)
{
	m_mutex.lock();
	m_taskQuene.push({ id, nLayer, nX, nY, func });
	m_mapTaskCount[id]++;
	m_mutex.unlock();
}

void CHmsLoadTextureThread::LoadTexture()
{
	CHmsMapTileSql &mapTileSql = CHmsMapTileSql::GetInstance();
    auto mapTypeName = mapTileSql.GetDefaultMapDb();
    mapTileSql.ChangeMapTypeName(mapTypeName);

	while (m_bRunning)
	{
		if (!m_taskQuene.empty())
		{
			m_mutex.lock();

			taskStu task = m_taskQuene.front();
			m_taskQuene.pop();
			m_mapTaskCount[task.id]--;
			if (m_mapTaskCount[task.id] != 0)
			{
				if (task.func)
				{
					task.func(nullptr, task.nLayer, task.nX, task.nY);
				}

				m_mutex.unlock();
				continue;
			}

			m_mutex.unlock();

			auto p = GetTileImage(task.nLayer, task.nX, task.nY, mapTileSql);
			if (task.func)
			{
				task.func(p, task.nLayer, task.nX, task.nY);
			}
		}
		else
		{
#ifdef __vxworks
			taskDelay(5);
#else
			std::this_thread::sleep_for (std::chrono::milliseconds(100));
#endif
		}
	}
}

HmsAviPf::Image * CHmsLoadTextureThread::GetTileImage(int nLayer, int nX, int nY, CHmsMapTileSql &mapTileSql)
{
	HmsAviPf::Image* image = nullptr;
	do
	{
		image = new  HmsAviPf::Image();
		HMS_BREAK_IF(nullptr == image);

		bool ret = mapTileSql.GetImageFromDB(nLayer, nX, nY, image);
		if (!ret)
		{
			delete image;
			image = nullptr;
			return nullptr;
		}	
		//HMS_BREAK_IF(!bRet);

	} while (0);

	return image;
}