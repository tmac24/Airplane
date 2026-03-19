#include "HmsNavSvsDataFactory.h"

CHmsNavSvsDataFactory::CHmsNavSvsDataFactory()
{

}

CHmsNavSvsDataFactory::~CHmsNavSvsDataFactory()
{

}

CHmsNavSvsDataFactory* CHmsNavSvsDataFactory::GetInstance()
{
    static CHmsNavSvsDataFactory *s_pIns = nullptr;
    if (!s_pIns)
    {
        s_pIns = new CHmsNavSvsDataFactory;
    }
    return s_pIns;
}

void CHmsNavSvsDataFactory::DealDemTask()
{
    if (m_dequeDem.size() > 0)
    {
        m_mutex.lock();

        FactoryDemStu task = m_dequeDem.front();
        m_dequeDem.pop_front();

        m_mutex.unlock();

        if (task.func)
        {
            task.func(task.pObj, task.nLayer, task.nX, task.nY);
        }
    }
}

void CHmsNavSvsDataFactory::DealTexTask()
{
    if (m_dequeTex.size() > 0)
    {
        m_mutex.lock();

        FactoryTextureStu task = m_dequeTex.front();
        m_dequeTex.pop_front();

        m_mutex.unlock();

        if (task.pImage && task.pTexture)
        {
            bool bRet = task.pTexture->initWithImage(task.pImage);

            if (task.func)
            {
                task.func(task.pObj, task.nLayer, task.nX, task.nY, bRet);
            }

            HMS_SAFE_RELEASE(task.pImage);
        }
    }
}

void CHmsNavSvsDataFactory::Run()
{
    static int ctrlImageSpeed = 0;
    ++ctrlImageSpeed;
    if (ctrlImageSpeed > 10000)
    {
        ctrlImageSpeed = 0;
    }

    int demTaskSize = m_dequeDem.size();
    int texTaskSize = m_dequeTex.size();

    if (true || demTaskSize + texTaskSize > 200)
    {
        //printf("\n deal 200 \n");
        while (m_dequeDem.size() > 0)
        {
            DealDemTask();
        }
        while (m_dequeTex.size() > 0)
        {
            DealTexTask();
        }
    }
    else if (ctrlImageSpeed % 3 == 0)
    {
        static bool s_bSvsDataFactorySwitch = false;
        s_bSvsDataFactorySwitch = !s_bSvsDataFactorySwitch;
        if (s_bSvsDataFactorySwitch)
        {
            DealDemTask();
        }
        else
        {
            DealTexTask();
        }
    }
}

void CHmsNavSvsDataFactory::AddDemTask(FactoryDemStu *pStu)
{
    m_mutex.lock();

    m_dequeDem.push_back(*pStu);

    m_mutex.unlock();
}

void CHmsNavSvsDataFactory::AddTextureTask(FactoryTextureStu *pStu)
{
    m_mutex.lock();

    m_dequeTex.push_back(*pStu);

    m_mutex.unlock();
}


