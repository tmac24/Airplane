#pragma once

#include "HmsAviMacros.h"
#include <string>

NS_HMS_BEGIN

namespace StringUtils{

	bool IsUnicodeSpace(char16_t ch);
	
	/**
	*CJK is short of (Chinese Japanese Korean),  
	*param   
	*returns 
	*by [4/17/2017 song.zhang079] 
	*/
	bool IsCJKUnicode(char16_t ch);

	HMS_DLL bool UTF8ToUTF16(const std::string& utf8, std::u16string& outUtf16);
	HMS_DLL bool UTF16ToUTF8(const std::u16string& utf16, std::string& outUtf8);

}

NS_HMS_END

