#include "HmsHudSpeedBar.h"
#include "HmsHudLine.h"
USING_NS_HMS;
/*文本识别号.*/
#define LABEL_IDENT       (10)
#define LINES_IDENT       (11)

CHmsHudSpeedBar * CHmsHudSpeedBar::Create(HmsAviPf::Size size)
{
	CHmsHudSpeedBar *ret = new CHmsHudSpeedBar();
	if (ret && ret->InitWithSize(size))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

CHmsHudSpeedBar::CHmsHudSpeedBar()
{
	m_tickSpan = 10.f;//10刻度间隔绘制刻度线，每20刻度绘制文本.
	m_tickNumber = 13;//绘制13条刻度.
	m_V1Value = 120.0f;
	m_V2Value = 220.0f;
	m_VrValue = 210.0f;
	m_VrefValue = 260.0f;
	m_SelSpeed = 0.0f;

	m_pTextNode = nullptr;//刻度文本节点集.
	m_pSpeedBar = nullptr;//空速条.
	m_pV1Mark = nullptr;//V1速度指示游标.
	m_pV2Mark = nullptr;//V2速度指示游标.
	m_pVrMark = nullptr;//VR速度指示游标.
	m_pVrefMark = nullptr;//VREF速度指示游标.
	m_pSelSpeedMark = nullptr;//选择空速游标.

	m_pClipText = nullptr;
}

CHmsHudSpeedBar::~CHmsHudSpeedBar()
{
}

bool CHmsHudSpeedBar::InitWithSize(HmsAviPf::Size size)
{
	if (!CHmsNode::Init())
	{
		return false;
	}

	SetContentSize(size);
	GenSpeedBar();

	return true;
}

void CHmsHudSpeedBar::GenSpeedBar()
{
	float fLineSpan = m_sizeContent.height / (float)(m_tickNumber - 1);
	m_tickPixels = fLineSpan / m_tickSpan;

	/*空速条带(在CHmsADISpeedBarDrawSpeedMark中绘制).*/
	m_pSpeedBar = CHmsDrawNode::Create();
	AddChild(m_pSpeedBar);

	/*刻线和文本指针部分裁剪区.*/
	CHmsImageNode *pStencil = CHmsImageNode::Create("res/Hud/pointer.png");
	if (!pStencil)
	{
		return;
	}
	pStencil->SetAnchorPoint(Vec2(1.0f, 0.5f));
	pStencil->SetScaleX(1.15f);
	pStencil->SetScaleY(1.10f);
	pStencil->SetPosition(m_sizeContent.width - 30.0f, m_sizeContent.height / 2.0f);
	CHmsClippingNode *pClipText = CHmsClippingNode::create();
	pClipText->setStencil(pStencil);
	pClipText->setInverted(true);
	pClipText->SetContentSize(m_sizeContent);
	AddChild(pClipText);
	m_pClipText = pClipText;

	/*刻度文本节点.*/
	m_pTextNode = CHmsNode::Create();
	m_pTextNode->SetContentSize(m_sizeContent);
	m_pTextNode->SetAnchorPoint(Vec2(0.0f, 0.0f));
	m_pTextNode->SetPosition(0.0f, m_sizeContent.height / 2.0f);
	pClipText->AddChild(m_pTextNode);

	/*修改刻度值集位置.*/
	for (unsigned int uIndexc = 0; uIndexc < m_tickNumber; uIndexc++)
	{
		std::stringstream osValue;
		osValue << (int)(uIndexc * 10.0f);
		CHmsNode *pLNode = GenTextLine(osValue.str().c_str(), (uIndexc % 2 == 0) ? true : false);
		pLNode->SetPosition(m_sizeContent.width - 10.0f, floorf(uIndexc * fLineSpan+0.5f));
		m_pTextNode->AddChild((CHmsNode*)pLNode);
		m_pTickLabel.push_back(pLNode);
	}

	/*V1速度指示游标.*/
	CreateMark((CHmsNode*)this, &m_pV1Mark, "res/Hud/V1.png");
	/*V2速度指示游标.*/
	CreateMark((CHmsNode*)this, &m_pV2Mark, "res/Hud/V2.png");
	/*VR速度指示游标.*/
	CreateMark((CHmsNode*)this, &m_pVrMark, "res/Hud/VR.png");
	/*VREF速度指示游标.*/
	CreateMark((CHmsNode*)this, &m_pVrefMark, "res/Hud/VREF.png");
	/*选择空速游标.*/
	CreateMark((CHmsNode*)this, &m_pSelSpeedMark, "res/Hud/bug.png");
}

/*文本加刻线对象.*/
HmsAviPf::CHmsNode* CHmsHudSpeedBar::GenTextLine(const char *szValue, bool bVisible)
{
	CHmsNode *pTextNode = CHmsNode::Create();

	CHmsLabel *pLabel = CHmsLabel::createWithTTF(szValue, "fonts/msyhbd.ttc", 25.0f);
	if (pLabel)
	{
		pLabel->SetAnchorPoint(Vec2(1.0f, 0.45f));
		pLabel->SetPosition(-40.0f, 0.0f);
		pLabel->SetTag(LABEL_IDENT);
		pLabel->SetVisible(bVisible);
		pLabel->SetColor(Color3B::GREEN);
		pTextNode->AddChild((CHmsNode*)pLabel);
	}
	

	CHmsHudHorizontalLine *pLine = CHmsHudHorizontalLine::CreateHorizontalLine(4);
	if (pLine)
	{
		pLine->SetAnchorPoint(Vec2(1.0f, 0.5f));
		pLine->SetWidth(30);
		pLine->SetTag(LINES_IDENT);
		pLine->SetColor(Color3B::GREEN);
		pTextNode->AddChild((CHmsNode*)pLine);
	}
	

	CHmsHudHorizontalLine * pLineSub = CHmsHudHorizontalLine::CreateHorizontalLine(4);
	if (pLineSub)
	{
		pLineSub->SetAnchorPoint(Vec2(1.0f, 0.5f));
		pLineSub->SetWidth(18);
		pLineSub->SetPosition(Vec2(0, m_tickPixels * 5));
		pLineSub->SetTag(LINES_IDENT);
		pLineSub->SetColor(Color3B::GREEN);
		pTextNode->AddChild((CHmsNode*)pLineSub);
	}
	

	return pTextNode;
}

void CHmsHudSpeedBar::CreateMark(HmsAviPf::CHmsNode *pParent, HmsAviPf::CHmsImageNode **curNode, const char *szpng)
{
	if (pParent)
	{
		*curNode = CHmsImageNode::Create(szpng);
		if (*curNode)
		{
			(*curNode)->SetAnchorPoint(Vec2(0.0f, 0.5f));
			(*curNode)->SetPosition(m_sizeContent.width, -100.0f);
			pParent->AddChild((CHmsNode*)(*curNode));
		}
	}	
}

HmsAviPf::CHmsImageNode* CHmsHudSpeedBar::GetMarkPointer(HmsMarkType mtype, float **fValP)
{
	CHmsImageNode *curMark = 0;
	switch (mtype)
	{
	case MARK_V1: curMark = m_pV1Mark; *fValP = &(m_V1Value); break;
	case MARK_V2: curMark = m_pV2Mark; *fValP = &(m_V2Value); break;
	case MARK_VR: curMark = m_pVrMark; *fValP = &(m_VrValue); break;
	case MARK_VREF:curMark = m_pVrefMark; *fValP = &(m_VrefValue); break;
	case MARK_SEL: curMark = m_pSelSpeedMark; *fValP = &(m_SelSpeed); break;
	default: break;
	}

	return curMark;
}

void CHmsHudSpeedBar::SetMarkValue(HmsMarkType mType, float fValue)
{
	float *curValue = 0;
	CHmsImageNode *curMark = GetMarkPointer(mType, &curValue);
	if (!curMark || !curValue){ 
		return; 
	}

	if (fabsf(*curValue - fValue) >= 0.01f)
	{
		*curValue = fValue;
	}
}

void CHmsHudSpeedBar::SetValue(float curAirspeed, float preAirspeed)
{
	float curVal = 0.0f;
	float curRef = (m_tickNumber - 1)*m_tickSpan / 2.0f;
	bool  bUpRef = (curAirspeed >= preAirspeed) ? true : false;

	/*获取初始值.*/
	CHmsNode *pLabelNode = GetValue(bUpRef, &curVal);
	/*显示速度值变换.*/
	if (bUpRef){/*上升时,取最小值Label更改其值并设置位置到最大值位置.*/
		while (curAirspeed - curVal >= (curRef + m_tickSpan - 5.0f) && pLabelNode)
		{
			curVal = curVal + m_tickSpan * m_pTickLabel.size();
			if (curVal >= MAX_AIRSPEED + m_tickSpan){ break; }
			/*设置值.*/
			SetTickLabelValue(pLabelNode, true, curVal);
			/*获取列表当前第一个记录信息.*/
			pLabelNode = GetValue(true, &curVal);
		}
	}
	else{/*下降时,取最大值Label更改其值并设置位置到最小值位置.*/
		while (curAirspeed - curVal <= -(curRef + m_tickSpan + 5.0f) && pLabelNode)
		{
			curVal = curVal - m_tickSpan /** 2.0f*/ * m_pTickLabel.size();
			/*设置值.*/
			SetTickLabelValue(pLabelNode, false, curVal);
			/*获取列表当前第一个记录信息.*/
			pLabelNode = GetValue(false, &curVal);
		}
	}

	/*绘制空速带.*/
	m_AirSpeed = curAirspeed;
	DrawSpeedMark(curAirspeed - curRef);
}

void CHmsHudSpeedBar::AddMaskNode(HmsAviPf::CHmsNode * node)
{
	//node->SetPosition(m_sizeContent.width - 30.0f, m_sizeContent.height / 2.0f);
	m_pClipText->setStencil(node);
	//addChild(node);
}

HmsAviPf::CHmsNode* CHmsHudSpeedBar::GetValue(bool bFront, float *pOut)//HMS_TURE获取头值，HMS_FALSE获取最后一个值.
{
	CHmsNode *pLabelNode = (CHmsNode*)(bFront ? m_pTickLabel.front() : m_pTickLabel.back());
	if (0 == pLabelNode){
		return 0;
	}
	CHmsLabel *pLabel = (CHmsLabel *)pLabelNode->GetChildByTag(LABEL_IDENT);
	if (0 == pLabel){
		return pLabelNode;
	}

	*pOut = (float)atof(pLabel->GetString().c_str());
	return pLabelNode;
}

void CHmsHudSpeedBar::SetTickLabelValue(HmsAviPf::CHmsNode *pLabelNode, bool bFront, float fVlaue)//HMS_TURE获取头值，HMS_FALSE获取最后一个值.
{
	/*获取文本节点（10的偶数倍显示，奇数倍隐藏）.*/
	CHmsLabel *pLabel = (CHmsLabel *)pLabelNode->GetChildByTag(LABEL_IDENT);
	if (pLabel)
	{
		std::stringstream os;
		os << (int)fVlaue;
		pLabel->SetString(os.str().c_str());
		bool bVisible = ((int)(fVlaue / m_tickSpan) % 2 == 0) ? true : false;
		pLabel->SetVisible(bVisible);
	}

	pLabelNode->SetVisible((fVlaue < 0.0f) ? false : true);
	pLabelNode->SetPositionY(fVlaue * m_tickPixels);
	if (bFront){
		m_pTickLabel.pop_front();//删除第一个记录.
		m_pTickLabel.push_back(pLabelNode);//放置到列表末尾.
	}
	else{
		m_pTickLabel.pop_back();//删除最后一个记录.
		m_pTickLabel.push_front(pLabelNode);//放置到列表头.
	}
}

void  CHmsHudSpeedBar::DrawSpeedMark(float fmin)
{
	if (!m_pSpeedBar){ return; }

	/*宽度和显示的最大空速值.*/
	float curWide = m_sizeContent.width;
	float fmax = fmin + (m_tickNumber - 1)*m_tickSpan;

	/*刻线位置.*/
	m_pTextNode->SetPositionY(floorf(m_sizeContent.height / 2.0f - (m_AirSpeed) * m_tickPixels));

	/*绘制速度带.*/
	m_pSpeedBar->clear();
	for (unsigned int uIndexr = 0; uIndexr < m_SpeedBarStyle.size(); ++uIndexr)
	{
		HmsAirspeedMark *am = &m_SpeedBarStyle[uIndexr];
		if (!am || am->vMin>fmax || am->vMax < fmin){ continue; }//不再该范围内.

		float fMinVal = HMS_MAX(am->vMin, fmin);//只能从当前的空速开始计算.
		float fMaxVal = HMS_MIN(HMS_MIN(am->vMax, fmax), MAX_AIRSPEED + 1.0f);
		/*绘制矩形带.*/
		m_pSpeedBar->DrawSolidRect(curWide - am->vWid, (fMinVal - fmin) * m_tickPixels, curWide, (fMaxVal - fmin)*m_tickPixels, am->color);
	}

	/*移动选择游标位置.*/
	float offset = 0.0f;
	if      (m_SelSpeed <= fmin){ offset = 0.0f; }
	else if (m_SelSpeed >= fmax){ offset = m_sizeContent.height; }
	else    { offset = (m_SelSpeed - fmin)*m_tickPixels; }
	m_pSelSpeedMark->SetPositionY(floorf(offset + 0.5f));

	/*移动标尺位置.*/
	SetMarkPosition(MARK_V1, fmin, fmax);
	SetMarkPosition(MARK_V2, fmin, fmax);
	SetMarkPosition(MARK_VR, fmin, fmax);
	SetMarkPosition(MARK_VREF, fmin, fmax);
}

void  CHmsHudSpeedBar::SetMarkPosition(HmsMarkType mtype, float frefmin, float frefmax)
{
	float *curValue = 0;
	CHmsImageNode *curMark = GetMarkPointer(mtype, &curValue);
	if (!curMark || !curValue){ return; }
	if (*curValue >= (frefmin - m_tickSpan) && *curValue <= (frefmax + m_tickSpan))
	{
		curMark->SetPositionY(floorf((*curValue - frefmin)*m_tickPixels - 0.5f));
	}
}