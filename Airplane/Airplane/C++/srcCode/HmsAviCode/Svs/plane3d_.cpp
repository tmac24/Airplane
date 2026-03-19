#include "HmsAviMacros.h"
#include "base/HmsFileUtils.h"
#include "HmsAviDisplayUnit.h"
#include "render/HmsTexture2D.h"
#include "render/HmsTextureCache.h"
#include "../Calculate/CalculateZs.h"
#include "plane3d_.h"

Plane3D* Plane3D::create(const std::string& name, const std::string& obj3dFilePath)
{
    auto newPlane = new Plane3D;
    if (newPlane->init(name, obj3dFilePath))
    {
        return newPlane;
    }
    else
    {
        delete newPlane;
        return nullptr;
    }
}

void Plane3D::Update(const CVec3d& LonLatAlt, const CVec3d& YawPitchRoll)
{
    auto pnodeMatTrans = (pCMatrixTransform)(_pnodeMatrix->pNodeData);
    pnodeMatTrans->matrix = UpdateMatrix(LonLatAlt, YawPitchRoll);
}


std::string Plane3D::getModelName()
{
    return _modelName;
}

//************************************
// Method:    CreatePlaneNode
// FullName:  Plane3D::CreatePlaneNode
// Access:    protected 
// Desc:      
// Returns:   pCNode
// Qualifier:
// Parameter: Sprite3DData * ps3dd
// Author:    yan.jiang
//************************************
pCNode Plane3D::CreateSprite3DNode(SpriteSvs3DData* ps3dd)
{
    if (ps3dd == nullptr)
    {
        return nullptr;
    }
    auto cntShapes = ps3dd->shapes.size();
    auto cntMats = ps3dd->materials.size();
    if (cntShapes == 0 || cntMats == 0)
    {
        return nullptr;
    }

    auto pnodeG = CNode_CreateNode(EN_NodeType::Group);

    CVec3f vertexTempf;
    CVec2f textcoord;
    CVec4f color;
    float alt;
    unsigned int inds;

    //mesh render order, 名为propeller 的半透明螺旋桨放最后
    std::vector<int> meshRenderOrder;
    int properllerIdx = 0;
	for (int i = 0; i < (int)cntShapes; ++i)
    {
        if ("propeller" == ps3dd->shapes[i].name)
        {
            properllerIdx = i;
        }
        else
        {
            meshRenderOrder.push_back(i);
        }
    }
    meshRenderOrder.push_back(properllerIdx);

    //for (int i : { 2, 4, 0, 3, 1})//temp hard code, for transparency
    for (auto& i : meshRenderOrder)
    {
        auto pnodeDraw = CNode_CreateNode(EN_NodeType::NT_Drawable);
        auto pdraw = (pCDrawable)pnodeDraw->pNodeData;

        unsigned int vtxCnt = ps3dd->shapes[i].mesh.positions.size() / 3;
        unsigned int indCnt = ps3dd->shapes[i].mesh.indices.size();

        float* pD = ps3dd->shapes[i].mesh.positions.data();
        float* pT = ps3dd->shapes[i].mesh.texcoords.data();
        auto pC = new float[vtxCnt * 4];
        auto pA = new float[vtxCnt];
        unsigned short* pI = ps3dd->shapes[i].mesh.indices.data();

        for (unsigned int j = 0; j < vtxCnt; ++j)
        {
            vertexTempf = *(CVec3f*)(pD + 3 * j);
            textcoord = *(CVec2f*)(pT + 2 * j);
            color = Vec_CreatVec4f(1, 1, 1, 1);
            alt = 0.0;

            //CVector_Pushback(pdraw->_vertexsF, &vertexTempf);
            pdraw->_vertexsF2.push_back(vertexTempf);
            //CVector_Pushback(pdraw->_texcoords, &textcoord);
            //CVector_Pushback(pdraw->_colors, &color);
            //CVector_Pushback(pdraw->_altitudes, &alt);
            pdraw->_texcoords2.push_back(textcoord);
            pdraw->_colors2.push_back(color);
            pdraw->_altitudes2.push_back(alt);
        }
        for (unsigned int j = 0; j < indCnt; ++j)
        {
            inds = *(pI + j);
            //CVector_Pushback(pdraw->_indices, &inds);
            pdraw->_indices2.push_back(inds);
        }

        DrawableNodeCreateVBO(pdraw);

        delete[] pC;
        delete[] pA;

        //texture
        int materialId = ps3dd->shapes[i].mesh.material_ids[0];//假定本mesh上所有的点都是相同的材质，取第0个点的材质

        std::string path1;
        if (ps3dd->materials[materialId].ambient_texname.empty())
        {
            path1 = ps3dd->materials[materialId].unknown_parameter["map_d"];//临时处理，3dsmax输出的透明贴图类
        }
        else
        {
            path1 = ps3dd->materials[materialId].ambient_texname;
        }

        std::string fullPath = ps3dd->GetFullPath(path1);

        HmsAviPf::Texture2D *texture = HmsAviPf::CHmsAviDisplayUnit::GetInstance()->GetTextureCache()->addImage(fullPath);
        if (texture)
        {
            pdraw->texture_image = new CImage;
            pdraw->texture_image->m_ID = texture->getName();
        }
        else
        {
            pdraw->texture_image = nullptr;
        }

        //make PrimitiveSets
        CPrimitiveSet ps;
        ps.beg = 0;
        ps.count = ps3dd->shapes[i].mesh.indices.size();
        ps.type = GL_TRIANGLES;
        //CVector_Pushback(pdraw->_primitiveSets, &ps);
        pdraw->_primitiveSets2.push_back(ps);

        CNode_AddChild(pnodeG, pnodeDraw);
    }

    return pnodeG;
}

bool Plane3D::init(const std::string& name, const std::string& obj3dFilePath)
{
    _modelName = name;
    return InitWithFile(obj3dFilePath);
}

pCNode Plane3D::CreateSprite3DMatrixNode()
{
    pCNode pnodemat = CNode_CreateNode(EN_NodeType::MatrixTransform);
    auto pnodeMatTrans = (pCMatrixTransform)(pnodemat->pNodeData);

    pnodeMatTrans->matrix = CMatrix_Identity();
    return pnodemat;
}

CMatrixd Plane3D::UpdateMatrix(const CVec3d &LonLatAlt, const CVec3d &YawPitchRoll)
{
    CMatrix matTrans = Coordinate_Vec3dDegreeLonlatToMatrix(LonLatAlt);
    CMatrix matAttitude = Coordinate_YawPitchRollVec3dDegreeToMatrix(YawPitchRoll);

    CMatrix mat90x = CMatrix_RotateAXYZ(SIM_Pi / 2, 1, 0, 0);
    CMatrix mat90z = CMatrix_RotateAXYZ(-SIM_Pi / 2, 0, 0, 1);

    CMatrix scaleMat = CMatrix_ScaleXYZ(1, 1, 1);
    CMatrixd tmpMatrix;

    tmpMatrix = CMatrix_MatrixMultMatrix(&scaleMat, &mat90x);
    tmpMatrix = CMatrix_MatrixMultMatrix(&tmpMatrix, &mat90z);
    tmpMatrix = CMatrix_MatrixMultMatrix(&tmpMatrix, &matAttitude);
    tmpMatrix = CMatrix_MatrixMultMatrix(&tmpMatrix, &matTrans);

    return tmpMatrix;
}

Plane3D::Plane3D()
= default;

