#pragma once
#include <iostream>
#include <string>
#include "SpriteSvs3DCache.h"
#include "SpriteSvs3D.h"
#include "Drawable.h"
#include "Node.h"
#include "MatrixNode.h"
#include "Global.h"
#include "Coordinate.h"
#include "CoordinateSystemMgr.h"

extern "C"
{
#include "_Vec3.h"
}

class Plane3D :public SpriteSvs3D
{
public:    
    static Plane3D* create(const std::string& modelName, const std::string& obj3dFilePath);
	void Update(const CVec3d& LonLatAlt, const CVec3d& YawPitchRoll);
    std::string getModelName(void);
protected:	
    bool init(const std::string& name, const std::string& obj3dFilePath);
	virtual pCNode CreateSprite3DMatrixNode() override;
	virtual pCNode CreateSprite3DNode(SpriteSvs3DData* ps3dd) override;
	CMatrixd UpdateMatrix(const CVec3d &LonLatAlt, const CVec3d &YawPitchRoll);
    std::string _modelName;
private:
    Plane3D();
};