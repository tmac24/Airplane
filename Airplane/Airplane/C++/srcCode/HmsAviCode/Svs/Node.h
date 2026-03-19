#ifndef _NODE__H_
#define _NODE__H_

#include <vector>
#include "GLHeader.h"
#include "NodeDefine.h"
#include <stdio.h>
#include <stdlib.h>
#include "NavSvs/mathd/Mat4d.h"


/*******************************************************************************
名称：场景节点 struct
功能：单向链表存储父节点和孩子节点，树的结构组织
参数：无
时间：4/7/2015 xiafei
版本：1.0
注意：
返回值：
*******************************************************************************/
#if 0
typedef struct _CNode
{
    /*引用计数*/
    unsigned int ref_count;
    /*暂时没用的一个ID */
    unsigned int id;
    /*父节点*/
    //c_pvector  parentsArrary; 
    std::vector<pCNode> parentsArray2;
    /*子节点*/
    //c_pvector  childrenArrary; 
    std::vector<pCNode> childrenArrary2;
    /*父节点*/
    //c_vector  m_v_parentsArrary; 
    /*子节点*/
    //c_vector  m_v_childrenArrary;  
    /*节点三维类型*/
    enum NodeType type;
    /*节点名*/
    char name[64];
    /*节点三维数据*/
    void * pNodeData;
}CNode, *pCNode;
#endif
class CNode;
typedef CNode* pCNode;

class CNode
{
public:
    bool ProcessParentFlags(const HmsAviPf::Mat4d& parentTransform, bool parentFlags);

    /*引用计数*/
    unsigned int ref_count;
    /*暂时没用的一个ID */
    unsigned int id;
    /*父节点*/
    //c_pvector  parentsArrary; 
    std::vector<pCNode> parentsArray2;
    /*子节点*/
    //c_pvector  childrenArrary; 
    std::vector<pCNode> childrenArrary2;
    /*父节点*/
    //c_vector  m_v_parentsArrary; 
    /*子节点*/
    //c_vector  m_v_childrenArrary;  
    /*节点三维类型*/
    EN_NodeType type;
    /*节点名*/
    char name[64];
    /*节点三维数据*/
    void * pNodeData;
    HmsAviPf::Mat4d m_transformModeView;    // mode view transform of the node
    bool    m_transformUpdated;         // Whether or not the Transform object was updated since the last frame

    CNode()
        :m_transformUpdated(true)
    {
        ref_count = 0;
        id = 0;
        type = EN_NodeType::Node;
        memset(name, 0, 64);
        pNodeData = NULL;
    }
};

/*******************************************************************************
名称：CNode_GetNumberChildren
功能：获取子节点数目
参数：无
时间:  18:10:41 4/8/2015
用户 : xiafei
版本：1.0
注意：
返回值：
*******************************************************************************/
unsigned int CNode_GetNumberChildren(CNode* p);

/*******************************************************************************
名称：CNode_GetNumberParents
功能：获取父节点数目
参数：无
时间:  18:11:00 4/8/2015
用户 : xiafei
版本：1.0
注意：
返回值：
*******************************************************************************/
unsigned int CNode_GetNumberParents(CNode* p);

/*******************************************************************************
名称：CNode_AddChild
功能：添加节点
参数：无
时间:  16:18:18 4/8/2015
用户 : xiafei
版本：1.0
注意：
返回值：1成功 0失败
*******************************************************************************/
void CNode_AddChild(CNode* node, CNode* child);
/*******************************************************************************
名称：CNode_RemoveChild
功能：移除节点
参数：无
时间:  16:18:37 4/8/2015
用户 : xiafei
版本：1.0
注意：
返回值：1 成功  0失败
*******************************************************************************/
bool CNode_DetachChild(CNode* pNode, CNode* pChild);

/*******************************************************************************
名称：获取node
功能：
参数：无
时间:  16:18:51 4/8/2015
用户 : xiafei
版本：1.0
注意：
返回值：
*******************************************************************************/
pCNode CNode_GetChildByIndex(CNode* node, unsigned int i);

/*******************************************************************************
名称：CNode_GetNodeId
功能：获取子节点的id
参数：无
时间:  16:51:21 4/8/2015
用户 : xiafei
版本：1.0
注意：
返回值：-1不存在
*******************************************************************************/
//static unsigned int  CNode_GetNodeIndex(CNode* parent, CNode* child);

/*******************************************************************************
名称：CNode_BindFunc
功能：节点的函数绑定 内部用外部用不到
参数：无
时间:  16:19:11 4/8/2015
用户 : xiafei
版本：1.0
注意：
返回值：void
*******************************************************************************/
//void  CNode_BindFunc(struct _CNode *node);
//void  CNode_BindFunc(CNode *node);


/*******************************************************************************
名称：创建一个null的node
功能：
参数：无
时间：4/7/2015 xiafei
版本：1.0
注意：
返回值：
*******************************************************************************/
static pCNode CNode_CreateNullNode();


pCNode CNode_CreateNode(EN_NodeType type);


/*
释放节点，-1：当前节点为NULL
*/
bool CNode_FreeNode(pCNode node);


/*
移除孩子节点，遍历孩子节点寻找
*/
//static int CNode_removeChild(pCNode cur_node, pCNode child_node);
/*移除孩子节点，按照编号移除*/

/*******************************************************************************
名称：
功能：
参数：无
时间：4/7/2015 xiafei
版本：1.0
注意：可能会内存泄露 ，先获取这个节点，然后移除，移除后考虑free内存空间问题
返回值：-1 cur_node isNULL， 0：success
*******************************************************************************/
bool CNode_RemoveChildByIndex(pCNode cur_node, unsigned int id);


void CNode_RenderChild(pCNode cur_node, const HmsAviPf::Mat4d & parentTransform, bool parentFlags);

void CNode_SetName(CNode*, char *);

bool CNode_Destory(pCNode cur_node);

int CNode_RemoveChildBegToNum(pCNode cur_node, unsigned int beg, unsigned int num);

int CNode_RemoveAllChildAndDelete(pCNode cur_node);

//int CNode_FreeNodeFLP(pCNode cur_node, int flp);

//int CNode_FreeNodeFLPfuck(pCNode *cur_node, int flp);

void CNode_RemoveFormParents(pCNode cur_node);
void CNode_RemoveFromParentsAndFreeNode(pCNode cur_node);

bool CNode_IsContainChild(CNode* node, CNode* child);

//返回0为成功
int CNode_RemoveAllChildAndDelete_n(pCNode *p);
//int CNode_FreeNode_n(pCNode *p);
//返回0为成功
//int CNode_RemoveAllChildAndDelete_FreeNode(pCNode *p);

//释放成功为1 失败为0
//int CNode_DestoryPtr(pCNode *p);



#endif/*end of def*/
