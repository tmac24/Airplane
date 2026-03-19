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

class CHmsRollLabel : public HmsAviPf::CHmsNode
{

public:
	typedef std::vector<float> BitFontSize;
	typedef std::vector<HmsAviPf::CHmsImageNode *> LabelBit;
	enum UnitCount
	{
		UC_ONE_01 = 0x00,/*个位一位,竖直取一位数字宽度.*/
		UC_ONE_02 = 0x01,/*个位一位,竖直取两位数字宽度.*/
		UC_TWO_05 = 0x02,/*个位是两位,间隔数字05.*/
		UC_TWO_10 = 0x03,/*个位时两位,间隔数字10.*/
		UC_TWO_20 = 0x04,/*个位是两位,间隔数字20.*/
	};

	/**
	 * 注意：
	 * 包含了RS_SYMBOL字符项只能存在一个,多个时为最大值.
	 */
	enum RollStyle
	{
		RS_NONE = 0x00,/*无样式.*/
		RS_ROLL = 0x01,/*滚动数字.*/

		/*从个位到高位,第一个大于数值最高位的位显示符号.
		 *自动隐藏为0高位.
		 *例如：-5  十位显示负号,百位以上存在则隐藏.
		        -50 百位显示负号,千位以上存在则隐藏.
				0   不显示负号,十位以上存在则隐藏.
		 */
		RS_SYMBOL_AUTO = 0x02,/*显示负号.*/
		/*最高位为0时显示负号,不为0时显示数字抛弃负号.*/
		RS_SYMBOL_HIGH = 0x04,/*显示负号.*/
		/*最高位前新建一位用于显示负号,为正数时隐藏.*/
		RS_SYMBOL_NEW = 0x08,/*显示负号.*/
		/*最高位为0时显示占位符,低一位为0则显示负号；
		 *低一为非0数字,最高位显示负号；
		 *最高位为非0数字时,抛弃负号显示.
		*/
		RS_SYMBOL_PLACE = 0x10,/*显示负号*/

		RS_BACKGROUND = 0x20,/*创建背景.*/
		
	};

	CHmsRollLabel();
	~CHmsRollLabel();

	CREATE_FUNC(CHmsRollLabel)

	/**
	 * 注意如果个位时两位要当作一位处理.
	 * 例如：198个位使用2位,vcFontSize 只能放入两个字体.
	 * vcFontSize	字体个数为显示数字位数(从低位到高位vcFontSize[0]表示个位字体大小).
	 * uType		个位类型(指明个位是一位还是两位和取值范围).
	 * rStyle		滚动样式(可使用|连接).		
	 * nSpan		位间距.
	 * 返回值:CHmsRollLabel* 滚动文本对象.
	 */
	static CHmsRollLabel* Create(BitFontSize vcFontSize, UnitCount uType, int rStyle, int nSpan);
	bool InitWithType(BitFontSize vcFontSize, UnitCount uType, int rStyle, int nSpan);

	void SetValue(float fValue);
	void SetColor(HmsAviPf::Color3B color);
	void SetBackgroundColor(Color4B color);
	void SetBackGroundVisible(bool bVisible);
	void SetSymbolColor(Color3B color);

protected:
	void CalcLabelSize(BitFontSize vcFontSize, UnitCount uType);
	void GenBackgorund();
	void GenEveryBits(BitFontSize vcFontSize, UnitCount uType);
	
	RollStyle GetSymbolStyle();

	int SymbolBits(std::vector<int> everyBit, float fValue);

protected:

	float m_DefFontWide;//默认字体宽度.
	float m_DefFontHigh;//默认字体高度.
	float m_DefDighWide;//单个数字宽度(例如 64中6或者4的数字宽度).

	float m_DightSpan;
	int   m_BitNumber;
	int	  m_RollStyle;/*滚动样式.*/
	
	UnitCount m_CurDtType;/*当前个位数字类型.*/
	LabelBit  m_LabelBits;/*个位开始存入.*/
	Color3B   m_SymbolColor;/*占位符颜色.*/
	Color3B   m_LabelColor; /*文本颜色.*/

	HmsAviPf::Size m_SingleSize;
	HmsAviPf::Size m_DoubleSize;
	HmsAviPf::CHmsDrawNode *m_pBackground;
};