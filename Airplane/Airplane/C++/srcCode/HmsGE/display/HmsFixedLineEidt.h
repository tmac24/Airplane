#pragma once
#include "HmsWidget.h"
#include "base/RefPtr.h"
#include "HmsLabel.h"
#include "base/HmsTimer.h"
#include "HmsImageNode.h"
#include "HmsDrawNode.h"
#include "HmsStretchImageNode.h"

NS_HMS_BEGIN

class CHmsFixedLineEidt : public HmsAviPf::CHmsStretchImageNode
{
public:
	typedef std::function<void(std::string)> OnTextChangedCallback;
	typedef std::function<void(int)> OnCurrentIndexChangedCallback;

	CHmsFixedLineEidt();
	~CHmsFixedLineEidt();

	static CHmsFixedLineEidt* Create(const HmsAviPf::Size &size, const int &length = 1);

	static CHmsFixedLineEidt* Create(const std::string &bgImage, HmsAviPf::Size size, const int &length = 1);

	void SetLength(const int &length);

	//获取当前索引 
	int GetCurrentIndex();

	//设置当前索引
	void SetCurrentIndex(int index);

	//当前索引上的字符
	char CurentIndexChar();

	//设置不能被改变的字符
	void SetUnchangeableChars(const std::string &chars);

	//设置文字大小
	void SetFontSize(const float &fontSize);

	//设置显示文本
	void SetText(const std::string &text);

	//获取字符串
	std::string GetText();

	//设置文本颜色
	void SetTextColor(const HmsAviPf::Color4B &color);

	//设置默认字符
	void SetDefaultChar(const char &defaultChar);

	//回删
	void Backspace();//回删

	//替换当前索引上的字符
	void ReplaceCurentIndexChar(const char &c);//替换当前字符

	//向后移动索引
	void ToNextIndex(); 

	//向前移动索引
	void ToPreviousIndex();

	virtual void Update(float delta) override;

	OnTextChangedCallback OnTextChanged;
	OnCurrentIndexChangedCallback OnCurrentIndexChanged;
protected:
	int   m_textLength;
	int   m_curentIndex;

	std::string m_unchangeableChars;//不能被改变的字符
	HmsAviPf::Color4B m_textColor;
	std::string m_measureChar;//用于测量字符长度的参考字符

	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>				m_curentLabel;
	HmsAviPf::Vector<HmsAviPf::CHmsLabel*>				m_pLabelTexts;
	HmsAviPf::RefPtr<HmsAviPf::CHmsStretchImageNode>	m_pImageBackground;
	HmsAviPf::RefPtr<HmsAviPf::CHmsDrawNode>			m_pNodeCursor;

	bool InitWithBackgroundImage(const int &length, const std::string& bgImage, const HmsAviPf::Size &size);

	virtual void ContentLayout();

	virtual void  EnableChanged();

private:
	HmsAviPf::TTFConfig m_ttfConfig;
	CHmsTime *m_time;
	double m_lastTime;
	std::vector<HmsAviPf::Rect> m_charBound;
	std::string m_defaltChar;

	HmsAviPf::TTFConfig CreateTTFConfig();

	void UpdateContentState();

	void CurentIndexChanged();
	//计算光标尺寸大小
	void UpdateCursor();

	bool IsUnchangeableChar(int index);
};

NS_HMS_END