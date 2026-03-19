#include "HmsHudAttitude.h"
#include "HmsLadderDrawNode.h"
USING_NS_HMS;

#define HUD_LINE_NUMBER (5) /*应为大于等于3的奇数值：显示的总条数.*/
#define FLOAT_ISEQUAL(VAL1,VAL2,COMVAL) ((fabsf(VAL1 - VAL2) < COMVAL) ? true : false)
#define MATH_PI         (3.1415926f)
/*浮点数定值.*/
#define FLOAT_CONST_ONE   (1.0f)
#define FLOAT_CONST1      (1e-2)
#define FLOAT_MULTIP      (1.2f)
/*线类型*/
#define LINE_ROTATE1      (97)
#define LINE_ROTATE2      (98)
#define LINE_FIXED        (99)
#define MIDDLE_SPAN       (60.0f)
/*中间间隔距离.*/

#define DEGREETORAD(degree)        (MATH_PI/180.0f*degree)

CHmsHudAttitude * CHmsHudAttitude::Create(HmsAviPf::Size size)
{
	CHmsHudAttitude *ret = new CHmsHudAttitude();
	if (ret && ret->InitWithSize(size))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

CHmsHudAttitude::CHmsHudAttitude()
: m_PitchValue(-720.0f)
, m_RollValue(-720.0f)
, m_CmmdValue(0.0f)
, m_FivePixels(100.0f)
{
}

CHmsHudAttitude::~CHmsHudAttitude()
{
}

bool CHmsHudAttitude::InitWithSize(HmsAviPf::Size size)
{
	if (!CHmsNode::Init())
	{
		return false;
	}

	SetContentSize(size);
	GenHudAttitude();
	SetHudPitch(0.0f);
	return true;
}


HmsAviPf::CHmsNode* CHmsHudAttitude::GenZeroLine(float vOffset)
{
	CHmsNode *pNode = CHmsNode::Create();

	CHmsImageNode *pHorLineL = CHmsImageNode::Create("res/Hud/horLine.png");
	if (pHorLineL)
	{
		pHorLineL->SetAnchorPoint(Vec2(1.0f, 0.0f));
		pHorLineL->SetTag(LINE_ROTATE1);
		pHorLineL->SetScaleX(FLOAT_MULTIP*2.6f);
		pHorLineL->SetPosition(-vOffset, 0.0f);
		pNode->AddChild(pHorLineL);
	}
	

	CHmsImageNode *pHorLineR = CHmsImageNode::Create("res/Hud/horLine.png");
	if (pHorLineR)
	{
		pHorLineR->SetAnchorPoint(Vec2(0.0f, 0.0f));
		pHorLineR->SetTag(LINE_ROTATE2);
		pHorLineR->SetScaleX(FLOAT_MULTIP*2.6f);
		pHorLineR->SetPosition(vOffset, 0.0f);
		pNode->AddChild(pHorLineR);
	}
	

	return pNode;
}

HmsAviPf::CHmsNode* CHmsHudAttitude::GenSolidLine(float vOffset)
{
	CHmsNode *pNode = CHmsNode::Create();

	/*左侧水平线(可旋转).*/
	CHmsImageNode *pHorLineL = CHmsImageNode::Create("res/Hud/horLine.png");
	if (pHorLineL)
	{
		pHorLineL->SetAnchorPoint(Vec2(1.0f, 0.0f));
		pHorLineL->SetTag(LINE_ROTATE1);
		pHorLineL->SetScaleX(FLOAT_MULTIP);
		pHorLineL->SetPosition(-vOffset, 0.0f);
		pNode->AddChild(pHorLineL);
	}
	

	/*左侧竖线(固定线).*/
	CHmsImageNode *pVerLineL = CHmsImageNode::Create("res/Hud/verline.png");
	if (pVerLineL)
	{
		pVerLineL->SetAnchorPoint(Vec2(0.0f, 0.9f));
		pVerLineL->SetTag(LINE_FIXED);
		pVerLineL->SetScaleX(2.0f);
		pVerLineL->SetPosition(-vOffset - pHorLineL->GetContentSize().width*FLOAT_MULTIP - 2.0f, 0.0f);
		pNode->AddChild(pVerLineL);
	}
	

	/*右侧水平线(可旋转).*/
	CHmsImageNode *pHorLineR = CHmsImageNode::Create("res/Hud/horLine.png");
	if (pHorLineR)
	{
		pHorLineR->SetAnchorPoint(Vec2(0.0f, 0.0f));
		pHorLineR->SetTag(LINE_ROTATE2);
		pHorLineR->SetScaleX(FLOAT_MULTIP);
		pHorLineR->SetPosition(vOffset, 0.0f);
		pNode->AddChild(pHorLineR);
	}
	

	/*右侧竖线(固定线).*/
	CHmsImageNode *pVerLineR = CHmsImageNode::Create("res/Hud/verline.png");
	if (pVerLineR)
	{
		pVerLineR->SetAnchorPoint(Vec2(1.0f, 0.9f));
		pVerLineR->SetTag(LINE_FIXED);
		pVerLineR->SetScaleX(2.0f);
		pVerLineR->SetPosition(vOffset + pHorLineR->GetContentSize().width*FLOAT_MULTIP + 2.0f, 0.0f);
		pNode->AddChild(pVerLineR);
	}
	

	/*计算文本偏移位置.*/
	m_TextOffset = vOffset + pHorLineL->GetContentSize().width*FLOAT_MULTIP;
	return pNode;
}

HmsAviPf::CHmsNode* CHmsHudAttitude::GenDotteLine(float vOffset)
{
	CHmsNode *pNode = CHmsNode::Create();

	/*左侧水平虚线(可旋转).*/
	CHmsImageNode *pHorLineL = CHmsImageNode::Create("res/Hud/xuhorline.png");
	if (pHorLineL)
	{
		pHorLineL->SetAnchorPoint(Vec2(1.0f, 0.0f));
		pHorLineL->SetTag(LINE_ROTATE1);
		pHorLineL->SetScaleX(FLOAT_MULTIP);
		pHorLineL->SetPosition(-vOffset, 0.0f);
		pNode->AddChild(pHorLineL);
	}
	

	/*左侧竖线(固定线).*/
	CHmsImageNode *pVerLineL = CHmsImageNode::Create("res/Hud/verline.png");
	if (pVerLineL)
	{
		pVerLineL->SetAnchorPoint(Vec2(0.0f, 0.0f));
		pVerLineL->SetTag(LINE_FIXED);
		pVerLineL->SetScaleX(2.0f);
		pVerLineL->SetPosition(-vOffset - 2.0f, 0.0f);
		pNode->AddChild(pVerLineL);
	}
	

	/*右侧水平虚线(可旋转).*/
	CHmsImageNode *pHorLineR = CHmsImageNode::Create("res/Hud/xuhorline.png");
	if (pHorLineR)
	{
		pHorLineR->SetAnchorPoint(Vec2(0.0f, 0.0f));
		pHorLineR->SetTag(LINE_ROTATE2);
		pHorLineR->SetScaleX(FLOAT_MULTIP);
		pHorLineR->SetPosition(vOffset, 0.0f);
		pNode->AddChild(pHorLineR);
	}
	

	/*右侧竖线(固定线).*/
	CHmsImageNode *pVerLineR = CHmsImageNode::Create("res/Hud/verline.png");
	if (pVerLineR)
	{
		pVerLineR->SetAnchorPoint(Vec2(0.0f, 0.0f));
		pVerLineR->SetTag(LINE_FIXED);
		pVerLineR->SetScaleX(2.0f);
		pVerLineR->SetPosition(vOffset, 0.0f);
		pNode->AddChild(pVerLineR);
	}
	

	return pNode;
}

void CHmsHudAttitude::GenHudAttitude()
{
	HmsAviPf::Vec2 centerPos = { m_sizeContent.width / 2.0f, m_sizeContent.height / 2.0f };

	/*姿态俯仰刻线.*/
	m_pPitch = CHmsClippingNode::create();
	m_pPitch->SetContentSize(m_sizeContent);
 	m_pPitch->SetAnchorPoint(Vec2(0.5f, 0.5f));
 	m_pPitch->SetPosition(centerPos);
	this->AddChild(m_pPitch);

	/*正方形背景.*/
	Color4F bkColor = { 0.0, 1.0, 0.0, 0.2f };
	float fBkWidth  = m_sizeContent.width;
	float fBkHeigth = m_sizeContent.height;
// 	CHmsDrawNode *pBkNode = CHmsDrawNode::Create();
// 	pBkNode->DrawSolidRect(-fBkWidth*0.25f-5.0f, -fBkHeigth*0.2f, fBkWidth*1.2f+15.0f, fBkHeigth*1.2f, bkColor);
// 	AddChild(pBkNode);

	/*圆形模板(下方有倾斜，裁剪区往下偏移一定距离).*/
	float dRadius = sqrtf(centerPos.x*centerPos.x + centerPos.y*centerPos.y);
	CHmsDrawNode *pStencil = CHmsDrawNode::Create();
	//pStencil->DrawSolidRect(0.0f, -10.0f, m_sizeContent.width, m_sizeContent.height+2.0f, Color4F::RED);
	pStencil->DrawFan(Vec2(centerPos.x, centerPos.y-10.0f), 0.0f, 360.0f, dRadius-5.0f, 50, Color4F::RED);
	m_pPitch->setStencil(pStencil);

#if 0
 	/*0刻度线.*/
	m_pZeroLine = GenZeroLine(MIDDLE_SPAN);
	m_pZeroLine->SetAnchorPoint(Vec2(0.5f, 0.5f));
	m_pZeroLine->SetPositionX(m_sizeContent.width / 2.0f);
	m_pZeroLine->SetVisible(false);
	m_pPitch->AddChild(m_pZeroLine);

	/*刻度线及文本.*/
	unsigned int uIndex = 0;
	for (uIndex = 0; uIndex < HUD_LINE_NUMBER+2; ++uIndex)
	{
		/*整数刻度线.*/
		Node *pPostive = GenSolidLine(MIDDLE_SPAN);
		pPostive->SetAnchorPoint(Vec2(0.5f, 0.5f));
		pPostive->SetPositionX(m_sizeContent.width / 2.0f);
		pPostive->SetVisible(false);
		m_pPitch->AddChild(pPostive);
		m_pPostiveLine.push_back(pPostive);
		/*负数刻度线.*/
		Node *pNagetive = GenDotteLine(MIDDLE_SPAN);
		pNagetive->SetAnchorPoint(Vec2(0.5f, 0.5f));
		pNagetive->SetPositionX(m_sizeContent.width / 2.0f);
		pNagetive->SetVisible(false);
		m_pPitch->AddChild(pNagetive);
		m_pNgativeLine.push_back(pNagetive);
		/*左侧刻度文本.*/
		Label *pLabelL = CHmsLabel::createWithTTF("0", "fonts/msyhbd.ttc", 18.0f);
		pLabelL->SetTextColor(Color4B::GREEN);
		pLabelL->SetAnchorPoint(Vec2(1.0f, 0.5f));
		m_pPitch->AddChild(pLabelL);
		m_pLeftLabel.push_back(pLabelL);
		/*右侧刻度文本.*/
		Label *pLabelR = CHmsLabel::createWithTTF("0", "fonts/msyhbd.ttc", 18.0f);
		pLabelR->SetTextColor(Color4B::GREEN);
		pLabelR->SetAnchorPoint(Vec2(0.0f, 0.5f));
		m_pPitch->AddChild(pLabelR);
		m_pRigtLabel.push_back(pLabelR);
	}
#else
	const float c_fHeightPerPitch = 62 / 5.0f;

	auto ladder = CHmsLadderDrawNode::Create();
	ladder->SetLineArea(22, 83, 2);
	ladder->SetPitchPerHeight(c_fHeightPerPitch);
	ladder->SetLadderScaleHeight(12);

	for (int i = 5; i < 90; i += 5)
	{
		ladder->AddSkyLadder(i);
		ladder->AddGroundLadder(-i);
	}

	ladder->AddSky90();
	ladder->AddGround90();

	m_pPitch->AddChild(ladder);
#endif

	/*飞机图标.*/
	CHmsImageNode *pAircraft = CHmsImageNode::Create("res/Hud/aircraft.png");
	if (pAircraft)
	{
		pAircraft->SetAnchorPoint(Vec2(0.5f, 1.0f));
		pAircraft->SetPosition(centerPos.x, centerPos.y);
		this->AddChild(pAircraft);
	}
	

	/*飞行指引.*/
	m_CmdLoad = CHmsImageNode::Create("res/Hud/flight_director.png");
	if (m_CmdLoad)
	{
		m_CmdLoad->SetAnchorPoint(Vec2(0.5f, 0.5f));
		m_CmdLoad->SetPosition(centerPos.x, centerPos.y + 28.0f);
		this->AddChild(m_CmdLoad);
	}


	/*滚转刻度线.*/
	float initRadius = dRadius + 70.0f;
	Vec2  tickPos;
	CHmsNode *pTickNode = CHmsNode::Create();
	for (int nDegree = -30; nDegree <= 30; nDegree += 10)
	{
		float fSinVal = sinf(DEGREETORAD(nDegree));
		float fCosVal = cosf(DEGREETORAD(nDegree));
		bool  bZero = (nDegree == 0) ? true : false;
		tickPos.x = centerPos.x + initRadius * fSinVal;
		tickPos.y = centerPos.y - initRadius * fCosVal;
		CHmsImageNode *pTick = CHmsImageNode::Create(bZero ? ("res/Hud/verline2.png") : ("res/Hud/verline.png"));
		if (pTick)
		{
			pTick->SetAnchorPoint(Vec2(0.5f, 0.0f));
			pTick->SetPosition(tickPos);
			pTick->SetScale(2.0f, bZero ? 2.5f : 1.5f);
			pTick->SetRotation(-nDegree);
			pTickNode->AddChild(pTick);
		}
		
	}
	this->AddChild(pTickNode);

	/*滚转指针*/
	CHmsImageNode *pRollImage = CHmsImageNode::Create("res/Hud/roll_pointer2.png");
	if (pRollImage)
	{
		pRollImage->SetAnchorPoint(Vec2(0.0f, 0.0f));
		if (m_pRoll.get())
		{
			m_pRoll = CHmsNode::Create();
			m_pRoll->SetContentSize(Size(pRollImage->GetContentSize().width, dRadius + 100.0f));
			m_pRoll->SetAnchorPoint(Vec2(0.5f, 1.0f));
			m_pRoll->SetPosition(centerPos.x, centerPos.y);
			m_pRoll->AddChild(pRollImage);
			this->AddChild(m_pRoll);
		}		
	}
	
}

void CHmsHudAttitude::HudDrawLine(float fOffset, float fValue, unsigned int uIndex)
{
	Vec2 centerPos = { m_sizeContent.width / 2.0f, m_sizeContent.height / 2.0f };

	centerPos.y = centerPos.y + fOffset;
	if (FLOAT_ISEQUAL(fValue, 0.0f, FLOAT_CONST1)) //==0
	{
		//m_pZeroLine->SetPositionY(centerPos.y);
		//m_pZeroLine->SetVisible(true);
	}
	else
	{
// 		Node *pNode = (Node*)((fValue > 0.0f) ? m_pPostiveLine[uIndex] : m_pNgativeLine[uIndex]);
// 		pNode->SetPositionY(centerPos.y);
// 		if (fValue < 0.0f)
// 		{
// 			CHmsImageNode *pLeft = (Sprite*)pNode->GetChildByTag(LINE_ROTATE1);
// 			CHmsImageNode *pRight = (Sprite*)pNode->GetChildByTag(LINE_ROTATE2);
// 			if (pLeft) { pLeft->SetRotation(fValue);   }
// 			if (pRight){ pRight->SetRotation(-fValue); }
// 		}
// 		pNode->SetVisible(true);
	}
}

void CHmsHudAttitude::HudDrawText(float fOffset, float fValue, unsigned int nIndex)
{
// 	Label *pLeftLabel = (Label*)m_pLeftLabel[nIndex];
// 	Label *pRigtLabel = (Label*)m_pRigtLabel[nIndex];
// 	if (!pLeftLabel || !pRigtLabel){ return; }
// 
// 	Vec2 centerPos = { m_sizeContent.width / 2.0f, m_sizeContent.height / 2.0f + 1.0f };
// 	centerPos.y  = floorf(centerPos.y + fOffset);
// 
// 	/*设置左侧文本值.*/
// 	char szTextBuf[256] = { 0 };
// 	float xPos = centerPos.x - m_TextOffset - 3.0f;
// 	float yPos = (fValue >= 0.0f) ? centerPos.y : (centerPos.y - fabsf(m_TextOffset)*tan(DEGREETORAD(-fValue / (2.0f))));
// 	sprintf_s(szTextBuf, "%02d", (unsigned int)fValue);
// 	pLeftLabel->SetString(szTextBuf);
// 	pLeftLabel->SetPosition(xPos, yPos);
// 	pLeftLabel->SetVisible(FLOAT_ISEQUAL(fValue, 0.0f, FLOAT_CONST1) ? false : true);
// 	/*设置右侧文本值.*/
// 	memset(szTextBuf, 0x00, 256);
// 	sprintf_s(szTextBuf, "%02d", (unsigned int)fValue);
// 	xPos = centerPos.x + m_TextOffset + 3.0f;
// 	pRigtLabel->SetString(szTextBuf);
// 	pRigtLabel->SetPosition(xPos, yPos);
// 	pRigtLabel->SetVisible(FLOAT_ISEQUAL(fValue, 0.0f, FLOAT_CONST1) ? false : true);
}

void CHmsHudAttitude::SetHudPitch(float fpitch)
{
	float fValidPitch = HMS_MIN(HMS_MAX(fpitch, -90.0f), 90.0f);
	if (!FLOAT_ISEQUAL(m_PitchValue, fValidPitch, FLOAT_CONST1))
	{
		m_PitchValue = fValidPitch;
		//m_pZeroLine->SetVisible(false);
// 		for (unsigned int uIndex = 0; uIndex < HUD_LINE_NUMBER+2; ++uIndex)
// 		{
// 			Node *pNodeP = (Node*)m_pPostiveLine[uIndex];
// 			pNodeP->SetVisible(false);
// 			Node *pNodeN = (Node*)m_pNgativeLine[uIndex];
// 			pNodeN->SetVisible(false);
// 		}
		
		/*取值.*/
		float absPitch = fabsf(fValidPitch);
		float fSmybol = (fValidPitch < 0.0f) ? -1.0f : 1.0f;
		/*中间俯仰度数.*/
		float midValue = (absPitch - fabsf(fmodf(fpitch, 5.0f))) * fSmybol;
		/*计算中间刻度的偏移值.*/
		float fOffset = -(fValidPitch - midValue) / 5.0f * m_FivePixels;
		float fSymbol = 1.0f;
		float fMutiply = 1.0f;
		unsigned int nSplitIndex = (HUD_LINE_NUMBER + 1) / 2;//1~nSplitIndex为上方刻度,nSplitIndex+1~HUD_LINE_NUMBER + 1为下方刻度.
		HudDrawLine(fOffset, midValue, 0);
		HudDrawText(fOffset, midValue, 0);
		for (unsigned int uIndex = 1; uIndex < HUD_LINE_NUMBER + 2; ++uIndex)
		{
			fSmybol  = (uIndex <= nSplitIndex) ? 1.0f : -1.0f;
			fMutiply = (uIndex <= nSplitIndex) ? uIndex : (uIndex - nSplitIndex);
			HudDrawLine(fOffset + m_FivePixels*fSmybol*fMutiply, midValue + 5.0f*fSmybol*fMutiply, uIndex);
			HudDrawText(fOffset + m_FivePixels*fSmybol*fMutiply, midValue + 5.0f*fSmybol*fMutiply, uIndex);
		}
	}
}

void CHmsHudAttitude::SetRoll(float fRoll)
{
	if (!FLOAT_ISEQUAL(m_RollValue, fRoll, FLOAT_CONST1))
	{
		/*设置滚转角.*/
		if (m_pRoll) { m_pRoll->SetRotation(-fRoll); }
		/*设置俯仰角.*/
		if (m_pPitch){ m_pPitch->SetRotation(-fRoll);}
		m_RollValue = fRoll;
	}
}