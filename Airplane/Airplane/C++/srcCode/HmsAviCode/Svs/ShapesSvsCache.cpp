#include "ShapesSvsCache.h"

ShapesSvsCache::ShapesSvsCache()
{

}

void ShapesSvsCache::removeShapesData(const std::string& key)
{
	auto it = _shapesData.find(key);
	if (it != _shapesData.end())
	{
		delete it->second;
		_shapesData.erase(it);
	}
}

void ShapesSvsCache::removeAllShapesData()
{
	for (auto& it : _shapesData)
	{
		delete it.second;
	}
	_shapesData.clear();
}

ShapesSvsCache::~ShapesSvsCache()
{
	removeAllShapesData();
}

ShapesSvsCache* ShapesSvsCache::_instance = nullptr;
ShapesSvsCache* ShapesSvsCache::getInstance()
{
	if (_instance == nullptr)
	{
		_instance = new ShapesSvsCache();
	}
	return _instance;
}

void ShapesSvsCache::AddShapesData(const std::string& key, pShapes shapes)
{
	auto prev = getShapesData(key);
	if (prev == shapes)
	{
		return;
	}

	_shapesData.erase(key);
	delete prev;

	_shapesData[key] = shapes;
	return;
}

ShapesSvsCache::pShapes ShapesSvsCache::getShapesData(const std::string& key)
{
	auto it = _shapesData.find(key);
	if (it != _shapesData.end())
	{
		return it->second;
	}
	return nullptr;
}

