#pragma once
#include "HmsLanguageObject.h"
#include <string>
#include "base/HmsTypes.h"

namespace HmsAviPf
{
	class CHmsLabel;
}

class CHmsLanguageLabel : public CHmsLanguageObject
{
public:
	CHmsLanguageLabel();
	~CHmsLanguageLabel();

	/**
	*create the label ,by utf8
	*param   
	*returns 
	*by [5/18/2018 song.zhang] 
	*/
	static CHmsLanguageLabel * CreateLabel(const char * strTextUtf8, float fSize, bool bold = false);

	/**
	*create the label by gbk
	*param   
	*returns 
	*by [5/18/2018 song.zhang] 
	*/
	static CHmsLanguageLabel * CreateLabelGbk(const char * strTextGbk, float fSize, bool bold = false);

	static void SetText(HmsAviPf::CHmsLabel * label, const char * strText);

	virtual void UpdateLanguage();

	void Init(const char * strTextUt8, float fSize);

	void InitBold(const char * strTextUt8, float fSize);

	void InitGbk(const char * strText, float fSize);

	void InitGbkBold(const char * strText, float fSize);

	void setTextColor(const HmsAviPf::Color4B &color);

	HmsAviPf::CHmsLabel * GetLabel(){ return m_label.get(); }
private:
	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>		m_label;
	std::string							m_strText;
};

