#include "HmsUTF8.h"
#include "external/convertUTF/ConvertUTF.h"
#include <string.h>
NS_HMS_BEGIN

namespace StringUtils{

	bool IsUnicodeSpace(char16_t ch)
	{
		return  (ch >= 0x0009 && ch <= 0x000D) || ch == 0x0020 || ch == 0x0085 || ch == 0x00A0 || ch == 0x1680
			|| (ch >= 0x2000 && ch <= 0x200A) || ch == 0x2028 || ch == 0x2029 || ch == 0x202F
			|| ch == 0x205F || ch == 0x3000;
	}

	bool IsCJKUnicode(char16_t ch)
	{
		return (ch >= 0x4E00 && ch <= 0x9FBF)   // CJK Unified Ideographs
			|| (ch >= 0x2E80 && ch <= 0x2FDF)   // CJK Radicals Supplement & Kangxi Radicals
			|| (ch >= 0x2FF0 && ch <= 0x30FF)   // Ideographic Description Characters, CJK Symbols and Punctuation & Japanese
			|| (ch >= 0x3100 && ch <= 0x31BF)   // Korean
			|| (ch >= 0xAC00 && ch <= 0xD7AF)   // Hangul Syllables
			|| (ch >= 0xF900 && ch <= 0xFAFF)   // CJK Compatibility Ideographs
			|| (ch >= 0xFE30 && ch <= 0xFE4F)   // CJK Compatibility Forms
			|| (ch >= 0x31C0 && ch <= 0x4DFF);  // Other exiensions
	}

	bool UTF8ToUTF16(const std::string& utf8, std::u16string& outUtf16)
	{
		if (utf8.empty())
		{
			outUtf16.clear();
			return true;
		}

		bool ret = false;

		const size_t utf16Bytes = (utf8.length() + 1) * sizeof(char16_t);
		char16_t* utf16 = (char16_t*)malloc(utf16Bytes);
		memset(utf16, 0, utf16Bytes);

		char* utf16ptr = reinterpret_cast<char*>(utf16);
		const UTF8* error = nullptr;

		if (llvm::ConvertUTF8toWide(2, utf8, utf16ptr, error))
		{
			outUtf16 = utf16;
			ret = true;
		}

		free(utf16);

		return ret;
	}

	bool UTF16ToUTF8(const std::u16string& utf16, std::string& outUtf8)
	{
		if (utf16.empty())
		{
			outUtf8.clear();
			return true;
		}

		return llvm::convertUTF16ToUTF8String(utf16, outUtf8);
	}

}

NS_HMS_END