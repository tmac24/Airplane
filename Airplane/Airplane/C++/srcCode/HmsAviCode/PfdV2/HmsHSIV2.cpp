#include "HmsHSIV2.h"
#include "HmsGlobal.h"
#include "base/HmsImage.h"
#include "display/HmsLightDrawNode.h"
#include "HmsRollLable.h"
#include "HmsPfdHeader.h"
#include "PfdV1/HmsHSIManager.h"
#include "ui/HmsUiImagePanel.h"
#include "FMS/HmsDuControlData.h"
#include "DataInterface/HmsDataBus.h"

USING_NS_HMS;

CHmsHSIV2 * CHmsHSIV2::Create(HmsAviPf::Size size)
{
	CHmsHSIV2 *ret = new CHmsHSIV2();
	if (ret && ret->InitWithSize(size))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

CHmsHSIV2::CHmsHSIV2()
: m_bTrueHeading(false)
, m_bTrack(false)
, m_bAutoScale(false)
, m_bObsMode(true)
, m_MidCrcRadius(0.0)
, m_BorderWide(4.0)
, m_CurHeading(0.0)
, m_CurCrsValue(0.0)
, m_CurSelValue(0.0)
, m_CurOffset(0.0)
, m_MaxOffset(1000.0)
, m_CrsShowTime(0.0)
, m_HdgShowTime(0.0)
, HmsUiEventInterface(this)
, m_ePfdCompassDataMode(PFD_COMPASS_DATA_MODE::magneticHeading)
{
	m_pHsiManager = new CHmsHSIManager(0);
}

CHmsHSIV2::~CHmsHSIV2()
{
	if (m_pHsiManager)
	{
		delete m_pHsiManager;
		m_pHsiManager = nullptr;
	}
}

bool CHmsHSIV2::InitWithSize(HmsAviPf::Size size)
{
	if (!CHmsNode::Init())
	{
		return false;
	}

	float fRadius = HALF_VALUE(HMS_MIN(size.width, size.height));
	float fWidth = fRadius + HMS_MIN(size.width, size.height);
	SetContentSize(Size(fWidth, fWidth));

	CHmsNode *pPlaneMarker = GenPlaneMarker(Size(32, 32));
	pPlaneMarker->SetAnchorPoint(0.5, 0.5);
	pPlaneMarker->SetPosition(HALF_VALUE(m_sizeContent));
	this->AddChild(pPlaneMarker);
	
	CHmsNode *pOuterTick = GenOutterTick(fRadius);
	pOuterTick->SetAnchorPoint(0.5, 0.5);
	pOuterTick->SetPosition(HALF_VALUE(m_sizeContent));
	this->AddChild(pOuterTick);

	m_pCricleDisk = GenCircleDisk(fRadius);
	m_pCricleDisk->SetAnchorPoint(0.5, 0.5);
	m_pCricleDisk->SetPosition(HALF_VALUE(m_sizeContent));
	this->AddChild(m_pCricleDisk);

	m_pCrsPointer = GenCrsPointer(fRadius, Color4B(251, 0, 244, 255), Color4B(251, 0, 244, 255));
	m_pCrsPointer->SetAnchorPoint(0.5, 0.5);
	m_pCrsPointer->SetPosition(HALF_VALUE(m_pCricleDisk->GetContentSize()));
	//m_pCricleDisk->AddChild(m_pCrsPointer);

	CHmsNode *pSelButton = GenSelectHead(Size(32.0, 20.0), fRadius, Color4B(0, 200, 200, 255));
	pSelButton->SetAnchorPoint(0.5, 0.0);
	pSelButton->SetPosition(HALF_VALUE(m_pCrsPointer->GetContentSize()));
	m_pCricleDisk->AddChild(pSelButton);

	CHmsNode *pOffsetCricle = GenOffsetPoint(HALF_VALUE(fRadius));
	pOffsetCricle->SetAnchorPoint(0.5, 0.5);
	pOffsetCricle->SetPosition(HALF_VALUE(m_pCrsPointer->GetContentSize()));
	m_pCrsPointer->AddChild(pOffsetCricle);

	Size staffSize(11.0, fRadius - 10.0);
	CHmsNode *pOffsetStaff = GenOffsetStaff(staffSize, 1.0, Color4B(251, 0, 244, 255), Color4B(251, 0, 244, 255), true);
	pOffsetStaff->SetAnchorPoint(0.5, 0.5);
	pOffsetStaff->SetPosition(HALF_VALUE(m_pCrsPointer->GetContentSize()));
	m_pCrsPointer->AddChild(pOffsetStaff);

	CHmsNode *pDisplayText = GenDisplayText(Size(100.0, 60.0));
	pDisplayText->SetAnchorPoint(0.5, 0.0);
	pDisplayText->SetPosition(HALF_VALUE(GetContentSize()) + Size(0.0, fRadius + 22.0));
	this->AddChild(pDisplayText);

	GenCrsHintText();
	if (m_pCrsHintText)
	{
		m_pCrsHintText->SetPosition(HALF_VALUE(m_sizeContent.width) + 200.0, pDisplayText->GetPoistion().y - 20.0);
		this->AddChild(m_pCrsHintText);
	}

	GenHdgHintText();
	if (m_pHdgHintText)
	{
		m_pHdgHintText->SetPosition(150.0, pDisplayText->GetPoistion().y - 20.0);
		this->AddChild(m_pHdgHintText);
	}

	GenOBSHintText();
	if (m_pOBSHintText)
	{
		m_pOBSHintText->SetTextColor(Color4B(0x80, 0x00, 0xFF, 0xFF));
		m_pOBSHintText->SetAnchorPoint(Vec2(0.5, 0.5));
		m_pOBSHintText->SetPosition(HALF_VALUE(m_sizeContent) + Size(48.0, -30.0));
		this->AddChild(m_pOBSHintText);
	}

	CHmsNode *pUnValid = GenValid(Size(140.0, 60.0));
	pUnValid->SetAnchorPoint(0.5, 0.0);
	pUnValid->SetPosition(HALF_VALUE(this->GetContentSize()) + Size(0.0, fRadius + 32.0));
	this->AddChild(pUnValid);

	m_DialRadius = fRadius;
	ValidSwitch(false);

	SetScheduleUpdate();
	return true;
}

HmsAviPf::CHmsNode* CHmsHSIV2::GenOutterTick(float fRadius)
{
	CHmsNode *pOuterTick = CHmsNode::Create();
	pOuterTick->SetContentSize(Size(fRadius*2.0, fRadius*2.0));

	Vec2 centerPos(fRadius, fRadius);
	CHmsLightDrawNode *pDrawTick = CHmsLightDrawNode::Create();

	for (int iIndex = 0; iIndex < 8; ++iIndex)
	{
		DrawSrcTick(pDrawTick, centerPos, iIndex * 45, fRadius + 4.0, fRadius + 20.0, 5.0, Color4B(255, 255, 255, 100));
	}

	pOuterTick->AddChild(pDrawTick);

	return pOuterTick;
}

HmsAviPf::CHmsNode* CHmsHSIV2::GenCircleDisk(float fRadius)
{
	CHmsNode *pCricleDisk = CHmsNode::Create();
	pCricleDisk->SetContentSize(Size(fRadius*2.0, fRadius*2.0));

	Vec2 centerPos(fRadius, fRadius);
	CHmsLightDrawNode *pDrawCricle = CHmsLightDrawNode::Create();
	pDrawCricle->DrawFan(centerPos, 0.0, 360.0, fRadius, 50, Color4F(0.0, 0.0, 0.0, 0.25));
	pCricleDisk->AddChild(pDrawCricle);

	for (int nAngle = 1; nAngle <= 36; nAngle++)
	{
		DrawSrcTick(pDrawCricle, centerPos, (float)nAngle * 10, fRadius - 20.0, fRadius - 4.0, 3.0, Color4B(255, 255, 255, 100));
		DrawSrcTick(pDrawCricle, centerPos, (float)(nAngle * 5.0 + (nAngle - 1) * 5.0), fRadius - 14.0, fRadius - 4.0, 3.0, Color4B(255, 255, 255, 100));
	}

	auto GenText = [](CHmsNode *pParentNode, Vec2 &center, float fAngle, float fRadius, const std::string &text)
	{
		float fRadian = ANGLE_TO_RADIAN(fAngle);
		Vec2 textPos = Vec2(sin(fRadian) * fRadius, cos(fRadian) * fRadius) + center;

		CHmsLabel *pText = CHmsGlobal::CreateLanguageLabel(text.c_str(), 32.0, Color4F::WHITE);
		if (pText)
		{

			pText->SetRotation(fAngle);
			pText->SetAnchorPoint(0.52f, 0.5f);
			pText->SetPosition(textPos);
			pParentNode->AddChild(pText);
		}
	};

	GenText(pCricleDisk, centerPos, 0.0, fRadius - 36.0, "N");
	GenText(pCricleDisk, centerPos, 90.0, fRadius - 36.0, "E");
	GenText(pCricleDisk, centerPos, 180.0, fRadius - 36.0, "S");
	GenText(pCricleDisk, centerPos, 270.0, fRadius - 36.0, "W");

	for (int nAngle = 30; nAngle < 360; nAngle+=30)
	{
		PFD_CONTINUE(nAngle % 90 == 0);
		GenText(pCricleDisk, centerPos, (float)nAngle, fRadius - 36.0, INT_TO_STRING(nAngle / 10, 1));
	}

	return pCricleDisk;
}

HmsAviPf::CHmsNode* CHmsHSIV2::GenCrsPointer(float fRadius, Color4B color, Color4B borderColor)
{
	CHmsNode *pCrsPointer= CHmsNode::Create();
	pCrsPointer->SetContentSize(Size(fRadius*2.0, fRadius*2.0));

	Vec2 centerPos(fRadius, fRadius);
	CHmsDrawNode *pHeaderTriangle = CHmsDrawNode::Create();
	CHmsLightDrawNode *pBorder = CHmsLightDrawNode::Create();

	/*以等边三角形方式绘制三角指针.*/
	float fTriangleWide = 100.0;
	float fTriangleHigh = 170.0;
	float fPtWidith = 11.0;
	float fBorderWide = 1.0;
	float fBorderOff = 10.0;

	Vec2 startTriangle, endTriangle;
	std::vector<Vec2> vcPoints;
	
	/*三角左弧.*/
	Vec2 centerLeft = centerPos + Vec2(-HALF_VALUE(fTriangleWide), fRadius - fBorderOff - HALF_VALUE(fTriangleHigh));
	for (int nAngle = -60; nAngle <= -12; nAngle+=6)
	{
		float fSinVal = sin(ANGLE_TO_RADIAN(nAngle));
		float fCosVal = cos(ANGLE_TO_RADIAN(nAngle));
		Vec2 curPos = centerLeft + Vec2(fCosVal * HALF_VALUE(fTriangleWide), (1 + fSinVal) * HALF_VALUE(fTriangleHigh));
		if (vcPoints.size() == 0)
		{
			startTriangle = curPos;
			vcPoints.push_back(Vec2(centerPos.x, curPos.y));
		}

		vcPoints.push_back(curPos);
	}
	/*三角右弧.*/
	Vec2 centerRight = centerPos + Vec2(0.0, fRadius - fBorderOff - HALF_VALUE(fTriangleHigh));
	for (int nAngle = 102; nAngle <= 150; nAngle += 6)
	{
		float fSinVal = sin(ANGLE_TO_RADIAN(nAngle));
		float fCosVal = cos(ANGLE_TO_RADIAN(nAngle));
		vcPoints.push_back(centerRight + Vec2((1 - fSinVal)* HALF_VALUE(fTriangleWide), (1 + fCosVal)* HALF_VALUE(fTriangleHigh)));
	}

	/*绘制指针上半部.*/
	{
		endTriangle = vcPoints[vcPoints.size() - 1];
		pHeaderTriangle->DrawTriangleFan(vcPoints, Color4F(color));
		pHeaderTriangle->DrawSolidRect(
			centerPos.x - HALF_VALUE(fPtWidith),
			centerPos.y + HALF_VALUE(fRadius) + 5.0,
			centerPos.x + HALF_VALUE(fPtWidith),
			startTriangle.y,
			Color4F(color));

		/*边.*/
		vcPoints.erase(vcPoints.begin());
		vcPoints.push_back(Vec2(centerPos.x + HALF_VALUE(fPtWidith) - HALF_VALUE(fBorderWide), endTriangle.y));
		vcPoints.push_back(Vec2(centerPos.x + HALF_VALUE(fPtWidith) - HALF_VALUE(fBorderWide), centerPos.y + HALF_VALUE(fRadius) + 5.0));
		vcPoints.push_back(Vec2(centerPos.x - HALF_VALUE(fPtWidith) + HALF_VALUE(fBorderWide), centerPos.y + HALF_VALUE(fRadius) + 5.0));
		vcPoints.push_back(Vec2(centerPos.x - HALF_VALUE(fPtWidith) + HALF_VALUE(fBorderWide), startTriangle.y));
		pBorder->DrawLineLoop((Vec2*)&vcPoints.at(0), (int)vcPoints.size(), 1.0, borderColor);
	}

	/*绘制中部圆.*/
	{
		m_MidCrcRadius = HALF_VALUE(fRadius);
		pBorder->DrawFanBorder(centerPos, 0.0f, 360.0f, HALF_VALUE(fRadius), 3.0f, 50, Color4F(0.14f, 0.42f, 0.16f, 1.0f));
	}

	/*绘制指针下半部.*/
	{
		pHeaderTriangle->DrawSolidRect(
			centerPos.x - HALF_VALUE(fPtWidith),
			fBorderOff * 2.0,
			centerPos.x + HALF_VALUE(fPtWidith),
			centerPos.y - HALF_VALUE(fRadius) - 5.0,
			Color4F(color));

		/*边.*/
		vcPoints.clear();
		vcPoints.push_back(Vec2(centerPos.x + HALF_VALUE(fPtWidith) - HALF_VALUE(fBorderWide), fBorderOff * 2.0));
		vcPoints.push_back(Vec2(centerPos.x + HALF_VALUE(fPtWidith) - HALF_VALUE(fBorderWide), centerPos.y - HALF_VALUE(fRadius) - 5.0));
		vcPoints.push_back(Vec2(centerPos.x - HALF_VALUE(fPtWidith) + HALF_VALUE(fBorderWide), centerPos.y - HALF_VALUE(fRadius) - 5.0));
		vcPoints.push_back(Vec2(centerPos.x - HALF_VALUE(fPtWidith) + HALF_VALUE(fBorderWide), fBorderOff * 2.0));
		pBorder->DrawLineLoop((Vec2*)&vcPoints.at(0), (int)vcPoints.size(), 1.0, borderColor);
	}

	pCrsPointer->AddChild(pHeaderTriangle);
	pCrsPointer->AddChild(pBorder);

	return pCrsPointer;
}

HmsAviPf::CHmsNode* CHmsHSIV2::GenPlaneMarker(HmsAviPf::Size size)
{
	CHmsNode *pPlaneMarker = CHmsNode::Create();
	pPlaneMarker->SetContentSize(size);

	CHmsLightDrawNode *pDrawNode = CHmsLightDrawNode::Create();
	pDrawNode->DrawHorizontalLine(0.0, size.width, HALF_VALUE(size.height), 4.0, Color4F::WHITE);
	pDrawNode->DrawHorizontalLine(FOURTH_VALUE(size.width), FOURTH_VALUE(size.width) * 3.0 + 1.0, 0.0, 4.0, Color4F::WHITE);
	pDrawNode->DrawVerticalLine(HALF_VALUE(size.width), 0.0, size.height, 5.0, Color4F::WHITE);

	pPlaneMarker->AddChild(pDrawNode);
	return pPlaneMarker;
}

HmsAviPf::CHmsNode* CHmsHSIV2::GenOffsetPoint(float fRadius)
{
	CHmsNode *pOffsetPointer = CHmsNode::Create();
	pOffsetPointer->SetContentSize(Size(fRadius*2.0, fRadius*2.0));


	auto DrawOffsetCricle = [](CHmsLightDrawNode *pNode, const Vec2 &pos)
	{
		pNode->DrawFanBorder(pos, 0.0, 360.0, 8.0, 4.0, 20, Color4F::WHITE);
	};

	CHmsLightDrawNode *pDrawNode = CHmsLightDrawNode::Create();

	float fSpan = fRadius / 3.0;
	Vec2 centerPos(fRadius, fRadius);

	DrawOffsetCricle(pDrawNode, centerPos + Vec2(fSpan, 0.0));
	DrawOffsetCricle(pDrawNode, centerPos + Vec2(2.0 * fSpan, 0.0));
	DrawOffsetCricle(pDrawNode, centerPos - Vec2(fSpan, 0.0));
	DrawOffsetCricle(pDrawNode, centerPos - Vec2(2.0 * fSpan, 0.0));

	m_OffsetScope.x = -2.0 * fSpan;
	m_OffsetScope.y = 2.0 * fSpan;

	pOffsetPointer->AddChild(pDrawNode);
	return pOffsetPointer;
}

HmsAviPf::CHmsNode* CHmsHSIV2::GenOffsetStaff(HmsAviPf::Size size, float fBorderWide, Color4B color, Color4B borderColor, bool bAutoScale)
{
	m_pOffsetStaff = CHmsNode::Create();
	m_pOffsetStaff->SetContentSize(size);

	CHmsLightDrawNode *pDrawNode = CHmsLightDrawNode::Create();

	pDrawNode->SetTag(ASS_LINE);
	pDrawNode->DrawSolidRect(0.0, 0.0, size.width, size.height, Color4F(color), false);

	/*边.*/
	std::vector<Vec2> vcPoints;
	vcPoints.push_back(Vec2(HALF_VALUE(fBorderWide), HALF_VALUE(fBorderWide)));
	vcPoints.push_back(Vec2(size.width - HALF_VALUE(fBorderWide), HALF_VALUE(fBorderWide)));
	vcPoints.push_back(Vec2(size.width - HALF_VALUE(fBorderWide), size.height - HALF_VALUE(fBorderWide)));
	vcPoints.push_back(Vec2(HALF_VALUE(fBorderWide), size.height - HALF_VALUE(fBorderWide)));
	pDrawNode->DrawLineLoop((Vec2*)&vcPoints.at(0), (int)vcPoints.size(), 1.0, borderColor);

	/*是否自动适应杆所在圆弧边.*/
	m_BorderWide = fBorderWide;
	m_bAutoScale = bAutoScale;
	m_StaffColor = color;
	m_StaffBorder = borderColor;

	m_pOffsetStaff->AddChild(pDrawNode);
	return m_pOffsetStaff;
}

HmsAviPf::CHmsNode* CHmsHSIV2::GenDisplayText(HmsAviPf::Size size)
{
	CHmsNode *pDisplayText = CHmsNode::Create();
	pDisplayText->SetContentSize(size);

	float fTriangleHigh = 10.0;

	CHmsLightDrawNode *pBackground = CHmsLightDrawNode::Create();
	pBackground->DrawSolidRect(0.0, fTriangleHigh, size.width, size.height, PFD_POINTER_BK_COLOR, false);

	std::vector<Vec2> vcPoints;
	vcPoints.push_back(Vec2(HALF_VALUE(size.width) - fTriangleHigh, fTriangleHigh));
	vcPoints.push_back(Vec2(HALF_VALUE(size.width), 0.0));
	vcPoints.push_back(Vec2(HALF_VALUE(size.width) + fTriangleHigh, fTriangleHigh));
	pBackground->DrawTriangles(vcPoints, PFD_POINTER_BK_COLOR);

	/*边.*/
	Vec2 borderPoints[7];
	borderPoints[0] = Vec2(0.0, fTriangleHigh);
	borderPoints[1] = Vec2(HALF_VALUE(size.width) - fTriangleHigh, fTriangleHigh);
	borderPoints[2] = Vec2(HALF_VALUE(size.width), 0.0);
	borderPoints[3] = Vec2(HALF_VALUE(size.width) + fTriangleHigh, fTriangleHigh);
	borderPoints[4] = Vec2(size.width, fTriangleHigh);
	borderPoints[5] = Vec2(size.width, size.height);
	borderPoints[6] = Vec2(0.0, size.height);
	pBackground->DrawLineLoop(borderPoints, 7, 2.0, Color4B::WHITE);

	/*值.*/
	m_pHeadingText = CHmsGlobal::CreateLanguageLabel("000", 32.0, Color4F::WHITE, true);
	if (m_pHeadingText)
	{
		m_pHeadingText->SetAnchorPoint(0.0, 0.5);
		m_pHeadingText->SetPosition(10.0, fTriangleHigh + HALF_VALUE(size.height - fTriangleHigh));
	}

	CHmsLabel *pUnitText = CHmsGlobal::CreateLanguageLabel("\302\260", 32.0, Color4F::WHITE, true);
	if (pUnitText)
	{
		pUnitText->SetAnchorPoint(1.0, 0.5);
		pUnitText->SetPosition(size.width - 10.0, fTriangleHigh + HALF_VALUE(size.height - fTriangleHigh));
	}
	
	m_pHeadingType = CHmsGlobal::CreateLanguageLabel("M", 32.0, Color4F::WHITE, true);
	if (m_pHeadingType)
	{
		m_pHeadingType->SetAnchorPoint(0.0, 0.5);
		m_pHeadingType->SetPosition(size.width + 10.0, fTriangleHigh + HALF_VALUE(size.height - fTriangleHigh));
	}

	m_pTrackType = CHmsGlobal::/*CreateLanguageLabel*/CreateLabel("HDG", 32.0, Color4F::WHITE, true);
	if (m_pTrackType)
	{
		m_pTrackType->SetAnchorPoint(1.0, 0.5);
		m_pTrackType->SetPosition(-10.0, fTriangleHigh + HALF_VALUE(size.height - fTriangleHigh));
	}

	pDisplayText->AddChild(pBackground);
	if (pUnitText)
	{
		pDisplayText->AddChild(pUnitText);
	}
	if (m_pHeadingType)
	{
		pDisplayText->AddChild(m_pHeadingType);
	}
	if (m_pTrackType)
	{
		pDisplayText->AddChild(m_pTrackType);
	}
	if (m_pHeadingText)
	{
		pDisplayText->AddChild(m_pHeadingText);
	}
	return pDisplayText;
}

HmsAviPf::CHmsNode* CHmsHSIV2::GenCrsHintText()
{
	m_pCrsHintText = CHmsGlobal::CreateLabel("CRS 000", 24.0, true);
	return m_pCrsHintText;
}

HmsAviPf::CHmsNode* CHmsHSIV2::GenHdgHintText()
{
	m_pHdgHintText = CHmsGlobal::CreateLabel("HDG 000", 24.0, true);
	return m_pHdgHintText;
}

HmsAviPf::CHmsNode* CHmsHSIV2::GenOBSHintText()
{
	m_pOBSHintText = CHmsGlobal::CreateLabel("OBS", 20.0, true);
	return m_pOBSHintText;
}

HmsAviPf::CHmsNode* CHmsHSIV2::GenSelectHead(HmsAviPf::Size size, float fRadius, Color4B color)
{
	m_pSelectHead = CHmsNode::Create();
	m_pSelectHead->SetContentSize(size + Size(0.0, fRadius));

	Vec2 ButtonPos[7];
	ButtonPos[0] = Vec2(0.0, fRadius + 4.0);
	ButtonPos[1] = Vec2(0.0, fRadius + size.height);
	ButtonPos[2] = Vec2(FOURTH_VALUE(size.width), fRadius + size.height);
	ButtonPos[3] = Vec2(HALF_VALUE(size.width), fRadius + THIRD_VALUE(size.height));
	ButtonPos[4] = Vec2(FOURTH_VALUE(size.width) * 3.0, fRadius + size.height);
	ButtonPos[5] = Vec2(size.width, fRadius + size.height);
	ButtonPos[6] = Vec2(size.width, fRadius + 4.0);

	CHmsLightDrawNode *pButton = CHmsLightDrawNode::Create();
	pButton->DrawLineLoop(ButtonPos, 7, 3.0, color);
	m_pSelectHead->AddChild(pButton);

	return m_pSelectHead;
}

void CHmsHSIV2::SetHeading(float fHeading)
{
	float fCurHeading = ANGLE_NORMAL_360(fHeading);
	if (!FUNC_FLOAT_EQUAL(m_CurHeading, fCurHeading, REF_VALUE1) && m_pCricleDisk)
	{
		m_CurHeading = fCurHeading;
		m_pCricleDisk->SetRotation(-fCurHeading);
		if (m_pHeadingText)
		{
			m_pHeadingText->SetString(INT_TO_STRING(round(fCurHeading), 3));
		}
	}
}

void CHmsHSIV2::SetTrueOrMagneticType(bool bTrueHead)
{
	if (m_pHeadingType)
	{
		m_pHeadingType->SetString(bTrueHead ? "T" : "M");
		m_bTrueHeading = bTrueHead;
	}
}

void CHmsHSIV2::SetPfdCompassDataMode(PFD_COMPASS_DATA_MODE eMode)
{
	m_ePfdCompassDataMode = eMode;
	switch (m_ePfdCompassDataMode)
	{
	case PFD_COMPASS_DATA_MODE::trueTrack:
		SetTrueOrMagneticType(true);
		SetTrackOrHeadingType(true);
		break;
	case PFD_COMPASS_DATA_MODE::trueHeading:
		SetTrueOrMagneticType(true);
		SetTrackOrHeadingType(false);
		break;
	case PFD_COMPASS_DATA_MODE::magneticTrack:
		SetTrueOrMagneticType(false);
		SetTrackOrHeadingType(true);
		break;
	case PFD_COMPASS_DATA_MODE::magneticHeading:
		SetTrueOrMagneticType(false);
		SetTrackOrHeadingType(false);
		break;
	default:
		break;
	}
}

void CHmsHSIV2::CheckPfdCompassMode(PFD_COMPASS_DATA_MODE eMode)
{
	if (m_ePfdCompassDataMode != eMode)
	{
		SetPfdCompassDataMode(eMode);
	}
}

void CHmsHSIV2::SetTrackOrHeadingType(bool bTrack)
{
	if (m_pTrackType)
	{
		m_pTrackType->SetString(bTrack ? "TRK" : "HDG");
		m_bTrack = bTrack;
	}
}

void CHmsHSIV2::SetVor(float fVorValue)
{
	float curVor = ANGLE_NORMAL_360(fVorValue);
	if (!FUNC_FLOAT_EQUAL(m_CurCrsValue, curVor, REF_VALUE1) && m_pCrsPointer && m_pCrsHintText)
	{
		m_CurCrsValue = curVor;
		CHmsGlobal::SetUTF8LabelFormat(m_pCrsHintText, "CRS  %03d\302\260", (int)curVor);
		m_CrsShowTime = 2000;
		m_pCrsPointer->SetRotation(curVor);
		m_pCrsPointer->ForceUpdate();
	}
}

void CHmsHSIV2::SetSelectHead(float fSelValue)
{
	float curSel = ANGLE_NORMAL_360(fSelValue);
	if (!FUNC_FLOAT_EQUAL(m_CurSelValue, curSel, REF_VALUE1) && m_pSelectHead)
	{
		m_CurSelValue = curSel;
		if (m_pHdgHintText)
		{
			CHmsGlobal::SetUTF8LabelFormat(m_pHdgHintText, "HDG  %03d\302\260", (int)curSel); 
			m_HdgShowTime = 2000;
			m_pSelectHead->SetRotation(curSel);
			m_pHdgHintText->ForceUpdate();
		}
		
	}
}

void CHmsHSIV2::SetOffset(float fOffset)
{
	float fScale = HMS_MIN(1.0, fabsf(fOffset) / m_MaxOffset);
	float fSymbol = (fOffset < 0.0) ? -1.0 : 1.0;
	float fPixsel = fScale * m_OffsetScope.y;
	if (m_pOffsetStaff && m_pCrsPointer && !FUNC_FLOAT_EQUAL(m_CurOffset, fOffset, 1.0))
	{
		m_CurOffset = fOffset;
		if (m_bAutoScale)
		{
			float fStaffHigh = sqrtf(pow(m_MidCrcRadius, 2) - pow(fPixsel, 2)) * 2.0;
			ReDrawStaff(fStaffHigh);
		}
		m_pOffsetStaff->SetPosition(HALF_VALUE(m_pCrsPointer->GetContentSize()) + Size(fPixsel * fSymbol, 0.0));
	}
}

void CHmsHSIV2::SetOffsetRatio(float Ratio)
{
	float fScale = HMS_MIN(1.0, fabsf(Ratio) / 2.0);
	float fSymbol = (Ratio < 0.0) ? -1.0 : 1.0;
	float fPixsel = fScale * m_OffsetScope.y;
	float fOffset = fScale * m_MaxOffset;
	if (m_pOffsetStaff && m_pCrsPointer&& !FUNC_FLOAT_EQUAL(m_CurOffset, fOffset, 1.0))
	{
		m_CurOffset = fOffset;
		if (m_bAutoScale)
		{
			float fStaffHigh = sqrtf(pow(m_MidCrcRadius, 2) - pow(fPixsel, 2)) * 2.0;
			ReDrawStaff(fStaffHigh);
		}
		m_pOffsetStaff->SetPosition(HALF_VALUE(m_pCrsPointer->GetContentSize()) + Size(fPixsel * fSymbol, 0.0));
	}
}

void CHmsHSIV2::SetMaxOffset(float fMaxOffset)
{
	m_MaxOffset = fMaxOffset;
}

void CHmsHSIV2::ReDrawStaff(float fHeight)
{
	if (m_pOffsetStaff)
	{
		Size size = m_pOffsetStaff->GetContentSize();
		CHmsLightDrawNode *pDrawNode = (CHmsLightDrawNode*)m_pOffsetStaff->GetChildByTag(ASS_LINE);
		pDrawNode->clear();
		pDrawNode->DrawSolidRect(0.0, HALF_VALUE(size.height - fHeight), size.width, HALF_VALUE(size.height + fHeight), Color4F(m_StaffColor), false);

		/*边.*/
		float fBaseY = HALF_VALUE(size.height - fHeight);
		std::vector<Vec2> vcPoints;
		vcPoints.push_back(Vec2(HALF_VALUE(m_BorderWide), fBaseY + HALF_VALUE(m_BorderWide)));
		vcPoints.push_back(Vec2(size.width - HALF_VALUE(m_BorderWide), fBaseY + HALF_VALUE(m_BorderWide)));
		vcPoints.push_back(Vec2(size.width - HALF_VALUE(m_BorderWide), fBaseY + fHeight - HALF_VALUE(m_BorderWide)));
		vcPoints.push_back(Vec2(HALF_VALUE(m_BorderWide), fBaseY + fHeight - HALF_VALUE(m_BorderWide)));
		pDrawNode->DrawLineLoop((Vec2*)&vcPoints.at(0), (int)vcPoints.size(), 1.0, m_StaffBorder);
	}
}

void CHmsHSIV2::DrawSrcTick(CHmsLightDrawNode *pDrawNode, Vec2 &center, float fAngle, float fInnRadius, float fOutRadius, float fLineWidth, Color4B color)
{
	float fRadian = ANGLE_TO_RADIAN(fAngle);
	float fSinRad = sin(fRadian);
	float fCosRad = cos(fRadian);

	Vec2 linePos[2];
	linePos[0] = Vec2(fSinRad * fInnRadius, fCosRad * fInnRadius) + center;
	linePos[1] = Vec2(fSinRad * fOutRadius, fCosRad * fOutRadius) + center;

	pDrawNode->DrawLineStrip(linePos, 2, fLineWidth, color);
}

bool CHmsHSIV2::ValidSwitch(bool bValid)
{
	bool bRet = CHmsValidV2::ValidSwitch(bValid);
	if (bRet && m_pCricleDisk && m_pCrsPointer)
	{
		if (bValid){
			m_pCricleDisk->RemoveChild(m_pCrsPointer);
			m_pCricleDisk->SetScheduleUpdate();
		}else{
			m_pCricleDisk->AddChild(m_pCrsPointer);
			m_pCricleDisk->SetUnSchedule();
		}
	}

	return bRet;
}


bool CHmsHSIV2::OnPressed(const Vec2 & posOrigin)
{
	PFD_RETURN_VAL(0 == m_pHsiManager || m_bUnValid, false);
	
	Vec2 center = HALF_VALUE(this->GetContentSize());
	bool inInnerCircle = m_pHsiManager->ContainsPoint(center, HALF_VALUE(m_DialRadius), posOrigin);
	bool inMiddleCircle = m_pHsiManager->ContainsPoint(center, m_DialRadius, posOrigin);
	bool inOutterCircle = m_pHsiManager->ContainsPoint(center, HALF_VALUE(m_DialRadius) * 3.0, posOrigin);
	m_PressPoint = posOrigin;

	bool bPressed = false;
	if (inInnerCircle){
		bPressed = true;
		m_ModType = MOD_OBS;

		bool bOBS = true;
		CHmsDuControlData *ctrlData = CHmsGlobal::GetInstance()->GetDataBus()->GetDuControlData();
		if (ctrlData)
		{
			bOBS = !ctrlData->IsObsMode();
		}
		ModifyCtrlData(bOBS, m_CurCrsValue);
	}else if(!inInnerCircle && inMiddleCircle){
		bPressed = true;
		m_ModType = MOD_CRS;
	}else if (!inMiddleCircle && inOutterCircle){
		bPressed = true;
		m_ModType = MOD_SELECT;
	}

	return bPressed;
}

void CHmsHSIV2::OnMove(const Vec2 & posOrigin)
{
	float rotateAngle = m_pHsiManager->CalTwoPointsAngle(HALF_VALUE(this->GetContentSize()), m_PressPoint, posOrigin);

	if (rotateAngle >= 1 || rotateAngle <= -1)
	{
		m_PressPoint = posOrigin;
		
		switch (m_ModType)
		{
		case MOD_OBS:   break;
		case MOD_CRS:	ModifyCtrlData(true, m_CurCrsValue + rotateAngle); break;
		case MOD_SELECT:ModifySelHeading(m_CurSelValue + rotateAngle); break;
		case MOD_UNKNOW:
		default:break;
		}
	}
}

void CHmsHSIV2::OnReleased(const Vec2 & posOrigin)
{
	m_ModType = MOD_UNKNOW;
}

void CHmsHSIV2::ModifyCtrlData(bool bOBs, float fCourse)
{
	CHmsDuControlData *ctrlData = CHmsGlobal::GetInstance()->GetDataBus()->GetDuControlData();
	if (ctrlData)
	{
		ctrlData->SetIsOBSMode(bOBs);
		ctrlData->SetCurCourse(fCourse);
	}
}

void CHmsHSIV2::ModifySelHeading(float fHeading)
{
	CHmsDuControlData *ctrlData = CHmsGlobal::GetInstance()->GetDataBus()->GetDuControlData();
	if (ctrlData)
	{
		ctrlData->SetSelectHeading(fHeading);
	}
}

void CHmsHSIV2::SetOBSMode(bool bOBs)
{
	if (m_bObsMode == bOBs)
		return;

	if (m_pOBSHintText)
	{
		m_pOBSHintText->SetVisible(bOBs);
		m_pOBSHintText->ForceUpdate();
	}
	m_bObsMode = bOBs;
}

void CHmsHSIV2::Update(float delta)
{
	if (m_CrsShowTime > 0)
	{
		m_CrsShowTime -= delta * 1000;
		if (m_pCrsHintText && !m_pCrsHintText->IsVisible())
		{
			m_pCrsHintText->SetVisible(true);
			m_pCrsHintText->ForceUpdate();
		}
	}
	else
	{
		if (m_pCrsHintText)
		{
			m_pCrsHintText->SetVisible(false);
		}
	}

	if (m_HdgShowTime > 0)
	{
		m_HdgShowTime -= delta * 1000;
		if (m_pHdgHintText && !m_pHdgHintText->IsVisible())
		{
			m_pHdgHintText->SetVisible(true);
			m_pHdgHintText->ForceUpdate();
		}
	}
	else
	{
		if (m_pHdgHintText)
		{
			m_pHdgHintText->SetVisible(false);
		}
	}
}
