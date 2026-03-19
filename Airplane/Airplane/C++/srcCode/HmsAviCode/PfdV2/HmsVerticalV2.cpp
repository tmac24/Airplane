#include "HmsVerticalV2.h"
#include "HmsRollLable.h"
#include "HmsPfdHeader.h"
#include "HmsGlobal.h"

USING_NS_HMS;

CHmsVerticalV2 * CHmsVerticalV2::Create(HmsAviPf::Size size)
{
	CHmsVerticalV2 *ret = new CHmsVerticalV2();
	if (ret && ret->InitWithSize(size))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

CHmsVerticalV2::CHmsVerticalV2()
{
	m_MaxValue = 20000.0;
	m_MinValue = -20000.0;
	m_TickSpan = 0.2f;/*决定刻度显示值.*/
	m_RealSpan = 200.0;/*决定指针放置位置.*/
	m_PointerSize = Size(94.0, 50.0);
}

CHmsVerticalV2::~CHmsVerticalV2()
{
}

bool CHmsVerticalV2::InitWithSize(HmsAviPf::Size size)
{
	m_bShowAbs = true;
	m_TickTextSize = 28.0;
	m_TextLineOffset = 7.0;
	if (!CHmsAltmeterV2::InitWithSize(size))
	{
		return false;
	}

	SetRollTick(false);
	m_pPointer->SetPositionX(20.0);
	return true;
}

void CHmsVerticalV2::GenRollText()
{
	m_pRollLabel = 0;
	m_pValue = CHmsGlobal::CreateLanguageLabel("00", 30.0, Color4F::WHITE);
	if (m_pValue)
	{
		m_pValue->SetAnchorPoint(0.5f, 0.45f);
		m_pValue->SetPosition(HALF_VALUE(m_PointerSize));
		m_pPointer->AddChild(m_pValue);
	}
}

void CHmsVerticalV2::GenTableBar()
{
	m_pTableBar = CHmsNode::Create();
	m_pTableBar->SetContentSize(m_sizeContent);
	m_pTableBar->SetPosition(0.0, 0.0);
	m_pClipAread->AddChild(m_pTableBar);

	float fLineSpan = (m_sizeContent.height - m_TickHigh) / (m_TickNumber - 9);
	float fStartPos = -fLineSpan * 4.0;
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
			pLabel->SetName(INT_TO_STRING(nCurValue, DEF_PERCISION));
		}
			
		m_TxtTicks.push_back(pTextTick);
	}

	m_TickPixel = fLineSpan / m_RealSpan;
}

void CHmsVerticalV2::UpdateValueAfter(float fValue)
{
	//m_pValue->SetString(FLOAT_TO_STRING(fValue, 0));
	if (m_pValue)
	{
		m_pValue->SetString(INT_TO_STRING((int)round(fValue), 1));
	}
}

HmsAviPf::CHmsNode* CHmsVerticalV2::GenMarker(HmsAviPf::Size size, Color4B color)
{
	return 0;
}

void CHmsVerticalV2::GenBarClip()
{
	/*不采用时裁剪区时,重写该接口,注释该行代码.*/
	//CHmsBaseTable::GenBarClip();

	if (m_pClipBarDraw)
	{
		float fHeight = m_PointerSize.height;
		float fPosY = HALF_VALUE(m_sizeContent.height - m_PointerSize.height);
		m_pClipBarDraw->clear();
		m_pClipBarDraw->DrawSolidRect(22.0, fPosY, m_sizeContent.width, fPosY + fHeight, Color4F::RED);
	}
}
