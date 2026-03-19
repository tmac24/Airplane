
#ifndef _CCFontAtlas_h_
#define _CCFontAtlas_h_

#include <string>
#include <unordered_map>

#include "HmsAviMacros.h"
#include "base/Ref.h"
#include "HmsStdC.h"

NS_HMS_BEGIN

class HmsFont;
class Texture2D;
class HmsFontFreeType;

struct FontLetterDefinition
{
    float U;
    float V;
    float width;
    float height;
    float offsetX;
    float offsetY;
    int textureID;
    bool validDefinition;
    int xAdvance;
};

class HMS_DLL FontAtlas : public Ref
{
public:
    static const int CacheTextureWidth;
    static const int CacheTextureHeight;
    static const char* CMD_PURGE_FONTATLAS;
    static const char* CMD_RESET_FONTATLAS;

    FontAtlas(HmsFont &theFont);
    virtual ~FontAtlas();
    
    void addLetterDefinition(char16_t utf16Char, const FontLetterDefinition &letterDefinition);
    bool getLetterDefinitionForChar(char16_t utf16Char, FontLetterDefinition &letterDefinition);
    
    bool prepareLetterDefinitions(const std::u16string& utf16String);

    inline const std::unordered_map<ssize_t, Texture2D*>& getTextures() const{ return _atlasTextures;}
    void  addTexture(Texture2D *texture, int slot);
    float getLineHeight() const { return _lineHeight; }
    void  setLineHeight(float newHeight);
    
    Texture2D* getTexture(int slot);
    const HmsFont* getFont() const { return _font; }
    
    /** Removes textures atlas.
     It will purge the textures atlas and if multiple texture exist in the FontAtlas.
     */
    void purgeTexturesAtlas();

    /** sets font texture parameters:
     - GL_TEXTURE_MIN_FILTER = GL_LINEAR
     - GL_TEXTURE_MAG_FILTER = GL_LINEAR
     */
     void setAntiAliasTexParameters();

     /** sets font texture parameters:
     - GL_TEXTURE_MIN_FILTER = GL_NEAREST
     - GL_TEXTURE_MAG_FILTER = GL_NEAREST
     */
     void setAliasTexParameters();

protected:
    void relaseTextures();

    void findNewCharacters(const std::u16string& u16Text, std::unordered_map<unsigned short, unsigned short>& charCodeMap);

    std::unordered_map<ssize_t, Texture2D*> _atlasTextures;
    std::unordered_map<char16_t, FontLetterDefinition> _letterDefinitions;
    float _lineHeight;
    HmsFont* _font;
    HmsFontFreeType* _fontFreeType;

    // Dynamic GlyphCollection related stuff
    int _currentPage;
    unsigned char *_currentPageData;
    int _currentPageDataSize;
    float _currentPageOrigX;
    float _currentPageOrigY;
    int _letterPadding;
    int _letterEdgeExtend;

    int _fontAscender;
    bool _antialiasEnabled;
    int _currLineHeight;

    friend class CHmsLabel;
};

NS_HMS_END

#endif
