#pragma once

#include "base/HmsFileUtils.h"
#include "base/HmsImage.h"
#include "HmsConfigXMLReader.h"
#include "HmsStandardDef.h"
#include "HmsNavSvsRangeArray.h"
#include <string>

using namespace HmsAviPf;

class CHmsImageFromFile
{
public:
    CHmsImageFromFile();
    ~CHmsImageFromFile();

    bool GetData(int nLayer, int nX, int nY, Image* pImage);
    void OpenFile(const std::string *pStrPath);
    void CloseFile();
    const std::string* GetResDataFileName(int nLayer, int nX, int nY);

private:
    FILE *m_pFinData;
    int m_onePktLen;
    HmsUint64 m_dataLen;
    std::string *m_pFilePath;
    CHmsNavSvsRangeArray m_rangeArrayStu;

    char *m_pTmpData;
    int m_tmpDataLen;

    HmsDbMap m_DbMap;
};

