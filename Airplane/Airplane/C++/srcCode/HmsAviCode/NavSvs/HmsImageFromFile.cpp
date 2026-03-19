#include "HmsImageFromFile.h"
#include "base/HmsTimer.h"
#include "HmsGlobal.h"
#include <map>

struct SvsTexHeaderStu
{
    int nLayer;
    int nX;
    int nY;
    int nLen;
    HmsUint64 nPos;
};

CHmsImageFromFile::CHmsImageFromFile()
{
    m_dataLen = 0;
    m_onePktLen = 0;
    m_pFinData = nullptr;
    m_rangeArrayStu.InitWithSize(200000);

    m_tmpDataLen = 256 * 256 * 3;
    m_pTmpData = (char*)malloc(sizeof(char)* m_tmpDataLen);
    memset(m_pTmpData, 0, m_tmpDataLen);

    std::map<std::string, HmsDbMap> mapMapConfig;
    if (CHmsConfigXMLReader::GetSingleConfig()->GetMapConfigInfo(mapMapConfig, "NavSvsMapConfig"))
    {
        auto iter = mapMapConfig.find("satellite");
        if (iter != mapMapConfig.end())
        {
            m_DbMap = iter->second;
        }
    }
}

CHmsImageFromFile::~CHmsImageFromFile()
{
    if (m_pTmpData)
    {
        free(m_pTmpData);
    }
}

bool CHmsImageFromFile::GetData(int nLayer, int nX, int nY, Image* pImage)
{
    bool bRet = false;

    auto pDbPath = GetResDataFileName(nLayer, nX, nY);
    if (pDbPath)
    {
        OpenFile(pDbPath);
        if (!m_pFinData)
        {
            return bRet;
        }

        HmsNavSvsRangeArrayValue value;
        if (m_rangeArrayStu.GetData(nX, nY, &value))
        {
            HmsUint64 nPos = value.dataA;
            HmsUint64 nLen = value.dataB;
            if ((nPos + nLen <= m_dataLen))
            {
                fseek(m_pFinData, nPos, SEEK_SET);

                if (nLen > m_tmpDataLen)
                {
                    if (m_pTmpData)
                    {
                        free(m_pTmpData);
                        m_pTmpData = NULL;
                        m_tmpDataLen = 0;
                    }
                    m_pTmpData = (char*)malloc(sizeof(char)* nLen);
                    if (!m_pTmpData)
                    {
                        return bRet;
                    }
                    m_tmpDataLen = nLen;
                }
                if (m_pTmpData)
                {
                    fread(m_pTmpData, sizeof(unsigned char), nLen, m_pFinData);
                    bRet = pImage->initWithImageData((const unsigned char*)m_pTmpData, nLen);
                }
            }
        }
    }
    return bRet;
}

void CHmsImageFromFile::OpenFile(const std::string *pStrPath)
{
    if (m_pFilePath == pStrPath)
    {
        return;
    }
    CloseFile();
#if 1
    //==================
    CHmsTime time;
    time.RecordCurrentTime();
    double start = time.GetTime();
    printf("\n open %s start\n", pStrPath->c_str());
    //==================
#endif

    m_pFilePath = (std::string*)pStrPath;

    m_pFinData = fopen(pStrPath->c_str(), "rb");
    if (!m_pFinData)
    {
        return;
    }
    fseek(m_pFinData, 0, SEEK_END);
    HmsUint64 fileLen = ftell(m_pFinData);

    int intLen = sizeof(int);
    int headerLen = 0;
    fseek(m_pFinData, -intLen, SEEK_END);
    fread((char*)&headerLen, sizeof(char), intLen, m_pFinData);

    m_dataLen = fileLen - headerLen - intLen;

    fseek(m_pFinData, -(intLen + headerLen), SEEK_END);
    SvsTexHeaderStu* pArrayEle = (SvsTexHeaderStu*)malloc(headerLen);
    if (!pArrayEle)
    {
        return;
    }
    fread((char*)pArrayEle, sizeof(char), headerLen, m_pFinData);

#if 1
    printf("\n open %s header %f\n", pStrPath->c_str(), time.GetElapsed());
    time.RecordCurrentTime();
#endif
    for (int i = 0; i < headerLen / (int)sizeof(SvsTexHeaderStu); ++i)
    {
        m_rangeArrayStu.UpdateRange(pArrayEle[i].nX, pArrayEle[i].nY);
    }
    for (int i = 0; i < headerLen / (int)sizeof(SvsTexHeaderStu); ++i)
    {
        m_rangeArrayStu.SetDataByAB(pArrayEle[i].nX, pArrayEle[i].nY, pArrayEle[i].nPos, pArrayEle[i].nLen);
    }
    free(pArrayEle);

#if 1
    printf("\n open %s insert %f\n", pStrPath->c_str(), time.GetElapsed());
    time.RecordCurrentTime();

    printf("\n open %s end %f\n", pStrPath->c_str(), time.GetTime() - start);
#endif
}

void CHmsImageFromFile::CloseFile()
{
    if (m_pFinData)
    {
        fclose(m_pFinData);
        m_pFinData = nullptr;
    }
    m_pFilePath = nullptr;
    m_rangeArrayStu.Reset();
}

const std::string* CHmsImageFromFile::GetResDataFileName(int nLayer, int nX, int nY)
{
    if (nLayer > m_DbMap.layerMax || nLayer < m_DbMap.layerMin)
    {
        return nullptr;
    }
    HmsDbMapLayer *pDbMapLayer = &m_DbMap.mapLayer.at(nLayer);
    if (pDbMapLayer->isSubdivision)
    {
        int count = pDbMapLayer->vDbPath.size();
        for (int i = 0; i < count; ++i)
        {
            HmsDbSubMapLayer* subLayer = &pDbMapLayer->vDbPath[i];
            if (nX >= subLayer->xMin && nX <= subLayer->xMax && nY >= subLayer->yMin && nY <= subLayer->yMax)
            {
                return &subLayer->strDbPath;
            }
        }
    }
    else
    {
        return &pDbMapLayer->strDbPath;
    }

    return nullptr;
}


