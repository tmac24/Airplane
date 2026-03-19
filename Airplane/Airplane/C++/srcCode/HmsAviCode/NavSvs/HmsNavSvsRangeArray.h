#pragma once

#include "HmsStandardDef.h"

struct HmsNavSvsRangeArrayValue
{
    HmsUint64 dataA;
    HmsUint64 dataB;

    HmsNavSvsRangeArrayValue()
    {
        dataA = 0;
        dataB = 0;
    }
};

class CHmsNavSvsRangeArray
{
public:
    CHmsNavSvsRangeArray();
    ~CHmsNavSvsRangeArray();

    void Init();
    void InitWithSize(int size);
    void Reset();
    void SetData(int x, int y, HmsNavSvsRangeArrayValue *pData);
    void SetDataByA(int x, int y, HmsUint64 dataA);
    void SetDataByAB(int x, int y, HmsUint64 dataA, HmsUint64 dataB);
    bool GetData(int x, int y, HmsNavSvsRangeArrayValue *pData);
    void UpdateRange(int x, int y);
    bool IsInRange(int x, int y);

private:
    int m_xStart;
    int m_xEnd;
    int m_yStart;
    int m_yEnd;
    int m_width;
    int m_height;
    int m_arraySize;
    HmsNavSvsRangeArrayValue *m_pDataArray;
};

