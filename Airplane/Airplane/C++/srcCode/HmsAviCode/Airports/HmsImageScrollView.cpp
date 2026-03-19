#include "HmsImageScrollView.h"
#include "HmsAviDisplayUnit.h"
#include "render/HmsTextureCache.h"
#include "display/HmsImageNode.h"
#include "display/HmsStretchImageNode.h"
#include "base/HmsActionTimeRef.h"

NS_HMS_BEGIN

CHmsImageScrollView* CHmsImageScrollView::Create(const Size & sizeShow)
{
	CHmsImageScrollView *ret = new CHmsImageScrollView();
	if (ret && ret->Init(sizeShow))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

CHmsImageScrollView::CHmsImageScrollView()
: HmsUiEventInterface(this)
{

}

CHmsImageScrollView::~CHmsImageScrollView()
{
}

bool CHmsImageScrollView::Init(const Size & sizeShow)
{
	if (!CHmsClippingNode::Init())
	{
		return false;
	}
	SetContentSize(sizeShow);

	m_nodeMask = CHmsDrawNode::Create();
	if (NULL != m_nodeMask.get())
	{
		m_nodeMask->DrawSolidRect(Rect(Vec2(), sizeShow));
		this->setStencil(m_nodeMask);
	}	

	RegisterAllNodeChild();

	return true;
}

bool CHmsImageScrollView::OnPressed(const Vec2 & posOrigin)
{
	m_time.RecordCurrentTime();
	m_posPressStart = posOrigin;
	m_posPanelPressStart = m_pImageNode->GetPoistion();
	return true;
}

void CHmsImageScrollView::OnMove(const Vec2 & posOrigin)
{
	auto delta = posOrigin - m_posPressStart;
	auto newPos = m_posPanelPressStart + delta;
	m_pImageNode->SetPosition(newPos);
}

void CHmsImageScrollView::OnReleased(const Vec2 & posOrigin)
{
	auto delta = posOrigin - m_posPressStart;
	auto deltaTime = m_time.GetElapsed();
	if (deltaTime < 2 && fabsf(delta.y) < 10)
	{
		HmsUiEventInterface::OnPressed(posOrigin);
		HmsUiEventInterface::OnReleased(posOrigin);
	}
	else
	{
		EdgeDetection();
	}
}

void CHmsImageScrollView::AttachImageNode(CHmsImageNode *pNode)
{
	if (m_pImageNode)
	{
		m_pImageNode->RemoveFromParent();
		m_pImageNode = nullptr;
	}
	m_pImageNode = pNode;
	if (nullptr != m_pImageNode)
	{
		this->AddChild(m_pImageNode);
	}	
}

void CHmsImageScrollView::SetImageToCenter()
{
	if (m_pImageNode)
	{
		m_pImageNode->SetPosition(GetUiCenter());
	}
}

void CHmsImageScrollView::EdgeDetection()
{
	float rotation = fabs(m_pImageNode->GetRotation());
	auto imageContentSize = m_pImageNode->GetContentSize();
	auto imageScale = m_pImageNode->GetScale();
	float imageHalfHeight = 0;
	float imageHalfWidth = 0;
	if ((int)(rotation / 90) % 2 == 0)
	{
		imageHalfHeight = imageContentSize.height / 2.0 * imageScale;
		imageHalfWidth = imageContentSize.width / 2.0 * imageScale;
	}
	else
	{
		imageHalfHeight = imageContentSize.width / 2.0 * imageScale;
		imageHalfWidth = imageContentSize.height / 2.0 * imageScale;
	}
	bool bSetPos = false;
	auto pos = m_pImageNode->GetPoistion();
	auto windowHeight = GetContentSize().height;
	auto windowWidth = GetContentSize().width;
	//Y
	auto bottomY = pos.y - imageHalfHeight;
	auto topY = pos.y + imageHalfHeight;
	if (!(bottomY > 0 && topY < windowHeight))
	{
		if (bottomY > 0 || topY < windowHeight)
		{
			if (fabs(topY - windowHeight) < fabs(bottomY))
			{
				pos.y = windowHeight - imageHalfHeight;
				bSetPos = true;
			}
			else
			{
				pos.y = imageHalfHeight;
				bSetPos = true;
			}
		}
	}
	//X
	auto leftX = pos.x - imageHalfWidth;
	auto rightX = pos.x + imageHalfWidth;
	if (!(leftX > 0 && rightX < windowWidth))
	{
		if (leftX > 0 || rightX < windowWidth)
		{
			if (fabs(rightX - windowWidth) < fabs(leftX))
			{
				pos.x = windowWidth - imageHalfWidth;
				bSetPos = true;
			}
			else
			{
				pos.x = imageHalfWidth;
				bSetPos = true;
			}
		}
	}
	if (bSetPos)
	{
		auto moveto = CHmsActionMoveTo::Create(0.3f, pos);
		if (moveto)
		{
			m_pImageNode->RunAction(moveto);
		}		
	}
}

void CHmsImageScrollView::ImageMoveDistance(int x, int y)
{
	if (!m_pImageNode)
	{
		return;
	}
	auto pos = m_pImageNode->GetPoistion();
	pos.x += x;
	pos.y += y;
	m_pImageNode->SetPosition(pos);

	EdgeDetection();
}

NS_HMS_END


