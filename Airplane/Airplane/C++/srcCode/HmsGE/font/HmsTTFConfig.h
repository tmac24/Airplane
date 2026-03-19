#ifndef _TTF_CONFIGH_H_
#define _TTF_CONFIGH_H_

#include <string>
#include "base/HmsTypes.h"

NS_HMS_BEGIN

typedef struct _ttfConfig
{
	std::string fontFilePath;
	float fontSize;

	GlyphCollection glyphs;
	const char *customGlyphs;

	bool distanceFieldEnabled;		//used for number texture scaled 
	int outlineSize;

	_ttfConfig(const char* filePath = "", float size = 12, const GlyphCollection& glyphCollection = GlyphCollection::DYNAMIC,
		const char *customGlyphCollection = nullptr, bool useDistanceField = false, int outline = 0)
		:fontFilePath(filePath)
		, fontSize(size)
		, glyphs(glyphCollection)
		, customGlyphs(customGlyphCollection)
		, distanceFieldEnabled(useDistanceField)
		, outlineSize(outline)
	{
		if (outline > 0)
		{
			distanceFieldEnabled = false;
		}
	}
}TTFConfig;

NS_HMS_END

#endif