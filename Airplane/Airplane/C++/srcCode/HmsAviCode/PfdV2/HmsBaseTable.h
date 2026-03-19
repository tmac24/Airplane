#pragma once
#include "../HmsGlobal.h"
#include "math/HmsMath.h"
#include "math/HmsGeometry.h"
#include "base/HmsNode.h"
#include "base/HmsClippingNode.h"
#include "display/HmsImageNode.h"
#include "display/HmsStretchImageNode.h"
#include "display/HmsLabel.h"
#include "display/HmsDrawNode.h"
#include "HmsValidV2.h"

class CHmsRollLabel;
class CHmsBaseTable : public CHmsValidV2
{
	typedef std::vector<HmsAviPf::CHmsNode *> TextTick;
public:
	CHmsBaseTable();
	virtual ~CHmsBaseTable();

	CREATE_FUNC(CHmsBaseTable)

	static CHmsBaseTable* Create(HmsAviPf::Size size);

	virtual bool InitWithSize(HmsAviPf::Size size);

	virtual void SetValue(float fValue);

	virtual void SetShowAbs(bool bAbs);

	void SetRollTick(bool bRoolTick);

protected:
	virtual void GenClipArea();
	virtual void GenBackground();
	virtual void GenDecorate();
	virtual void GenPointer();
	virtual void GenTableBar();
	virtual void GenBarClip();
	virtual void GenRollText();
	virtual HmsAviPf::CHmsNode* GenTextTick(bool bNeedText, bool bLong);
	virtual HmsAviPf::CHmsNode* GetTextValue(int nIndex, float *pOutput);
	virtual void UpdateValueAfter(float fValue){}
	virtual void BeforeGenUnvalid(){}

protected:
	int		m_TickNumber;

	float	m_MaxValue;/*最大值.*/
	float   m_MinValue;/*最小值.*/
	float	m_CurValue;/*当前值.*/

	float	m_TickLong; /*长刻度长度.*/
	float	m_TickShort;/*段刻度长度.*/
	float	m_TickHigh; /*刻线高度.*/
	float	m_TickSpan; /*刻线间隔值.*/
	float   m_RealSpan; /*真实间隔值.*/
	float   m_TickPixel;/*多少像素对应1:重写GenTableBar你应该计算改值.*/

	bool	m_bSwitched;
	bool    m_bShowAbs; /*负数是否显示为绝对值.*/
	bool    m_bRollTick;/*是否滚动刻度.*/

	TextTick					m_TxtTicks;
	HmsAviPf::Size				m_PointerSize; /*指针长宽.*/
	HmsAviPf::RefPtr<HmsAviPf::CHmsClippingNode> m_pClipAread;
	HmsAviPf::RefPtr<HmsAviPf::CHmsClippingNode> m_pClipBar;  /*特殊裁剪区.*/
	HmsAviPf::RefPtr<HmsAviPf::CHmsDrawNode>     m_pClipBarDraw;
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>		 m_pTableBar;
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>		 m_pPointer;
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>		 m_pUnValid;
	CHmsRollLabel								*m_pRollLabel; /*滚动文本对象.*/
};

