#include "HmsAirspeedV2.h"
#include "display/HmsLightDrawNode.h"
#include "HmsPfdHeader.h"
#include "Calculate/CalculateZs.h"

USING_NS_HMS;

CHmsAirspeedV2 * CHmsAirspeedV2::Create(HmsAviPf::Size size)
{
	CHmsAirspeedV2 *ret = new CHmsAirspeedV2();
	if (ret && ret->InitWithSize(size))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

CHmsAirspeedV2::CHmsAirspeedV2()
{
	m_MaxValue = 999.0;
	m_PointerSize = Size(130.0, 75.0);
}

CHmsAirspeedV2::~CHmsAirspeedV2()
{
}

bool CHmsAirspeedV2::InitWithSize(HmsAviPf::Size size)
{
	if (!CHmsBaseTable::InitWithSize(size))
	{
		return false;
	}

	HmsAviPf::CHmsNode *pMarker = GenMarker(Size(144, 52), Color4B(0, 0, 0, 62));
	if (pMarker)
	{
		pMarker->SetAnchorPoint(0.0, 1.0);
		pMarker->SetPosition(0.0, -5.0);
		this->AddChild(pMarker);
	}

	SetValue(0.0);
	MetricSwitch(false);

	return true;
}

HmsAviPf::CHmsNode* CHmsAirspeedV2::GenFeetFrame(HmsAviPf::Size size, Color4B color)
{
	m_pFeetFrame = CHmsNode::Create();
	m_pFeetFrame->SetContentSize(size);

	CHmsLightDrawNode *pBorder = CHmsLightDrawNode::Create();
	pBorder->DrawSolidRect(0.0, 0.0, size.width, size.height, PFD_POINTER_BK_COLOR, false);
	pBorder->DrawInnerRect(0.0, 0.0, size.width, size.height, 2.0, Color4F(color));
	m_pFeetFrame->AddChild(pBorder);

	CHmsLabel *pFeetUnit = CHmsGlobal::CreateLanguageLabel("kn", 28.0, Color4F(UNIT_COLOR), true);
	if (pFeetUnit)
	{
		pFeetUnit->SetAnchorPoint(1.0, 0.5);
		pFeetUnit->SetPosition(size.width - 12.0, HALF_VALUE(size.height));
		m_pFeetFrame->AddChild(pFeetUnit);
	}

	m_pFeetText = CHmsGlobal::CreateLanguageLabel("__", 28.0, Color4F::WHITE, true);
	if (m_pFeetText)
	{
		m_pFeetText->SetAnchorPoint(1.0, 0.5);
		m_pFeetText->SetPosition(size.width - 48.0, HALF_VALUE(size.height));
		m_pFeetFrame->AddChild(m_pFeetText);
	}

	return m_pFeetFrame;
}

HmsAviPf::CHmsNode* CHmsAirspeedV2::GenMarker(HmsAviPf::Size size, Color4B color)
{
	CHmsNode *pMarker = CHmsNode::Create();
	pMarker->SetContentSize(size);

	CHmsLightDrawNode *pDrawNode = CHmsLightDrawNode::Create();
	pDrawNode->DrawSolidRect(0.0, 0.0, size.width - 21.0, size.height, Color4F(color), false);

	std::vector<Vec2> vcPoints;
	vcPoints.push_back(Vec2(size.width - 21.0, 0.0));
	vcPoints.push_back(Vec2(size.width, size.height));
	vcPoints.push_back(Vec2(size.width - 21.0, size.height));
	pDrawNode->DrawTriangles(vcPoints, Color4F(color), false);
	vcPoints.clear();
	vcPoints.push_back(Vec2(size.width - 20.5, 0.0));
	vcPoints.push_back(Vec2(size.width, size.height));
	pDrawNode->DrawLineStrip(vcPoints, 2.0, color);

	m_pShowText = CHmsGlobal::CreateLanguageLabel("GS KM/H", 28.0, Color4F::WHITE, true);
	if (m_pShowText)
	{
		m_pShowText->SetAnchorPoint(0.5, 0.5);
		m_pShowText->SetPosition(HALF_VALUE(size) - Size(7.5, -2.0));
	}

	pMarker->AddChild(pDrawNode);
	if (m_pShowText)
	{
		pMarker->AddChild(m_pShowText);
	}
	return pMarker;
}

void CHmsAirspeedV2::BeforeGenUnvalid()
{
	Vec2 PointerVec2 = m_pPointer->GetPoistion();
	Size PointerSize = m_pPointer->GetContentSize();
	GenFeetFrame(Size(140.0, 50.0), Color4B::WHITE);
	m_pFeetFrame->SetAnchorPoint(1.0, 0.0);
	m_pFeetFrame->SetPosition(PointerVec2.x + PointerSize.width - 10.0, PointerVec2.y + HALF_VALUE(PointerSize.height) - 0.5);
	this->AddChild(m_pFeetFrame);
}

void CHmsAirspeedV2::SetText(const std::string &text)
{
	PFD_RETURN(0 == m_pShowText);
	m_pShowText->SetString(text);
}

void CHmsAirspeedV2::UpdateValueAfter(float fValue)
{
	PFD_RETURN(0 == m_pFeetText);

	m_pFeetText->SetString(INT_TO_STRING(fValue / KnotKMH + 0.5, 0));
}

void CHmsAirspeedV2::GenBarClip()
{
	/*不采用时裁剪区时,重写该接口,注释该行代码.*/
	//CHmsBaseTable::GenBarClip();
}

void CHmsAirspeedV2::MetricSwitch(bool bOn)
{
	PFD_RETURN(0 == m_pFeetFrame);

	m_pFeetFrame->SetVisible(bOn);
	m_pFeetFrame->ForceUpdate();

	if (m_pClipBarDraw &&  m_pClipBar && m_bSwitched != bOn)
	{
		m_bSwitched = bOn;
		m_pClipBarDraw->clear();

		float fHeight = bOn ? (m_PointerSize.height + 50.0) : (m_PointerSize.height);
		float fPosY = HALF_VALUE(m_sizeContent.height - m_PointerSize.height);
		m_pClipBarDraw->DrawSolidRect(-7.0, fPosY, m_PointerSize.width - 7.0, fPosY + fHeight, Color4F::RED);
	}
}