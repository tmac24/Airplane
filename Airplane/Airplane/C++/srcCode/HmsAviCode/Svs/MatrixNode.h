#ifndef _MATRIX_NODE__H_
#define _MATRIX_NODE__H_


#include "Node.h"
#include "GLHeader.h"
#include "Matrix.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "_Vec4.h"

#ifdef __cplusplus
}
#endif

enum EN_CCoord
{
    WGS84 = 0,
    MERCATOR
};// CCoord, *pCCoord;
//typedef CCoord* pCCoord;

//typedef struct _CMatrixTransform
class CMatrixTransform
{
public:
    CMatrixd matrix;
    pCNode node; //本数据附在该节点上
    EN_CCoord coord;
    CVec4d rect;
    int display;
    CMatrixTransform(){
        matrix = CMatrix_Identity();
        node = NULL;
        coord = EN_CCoord::WGS84;
        rect = Vec_CreatVec4d(0, 0, 0, 0);
        display = 0;
    }
};
typedef CMatrixTransform* pCMatrixTransform;

pCMatrixTransform CMatrixTransform_Create();

void CMatrixTransform_Draw(pCMatrixTransform pnode, const HmsAviPf::Mat4d & parentTransform, bool parentFlags);
bool CMatrixTransform_Destory(pCMatrixTransform pnode);

//bool CMatrixTransform_DestoryPtr(pCMatrixTransform *pnode);


#endif