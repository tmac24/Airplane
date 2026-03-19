#include "HmsTextureFactory.h"


CHmsTextureFactory* CHmsTextureFactory::GetInstance()
{
	static CHmsTextureFactory fac;
	return &fac;
}

CHmsTextureFactory::CHmsTextureFactory()
{

}

CHmsTextureFactory::~CHmsTextureFactory()
{

}

void CHmsTextureFactory::AddTask(HmsAviPf::Image *pImage, int nLayer, int nX, int nY, CallbackFunc func)
{
	m_mutex.lock();

	m_taskQuene.push({ pImage, nLayer, nX, nY, func });

	m_mutex.unlock();
}

void CHmsTextureFactory::LoadTexture()
{
	for (int i = 0; i < 20; ++i)
	{
		m_mutex.lock();
		if (m_taskQuene.empty())
		{
			m_mutex.unlock();
			break;
		}
		taskStu stu = m_taskQuene.front();
		m_taskQuene.pop();

		m_mutex.unlock();
		
		if (stu.pImage)
		{
			auto texture = new  HmsAviPf::Texture2D();
			if (texture && texture->initWithImage(stu.pImage))
			{
				texture->autorelease();
				if (stu.func)
				{
					stu.func(texture, stu.nLayer, stu.nX, stu.nY);
				}
			}

			delete stu.pImage;
			stu.pImage = nullptr;
		}
	}
}
