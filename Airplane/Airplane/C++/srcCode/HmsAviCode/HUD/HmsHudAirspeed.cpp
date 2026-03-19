#include "HmsHudAirspeed.h"
#include "HmsMultLineDrawNode.h"
#include "base/HmsClippingNode.h"
USING_NS_HMS;

#define KnotKMH				1.852

static Color4F UnValidColor = { 1.0f, 0.0f, 0.0f, 0.3f };

CHmsHudAirspeed * CHmsHudAirspeed::Create(HmsAviPf::Size size)
{
	CHmsHudAirspeed *ret = new CHmsHudAirspeed();
	if (ret && ret->InitWithSize(size))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

CHmsHudAirspeed::CHmsHudAirspeed()
{
	m_AirSpeedValue = -MAX_AIRSPEED;
	m_CurUnValid = -1;
	m_pSpeedBar = nullptr;
	m_pSpeedU = nullptr;
	m_pSpeedH = nullptr;
	m_pSpeedD = nullptr;
	m_pSpeedPointer = nullptr; 
	m_pSelSpeed = nullptr;
}

CHmsHudAirspeed::~CHmsHudAirspeed()
{
}

bool CHmsHudAirspeed::InitWithSize(HmsAviPf::Size size)
{
	if (!CHmsNode::Init())
	{
		return false;
	}

	SetContentSize(size);
	/*获取当前空速值宽度和高度.*/
	Image img;
	img.initWithImageFile("res/Hud/NumberLap.png");
	m_DightSize.width  = (float)img.getWidth();
	m_DightSize.height = (float)img.getHeight();
	m_DightHeight = 36.0f;

	GenAirspeed();
	SetAirSpeed(0.0f);
	SetUnValid(1);

	return true;
}

void CHmsHudAirspeed::GenAirspeed()
{
	float fWidth  = m_sizeContent.width;
	float fHeight = m_sizeContent.height;

	/*空速表裁剪区域(限制空速表以外数据显示).*/
	CHmsDrawNode *pStencil = CHmsDrawNode::Create();
	if (pStencil)
	{
		pStencil->DrawSolidRect(-fWidth, -2.0f, fWidth, fHeight + 2.0f, Color4F::RED);
		CHmsClippingNode *pClipAirspeed = CHmsClippingNode::create();
		if (pClipAirspeed)
		{
			pClipAirspeed->setStencil(pStencil);
			AddChild(pClipAirspeed);

			/*空速条（指示线条、刻线和标识文本）.*/
			m_pSpeedBar = CHmsHudSpeedBar::Create(m_sizeContent);
			pClipAirspeed->AddChild((CHmsNode*)m_pSpeedBar);
		}		
	}

	if (!m_pSpeedBar)
	{
		return;
	}
	

	auto textBoxStencil = CHmsNode::Create();
	auto curBoxStencil = CHmsDrawNode::Create();
	curBoxStencil->SetAnchorPoint(Vec2(1.0f, 0.5f));
	curBoxStencil->SetPosition(m_sizeContent.width - 30.0, m_sizeContent.height / 2.0f);
	textBoxStencil->AddChild(curBoxStencil);
	auto curSubBoxStencil = CHmsDrawNode::Create();
	curSubBoxStencil->SetAnchorPoint(Vec2(1.0f, 0.5f));
	textBoxStencil->AddChild(curSubBoxStencil);
	m_pBoxMetricCurSpeedMask = curSubBoxStencil;
	m_pSpeedBar->AddMaskNode(textBoxStencil);

	/*空速指针.**************************/
#if 1
	float fTempLineWidth = 4.0f;
	auto SpeedBox = CHmsMultLineDrawNode::Create();

	if (!SpeedBox)
	{
		return;
	}

	{
		float fXLeft = -90.0f;
		float fXRight = -12.0f;
		float fYTop = 32.0f;
		float fYMid = 10.0f;
		Vec2 verts[] = {
			{ fXLeft, fYTop },
			{ fXRight, fYTop },
			{ fXRight, fYMid },
			{ 0.0f, 0.0f },
			{ fXRight, -fYMid },
			{ fXRight, -fYTop },
			{ fXLeft, -fYTop },		
		};

 		Vec2 move(-fXLeft, fYTop);
		for (auto & c:verts)
		{
			c += move;
		}
		SpeedBox->SetContentSize(Size(move.x + fTempLineWidth, move.y * 2 + fTempLineWidth));

		SpeedBox->DrawLineLoop(verts, HMS_ARRAYSIZE(verts), fTempLineWidth, Color4B(GetColor()));

		curBoxStencil->DrawSolidRect(verts[0].x, verts[0].y, verts[5].x, verts[5].y);
		std::vector<Vec2> vT;
		vT.push_back(verts[2]);
		vT.push_back(verts[3]);
		vT.push_back(verts[4]);
		curBoxStencil->SetContentSize(SpeedBox->GetContentSize());
		curBoxStencil->DrawTriangles(vT);
	}
	m_pSpeedPointer = SpeedBox;
#else
	m_pSpeedPointer = CHmsImageNode::Create("res/Hud/pointer.png");
#endif
	m_pSpeedPointer->SetAnchorPoint(Vec2(1.0f, 0.5f));
// 	m_pSpeedPointer->SetScaleX(1.15f);
// 	m_pSpeedPointer->SetScaleY(1.1f);
	m_pSpeedPointer->SetColor(Color3B::GREEN);
	m_pSpeedPointer->SetPosition(fWidth - 30.0f, fHeight*0.5f);
	AddChild((CHmsNode*)m_pSpeedPointer);
	{
		auto pointerSize = m_pSpeedPointer->GetContentSize();
#if 0
		m_pBoxMetricCurSpeed = CHmsImageNode::Create("res/Pfd/Airspeed/rectangle.png");
#else
		auto SubSpeedBox = CHmsMultLineDrawNode::Create();
		{
			float fXLeft = -120.0f;
			float fXRight = -12.0f;
			float fYTop = 18.0f;
			Vec2 verts[] = {
				{ fXLeft, fYTop },
				{ fXRight, fYTop },
				{ fXRight, -fYTop },
				{ fXLeft, -fYTop },
			};

			Vec2 move(-fXLeft, fYTop);
			for (auto & c : verts)
			{
				c += move;
			}

			auto tempSize = Size(move.x + fTempLineWidth, move.y * 2 + fTempLineWidth);
			SubSpeedBox->SetContentSize(tempSize);

			SubSpeedBox->DrawLineLoop(verts, HMS_ARRAYSIZE(verts), fTempLineWidth, Color4B(GetColor()));

			curSubBoxStencil->DrawSolidRect(verts[0].x, verts[0].y, verts[2].x, verts[2].y);
			curSubBoxStencil->SetContentSize(tempSize);
			curSubBoxStencil->SetPosition(curBoxStencil->GetPoistion() + Vec2(0, pointerSize.height / 2 + tempSize.height / 2.0f - fTempLineWidth));
		}
		m_pBoxMetricCurSpeed = SubSpeedBox;
#endif
		m_pBoxMetricCurSpeed->SetAnchorPoint(Vec2(1, 0));
		m_pBoxMetricCurSpeed->SetPosition(pointerSize.width, pointerSize.height - fTempLineWidth);
		m_pBoxMetricCurSpeed->SetColor(Color3B::GREEN);
		auto unit_kmh = CHmsGlobal::CreateLabel("kn", 21, true);
		unit_kmh->SetTextColor(Color4B::GREEN);
		unit_kmh->SetAnchorPoint(Vec2(1, 0.5));
		unit_kmh->SetPosition(m_pBoxMetricCurSpeed->GetContentSize().width - 12 - 12, m_pBoxMetricCurSpeed->GetContentSize().height / 2.0f);
		m_pLableMetricCurSpeed = CHmsGlobal::CreateLabel("---", 24, true);
		m_pLableMetricCurSpeed->SetAnchorPoint(Vec2(1, 0.5));
		m_pLableMetricCurSpeed->SetPosition(unit_kmh->GetPoistion() - Vec2(unit_kmh->GetContentSize().width + 4, 0));
		m_pLableMetricCurSpeed->SetTextColor(Color4B::GREEN);
		m_pBoxMetricCurSpeed->AddChild(unit_kmh);
		m_pBoxMetricCurSpeed->AddChild(m_pLableMetricCurSpeed);
		m_pSpeedPointer->AddChild(m_pBoxMetricCurSpeed);
	}

	/*空速值(个位).*/
	float fTempDightShowHeight = m_pSpeedPointer->GetContentSize().height - fTempLineWidth;
	Rect valRect = { 0, 0, m_DightSize.width, m_DightHeight };
	float fOffsetX = m_pSpeedPointer->GetContentSize().width - 22.0f;
	float fHalfBgkHight = fTempDightShowHeight * 0.5f;
	valRect.size.height = m_DightHeight * 2.0f;
	float fScaleParam = fTempDightShowHeight / (valRect.size.height + 6.0f);
	m_pSpeedU = CHmsImageNode::Create("res/Hud/NumberLap.png", valRect);
	if (m_pSpeedU)
	{
		m_pSpeedU->SetAnchorPoint(Vec2(1.0f, 0.49f));
		m_pSpeedU->SetScaleY(fScaleParam);
		m_pSpeedU->SetScaleX(fScaleParam);
		m_pSpeedU->SetColor(Color3B::GREEN);
		m_pSpeedU->SetPosition(fOffsetX, fHalfBgkHight);
		m_pSpeedPointer->AddChild((CHmsNode*)m_pSpeedU);
	}
	

	/*空速值(十位).*/
	fOffsetX -= m_DightSize.width * fScaleParam;
	valRect.size.height = m_DightHeight;
	m_pSpeedD = CHmsImageNode::Create("res/Hud/NumberLap.png", valRect);
	if (m_pSpeedD)
	{
		m_pSpeedD->SetAnchorPoint(Vec2(1.0f, 0.5f));
		m_pSpeedD->SetScaleY(fScaleParam);
		m_pSpeedD->SetScaleX(fScaleParam);
		m_pSpeedD->SetColor(Color3B::GREEN);
		m_pSpeedD->SetPosition(fOffsetX, fHalfBgkHight);
		m_pSpeedPointer->AddChild((CHmsNode*)m_pSpeedD);
	}
	

	/*空速值(百位).*/
	fOffsetX -= m_DightSize.width * fScaleParam;
	m_pSpeedH = CHmsImageNode::Create("res/Hud/NumberLap.png", valRect);
	if (m_pSpeedH)
	{
		m_pSpeedH->SetAnchorPoint(Vec2(1.0f, 0.5f));
		m_pSpeedH->SetScaleY(fScaleParam);
		m_pSpeedH->SetScaleX(fScaleParam);
		m_pSpeedH->SetColor(Color3B::GREEN);
		m_pSpeedH->SetPosition(fOffsetX, fHalfBgkHight);
		m_pSpeedPointer->AddChild((CHmsNode*)m_pSpeedH);
	}
	

	/*空速值(千位).*/
	// 	fOffsetX -= m_DightSize.width * fScaleParam*0.7f;
	// 	m_pSpeedT = CHmsImageNode::Create("res/Hud/NumberLap.png", valRect);
	// 	m_pSpeedT->SetAnchorPoint(Vec2(1.0f, 0.5f));
	// 	m_pSpeedT->SetScaleY(fScaleParam);
	// 	m_pSpeedT->SetScaleX(fScaleParam*0.75f);
	//  m_pSpeedT->SetColor(Color3B::GREEN);
	// 	m_pSpeedT->SetPosition(fOffsetX, fHalfBgkHight);
	// 	m_pSpeedPointer->AddChild((CHmsNode*)m_pSpeedT);

	/*选择空速值文本.*/
	m_pSelSpeed = CHmsLabel::createWithTTF("000", "fonts/msyhbd.ttc", 18.0f);
	if (m_pSelSpeed)
	{
		m_pSelSpeed->SetAnchorPoint(Vec2(0.5f, 0.5f));
		m_pSelSpeed->SetPosition(fWidth / 2.0f, 13.0f);
	}
	
	//AddChild((Node*)m_pSelSpeed);

	/*****************无效节点.****************************/
	m_pUnValid = CHmsMultLineDrawNode::Create();
	AddChild((CHmsNode*)m_pUnValid);
}

void CHmsHudAirspeed::SetAirSpeed(float fAirSpeedVlaue)
{
	float fCurVlaue = HMS_MIN(HMS_MAX(fAirSpeedVlaue, 0.0f), MAX_AIRSPEED);
	if (fabsf(m_AirSpeedValue - fAirSpeedVlaue) < 1e-2 || m_CurUnValid == 0){//有效数据才能更新.
		return;
	}

	/*取数.*/
	float fBasePosn = (m_DightSize.height - m_DightHeight)*0.5f;

	/*千位数值.*/
	unsigned int uThouNum = (unsigned int)floor(fCurVlaue * 0.001f);
	unsigned int uThouDig = HMS_MIN(uThouNum, 9);//千位数限制最大值9.
	float fRemVal = fCurVlaue - uThouNum * 1000.0f;
	/*百位数值.*/
	unsigned int uHundDig = HMS_MIN((unsigned int)floor(fRemVal * 0.01f), 9);
	fRemVal -= uHundDig * 100.0f;
	/*十位数值.*/
	unsigned int  uDecaDig = HMS_MIN((unsigned int)floor(fRemVal * 0.1f), 9);
	float fDecaVal = fRemVal - uDecaDig * 10.0f;
	/*个位数值.*/
	unsigned int  uUnitNum = HMS_MIN((unsigned int)floor(fDecaVal), 9);
	float fUnitVal = fDecaVal - uUnitNum;

	/*个位滚动.*/
	Rect texRect = { 0.0f, 0.0f, m_DightSize.width, 70.0f };
	texRect.origin.y = m_DightSize.height * 0.5f - (1.0f + (float)uUnitNum + fUnitVal) * m_DightHeight;
	if (m_pSpeedU)
	{
		m_pSpeedU->SetTextureRect(texRect);
	}
	/*十位滚动.*/
	float fDecaRoll = (uUnitNum < 9) ? 0.0f : (fDecaVal - 9.0f);
	texRect.size.height = m_DightHeight;
	texRect.origin.y = fBasePosn - ((float)uDecaDig + fDecaRoll) * m_DightHeight - 1.0f;
	m_pSpeedD->SetTextureRect(texRect);
	/*百位滚动.*/
	float fHundRoll = (uDecaDig >= 9) ? fDecaRoll : 0.0f;
	texRect.origin.y = fBasePosn - ((float)uHundDig + fHundRoll)* (m_DightHeight) - 1.0f;
	if (m_pSpeedH)
	{
		m_pSpeedH->SetTextureRect(texRect);
	}
	/*千位滚动.*/
	//float fThouRoll = (uThouDig == 9) ? 0.0f : ((uHundDig >= 9 && uDecaDig >=9 ) ? fDecaRoll : 0.0f);
	//texRect.origin.y = fBasePosn - ((float)uThouDig + fThouRoll)* (m_DightHeight);
	//m_pSpeedT->SetTextureRect(texRect);

	/*移动空速条.*/
	if (m_pSpeedBar)
	{
		m_pSpeedBar->SetValue(fCurVlaue, m_AirSpeedValue);
	}

	/*记录当前速度.*/
	m_AirSpeedValue = fCurVlaue;

	CHmsGlobal::SetLabelFormat(m_pLableMetricCurSpeed, "%d", std::lround((fCurVlaue / KnotKMH)));
}

void CHmsHudAirspeed::SetSelSpeed(float fSelSpeedVlaue)
{
	if (m_pSelSpeed && !(fabsf(m_SelSpeedValue-fSelSpeedVlaue) >= 1.0f))
	{
		/*设置选择空速值.*/
		std::stringstream osVal;
		osVal << (int)(fSelSpeedVlaue);
		m_pSelSpeed->SetString(osVal.str().c_str());
		/*更新选择空速位置.*/
		if (m_pSpeedBar)
		{
			m_pSpeedBar->SetMarkValue(MARK_SEL, fSelSpeedVlaue);
		}
		/*记录选择空速.*/
		m_SelSpeedValue = fSelSpeedVlaue;
	}
}

void CHmsHudAirspeed::SetUnValid(int unValid)
{
	if (m_pUnValid && m_CurUnValid != unValid)
	{
		m_CurUnValid = unValid;
		if (unValid == 1)//全部有效时.
		{
			m_pUnValid->SetVisible(false);
			return;
		}
		m_pUnValid->SetVisible(true);

		float fWide = m_sizeContent.width;
		float fHigh = m_sizeContent.height;
		Vec2 vertVec[4];

		m_pUnValid->clear();

		/*空速表是否有效(数值随右侧第二位为1则为有效，为0无效).*/
		if ((unValid & 0x01) == 0)
		{
			vertVec[0].x = 0.0f - 50;  vertVec[0].y = 1.0f;
			vertVec[1].x = fWide; vertVec[1].y = fHigh;
			vertVec[2].x = 0.0f - 50;  vertVec[2].y = fHigh;
			vertVec[3].x = fWide; vertVec[3].y = 1.0f;
			m_pUnValid->DrawLines(vertVec, 4, 4.0f, Color4B::RED);
			m_pUnValid->SetDrawColor(Color4F::RED);		
		}
	}
}

void CHmsHudAirspeed::MetricSwitch(bool bOn)
{
	m_pBoxMetricCurSpeed->SetVisible(bOn);
	m_pBoxMetricCurSpeedMask->SetVisible(bOn);

	this->ForceUpdate();
}
