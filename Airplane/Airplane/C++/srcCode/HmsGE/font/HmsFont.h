#ifndef _HMS_FONT_H_
#define _HMS_FONT_H_

#include <string>
#include "base/HmsTypes.h"

NS_HMS_BEGIN

class FontAtlas;

class HMS_DLL HmsFont : public Ref
{
public:
    virtual FontAtlas* createFontAtlas() = 0;

    virtual int* getHorizontalKerningForTextUTF16(const std::u16string& text, int &outNumLetters) const = 0;
    
    virtual int getFontMaxHeight() const { return 0; }
};

NS_HMS_END

#endif
