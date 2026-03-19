#include "Drawable.h"
#include "Group.h"
#include "MatrixNode.h"


pCGroup CGroup_Create()
{
    //pCGroup group = (pCGroup)malloc(sizeof(CGroup));
    //if (group == NULL)
    //{
    //    printf("pCGroup group = (pCGroup)malloc(sizeof(CGroup))  failed\n");
    //    exit(1);
    //}
    //memset(group, 0, sizeof(CGroup));
    auto group = new CGroup;
    return group;
}

#if 0
void CGroup_Draw(pCGroup pnode)
{
    if (pnode->node != NULL) {
        CNode_RenderChild(pnode->node, TODO, TODO);
    }
}
#endif

bool CGroup_Destory(pCGroup pnode)
{
    if (pnode)
    {
        delete pnode;
    }
    return true;
}

//
//int CGroup_FreeGroup(pCGroup pnode)
//{
//#if TAWS_USE_CSTL_VECTOR
//
//
//	pCNode cur_node =  pnode->node;
//	if (cur_node == NULL)
//	{
//		return 1;
//	}
//	int i = 0 ;
//	int childNum = CNode_GetNumberChildren(cur_node);
//	c_pvector  childrenArrary= cur_node->childrenArrary;
//	CIterator iter;
//	printf("释放节点:\n");
//	for (iter = CVector_Begin(childrenArrary);iter != CVector_End(childrenArrary);iter=CVector_Next(childrenArrary,iter))
//	{
//		pCNode childNode = NULL; 
//		enum NodeType nodetype;
//		CVector_Iter_val(childrenArrary,iter,&childNode);		 
//		if (childNode ==NULL)
//		{
//			continue;
//		}
//		/*先从当前节点移除该子节点*/
//		cur_node->RemoveChild(cur_node,childNode); 
//		nodetype  = childNode->type;
//		printf("\t%s\t",childNode->name);
//		switch(nodetype)
//		{
//		case  Group:/*group 节点的三维数据没有，所以直接继续向下draw*/
//			{  
//				pCGroup pGroup = (pCGroup)(childNode->pNodeData);
//				if (pGroup) 
//					CGroup_FreeGroup(pGroup);
//			}
//			break;
//		case  MatrixTransform:
//			{
//				pCMatrixTransform pMatNode = (pCMatrixTransform)(childNode->pNodeData);
//				if (pMatNode!=NULL)
//				{	
//					CMatrixTransform_Destory(pMatNode);
//				}
//			}break;
//		case  Node:
//			{
//				CNode_FreeNode(childNode);
//			}
//			break;
//		case  DrawFunc:
//			{
//				CNode_Destory(childNode);
//			}
//			break;
//		case  Geometry:
//			break;
//		case Drawable:
//			{
//				pCDrawable draw = (pCDrawable)(childNode->pNodeData);
//				if (draw!=NULL)
//				{	
//					CDrawable_Draw(draw);
//				} 
//			}
//			break;
//		default:
//			NULL;
//			break; 
//		};
//	}  
//	CNode_Destory(cur_node);
//	return 1;
//#else
//	pCNode cur_node =  pnode->node;
//	int i = 0;
//	int childNum = CNode_GetNumberChildren(cur_node);
//	pCVector  childrenArrary = cur_node->childrenArrary;
//	CIterator iter;
//	printf("释放节点:\n");
//	for (iter = CVector_Begin(childrenArrary); iter != CVector_End(childrenArrary); iter = CVector_Next(childrenArrary, iter))
//	{
//		pCNode childNode = NULL;
//		enum NodeType nodetype;
//		CVector_Iter_val(childrenArrary, iter, &childNode);
//		if (childNode == NULL)
//		{
//			continue;
//		}
//		/*先从当前节点移除该子节点*/
//		cur_node->RemoveChild(cur_node, childNode);
//		nodetype = childNode->type;
//		printf("\t%s\t", childNode->name);
//		switch (nodetype)
//		{
//		case  Group:/*group 节点的三维数据没有，所以直接继续向下draw*/
//		{
//									pCGroup pGroup = (pCGroup)(childNode->pNodeData);
//									if (pGroup)
//										CGroup_FreeGroup(pGroup);
//		}
//			break;
//		case  MatrixTransform:
//		{
//													 pCMatrixTransform pMatNode = (pCMatrixTransform)(childNode->pNodeData);
//													 if (pMatNode != NULL)
//													 {
//														 CMatrixTransform_Destory(pMatNode);
//													 }
//		}break;
//		case  Node:
//		{
//								CNode_FreeNode(childNode);
//		}
//			break;
//		case  DrawFunc:
//		{
//										CNode_Destory(childNode);
//		}
//			break;
//		case  Geometry:
//			break;
//		case Drawable:
//		{
//									 pCDrawable draw = (pCDrawable)(childNode->pNodeData);
//									 if (draw != NULL)
//									 {
//										 CDrawable_Draw(draw);
//									 }
//		}
//			break;
//		default:
//			NULL;
//			break;
//		};
//	}
//	CNode_Destory(cur_node);
//	return 1;
//#endif
//}
