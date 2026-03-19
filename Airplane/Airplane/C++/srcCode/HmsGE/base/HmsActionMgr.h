#pragma once

#include "base/Ref.h"
#include "base/HmsAction.h"
#include "base/HmsIdMgr.h"
NS_HMS_BEGIN

struct ActionHashElement;
/*
*用于最多32个id的管理
*/
class CHmsActionMgr : public Ref
{
public:
	CHmsActionMgr();
	~CHmsActionMgr();

	void AddAction(CHmsAction * action, CHmsNode * pNode);

	void RemoveAction(CHmsAction * action);
	void RemoveActionByTag(int nTag, CHmsNode * pNode);

	void RemoveNodeActions(CHmsNode * node);

	void RemoveAllActions();

	void Update(float fDelta);

private:
	void DeleteHashElement(ActionHashElement * element);
	

private:
	CHmsIdMgr				m_mgrID;			//the management of the id
	ActionHashElement	*	m_pActionHashElement;
};

NS_HMS_END