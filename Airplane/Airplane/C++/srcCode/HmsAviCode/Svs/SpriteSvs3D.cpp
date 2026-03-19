//#include "base\ccMacros.h"
//#include "platform\CCFileUtils.h"
//#include "base\CCDirector.h"
//#include "renderer\CCTexture2D.h"
//#include "renderer\CCTextureCache.h"
#include "render/HmsTexture2D.h"
#include "render/HmsTextureCache.h"
#include "Calculate/CalculateZs.h"
#include "SpriteSvs3D.h"

#include "HmsAviDisplayUnit.h"

bool SpriteSvs3D::InitWithFile(const std::string& filename)
{
	SpriteSvs3DData* ps3dd = Load3DDataFromCache(filename);
	if (ps3dd)
	{
		if (InitFrom(ps3dd))
		{
			return true;
		}
	}

	ps3dd = new SpriteSvs3DData;
	if (Load3DDataFromFile(filename, ps3dd))
	{
		if (InitFrom(ps3dd))
		{
			SpriteSvs3DCache::getInstance()->AddSprite3DData(filename, ps3dd);
			return true;
		}
	}

	delete ps3dd;
	return false;
}

bool SpriteSvs3D::Load3DDataFromFile(const std::string& filename, SpriteSvs3DData* ps3dd)
{
	return SpriteSvs3DCache::getInstance()->Load3DObj(filename, nullptr, ps3dd);
}

bool SpriteSvs3D::InitFrom(SpriteSvs3DData* ps3dd)
{
	if (ps3dd->shapes.size()<=0)
	{
		return false;
	}
	_pnodeRoot = CNode_CreateNode(EN_NodeType::Node);
	
	pCNode pnodeDrawable = CreateSprite3DNode(ps3dd);
	if (pnodeDrawable == nullptr)
	{
		CNode_FreeNode(_pnodeRoot);
		return false;
	}
	_pnodeMatrix = CreateSprite3DMatrixNode();

	CNode_AddChild(_pnodeMatrix, pnodeDrawable);
	CNode_AddChild(_pnodeRoot, _pnodeMatrix);
	return true;
}

SpriteSvs3DData* SpriteSvs3D::Load3DDataFromCache(const std::string& path)
{
	return SpriteSvs3DCache::getInstance()->getSprite3DData(path);
}

void SpriteSvs3D::Render()
{
    CNode_RenderChild(_pnodeRoot, HmsAviPf::Mat4d::IDENTITY, false);
}


//************************************
// Method:    CreateSprite3DNode
// FullName:  Sprite3D::CreateSprite3DNode
// Access:    virtual protected 
// Desc:      
// Returns:   pCNode
// Qualifier:
// Parameter: Sprite3DData * ps3dd
// Author:    yan.jiang
//************************************
pCNode SpriteSvs3D::CreateSprite3DNode(SpriteSvs3DData* ps3dd)
{
	if (ps3dd == nullptr)
	{
		return nullptr;
	}
	auto cntShapes = ps3dd->shapes.size();
	auto cntMats = ps3dd->materials.size();
	if (cntMats != 0 && cntMats != cntShapes)
	{
		return nullptr;
	}

	auto pnodeG = CNode_CreateNode(EN_NodeType::Group);

	CVec3f vertexTempf;
	CVec2f textcoord;
	CVec4f color;
	float alt;
	unsigned int inds;

	for (int i = 0; i < (int)cntShapes; ++i)	
	{
		auto pnodeDraw = CNode_CreateNode(EN_NodeType::NT_Drawable);
		pCDrawable pdraw = (pCDrawable)pnodeDraw->pNodeData;

		unsigned int vtxCnt = ps3dd->shapes[i].mesh.positions.size() / 3;
		unsigned int indCnt = ps3dd->shapes[i].mesh.indices.size();

		float* pD = ps3dd->shapes[i].mesh.positions.data();
		float* pT = ps3dd->shapes[i].mesh.texcoords.data();
		float* pC = new float[vtxCnt * 4];
		float* pA = new float[vtxCnt];
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
			inds = *(unsigned short*)(pI + j);
			//CVector_Pushback(pdraw->_indices, &inds);
            pdraw->_indices2.push_back(inds);
		}

		DrawableNodeCreateVBO(pdraw);

		delete[] pC;
		delete[] pA;

		//texture
		std::string path1;
		if (ps3dd->materials[i].ambient_texname.compare(""))
		{
			path1 = ps3dd->materials[i].ambient_texname;
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

pCNode SpriteSvs3D::CreateSprite3DMatrixNode()
{
	pCNode pnodemat = CNode_CreateNode(EN_NodeType::MatrixTransform);
	pCMatrixTransform pnodeMatTrans = (pCMatrixTransform)(pnodemat->pNodeData);

	pnodeMatTrans->matrix = CMatrix_Identity();

	return pnodemat;
}
