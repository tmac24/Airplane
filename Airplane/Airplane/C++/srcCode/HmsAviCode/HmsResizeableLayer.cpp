#include "HmsResizeableLayer.h"
#include "base/HmsActionTimeRef.h"
#include "base/HmsActionImmediatelyRef.h"
#include "ui/HmsUiEventInterface.h"
USING_NS_HMS;

float CHmsResizeableLayer::s_fActionTime = 0.5f;

class CHmsActionResizeableLayerResizeTo : public CHmsActionTimeFargment
{
public:
	static CHmsActionResizeableLayerResizeTo * Create(float fTimeDuration, const Size & size, const Vec2 & pos, CHmsResizeableLayer * pLayer)
	{
		CHmsActionResizeableLayerResizeTo * ret = new CHmsActionResizeableLayerResizeTo();
		if (ret)
		{
			ret->m_fTimeDuration = fTimeDuration;
			ret->m_sizeEnd = size;
			ret->m_posEnd = pos;
			ret->m_layer = pLayer;
			ret->autorelease();
			return ret;
		}
		return nullptr;
	}

protected:
	virtual void Update(float fDelta) override 
	{
		auto size = m_sizeBegin + m_sizeDelta * fDelta;
		auto pos = m_posBegin + m_posDelta * fDelta;
		if (m_layer)
			m_layer->ResetLayerSize(size, pos);
	}

	virtual void OnActionEnter() override 
	{
		if (m_layer)
		{
			m_sizeBegin = m_layer->GetContentSize();
			m_sizeDelta = m_sizeEnd - m_sizeBegin;
			m_posBegin = m_layer->GetPoistion();
			m_posDelta = m_posEnd - m_posBegin;
		}
	}

	virtual void OnActionFinished() override 
	{
		if (m_layer)
			m_layer->ResizeActionFinished(m_sizeEnd, m_posEnd);
	}

private:
	Size							m_sizeBegin;
	Size							m_sizeEnd;
	Size							m_sizeDelta;
	Vec2							m_posBegin;
	Vec2							m_posEnd;
	Vec2							m_posDelta;
	RefPtr<CHmsResizeableLayer>		m_layer;
};

CHmsResizeableLayer::CHmsResizeableLayer()
	: m_bRemoveSelfAfterAction(false)
{
	
}


CHmsResizeableLayer::~CHmsResizeableLayer()
{

}

bool CHmsResizeableLayer::Init()
{
	if (!CHmsNode::Init())
	{
		return false;
	}

	return true;
}

void CHmsResizeableLayer::ResetLayerSize(const HmsAviPf::Size & size, const HmsAviPf::Vec2 & pos)
{
	this->SetContentSize(size);
	this->SetPosition(pos);
}

void CHmsResizeableLayer::ResetLayerSizeByAction(const HmsAviPf::Size & size, const HmsAviPf::Vec2 & pos, bool bRemoveSelfAfterAction)
{
	StopAllActions();
	auto jumpTo = CHmsActionResizeableLayerResizeTo::Create(s_fActionTime, size, pos, this);
	if (jumpTo)
	{
		this->RunAction(jumpTo);
	}
	m_bRemoveSelfAfterAction = bRemoveSelfAfterAction;
}

void CHmsResizeableLayer::ResizeActionFinished(const HmsAviPf::Size & size, const HmsAviPf::Vec2 & pos)
{
	ResetLayerSize(size, pos);
	if (m_bRemoveSelfAfterAction)
	{
		RemoveFromParent();
		m_bRemoveSelfAfterAction = false;

		auto temp = dynamic_cast<HmsUiEventInterface*>(this);
		if (temp)
		{
			temp->RemoveFromParentInterface();
		}
	}
}

