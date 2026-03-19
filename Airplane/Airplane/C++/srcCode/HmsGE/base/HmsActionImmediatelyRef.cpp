#include "HmsActionImmediatelyRef.h"
#include "HmsNode.h"
NS_HMS_BEGIN


CHmsActionHide * CHmsActionHide::Create()
{
	CHmsActionHide * ret = new CHmsActionHide();
	if (ret)
	{
		ret->autorelease();
		return ret;
	}
	return nullptr;
}

void CHmsActionHide::Update(float fDelta)
{
	if (m_pNode)
	{
		m_pNode->SetVisible(false);
	}
}


CHmsActionShow * CHmsActionShow::Create()
{
	CHmsActionShow * ret = new CHmsActionShow();
	if (ret)
	{
		
		ret->autorelease();
		return ret;
	}
	return nullptr;
}

void CHmsActionShow::Update(float fDelta)
{
	if (m_pNode)
	{
		m_pNode->SetVisible(true);
	}
}


CHmsActionRemove * CHmsActionRemove::Create()
{
	CHmsActionRemove * ret = new CHmsActionRemove();
	if (ret)
	{
		
		ret->autorelease();
		return ret;
	}
	return nullptr;
}

void CHmsActionRemove::Update(float fDelta)
{
	if (m_pNode)
	{
		m_pNode->RemoveFromParent();
	}
}


CHmsActionRemoveAndCleanUp * CHmsActionRemoveAndCleanUp::Create()
{
	CHmsActionRemoveAndCleanUp * ret = new CHmsActionRemoveAndCleanUp();
	if (ret)
	{
		
		ret->autorelease();
		return ret;
	}
	return nullptr;
}

void CHmsActionRemoveAndCleanUp::Update(float fDelta)
{
	if (m_pNode)
	{
		m_pNode->RemoveFromParentAndCleanup(true);
	}
}


CHmsActionCallback * CHmsActionCallback::Create(const std::function<void()> & callback)
{
	CHmsActionCallback * ret = new CHmsActionCallback();
	if (ret)
	{
		
		ret->m_func = callback;
		ret->autorelease();
		return ret;
	}
	return nullptr;
}

void CHmsActionCallback::Update(float fDelta)
{
	if (m_func)
	{
		m_func();
	}
}


CHmsActionCallbackNode * CHmsActionCallbackNode::Create(const std::function<void(CHmsNode*)> & callback)
{
	CHmsActionCallbackNode * ret = new CHmsActionCallbackNode();
	if (ret)
	{
		
		ret->m_func = callback;
		ret->autorelease();
		return ret;
	}
	return nullptr;
}

void CHmsActionCallbackNode::Update(float fDelta)
{
	if (m_func)
	{
		m_func(m_pNode);
	}
}

NS_HMS_END