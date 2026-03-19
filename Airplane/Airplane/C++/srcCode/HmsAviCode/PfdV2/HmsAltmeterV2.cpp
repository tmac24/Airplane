#include "HmsAltmeterV2.h"
#include "HmsGlobal.h"
#include "display/HmsLightDrawNode.h"
#include "HmsRollLable.h"
#include "HmsPfdHeader.h"
#include "Calculate/CalculateZs.h"

USING_NS_HMS;

CHmsAltmeterV2 * CHmsAltmeterV2::Create(HmsAviPf::Size size)
{
	CHmsAltmeterV2 *ret = new CHmsAltmeterV2();
	if (ret && ret->InitWithSize(size))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

CHmsAltmeterV2::CHmsAltmeterV2()
{
	m_TickTextSize = 32.0;
	m_MaxValue = 30000.0;
	m_MinValue = -500.0;
	m_TickSpan = 20.0;
	m_PointerSize = Size(130.0, 75.0);
	m_TextLineOffset = 0.0;
}

CHmsAltmeterV2::~CHmsAltmeterV2()
{
}

bool CHmsAltmeterV2::InitWithSize(HmsAviPf::Size size)
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

void CHmsAltmeterV2::GenDecorate()
{
	CHmsLightDrawNode *pRightVLine = CHmsLightDrawNode::Create();
	pRightVLine->DrawVerticalLine(1.0f, 0.0, m_sizeContent.height, 3.0f, Color4F(1.0, 1.0, 1.0, 0.5));
	m_pClipAread->AddChild(pRightVLine);
}

void CHmsAltmeterV2::GenPointer()
{
	m_pPointer = CHmsNode::Create();
	m_pPointer->SetContentSize(m_PointerSize);
	m_pPointer->SetAnchorPoint(0.0, 0.5);
	m_pPointer->SetPosition(13.0, HALF_VALUE(m_sizeContent.height));
	this->AddChild(m_pPointer);

	float fRefWidth = 10.0;
	std::vector<Vec2> vcPoints;
	vcPoints.push_back(Vec2(fRefWidth, m_PointerSize.height / 3.0));
	vcPoints.push_back(Vec2(0.0, HALF_VALUE(m_PointerSize.height)));
	vcPoints.push_back(Vec2(fRefWidth, m_PointerSize.height * 2.0 / 3.0));

	CHmsDrawNode *pBkPointer = CHmsDrawNode::Create();
	pBkPointer->DrawSolidRect(fRefWidth, 0.0, m_PointerSize.width, m_PointerSize.height, PFD_POINTER_BK_COLOR);
	pBkPointer->DrawTriangles(vcPoints, PFD_POINTER_BK_COLOR);
	m_pPointer->AddChild(pBkPointer);

	vcPoints.push_back(Vec2(fRefWidth, m_PointerSize.height));
	vcPoints.push_back(Vec2(m_PointerSize.width, m_PointerSize.height));
	vcPoints.push_back(Vec2(m_PointerSize.width, 0.0));
	vcPoints.push_back(Vec2(fRefWidth, 0.0));
	CHmsLightDrawNode *pBorder = CHmsLightDrawNode::Create();
	pBorder->DrawLineLoop(vcPoints, 2.0f, Color4B::WHITE);
	m_pPointer->AddChild(pBorder);
}

void CHmsAltmeterV2::GenTableBar()
{
	m_pTableBar = CHmsNode::Create();
	m_pTableBar->SetContentSize(m_sizeContent);
	m_pTableBar->SetPosition(0.0, 0.0);
	m_pClipAread->AddChild(m_pTableBar);

	float fLineSpan = (m_sizeContent.height - m_TickHigh) / (m_TickNumber - 11);
	float fStartPos = -fLineSpan * 5.0;
	for (int iIndex = 0; iIndex < m_TickNumber; ++iIndex)
	{
		bool bNeedText = (iIndex % 5 == 0) ? true : false;
		CHmsNode *pTextTick = GenTextTick(bNeedText, bNeedText);
		pTextTick->SetAnchorPoint(0.0, 0.5);
		pTextTick->SetPosition(9.0, floorf(fStartPos + fLineSpan * iIndex + HALF_VALUE(m_TickHigh)));
		m_pTableBar->AddChild(pTextTick);

		CHmsLabel *pLabel = (CHmsLabel *)pTextTick->GetChildByTag(ASS_TEXT);
		if (pLabel)
		{
			int nCurValue = (int)((iIndex - 5) * m_TickSpan);
			pLabel->SetString(m_bShowAbs ? INT_TO_STRING(abs(nCurValue), DEF_PERCISION) : INT_TO_STRING(nCurValue, DEF_PERCISION));
			pLabel->SetName(INT_TO_STRING(nCurValue, DEF_PERCISION));/*用于记录实际值.*/
		}
			
		m_TxtTicks.push_back(pTextTick);
	}

	m_TickPixel = fLineSpan / m_TickSpan;
}

void CHmsAltmeterV2::GenRollText()
{
	CHmsRollLabel::BitFontSize fontSize;
	fontSize.push_back(38.0);
	fontSize.push_back(38.0);
	fontSize.push_back(40.0);
	fontSize.push_back(40.0);
	m_pRollLabel = CHmsRollLabel::Create(fontSize, CHmsRollLabel::UC_TWO_20, CHmsRollLabel::RS_ROLL | CHmsRollLabel::RS_SYMBOL_PLACE | CHmsRollLabel::RS_BACKGROUND, 1.5);
	m_pRollLabel->SetBackgroundColor(Color4B(0, 0, 0, 0));
	m_pRollLabel->SetAnchorPoint(1.0, 0.5);
	m_pRollLabel->SetPosition(m_PointerSize.width, HALF_VALUE(m_PointerSize.height));
	m_pPointer->AddChild(m_pRollLabel);
}

CHmsNode* CHmsAltmeterV2::GenTextTick(bool bNeedText, bool bLong)
{
	CHmsNode *pTextTick = CHmsNode::Create();
	pTextTick->SetContentSize(Size(m_TickLong, m_TickHigh));

	float curLength = bLong ? m_TickLong : m_TickShort;
	Color4F curColr = bLong ? Color4F(1.0, 1.0, 1.0, 0.5) : Color4F(1.0, 1.0, 1.0, 0.25);

	CHmsLightDrawNode *pTick = CHmsLightDrawNode::Create();
	pTick->DrawHorizontalLine(0.0, curLength, HALF_VALUE(m_TickHigh), m_TickHigh, curColr);
	pTick->SetTag(ASS_LINE);
	pTextTick->AddChild(pTick);

	if (bNeedText)
	{
		CHmsLabel *pText = CHmsGlobal::CreateLabel("00", m_TickTextSize, Color4F::WHITE);
		pText->SetAnchorPoint(0.0f, 0.4f);
		pText->SetPosition(m_TickLong + m_TextLineOffset, HALF_VALUE(m_TickHigh));
		pText->SetTag(ASS_TEXT);
		pTextTick->AddChild(pText);
	}

	return pTextTick;
}

HmsAviPf::CHmsNode* CHmsAltmeterV2::GenMarker(HmsAviPf::Size size, Color4B color)
{
	CHmsNode *pMarker = CHmsNode::Create();
	pMarker->SetContentSize(size);

	CHmsLightDrawNode *pDrawNode = CHmsLightDrawNode::Create();
	pDrawNode->DrawSolidRect(21.0, 0.0, size.width, size.height, Color4F(color), false);

	std::vector<Vec2> vcPoints;
	vcPoints.push_back(Vec2(21.0, 0.0));
	vcPoints.push_back(Vec2(0.0, size.height));
	vcPoints.push_back(Vec2(21.0, size.height));
	pDrawNode->DrawTriangles(vcPoints, Color4F(color), false);
	vcPoints.pop_back();
	pDrawNode->DrawLineStrip(vcPoints, 2.0, color);

	m_pShowText = CHmsGlobal::CreateLanguageLabel("GPS ALT", 28.0, Color4F::WHITE, true);
	if (m_pShowText)
	{
		m_pShowText->SetAnchorPoint(0.5, 0.5);
		m_pShowText->SetPosition(HALF_VALUE(size) - Size(-5.0, -2.0));
	}

	pMarker->AddChild(pDrawNode);
	if (m_pShowText)
	{
		pMarker->AddChild(m_pShowText);
	}
	return pMarker;
}

void CHmsAltmeterV2::BeforeGenUnvalid()
{
	Vec2 PointerVec2 = m_pPointer->GetPoistion();
	Size PointerSize = m_pPointer->GetContentSize();
	GenFeetFrame(Size(120.0, 50.0), Color4B::WHITE);
	m_pFeetFrame->SetAnchorPoint(0.0, 0.0);
	m_pFeetFrame->SetPosition(23.0, PointerVec2.y + HALF_VALUE(PointerSize.height) - 0.5);
	this->AddChild(m_pFeetFrame);
}

HmsAviPf::CHmsNode* CHmsAltmeterV2::GenFeetFrame(HmsAviPf::Size size, Color4B color)
{
	m_pFeetFrame = CHmsNode::Create();
	m_pFeetFrame->SetContentSize(size);

	CHmsLightDrawNode *pBorder = CHmsLightDrawNode::Create();
	pBorder->DrawSolidRect(0.0, 0.0, size.width, size.height, PFD_POINTER_BK_COLOR, false);
	pBorder->DrawInnerRect(0.0, 0.0, size.width, size.height, 2.0, Color4F(color));
	m_pFeetFrame->AddChild(pBorder);

	CHmsLabel *pFeetUnit = CHmsGlobal::CreateLanguageLabel("ft", 28.0, Color4F(UNIT_COLOR), true);

	m_pFeetText = CHmsGlobal::CreateLanguageLabel("0000", 28.0, Color4F::WHITE, true);
	if (m_pFeetText && m_pFeetFrame)
	{
        pFeetUnit->SetAnchorPoint(1.0, 0.5);
        pFeetUnit->SetPosition(size.width - 8.0, HALF_VALUE(size.height));
        m_pFeetFrame->AddChild(pFeetUnit);

		m_pFeetText->SetAnchorPoint(1.0, 0.5);
        m_pFeetText->SetPosition(size.width - 10 - pFeetUnit->GetContentSize().width, HALF_VALUE(size.height));
		m_pFeetFrame->AddChild(m_pFeetText);
	}

	return m_pFeetFrame;
}

void CHmsAltmeterV2::SetText(const std::string &text)
{
	PFD_RETURN(0 == m_pShowText);
	m_pShowText->SetString(text);
}

void CHmsAltmeterV2::UpdateValueAfter(float fValue)
{
	PFD_RETURN(0 == m_pFeetText);

	m_pFeetText->SetString(INT_TO_STRING(MeterToFeet(fValue), 4));
}

void CHmsAltmeterV2::GenBarClip()
{
	/*不采用时裁剪区时,重写该接口,注释该行代码.*/
	//CHmsBaseTable::GenBarClip();
}

void CHmsAltmeterV2::MetricSwitch(bool bOn)
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
		m_pClipBarDraw->DrawSolidRect(23.0, fPosY, m_PointerSize.width, fPosY + fHeight, Color4F::RED);
	}
}
