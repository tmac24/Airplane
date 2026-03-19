#ifndef _BARRIER_COMMON__H_
#define _BARRIER_COMMON__H_

#include "Node.h"
#include "Common.h"
#include "Image.h"

#ifdef __cplusplus
extern "C" {
#endif
    
#include "_Vec3.h"

#ifdef __cplusplus
}
#endif


#pragma pack(1)
typedef struct _BarrierParameters
{
    //索引ID号
    long long record_id;
    /*障碍物类型*/
    char obsType[51];
    char lightingType[2];
    //纬度
    double latitude;
    /*经度*/
    double longitude;
    //障碍物最高点平均海拔高
    int altitude;
    //障碍物最高点离地高度
    int height;
}BarrierParameters, *pCBarrierParameters;

typedef struct  _C3DBarrierRecord
{
    pCBarrierParameters barrier;
    pCNode m_mat;
    pCNode m_drawable;
}C3DBarrierRecord, *pC3DBarrierRecord;

#pragma pack() 

void LittleEndianToBigEndianBarrierParameters(BarrierParameters *ptr);
void PrintfBarrierParameters(BarrierParameters *ptr);


#endif
