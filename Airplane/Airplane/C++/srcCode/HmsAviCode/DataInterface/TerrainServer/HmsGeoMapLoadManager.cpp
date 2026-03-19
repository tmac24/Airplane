#include <HmsAviPf.h>
#include "HmsGeoMapLoadManager.h"
#include "../../HmsGlobal.h"

USING_NS_HMS;

CHmsGeoMapLoadManager::CHmsGeoMapLoadManager()
: m_pThread(nullptr)
, m_bThreadRun(true)
, m_nMapLayer(0)
, m_bImageUpdate(false)
, m_nThreadLoadingUse(0)
, m_fLngLatOffset(1)
{
    m_mapTileMapCenter.m_nBindRow = GetCenterRow();
    m_mapTileMapCenter.m_nBindCol = GetCenterCol();

    m_mapDstByOffset = Vec2(110, 110);

    m_image = new Image;

    int nWidth = CHmsTerrainTile::GetWidth() * 3;
    int nHeight = CHmsTerrainTile::GetHeight() * 3;
    int nSize = nWidth * nHeight;
    HmsColorCell * pData = new HmsColorCell[nSize];
    m_image->initWithRawData((unsigned char *)pData, nSize * sizeof(HmsColorCell), nWidth, nHeight, 4);
    delete[] pData;
}


CHmsGeoMapLoadManager::~CHmsGeoMapLoadManager()
{
    m_bThreadRun = false;
    if (m_pThread)
    {
        m_pThread->join();
        delete m_pThread;
        m_pThread = nullptr;
    }
}

void CHmsGeoMapLoadManager::InitManager(const std::string & geoTiffPath)
{
    CHmsTerrainTiffReader::SetGeoTiffPath(geoTiffPath);
}

void CHmsGeoMapLoadManager::Start()
{
    if (nullptr == m_pThread)
    {
        m_bThreadRun = true;
        m_pThread = new std::thread(&CHmsGeoMapLoadManager::ThreadRunning, this);
    }
}

void CHmsGeoMapLoadManager::End()
{
    m_bThreadRun = false;
    if (m_pThread)
    {
        m_pThread->join();
        delete m_pThread;
        m_pThread = nullptr;
    }
}

void CHmsGeoMapLoadManager::ThreadRunning()
{
    while (m_bThreadRun)
    {
        bool bDoLoad = false;
        TerrainTiffIndex gti;

        while (0 == m_nThreadLoadingUse)
        {
            //如果暂停加载，线程暂停
#ifdef  __vxworks
            sleep(10);
#else
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
#endif
        }

        //检查地图是否需要更新
        CheckLoad();

        if (!LoadingMapTile())
        {
#ifdef  __vxworks
            sleep(10);
#else
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
#endif
        }
        else
        {
            UpdateImageData();
        }
    }
}

void CHmsGeoMapLoadManager::SetMapCenter(const HmsAviPf::Vec2 & pos)
{
    m_posCenter = pos;
}

void CHmsGeoMapLoadManager::AddTempMapTileToLoading(const CHmsTerrainTileDescribe & des)
{
#define MultMapTileDescribe std::pair<TerrainTiffIndex, CHmsTerrainTile*>

    if ((des.m_nBindCol >= 0 && des.m_nBindCol < c_nMapTileRow) &&
        (des.m_nBindRow >= 0 && des.m_nBindRow < c_nMapTileCol))

    {
        //CHmsMapTile * pMapTile = new CHmsMapTile();
        CHmsTerrainTile * pMapTile = &m_mapTile[des.m_nBindRow][des.m_nBindCol];
        pMapTile->SetTileDescribe(des);
        pMapTile->SetLayer(m_nMapLayer);
        pMapTile->SetTileDescribe(des);
        m_mapLoadTile.insert(MultMapTileDescribe(des.m_indexTif, pMapTile));
    }
}

void CHmsGeoMapLoadManager::CheckLoad()
{
    auto curMapTile = CHmsTerrainTileDescribe::GetMapTile(m_posCenter, m_nMapLayer);
    curMapTile.m_nBindCol = GetCenterCol();
    curMapTile.m_nBindRow = GetCenterRow();

    if (m_mapTileMapCenter == curMapTile)
    {
        //不需要处理
    }
    else
    {
        //依照行加载
        auto loadRow = [=](CHmsTerrainTileDescribe & mtd)
        {
            auto tempMtd = mtd;
            AddTempMapTileToLoading(tempMtd);

            for (auto nRow = GetCenterRow() - 1; nRow >= 0; nRow--)
            {
                tempMtd = tempMtd.GetUpDescribe();
                AddTempMapTileToLoading(tempMtd);
            }

            tempMtd = mtd;
            for (auto nRow = GetCenterRow() + 1; nRow < c_nMapTileRow; nRow++)
            {
                tempMtd = tempMtd.GetDownDescribe();
                AddTempMapTileToLoading(tempMtd);
            }
        };

        //依照列加载
        auto tempColMtd = curMapTile;
        loadRow(tempColMtd);

        for (auto nCol = GetCenterCol() - 1; nCol >= 0; nCol--)
        {
            tempColMtd = tempColMtd.GetLeftDescribe();
            loadRow(tempColMtd);
        }

        tempColMtd = curMapTile;
        for (auto nCol = GetCenterCol() + 1; nCol < c_nMapTileCol; nCol++)
        {
            tempColMtd = tempColMtd.GetRightDescribe();
            loadRow(tempColMtd);
        }

        m_mapTileMapCenter = curMapTile;
    }
}

void CHmsGeoMapLoadManager::CleanTempMap()
{
    for (auto & c : m_mapTempTerrainRawData)
    {
        if (c.second)
        {
            delete c.second;
            c.second = nullptr;
        }
    }
    m_mapTempTerrainRawData.clear();
}

bool CHmsGeoMapLoadManager::LoadingMapTile()
{
    bool bRet = false;
    for (auto & c : m_mapLoadTile)
    {
        auto find = m_mapTempTerrainRawData.find(c.first);
        if (find != m_mapTempTerrainRawData.end())
        {
            if (find->second && GetMapTileData(*find->second, c.second))
            {
                bRet = true;
            }
            else if (c.second)
            {
                c.second->Clean();
                bRet = true;
            }
        }
        else
        {
            //清空缓存
            CleanTempMap();

            HmsTerrainRawData * pNewRawData = new HmsTerrainRawData;
            if (pNewRawData)
            {
                LoadIndexRawData(c.first, *pNewRawData);
                m_mapTempTerrainRawData[c.first] = pNewRawData;

                if (GetMapTileData(*pNewRawData, c.second))
                {
                    bRet = true;
                }
                else if (c.second)
                {
                    c.second->Clean();
                    bRet = true;
                }
            }
            else if (c.second)
            {
                c.second->Clean();
                bRet = true;
            }
        }

        // 		if (c.second)
        // 		{
        // 			delete c.second;
        // 			c.second = nullptr;
        // 		}
    }

    m_mapLoadTile.clear();
    return bRet;
}

bool CHmsGeoMapLoadManager::LoadIndexRawData(const TerrainTiffIndex & index, HmsTerrainRawData & rawData)
{
    static CHmsTerrainTiffReader s_reader;
    rawData.gtIndex = index;
    if (s_reader.GetTerrainData(index, rawData))
    {
        return true;
    }

    return false;
}

bool CHmsGeoMapLoadManager::GetMapTileData(HmsTerrainRawData & rawData, CHmsTerrainTile * mapTile)
{
    bool bRet = false;
    auto orgData = rawData.pData;
    auto dstData = mapTile->GetData();
    if (mapTile && orgData && dstData)
    {
        auto & tileData = mapTile->GetTileDescribe();

        unsigned int nMaxTile = 1 << tileData.m_nLayer;
        //int nRawPerSegCnt = ((rawData.nDataHeight==6001)?6000:rawData.nDataHeight) / nMaxTile;
        float fRawPerSegCnt = rawData.nDataHeight / (float)nMaxTile;
        float fMove = fRawPerSegCnt / (float)mapTile->GetHeight();

        int nSegRow = tileData.m_nIndex / nMaxTile;
        int nSegCol = tileData.m_nIndex % nMaxTile;

        int nOrgWidthStart = (int)floor(nSegCol * fRawPerSegCnt);
        for (int nRow = 0, i = 0; nRow < mapTile->GetHeight(); nRow++)
        {
            int nColDataPos = (int)(nSegRow * fRawPerSegCnt + nRow * fMove) * rawData.nDataWidth		//列开始点
                + (int)(nSegCol * fRawPerSegCnt);	//行开始点

            for (int nCol = 0; nCol < mapTile->GetWidth(); nCol++, i++)
            {
                dstData[i] = orgData[int(nColDataPos + nCol*fMove)];
            }
        }

        bRet = true;
    }
    return bRet;
}

void CHmsGeoMapLoadManager::UpdateImageData()
{
    std::lock_guard<std::mutex> locker(m_mutexThreadLoading);
    if (m_image)
    {
        HmsColorCell * pData = (HmsColorCell *)m_image->getData();
		if (!pData)
		{
			return;
		}

        auto FillDataFunc = [=](CHmsTerrainTile & mapTile, int nTileRow, int nTileCol)
        {
            auto data = mapTile.GetData();
            if (nullptr == data)
            {
                return;
            }

            int nPosStart = nTileRow * mapTile.GetHeight() * m_image->getWidth() + nTileCol * mapTile.GetWidth();
            int j = 0;
            unsigned short nTemp = 0;
            for (int nRow = 0; nRow < mapTile.GetHeight(); nRow++)
            {
                for (int nCol = 0, i = nPosStart + nRow*m_image->getWidth(); nCol < mapTile.GetWidth(); nCol++, i++, j++)
                {
                    if(data[j]<0)
                    {
                        pData[i].rg = -data[j];//rg只保存绝对值
                        pData[i].a = 0x40;//用a小于0x7F来表示此值为负数；a大于等于0x7F表示为正数（a默认为0xFF）。 目的是便于shader判断正负。
                    }
                    else
                    {
                        pData[i].rg = data[j];//rg只保存绝对值
                        pData[i].a = 0xFF;
                    }
                }
            }
        };

        if (pData)
        {
            for (int nRow = 0; nRow < c_nMapTileRow; nRow++)
            {
                for (int nCol = 0; nCol < c_nMapTileCol; nCol++)
                {
                    auto & mapTile = m_mapTile[nRow][nCol];
                    FillDataFunc(mapTile, nRow, nCol);
                }
            }

            m_mapCenter = m_mapTile[GetCenterRow()][GetCenterCol()].GetTileDescribe().Center();
            auto & leftTopDes = m_mapTile[0][0].GetTileDescribe();
            auto left = leftTopDes.Left();
            auto top = leftTopDes.Top();

            auto & rightBottomDes = m_mapTile[c_nMapTileRow - 1][c_nMapTileCol - 1].GetTileDescribe();
            auto right = rightBottomDes.Right();
            auto bottom = rightBottomDes.Bottom();

            m_mapRect.setRect(left, bottom, right - left, top - bottom);

            m_fLngLatOffset = leftTopDes.GetOffset();

            m_mapDstByOffset.setPoint(CHmsGlobal::GetKmPerLongitude(m_mapCenter.y)*m_fLngLatOffset * 3, CHmsGlobal::GetKmPerLatitude()*m_fLngLatOffset * 3);

        }

        m_bImageUpdate = true;
    }
}

HmsAviPf::Image * CHmsGeoMapLoadManager::GetImage(HmsAviPf::Vec2 & mapCenter, HmsAviPf::Rect & mapRect, HmsAviPf::Vec2 & mapDstOffset, float & fLonLatOffset)
{
    std::lock_guard<std::mutex>		locker(m_mutexThreadLoading);
    Image * imageOut = nullptr;

    if (m_image)
    {
        imageOut = new Image;
        if (imageOut && imageOut->initWithRawData(m_image->getData(), m_image->getDataLen(), m_image->getWidth(), m_image->getHeight(), 4))
        {
            imageOut->autorelease();
        }
        else if (imageOut)
        {
            delete imageOut;
            imageOut = nullptr;
        }
    }

    mapCenter = m_mapCenter;
    mapRect = m_mapRect;
    fLonLatOffset = m_fLngLatOffset;
    mapDstOffset = m_mapDstByOffset;
    m_bImageUpdate = false;

    return imageOut;
}

void CHmsGeoMapLoadManager::SetUsingLoading()
{
    m_nThreadLoadingUse++;
    if (m_nThreadLoadingUse <= 0)
    {
        m_nThreadLoadingUse = 1;
    }
}

void CHmsGeoMapLoadManager::SetUnusedLoading()
{
    m_nThreadLoadingUse--;
    if (m_nThreadLoadingUse < 0)
    {
        m_nThreadLoadingUse = 0;
    }
}










