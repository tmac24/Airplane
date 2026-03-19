#include "HmsUiEventInterface.h"
#include "base/HmsEventListenerTouch.h"
#include "../HmsAviDisplayUnit.h"
#include "display/HmsLabel.h"
#include "HmsGlobal.h"
#define CHECK_RELEASE_POS 0

NS_HMS_BEGIN

HmsUiEventInterface::HmsUiEventInterface(CHmsNode * node)
: m_node(node)
, m_pParentUiInterface(nullptr)
, m_pVectorMutiPoint(nullptr)
{
	m_bDisable = false;
	m_bReorderNeeded = false;
	m_eUiEventType = HMS_UI_EVENT_TYPE::none_ui_event;
}

HmsUiEventInterface::~HmsUiEventInterface()
{
	RemoveAllChildInterface();
	RemoveFromParentInterface();
	HMS_SAFE_DELETE(m_pVectorMutiPoint);
}

void HmsUiEventInterface::RegisterTouchEvent(int nEventOrder)
{
	auto listener = EventListenerTouchOneByOne::create();

	listener->onTouchBegan = [=](HmsAviPf::Touch * touch, HmsAviPf::CHmsEvent * event)
	{
		if (!IsActive())
		{
			return false;
		}

		auto posOrg = Touch2UiPosition(touch);
// 		auto posAr = m_node->ConvertOriginPointToAR(posOrg);
        auto rect = GetRectByOriginUI();
		if (rect.containsPoint(posOrg))
		{
			if (this->OnPressed(posOrg))
			{
				m_eUiEventType = HMS_UI_EVENT_TYPE::single_ui_event;
				return true;
			}
		}
		return false;
	};

	listener->onTouchMoved = [=](HmsAviPf::Touch * touch, HmsAviPf::CHmsEvent * event)
	{
		auto pos = Touch2UiPosition(touch);
		if (m_eUiEventType == HMS_UI_EVENT_TYPE::single_ui_event)
		{
			this->OnMove(pos);
		}
	};

	listener->onTouchEnded = [=](HmsAviPf::Touch * touch, HmsAviPf::CHmsEvent * event)
	{
		auto pos = Touch2UiPosition(touch);

		if (m_eUiEventType == HMS_UI_EVENT_TYPE::single_ui_event)
		{
#if CHECK_RELEASE_POS
			auto rect = m_node->GetRectByOrigin();
			if (rect.containsPoint(pos))
#else
			if (true)
#endif
			{
				this->OnReleased(pos);
			}
			else
			{
				this->OnCancel(pos);
			}
			m_eUiEventType = HMS_UI_EVENT_TYPE::none_ui_event;
		}

	};

	auto m_pDU = HmsAviPf::CHmsAviDisplayUnit::GetInstance();
	auto _eventDispatcher = m_pDU->GetEventDispatcher();

	//if (nEventOrder < 0)
	{
		listener->setSwallowTouches(true);
	}
	_eventDispatcher->AddEventListener(listener, this->GetNode(), nEventOrder);
}

void HmsUiEventInterface::FillMultiTouch(const std::vector<Touch*> touchs)
{
	if (m_pVectorMutiPoint)
	{
		for (unsigned int i = 0; i < touchs.size(); i++)
		{
			Touch *newt = touchs[i];

			auto it = m_pVectorMutiPoint->begin();
			for (; it != m_pVectorMutiPoint->end(); it++)
			{
				if (it->Id == newt->GetID())
				{
					it->touch = newt;
					it->uiPoint = Touch2UiPosition(newt);
				}
			}
		}
	}

}

std::vector<UiEventPoint> HmsUiEventInterface::FillMultiTouchAndReturnNewFill(const std::vector<Touch*> touchs)
{
	std::vector<UiEventPoint> vUep;

	if (m_pVectorMutiPoint)
	{
		for (unsigned int i = 0; i < touchs.size(); i++)
		{
			Touch *newt = touchs[i];

			auto it = m_pVectorMutiPoint->begin();
			for (; it != m_pVectorMutiPoint->end(); it++)
			{
				if (it->Id == newt->GetID())
				{
					it->touch = newt;
					it->uiPoint = Touch2UiPosition(newt);
					vUep.push_back(*it);
				}
			}
		}
	}
	return vUep;
}

bool HmsUiEventInterface::FillMultiTouchByCheck(const std::vector<Touch*> touchs)
{
	bool bUpdate = false;
	if (m_pVectorMutiPoint)
	{
		auto rect = GetRectByOriginUI();

		for (unsigned int i = 0; i < touchs.size(); i++)
		{
			Touch *newt = touchs[i];

			auto pos = Touch2UiPosition(newt);
			if (rect.containsPoint(pos))
			{
				auto it = m_pVectorMutiPoint->begin();
				for (; it != m_pVectorMutiPoint->end(); it++)
				{
					if (it->Id == newt->GetID())
						break;
				}

				bUpdate = true;

				if (it != m_pVectorMutiPoint->end())
				{
					it->touch = newt;
					it->uiPoint = pos;
				}
				else
				{
					UiEventPoint uep;
					uep.Id = newt->GetID();
					uep.touch = newt;
					uep.uiPoint = pos;
					m_pVectorMutiPoint->push_back(uep);
				}
			}
		}
	}
	return bUpdate;
}

UiEventPoint * HmsUiEventInterface::GetFilledUiEventPoint(int nID)
{
	if (m_pVectorMutiPoint)
	{
		auto it = m_pVectorMutiPoint->begin();
		for (; it != m_pVectorMutiPoint->end(); it++)
		{
			if (it->Id == nID)
			{
				return &*it;
			}
		}
	}
	return nullptr;
}

void HmsUiEventInterface::RemoveFilledUiEventPoint(int nID)
{
	if (m_pVectorMutiPoint)
	{
		
		auto it = m_pVectorMutiPoint->begin();
		for (; it != m_pVectorMutiPoint->end(); it++)
		{
			if (it->Id == nID)
			{
				m_pVectorMutiPoint->erase(it);
				break;
			}
		}
	}
}

void HmsUiEventInterface::RegisterMutiTouchEvent(int nEventOrder)
{
	if (nullptr == m_pVectorMutiPoint)
	{
		m_pVectorMutiPoint = new std::vector<UiEventPoint>;
	}
	m_pVectorMutiPoint->clear();
	
	auto listener = EventListenerTouchAllAtOnce::create();

	listener->onTouchesBegan = [=](const std::vector<Touch*> touchs, CHmsEvent* event)
	{
		if ((!IsActive()) || touchs.empty())
		{
			return;
		}
		
		bool bUpdate = FillMultiTouchByCheck(touchs);

		if (bUpdate)
		{
			if (HMS_UI_EVENT_TYPE::none_ui_event == m_eUiEventType)
			{
				if (m_pVectorMutiPoint->size() == 1)
				{
					auto & uep = m_pVectorMutiPoint->at(0);
					uep.bActive = this->OnPressed(uep.uiPoint);
					if (uep.bActive)
						m_eUiEventType = HMS_UI_EVENT_TYPE::single_ui_event;
				}
				else if (m_pVectorMutiPoint->size() > 1)
				{
					if (this->OnMutiPressed(*m_pVectorMutiPoint))
					{
						m_eUiEventType = HMS_UI_EVENT_TYPE::multi_ui_event;
					}
					else
					{
						for (auto & c : *m_pVectorMutiPoint)
						{
							auto & uep = c;
							uep.bActive = this->OnPressed(uep.uiPoint);
							if (uep.bActive)
							{
								m_eUiEventType = HMS_UI_EVENT_TYPE::single_ui_event;
								break;
							}
						}
					}
				}
			}
			else if (HMS_UI_EVENT_TYPE::single_ui_event == m_eUiEventType)
			{
				if (this->OnMutiPressed(*m_pVectorMutiPoint))
				{
					m_eUiEventType = HMS_UI_EVENT_TYPE::multi_ui_event;
					for (auto & c : *m_pVectorMutiPoint)
					{
						if (c.bActive)
						{
							this->OnCancel(c.uiPoint);
						}
					}
				}
			}
			else if (HMS_UI_EVENT_TYPE::multi_ui_event == m_eUiEventType)
			{
				this->OnMutiPressed(*m_pVectorMutiPoint);
			}
		}

		if (m_eUiEventType != HMS_UI_EVENT_TYPE::none_ui_event)
		{
			event->StopPropagation();
		}
	};

	listener->onTouchesMoved = [=](const std::vector<Touch*> touchs, HmsAviPf::CHmsEvent * event)
	{
		if (0 == touchs.size())
		{
			return;
		}

		if (m_eUiEventType == HMS_UI_EVENT_TYPE::single_ui_event)
		{
			for (auto c:touchs)
			{
				auto uep = GetFilledUiEventPoint(c->GetID());
				if (uep && uep->bActive)
				{
					uep->uiPoint = Touch2UiPosition(c);
					this->OnMove(uep->uiPoint);
				}
			}
		}
		else if (m_eUiEventType == HMS_UI_EVENT_TYPE::multi_ui_event)
		{
			FillMultiTouch(touchs);
			if (m_pVectorMutiPoint->size())
			{
				this->OnMutiMove(*m_pVectorMutiPoint);
			}
		}
	};

	listener->onTouchesEnded = [=](const std::vector<Touch*> touchs, HmsAviPf::CHmsEvent * event)
	{
		if (0 == touchs.size())
		{
			return;
		}

		if (m_eUiEventType == HMS_UI_EVENT_TYPE::single_ui_event)
		{
			for (auto c : touchs)
			{
				auto uep = GetFilledUiEventPoint(c->GetID());
				if (uep)
				{
					if (uep->bActive)
					{
						uep->uiPoint = Touch2UiPosition(c);
#if CHECK_RELEASE_POS
						auto rect = m_node->GetRectByOrigin();
						if (rect.containsPoint(uep->uiPoint))
#else
						if (true)
#endif
						{
							this->OnReleased(uep->uiPoint);
						}
						else
						{
							this->OnCancel(uep->uiPoint);
						}			
					}
					
					RemoveFilledUiEventPoint(uep->Id);

					m_eUiEventType = HMS_UI_EVENT_TYPE::none_ui_event;
				}
			}
		}
		else if (m_eUiEventType == HMS_UI_EVENT_TYPE::multi_ui_event)
		{
			auto vNew = FillMultiTouchAndReturnNewFill(touchs);
			if (vNew.size())
			{
				this->OnMutiReleased(vNew);
				for (auto & c : vNew)
				{
					RemoveFilledUiEventPoint(c.Id);
				}
			}

			if (0 == m_pVectorMutiPoint->size())
			{
				this->OnMutiEnd();
				m_eUiEventType = HMS_UI_EVENT_TYPE::none_ui_event;
			}

		}
		else
		{
			for (auto c : touchs)
			{
				RemoveFilledUiEventPoint(c->GetID());
			}
		}

		if (m_vUiEventInterface.size() == 0)
		{
			m_eUiEventType = HMS_UI_EVENT_TYPE::none_ui_event;
		}
	};

	auto m_pDU = HmsAviPf::CHmsAviDisplayUnit::GetInstance();
	auto _eventDispatcher = m_pDU->GetEventDispatcher();

	_eventDispatcher->AddEventListener(listener, this->GetNode(), nEventOrder);
}

bool HmsUiEventInterface::IsActive()
{
	bool bRet = false;

	if (m_node && m_node->IsVisible() && !m_bDisable)
	{
		bRet = true;
	}

	return bRet;
}

bool HmsUiEventInterface::OnPressed(const Vec2 & posOrigin)
{
	bool bRet = false;
	if (m_vUiEventInterface.size())
	{
		ShortChildInterface();

		for (auto c : m_vUiEventInterface)
		{
			if (c && c->m_node && (c->m_node->IsUpdateAble()))
			{
				auto posRelate = GetUiOriginPostion(posOrigin, c);
                auto rect = GetRectByOriginUI(c->m_node);
				if (rect.containsPoint(posRelate))
				{
					if (bRet = c->OnPressed(posRelate))
					{
						c->m_eUiEventType = HMS_UI_EVENT_TYPE::single_ui_event;
						break;
					}
					else
					{
						OnClick();
					}
				}
			}
		}
	}
	return bRet;
}

void HmsUiEventInterface::OnMove(const Vec2 & posOrigin)
{
	if (m_vUiEventInterface.size())
	{
		for (auto c : m_vUiEventInterface)
		{
			if (c && (c->m_eUiEventType == HMS_UI_EVENT_TYPE::single_ui_event))
			{
				auto posRelate = GetUiOriginPostion(posOrigin, c);
				c->OnMove(posRelate);
				break;
			}
		}
	}
}

void HmsUiEventInterface::OnReleased(const Vec2 & posOrigin)
{
	if (m_vUiEventInterface.size())
	{
		for (auto c : m_vUiEventInterface)
		{
			if (c && (c->m_eUiEventType == HMS_UI_EVENT_TYPE::single_ui_event))
			{
				auto posRelate = GetUiOriginPostion(posOrigin, c);
#if CHECK_RELEASE_POS
				auto rect = c->m_node->GetRectByOrigin();
				if (rect.containsPoint(posRelate))
#else
				if (true)
#endif
				{
					c->OnReleased(posRelate);
				}
				else
				{
					c->OnCancel(posRelate);
				}

				c->m_eUiEventType = HMS_UI_EVENT_TYPE::none_ui_event;
				break;
			}
		}
	}
}

void HmsUiEventInterface::OnCancel(const Vec2 & posOrigin)
{
	if (m_vUiEventInterface.size())
	{
		for (auto c : m_vUiEventInterface)
		{
			if (c && (c->m_eUiEventType == HMS_UI_EVENT_TYPE::single_ui_event))
			{
				auto posRelate = GetUiOriginPostion(posOrigin, c);
				c->OnCancel(posRelate);
				c->m_eUiEventType = HMS_UI_EVENT_TYPE::none_ui_event;
				break;
			}
		}
	}
}

bool HmsUiEventInterface::OnMutiPressed(const std::vector<UiEventPoint> &pos)
{
	bool bRet = false;
	if (m_vUiEventInterface.size())
	{
		ShortChildInterface();

		for (auto c : m_vUiEventInterface)
		{
			if (c && c->m_node && (c->m_node->IsVisible()))
			{
				std::vector<UiEventPoint> containPoints;
				for (UiEventPoint posOrigin : pos)
				{
					auto posRelate = GetUiOriginPostion(posOrigin.uiPoint, c);
                    auto rect = GetRectByOriginUI(c->m_node);
					if (rect.containsPoint(posRelate))
					{
						posOrigin.uiPoint = posRelate;
						containPoints.push_back(posOrigin);
					}
				}
				if (containPoints.size() > 1)
				{
					if (bRet = c->OnMutiPressed(containPoints))
					{
						c->m_eUiEventType = HMS_UI_EVENT_TYPE::multi_ui_event;
						break;
					}
					
				}
			}
		}
	}
	return bRet;
}

void HmsUiEventInterface::OnMutiMove(const std::vector<UiEventPoint> &pos)
{
	if (m_vUiEventInterface.size())
	{
		for (auto c : m_vUiEventInterface)
		{
			if (c && (c->m_eUiEventType == HMS_UI_EVENT_TYPE::multi_ui_event))
			{
				std::vector<UiEventPoint> containPoints;
				for (UiEventPoint posOrigin : pos)
				{
					auto posRelate = GetUiOriginPostion(posOrigin.uiPoint, c);
					posOrigin.uiPoint = posRelate;
					containPoints.push_back(posOrigin);
				}
				c->OnMutiMove(containPoints);
				break;
			}
		}
	}
}

void HmsUiEventInterface::OnMutiReleased(const std::vector<UiEventPoint> &pos)
{
	if (m_vUiEventInterface.size())
	{
		for (auto c : m_vUiEventInterface)
		{
			if (c && (c->m_eUiEventType == HMS_UI_EVENT_TYPE::multi_ui_event))
			{
				std::vector<UiEventPoint> containPoints;
				for (UiEventPoint posOrigin : pos)
				{
					auto posRelate = GetUiOriginPostion(posOrigin.uiPoint, c);
					posOrigin.uiPoint = posRelate;
					containPoints.push_back(posOrigin);
				}
				c->OnMutiReleased(containPoints);
				
				break;
			}
		}
	}
}

void HmsUiEventInterface::OnMutiEnd()
{
	if (m_vUiEventInterface.size())
	{
		for (auto c : m_vUiEventInterface)
		{
			if (c && (m_eUiEventType == HMS_UI_EVENT_TYPE::multi_ui_event))
			{
				c->OnMutiEnd();
				c->m_eUiEventType = HMS_UI_EVENT_TYPE::none_ui_event;
				break;
			}
		}
	}
}

int HmsUiEventInterface::RegisterAllNodeChild()
{
	int nRet = 0;
	if (m_node)
	{
		auto & child = m_node->GetChildren();
		for (auto c : child)
		{
			HmsUiEventInterface * ti = dynamic_cast<HmsUiEventInterface*>(c);
			if (ti)
			{
				m_vUiEventInterface.push_back(ti);
				ti->SetParentUiInterface(this);
			}
		}
		nRet = m_vUiEventInterface.size();
	}
	
	m_bReorderNeeded = true;
	return nRet;
}

void HmsUiEventInterface::AddChildInterface(HmsUiEventInterface * child)
{
	if (child)
	{
		m_vUiEventInterface.push_back(child);
		child->SetParentUiInterface(this);
		m_bReorderNeeded = true;
	}

}

void HmsUiEventInterface::RemoveAllChildInterface()
{
	for (auto c:m_vUiEventInterface)
	{
		if (c)
			c->SetParentUiInterface(nullptr);
	}
	m_vUiEventInterface.clear();
}

void HmsUiEventInterface::RemoveChildInterface(HmsUiEventInterface * child)
{
	auto iter = std::find(m_vUiEventInterface.begin(), m_vUiEventInterface.end(), child);
	if (iter != m_vUiEventInterface.end())
	{
		(*iter)->SetParentUiInterface(nullptr);
		m_vUiEventInterface.erase(iter);
	}	
}

Vec2 HmsUiEventInterface::GetPositionAr()
{
	if (m_node)
	{
		return m_node->GetPoistion();
	}
	return Vec2(0, 0);
}

HmsAviPf::Vec2 HmsUiEventInterface::GetPositionOrigin()
{
	if (m_node)
	{
		return m_node->GetPoistion() - m_node->GetAnchorPointInPoints();
	}
	return Vec2(0, 0);
}

HmsAviPf::Vec2 HmsUiEventInterface::GetUiArPostion(const Vec2 & posParent, HmsUiEventInterface * pNode)
{
	auto posRelate = posParent - pNode->GetPositionAr();
	return posRelate;
}

HmsAviPf::Vec2 HmsUiEventInterface::GetUiOriginPostion(const Vec2 & posParent, HmsUiEventInterface * pNode)
{
    Vec2 posRelate;
    if (pNode && pNode->m_node)
    {
        posRelate = posParent - pNode->m_node->GetPoistion();
        posRelate.x /= pNode->m_node->GetScaleX();
        posRelate.y /= pNode->m_node->GetScaleY();
        posRelate += pNode->m_node->GetAnchorPointInPoints();
    }
    return posRelate;
}

CHmsLabel * HmsUiEventInterface::CreateLabel(const char * strText, float fSize, bool border /*= false*/)
{
#if 0
	auto fontPath = "fonts/msyh.ttc";
	if (border)
	{
		fontPath = "fonts/msyhbd.ttc";
	}
	auto label = CHmsLabel::createWithTTF(strText, fontPath, fSize);//calibri.ttf
	return label;
#else
	return CHmsGlobal::CreateLanguageLabel(strText, fSize, border);
#endif
}

HmsAviPf::Vec2 HmsUiEventInterface::GetUiTopCenter()
{
	if (m_node)
	{
		auto size = m_node->GetContentSize();
		return Vec2(size.width/2.0f,size.height);
	}
	return Vec2();
}

HmsAviPf::Vec2 HmsUiEventInterface::GetUiTopLeft()
{
	if (m_node)
	{
		auto size = m_node->GetContentSize();
		return Vec2(0, size.height);
	}
	return Vec2();
}

HmsAviPf::Vec2 HmsUiEventInterface::GetUiTopRight()
{
	if (m_node)
	{
		auto size = m_node->GetContentSize();
		return Vec2(size.width, size.height);
	}
	return Vec2();
}

HmsAviPf::Vec2 HmsUiEventInterface::GetUiBottomRight()
{
	if (m_node)
	{
		auto size = m_node->GetContentSize();
		return Vec2(size.width, 0);
	}
	return Vec2();
}

HmsAviPf::Vec2 HmsUiEventInterface::GetUiBottomLeft()
{
	if (m_node)
	{
		auto size = m_node->GetContentSize();
		return Vec2(0, 0);
	}
	return Vec2();
}

HmsAviPf::Vec2 HmsUiEventInterface::GetUiBottomCenter()
{
	if (m_node)
	{
		auto size = m_node->GetContentSize();
		return Vec2(size.width / 2.0f, 0);
	}
	return Vec2();
}

HmsAviPf::Vec2 HmsUiEventInterface::GetUiCenter()
{
	if (m_node)
	{
		auto size = m_node->GetContentSize();
		return Vec2(size.width / 2.0f, size.height/2.0f);
	}
	return Vec2();
}

HmsAviPf::Vec2 HmsUiEventInterface::GetUiCenterLeft()
{
	if (m_node)
	{
		auto size = m_node->GetContentSize();
		return Vec2(0, size.height / 2.0f);
	}
	return Vec2();
}

HmsAviPf::Vec2 HmsUiEventInterface::GetUiCenterRight()
{
	if (m_node)
	{
		auto size = m_node->GetContentSize();
		return Vec2(size.width, size.height / 2.0f);
	}
	return Vec2();
}

void HmsUiEventInterface::EnableUi(bool bEnable)
{
	m_bDisable = !bEnable;
}

bool HmsUiEventInterface::UiEventComparisonGreater(HmsUiEventInterface* ei1, HmsUiEventInterface* ei2)
{
	if (ei1 && ei2 && ei1->m_node && ei2->m_node)
	{
		auto & n2 = ei1->m_node;
		auto & n1 = ei2->m_node;
		return(n1->GetLocalZOrder() < n2->GetLocalZOrder() ||
			(n1->GetLocalZOrder() == n2->GetLocalZOrder() && n1->GetOrderOfArrival() < n2->GetOrderOfArrival())
			);
	}
	return true;

}

void HmsUiEventInterface::ShortChildInterface()
{
	if (m_bReorderNeeded)
	{
		std::sort(std::begin(m_vUiEventInterface), std::end(m_vUiEventInterface), UiEventComparisonGreater);
		m_bReorderNeeded = false;
	}
}

void HmsUiEventInterface::RemoveFromParentInterface()
{
	if (m_pParentUiInterface)
	{
		m_pParentUiInterface->RemoveChildInterface(this);
	}
}

HmsAviPf::Vec2 HmsUiEventInterface::Touch2UiPosition(Touch * touch)
{
	if (m_node)
	{
		auto pos = m_node->ConvertTouchToNodeSpace(touch);
		//pos.x *= m_node->GetScaleX();
		//pos.y *= m_node->GetScaleY();
		return pos;
	}
	
	return  Vec2();
}

Rect HmsUiEventInterface::GetRectByOriginUI()
{
    return GetRectByOriginUI(m_node);
}

Rect HmsUiEventInterface::GetRectByOriginUI(CHmsNode * node)
{
#if 0
	Size size;
	float fScaleX = 1.0f;
	float fScaleY = 1.0f;

	if (node)
	{
		size = node->getContentSize();
 		if (m_node != node)
		{
			fScaleX = node->getScaleX();
			fScaleY = node->getScaleY();
		}
	}

	float scaleWidth = size.width*fScaleX;
	float scaleHeight = size.height*fScaleY;
#else
	Size size;
	if (node)
	{
		size = node->GetContentSize();
	}
	float scaleWidth = size.width;
	float scaleHeight = size.height;
#endif

	if (scaleHeight < 0)
	{
		return Rect(0, scaleHeight, scaleWidth, -scaleHeight);
	}

	return Rect(0, 0, scaleWidth, scaleHeight);
}

NS_HMS_END

