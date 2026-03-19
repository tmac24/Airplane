#include "HmsAttitudeV2.h"
#include "HmsGlobal.h"
#include "base/HmsImage.h"
#include "display/HmsLightDrawNode.h"
#include "HmsRollLable.h"
#include "HmsPfdHeader.h"

USING_NS_HMS;

CHmsAttitudeV2 * CHmsAttitudeV2::Create(HmsAviPf::Size size)
{
	CHmsAttitudeV2 *ret = new CHmsAttitudeV2();
	if (ret && ret->InitWithSize(size))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

CHmsAttitudeV2::CHmsAttitudeV2()
{
	m_nShowNumber = 5;
	m_CurPitch = -1.0;
}

CHmsAttitudeV2::~CHmsAttitudeV2()
{
}

bool CHmsAttitudeV2::InitWithSize(HmsAviPf::Size size)
{
	if (!CHmsNode::Init())
	{
		return false;
	}

	SetContentSize(size);

	float fRadius = HALF_VALUE(HMS_MIN(size.width, size.height));
	CHmsNode *pSrcTick = GenCircleTick(fRadius);
	pSrcTick->SetAnchorPoint(0.5, 0.5);
	pSrcTick->SetPosition(HALF_VALUE(m_sizeContent));
	this->AddChild(pSrcTick);

	m_RollPointer = GenRollPointer(fRadius);
	m_RollPointer->SetAnchorPoint(0.0, 0.0);
	m_RollPointer->SetPosition(HALF_VALUE(m_sizeContent));
	this->AddChild(m_RollPointer);

	m_PitchPointer = GenPitchPointer(fRadius - 40);
	m_PitchPointer->SetAnchorPoint(0.5, 0.5);
	m_PitchPointer->SetPosition(HALF_VALUE(m_sizeContent));
	this->AddChild(m_PitchPointer);

	CHmsNode *pPlaneMarker = GenPlaneMarker(Size(size.width - 40.0, 33.0));
	pPlaneMarker->SetAnchorPoint(0.5, 1.0);
	pPlaneMarker->SetPosition(HALF_VALUE(m_sizeContent));
	this->AddChild(pPlaneMarker);

	CHmsNode *pUnValid = GenValid(size);
	this->AddChild(pUnValid);

	SetPitch(0.0);
	SetRoll(0.0);
	ValidSwitch(false);

	return true;
}

HmsAviPf::CHmsNode* CHmsAttitudeV2::GenCircleTick(float fRadius)
{
	auto DrawSrcTick = [](CHmsLightDrawNode *pDrawNode, Vec2 &center,
		float fAngle, float fInnRadius, float fOutRadius, float fLineWidth)
	{
		float fRadian = ANGLE_TO_RADIAN(fAngle);
		float fSinRad = sin(fRadian);
		float fCosRad = cos(fRadian);

		Vec2 linePos[2];
		linePos[0] = Vec2(fSinRad * fInnRadius, fCosRad * fInnRadius) + center;
		linePos[1] = Vec2(fSinRad * fOutRadius, fCosRad * fOutRadius) + center;

		pDrawNode->DrawLineStrip(linePos, 2, fLineWidth, Color4B::WHITE);
	};

	CHmsNode *pSrcTick = CHmsNode::Create();
	pSrcTick->SetContentSize(Size(fRadius*2.0, fRadius*2.0));

	Vec2 centerPos(fRadius, fRadius);
	CHmsLightDrawNode *pDrawSrc = CHmsLightDrawNode::Create();
	pDrawSrc->DrawArc(centerPos, -60.5, 60.5, fRadius, 4.0, 40, Color4F::WHITE);

	float fOutRadius = fRadius + 20.0;
	DrawSrcTick(pDrawSrc, centerPos, -60.0, fRadius, fOutRadius, 4.0);
	DrawSrcTick(pDrawSrc, centerPos, -30.0, fRadius, fOutRadius, 4.0);
	DrawSrcTick(pDrawSrc, centerPos, 30.0, fRadius, fOutRadius, 4.0);
	DrawSrcTick(pDrawSrc, centerPos, 60.0, fRadius, fOutRadius, 4.0);

	fOutRadius = fRadius + 14.0;
	DrawSrcTick(pDrawSrc, centerPos, -45.0, fRadius, fOutRadius, 4.0);
	DrawSrcTick(pDrawSrc, centerPos, -20.0, fRadius, fOutRadius, 4.0);
	DrawSrcTick(pDrawSrc, centerPos, -10.0, fRadius, fOutRadius, 4.0);
	DrawSrcTick(pDrawSrc, centerPos, 10.0, fRadius, fOutRadius, 4.0);
	DrawSrcTick(pDrawSrc, centerPos, 20.0, fRadius, fOutRadius, 4.0);
	DrawSrcTick(pDrawSrc, centerPos, 45.0, fRadius, fOutRadius, 4.0);

	Vec2 trangles[3];
	float fRadian = ANGLE_TO_RADIAN(0.0);
	fOutRadius = fRadius + 10.0;
	trangles[0] = Vec2(sin(fRadian) * fOutRadius, cos(fRadian) * fOutRadius) + centerPos;
	fRadian = ANGLE_TO_RADIAN(2.0f);
	fOutRadius = fRadius + 25.0;
	trangles[1] = Vec2(sin(fRadian) * fOutRadius, cos(fRadian) * fOutRadius) + centerPos;
	fRadian = ANGLE_TO_RADIAN(-2.0f);
	trangles[2] = Vec2(sin(fRadian) * fOutRadius, cos(fRadian) * fOutRadius) + centerPos;
	pDrawSrc->DrawLineLoop(trangles, 3, 4.0, Color4B::WHITE);

	pSrcTick->AddChild(pDrawSrc);

	return pSrcTick;
}

HmsAviPf::CHmsNode* CHmsAttitudeV2::GenRollPointer(float fRadius)
{
	CHmsNode *pRollPointer = CHmsNode::Create();
	pRollPointer->SetContentSize(Size(1, fRadius));

	CHmsLightDrawNode *pDrawPointer = CHmsLightDrawNode::Create();
	Vec2 centerPos(fRadius, fRadius);
	Vec2 trangles[3];
	float fRadian = ANGLE_TO_RADIAN(0.0);
	float fInRadius = fRadius - 10.0;
	trangles[0] = Vec2(sin(fRadian) * fInRadius, cos(fRadian) * fInRadius);
	fRadian = ANGLE_TO_RADIAN(2.0f);
	fInRadius = fRadius - 25.0;
	trangles[1] = Vec2(sin(fRadian) * fInRadius, cos(fRadian) * fInRadius);
	fRadian = ANGLE_TO_RADIAN(-2.0f);
	trangles[2] = Vec2(sin(fRadian) * fInRadius, cos(fRadian) * fInRadius);
	pDrawPointer->DrawLineLoop(trangles, 3, 4.0, Color4B::WHITE);
	pRollPointer->AddChild(pDrawPointer);

	return pRollPointer;
}

HmsAviPf::CHmsNode* CHmsAttitudeV2::GenPlaneMarker(HmsAviPf::Size size)
{
	CHmsNode *pPlaneMarker = CHmsNode::Create();
	pPlaneMarker->SetContentSize(size);

	CHmsDrawNode *pDrawMarker = CHmsDrawNode::Create();
	CHmsLightDrawNode *pPDrawLight = CHmsLightDrawNode::Create();

	float fHalfWidth = HALF_VALUE(size.width);
	std::vector<Vec2> vcPointers;

	vcPointers.clear();
	vcPointers.push_back(Vec2(0.0, size.height + 5.0));
	vcPointers.push_back(Vec2(0.0, size.height));
	vcPointers.push_back(Vec2(51.0, size.height + 5.0));
	vcPointers.push_back(Vec2(60.0, size.height));
	pDrawMarker->DrawTriangleStrip(vcPointers, Color4F(1.0f, 0.88f, 0.35f, 1.0f));
	vcPointers.clear();
	vcPointers.push_back(Vec2(51.0, size.height + 5.0));
	vcPointers.push_back(Vec2(60.0, size.height));
	pPDrawLight->DrawLineStrip(vcPointers, 1.0, Color4B(254, 224, 9, 255));
	vcPointers.clear();
	vcPointers.push_back(Vec2(0.0, size.height));
	vcPointers.push_back(Vec2(0.0, size.height - 5.0));
	vcPointers.push_back(Vec2(60.0, size.height));
	vcPointers.push_back(Vec2(51.0, size.height - 5.0));
	pDrawMarker->DrawTriangleStrip(vcPointers, Color4F(0.76f, 0.67f, 0.3f, 1.0f));
	vcPointers.clear();
	vcPointers.push_back(Vec2(51.0, size.height - 5.0));
	vcPointers.push_back(Vec2(60.0, size.height));
	pPDrawLight->DrawLineStrip(vcPointers, 1.0, Color4B(194, 171, 7, 255));

	vcPointers.clear();
	vcPointers.push_back(Vec2(size.width - 60.0, size.height));
	vcPointers.push_back(Vec2(size.width - 51.0, size.height + 5.0));
	vcPointers.push_back(Vec2(size.width, size.height));
	vcPointers.push_back(Vec2(size.width, size.height + 5.0));
	pDrawMarker->DrawTriangleStrip(vcPointers, Color4F(1.0f, 0.88f, 0.35f, 1.0f));
	vcPointers.clear();
	vcPointers.push_back(Vec2(size.width - 60.0, size.height));
	vcPointers.push_back(Vec2(size.width - 51.0, size.height + 5.0));
	pPDrawLight->DrawLineStrip(vcPointers, 1.0, Color4B(254, 225, 10, 255));
	vcPointers.clear();
	vcPointers.push_back(Vec2(size.width - 60.0, size.height));
	vcPointers.push_back(Vec2(size.width - 51.0, size.height - 5.0));
	vcPointers.push_back(Vec2(size.width, size.height));
	vcPointers.push_back(Vec2(size.width, size.height - 5.0));
	pDrawMarker->DrawTriangleStrip(vcPointers, Color4F(0.76f, 0.67f, 0.3f, 1.0f));
	vcPointers.clear();
	vcPointers.push_back(Vec2(size.width - 60.0, size.height));
	vcPointers.push_back(Vec2(size.width - 51.0, size.height - 5.0));
	pPDrawLight->DrawLineStrip(vcPointers, 1.0, Color4B(194, 171, 7, 255));

	vcPointers.clear();
	vcPointers.push_back(Vec2(fHalfWidth, size.height));
	vcPointers.push_back(Vec2(fHalfWidth - 109.0, 0.0));
	vcPointers.push_back(Vec2(fHalfWidth - 81.0, 0.0));
	pDrawMarker->DrawTriangles(vcPointers, Color4F(1.0f, 0.89f, 0.35f, 1.0f));
	vcPointers.clear();
	vcPointers.push_back(Vec2(fHalfWidth - 108.5, 0.5));
	vcPointers.push_back(Vec2(fHalfWidth, size.height));
	pPDrawLight->DrawLineStrip(vcPointers, 2.0, Color4B(254, 225, 10, 255));
	vcPointers.clear();
	vcPointers.push_back(Vec2(fHalfWidth, size.height));
	vcPointers.push_back(Vec2(fHalfWidth - 81.0, 0.0));
	vcPointers.push_back(Vec2(fHalfWidth - 62.0, 0.0));
	pDrawMarker->DrawTriangles(vcPointers, Color4F(0.76f, 0.67f, 0.3f, 1.0f));
	vcPointers.clear();
	vcPointers.push_back(Vec2(fHalfWidth, size.height));
	vcPointers.push_back(Vec2(fHalfWidth - 62.5, 0.5));
	pPDrawLight->DrawLineStrip(vcPointers, 1.0, Color4B(194, 171, 7, 255));

	vcPointers.clear();
	vcPointers.push_back(Vec2(fHalfWidth, size.height));
	vcPointers.push_back(Vec2(fHalfWidth + 109.0, 0.0));
	vcPointers.push_back(Vec2(fHalfWidth + 81.0, 0.0));
	pDrawMarker->DrawTriangles(vcPointers, Color4F(1.0f, 0.89f, 0.35f, 1.0f));
	vcPointers.clear();
	vcPointers.push_back(Vec2(fHalfWidth + 108.5, 0.5));
	vcPointers.push_back(Vec2(fHalfWidth, size.height));
	pPDrawLight->DrawLineStrip(vcPointers, 2.0, Color4B(254, 225, 10, 255));
	vcPointers.clear();
	vcPointers.push_back(Vec2(fHalfWidth, size.height));
	vcPointers.push_back(Vec2(fHalfWidth + 81.0, 0.0));
	vcPointers.push_back(Vec2(fHalfWidth + 62.0, 0.0));
	pDrawMarker->DrawTriangles(vcPointers, Color4F(0.76f, 0.67f, 0.3f, 1.0f));
	vcPointers.clear();
	vcPointers.push_back(Vec2(fHalfWidth, size.height));
	vcPointers.push_back(Vec2(fHalfWidth + 62.5, 0.5));
	pPDrawLight->DrawLineStrip(vcPointers, 1.0, Color4B(194, 171, 7, 255));

	pPlaneMarker->AddChild(pDrawMarker);
	pPlaneMarker->AddChild(pPDrawLight);
	return pPlaneMarker;
}

HmsAviPf::CHmsNode* CHmsAttitudeV2::GenPitchPointer(float fRadius)
{
	m_PitchPointer = CHmsNode::Create();
	m_PitchPointer->SetContentSize(Size(fRadius * 2.0, fRadius * 2.0));

	CHmsDrawNode *pCricle = CHmsDrawNode::Create();
	pCricle->DrawFan(HALF_VALUE(m_PitchPointer->GetContentSize()), 0.0, 360.0, fRadius, 100);
	CHmsClippingNode *pClipNode = CHmsClippingNode::create();
	pClipNode->setStencil(pCricle);
	m_PitchPointer->AddChild(pClipNode);

	int nTickNumber = (m_nShowNumber % 2 == 0) ? ((m_nShowNumber + 2) / 2) : ((m_nShowNumber + 1) / 2);

	for (int nIndex = 0; nIndex < nTickNumber; ++nIndex)
	{
		CHmsNode *pLine = GenTickText(fRadius, 4.0, 8.0);
		pLine->SetPosition(HALF_VALUE(m_PitchPointer->GetContentSize()));
		m_LongsTick.push_back(pLine);
		pClipNode->AddChild(pLine);
	}

	for (int nIndex = 0; nIndex < nTickNumber; ++nIndex)
	{
		CHmsNode *pLine = GenTickText(HALF_VALUE(fRadius), 4.0, fRadius * 0.25 + 8.0);
		pLine->SetPosition(HALF_VALUE(m_PitchPointer->GetContentSize()));
		m_ShortTick.push_back(pLine);
		pClipNode->AddChild(pLine);
	}

	m_pZeroTick = CHmsNode::Create();
	m_pZeroTick->SetContentSize(Size(fRadius * 4.0, 6.0));
	m_pZeroTick->SetAnchorPoint(0.5, 0.5);
	m_pZeroTick->SetPosition(HALF_VALUE(m_PitchPointer->GetContentSize()));
	m_pZeroTick->SetVisible(false);
	CHmsLightDrawNode *pDrawZero = CHmsLightDrawNode::Create();
	//pDrawZero->DrawSolidRect(0.0, 0.0, m_pZeroTick->GetContentSize().width, m_pZeroTick->GetContentSize().height, Color4F::WHITE);
	pDrawZero->DrawHorizontalLine(0.0, m_pZeroTick->GetContentSize().width, HALF_VALUE(m_pZeroTick->GetContentSize().height), 4.0, Color4F::WHITE);
	m_pZeroTick->AddChild(pDrawZero);
	pClipNode->AddChild(m_pZeroTick);

	m_FiveSpan = (fRadius * 2.0 - 10.0) / (nTickNumber + 1);
	return m_PitchPointer;
}

HmsAviPf::CHmsNode* CHmsAttitudeV2::GenTickText(float fLength, float fLineHigh, float txtLineSpan)
{
	CHmsNode *pTickText = CHmsNode::Create();
	pTickText->SetContentSize(Size(fLength, fLineHigh));
	pTickText->SetAnchorPoint(0.5, 0.5);
	pTickText->SetVisible(false);

	CHmsLightDrawNode *pLine = CHmsLightDrawNode::Create();
	pLine->DrawHorizontalLine(0.0, fLength, HALF_VALUE(fLineHigh), fLineHigh, Color4F::WHITE);
	pLine->SetTag(ASS_LINE);
	pTickText->AddChild(pLine);

	CHmsLabel *pTextLeft = CHmsGlobal::CreateLanguageLabel("00", 32.0f, Color4F::WHITE);
	pTextLeft->SetAnchorPoint(1.0f, 0.45f);
	pTextLeft->SetPosition(-txtLineSpan, HALF_VALUE(fLineHigh));
	pTextLeft->SetTag(ASS_TEXT_LEFT);
	pTickText->AddChild(pTextLeft);

	CHmsLabel *pTextRight = CHmsGlobal::CreateLanguageLabel("00", 32.0f, Color4F::WHITE);
	pTextRight->SetAnchorPoint(0.0f, 0.45f);
	pTextRight->SetPosition(fLength + txtLineSpan, HALF_VALUE(fLineHigh));
	pTextRight->SetTag(ASS_TEXT_RIGHT);
	pTickText->AddChild(pTextRight);

	return pTickText;
}

void CHmsAttitudeV2::SetPitch(float fPitch)
{
	float fValidPitch = HMS_MIN(HMS_MAX(fPitch, -90.0f), 90.0f);
	if (!FUNC_FLOAT_EQUAL(m_CurPitch, fValidPitch, REF_VALUE1))
	{
		m_CurPitch = fValidPitch;

		int nTickNumber = (m_nShowNumber % 2 == 0) ? ((m_nShowNumber + 2) / 2) : ((m_nShowNumber + 1) / 2);
		m_pZeroTick->SetVisible(false);
		for (int uIndex = 0; uIndex < nTickNumber; ++uIndex)
		{
			m_LongsTick[uIndex]->SetVisible(false);
			m_ShortTick[uIndex]->SetVisible(false);
		}

		float absPitch  = fabsf(fValidPitch);
		float fVSymbol  = (fValidPitch < 0.0f) ? -1.0f : 1.0f;
		float midValue = (absPitch - fabsf(fmodf(fValidPitch, 5.0f))) * fVSymbol;/*中间俯仰度数.*/
		float midOffset = -(fValidPitch - midValue) / 5.0f * m_FiveSpan;	/*中间刻度的偏移值.*/
		float btmValue  = midValue - (m_nShowNumber - 1) / 2 * 5.0;
		float bmtOffset = midOffset - (m_nShowNumber - 1) / 2 * m_FiveSpan;

		for (unsigned int uIndex = 0; uIndex < m_nShowNumber; ++uIndex)
			ShowTick(bmtOffset + uIndex * m_FiveSpan, btmValue + uIndex * 5.0, uIndex%nTickNumber);
	}
}

void CHmsAttitudeV2::ShowTick(float fOffset, float fValue, unsigned int uIndex)
{
	Vec2 centerPos = HALF_VALUE(m_PitchPointer->GetContentSize());
	centerPos.y += fOffset;

	if (FUNC_FLOAT_EQUAL(fValue, 0.0f, REF_VALUE1)) //==0
	{
		m_pZeroTick->SetPositionY(/*floorf*/(centerPos.y));
		m_pZeroTick->SetVisible(true);
	}
	else
	{
		CHmsNode *pTickLine = (CHmsNode*)(FUNC_FLOAT_EQUAL(fmodf(fValue, 10.0), 0.0, REF_VALUE1) ? m_LongsTick[uIndex] : m_ShortTick[uIndex]);
		pTickLine->SetPositionY(/*floorf*/(centerPos.y));

		std::string strSzValue = INT_TO_STRING(fValue, 1);
		CHmsLabel *pLeftValue  = (CHmsLabel*)pTickLine->GetChildByTag(ASS_TEXT_LEFT);
		CHmsLabel *pRightValue = (CHmsLabel*)pTickLine->GetChildByTag(ASS_TEXT_RIGHT);
		if (pLeftValue) { pLeftValue->SetString(strSzValue);  }
		if (pRightValue){ pRightValue->SetString(strSzValue); }

		pTickLine->SetVisible((fabsf(fValue) > 90.0) ? false : true);
	}
}

void CHmsAttitudeV2::SetRoll(float fRoll)
{
	if (!FUNC_FLOAT_EQUAL(m_CurRoll, fRoll, REF_VALUE1) && m_RollPointer)
	{
		m_RollPointer->SetRotation(fRoll);
		m_PitchPointer->SetRotation(-fRoll);
	}
}