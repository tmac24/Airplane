#pragma once
#include "base/RefPtr.h"
#include <functional>

class CHmsLanguageObject;

typedef std::function<void(CHmsLanguageObject*,  bool)> HmsTextChangedCallback;		//true:language changed  false:text changed

class CHmsLanguageObject : public HmsAviPf::Ref
{
public:
	CHmsLanguageObject();
	~CHmsLanguageObject();

	virtual void UpdateLanguage(){}

	void SetTextChangeCallback(HmsTextChangedCallback cb){ m_callback = cb; }

	void OnTextChanged(bool bLanguageChanged){
		if (m_callback)
		{
			m_callback(this, bLanguageChanged);
		}
	}
protected:
	HmsTextChangedCallback  m_callback;
};

