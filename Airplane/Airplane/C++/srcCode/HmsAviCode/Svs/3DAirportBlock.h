#ifndef _3D_AIRPORT_BLOCK__H_
#define _3D_AIRPORT_BLOCK__H_


#include "Node.h"
#include "AirportCommon.h"
#include "MatrixNode.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "_Vec3.h"
#include "_Vec4.h"

#ifdef __cplusplus
}
#endif


class C3DAirportBlock
{
public:
    /*经纬高*/
    CVec3d m_lonlat;
    /*飞行器姿态*/
    CVec3d m_hpr;
    /*经纬度转墨卡托位置*/
    CVec3d m_m_lonlatPos;
    /*本块的根节点*/
    pCNode m_root;

    pCMatrixTransform m_mat;
    /*机场数据集*/
    //c_pvector  m_airportInfo;
    std::vector<pCAirport> m_airportInfo2;
    /*跑道数据集*/
    //c_pvector  m_runWayInfo;
    std::vector<pCRunway> m_runWayInfo2;
    /*绘制跑道数据集*/
    //c_pvector  m_drawRunwayInfo; 
    std::vector<pCDrawRunway> m_drawRunwayInfo2;
    /*机场数据集*/
    //c_vector  m_v_airportInfo;
    /*跑道数据集*/
    //c_vector  m_v_runWayInfo;
    /*绘制跑道数据集*/
    //c_vector  m_v_drawRunwayInfo; 
    C3DAirportBlock(){
        m_lonlat = Vec_CreatVec3d(0, 0, 0);
        m_hpr = Vec_CreatVec3d(0, 0, 0);
        m_m_lonlatPos = Vec_CreatVec3d(0, 0, 0);
        m_root = 0;
        m_mat = 0;
    }
};

typedef C3DAirportBlock* pC3DAirportBlock;

pC3DAirportBlock C3DAirportBlock_Create();

void C3DAirportBlock_Destory(pC3DAirportBlock *blk);

void C3DAirportBlockReadData(pC3DAirportBlock blk);

void C3DAirportBlockCreatNode(pC3DAirportBlock blk);

void C3DAirportBlockCreatMatAirport(pCRunway  ruw);

pCNode C3DAirportBlockCreatRunwayDrawable(pCDrawRunway ruw);

pCNode C3DAirportBlockCreatRunwayFrame(pCDrawRunway ruw);

pCNode C3DAirportBlockCreatAirportDrawable(pCAirport airport);

void C3DAirportBlockSetLonlat(pC3DAirportBlock blk, CVec3d lonlatalt, CVec3d hpr);

void  C3DAirportAnysysRunwayData(pC3DAirportBlock blk);

/*绘制跑道路*/
pCNode C3DAirportBlockCreatRunway(pCDrawRunway ruw);
/*绘制跑道 端口*/
pCNode C3DAirportBlockCreatRunwayThreshold(pCDrawRunway ruw, unsigned char isLittle);
/*绘制跑道 左右的字符*/
pCNode C3DAirportBlockCreatRunwayLeft(pCDrawRunway ruw, unsigned char isLittle);
/*绘制跑道 号*/
pCNode C3DAirportBlockCreatRunwayNum(pCDrawRunway ruw, unsigned char isLittle);

#endif