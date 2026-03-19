#ifndef _ALTMANAGER__H_
#define _ALTMANAGER__H_

#include <vector>
#include "GLHeader.h"
#include "Coordinate.h"
#include "TerrainHead.h"


#if TERRAIN2D_OPTMIZITION
#define TERRAIN2D_ALT_PRECISION 1200
#define TERRAIN2D_ALT_PRECISION_2D 1200
#else
#define TERRAIN2D_ALT_PRECISION 512
#endif

typedef struct _Terrain2d_chunk
{
    short alt;
    unsigned char init;
}Terrain2d_chunk, pTerrain2d_chunk;

typedef struct _AltData
{
    /*经度*/
    int lon;
    /*纬度*/
    int lat;
    /*点个数*/
    int count;
    /*每个点代表的精度*/
    double unit;
    /*数据*/
    unsigned char* data;
    /*平均值*/
    unsigned short averageAlt;
    /*文件是否存在*/
    unsigned char isExist;
#if TERRAIN2D_OPTMIZITION
    Terrain2d_chunk altcache[TERRAIN2D_ALT_PRECISION*TERRAIN2D_ALT_PRECISION];
#endif
}AltData, *pAltData;

//struct _CAltDataMgr
class CAltDataMgr
{
public:
    unsigned int m_cacheDataNum;
    //c_pvector m_altData;
    //c_vector m_v_altData;
    std::vector<pAltData> m_altData2;
    pAltData m_curBlock = nullptr;
};

typedef CAltDataMgr* pCAltDataMgr;

pCAltDataMgr GetCAltDataMgr();

pAltData  AltData_Creatp();

int CAltManager_GetAltByLonLat(double lon, double lat);
unsigned char* CAltManager_ReadElevation(double lon, double lat);
static pAltData ReadAltDataBlock(double lon, double lat);
static short GetAnysisBlockAlt(pAltData m_altData, double lon, double lat);


#endif//_ALTMANAGER__H_
