#ifndef _AABBOX_H_
#define _AABBOX_H_

#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

#include "_Vec3.h"

#ifdef __cplusplus
}
#endif

typedef struct _aabbox
{
    CVec3d maxedge;		//top left
    CVec3d minedge;		//down right
    int    inited;
}Aabbox, *pAabbox;

pAabbox Aabbox_Create();

pAabbox Aabbox_CreatValue(pCVec3d min, pCVec3d max);

void Aabbox_Destory(pAabbox box);

void Aabbox_AddInternalPoint(pAabbox box, pCVec3d point);

void Aabbox_GetEdges(pAabbox box, pCVec3d edges);

CVec3d Aabbox_GetCenter(pAabbox box);

CVec3d Aabbox_GetExtent(pAabbox box);

int Aabbox_IsPointInside(pAabbox box, CVec3d point);

int Aabbox_intersectBox(pAabbox box1, pAabbox box2);

void Aabbox_PushVertexs(pAabbox box, std::vector<CVec3d>& v);



#endif