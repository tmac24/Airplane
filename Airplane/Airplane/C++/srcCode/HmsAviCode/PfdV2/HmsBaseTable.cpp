#include "HmsBaseTable.h"
#include "HmsGlobal.h"
#include "base/HmsImage.h"
#include "display/HmsLightDrawNode.h"
#include "HmsRollLable.h"
#include "HmsPfdHeader.h"

USING_NS_HMS;

CHmsBaseTable * CHmsBaseTable::Create(HmsAviPf::Size size)
{
	CHmsBaseTable *ret = new CHmsBaseTable();
	if (ret && ret->InitWithSize(size))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

CHmsBaseTable::CHmsBaseTable()
: m_TickNumber(31)/*显示-10条.*/
, m_MaxValue(999.0)
, m_MinValue(0.0)
, m_CurValue(-1.0)
, m_TickLong(20.0)
, m_TickShort(14.0)
, m_TickHigh(3.0)
, m_TickSpan(2.0)
, m_RealSpan(2.0)
, m_TickPixel(1.0)
, m_bShowAbs(false)
, m_bSwitched(true)
, m_bRollTick(true)
{
	m_PointerSize = Size(130.0, 75.0);
}

CHmsBaseTable::~CHmsBaseTable()
{
}

bool CHmsBaseTable::InitWithSize(HmsAviPf::Size size)
{
	if (!CHmsNode::Init())
	{
		return false;
	}

	SetContentSize(size);

	GenClipArea();
	GenBackground();
	GenDecorate();
	GenPointer();
	GenTableBar();
	GenBarClip();
	GenRollText();
	BeforeGenUnvalid();
	GenValid(size);
	this->AddChild(m_pUnValid);

	SetValue(0);
	ValidSwitch(false);

	return true;
}

void CHmsBaseTable::GenClipArea()
{
	CHmsDrawNode *pBkStencil = CHmsDrawNode::Create();
	pBkStencil->DrawSolidRect(0.0, 0.0, m_sizeContent.width, m_sizeContent.height, Color4F::WHITE);
	m_pClipAread = CHmsClippingNode::create();
	m_pClipAread->setStencil(pBkStencil);
	AddChild(m_pClipAread);
}

void CHmsBaseTable::GenBackground()
{
	CHmsDrawNode *pBackGround = CHmsDrawNode::Create();
	pBackGround->DrawSolidRect(0.0, 0.0, m_sizeContent.width, m_sizeContent.height, Color4F(0.0, 0.0, 0.0, 0.25));
	m_pClipAread->AddChild(pBackGround);
}

void CHmsBaseTable::GenDecorate()
{
	CHmsLightDrawNode *pRightVLine = CHmsLightDrawNode::Create();
	pRightVLine->DrawVerticalLine(m_sizeContent.width - 1.0f, 0.0, m_sizeContent.height, 3.0f, Color4F(1.0, 1.0, 1.0, 0.5));
	m_pClipAread->AddChild(pRightVLine);
}

void CHmsBaseTable::GenPointer()
{
	float fRefWidth = m_PointerSize.width - 10.0;
	m_pPointer = CHmsNode::Create();
	m_pPointer->SetContentSize(m_PointerSize);
	m_pPointer->SetAnchorPoint(0.0, 0.5);
	m_pPointer->SetPosition(2.0, HALF_VALUE(m_sizeContent.height));
	this->AddChild(m_pPointer);

	std::vector<Vec2> vcPoints;
	vcPoints.push_back(Vec2(fRefWidth, m_PointerSize.height / 3.0));
	vcPoints.push_back(Vec2(m_PointerSize.width, HALF_VALUE(m_PointerSize.height)));
	vcPoints.push_back(Vec2(fRefWidth, m_PointerSize.height * 2.0 / 3.0));

	CHmsDrawNode *pBkPointer = CHmsDrawNode::Create();
	pBkPointer->DrawSolidRect(0.0, 0.0, m_PointerSize.width - 10.0, m_PointerSize.height, PFD_POINTER_BK_COLOR);
	pBkPointer->DrawTriangles(vcPoints, PFD_POINTER_BK_COLOR);
	m_pPointer->AddChild(pBkPointer);

	vcPoints.push_back(Vec2(fRefWidth, m_PointerSize.height));
	vcPoints.push_back(Vec2(0.0, m_PointerSize.height));
	vcPoints.push_back(Vec2(0.0, 0.0));
	vcPoints.push_back(Vec2(fRefWidth, 0.0));
	CHmsLightDrawNode *pBorder = CHmsLightDrawNode::Create();
	pBorder->DrawLineLoop(vcPoints, 2.0f, Color4B::WHITE);
	m_pPointer->AddChild(pBorder);
}

void CHmsBaseTable::GenBarClip()
{
	Size size = m_PointerSize + Size(0.0, 50.0);

	m_pClipBarDraw = CHmsDrawNode::Create();
	m_pClipBar = CHmsClippingNode::create();
	m_pClipBar->setStencil(m_pClipBarDraw);
	m_pClipBar->setInverted(true);

	m_pTableBar->RemoveFromParent();
	m_pClipBar->AddChild(m_pTableBar);
	m_pClipAread->AddChild(m_pClipBar);
}

void CHmsBaseTable::GenTableBar()
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
		pTextTick->SetAnchorPoint(1.0, 0.5);
		pTextTick->SetPosition(m_sizeContent.width - 9.0, floorf(fStartPos + fLineSpan * iIndex + HALF_VALUE(m_TickHigh)));
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

void CHmsBaseTable::GenRollText()
{
	CHmsRollLabel::BitFontSize fontSize;
	fontSize.push_back(39.5);
	fontSize.push_back(42.0);
	fontSize.push_back(42.0);
	m_pRollLabel = CHmsRollLabel::Create(fontSize, CHmsRollLabel::UC_ONE_02, CHmsRollLabel::RS_ROLL | CHmsRollLabel::RS_BACKGROUND, 3.0);
	m_pRollLabel->SetBackgroundColor(Color4B(0, 0, 0, 0));
	m_pRollLabel->SetAnchorPoint(0.5, 0.5);
	m_pRollLabel->SetPosition(HALF_VALUE(m_PointerSize));
	m_pPointer->AddChild(m_pRollLabel);
}

CHmsNode* CHmsBaseTable::GenTextTick(bool bNeedText, bool bLong)
{
	CHmsNode *pTextTick = CHmsNode::Create();
	pTextTick->SetContentSize(Size(m_TickLong, m_TickHigh));

	float halfHigth = HALF_VALUE(m_TickHigh);
	float curLength = bLong ? m_TickLong : m_TickShort;
	Color4F curColr = bLong ? Color4F(1.0, 1.0, 1.0, 0.5) : Color4F(1.0, 1.0, 1.0, 0.25);

	CHmsLightDrawNode *pTick = CHmsLightDrawNode::Create();
	pTick->DrawHorizontalLine(m_TickLong - curLength, m_TickLong, halfHigth, m_TickHigh, curColr);
	pTick->SetTag(ASS_LINE);
	pTextTick->AddChild(pTick);

	if (bNeedText)
	{
		CHmsLabel *pText = CHmsGlobal::CreateLabel("00", 32.0, Color4F::WHITE);
		if (pText)
		{
			pText->SetAnchorPoint(1.0, 0.5);
			pText->SetPosition(-5.0f, HALF_VALUE(m_TickHigh));
			pText->SetTag(ASS_TEXT);
			pTextTick->AddChild(pText);
		}
	}

	return pTextTick;
}

HmsAviPf::CHmsNode* CHmsBaseTable::GetTextValue(int nIndex, float *pOutput)
{
	CHmsNode *pLabelNode = (CHmsNode*)m_TxtTicks[nIndex];
	PFD_RETURN_VAL(0 == pLabelNode, 0);

	CHmsLabel *pLabel = (CHmsLabel *)pLabelNode->GetChildByTag(ASS_TEXT);
	PFD_RETURN_VAL(0 == pLabel, 0);

	*pOutput = (float)atof(pLabel->GetName().c_str());
	return pLabelNode;
}

void CHmsBaseTable::SetValue(float fValue)
{
	if (m_bUnValid || FUNC_FLOAT_EQUAL(m_CurValue, fValue, REF_VALUE1)){
		return;
	}

	float fCurVlaue = HMS_MAX(m_MinValue, HMS_MIN(m_MaxValue, fValue));

	float abSpeed = fabsf(fCurVlaue);
	float fSmybol = (fCurVlaue < 0.0f) ? -1.0f : 1.0f;
	float fMiddle = (abSpeed - fmodf(abSpeed, m_TickSpan*5.0)) * fSmybol;

	float fCurMid = 0.0f;
	int   nMidInd = (int)((m_TickNumber - 1)/ 2);
	GetTextValue(nMidInd, &fCurMid);

	if (m_bRollTick)/*指针位置不变滚动刻度.*/
	{
		if (fabsf(fCurMid - fMiddle) + 0.001f >= m_TickSpan * 5.0)
		{
			for (int iIndex = 0; iIndex < m_TickNumber; ++iIndex)
			{
				bool bChange = (iIndex % 5 == 0) ? true : false;
				PFD_CONTINUE(!bChange);

				CHmsNode *pNode = GetTextValue(iIndex, &fCurMid);
				PFD_CONTINUE(!pNode);

				CHmsLabel *pLabel = (CHmsLabel *)pNode->GetChildByTag(ASS_TEXT);
				PFD_CONTINUE(!pLabel);

				float curValue = fMiddle - (nMidInd - iIndex) / 5 * m_TickSpan * 5.0;
				pLabel->SetString(m_bShowAbs ? INT_TO_STRING((int)round(fabsf(curValue)), 1) : INT_TO_STRING((int)round(curValue), 1));
				pLabel->SetName(INT_TO_STRING((int)round(curValue), 1));
			}

			m_CurValue = fCurVlaue;
		}

		m_pTableBar->SetPositionY(floorf(-(fCurVlaue - fMiddle) * m_TickPixel));
		if (m_pRollLabel){ m_pRollLabel->SetValue(fCurVlaue); }
	}
	else/*刻度不滚动不变,更改指针位置.*/
	{
		float fPosY = HALF_VALUE(m_sizeContent.height) + (fCurVlaue - fCurMid * m_RealSpan) * m_TickPixel;
		float fPoxMaxY = HALF_VALUE(m_sizeContent.height) + (m_RealSpan * 10.0 - fCurMid * m_RealSpan) * m_TickPixel;
		float fPoxMinY = HALF_VALUE(m_sizeContent.height) - (m_RealSpan * 10.0 - fCurMid * m_RealSpan) * m_TickPixel;
		
		fPosY = HMS_MIN(fPoxMaxY, HMS_MAX(fPoxMinY, fPosY));
		m_pPointer->SetPositionY(fPosY);

		if (m_pClipBarDraw)
		{
			float fHeight = m_PointerSize.height;
			float fPosY2 = fPosY - HALF_VALUE(m_PointerSize.height);
			m_pClipBarDraw->clear();
			m_pClipBarDraw->DrawSolidRect(22.0, fPosY2, m_sizeContent.width, fPosY2 + fHeight, Color4F::RED);
		}
	}
	
	UpdateValueAfter(fCurVlaue);
}

void CHmsBaseTable::SetShowAbs(bool bAbs)
{
	m_bShowAbs = bAbs;
}

void CHmsBaseTable::SetRollTick(bool bRoolTick)
{
	m_bRollTick = bRoolTick;
}
