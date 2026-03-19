#pragma once
#include <unordered_map>
#include "CCObjLoader.h"


class ShapesSvsCache
{
public:
	typedef std::vector<tinyobj::shape_t> Shapes;
	typedef Shapes* pShapes;
	static ShapesSvsCache* getInstance();
	void AddShapesData(const std::string& key, pShapes shapes);
	pShapes getShapesData(const std::string& key);
	void removeShapesData(const std::string& key);
	void removeAllShapesData();	
protected:
	ShapesSvsCache();
	~ShapesSvsCache();
	static ShapesSvsCache* _instance;
	std::unordered_map<std::string, pShapes> _shapesData;
	
};