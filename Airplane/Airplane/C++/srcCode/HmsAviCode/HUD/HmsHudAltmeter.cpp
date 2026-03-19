#include "HmsHudAltmeter.h"
#include "HmsHudLine.h"
#include "base/HmsClippingNode.h"
#include "../Calculate/CalculateZs.h"
#include "HmsMultLineDrawNode.h"
#define ALTMETER_USER_20 (1)
USING_NS_HMS;

static Color4F UnValidColor = { 1.0f, 0.0f, 0.0f, 0.3f };

CHmsHudAltMeter * CHmsHudAltMeter::Create(HmsAviPf::Size size)
{
	CHmsHudAltMeter *ret = new CHmsHudAltMeter();
	if (ret && ret->InitWithSize(size))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

CHmsHudAltMeter::CHmsHudAltMeter()
{
	m_AltValue = -10000.0f;
	m_tickSpan = 50.f;//50刻度间隔绘制刻度线，每100刻度绘制文本.
	m_tickNumber = 14;
	m_pAltSm = 0;
	m_bInit = true;

	m_pAltSm = nullptr;
	m_pAltTP = nullptr;
	m_pAltTN = nullptr;
	m_pAltHP = nullptr;
	m_pAltHN = nullptr;
	m_pAltDP = nullptr;
	m_pAltDN = nullptr;
	m_pAltUP = nullptr;
	m_pAltUN = nullptr;
}

CHmsHudAltMeter::~CHmsHudAltMeter()
{
}

bool CHmsHudAltMeter::InitWithSize(HmsAviPf::Size size)
{
	if (!CHmsNode::Init())
	{
		return false;
	}

	SetContentSize(size);

	/*获取百位和十位宽度和高度.*/
	Image img;
	img.initWithImageFile("res/Hud/NumberLap.png");
	m_HighBySize.width = (float)img.getWidth();
	m_HighBySize.height = (float)img.getHeight();
	m_DightHeight = 36.0f;

	/*获取个位宽度和高度.*/
#if ALTMETER_USER_20
	img.initWithImageFile("res/Hud/single.png");
#else
	img.initWithImageFile("res/Hud/single05.png");
#endif
	m_SingleSize.width = (float)img.getWidth();
	m_SingleSize.height = (float)img.getHeight();

	GenHudAltmeter();
	SetUnValid(1);
	//SetSelAltmeter(0.0f);
	SetAltMeter(0.0f);

	return true;
}

void CHmsHudAltMeter::GenHudAltmeter()
{
	float fWidth  = m_sizeContent.width;
	float fHeight = m_sizeContent.height;
	unsigned int labelNum = (m_tickNumber + 1) / 2;

	/*高度表裁剪区域(限制高度表以外数据显示).*/
	CHmsDrawNode *pStencil = CHmsDrawNode::Create();
	pStencil->DrawSolidRect(0.0f, -2.0f, fWidth*2.0f, fHeight + 2.0f, Color4F::RED);
	CHmsClippingNode *pClipAltmeter = CHmsClippingNode::create();
	pClipAltmeter->setStencil(pStencil);
	AddChild(pClipAltmeter);

	auto textBoxStencil = CHmsNode::Create();
	auto curBoxStencil = CHmsDrawNode::Create();
	curBoxStencil->SetAnchorPoint(Vec2(0.0f, 0.5f));
	curBoxStencil->SetPosition(15.0f, m_sizeContent.height / 2.0f);;
	textBoxStencil->AddChild(curBoxStencil);
	auto curSubBoxStencil = CHmsDrawNode::Create();
	curSubBoxStencil->SetAnchorPoint(Vec2(0.0f, 0.5f));
	textBoxStencil->AddChild(curSubBoxStencil);
	m_pBoxMetricCurAltMask = curSubBoxStencil;
	
	//pClipAltmeter->AddChild(textBoxStencil);

	/*文本刻度裁剪区（隐藏指针下面的文本值）.*/
// 	CHmsImageNode *pPtStencil = CHmsImageNode::Create("res/Hud/pointerbg.png");
// 	pPtStencil->SetAnchorPoint(Vec2(0.0f, 0.5f));
// 	pPtStencil->SetScaleY(1.05f);
// 	pPtStencil->SetPosition(15.0f, m_sizeContent.height / 2.0f);
	CHmsClippingNode *pClipText = CHmsClippingNode::create();
//	pClipText->setStencil(pPtStencil);
	pClipText->setStencil(textBoxStencil);
	pClipText->setInverted(true);
	pClipText->SetContentSize(m_sizeContent);
	pClipAltmeter->AddChild(pClipText);

	/*刻度条（包括刻度条和文本）.*/
	m_pAltBar = CHmsNode::Create();
	m_pAltBar->SetContentSize(m_sizeContent);
	pClipText->AddChild(m_pAltBar);

	/*刻度线.*/
	float fLineSpan = m_sizeContent.height / (float)(m_tickNumber - 1);

	m_pAltLines = CHmsNode::Create();
	for (unsigned int uIndex = 0; uIndex < m_tickNumber; uIndex++)
	{
		auto line = CHmsHudHorizontalLine::CreateHorizontalLine(4);
		line->SetColor(Color3B::GREEN);
		if (uIndex%2 == 0)
		{
			line->SetWidth(30);
		}
		else
		{
			line->SetWidth(20);
		}
		line->SetAnchorPoint(Vec2(0.0f, 0.5f));
		line->SetPosition(Vec2(0, uIndex * fLineSpan));
		m_pAltLines->AddChild(line);
	}
	m_pAltBar->AddChild(m_pAltLines);

	/*刻度值文本.*/
	m_pLabelNode = CHmsNode::Create();
	m_pLabelNode->SetContentSize(m_sizeContent);
	m_pAltBar->AddChild(m_pLabelNode);

	/*创建文本刻度值.*/
	m_PressurePixels = fLineSpan / m_tickSpan;
	for (unsigned int uIndex = 0; uIndex < labelNum; uIndex++)
	{
		std::stringstream txtStream;
		txtStream << (int)(uIndex * m_tickSpan * 2 - m_tickSpan*(labelNum - 1));
		std::string strText = txtStream.str();
		CHmsLabel *pLabel = CHmsLabel::createWithTTF(strText.c_str(), "fonts/msyhbd.ttc", 25.0f);
		if (pLabel)
		{
			pLabel->SetTextColor(Color4B::GREEN);
			pLabel->SetAnchorPoint(Vec2(0.0f, 0.5f));
			pLabel->SetPosition(Vec2(35.0f, uIndex * 2.0f * fLineSpan - m_sizeContent.height / 2.0f));
			m_pLabelNode->AddChild(pLabel);
			m_pAltLabel.push_back(pLabel);
		}		
	}

	/*高度指针.*/
#if 0
	m_pPointer = CHmsImageNode::Create("res/Hud/altpointer.png");
	m_pPointer->SetScaleY(1.05f);
#else
	float fTempLineWidth = 4.0f;
	auto SpeedBox = CHmsMultLineDrawNode::Create();
	{
		float fXLeft = 125.0f;
		float fXRight = 12.0f;
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

		Vec2 move(0, fYTop);
		for (auto & c : verts)
		{
			c += move;
		}
		SpeedBox->SetContentSize(Size(fXLeft + fTempLineWidth, move.y * 2 + fTempLineWidth));

		SpeedBox->DrawLineLoop(verts, HMS_ARRAYSIZE(verts), fTempLineWidth, Color4B(GetColor()));

		curBoxStencil->DrawSolidRect(verts[0].x, verts[0].y, verts[5].x, verts[5].y);
		std::vector<Vec2> vT;
		vT.push_back(verts[2]);
		vT.push_back(verts[3]);
		vT.push_back(verts[4]);
		curBoxStencil->SetContentSize(SpeedBox->GetContentSize());
		curBoxStencil->DrawTriangles(vT);
	}
	m_pPointer = SpeedBox;
#endif
	if (!m_pPointer)
	{
		return;
	}
	m_pPointer->SetColor(Color3B::GREEN);
	m_pPointer->SetAnchorPoint(Vec2(0.0f, 0.5f));
	m_pPointer->SetPosition(15.0f, m_sizeContent.height / 2.0f);
	AddChild(m_pPointer);
	if (1)
	{
		auto pointerSize = m_pPointer->GetContentSize();
#if 0
		m_pBoxMetricCurAlt = CHmsImageNode::Create("res/Hud/altpointerMeter.png");
#else
		auto SubSpeedBox = CHmsMultLineDrawNode::Create();
		{
			float fXLeft = 125.0f;
			float fXRight = 12.0f;
			float fYTop = 18.0f;
			Vec2 verts[] = {
				{ fXLeft, fYTop },
				{ fXRight, fYTop },
				{ fXRight, -fYTop },
				{ fXLeft, -fYTop },
			};

			Vec2 move(0, fYTop);
			for (auto & c : verts)
			{
				c += move;
			}
			auto tempSize = Size(fXLeft + fTempLineWidth, move.y * 2 + fTempLineWidth);
			SubSpeedBox->SetContentSize(tempSize);

			SubSpeedBox->DrawLineLoop(verts, HMS_ARRAYSIZE(verts), fTempLineWidth, Color4B(GetColor()));


			curSubBoxStencil->DrawSolidRect(verts[0].x, verts[0].y, verts[2].x, verts[2].y);
			curSubBoxStencil->SetContentSize(tempSize);
			curSubBoxStencil->SetPosition(curBoxStencil->GetPoistion() + Vec2(0, pointerSize.height / 2 + tempSize.height/2.0f - fTempLineWidth));
		}
		m_pBoxMetricCurAlt = SubSpeedBox;
#endif
		m_pBoxMetricCurAlt->SetColor(Color3B::GREEN);
		m_pBoxMetricCurAlt->SetAnchorPoint(Vec2(1.0, 0));
		m_pBoxMetricCurAlt->SetPosition(pointerSize.width, pointerSize.height - fTempLineWidth);
		auto unit_m = CHmsGlobal::CreateLabel("ft", 21, true);
		unit_m->SetTextColor(Color4B::GREEN);
		unit_m->SetAnchorPoint(Vec2(1, 0.5));
		unit_m->SetPosition(m_pBoxMetricCurAlt->GetContentSize().width - 12, m_pBoxMetricCurAlt->GetContentSize().height / 2.0f);
		m_pLableMetricCurAlt = CHmsGlobal::CreateLabel("0000", 24, true);
		m_pLableMetricCurAlt->SetAnchorPoint(Vec2(1, 0.5));
		m_pLableMetricCurAlt->SetPosition(unit_m->GetPoistion() - Vec2(unit_m->GetContentSize().width + 8, 0));
		m_pLableMetricCurAlt->SetTextColor(Color4B::GREEN);
		m_pBoxMetricCurAlt->AddChild(unit_m);
		m_pBoxMetricCurAlt->AddChild(m_pLableMetricCurAlt);
		m_pPointer->AddChild(m_pBoxMetricCurAlt);
	}

	/*数字.*/
#if ALTMETER_USER_20
	m_SingleUseHeight = 80.0f;
#else
	m_SingleUseHeight = m_SingleSize.height / 24.0f * 2.0f;
#endif

	CreateDight(true, true);
	CreateDight(false, false);

	/*****************无效节点.****************************/
	m_pUnValid = CHmsMultLineDrawNode::Create();
	AddChild(m_pUnValid);
}

void CHmsHudAltMeter::GenDightPlace(HmsAviPf::CHmsImageNode **pPalce, float fScale, float x, float y, const std::string &szRes, bool bDisplay)
{
	Rect valRect = { 0, 0, m_HighBySize.width, m_DightHeight };
	*pPalce = CHmsImageNode::Create(szRes, valRect);
	(*pPalce)->SetAnchorPoint(Vec2(1.0f, 0.5f));
	(*pPalce)->SetScaleY(fScale);
#if ALTMETER_USER_20
	(*pPalce)->SetScaleX(fScale);
#else
	(*pPalce)->SetScaleX(fScale*0.72f);
#endif
	(*pPalce)->SetColor(Color3B::GREEN);
	(*pPalce)->SetPosition(x, y);
	(*pPalce)->SetVisible(bDisplay);
	m_pPointer->AddChild((CHmsNode*)(*pPalce));
}

void CHmsHudAltMeter::CreateDight(bool bPositive, bool bDisplay)
{
	/*数字偏移位置.*/
	float fHeightHalf = (m_pPointer->GetContentSize().height - 4) * 0.5f;
	float fOffsetWide = m_pPointer->GetContentSize().width - 10.0f;

	/*高度值(个位).*/
	CHmsImageNode **pAltUnit = (bPositive ? &m_pAltUP : &m_pAltUN);
#if ALTMETER_USER_20
	const char *szUintResce = bPositive ? ("res/Hud/single.png") : ("res/Hud/single_negative.png");
#else
	const char *szUintResce = bPositive ? ("res/Hud/single05.png") : ("res/Hud/single05_negative.png");
#endif
	HmsAviPf::Rect valRect = { 0, 0, m_SingleSize.width, m_SingleUseHeight };
	float fScaleParam = m_pPointer->GetContentSize().height / (valRect.size.height + 12.0f);
	(*pAltUnit) = CHmsImageNode::Create(szUintResce, valRect);
	if ((*pAltUnit))
	{
		(*pAltUnit)->SetColor(Color3B::GREEN);
		(*pAltUnit)->SetAnchorPoint(Vec2(1.0f, 0.5f));
		(*pAltUnit)->SetScale(fScaleParam*0.95);
		(*pAltUnit)->SetScaleX(fScaleParam*0.95);
		(*pAltUnit)->SetPosition(fOffsetWide, fHeightHalf);
		(*pAltUnit)->SetVisible(bDisplay);
		m_pPointer->AddChild((*pAltUnit));
	}
	

	/*十位、百位、千位显示比例调整.*/
	//fScaleParam *= 0.95f;
	const char *szResource = bPositive ? ("res/Hud/NumberLap.png") : ("res/Hud/NumberLap_Negative.png");

	/*高度值(十位).*/
	CHmsImageNode **pAltDeac = bPositive ? &m_pAltDP : &m_pAltDN;
	fScaleParam *= 1.1f;
	fOffsetWide -= m_SingleSize.width * fScaleParam - 5;
	GenDightPlace(pAltDeac, fScaleParam, fOffsetWide, fHeightHalf, szResource, bDisplay);
	/*高度值(百位).*/
	CHmsImageNode **pAltHund = bPositive ? &m_pAltHP : &m_pAltHN;
#if ALTMETER_USER_20
	fOffsetWide -= (m_HighBySize.width * fScaleParam + 1.0f);
#else
	fOffsetWide -= (m_HighBySize.width * fScaleParam + 1.0f) * 0.72f;
#endif
	GenDightPlace(pAltHund, fScaleParam, fOffsetWide, fHeightHalf, szResource, bDisplay);
	/*高度值(千位).*/
	CHmsImageNode **pAltThou = bPositive ? &m_pAltTP : &m_pAltTN;
#if ALTMETER_USER_20
	fOffsetWide -= (m_HighBySize.width * fScaleParam + 1.0f);
#else
	fOffsetWide -= (m_HighBySize.width * fScaleParam + 1.0f) * 0.72f;
#endif
	GenDightPlace(pAltThou, fScaleParam, fOffsetWide, fHeightHalf, szResource, bDisplay);

	/*高位占位符.*/
	if (m_pAltSm == 0)
	{
		m_pAltSm = CHmsImageNode::Create("res/Hud/placeholder.png");
		if (m_pAltSm)
		{
			m_pAltSm->SetAnchorPoint(Vec2(1.0f, 0.5f));
			m_pAltSm->SetPosition(fOffsetWide - 1.0f, fHeightHalf);
			m_pAltSm->SetVisible(bDisplay);
			m_pPointer->AddChild((CHmsNode*)m_pAltSm);
		}		
	}
}

void CHmsHudAltMeter::SetUnValid(int unValid)
{
	if (m_pUnValid && m_CurUnValid != unValid)
	{
		m_CurUnValid = unValid;
		if (unValid == 1)//全部有效时.
		{
			m_pUnValid->SetVisible(false);
			return;
		}

		float fWide = m_sizeContent.width + 50;
		float fHigh = m_sizeContent.height;
		float fText = 25.0f;
		Vec2  vertVec[4];

		m_pUnValid->SetVisible(true);
		m_pUnValid->clear();
		/*马赫数是否有效(数值随右侧第一位为1则为有效，为0无效).*/
		if ((unValid & 0x01) == 0)
		{
			vertVec[0].x = 0.0f; vertVec[0].y = 1.0f;
			vertVec[1].x = fWide; vertVec[1].y = fHigh - 1.0f;
			vertVec[2].x = 0.0f; vertVec[2].y = fHigh - 1.0f;
			vertVec[3].x = fWide; vertVec[3].y = 1.0f;
			//m_pUnValid->DrawSolidRect(0.0f, 0.0f, fWide, fText, UnValidColor);
			//m_pUnValid->DrawSolidRect(0.0f, 0.0f, fWide, fText, Color4F::WHITE);
			m_pUnValid->DrawLines(vertVec, 4, 4.0f, Color4B::RED);
			m_pUnValid->SetDrawColor(Color4F::RED);
		}
	}
}

void CHmsHudAltMeter::SetSelAltmeter(float fSelAltVlaue)
{
	if (m_pSelAltmeter&& fabsf(m_SelAltValue - fSelAltVlaue) > 1e-2)
	{
		/*设置文本值.*/
		std::stringstream os;
		os << fSelAltVlaue;
		m_pSelAltmeter->SetString(os.str().c_str());
		m_SelAltValue = fSelAltVlaue;

		/*每设置一次计算移动选择高度游标.*/
		float fCurVlaue = m_AltValue;
		float comVal = m_tickSpan * (m_pAltLabel.size() - 1);
		float fOffsetY = 0.0f;
		if (m_SelAltValue <= fCurVlaue - comVal){
			fOffsetY = 0.0f;
		}
		else if (m_SelAltValue >= fCurVlaue + comVal){
			fOffsetY = m_pAltBar->GetContentSize().height;
		}
		else{
			fOffsetY = m_pAltBar->GetContentSize().height / 2 + (m_SelAltValue - fCurVlaue)*m_PressurePixels;
		}

		m_pSelAltmeterMark->SetPositionY(fOffsetY);

	}
}

void CHmsHudAltMeter::SetAltMeter(float fAltMeterVlaue)
{
	float fCurVlaue = HMS_MAX(HMS_MIN(fAltMeterVlaue, 30000.0f), -500.0f);//现在-500-30000之间.
	if (0 == m_pAltBar || fabsf(m_AltValue - fCurVlaue) < 0.005f){
		return;
	}

	if (!m_bInit)
	{
		if (m_CurUnValid == 0){ return; }//有效数据才能更新.
	}else{ m_bInit = false; }

	/*取数.*/
	float fRemVlaue = fCurVlaue;
	bool  bLessZero = (fCurVlaue < 0.0f) ? true : false;
	if ((!m_pAltTP || !m_pAltDP || !m_pAltUP || !m_pAltTN || !m_pAltHN || !m_pAltDN || !m_pAltUN))
	{
		return;
	}
	m_pAltTP->SetVisible(!bLessZero);
	m_pAltHP->SetVisible(!bLessZero);
	m_pAltDP->SetVisible(!bLessZero);
	m_pAltUP->SetVisible(!bLessZero);
	m_pAltTN->SetVisible(bLessZero);
	m_pAltHN->SetVisible(bLessZero);
	m_pAltDN->SetVisible(bLessZero);
	m_pAltUN->SetVisible(bLessZero);

	/*千位数字.*/
	CHmsImageNode *pThusImg = m_pAltTP->IsVisible() ? m_pAltTP : m_pAltTN;
	int uThouNum = (int)(bLessZero ? ceilf(fCurVlaue * 0.0001f) : floorf(fCurVlaue * 0.0001f));//对应数值千位.
	int uThouMax = bLessZero ? HMS_MAX(uThouNum, -9) : HMS_MIN(uThouNum, 9);//百位数限制最大值9.
	fRemVlaue -= uThouNum * 10000.0f;
	/*百位数字.*/
	int uHundNum = (int)(bLessZero ? ceilf(fRemVlaue * 0.001f) : floorf(fRemVlaue * 0.001f));//对应数值千位.
	fRemVlaue -= uHundNum * 1000.0f;
	/*十位数字.*/
	int uDecaNum = (int)(bLessZero ? ceilf(fRemVlaue * 0.01f) : floorf(fRemVlaue* 0.01f));
	fRemVlaue -= uDecaNum * 100.0f;
	/*个位数字.*/
	float uUnitNum = fRemVlaue;
	/*符号.*/
	float fSmybol = bLessZero ? -1.0f : 1.0f;
	/*高位占位符显示与隐藏.*/
	if (abs(uThouMax) > 0 || (abs(uHundNum) >= 9 && abs(uDecaNum) >= 9 && fabsf(uUnitNum) >= 80.0f)){
		pThusImg->SetVisible(true);
		m_pAltSm->SetVisible(false);
	}
	else{
		pThusImg->SetVisible(false);
		m_pAltSm->SetVisible(true);
	}

	/*个位滚动.*/
#if ALTMETER_USER_20
	Rect texRect = { 0, 0, m_SingleSize.width, m_SingleUseHeight };
	texRect.origin.y = m_SingleSize.height*0.5f - (uUnitNum / 40.0f/*每次取的值宽度.*/*76.0f/*宽度对应的像素值.*/ + 40.0f/*单数字像素宽度*/);
#else
	Rect texRect = { 0, 0, m_SingleSize.width, m_SingleUseHeight };
	if (bLessZero){
		texRect.origin.y = 9.0f + fabsf(uUnitNum) / 5.0f/*间隔值.*/ * (m_SingleSize.height / 24.0f + 0.1f)/*图中数字个数.*/;
	}
	else{
		texRect.origin.y = 9.0f + (100.0f - uUnitNum) / 5.0f/*间隔值.*/ * (m_SingleSize.height / 24.0f + 0.1f)/*图中数字个数.*/;
	}
#endif
	CHmsImageNode *pOpImage = bLessZero ? m_pAltUN : m_pAltUP;
	pOpImage->SetTextureRect(texRect);
	/*其余位于个位范围不同.*/
	texRect.size.width = m_HighBySize.width;
	texRect.size.height = m_DightHeight;
	/*十位滚动.*/
#if ALTMETER_USER_20
	float fDecaRoll = (fabsf(uUnitNum) < 80.0f) ? 0.0f : (fabsf(uUnitNum) - 80.0f) * 0.05f * fSmybol;//0.05 = 1/20.0f
#else
	float fDecaRoll = (fabsf(uUnitNum) < 95.0f) ? 0.0f : (fabsf(uUnitNum) - 95.0f) * 0.2f * fSmybol;//0.2 = 1/5.0f
#endif
	texRect.origin.y = (m_HighBySize.height - m_DightHeight - 6.0f)*0.5f - ((float)uDecaNum + fDecaRoll) * m_DightHeight;
	pOpImage = bLessZero ? m_pAltDN : m_pAltDP;
	pOpImage->SetTextureRect(texRect);
	/*百位滚动.*/
	float fHundRoll = (abs(uDecaNum) < 9) ? 0.0f : fDecaRoll;
	pOpImage = bLessZero ? m_pAltHN : m_pAltHP;
	if (bLessZero)
	{
		texRect.origin.y = (m_HighBySize.height - m_DightHeight - 6.0f)*0.5f - ((float)-11.6f)* (m_DightHeight);
		pOpImage->SetTextureRect(texRect);
	}
	else
	{
		texRect.origin.y = (m_HighBySize.height - m_DightHeight - 6.0f)*0.5f - ((float)uHundNum + fHundRoll)* (m_DightHeight);
		pOpImage->SetTextureRect(texRect);
	}

	/*千位滚动(当小于时，千位显示负号).*/
	float fThouRoll = (uThouMax >= 9) ? 0.0f : ((abs(uHundNum) >= 9 && abs(uDecaNum) >= 9) ? fDecaRoll : 0.0f);
	texRect.origin.y = (m_HighBySize.height - m_DightHeight - 6.0f)*0.5f - ((float)uThouMax + fThouRoll)* (m_DightHeight);
	pOpImage = bLessZero ? m_pAltTN : m_pAltTP;
	pOpImage->SetTextureRect(texRect);

	/*高度表偏移量.*/
	float fOffsetY = m_pAltBar->GetContentSize().height / 2.0f - fCurVlaue * m_PressurePixels;
	m_pAltBar->SetPositionY(floorf(fOffsetY));
	m_pLabelNode->SetPositionY(m_pLabelNode->GetParent() ? 0.0f : floorf(fOffsetY));

	/*滚动高度Bar.*/
	UpdateAltBar(fCurVlaue);
	m_AltValue = fCurVlaue;

	/*移动选择高度游标.*/
	float comVal = m_tickSpan * (m_pAltLabel.size() - 1);
	if (m_SelAltValue <= fCurVlaue - comVal){ fOffsetY = 0.0f; }
	else if (m_SelAltValue >= fCurVlaue + comVal){ fOffsetY = m_pAltBar->GetContentSize().height; }
	else{
		fOffsetY = m_pAltBar->GetContentSize().height / 2 + (m_SelAltValue - fCurVlaue)*m_PressurePixels;
	}

	//m_pSelAltmeterMark->SetPositionY(fOffsetY);
	if (m_pBoxMetricCurAlt->IsVisible())
	{
		CHmsGlobal::SetLabelFormat(m_pLableMetricCurAlt, "%04d", (int)MeterToFeet(fCurVlaue));
	}
}

void CHmsHudAltMeter::MetricSwitch(bool bOn)
{
	m_pBoxMetricCurAlt->SetVisible(bOn);
	m_pBoxMetricCurAltMask->SetVisible(bOn);

	this->ForceUpdate();
}

void CHmsHudAltMeter::UpdateAltBar(float fAltVlaue)
{
	CHmsLabel *pLabel = 0;
	float curVal = 0.0f;
	float curRef = (m_tickNumber - 1)*m_tickSpan / 2.0f;
	bool  bUp = (fAltVlaue >= m_AltValue) ? true : false;

	/*获取初始值.*/
	pLabel = GetValue(bUp, &curVal);
	/*显示高度值变换.*/
	if (bUp){/*上升时,取最小值Label更改其值并设置位置到最大值位置.*/
		while (fAltVlaue - curVal > (curRef + m_tickSpan) && pLabel)
		{
			curVal = curVal + m_tickSpan * 2.0f * m_pAltLabel.size();

			/*设置值.*/
			SetValue(pLabel, true, curVal);
			/*获取列表当前第一个记录信息.*/
			pLabel = GetValue(true, &curVal);
		}
	}
	else{/*下降时,取最大值Label更改其值并设置位置到最小值位置.*/
		while (fAltVlaue - curVal < -(curRef + m_tickSpan) && pLabel)
		{
			curVal = curVal - m_tickSpan * 2.0f * m_pAltLabel.size();
			/*设置值.*/
			SetValue(pLabel, false, curVal);
			/*获取列表当前第一个记录信息.*/
			pLabel = GetValue(false, &curVal);
		}
	}

	/*更改刻线位置.*/
	pLabel = GetValue(true, &curVal);
	if (pLabel)
	{
		m_pAltLines->SetPositionY(floorf(pLabel->GetPoistionY()));
	}
}

HmsAviPf::CHmsLabel* CHmsHudAltMeter::GetValue(bool bFront, float *pOut)//HMS_TURE获取头值，HMS_FALSE获取最后一个值.
{
	CHmsLabel *pLabel = (CHmsLabel*)(bFront ? m_pAltLabel.front() : m_pAltLabel.back());
	if (0 == pLabel){ 
		return 0; 
	}

	*pOut = (float)atof(pLabel->GetString().c_str());
	return pLabel;
}

void CHmsHudAltMeter::SetValue(HmsAviPf::CHmsLabel *pLabel, bool bFront, float fVlaue)//HMS_TURE获取头值，HMS_FALSE获取最后一个值.
{
	/*转换获取值字符串.*/
	std::stringstream os;
	os << (int)fVlaue;
	pLabel->SetString(os.str().c_str());

	pLabel->SetPositionY(fVlaue * m_PressurePixels);
	if (bFront){
		m_pAltLabel.pop_front();//删除第一个记录.
		m_pAltLabel.push_back(pLabel); //放置到列表末尾.
	}
	else{
		m_pAltLabel.pop_back();//删除最后一个记录.
		m_pAltLabel.push_front(pLabel);//放置到列表头.
	}
}