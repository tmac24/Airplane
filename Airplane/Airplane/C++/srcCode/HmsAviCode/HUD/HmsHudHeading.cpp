#include "HmsHudHeading.h"
#include "HmsHudLine.h"
#include "HmsMultLineDrawNode.h"
#include "base/HmsClippingNode.h"
USING_NS_HMS;

CHmsHudHeading * CHmsHudHeading::Create(HmsAviPf::Size size)
{
	CHmsHudHeading *ret = new CHmsHudHeading();
	if (ret && ret->InitWithSize(size))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

CHmsHudHeading::CHmsHudHeading()
: m_TickNumber(8)/*双数且不能为10的整数倍：长短成对出现(两刻度间隔为10).*/
, m_CurHeading(-1)
, m_bTrueHead(false)
, m_bTrack(false)
, m_ePfdCompassDataMode(PFD_COMPASS_DATA_MODE::magneticHeading)
{
	m_pTickText = nullptr;
	m_pHeadingText = nullptr;
	m_pSignText = nullptr;
	m_pTrackText = nullptr;
}

CHmsHudHeading::~CHmsHudHeading()
{
}

bool CHmsHudHeading::InitWithSize(HmsAviPf::Size size)
{
	if (!CHmsNode::Init())
	{
		return false;
	}

	SetContentSize(size);
	m_TickSpan = m_sizeContent.width / (m_TickNumber - 2);

	GenHudHeading();
	SetHudHeading(0.0);
	return true;
}

HmsAviPf::CHmsImageNode * CHmsHudHeading::GenTick(bool bLong)
{
#if 0
	CHmsDrawNode *pLine = CHmsDrawNode::Create();
	if (bLong)
	{
		pLine->DrawVerticalLine(0.0f, 0.0f, m_sizeContent.height, 2.7f, Color4F::GREEN);
	}
	else
	{
		pLine->DrawVerticalLine(0.0f, m_sizeContent.height*0.25f, m_sizeContent.height * 0.75f, 2.7f, Color4F::GREEN);
	}
	return pLine;
#else
	CHmsHudVerticalLine * line = nullptr;
	if (bLong)
	{
		line = CHmsHudVerticalLine::CreateVerticalLine(4);
		line->SetHeight(m_sizeContent.height);
	}
	else
	{
		line = CHmsHudVerticalLine::CreateVerticalLine(4);
		line->SetHeight(m_sizeContent.height * 0.65);
	}
	line->SetAnchorPoint(Vec2(0.5, 0));
	line->SetColor(Color3B::GREEN);
	return line;
#endif
}

HmsAviPf::CHmsLabel* CHmsHudHeading::GenText()
{
	CHmsLabel *pLabel = CHmsLabel::createWithTTF("00", "fonts/msyhbd.ttc", 25.0f);
	if (pLabel)
	{
		pLabel->SetTextColor(Color4B::GREEN);
		pLabel->SetAnchorPoint(Vec2(0.5f, 0.0f));
		pLabel->SetPosition(Vec2(0.0f, m_sizeContent.height + 3));
	}

	
	return pLabel;
}

void CHmsHudHeading::GenHudHeading()
{
	HmsAviPf::Vec2 centerPos = { m_sizeContent.width / 2.0f, m_sizeContent.height / 2.0f };

	/*航向带显示条.*/
	CHmsClippingNode *pClip = CHmsClippingNode::create();
	pClip->SetContentSize(Size(m_sizeContent.width, m_sizeContent.height * 3.0f));
	this->AddChild(pClip);

	/*剪裁模板.*/
	CHmsDrawNode *pStencil = CHmsDrawNode::Create();
	pStencil->DrawSolidRect(-10.0f, 0.0, m_sizeContent.width+10.0f, m_sizeContent.height*2.0f, Color4F::RED);
	pClip->setStencil(pStencil);

// 	/*显示框裁剪区.*/
// 	CHmsDrawNode *pTxtFrame = CHmsDrawNode::Create();
// 	pTxtFrame->DrawSolidRect(
// 		(m_sizeContent.width - m_TickSpan) / 2.0f, m_sizeContent.height + 2.0f,
// 		(m_sizeContent.width + m_TickSpan) / 2.0f, m_sizeContent.height * 2.0f,
// 		Color4F::GREEN);
// 	ClippingNode *pClipShow = CHmsClippingNode::create();
// 	pClipShow->setStencil(pTxtFrame);
// 	pClipShow->setInverted(true);
// 	pClip->AddChild(pClipShow);

	/*刻线和文本管理节点.*/
	m_pTickText = CHmsNode::Create();
	m_pTickText->SetContentSize(Size(m_sizeContent.width, m_sizeContent.height*3.0f));
	pClip->AddChild(m_pTickText);

	/*刻度线和文本(绘制m_TickNumber+1条刻线，显示m_TickNumber-1刻线).*/
	for (int uIndex = -1; uIndex < m_TickNumber; ++uIndex)
	{
		if (uIndex % 2 != 0)
		{
			CHmsLabel *pText = GenText();
			pText->SetPositionX(uIndex * m_TickSpan);
			m_pTickText->AddChild(pText);
			m_Texts.push_back(pText);
		}

		auto *pLine = GenTick((uIndex % 2 == 0) ? false : true);
		pLine->SetPositionX(uIndex * m_TickSpan);
		m_pTickText->AddChild(pLine);
	}

	//显示框区域，不需要剪裁

	/*当前航向显示框.*/
#if 1
	CHmsMultLineDrawNode *pCurHeadingBox = CHmsMultLineDrawNode::Create();
	pCurHeadingBox->DrawOutterRect(38, -18, -38, 18,
		4,
		Color4F::GREEN);
	pCurHeadingBox->SetPosition(Vec2(centerPos.x, m_sizeContent.height * 2 + 30));
	this->AddChild(pCurHeadingBox);

	/*航向显示文本.*/
	m_pHeadingText = CHmsLabel::createWithTTF("000", "fonts/msyhbd.ttc", 32.0f);
	if (m_pHeadingText)
	{
		m_pHeadingText->SetTextColor(Color4B::GREEN);
		m_pHeadingText->SetAnchorPoint(Vec2(0.5f, 0.5f));
		pCurHeadingBox->AddChild(m_pHeadingText);
	}


	/*真航向与磁航向标示.*/
	m_pSignText = CHmsLabel::createWithTTF("M", "fonts/msyhbd.ttc", 24.0f);
	if (m_pSignText)
	{
		m_pSignText->SetTextColor(Color4B::GREEN);
		m_pSignText->SetAnchorPoint(Vec2(0.0f, 0.5f));
		m_pSignText->SetPosition(Vec2(50.0, 0));
		pCurHeadingBox->AddChild(m_pSignText);
	}

	/*航向与航迹显示.*/
	m_pTrackText = CHmsLabel::createWithTTF("HDG", "fonts/msyhbd.ttc", 24.0f);
	if (m_pTrackText)
	{
		m_pTrackText->SetTextColor(Color4B::GREEN);
		m_pTrackText->SetAnchorPoint(Vec2(1.0f, 0.5f));
		m_pTrackText->SetPosition(Vec2(-50.0, 0));
		pCurHeadingBox->AddChild(m_pTrackText);
	}
#else
	auto pCurHeadingBox = CHmsImageNode::Create("res/Hud/headingbox.png");
	pCurHeadingBox->SetColor(Color3B::GREEN);
	pCurHeadingBox->SetAnchorPoint(Vec2(0.5f, 0.5f));
	pCurHeadingBox->SetPosition(Vec2(centerPos.x, m_sizeContent.height * 2 + 25));
	this->AddChild(pCurHeadingBox);

	/*航向显示文本.*/
	m_pHeadingText = CHmsLabel::createWithTTF("000", "fonts/msyhbd.ttc", 24.0f);
	m_pHeadingText->SetTextColor(Color4B::GREEN);
	m_pHeadingText->SetAnchorPoint(Vec2(0.5f, 0.5f));
	m_pHeadingText->SetPosition(pCurHeadingBox->GetContentSize() / 2.0f);
	pCurHeadingBox->AddChild(m_pHeadingText);


	/*真航向与磁航向标示.*/
	m_pSignText = CHmsLabel::createWithTTF("M", "fonts/msyhbd.ttc", 24.0f);
	m_pSignText->SetTextColor(Color4B::GREEN);
	m_pSignText->SetAnchorPoint(Vec2(1.0f, 0.5f));
	m_pSignText->SetPosition(Vec2(-48.0f, pCurHeadingBox->GetContentSize().height/2.0f));
	pCurHeadingBox->AddChild(m_pSignText);
#endif
}

void CHmsHudHeading::SetHudHeading(float fHeading)
{
	/*限制到0-360范围内.*/
	float fUseHeading  = fHeading;
	float fCalcHeading = fmodf(fmodf(fUseHeading, 360.0f) + 360.0f, 360.0f);
	if (fabsf(m_CurHeading - fCalcHeading) < 0.001f || 0 == m_pTickText){
		return; 
	}
	
	/*显示范围.*/
	float fExtent = m_TickNumber* 5.0f;
	/*移动刻线(限制在10度范围内移动).*/;
	float fMoveDisance = fmodf(fmodf(fCalcHeading, 10.0f) / 5.0f * m_TickSpan, m_TickSpan*2.0f);
	m_pTickText->SetPositionX(-fMoveDisance);
	/*显示的第一个刻线数值.*/
	char szDight[32] = { 0 };
	int nFirst = (int)floorf((fCalcHeading - fExtent / 2.0f) / 10.0f);
	/*文本处理.*/
	std::list<HmsAviPf::CHmsLabel *>::iterator iterBeg = m_Texts.begin();
	for (; iterBeg != m_Texts.end(); ++iterBeg)
	{
		CHmsLabel *pText = (CHmsLabel *)(*iterBeg);
		if (pText)
		{
			sprintf(szDight, "%02d", ((nFirst++)+36)%36);
			pText->SetString(szDight);
		}
	}

	/*设置航向值.*/
	if (m_pHeadingText)
	{
		sprintf(szDight, "%03d", (int)round(fCalcHeading));
		m_pHeadingText->SetString(szDight);
	}
	
	/*保存当前航向值.*/
	m_CurHeading = fCalcHeading;
}

void CHmsHudHeading::SetTrueOrMagneticType(bool bTrue)
{
	if (m_pSignText)
	{
		m_pSignText->SetString(bTrue ? "T" : "M");
		m_bTrueHead = bTrue;
	}
}

void CHmsHudHeading::SetTrackOrHeadingType(bool bTrack)
{
	if (m_pTrackText)
	{
		m_pTrackText->SetString(bTrack ? "TRK" : "HDG");
		m_bTrack = bTrack;
	}
}

void CHmsHudHeading::CheckPfdCompassMode(PFD_COMPASS_DATA_MODE eMode)
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

void CHmsHudHeading::SetPfdCompassDataMode(PFD_COMPASS_DATA_MODE eMode)
{
	if (m_ePfdCompassDataMode != eMode)
	{
		SetPfdCompassDataMode(eMode);
	}
}
