#include "MatrixNode.h"
#include "Drawable.h"
#include "Group.h"
#include "MathCommon.h"
#include "CoordinateSystemMgr.h"
#include "Coordinate.h"
#include "Matrix.h"


extern GLuint MatrixM2ID;

pCMatrixTransform CMatrixTransform_Create()
{
    //pCMatrixTransform p = (pCMatrixTransform)malloc(sizeof(CMatrixTransform));
    //if (p == NULL)
    //{
    //    printf("pCMatrixNode p = (pCMatrixNode)malloc(sizeof(CMatrixNode))------>failed\n");
    //    exit(1);
    //}
    //memset(p, 0, sizeof(CMatrixTransform));
    auto p = new CMatrixTransform;
    return p;
}

void CMatrixTransform_Draw(pCMatrixTransform pnode, const HmsAviPf::Mat4d & parentTransform, bool parentFlags)
{
    if (pnode == nullptr)
    {
        return;
    }
    CMatrix transMat = pnode->matrix;
    CVec3d transPos = CMatrix_GetTrans(&transMat);

    //jy:修改节点矩阵的平移部分，变换矩阵原来的原点在地心，现在原点改为当前摄像机所在的1度整经度1度整纬度的区块的中央且海拔为0的位置。当然，view矩阵也以此点为参考点。
    transPos = CCoordinateMgr_GetWGS84RelativePoint(transPos);//转换后transPos是相对于当前摄像机所在的1度整经度1度整纬度的区块的中央且海拔为0的位置
    CMatrix_SetTransVec3d(&transMat, transPos);

    CMatrixf matrixf = CMatrix_d2f(transMat);
    glUniformMatrix4fv(MatrixM2ID, 1, GL_FALSE, &matrixf._mat[0][0]);

    if (!pnode->display)
    {
        CNode_RenderChild(pnode->node, parentTransform, parentFlags);
    }
}

bool CMatrixTransform_Destory(pCMatrixTransform pnode)
{
    if (pnode)
    {
        delete pnode;
    }
    return true;
}

#if 0
bool CMatrixTransform_DestoryPtr(pCMatrixTransform *pnode)
{
    pCMatrixTransform node = *pnode;
    if (node != NULL)
    {
        //free(node);
        delete node;
        node = NULL;
        *pnode = 0;
        return true;
    }
    else
    {
        return false;
    }
}
#endif

