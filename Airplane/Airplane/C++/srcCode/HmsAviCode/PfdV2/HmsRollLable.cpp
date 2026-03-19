#include "HmsRollLable.h"
#include "HmsGlobal.h"
#include "HmsPfdHeader.h"

USING_NS_HMS;

CHmsRollLabel* CHmsRollLabel::Create(BitFontSize vcFontSize, UnitCount uType, int rStyle, int nSpan)
{
	CHmsRollLabel *ret = new CHmsRollLabel();
	if (ret && ret->InitWithType(vcFontSize, uType, rStyle, nSpan))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

CHmsRollLabel::CHmsRollLabel()
: m_DefFontWide(60.0)
, m_DefFontHigh(55.0)
, m_DefDighWide(35.0)
, m_pBackground(0)
{
	m_SingleSize = Size(68, 768);
	m_SymbolColor = Color3B::GREEN;
	m_LabelColor = Color3B::WHITE;
}

CHmsRollLabel::~CHmsRollLabel()
{
}

bool CHmsRollLabel::InitWithType(BitFontSize vcFontSize, UnitCount uType, int rStyle, int nSpan)
{
	if (!CHmsNode::Init() || vcFontSize.size() <= 0)
	{
		return false;
	}

	m_CurDtType = uType;
	m_RollStyle = rStyle;
	m_BitNumber = vcFontSize.size();
	m_DightSpan = (float)nSpan;

	CalcLabelSize(vcFontSize, uType);

	if (rStyle & RS_BACKGROUND)
	{
		GenBackgorund();
	}

	GenEveryBits(vcFontSize, uType);

	return true;
}

void CHmsRollLabel::CalcLabelSize(BitFontSize vcFontSize, UnitCount uType)
{
	HmsAviPf::Size cwSize(0.0, 0.0);
	for (int nIndex = 0; nIndex < (int)vcFontSize.size(); ++nIndex)
	{
		if (nIndex == 0)/*个位.*/
		{
			float getHigh = (m_CurDtType == UC_ONE_01) ? m_DefFontHigh : (2.0 * m_DefFontHigh);
			float getWide = (m_CurDtType == UC_ONE_01 || m_CurDtType == UC_ONE_02) ? m_DefDighWide : (2.0 * m_DefDighWide);
			cwSize.height = HMS_MAX(cwSize.height, vcFontSize[0] / m_DefFontWide * getHigh);
			cwSize.width += (vcFontSize[0] / m_DefFontWide * getWide);
		}
		else /*百位及百位之上为单数字,一数字宽度和高度.*/
		{
			cwSize.width += (vcFontSize[nIndex] / m_DefFontWide * m_DefDighWide);
			cwSize.height = HMS_MAX(cwSize.height, vcFontSize[nIndex] / m_DefFontWide * m_DefFontHigh);
		}
	}

	cwSize.width += (2.0 * m_DightSpan + (vcFontSize.size() - 1) * m_DightSpan);

	RollStyle curSymbol = GetSymbolStyle();
	if (curSymbol == RS_SYMBOL_NEW)
	{
		cwSize.width += (vcFontSize[vcFontSize.size() - 1] / m_DefFontWide * m_DefDighWide + m_DightSpan);
	}

	SetContentSize(cwSize);
}

void CHmsRollLabel::GenBackgorund()
{
	m_pBackground = CHmsDrawNode::Create();
	m_pBackground->DrawSolidRect(0.0, 0.0, m_sizeContent.width, m_sizeContent.height, Color4F::BLACK);
	this->AddChild(m_pBackground);
}

void CHmsRollLabel::GenEveryBits(BitFontSize vcFontSize, UnitCount uType)
{
	int nFontCount = (int)vcFontSize.size();
	PFD_RETURN(nFontCount <= 0);

	float fRightOffset = m_DightSpan;/*右侧偏移距离.*/
	const char *szResPath = 0;

	CHmsImageNode *pBitImg = 0;
	HmsAviPf::Rect dightRect;

	/*创建个位.*/
	float fScaledValue = vcFontSize[0] / m_DefFontWide;
	switch (uType)
	{
	case UC_ONE_01:
	case UC_ONE_02: szResPath = "res/rolllabel/01.png";
		dightRect = Rect(0, 0, m_DefDighWide, (m_CurDtType == UC_ONE_01) ? m_DefFontHigh : (2.0 * m_DefFontHigh));
		fRightOffset += (vcFontSize[0] / m_DefFontWide * m_DefDighWide);
		break;
	case UC_TWO_05:szResPath = "res/rolllabel/05.png";
		dightRect = Rect(0, 0, 2 * m_DefDighWide, 2 * m_DefFontHigh);
		fRightOffset += (vcFontSize[0] / m_DefFontWide * 2.0 * m_DefDighWide);
		break;
	case UC_TWO_10:szResPath = "res/rolllabel/10.png";
		dightRect = Rect(0, 0, 2 * m_DefDighWide, 2 * m_DefFontHigh);
		fRightOffset += (vcFontSize[0] / m_DefFontWide * 2.0 * m_DefDighWide);
		break;
	case UC_TWO_20:szResPath = "res/rolllabel/20.png";
		dightRect = Rect(0, 0, 2 * m_DefDighWide, 2 * m_DefFontHigh);
		fRightOffset += (vcFontSize[0] / m_DefFontWide * 2.0 * m_DefDighWide);
		break;
	default: break;
	}
	
	pBitImg = CHmsImageNode::Create(szResPath, dightRect);
	if (pBitImg)
	{
		pBitImg->SetAnchorPoint(Vec2(0.0, 0.5));
		pBitImg->SetScale(fScaledValue);
		pBitImg->SetPosition(m_sizeContent.width - fRightOffset, HALF_VALUE(m_sizeContent.height));
		this->AddChild(pBitImg);
		m_LabelBits.push_back(pBitImg);
	}
	

	switch (uType)
	{
	case CHmsRollLabel::UC_ONE_01:
	case CHmsRollLabel::UC_ONE_02:break;
	case CHmsRollLabel::UC_TWO_05:
	case CHmsRollLabel::UC_TWO_10:
	case CHmsRollLabel::UC_TWO_20:
		if (pBitImg)
		{
			m_DoubleSize = pBitImg->GetTexture()->getContentSize();
		}
		break;
	default:break;
	}
	
	/*创建其他位.*/
	dightRect = Rect(0, 0, m_DefDighWide, m_DefFontHigh);
	for (int nIndex = 1; nIndex < nFontCount; nIndex++)
	{
		fRightOffset += m_DightSpan;
		fRightOffset += (vcFontSize[nIndex] / m_DefFontWide * m_DefDighWide);
		fScaledValue = vcFontSize[nIndex] / m_DefFontWide;
		pBitImg = CHmsImageNode::Create("res/rolllabel/01.png", dightRect);
		if (pBitImg)
		{
			pBitImg->SetAnchorPoint(Vec2(0.0, 0.5));
			pBitImg->SetScale(fScaledValue);
			pBitImg->SetPosition(m_sizeContent.width - fRightOffset, HALF_VALUE(m_sizeContent.height));
			this->AddChild(pBitImg);
			m_LabelBits.push_back(pBitImg);
		}
		
	}

	/*创建负号位.*/
	RollStyle curSymbol = GetSymbolStyle();
	if (curSymbol == RS_SYMBOL_NEW)
	{
		fRightOffset += m_DightSpan;
		fRightOffset += (vcFontSize[nFontCount - 1] / m_DefFontWide * m_DefDighWide);
		fScaledValue = vcFontSize[nFontCount - 1] / m_DefFontWide;
		pBitImg = CHmsImageNode::Create("res/rolllabel/01.png", dightRect);
		if (pBitImg)
		{
			pBitImg->SetAnchorPoint(Vec2(0.0, 0.5));
			pBitImg->SetScale(fScaledValue);
			pBitImg->SetPosition(m_sizeContent.width - fRightOffset, HALF_VALUE(m_sizeContent.height));
			this->AddChild(pBitImg);
			m_LabelBits.push_back(pBitImg);
		}
	}
}

void CHmsRollLabel::SetValue(float fValue)
{
	/*分离整数和小数部分.*/
	double dInteVal = 0.0f;
	double dDecimal = modf(fValue, &dInteVal);
	bool  bLessZero = (fValue < 0.0f) ? true : false;

	/*取个位数值.*/
	std::vector<int> erverBit;
	int nCalcuteValue = (int)dInteVal;
	int nValue = nCalcuteValue % ((m_CurDtType == UC_ONE_01 || m_CurDtType == UC_ONE_02) ? 10 : 100);
	nValue = HMS_MAX(-99, HMS_MIN(99, nValue));
	erverBit.push_back(nValue);
	nCalcuteValue = (nCalcuteValue - nValue) / ((m_CurDtType == UC_ONE_01 || m_CurDtType == UC_ONE_02) ? 10 : 100);

	/*取其他位数值.*/
	int nReferenceValue = 1;
	for (int nIndex = 1; nIndex < m_BitNumber; ++nIndex)
	{
		int nValue = nCalcuteValue / nReferenceValue % 10;
		erverBit.push_back(nValue);
		nReferenceValue *= 10;
	}

	/*根据符号类型处理显隐位.*/
	int nSymbol = SymbolBits(erverBit, fValue);

	/*范围向下截取.*/
	Rect texRect;

	/*滚动数值个位特别处理.*/
	float fUnitRoll = (m_RollStyle & RS_ROLL) ? fabsf(dDecimal) : 0.0;
	switch (m_CurDtType)
	{
	case UC_ONE_01: 
		if (bLessZero){
			texRect = Rect(0.0, m_DefFontHigh, m_DefDighWide, m_DefFontHigh);
			texRect.origin.y += ((fabsf(erverBit[0]) + fUnitRoll) * m_DefFontHigh);
		}else{
			texRect = Rect(m_DefDighWide, m_SingleSize.height - m_DefFontHigh, m_DefDighWide, m_DefFontHigh);
			texRect.origin.y -= ((fabsf(erverBit[0]) + 1.0 + fUnitRoll) * m_DefFontHigh);
		}
		break;
	case UC_ONE_02:
		if (bLessZero){
			texRect = Rect(0.0, HALF_VALUE(m_DefFontHigh), m_DefDighWide, m_DefFontHigh * 2.0);
			texRect.origin.y += ((fabsf(erverBit[0]) + fUnitRoll) * m_DefFontHigh);
		}else{
			texRect = Rect(m_DefDighWide, m_SingleSize.height - HALF_VALUE(m_DefFontHigh), m_DefDighWide, m_DefFontHigh * 2.0);
			texRect.origin.y -= ((fabsf(erverBit[0]) + 2.0 + fUnitRoll) * m_DefFontHigh);
		}
		break;
	case UC_TWO_05:
		if (bLessZero){
			texRect = Rect(0.0, HALF_VALUE(m_DefFontHigh), m_DefDighWide * 2.0, m_DefFontHigh * 2.0);
			texRect.origin.y += ((fabsf(erverBit[0]) + fUnitRoll) / 5.0 * m_DefFontHigh);
		}else{
			texRect = Rect(m_DefDighWide * 2.0, m_DoubleSize.height - HALF_VALUE(m_DefFontHigh), m_DefDighWide * 2.0, m_DefFontHigh * 2.0);
			texRect.origin.y -= ((fabsf(erverBit[0] + fUnitRoll) / 5.0 + 2.0) * m_DefFontHigh);
		}
		break;
	case UC_TWO_10:
		if (bLessZero){
			texRect = Rect(0.0, HALF_VALUE(m_DefFontHigh), m_DefDighWide * 2.0, m_DefFontHigh * 2.0);
			texRect.origin.y += ((fabsf(erverBit[0]) + fUnitRoll) / 10.0 * m_DefFontHigh);
		}else{
			texRect = Rect(m_DefDighWide * 2.0, m_DoubleSize.height - HALF_VALUE(m_DefFontHigh), m_DefDighWide * 2.0, m_DefFontHigh * 2.0);
			texRect.origin.y -= ((fabsf(erverBit[0] + fUnitRoll) / 10.0 + 2.0) * m_DefFontHigh);
		}
		break;
	case CHmsRollLabel::UC_TWO_20:
		if (bLessZero){
			texRect = Rect(0.0, HALF_VALUE(m_DefFontHigh), m_DefDighWide * 2.0, m_DefFontHigh * 2.0);
			texRect.origin.y += ((fabsf(erverBit[0]) + fUnitRoll) / 20.0 * m_DefFontHigh);
		}else{
			texRect = Rect(m_DefDighWide * 2.0, m_DoubleSize.height - HALF_VALUE(m_DefFontHigh), m_DefDighWide * 2.0, m_DefFontHigh * 2.0);
			texRect.origin.y -= ((fabsf(erverBit[0] + fUnitRoll) / 20.0 + 2.0) * m_DefFontHigh);
		}
		break;
	default:
		break;
	}
	m_LabelBits[0]->SetTextureRect(texRect);

	/*十位滚动：十位由个位决定，特殊处理.*/
	float fDecaRoll = 0.0;
	if (m_BitNumber >= 2)
	{
		switch (m_CurDtType)
		{
		case UC_ONE_01:
		case UC_ONE_02:fDecaRoll = (fabsf(erverBit[0] + dDecimal) < 9.0) ? 0.0 : (fabsf(erverBit[0] + dDecimal) - 9.0); break;
		case UC_TWO_05:fDecaRoll = (fabsf(erverBit[0]) < 95.0) ? 0.0 : (fabsf(erverBit[0]) - 95.0) * 0.2; break;
		case UC_TWO_10:fDecaRoll = (fabsf(erverBit[0]) < 90.0) ? 0.0 : (fabsf(erverBit[0]) - 90.0) * 0.1; break;
		case UC_TWO_20:fDecaRoll = (fabsf(erverBit[0]) < 80.0) ? 0.0 : (fabsf(erverBit[0]) - 80.0) * 0.05;break;
		default:break;
		}

		fDecaRoll = (m_RollStyle & RS_ROLL) ? fDecaRoll : 0.0;

		if (nSymbol == -2 && m_BitNumber == 2)
		{
			texRect = Rect(m_DefDighWide, m_SingleSize.height - m_DefFontHigh, m_DefDighWide, m_DefFontHigh);
			texRect.origin.y -= (13.0 * m_DefFontHigh);
			m_LabelBits[1]->SetColor(m_SymbolColor);
		}
		else if (nSymbol == -3 && m_BitNumber == 3)
		{
			texRect = Rect(0.0, m_DefFontHigh, m_DefDighWide, m_DefFontHigh);
			texRect.origin.y += (12.0 * m_DefFontHigh);
		}
		else
		{
			if (nSymbol != 1)
			{
				if (bLessZero){
					texRect = Rect(0.0, m_DefFontHigh, m_DefDighWide, m_DefFontHigh);
					texRect.origin.y += ((fabsf(erverBit[1]) + fDecaRoll) * m_DefFontHigh);
				}
				else{
					texRect = Rect(m_DefDighWide, m_SingleSize.height - m_DefFontHigh, m_DefDighWide, m_DefFontHigh);
					texRect.origin.y -= ((fabsf(erverBit[1]) + 1.0 + fDecaRoll) * m_DefFontHigh);
				}

				m_LabelBits[1]->SetColor(m_LabelColor);
			}
			else
			{
				texRect = Rect(0.0, m_DefFontHigh, m_DefDighWide, m_DefFontHigh);
				texRect.origin.y += (12.0 * m_DefFontHigh);
			}
		}
		
		m_LabelBits[1]->SetTextureRect(texRect);
	}

	/*滚动数值其他位.*/
	for (int nIndex = 2; nIndex < m_BitNumber; ++nIndex)
	{
		if (nSymbol == -3)/*显示占位符和负号.*/
		{
			if (nIndex == m_BitNumber - 2)
			{
				texRect = Rect(0.0, m_DefFontHigh, m_DefDighWide, m_DefFontHigh);
				texRect.origin.y += (12.0 * m_DefFontHigh);
				m_LabelBits[nIndex]->SetTextureRect(texRect);
				continue;
			}
			else if ( nIndex == m_BitNumber - 1)
			{
				texRect = Rect(m_DefDighWide, m_SingleSize.height - m_DefFontHigh, m_DefDighWide, m_DefFontHigh);
				texRect.origin.y -= (13.0 * m_DefFontHigh);
				m_LabelBits[nIndex]->SetColor(m_SymbolColor);
				m_LabelBits[nIndex]->SetTextureRect(texRect);
				continue;
			}
		}
		else if (nSymbol == -2)/*显示占位符.*/
		{
			if ( nIndex == m_BitNumber - 1)
			{
				texRect = Rect(m_DefDighWide, m_SingleSize.height - m_DefFontHigh, m_DefDighWide, m_DefFontHigh);
				texRect.origin.y -= (13.0 * m_DefFontHigh);
				m_LabelBits[nIndex]->SetColor(m_SymbolColor);
				m_LabelBits[nIndex]->SetTextureRect(texRect);
				continue;
			}
		}

		if (nSymbol != nIndex)
		{
			float fCurRoll = (fabsf(erverBit[nIndex - 1]) >= 9.0) ? fDecaRoll : 0.0f;
			if (bLessZero){
				texRect = Rect(0.0, m_DefFontHigh, m_DefDighWide, m_DefFontHigh);
				texRect.origin.y += ((fabsf(erverBit[nIndex]) + fCurRoll) * m_DefFontHigh);
			}
			else{
				texRect = Rect(m_DefDighWide, m_SingleSize.height - m_DefFontHigh, m_DefDighWide, m_DefFontHigh);
				texRect.origin.y -= ((fabsf(erverBit[nIndex]) + 1.0 + fCurRoll) * m_DefFontHigh);
			}

			m_LabelBits[nIndex]->SetColor(m_LabelColor);
		}
		else
		{
			texRect = Rect(0.0, m_DefFontHigh, m_DefDighWide, m_DefFontHigh);
			texRect.origin.y += (12.0 * m_DefFontHigh);
		}

		m_LabelBits[nIndex]->SetTextureRect(texRect);
	}

	/*处理增加的符号位.*/
	RollStyle curSymbol = GetSymbolStyle();
	if (curSymbol == RS_SYMBOL_NEW && nSymbol >= 0)
	{
		texRect = Rect(0.0, m_DefFontHigh, m_DefDighWide, m_DefFontHigh);
		texRect.origin.y += (12.0 * m_DefFontHigh);
		m_LabelBits[nSymbol]->SetTextureRect(texRect);
	}
}

void CHmsRollLabel::SetColor(HmsAviPf::Color3B color)
{
	for (auto pLabel : m_LabelBits)
	{
		pLabel->SetColor(color);
	}

	m_LabelColor = color;
}

void CHmsRollLabel::SetBackgroundColor(Color4B color)
{
	if (m_pBackground)
	{
		m_pBackground->clear();
		m_pBackground->DrawSolidRect(0.0, 0.0, m_sizeContent.width, m_sizeContent.height, Color4F(color));
	}
}

void CHmsRollLabel::SetBackGroundVisible(bool bVisible)
{
	if (m_pBackground)
	{
		m_pBackground->SetVisible(bVisible);
	}
}

CHmsRollLabel::RollStyle CHmsRollLabel::GetSymbolStyle()
{
	if (m_RollStyle & RS_SYMBOL_PLACE){	return RS_SYMBOL_PLACE;	}
	if (m_RollStyle & RS_SYMBOL_NEW){	return RS_SYMBOL_NEW;	}
	if (m_RollStyle & RS_SYMBOL_HIGH){ return RS_SYMBOL_HIGH;	}
	if (m_RollStyle & RS_SYMBOL_AUTO){ return RS_SYMBOL_AUTO;	}

	return RS_NONE;
}

int CHmsRollLabel::SymbolBits(std::vector<int> everyBit, float fValue)
{
	if (m_BitNumber <= 1)/*一位时没有负号显示位.*/
		return - 1;

	bool  bLessZero = (fValue < 0.0f) ? true : false;
	RollStyle curSymbol = GetSymbolStyle();

	switch (curSymbol)
	{
	case CHmsRollLabel::RS_SYMBOL_AUTO:
	{
		if (everyBit[m_BitNumber - 1] != 0){ return -1; }/*没有负号显示位置.*/

		for (auto pLabel : m_LabelBits) { pLabel->SetVisible(true); }
		for (int nIndex = m_BitNumber - 1; nIndex >= 0; --nIndex)
		{
			if (everyBit[nIndex] == 0 && nIndex >= 1)
			{
				if (everyBit[nIndex - 1] == 0)
				{
					m_LabelBits[nIndex]->SetVisible(false);
				}
				else
				{ 
					if (!bLessZero)
					{
						m_LabelBits[nIndex]->SetVisible(false);
						return -1;
					}

					return nIndex;
				}
			}
		}

		break;
	}
	case CHmsRollLabel::RS_SYMBOL_HIGH:
	{
		if (everyBit[m_BitNumber - 1] != 0 || !bLessZero){ return -1; }/*没有负号显示位置.*/
		return m_BitNumber - 1;
	}
	case CHmsRollLabel::RS_SYMBOL_NEW:
	{
		if (!bLessZero)
		{
			m_LabelBits[m_BitNumber]->SetVisible(false);
		}

		return bLessZero ? m_BitNumber : -1;
	}
	case CHmsRollLabel::RS_SYMBOL_PLACE:
	{
		if (everyBit[m_BitNumber - 1] != 0){ return -1; }/*没有负号显示位置.*/
		if (m_BitNumber == 2)
		{
			return (!bLessZero) ? -2 : (m_BitNumber - 1);/*-2表示最高显示占位符,低一位不显示负号.*/
		}

		if (everyBit[m_BitNumber - 2] == 0)
		{
			return (!bLessZero) ? -2 : -3;/*-3表示最高显示占位符,低一位显示负号.*/
		}
		
		return (!bLessZero) ? -2 : (m_BitNumber - 1);/*-2表示最高显示占位符,低一位不显示负号.*/
	}
	default: break;
	}

	return -1;
}

void CHmsRollLabel::SetSymbolColor(Color3B color)
{
	m_SymbolColor = color;
}
