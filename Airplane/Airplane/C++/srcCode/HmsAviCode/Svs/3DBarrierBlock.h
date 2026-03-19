#ifndef _3D_BARRIER_BLOCK__H_
#define _3D_BARRIER_BLOCK__H_

#include "MatrixNode.h"
#include "Node.h"
#include "BarrierCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "_Vec3.h"
#include "_Vec4.h"

#ifdef __cplusplus
}
#endif

struct C3DBarrierBlock
{
    /*经纬高*/
    CVec3d m_lonlat;
    /*经纬度转墨卡托位置*/
    CVec3d m_lonlatPos;
    /*本快的根节点*/
    pCNode m_root;
    /*所有障碍物数据  <BarrierParameters>*/
    //c_pvector  m_Barriernfo; 
    std::vector<pCBarrierParameters> m_Barriernfo2;
    /*所有障碍物 数据和节点关联record*/
    //c_pvector  m_pBarrierRecord;
    std::vector<pC3DBarrierRecord> m_pBarrierRecord2;
    //c_vector  m_v_Barriernfo;  
    //c_vector  m_v_pBarrierRecord;
    C3DBarrierBlock(){
        m_lonlat = Vec_CreatVec3d(0, 0, 0);
        m_lonlatPos = Vec_CreatVec3d(0, 0, 0);
        m_root = NULL;
    }
};
typedef C3DBarrierBlock* pC3DBarrierBlock;

pC3DBarrierBlock C3DBarrierBlock_Create();

void C3DBarrierBlock_Destory(pC3DBarrierBlock blk);

void C3DBarrierBlockReadBarrierData(pC3DBarrierBlock blk);

void C3DBarrierBlockCreatBarrierNode(pC3DBarrierBlock blk);

//pCNode C3DBarrierBlockCreatBarrierDrawable(pCBarrierParameters ruw);

pCNode C3DBarrierBlockCreatBarrierDrawableBillboard(pCBarrierParameters bp);

//pCNode C3DBarrierBlockCreatBarrierDrawable3D(pCBarrierParameters bp);

void C3DBarrierBlockSetLonlat(pC3DBarrierBlock blk, CVec3d lon);

void  C3DBarrierBlockUpdate(pC3DBarrierBlock blk);

#endif
