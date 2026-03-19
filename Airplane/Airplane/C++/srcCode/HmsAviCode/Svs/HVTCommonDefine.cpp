
#include "HVTCommonDefine.h"


pCOBSDataOutLine C3DHVTLLineData_Create()
{
    /*创建高压线数据*/
    pCOBSDataOutLine data = (pCOBSDataOutLine)malloc(sizeof(COBSDataOutLine));
    memset(data, 0, sizeof(COBSDataOutLine));
    return data;
}
void C3DHVTLLineData_Reset(pCOBSDataOutLine data, unsigned int num)
{
    data->count = num;

    data->points = (pCOBSDataOut)malloc(sizeof(COBSDataOut)*num);

    memset(data->points, 0, sizeof(COBSDataOut)*num);
}
int C3DHVTLLineData_Destroy(pCOBSDataOutLine *Line)
{
    pCOBSDataOutLine data = *Line;
    if (data)
    {
        if (data->count && data->points)
        {
            free(data->points);
            data->points = 0;
        }
        free(data);
        data = 0;
        *Line = 0;
    }
    return 0;
}
void LittleEndianToBigEndianObsData(pCOBSDataOut point)
{
    point->ID = LittleEndianToBigEndianLongLong(point->ID);
    point->seqNum = LittleEndianToBigEndianInt(point->seqNum);
    point->lon = LittleEndianToBigEndianDouble(point->lon);
    point->lat = LittleEndianToBigEndianDouble(point->lat);
    point->alt = LittleEndianToBigEndianDouble(point->alt);
}
void PrintfObsData(pCOBSDataOut p)
{
    printf("高压线点 Id:%lld,端号seqNum:%d,经纬高lonlatalt:%lf,%lf,%d\n",
        p->ID,
        p->seqNum,
        p->lon,
        p->lat,
        p->alt);
}
