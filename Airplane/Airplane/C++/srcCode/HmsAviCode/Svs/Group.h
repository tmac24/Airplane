#ifndef _GROUP_H_
#define _GROUP_H_

#include "Node.h"
#include "GLHeader.h" 


struct CGroup
{
    pCNode   node;
    CGroup(){
        node = NULL;
    }
};
typedef CGroup* pCGroup;

pCGroup CGroup_Create();

//void CGroup_Draw(pCGroup pnode);
bool CGroup_Destory(pCGroup pnode);

int CGroup_FreeGroup(pCGroup pnode);


#endif