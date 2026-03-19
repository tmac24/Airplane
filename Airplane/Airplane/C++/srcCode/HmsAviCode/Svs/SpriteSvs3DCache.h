#pragma once
#include <unordered_map>
#include <vector>
#include "CCObjLoader.h"

class SpriteSvs3DData{
public:
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string						strPath;

	std::string GetFullPath(const std::string & strName)
	{
		return strPath + strName;
	}
};


class SpriteSvs3DCache
{
public:
	static SpriteSvs3DCache* getInstance();
	void AddSprite3DData(const std::string& key, SpriteSvs3DData* sprite3d);
	SpriteSvs3DData* getSprite3DData(const std::string& key);
	void removeSprite3DData(const std::string& key);
	void removeAllSprite3DData();	
	bool Load3DObj(const std::string& modePath, const char* mtl_basepath, SpriteSvs3DData* ps3dd);
	std::string getModelFullPath(const std::string& modelFileName);
protected:
	SpriteSvs3DCache();
	~SpriteSvs3DCache();
	static SpriteSvs3DCache* _instance;
	std::unordered_map<std::string, SpriteSvs3DData*> _sprite3dData;
};