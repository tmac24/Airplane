#include "HmsNavSvsRangeArray.h"
#include <stdlib.h>
#include <string>
#include <string.h>

CHmsNavSvsRangeArray::CHmsNavSvsRangeArray()
{
    Init();
}

CHmsNavSvsRangeArray::~CHmsNavSvsRangeArray()
{
    if (m_pDataArray)
    {
        free(m_pDataArray);
    }
}

void CHmsNavSvsRangeArray::Init()
{
    Reset();
    m_arraySize = 0;
    m_pDataArray = nullptr;
}

void CHmsNavSvsRangeArray::InitWithSize(int size)
{
    Reset();
    m_arraySize = size;
    m_pDataArray = (HmsNavSvsRangeArrayValue*)malloc(sizeof(HmsNavSvsRangeArrayValue)* size);
    if (m_pDataArray)
    {
        memset(m_pDataArray, 0, m_arraySize * sizeof(HmsNavSvsRangeArrayValue));
    }
}

void CHmsNavSvsRangeArray::Reset()
{
    m_xStart = 99999;
    m_xEnd = -99999;
    m_yStart = 99999;
    m_yEnd = -99999;
    m_width = 0;
    m_height = 0;
}

void CHmsNavSvsRangeArray::SetData(int x, int y, HmsNavSvsRangeArrayValue *pData)
{
    if (m_pDataArray && IsInRange(x, y))
    {
        int index = (x - m_xStart) * m_height + (y - m_yStart);
        m_pDataArray[index] = *pData;
    }
}

void CHmsNavSvsRangeArray::SetDataByA(int x, int y, HmsUint64 dataA)
{
    if (m_pDataArray && IsInRange(x, y))
    {
        int index = (x - m_xStart) * m_height + (y - m_yStart);
        m_pDataArray[index].dataA = dataA;
    }
}

void CHmsNavSvsRangeArray::SetDataByAB(int x, int y, HmsUint64 dataA, HmsUint64 dataB)
{
    if (m_pDataArray && IsInRange(x, y))
    {
        int index = (x - m_xStart) * m_height + (y - m_yStart);
        m_pDataArray[index].dataA = dataA;
        m_pDataArray[index].dataB = dataB;
    }
}

bool CHmsNavSvsRangeArray::GetData(int x, int y, HmsNavSvsRangeArrayValue *pData)
{
    if (m_pDataArray && IsInRange(x, y))
    {
        int index = (x - m_xStart) * m_height + (y - m_yStart);
        *pData = m_pDataArray[index];
        return true;
    }
    return false;
}

void CHmsNavSvsRangeArray::UpdateRange(int x, int y)
{
    if (x < m_xStart)
    {
        m_xStart = x;
    }
    if (x > m_xEnd)
    {
        m_xEnd = x;
    }
    if (y < m_yStart)
    {
        m_yStart = y;
    }
    if (y > m_yEnd)
    {
        m_yEnd = y;
    }
    m_width = m_xEnd - m_xStart + 1;
    m_height = m_yEnd - m_yStart + 1;

    if (m_width * m_height > m_arraySize)
    {
        if (m_pDataArray)
        {
            free(m_pDataArray);
        }
        m_arraySize = m_width * m_height;
        m_pDataArray = (HmsNavSvsRangeArrayValue*)malloc(sizeof(HmsNavSvsRangeArrayValue)* m_arraySize);
        if (m_pDataArray)
        {
            memset(m_pDataArray, 0, m_arraySize * sizeof(HmsNavSvsRangeArrayValue));
        }
    }
}

bool CHmsNavSvsRangeArray::IsInRange(int x, int y)
{
    if (x >= m_xStart && x <= m_xEnd)
    {
        if (y >= m_yStart && y <= m_yEnd)
        {
            return true;
        }
    }
    return false;
}
