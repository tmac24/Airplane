#include "Drawable.h"
#include "MathCommon.h"
#include "Node.h"
#include "Group.h"
#include "MatrixNode.h"
#include "TerrainTile.h"
#include "TerrainChunk.h"
#include "Log.h"
#include "math/HmsMath.h"
#include <assert.h>


pCNode CNode_CreateNullNode()
{
#if 0
    pCNode node = (pCNode)malloc(sizeof(CNode));
    if (node == 0)
    {
        return 0;
    }
    memset(node, 0, sizeof(CNode));
#endif
    auto node = new CNode;

    //node->childrenArrary = &(node->m_v_childrenArrary);
    //node->parentsArrary = &(node->m_v_parentsArrary);
    //c_vector_create(node->childrenArrary, NULL);
    //c_vector_create(node->parentsArrary, NULL);

    //CNode_BindFunc(node);

    return node;
}

#if 0
unsigned int CNode_GetNodeIndex(CNode* parent, CNode* child)
{
    //if (parent == NULL || parent->childrenArrary == NULL
    //    || c_vector_size(parent->parentsArrary) <= 0 || child == NULL)
    //{
    //    return -1;
    //}
    if (parent == NULL || parent->parentsArray2.size() <= 0 || child == NULL)
    {
        return -1;
    }
    return 0;
}
#endif

//void CNode_BindFunc(struct _CNode *node)
//void CNode_BindFunc(CNode *node)
//{

//}

pCNode CNode_CreateNode(EN_NodeType type)
{
    pCNode fuckNode = CNode_CreateNullNode();
    if (NULL == fuckNode)
    {
        return fuckNode;
    }

    pCMatrixTransform pMtx = NULL;
    pCDrawable draw = NULL;
    pCTerrainTile pTt = NULL;
    pCGroup pgp = NULL;
    pCGroup pnd = NULL;

    switch (type)
    {
    case Group:
        fuckNode->type = EN_NodeType::Group;
        pgp = CGroup_Create();
        fuckNode->pNodeData = pgp;
        pgp->node = fuckNode;
        CNode_SetName(fuckNode, "group");
        break;
    case MatrixTransform:
        fuckNode->type = EN_NodeType::MatrixTransform;
        pMtx = CMatrixTransform_Create();
        fuckNode->pNodeData = pMtx;
        pMtx->node = fuckNode;
        CNode_SetName(fuckNode, "MatrixTransform");
        break;
    case Node:
        fuckNode->type = EN_NodeType::Node;
        pnd = CGroup_Create();//...same as Group
        fuckNode->pNodeData = pnd;
        pnd->node = fuckNode;
        CNode_SetName(fuckNode, "Node");
        break;
    case DrawFunc:
        fuckNode->type = EN_NodeType::DrawFunc;
        CNode_SetName(fuckNode, "DrawFunc");
        break;
    case Geometry:
        fuckNode->type = EN_NodeType::Geometry;
        CNode_SetName(fuckNode, "Geometry");
        break;
    case NT_Drawable:
        fuckNode->type = EN_NodeType::NT_Drawable;
        draw = CDrawable_Create();
        fuckNode->pNodeData = draw;
        draw->node = fuckNode;
        CNode_SetName(fuckNode, "Drawable");
        break;
    case TerrainTile:
        fuckNode->type = EN_NodeType::TerrainTile;
        pTt = CTerrainTile_Create();
        fuckNode->pNodeData = pTt;
        pTt->node = fuckNode;
        CNode_SetName(fuckNode, "TerrainTile");
        break;
    case TerrainChunk:
        fuckNode->type = EN_NodeType::TerrainChunk;
        printf("废弃的 TerrainChunk\n");
        // 			pCTerrainChunk draw = CTerrainChunk_Creat();
        // 			fuckNode->pNodeData = draw;
        // 			draw->m_terrain = fuckNode; 
        // 			fuckNode->type = TerrainChunk;
        CNode_SetName(fuckNode, "TerrainChunk");
        break;
    case BillBoard:
        fuckNode->type = EN_NodeType::BillBoard;
        CNode_SetName(fuckNode, "BillBoard");
        break;
    default:
        break;
    }

    return fuckNode;
}

unsigned int CNode_GetNumberChildren(CNode* ptr)
{
    if (NULL == ptr)
    {
        return 0;
    }
    return ptr->childrenArrary2.size();
}

unsigned int CNode_GetNumberParents(CNode* p)
{
    //if (ptr == NULL || ptr->parentsArrary == NULL)
    //{
    //    return 0;
    //}
    //return  c_vector_size(ptr->parentsArrary);
    if (p == NULL)
    {
        return 0;
    }
    return  p->parentsArray2.size();
}

bool CNode_RemoveChildByIndex(pCNode cur_node, unsigned int id)
{
    if (NULL == cur_node || id >= cur_node->childrenArrary2.size())
    {
        return false;
    }
    cur_node->childrenArrary2.erase(cur_node->childrenArrary2.begin() + id);
    return true;
}

void CNode_AddChild(CNode* node, CNode* child)
{
    if (node == NULL || child == NULL)
    {
        return;
    }

    node->childrenArrary2.push_back(child);
    child->parentsArray2.push_back(node);
    child->ref_count++;
}

bool CNode_DetachChild(CNode* pNode, CNode* pChild)
{
    if (NULL == pNode)
    {
        return false;
    }

    if (NULL == pChild)
    {
        LogWrite(LOG_LEVEL_FATAL, "怎么会移除空指针？？？？？？\n");
        printf("怎么会移除空指针？？？？？？\n");
        //system("pause");//xyh 不执行就好,不要暂停它.
        return false;
    }

    for (unsigned int i = 0; i < pNode->childrenArrary2.size(); ++i)
    {
        auto pc = pNode->childrenArrary2[i];
        if (pc == pChild)
        {
            pNode->childrenArrary2.erase(pNode->childrenArrary2.begin() + i);
            pChild->ref_count--;

            for (unsigned int i = 0; i < pChild->parentsArray2.size(); ++i)
            {
                auto par = pChild->parentsArray2[i];
                if (par == pNode)
                {
                    pChild->parentsArray2.erase(pChild->parentsArray2.begin() + i);
                    return true;
                }
            }
            return true;
        }
    }
    return false;//not found , remove failed
}

#if 0
int CNode_RemoveChild_B(CNode* node, CNode* child)
{
    int res = SCENCE_NODE_REMOVE_FAILED;
    if (child == NULL)
    {
        return 1;
    }

    {
        //c_iterator finditer = c_vector_find(node->childrenArrary, child);
        //c_iterator enditer = c_vector_end(node->childrenArrary);
        //if (!ITER_EQUAL(finditer, enditer))
        //{
        //    c_vector_erase(node->childrenArrary, finditer);
        //}
        //else
        //{
        //    return res = SCENCE_NODE_REMOVE_FAILED;
        //}
        auto finditer1 = std::find(node->childrenArrary2.begin(), node->childrenArrary2.end(), child);
        if (finditer1 != node->childrenArrary2.end())
        {
            node->childrenArrary2.erase(finditer1);
        }
        else
        {
            return res = SCENCE_NODE_REMOVE_FAILED;
        }
        child->ref_count--;

        //finditer = c_vector_find(child->parentsArrary, node);
        //enditer = c_vector_end(child->parentsArrary);
        //if (!ITER_EQUAL(finditer, enditer))
        //{
        //    c_vector_erase(child->parentsArrary, enditer);//jy: old bug
        //}
        //else
        //{
        //    return res = SCENCE_NODE_REMOVE_FAILED;
        //}
        auto finditer2 = std::find(child->parentsArray2.begin(), child->parentsArray2.end(), node);
        if (finditer2 != child->parentsArray2.end())
        {
            child->parentsArray2.erase(finditer2);
        }
        else
        {
            return res = SCENCE_NODE_REMOVE_FAILED;
        }

        res = SCENCE_NODE_REMOVE_SUCCESS;
    }

    return	res;
}
#endif

pCNode CNode_GetChildByIndex(CNode* node, unsigned int i)
{
    if (NULL == node || i >= node->childrenArrary2.size())
    {
        return NULL;
    }
    return node->childrenArrary2[i];
}

bool CNode_Destory(pCNode cur_node)
{
    /*  如果
    当前节点为NULL，
    当前节点还有引用，
    当前节点还有孩子节点，
    则不释放当前节点空间
    */
    if (cur_node == NULL)
    {
        return false;
    }

    if (cur_node->ref_count > 0)
    {
        return false;
    }

    if (cur_node->childrenArrary2.size() > 0)
    {
        return false;
    }

    cur_node->childrenArrary2.clear();
    cur_node->parentsArray2.clear();

    if (cur_node->pNodeData != NULL)
    {
        printf("节点:%s的三维节点未释放\n", cur_node->name);
        return false;
    }

    delete cur_node;
    cur_node = NULL;
    return true;
}

/*释放三维数据节点
1释放成功
0释放失败
*/
bool FreeScenceData(pCNode cur_node)
{
    if (cur_node == NULL)
    {
        return true;
    }

    bool ret = false;

    /*处理当前节点*/
    switch (cur_node->type)
    {
    case Group:
        ret = CGroup_Destory((pCGroup)(cur_node->pNodeData));
        break;
    case Node:
        ret = CGroup_Destory((pCGroup)(cur_node->pNodeData));//...same as above
        break;
    case MatrixTransform:
        ret = CMatrixTransform_Destory((pCMatrixTransform)(cur_node->pNodeData));
        break;
    case BillBoard:

        break;
    case NT_Drawable:
        ret = CDrawable_Destory((pCDrawable)(cur_node->pNodeData));
        break;
    case TerrainTile:
        ret = CTerrainTile_Destory((pCTerrainTile)(cur_node->pNodeData));
        break;
    case TerrainChunk:
        //if (CTerrainChunk_Destroy((pCTerrainChunk)(curChildNode->pNodeData)))
        //{
        //	curChildNode->pNodeData = NULL;
        //	return SCENCE_NODE_DATA_RELEASE_SUCCESS;
        //}
        //printf("废弃的 TerrainChunk FreeScenceData\n");
        //return  SCENCE_NODE_DATA_RELEASE_FAILED;
        ret = true;
        break;
    default:
        assert(NULL);//impossible here
        break;
    }

    if (ret)
    {
        cur_node->pNodeData = NULL;
    }

    return ret;
}

bool CNode_FreeScenceData(pCNode cur_node)
{
    if (cur_node == NULL)
    {
        return true;
    }

    unsigned int childNum = CNode_GetNumberChildren(cur_node);

    if (childNum <= 0)
    {
        /*cur_node is already a leaf. so, free cur_node */
        return FreeScenceData(cur_node);
    }

    unsigned char* childResState = (unsigned char*)malloc(sizeof(unsigned char)*childNum);
    memset(childResState, 0, sizeof(unsigned char)*childNum);

    bool perChildNodeIsReleaseSuccess = true;

    for (unsigned int i = 0; i < childNum; ++i)
    {
        pCNode curChildNode = cur_node->childrenArrary2[i];
        if (curChildNode == NULL)
        {
            printf("当前孩子节点为null，Current Child Node is NULL\n");
            continue;
        }
        perChildNodeIsReleaseSuccess = true;
        /*处理当前节点*/
        switch (curChildNode->type)
        {
        case  Group:/*group 节点的三维数据没有，所以直接继续向下draw*/
            /*向下递归释放所有孩子节点*/
        case  MatrixTransform:
            /*向下递归释放所有孩子节点*/
        case  BillBoard:
            /*向下递归释放所有孩子节点*/
        case  Node:
            /*向下递归释放所有孩子节点*/
        case  DrawFunc:
            /*向下递归释放所有孩子节点*/
        case  Geometry:
            /*向下递归释放所有孩子节点*/
        case NT_Drawable:
            /*向下递归释放所有孩子节点*/
        case TerrainTile:
            /*pCTerrainTile draw = CTerrainTile_Creat();
            fuckNode->pNodeData = draw;
            draw->node = fuckNode;
            fuckNode->type = TerrainTile;
            CNode_SetName(fuckNode,"TerrainTile");*/
            /*向下递归释放所有孩子节点*/
        case TerrainChunk:
            perChildNodeIsReleaseSuccess = CNode_FreeScenceData(curChildNode);
            break;
        default:
            printf("这里有一个CNode_FreeScenceData奇葩节点,unknow node\n");
            break;
        };

        if (perChildNodeIsReleaseSuccess)
        {
            curChildNode = NULL;
        }

        childResState[i] = (char)(perChildNodeIsReleaseSuccess ? 1 : 0);
    }

    /*检查所有孩子节点是否被释放*/
    unsigned char res = 1;
    for (unsigned int j = 0; j < childNum; j++)
    {
        /*倒着来取数据*/
        unsigned char childRelease = *(childResState + childNum - 1 - j);
        if (childRelease)
        {
        }
        else
        {
            printf("scence data not be release!\n");
        }
        res = res&&childRelease;
    }
    free(childResState);

    if (res)
    {
        perChildNodeIsReleaseSuccess = FreeScenceData(cur_node);
    }
    else
    {
        printf("else scence data not be release!\n");
        perChildNodeIsReleaseSuccess = false;
    }

    return perChildNodeIsReleaseSuccess;
}

bool CNode_FreeScenceNode(pCNode cur_node)
{
    unsigned int childNum = CNode_GetNumberChildren(cur_node);

    if (childNum <= 0)
    {
        return CNode_Destory(cur_node);
    }

    unsigned char* childResState = (unsigned char*)malloc(sizeof(unsigned char)*childNum);
    memset(childResState, 0, sizeof(unsigned char)*childNum);

    bool perChildNodeIsReleaseSuccess = true;

    for (unsigned int i = 0; i < cur_node->childrenArrary2.size(); ++i)
    {
        pCNode& curChildNode = cur_node->childrenArrary2[i];
        if (curChildNode == NULL)
        {
            printf("当前孩子节点为null，Current Child Node is NULL\n");
            continue;
        }

        curChildNode->ref_count--;

        /*当前节点的所有子节点 当释放成功*/
        perChildNodeIsReleaseSuccess = true;
        /*处理当前节点*/
        switch (curChildNode->type)
        {
        case Group:/*group 节点的三维数据没有，所以直接继续向下draw*/
            /*向下递归释放所有孩子节点*/
        case MatrixTransform:
            /*向下递归释放所有孩子节点*/
        case BillBoard:
            /*向下递归释放所有孩子节点*/
        case Node:
            /*向下递归释放所有孩子节点*/
        case DrawFunc:
            /*向下递归释放所有孩子节点*/
        case Geometry:
            /*向下递归释放所有孩子节点*/
        case NT_Drawable:
            /*向下递归释放所有孩子节点*/
        case TerrainTile:
            /*向下递归释放所有孩子节点*/
        case TerrainChunk:
            /*向下递归释放所有孩子节点*/
            perChildNodeIsReleaseSuccess = CNode_FreeScenceNode(curChildNode);
            break;
        default:
            printf("这里有一个CNode_FreeScenceNode奇葩节点,unknow node\n");
            break;
        };

        if (!perChildNodeIsReleaseSuccess)
        {
            curChildNode->ref_count++;
        }
        else
        {
            curChildNode = NULL;
        }

        childResState[i] = (char)(perChildNodeIsReleaseSuccess ? 1 : 0);
    }

    /*检查所有孩子节点是否被释放*/
    unsigned char res = 1;
    for (unsigned int j = 0; j < childNum; j++)
    {
        /*倒着来取数据*/
        unsigned char childRelease = *(childResState + childNum - 1 - j);
        if (childRelease)
        {
            CNode_RemoveChildByIndex(cur_node, childNum - 1 - j);
        }
        else
        {
            printf("节点%s的第%d个节点未释放完全\n", cur_node->name, childNum - 1 - j);
        }

        res = res&&childRelease;
    }
    free(childResState);

    if (res)
    {
        if (perChildNodeIsReleaseSuccess = CNode_Destory(cur_node))
        {
            cur_node = NULL;
        }
    }
    else
    {
        perChildNodeIsReleaseSuccess = false;
    }

    return perChildNodeIsReleaseSuccess;
}

//#define _NODE_FREE_PRINTF_

//************************************
// Method:    CNode_FreeNode
// FullName:  CNode_FreeNode
// Access:    public 
// Desc:      
// Returns:   int , 1: release success, 0: release failed
// Qualifier:
// Parameter: pCNode cur_node
// Author:    yan.jiang
//************************************
bool CNode_FreeNode(pCNode node)
{
    if (node == NULL)
    {
        return 1;
    }

    bool res0 = 0;
    bool res1 = 0;
    char name[256] = { 0 };

    memcpy_op(name, node->name, TAWS_MIN(sizeof(name)-1, strlen(node->name)));
#ifdef _NODE_FREE_PRINTF_
    printf("释放节点%s三维数据      开始\n", name);
#endif

    res0 = CNode_FreeScenceData(node);
    if (!res0)
    {
        printf("释放节点 %s 三维数据失败\n", name);
    }
    //Attention! Beware of memory leaks.
    //if res0 == 0 and continue do the following CNode_FreeScenceNode
    res1 = CNode_FreeScenceNode(node);
    if (!res1)
    {
        printf("释放节点 %s 失败\n", name);
    }

    return res0 && res1;
}

#if 0
int CNode_FreeNodeFLPfuck(pCNode* _node, int flp)
{
    pCNode cur_node = *_node;
    unsigned char perChildNodeIsReleaseSuccess = 1, res = 0;
    /*孩子节点个数*/
    unsigned int childNum = CNode_GetNumberChildren(cur_node);
    unsigned char* childResState = 0;
    unsigned int iterNum = 0;

    /*父节点个数*/
    int parentsNum = CNode_GetNumberParents(cur_node);
    /*所有孩子节点*/
    //c_pvector  childrenArrary = cur_node->childrenArrary;
    /*迭代指针*/
    //c_iterator iter, beg = c_vector_begin(childrenArrary), end = c_vector_end(childrenArrary);
    if (childNum)
    {
        childResState = (unsigned char*)malloc(sizeof(unsigned char)*childNum);
    }
    /*叶节点，没有孩子节点了，直接释放他*/
    if (childNum <= 0)
    {
        /*父节点移除*/
        CNode_RemoveFormParents(cur_node);
        res = FreeScenceData(cur_node);
        cur_node->pNodeData = 0;
        /*然后释放节点*/
        perChildNodeIsReleaseSuccess = CNode_Destory(cur_node);

        *_node = cur_node = NULL;
        if (childResState)
        {
            free(childResState);
            childResState = 0;
        }
        return perChildNodeIsReleaseSuccess;
    }

    //for (iter = beg; !ITER_EQUAL(iter, end); ITER_INC(iter))
    for (unsigned int i = 0; i < cur_node->childrenArrary2.size(); ++i)
    {
        pCNode curChildNode = NULL;
        //curChildNode = (pCNode)(ITER_REF(iter));
        curChildNode = cur_node->childrenArrary2[i];
        if (curChildNode == NULL)
        {
            res = childResState[iterNum] = CNode_FreeNodeFLP(curChildNode, 1);
        }
        iterNum++;
    }

    return res;
}

int CNode_FreeNodeFLP(pCNode _node, int flp)
{
    pCNode cur_node = _node;
    unsigned char perChildNodeIsReleaseSuccess = 1, res = 0;
    /*孩子节点个数*/
    unsigned int childNum = CNode_GetNumberChildren(cur_node);
    unsigned char* childResState = 0;
    unsigned int iterNum = 0;

    /*父节点个数*/
    int parentsNum = CNode_GetNumberParents(cur_node);
    /*所有孩子节点*/
    //c_pvector  childrenArrary = cur_node->childrenArrary;
    /*迭代指针*/
    //c_iterator iter, beg = c_vector_begin(childrenArrary), end = c_vector_end(childrenArrary);
    if (childNum)
    {
        childResState = (unsigned char*)malloc(sizeof(unsigned char)*childNum);
    }
    /*叶节点，没有孩子节点了，直接释放他*/
    if (childNum <= 0)
    {
        /*父节点移除*/

        CNode_RemoveFormParents(cur_node);
        res = FreeScenceData(cur_node);
        cur_node->pNodeData = 0;
        /*然后释放节点*/
        perChildNodeIsReleaseSuccess = CNode_Destory(cur_node);

        _node = cur_node = NULL;
        if (childResState)
        {
            free(childResState);
            childResState = 0;
        }
        return perChildNodeIsReleaseSuccess;
    }

    //for (iter = beg; !ITER_EQUAL(iter, end); ITER_INC(iter))
    for (unsigned int i = 0; i < cur_node->childrenArrary2.size(); ++i)
    {
        pCNode curChildNode = NULL;
        //curChildNode = (pCNode)(ITER_REF(iter));
        curChildNode = cur_node->childrenArrary2[i];
        if (curChildNode == NULL)
        {
            res = childResState[iterNum] = CNode_FreeNodeFLP(curChildNode, 1);
        }
        iterNum++;
    }

    return res;
}
#endif


void CNode_SetName(CNode* p, char *name)
{
    memset(p->name, 0, 64);
    memcpy_op(p->name, name, HMS_MIN(63, strlen(name)));
}

int CNode_RemoveChildBegToNum(pCNode cur_node, unsigned int beg, unsigned int num)
{
#if 0
    c_pvector childrenArrary = cur_node->childrenArrary;
    int pos = beg + num - 1;
    int end = beg;
    if (pos > (int)(c_vector_size(childrenArrary)) || num <= 0)//jy: old bug
    {
        return -1;
    }
    while (pos >= end)
    {
        c_vector_erase(childrenArrary, c_vector_idfind(childrenArrary, (unsigned int)pos));
        --pos;
    }
#endif
    int pos = beg + num - 1;
    int end = beg;
    if (pos >= (int)(cur_node->childrenArrary2.size()) || num <= 0)
    {
        return -1;
    }
    while (pos >= end)
    {
        cur_node->childrenArrary2.erase(cur_node->childrenArrary2.begin() + pos);
        --pos;
    }
    return 1;
}

int CNode_RemoveAllChildAndDelete(pCNode cur_node)
{
    unsigned int res = 1;
    unsigned int childNum = cur_node->childrenArrary2.size();
    unsigned char perChildNodeIsReleaseSuccess = 1;

    unsigned char* childResState = (unsigned char*)malloc(sizeof(unsigned char)*childNum);

    memset(childResState, 0, sizeof(unsigned char)*childNum);

    for (unsigned int i = 0; i < childNum; ++i)
    {
        pCNode curChildNode = NULL;
        curChildNode = cur_node->childrenArrary2[i];

        if (curChildNode == NULL)
        {
            printf("CNode_RemoveAllChildAndDelete 当前孩子节点为null，Current Child Node is NULL\n");
            continue;
        };
        curChildNode->ref_count--;
        perChildNodeIsReleaseSuccess = CNode_FreeNode(curChildNode) ? 1 : 0;
        if (!perChildNodeIsReleaseSuccess)
        {
            curChildNode->ref_count++;
        }
        else
        {
            curChildNode = NULL;
        }
        childResState[i] = perChildNodeIsReleaseSuccess;
    }

    /*检查所有孩子节点是否被释放*/
    {
        unsigned int j = 0;
        for (j = 0; j < childNum; j++)
        {
            /*倒着来取数据*/
            unsigned char childRelease = *(childResState + childNum - 1 - j);
            if (childRelease)
            {
                CNode_RemoveChildByIndex(cur_node, childNum - 1 - j);//jy: mem leak
            }
            else
            {
                printf("节点%s的第%d个节点未释放完全\n", cur_node->name, childNum - 1 - j);
            }
            res = res&&childRelease;
        }
        free(childResState);
    }
    return res;
}

void CNode_RenderChild(pCNode cur_node, const HmsAviPf::Mat4d & parentTransform, bool parentFlags)
{
    if (!cur_node)
    {
        LogWrite(LOG_LEVEL_FATAL, "rendering  null node \n");
        //system("pause");//xyh
        return;
    }

    auto flags = cur_node->ProcessParentFlags(parentTransform, parentFlags);

    for (unsigned int i = 0; i < cur_node->childrenArrary2.size(); ++i)
    {
        pCNode childNode = cur_node->childrenArrary2[i];
        if (childNode == nullptr)
        {
            continue;
        }

        pCTerrainTile pterraintile = nullptr;

        switch (childNode->type)
        {
        case Model:
            break;
        case Group:/*group 节点的三维数据没有，所以直接继续向下draw*/
            CNode_RenderChild(childNode, cur_node->m_transformModeView, flags);
            break;
        case MatrixTransform:
            CMatrixTransform_Draw((pCMatrixTransform)(childNode->pNodeData), cur_node->m_transformModeView, flags);
            break;
        case BillBoard:
            break;
        case Node:
            CNode_RenderChild(childNode, cur_node->m_transformModeView, flags);
            break;
        case DrawFunc:
            break;
        case Geometry:
            break;
        case NT_Drawable:
            CDrawable_Draw((pCDrawable)(childNode->pNodeData));
            break;
        case TerrainTile:
            pterraintile = (pCTerrainTile)(childNode->pNodeData);
            if (pterraintile != nullptr)
            {
                CNode_RenderChild(pterraintile->node, cur_node->m_transformModeView, flags);
            }
            break;
        case TerrainChunk:
            break;
        default:
            break;
        }
    }
}

void CNode_RemoveFromParentsAndFreeNode(pCNode cur_node)
{
    CNode_RemoveFormParents(cur_node);
    CNode_FreeNode(cur_node);
}

void CNode_RemoveFormParents(pCNode cur_node)
{
    if (cur_node == NULL)
    {
        return;
    }

    //c_pvector  parentsArrary = cur_node->parentsArrary;
    //c_iterator iter, beg = c_vector_begin(parentsArrary), end = c_vector_end(parentsArrary);

    //for (iter = beg; !ITER_EQUAL(iter, end); ITER_INC(iter))
    //{
    //    pCNode pnode = NULL;
    //    pnode = (pCNode)(ITER_REF(iter));
    //    if (pnode == NULL)
    //    {
    //        continue;
    //    }
    //    CNode_RemoveChild(pnode, cur_node);
    //}
    for (unsigned int i = 0; i < cur_node->parentsArray2.size(); ++i)
    {
        auto pnode = cur_node->parentsArray2[i];
        if (NULL == pnode)
        {
            continue;
        }
        CNode_DetachChild(pnode, cur_node);
    }
}

bool CNode_IsContainChild(CNode* node, CNode* child)
{
    if (child == NULL)
    {
        return false;
    }

    auto find_iter = std::find(node->childrenArrary2.begin(), node->childrenArrary2.end(), child);
    if (find_iter == node->childrenArrary2.end())
    {
        return false;
    }

    return true;
}


#if 0
int CNode_RemoveAllChildAndDelete_FreeNode(pCNode *node)
{
    pCMatrixTransform mat;
    pCDrawable draw;

    switch ((*node)->type)
    {
    case Node:
        CNode_DestoryPtr(node);
        break;
    case Group:
        CNode_DestoryPtr(node);
        break;
    case MatrixTransform:
        mat = (pCMatrixTransform)((*node)->pNodeData);
        CMatrixTransform_DestoryPtr(&mat);
        CNode_DestoryPtr(node);
        return (int)*node;
        break;
    case Camera:
        break;
    case DrawFunc:
        break;
    case Geometry:
        break;
    case NT_Drawable:
        draw = (pCDrawable)(*node)->pNodeData;
        CDrawable_DestoryPtr(&draw);
        CNode_DestoryPtr(node);
        return (int)*node;
        break;
    case TerrainTile:
        break;
    case TerrainChunk:
        break;
    case BillBoard:
        break;
    default:
        assert(NULL);//impossiable here
    }
    return (int)NULL;
}
#endif

#if 0
int CNode_RemoveAllChildAndDelete_n(pCNode *p)
{
    unsigned int res = 1;
    unsigned int iterNum = 0;
    unsigned int indexId = 0;
    pCNode cur_node = *p;
    //unsigned int childNum = c_vector_size(cur_node->childrenArrary);
    unsigned int childNum = cur_node->childrenArrary2.size();
    unsigned char perChildNodeIsReleaseSuccess = 0;
    unsigned char* childResState = (unsigned char*)malloc(sizeof(unsigned char)*childNum);
    //c_pvector  vectors = cur_node->childrenArrary;
    //c_iterator iter, beg = c_vector_begin(vectors), end = c_vector_end(vectors);
    unsigned int index = 0;
    memset(childResState, 255, sizeof(unsigned char)*childNum);
    //如果没有子节点则直接释放当前节点内容
    if (childNum == 0)
    {
        if (!CNode_RemoveAllChildAndDelete_FreeNode(p))
        {
            free(childResState);
        }
        return (int)*p;
    }

    //for (iter = beg; !ITER_EQUAL(iter, end); ITER_INC(iter))
    for (unsigned int i = 0; i < childNum; ++i)
    {
        //获得每一个子节点
        pCNode curChildNode = NULL;
        //curChildNode = (pCNode)(ITER_REF(iter));
        curChildNode = cur_node->childrenArrary2[i];

        if (curChildNode == NULL)
        {
            printf("CNode_RemoveAllChildAndDelete 当前孩子节点为null，Current Child Node is NULL\n");
            continue;
        }
        //先将他的引用减少1
        curChildNode->ref_count--;
        //然后释放节点
        if (!CNode_RemoveAllChildAndDelete_n(&curChildNode))
        {
            childResState[iterNum] = perChildNodeIsReleaseSuccess;
        }
        iterNum++;
    }

    /*检查所有孩子节点是否被释放*/
    {
        unsigned int j = 0;
        for (j = 0; j < childNum; j++)
        {
            /*倒着来取数据*/
            unsigned char childRelease = *(childResState + childNum - 1 - j);
            if (!childRelease)
            {
                CNode_RemoveChildIndex(cur_node, childNum - 1 - j);
            }
            else
            {
                printf("节点%s的第%d个节点未释放完全\n", cur_node->name, childNum - 1 - j);
            }
            res = res&&childRelease;
        }
    }
    free(childResState);
    return res;
}
#endif

#if 0
int CNode_FreeNode_n(pCNode *p)
{
    pCNode cur_node = *p;
    int res = 0;
    char name[256] = { 0 };
    if (cur_node == NULL)
    {
        return 1;
    }
    memcpy_op(name, cur_node->name, strlen(cur_node->name));
#ifdef _NODE_FREE_PRINTF_	
    printf("释放节点%s三维数据      开始\n", name);
#endif

    if (res = CNode_FreeScenceData(cur_node))
    {
        //res =  FreeScenceData(cur_node);
#ifdef _NODE_FREE_PRINTF_
        printf("释放节点%s三维数据结束-->%s\n", name, res ? "成功" : "失败");
        printf("释放节点%s          开始\n", name, res ? "成功" : "失败");
#endif
        if (res = CNode_FreeScenceNode(cur_node))
        {
            //res = CNode_Destory(cur_node);
#ifdef _NODE_FREE_PRINTF_
            printf("释放节点%s    结束-->%s\n", name, res ? "成功" : "失败");
#endif
        }
        else
        {
            printf("释放节点%s    结束-->%s\n", name, res ? "成功" : "失败");
        }
    }
    else
    {
        printf("释放节点%s三维数据结束：%s,未释放三维节点\n", name, res ? "成功" : "失败");
    }
    return res;
}

int CNode_DestoryPtr(pCNode *p)
{
    pCNode cur_node = *p;
    /*  如果
    当前节点为NULL，
    当前节点还有引用，
    当前节点还有孩子节点，
    则不释放当前节点空间
    */
    //LogWrite(LOG_LEVEL_DEBUG_FP, "CNode_Destory(pCNode cur_node) %s  :%s,%d\n", cur_node->name, __FILE__, __LINE__);
    if (cur_node == NULL)
    {
        return SCENCE_NODE_RELEASE_FAILED;
    }

    if (cur_node->ref_count >= 1)
    {
        //LogWrite(LOG_LEVEL_DEBUG_FP, "CNode_Destory node %s ref_count is %d,so don't release :%s,%d\n", cur_node->name, cur_node->ref_count, __FILE__, __LINE__);
        return SCENCE_NODE_RELEASE_FAILED;
    }
    //if (c_vector_size(cur_node->childrenArrary) >= 1)
    if (cur_node->childrenArrary2.size() >= 1)
    {
        //LogWrite(LOG_LEVEL_DEBUG_FP, "当前节点%s还有%d个孩子节点:%s,%d\n", cur_node->name, c_vector_size(cur_node->childrenArrary), __FILE__, __LINE__);
        return SCENCE_NODE_RELEASE_FAILED;
    }

    //c_vector_clear(cur_node->childrenArrary);
    //c_vector_clear(cur_node->parentsArrary);
    cur_node->childrenArrary2.clear();
    cur_node->parentsArray2.clear();
    //LogWrite(LOG_LEVEL_DEBUG_FP, "c_vector_destroy(cur_node->childrenArrary) (%s) over :%s,%d\n", cur_node->name, __FILE__, __LINE__);
    //c_vector_destroy(cur_node->parentsArrary);
    //c_vector_destroy(cur_node->childrenArrary);

    //free(cur_node->childrenArrary);
    //free(cur_node->parentsArrary);
    //cur_node->childrenArrary = 0;
    //cur_node->parentsArrary = 0;

    if (cur_node->pNodeData != NULL)
    {
        printf("节点:%s的三维节点未释放\n", cur_node->name);
        return SCENCE_NODE_RELEASE_FAILED;
    }
    cur_node->pNodeData = NULL;
    //free(cur_node);
    delete cur_node;
    cur_node = *p = NULL;
    return SCENCE_NODE_RELEASE_SUCCESS;
}
#endif

bool CNode::ProcessParentFlags(const HmsAviPf::Mat4d& parentTransform, bool parentFlags)
{
    bool flags = parentFlags || m_transformUpdated;

    if (flags)
    {
        m_transformModeView = m_transformModeView * parentTransform;
    }

    m_transformUpdated = false;

    return flags;
}
