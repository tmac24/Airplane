#pragma once

#include "base/HmsImage.h"
#include "render/HmsTexture2D.h"
#include "HmsStandardDef.h"
#include <string>
#include <thread>
#include <mutex>
#include <deque>

using namespace HmsAviPf;

typedef std::function<void(void *pObj, int nLayer, int nX, int nY)> SvsDataFactoryDemCallback;
struct FactoryDemStu
{
    void *pObj;
    int nLayer;
    int nX;
    int nY;
    SvsDataFactoryDemCallback func;

    FactoryDemStu()
    {
        pObj = nullptr;
        nLayer = 0;
        nX = 0;
        nY = 0;
        func = nullptr;
    }
};

typedef std::function<void(void *pObj, int nLayer, int nX, int nY, bool bSuccess)> SvsDataFactoryTextureCallback;
struct FactoryTextureStu
{
    void *pObj;
    Image *pImage;
    Texture2D *pTexture;
    int nLayer;
    int nX;
    int nY;
    SvsDataFactoryTextureCallback func;

    FactoryTextureStu()
    {
        pObj = nullptr;
        pImage = nullptr;
        pTexture = nullptr;
        nLayer = 0;
        nX = 0;
        nY = 0;
        func = nullptr;
    }
};

class CHmsNavSvsDataFactory
{
public:
    static CHmsNavSvsDataFactory* GetInstance();
    void Run();
    void AddDemTask(FactoryDemStu *pStu);
    void AddTextureTask(FactoryTextureStu *pStu);

private:
    CHmsNavSvsDataFactory();
    ~CHmsNavSvsDataFactory();

    void DealDemTask();
    void DealTexTask();

private:
    std::deque<FactoryDemStu> m_dequeDem;//FactoryDemStu
    std::deque<FactoryTextureStu> m_dequeTex;//FactoryTextureStu

    std::mutex m_mutex;
};



