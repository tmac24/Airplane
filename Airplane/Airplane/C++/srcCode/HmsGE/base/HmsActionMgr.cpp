#include "HmsActionMgr.h"
#include "base/uthash.h"
#include "base/HmsVector.h"
NS_HMS_BEGIN

struct ActionHashElement
{
	CHmsNode				 *	pNode;					//id used for the hash
	bool						bIsActionUsing;				//action is current using
	bool						bRemoveFirstAction;		//the first action is remove by themselves action
	Vector<CHmsAction*>			vAction;
	UT_hash_handle				hh;

	ActionHashElement(CHmsNode * node)
	{
		pNode = node;
		bIsActionUsing = false;
		bRemoveFirstAction = false;
		memset(&hh, 0, sizeof(hh));
	}
};

CHmsActionMgr::CHmsActionMgr()
{
	m_pActionHashElement = nullptr;
}

CHmsActionMgr::~CHmsActionMgr()
{

}

void CHmsActionMgr::AddAction(CHmsAction * action, CHmsNode * pNode)
{
	if (action == nullptr || pNode == nullptr)
		return;

	ActionHashElement *element = nullptr;

	HASH_FIND_PTR(m_pActionHashElement, &pNode, element);
	if (!element)
	{
		element = new ActionHashElement(pNode);
		HASH_ADD_PTR(m_pActionHashElement, pNode, element);
	}
	action->SetActionNode(pNode);
	element->vAction.pushBack(action);
}

void CHmsActionMgr::RemoveAction(CHmsAction * action)
{
	ActionHashElement *element = nullptr;
	CHmsNode *target = action->GetActionNode();
	HASH_FIND_PTR(m_pActionHashElement, &target, element);
	if (element)
	{
		auto index = element->vAction.getIndex(action);
		if (index != -1)
		{
			if (index == 0 && element->bIsActionUsing)
			{
				element->bRemoveFirstAction = true;
			}
			else
			{
				element->vAction.erase(index);
			}
		}	
	}
}

void CHmsActionMgr::Update(float fDelta)
{
	ActionHashElement * tempElement = nullptr;
	for (ActionHashElement * curElement = m_pActionHashElement; curElement != nullptr; )
	{
		if (curElement->vAction.size() > 0)
		{
			//only deal with with the first action
			curElement->bIsActionUsing = true;
			auto action = curElement->vAction.at(0);
			if (action)
			{
				action->RunAction(fDelta);

				if (action->IsAccomplish())
					curElement->bRemoveFirstAction = true;

				if (curElement->bRemoveFirstAction)
					curElement->vAction.erase(0);
			}
		}
		curElement->bIsActionUsing = false;

		tempElement = curElement;
		curElement = (ActionHashElement*)curElement->hh.next;
		if (tempElement->vAction.size() == 0)
		{
			DeleteHashElement(tempElement);
		}
	}
}

void CHmsActionMgr::DeleteHashElement(ActionHashElement * element)
{
	if (element)
	{
		HASH_DEL(m_pActionHashElement, element);
		//element->pNode->release();
		delete element;
	}
}

void CHmsActionMgr::RemoveAllActions()
{
	ActionHashElement * tempElement = nullptr;
	for (ActionHashElement * curElement = m_pActionHashElement; curElement != nullptr;)
	{
		tempElement = curElement;
		curElement = (ActionHashElement*)curElement->hh.next;
		RemoveNodeActions(tempElement->pNode);
	}
}

void CHmsActionMgr::RemoveNodeActions(CHmsNode * node)
{
	if (node)
	{
		ActionHashElement *element = nullptr;
		HASH_FIND_PTR(m_pActionHashElement, &node, element);
		if (element)
		{
			if (element->bIsActionUsing)
			{
				element->bRemoveFirstAction = true;
				if (element->vAction.size() > 1)
				{
					auto itBegin = std::next(element->vAction.begin(), 1);
					element->vAction.erase(itBegin, element->vAction.end());
				}
				//remove later
			}
			else
			{
				DeleteHashElement(element);
			}
		}
	}
}

void CHmsActionMgr::RemoveActionByTag(int nTag, CHmsNode * node)
{
	if (node)
	{
		ActionHashElement *element = nullptr;
		HASH_FIND_PTR(m_pActionHashElement, &node, element);
		if (element)
		{
			int index = -1;
			CHmsAction * tempAction = nullptr;
			for (int i=0; i < element->vAction.size(); i++)
			{
				tempAction = element->vAction.at(i);
				if (tempAction && (tempAction->GetTag() == nTag))
				{
					index = i;
					break;
				}
			}

			if (index != -1)
			{
				if (index == 0 && element->bIsActionUsing)
				{
					element->bRemoveFirstAction = true;
				}
				else
				{
					element->vAction.erase(index);
				}
			}
		}
	}
}



NS_HMS_END