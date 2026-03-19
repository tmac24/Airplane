
#include <iostream>
#include <sstream>
#include "font/HmsFontFreeType.h"
#include "font/HmsFontAtlasCache.h"
#include "font/HmsFontAtlas.h"
#include <stdio.h>
#include "font/HmsTTFConfig.h"

NS_HMS_BEGIN

std::unordered_map<std::string, FontAtlas *> FontAtlasCache::_atlasMap;

void FontAtlasCache::purgeCachedData()
{
    auto atlasMapCopy = _atlasMap;
    for (auto&& atlas : atlasMapCopy)
    {
        atlas.second->purgeTexturesAtlas();
    }
}

FontAtlas* FontAtlasCache::getFontAtlasTTF(const _ttfConfig* config)
{  
    bool useDistanceField = config->distanceFieldEnabled;
    if(config->outlineSize > 0)
    {
        useDistanceField = false;
    }

    auto atlasName = generateFontName(config->fontFilePath, config->fontSize, useDistanceField);
    atlasName.append("_ol_");//outline
    std::stringstream ss;
    ss << config->outlineSize;
    atlasName.append(ss.str());

    auto it = _atlasMap.find(atlasName);

    if ( it == _atlasMap.end() )
    {
        auto font = HmsFontFreeType::create(config->fontFilePath, config->fontSize, config->glyphs,
            config->customGlyphs, useDistanceField, config->outlineSize);
        if (font)
        {
            auto tempAtlas = font->createFontAtlas();
            if (tempAtlas)
            {
                _atlasMap[atlasName] = tempAtlas;
                return _atlasMap[atlasName];
            }
        }
    }
    else
    {
        _atlasMap[atlasName]->retain();
        return _atlasMap[atlasName];
    }

    return nullptr;
}

FontAtlas* FontAtlasCache::getFontAtlasFNT(const std::string& fontFileName, const Vec2& imageOffset /* = Vec2::ZERO */)
{
    std::string atlasName = generateFontName(fontFileName, 0,false);
    auto it = _atlasMap.find(atlasName);

    if ( it == _atlasMap.end() )
    {
#if 0
        auto font = FontFNT::create(fontFileName,imageOffset);

        if(font)
        {
            auto tempAtlas = font->createFontAtlas();
            if (tempAtlas)
            {
                _atlasMap[atlasName] = tempAtlas;
                return _atlasMap[atlasName];
            }
        }
#endif
    }
    else
    {
        _atlasMap[atlasName]->retain();
        return _atlasMap[atlasName];
    }
    
    return nullptr;
}

FontAtlas* FontAtlasCache::getFontAtlasCharMap(const std::string& plistFile)
{
    std::string atlasName = generateFontName(plistFile, 0,false);
    auto it = _atlasMap.find(atlasName);

    if ( it == _atlasMap.end() )
    {
#if 0
        auto font = FontCharMap::create(plistFile);

        if(font)
        {
            auto tempAtlas = font->createFontAtlas();
            if (tempAtlas)
            {
                _atlasMap[atlasName] = tempAtlas;
                return _atlasMap[atlasName];
            }
        }
#endif
    }
    else
    {
        _atlasMap[atlasName]->retain();
        return _atlasMap[atlasName];
    }

    return nullptr;
}

FontAtlas* FontAtlasCache::getFontAtlasCharMap(Texture2D* texture, int itemWidth, int itemHeight, int startCharMap)
{
#if 0
    char tmp[30];
    sprintf(tmp,"name:%u_%d_%d_%d",texture->getName(),itemWidth,itemHeight,startCharMap);
    std::string atlasName = generateFontName(tmp, 0,false);

    auto it = _atlasMap.find(atlasName);
    if ( it == _atlasMap.end() )
    {
        auto font = FontCharMap::create(texture,itemWidth,itemHeight,startCharMap);

        if(font)
        {
            auto tempAtlas = font->createFontAtlas();
            if (tempAtlas)
            {
                _atlasMap[atlasName] = tempAtlas;
                return _atlasMap[atlasName];
            }
        }
    }
    else
    {
        _atlasMap[atlasName]->retain();
        return _atlasMap[atlasName];
    }
#endif
    return nullptr;
}

FontAtlas* FontAtlasCache::getFontAtlasCharMap(const std::string& charMapFile, int itemWidth, int itemHeight, int startCharMap)
{
#if 0
    char tmp[255];
    (tmp,250,"name:%s_%d_%d_%d",charMapFile.c_str(),itemWidth,itemHeight,startCharMap);

    std::string atlasName = generateFontName(tmp, 0,false);

    auto it = _atlasMap.find(atlasName);
    if ( it == _atlasMap.end() )
    {
        auto font = FontCharMap::create(charMapFile,itemWidth,itemHeight,startCharMap);

        if(font)
        {
            auto tempAtlas = font->createFontAtlas();
            if (tempAtlas)
            {
                _atlasMap[atlasName] = tempAtlas;
                return _atlasMap[atlasName];
            }
        }
    }
    else
    {
        _atlasMap[atlasName]->retain();
        return _atlasMap[atlasName];
    }
#endif
    return nullptr;
}

std::string FontAtlasCache::generateFontName(const std::string& fontFileName, float size, bool useDistanceField)
{
    std::string tempName(fontFileName);
    
    if(useDistanceField)
        tempName.append("df");
    // std::to_string is not supported on android, using std::stringstream instead.
    std::stringstream ss;
    ss.precision(2);
    ss << std::fixed << size;
    return  tempName.append(ss.str());
}

bool FontAtlasCache::releaseFontAtlas(FontAtlas *atlas)
{
    if (nullptr != atlas)
    {
        for( auto &item: _atlasMap )
        {
            if ( item.second == atlas )
            {
                if (atlas->getReferenceCount() == 1)
                {
                  _atlasMap.erase(item.first);
                }
                
                atlas->release();
                
                return true;
            }
        }
    }
    
    return false;
}

NS_HMS_END
