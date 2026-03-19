#include "SpriteSvs3DCache.h"
#include "base/HmsFileUtils.h"
#include "Common.h"


SpriteSvs3DCache::SpriteSvs3DCache()
{

}

void SpriteSvs3DCache::removeSprite3DData(const std::string& key)
{
	auto it = _sprite3dData.find(key);
	if (it != _sprite3dData.end())
	{
		delete it->second;
		_sprite3dData.erase(it);
	}
}

void SpriteSvs3DCache::removeAllSprite3DData()
{
	for (auto& it : _sprite3dData)
	{
		delete it.second;
	}
	_sprite3dData.clear();
}

SpriteSvs3DCache::~SpriteSvs3DCache()
{
	removeAllSprite3DData();
}

SpriteSvs3DCache* SpriteSvs3DCache::_instance = nullptr;
SpriteSvs3DCache* SpriteSvs3DCache::getInstance()
{
	if (_instance == nullptr)
	{
		_instance = new SpriteSvs3DCache();
	}
	return _instance;
}

void SpriteSvs3DCache::AddSprite3DData(const std::string& key, SpriteSvs3DData* sprite3d)
{
	auto prev = getSprite3DData(key);
	if (prev == sprite3d)
	{
		return;
	}

	_sprite3dData.erase(key);
	delete prev;

	_sprite3dData[key] = sprite3d;
	return;
}

SpriteSvs3DData* SpriteSvs3DCache::getSprite3DData(const std::string& key)
{
	auto it = _sprite3dData.find(key);
	if (it != _sprite3dData.end())
	{
		return it->second;
	}
	return nullptr;
}


#define WIN_PROJ_1701A 1
#define WIN_PROJ_HMSTAWS_WIN32 2

#define WIN_TARGET_PROJ WIN_PROJ_HMSTAWS_WIN32

std::string SpriteSvs3DCache::getModelFullPath(const std::string& modelFileName)
{
	std::string fullpath;
    fullpath = HmsAviPf::CHmsFileUtils::getInstance()->fullPathForFilename(modelFileName);
	return fullpath;
}

bool SpriteSvs3DCache::Load3DObj(const std::string& modefilename, const char* mtl_basepath, SpriteSvs3DData* ps3dd)
{
	std::string fullPath = getModelFullPath(modefilename);

	std::string mtlPath = "";
	if (mtl_basepath)
		mtlPath = mtl_basepath;
	else
		mtlPath = fullPath.substr(0, fullPath.find_last_of("\\/") + 1);

	ps3dd->strPath = mtlPath;
	std::string ret = tinyobj::LoadObj(ps3dd->shapes, ps3dd->materials, fullPath.c_str(), mtlPath.c_str());
	if (ret.empty())
	{
		return true;
	}
	else
	{
		return false;
	}
}

