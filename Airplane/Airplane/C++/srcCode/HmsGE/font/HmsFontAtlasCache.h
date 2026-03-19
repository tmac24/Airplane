
#ifndef _CCFontAtlasCache_h_
#define _CCFontAtlasCache_h_

#include <unordered_map>
#include "base/HmsTypes.h"

NS_HMS_BEGIN

class FontAtlas;
class Texture2D;
struct _ttfConfig;

class HMS_DLL FontAtlasCache
{  
public:
    static FontAtlas* getFontAtlasTTF(const _ttfConfig* config);
    static FontAtlas* getFontAtlasFNT(const std::string& fontFileName, const Vec2& imageOffset = Vec2::ZERO);

    static FontAtlas* getFontAtlasCharMap(const std::string& charMapFile, int itemWidth, int itemHeight, int startCharMap);
    static FontAtlas* getFontAtlasCharMap(Texture2D* texture, int itemWidth, int itemHeight, int startCharMap);
    static FontAtlas* getFontAtlasCharMap(const std::string& plistFile);
    
    static bool releaseFontAtlas(FontAtlas *atlas);

    /** Removes cached data.
     It will purge the textures atlas and if multiple texture exist in one FontAtlas.
     */
    static void purgeCachedData();
    
private:
    static std::string generateFontName(const std::string& fontFileName, float size, bool useDistanceField);
    static std::unordered_map<std::string, FontAtlas *> _atlasMap;
};

NS_HMS_END

#endif
