

#ifndef  _HVT_DATA_COMMON_DEFINE__H_
#define _HVT_DATA_COMMON_DEFINE__H_

#include "GLHeader.h"
#include "Common.h"

#pragma pack(1)
typedef struct _COBSDataOut
{
    long long ID;
    //高压线端号
    int  seqNum;
    //
    double lon;
    double lat;
    int alt;
}COBSDataOut, *pCOBSDataOut;
typedef struct _COBSDataOutLine
{
    //高压线编号
    long long ID;
    //点个数
    unsigned int count;
    //障碍物点
    pCOBSDataOut points;
}COBSDataOutLine, *pCOBSDataOutLine;
#pragma pack()



pCOBSDataOutLine C3DHVTLLineData_Create();
//windows端转大端
void LittleEndianToBigEndianObsData(pCOBSDataOut p);
//打印函数
void PrintfObsData(pCOBSDataOut p);

int  C3DHVTLLineData_Destroy(pCOBSDataOutLine *Line);

void C3DHVTLLineData_Reset(pCOBSDataOutLine data, unsigned int num);
#endif//end of define
