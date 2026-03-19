#ifndef _3D_BARRIER_MGR__H_
#define _3D_BARRIER_MGR__H_

#include "Camera.h"
#include "3DBarrierBlock.h"
#include "Node.h"
#include "Matrix.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "_Vec3.h"
#include "_Vec2.h"

#ifdef __cplusplus
}
#endif

class C3DBarrierMgr
{
public:
    pCNode m_root;
    CVec4d m_2DProjectOrth;
    int m_lonCount;
    int m_latCount;
    int m_startLon;
    int m_startLat;
    int m_endLon;
    int m_endLat;

    /*需要添加的BarrierLonLatUnit*/
    //c_pvector addChunk ;
    std::vector<pC3DBarrierBlock> addChunk2;
    /*需要删除的BarrierLonLatUnit*/
    //c_pvector delChunk ;
    std::vector<pC3DBarrierBlock> delChunk2;
    /*需要显示的BarrierLonLatUnit*/
    //c_pvector renderChunk; 
    std::vector<pC3DBarrierBlock> renderChunk2;
    /*需要显示的BarrierLonLatUnit*/
    //c_pvector tmpChunk; 
    std::vector<pC3DBarrierBlock> tmpChunk2;

    //c_vector m_v_addChunk ; 
    //c_vector m_v_delChunk ; 
    //c_vector m_v_renderChunk;  
    //c_vector m_v_tmpChunk; 

    CVec3d m_lonlatPos;
    CVec3d m_aircraftLonlat;
    CVec3d m_hpr;
    unsigned int updateNum;
    pCNode  m_terrainMat;
    pCNode m_aircraftIco;

    C3DBarrierMgr(){
        m_root = NULL;
        m_2DProjectOrth = Vec_CreatVec4d(0, 0, 0, 0);
        m_lonCount = 0;
        m_latCount = 0;
        m_startLon = 0;
        m_startLat = 0;
        m_endLon = 0;
        m_endLat = 0;
        m_lonlatPos = Vec_CreatVec3d(0, 0, 0);
        m_aircraftLonlat = Vec_CreatVec3d(0, 0, 0);
        m_hpr = Vec_CreatVec3d(0, 0, 0);;
        updateNum = 0;
        m_terrainMat = NULL;
        m_aircraftIco = NULL;
    }
};

typedef C3DBarrierMgr* pC3DBarrierMgr;


static pC3DBarrierMgr C3DBarrierMgr_Create();

//static int C3DBarrierIsContain(c_pvector chunkLonLatVector, pC3DBarrierBlock chunk);

pC3DBarrierMgr GetC3DBarrierMgr();

void C3DBarrierMgrRangeCalc(CVec3d);
void C3DBarrierMgrUpdateChunkDelete();
void C3DBarrierMgrUpdate();
void C3DBarrierMgrSetLonLat(CVec3d lonlat);
void C3DBarrierMgrProjectOrth(CVec4d vp);

void C3DBarrierUpdateChunkAdd();
void C3DBarrierUpdateChunk();
void C3DBarrierUpdateRender();

#endif