#ifndef _AIRPORT_COMMON_H
#define _AIRPORT_COMMON_H 

#include "Node.h"
#include "GLHeader.h"
#include "Common.h"


#if VxWorks_JINGJIA_5400
typedef struct _CRunway
{
    /*机场四字码*/
    char ICAO_id[5];
    /*机场编号*/
    char ICAO_code[3];
    /*跑道编号*/
    char runway_id[6];	
    /*跑道经纬度*/
    double lat;
    double lon;

    /*跑道长*/
    int len;
    /*跑道宽*/
    int width;
    /*高度*/
    int altitude;
    /*跑道方向*/
    double direction;
    /*跑道梯度*/
    double gradient;	
}__attribute__((packed, aligned(1))) CRunway,*pCRunway;

typedef struct _CAirport
{
    /*四字节编码*/
    char airportICAO[5];
    /*ATA_IATA_Designator*/
    char ATA_IATA_Designator[4];
    /*名字*/
    char name[31];
    /*纬度*/
    double lat;
    /*经度*/
    double lon;
    /*海拔*/
    int altitude;
    /*校验*/
    short CRC;
}__attribute__((packed, aligned(1))) CAirport,*pCAirport;


typedef struct _CDrawRunway
{
    /*小角度端*/
    pCRunway runway;
    /*两端跑道*/
    unsigned char isDoth;
    /*大角度端跑道名称*/
    char runway_id[6];
}CDrawRunway, *pCDrawRunway;

#else

#pragma pack(push)
#pragma pack(1)

typedef struct  _CAirport
{
    /*四字节编码*/
    char airportICAO[5];
    /*ATA_IATA_Designator*/
    char ATA_IATA_Designator[4];
    /*名字*/
    char name[31];
    /*纬度*/
    double lat;
    /*经度*/
    double lon;
    /*海拔*/
    int altitude;
    /*校验*/
    short CRC;
}CAirport, *pCAirport;


typedef struct _CRunway
{
    /*机场四字码*/
    char ICAO_id[5];
    /*机场编号*/
    char ICAO_code[3];
    /*跑道编号*/
    char runway_id[6];
    /*跑道经纬度*/
    double lat;
    double lon;

    /*跑道长*/
    int len;
    /*跑道宽*/
    int width;
    /*高度*/
    int altitude;
    /*跑道方向（真航向）*/
    double direction;//todo： jy 此处为跑道的真航向。经检查，老的数据除了zuuu和zlxy为真航向外，中国其他机场都为磁航向，跑道数据需要更新。新数据此处需为真航向。
    /*跑道梯度*/
    double gradient;
}CRunway, *pCRunway;


typedef struct _CDrawRunway
{
    /*小角度端*/
    pCRunway runway;
    /*两端跑道*/
    unsigned char isDoth;
    /*大角度端跑道名称*/
    char runway_id[6];
}CDrawRunway, *pCDrawRunway;

#pragma pack(pop)


#pragma pack(push)
#pragma pack(1) 

#pragma pack(pop)
#endif 

#if 0
typedef struct _RunwayBlock 
{
    /*标示该经纬度块*/
    int lon,lat;
    /*该块的跑道根节点*/
    pCNode runway_block_node;
    c_pvector runway_info_vec;
    c_vector _v_runway_info_vec;
    int needKeep;
}RunwayBlock,*pRunwayBlock;
pRunwayBlock RunwayBlock_Init();
#endif


/*******************************************************************************
名称：LittleEndianToBigEndianRunway
功能：vxworks是大端 windows小端，所以要进行转换一下
参数：无
时间:  11:17:53 7/25/2015
用户 : xiafei
版本：1.0
注意：
返回值：
*******************************************************************************/
void LittleEndianToBigEndianRunway(CRunway *info);

void PrintRunway(CRunway *info);


#endif
