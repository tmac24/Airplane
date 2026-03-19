#include "HmsScratchPadClipping.h"


CHmsScratchPadClipping::CHmsScratchPadClipping()
: HmsUiEventInterface(this)
, m_eventMode(ClippingEventMode::E_DRAG)
, m_bEdgeBy00(true)
{

}

CHmsScratchPadClipping::~CHmsScratchPadClipping()
{

}

bool CHmsScratchPadClipping::Init()
{
	if (!CHmsClippingNode::Init())
	{
		return false;
	}
	return true;
}

bool CHmsScratchPadClipping::OnPressed(const Vec2 & posOrigin)
{
	switch (m_eventMode)
	{
	case ClippingEventMode::E_TRANSMIT:
		return HmsUiEventInterface::OnPressed(posOrigin);

		break;
	case ClippingEventMode::E_DRAG:
	case ClippingEventMode::E_DRAG_CLICK:
		m_posPressStart = posOrigin;
#if _NAV_SVS_LOGIC_JUDGE
		if (m_pChildNode)
#endif
		m_posChildPressStart = m_pChildNode->GetPoistion();
		return true;
		break;
	default:
		break;
	}

	return true;
}

void CHmsScratchPadClipping::OnMove(const Vec2 & posOrigin)
{
	switch (m_eventMode)
	{
	case ClippingEventMode::E_TRANSMIT:
		HmsUiEventInterface::OnMove(posOrigin);
		
		break;
	case ClippingEventMode::E_DRAG:
	case ClippingEventMode::E_DRAG_CLICK:
	{
		auto delta = posOrigin - m_posPressStart;
		auto newPos = m_posChildPressStart + delta;
#if _NAV_SVS_LOGIC_JUDGE
		if (m_pChildNode)
#endif
		m_pChildNode->SetPosition(newPos);
	}
		break;
	default:
		break;
	}
}

void CHmsScratchPadClipping::OnReleased(const Vec2 & posOrigin)
{
	switch (m_eventMode)
	{
	case ClippingEventMode::E_TRANSMIT:
		HmsUiEventInterface::OnReleased(posOrigin);
		
		break;
	case ClippingEventMode::E_DRAG:
		EdgeDetection();
		break;
	case ClippingEventMode::E_DRAG_CLICK:
	{
		auto delta = posOrigin - m_posPressStart;
		if (fabsf(delta.y) < 10)
		{
			HmsUiEventInterface::OnPressed(posOrigin);
			HmsUiEventInterface::OnReleased(posOrigin);
		}
		EdgeDetection();
	}
		break;
	default:
		break;
	}
}

void CHmsScratchPadClipping::AddChildWithEvent(CHmsNode * child, bool bEvent)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!child)
		return;
#endif
	m_pChildNode = child;

	CHmsClippingNode::AddChild(child);

	if (bEvent)
	{
		auto tempChild = dynamic_cast<HmsUiEventInterface*>(child);
		if (tempChild)
		{
			this->AddChildInterface(tempChild);
		}
	}
}

void CHmsScratchPadClipping::EdgeDetection()
{
	if (m_bEdgeBy00)
	{
		EdgeDetection_0_0();
	}
	else
	{
		EdgeDetection_05_05();
	}
}

void CHmsScratchPadClipping::EdgeDetection_0_0()
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pChildNode)
		return;
#endif
	auto imageContentSize = m_pChildNode->GetContentSize();
	auto imageScale = m_pChildNode->GetScale();
	float imageHeight = imageContentSize.height * imageScale;
	float imageWidth = imageContentSize.width * imageScale;

	bool bSetPos = false;
	auto pos = m_pChildNode->GetPoistion();
	auto windowHeight = this->GetContentSize().height;
	auto windowWidth = this->GetContentSize().width;
	//Y
	auto bottomY = pos.y;
	auto topY = pos.y + imageHeight;
	if (!(bottomY > 0 && topY < windowHeight))
	{
		if (bottomY > 0 || topY < windowHeight)
		{
			if (fabsf(topY - windowHeight) < fabsf(bottomY))
			{
				pos.y = windowHeight - imageHeight;
				bSetPos = true;
			}
			else
			{
				pos.y = 0;
				bSetPos = true;
			}
		}
	}
	//X
	auto leftX = pos.x;
	auto rightX = pos.x + imageWidth;
	if (!(leftX > 0 && rightX < windowWidth))
	{
		if (leftX > 0 || rightX < windowWidth)
		{
			if (fabsf(rightX - windowWidth) < fabsf(leftX))
			{
				pos.x = windowWidth - imageWidth;
				bSetPos = true;
			}
			else
			{
				pos.x = 0;
				bSetPos = true;
			}
		}
	}
	if (bSetPos)
	{
		auto moveto = CHmsActionMoveTo::Create(0.3f, pos);
		m_pChildNode->RunAction(moveto);
	}
}
void CHmsScratchPadClipping::EdgeDetection_05_05()
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pChildNode)
		return;
#endif
	float rotation = fabsf(m_pChildNode->GetRotation());
	auto imageContentSize = m_pChildNode->GetContentSize();
	auto imageScale = m_pChildNode->GetScale();
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
	auto pos = m_pChildNode->GetPoistion();
	auto windowHeight = GetContentSize().height;
	auto windowWidth = GetContentSize().width;
	//Y
	auto bottomY = pos.y - imageHalfHeight;
	auto topY = pos.y + imageHalfHeight;
	if (!(bottomY > 0 && topY < windowHeight))
	{
		if (bottomY > 0 || topY < windowHeight)
		{
			if (fabsf(topY - windowHeight) < fabsf(bottomY))
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
			if (fabsf(rightX - windowWidth) < fabsf(leftX))
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
		m_pChildNode->RunAction(moveto);
	}
}

void CHmsScratchPadClipping::SetEventMode(ClippingEventMode mode)
{
	m_eventMode = mode;
}

void CHmsScratchPadClipping::ChildMoveDistance(int x, int y)
{
	if (!m_pChildNode)
	{
		return;
	}
	auto pos = m_pChildNode->GetPoistion();
	pos.x += x;
	pos.y += y;
	m_pChildNode->SetPosition(pos);

	EdgeDetection();
}
