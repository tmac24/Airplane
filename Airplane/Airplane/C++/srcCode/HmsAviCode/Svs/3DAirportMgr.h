#ifndef _3DAIRPORT_MGR_H_
#define _3DAIRPORT_MGR_H_

#include "GLHeader.h"
#include "_Vec3.h"
#include "Node.h"

#include "3DAirportBlock.h"

class C3DAirportMgr
{
public:
    pCNode  m_root;
    int m_lonCount;
    int m_latCount;
    int m_startLon;
    int m_startLat;
    int m_endLon;
    int m_endLat;

    /*需要添加的BarrierLonLatUnit*/
    //c_pvector addChunk ;
    std::vector<pC3DAirportBlock> addChunk2;
    /*需要删除的BarrierLonLatUnit*/
    //c_pvector delChunk ;
    std::vector<pC3DAirportBlock> delChunk2;
    /*需要显示的BarrierLonLatUnit*/
    //c_pvector renderChunk; 
    std::vector<pC3DAirportBlock> renderChunk2;
    /*需要显示的BarrierLonLatUnit*/
    //c_pvector tmpChunk; 
    std::vector<pC3DAirportBlock> tmpChunk2;

    //c_vector m_v_addChunk ;
    /*需要删除的BarrierLonLatUnit*/
    //c_vector m_v_delChunk ;
    /*需要显示的BarrierLonLatUnit*/
    //c_vector m_v_renderChunk; 
    /*需要显示的BarrierLonLatUnit*/
    //c_vector m_v_tmpChunk; 

    CVec3d m_lonlatPos;
    CVec3d m_aircraftLonlat;
    CVec3d m_hpr;//jy: heading pitch roll
    unsigned int updateNum;

    pCNode m_terrainMat;
    pCNode m_aircraftIco;

    C3DAirportMgr(){
        m_lonCount = 0;
        m_latCount = 0;
        m_startLon = 0;
        m_startLat = 0;
        m_endLon = 0;
        m_endLat = 0;
        m_lonlatPos = Vec_CreatVec3d(0, 0, 0);
        m_aircraftLonlat = Vec_CreatVec3d(0, 0, 0);
        m_hpr = Vec_CreatVec3d(0, 0, 0);
        updateNum = 0;
        m_terrainMat = 0;
        m_aircraftIco = 0;
    }
};

typedef C3DAirportMgr* pC3DAirportMgr;

static pC3DAirportMgr C3DAirportMgr_Create();

//static unsigned char C3DAirportMgrIsContain(c_pvector chunkLonLatVector,pC3DAirportBlock chunk);
bool C3DAirportMgrIsContain(std::vector<pC3DAirportBlock>& chunkLonLatVector, pC3DAirportBlock chunk);
pC3DAirportMgr GetC3DAirportMgr();

void C2DC3DAirportRangeCalc(void);
void C3DAirportMgrUpdateBlockDelete();
void C3DAirportMgrUpdate();
void C3DAirportMgrSetLonLat(CVec3d lonlat, CVec3d hpr);


void C3DAirportUpdateChunkAdd();
void C3DAirportUpdateChunk();
void C3DAirportUpdateRender();

void C3DAirportMgrVectorRemove(std::vector<pC3DAirportBlock>& chunkVector, pC3DAirportBlock blk);
//void  C3DAirportClearChunkVector(c_pvector chunkVector);
void C3DAirportClearChunkVector(std::vector<pC3DAirportBlock>& chunkVector);

#endif