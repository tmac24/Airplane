#pragma once
#include <iostream>
#include <string>
#include "SpriteSvs3DCache.h"
#include "Drawable.h"
#include "MatrixNode.h"
#include "Node.h"
#include "Global.h"
#include "Coordinate.h"
#include "CoordinateSystemMgr.h"

extern "C"
{
#include "_Vec3.h"
}

class SpriteSvs3D
{
public:
	bool InitWithFile(const std::string& path);	
	void Render();
protected:
	bool Load3DDataFromFile(const std::string& path, SpriteSvs3DData* ps3dd);	
	SpriteSvs3DData* Load3DDataFromCache(const std::string& path);
	bool InitFrom(SpriteSvs3DData* ps3dd);
	virtual pCNode CreateSprite3DMatrixNode();
	virtual pCNode CreateSprite3DNode(SpriteSvs3DData* ps3dd);
	
	pCNode _pnodeRoot;
	pCNode _pnodeMatrix;
};