#include "HmsMercatorEarthTile.h"
#include "render/HmsGLProgram.h"
#include "render/HmsGLProgramState.h"
#include "render/HmsGLStateCache.h"
#include "render/HmsRenderer.h"
#include "render/HmsTexture2D.h"
#include "base/HmsImage.h"
#include "HmsGlobal.h"
USING_NS_HMS;

#define EARTHTILE_BG_GRADE 4


GLuint CHmsMercatorEarthTile::s_nVEAB = 0;
GLushort * CHmsMercatorEarthTile::s_pIndices = nullptr;
GLuint CHmsMercatorEarthTile::s_nIndicesCnt = 0;
GLuint CHmsMercatorEarthTile::s_nVerticalsCnt = 0;
bool CHmsMercatorEarthTile::s_bDrawTexture = true;

#ifdef __vxworks
std::string CHmsMercatorEarthTile::s_strTilePathFormat = "/ata1:1/MapTile/satellite/%d/%d/%d.jpg";
#else
std::string CHmsMercatorEarthTile::s_strTilePathFormat = "F:/mapTile/satellite/%d/%d/%d.jpg";
#endif

const int c_nNumRows = 17;
const int c_nNumCols = 17;

MercatorEarthTileInfo::MercatorEarthTileInfo()
{
	nLayer = 1;
	nX = 0;
	nY = 0;
}

MercatorEarthTileInfo::MercatorEarthTileInfo(int nLayer, int nX, int nY)
{
	this->nLayer = nLayer;
	this->nX = nX;
	this->nY = nY;
}

MercatorEarthTileInfo::MercatorEarthTileInfo(const MercatorEarthTileInfo & info, int nChildIndex)
{
	nLayer = info.nLayer + 1;
	nX = (info.nX << 1) + nChildIndex%2;
	nY = (info.nY << 1) + nChildIndex/2;
}

void MercatorEarthTileInfo::SetInfo(int nLayer, int nX, int nY)
{
	this->nLayer = nLayer;
	this->nX = nX;
	this->nY = nY;
}

MercatorEarthTileInfo MercatorEarthTileInfo::GetChild(int nChildIndex)
{
	return MercatorEarthTileInfo(*this, nChildIndex);
}

void CHmsMercatorEarthTile::SetEarthTilePath(const std::string & strPath, const std::string & strSuffix)
{
#if 0
	auto it = strPath.rbegin();
	if (it != strPath.rend())
	{
		if ((*it != '/') || (*it != '\\'))
		{
			s_strTilePathFormat = strPath + "/%d/%d/%d";
		}
		else
		{
			s_strTilePathFormat = strPath + "%d/%d/%d";
		}
		s_strTilePathFormat += strSuffix;
	}
#else
	if (strPath.size())
	{
		auto c = strPath.at(strPath.length() - 1);
		if ((c == '/') || (c == '\\'))
		{
			s_strTilePathFormat = strPath + "%d/%d/%d";
		}
		else
		{
			s_strTilePathFormat = strPath + "/%d/%d/%d";
		}
		s_strTilePathFormat += strSuffix;
	}
#endif
}

CHmsMercatorEarthTile * CHmsMercatorEarthTile::Create(int nLayer, int nX, int nY)
{
	CHmsMercatorEarthTile *ret = new CHmsMercatorEarthTile();
	if (ret && ret->Init(nLayer, nX, nY))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

CHmsMercatorEarthTile * CHmsMercatorEarthTile::Create(const MercatorEarthTileInfo & info)
{
	return Create(info.nLayer, info.nX, info.nY);
}

void CHmsMercatorEarthTile::SetDrawMode(bool bDrawTexture)
{
	s_bDrawTexture = bDrawTexture;
}

CHmsMercatorEarthTile::CHmsMercatorEarthTile()
	: m_pData(nullptr)
	, m_nVBO(0)
	, m_bDirty(false)
	, m_eDrawState(TitleDrawState::drawSelf)
	, m_pImage(nullptr)
	, m_bIsTextureRelease(true)
{
	if (nullptr == s_pIndices)
	{
		s_nIndicesCnt = 6 * 16 * 16;
		s_pIndices = new GLushort[s_nIndicesCnt];
		int nPos = 0;
		int nCurStart = 0;
		for (int i = 0; i < 16; i++)
		{
			for (int j = 0; j < 16; j++)
			{
				nCurStart = i * 17 + j;

				s_pIndices[nPos++] = nCurStart + 1;
				s_pIndices[nPos++] = nCurStart ;
				s_pIndices[nPos++] = nCurStart + 17;
				s_pIndices[nPos++] = nCurStart + 1;
				s_pIndices[nPos++] = nCurStart + 17;
				s_pIndices[nPos++] = nCurStart + 18;
			}
		}
#if 0
		s_pIndices[0] = 0;
		s_pIndices[1] = 1;
		s_pIndices[2] = 2;
		s_pIndices[3] = 0;
		s_pIndices[4] = 2;
		s_pIndices[5] = 3;
#endif
	}


	m_vecTileChildState.assign(4, ChildState::Child_NO);
}

CHmsMercatorEarthTile::~CHmsMercatorEarthTile()
{
	int a = 0;
}

bool CHmsMercatorEarthTile::Init(int nLayer, int nX, int nY)
{
	if (!CHmsNode::Init())
	{
		return false;
	}

	SetGLProgramState(GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_3D_POSITION_TEXTURE));

	auto glProgramState = GetGLProgramState();
#if _NAV_SVS_LOGIC_JUDGE
	if (!glProgramState)
		return false;
#endif
	glProgramState->setVertexAttribPointer(GLProgram::ATTRIBUTE_NAME_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(V3F_T2F), (GLvoid *)offsetof(V3F_T2F, vertices));
	glProgramState->setVertexAttribPointer(GLProgram::ATTRIBUTE_NAME_TEX_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(V3F_T2F), (GLvoid *)offsetof(V3F_T2F, texCoords));
	glProgramState->setUniformVec4("u_color", Vec4(1, 1, 1, 1));

	m_stateBlock = RenderState::StateBlock::create();
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_stateBlock)
		return false;
#endif
	m_stateBlock->setCullFace(true);
	m_stateBlock->setCullFaceSide(RenderState::CULL_FACE_SIDE_FRONT);
	//m_stateBlock->setCullFaceSide(RenderState::CULL_FACE_SIDE_FRONT_AND_BACK);

	if (nLayer > EARTHTILE_BG_GRADE)
	{
		m_stateBlock->setDepthTest(false);
	}
	else
	{
		m_stateBlock->setDepthTest(true);
	}

	

	m_stateBlock->setDepthWrite(true);
	m_stateBlock->setBlend(true);
	m_stateBlock->setBlendFunc(BlendFunc::ALPHA_PREMULTIPLIED);

	m_meshCommand.setSkipBatching(true);
	m_meshCommand.setTransparent(true);

	//InitTileInfoPlane(nLayer, nX, nY);
	//InitTileInfo3DEarth(nLayer, nX, nY);

	//auto tilePath = GetTilePath(nLayer, nX, nY);
	//m_texture = GetTileTexture(tilePath);
	//CHmsLoadTextureThread::GetInstance()->AddLoadTask(tilePath, HMS_CALLBACK_1(CHmsMercatorEarthTile::SetTileImageCallback, this));

	return true;
}

std::string CHmsMercatorEarthTile::GetTilePath(int nLayer, int nX, int nY)
{
// #ifdef __vxworks
// 	std::string str = CHmsGlobal::FormatString("/ata1:1/MapTile/satellite/%d/%d/%d.jpg", nLayer, nX, nY);
// #else
	std::string  str = CHmsGlobal::FormatString(s_strTilePathFormat.c_str(), nLayer, nX, nY);
//#endif
	return str;
}

Texture2D * CHmsMercatorEarthTile::GetTileTexture(const std::string & path)
{
	Texture2D * texture = nullptr;
	Image* image = nullptr;
	do
	{
		image = new  Image();
		HMS_BREAK_IF(nullptr == image);

		bool bRet = image->initWithImageFile(path);
		HMS_BREAK_IF(!bRet);

		texture = new  Texture2D();

		if (texture && texture->initWithImage(image))
		{
			texture->autorelease();
		}
		else
		{
			CCLOG("HmsAviPf: Couldn't create texture for file:%s in TextureCache", path.c_str());
		}
	} while (0);

	HMS_SAFE_RELEASE(image);
	return texture;
}

#define ZS_E 2.718281828459	
#define ZS_PI 3.14159265358979323846

double CHmsMercatorEarthTile::MercatorPosYToLatDegree(double dPosY)
{
	double z = pow(ZS_E, dPosY*2);
	double siny = (z - 1) / (z + 1);
    return HmsMath_asin(siny) * 180 / ZS_PI;
}

double CHmsMercatorEarthTile::MercatorPosYToLatRadian(double dPosY)
{
	double z = pow(ZS_E, dPosY * 2);
	double siny = (z - 1) / (z + 1);
    return HmsMath_asin(siny);
}

void CHmsMercatorEarthTile::InitTileInfoPlane(int nLayer, int nX, int nY)
{
	int nLayerCnt = 1 << nLayer;
	int nLayerHalfCnt = 1 << (nLayer - 1);

	double xStart = (nX - nLayerHalfCnt)*ZS_PI / nLayerHalfCnt;
	double xEnd = (nX + 1 - nLayerHalfCnt)*ZS_PI / nLayerHalfCnt;
	double xLength = xEnd - xStart;

	double yStart = (nY - nLayerHalfCnt)*ZS_PI / nLayerHalfCnt;
	double yEnd = (nY + 1 - nLayerHalfCnt)*ZS_PI / nLayerHalfCnt;
	double yLength = yEnd - yStart;

	m_tileInfo.SetInfo(nLayer, nX, nY);

	if (0 == s_nVerticalsCnt)
	{
		s_nVerticalsCnt = c_nNumRows * c_nNumCols;
	}
	
	if (nullptr == m_pData)
	{
		m_pData = new V3F_T2F[s_nVerticalsCnt];
	}

	m_fLongEast = xStart;
	m_fLongWest = xEnd;

	for (int j = 0; j < c_nNumRows; j++)
	{
		double ndc_y = ((double)j) / (double)(c_nNumRows - 1);
		double yTemp = yStart + yLength*ndc_y;
		double dLat = MercatorPosYToLatRadian(yTemp);

		if (j == 0)
			m_fLatNorth = dLat;
		else if (j == (c_nNumRows - 1))
			m_fLatSouth = dLat;

		for (int i = 0; i < c_nNumCols; i++)
		{
			double ndc_x = ((double)i) / (double)(c_nNumCols - 1);
			double dLong = (xStart + xLength * ndc_x);

			//计算贴图坐标
			unsigned int iv = j*c_nNumCols + i;
			//Vec3 ndc(((double)i) / (double)(nNumCols - 1), ((double)j) / (double)(nNumRows - 1), 0.0);
			auto & tempPos = m_pData[iv];

			tempPos.texCoords.u = float(ndc_x);
			tempPos.texCoords.v = 1.0f - float(ndc_y);

			tempPos.vertices.x = float(dLong);
			tempPos.vertices.y = float(dLat);
			tempPos.vertices.z = float(0);
		}
	}

#if 0
	m_pData[0].vertices.set(0, 0, 0);
	m_pData[1].vertices.set(0, 200, 0);
	m_pData[2].vertices.set(200, 200, 0);
	m_pData[3].vertices.set(200, 0, 0);

	m_pData[0].texCoords.u = 0;
	m_pData[1].texCoords.u = 0;
	m_pData[2].texCoords.u = 1.0;
	m_pData[3].texCoords.u = 1.0;
	m_pData[0].texCoords.v = 0;
	m_pData[1].texCoords.v = 1.0;
	m_pData[2].texCoords.v = 1.0;
	m_pData[3].texCoords.v = 0;
#endif

	m_bDirty = true;
}

void CHmsMercatorEarthTile::InitTileInfo3DEarth(int nLayer, int nX, int nY)
{
	if (m_tileInfo.nLayer == nLayer && m_tileInfo.nX == nX && m_tileInfo.nY == nY)
	{
		return;
	}

	int nLayerCnt = 1 << nLayer;
	int nLayerHalfCnt = 1 << (nLayer - 1);

	double xStart = (nX - nLayerHalfCnt)*ZS_PI / nLayerHalfCnt;
	double xEnd = (nX + 1 - nLayerHalfCnt)*ZS_PI / nLayerHalfCnt;
	double xLength = xEnd - xStart;

	double yStart = (nY - nLayerHalfCnt)*ZS_PI / nLayerHalfCnt;
	double yEnd = (nY + 1 - nLayerHalfCnt)*ZS_PI / nLayerHalfCnt;
	double yLength = yEnd - yStart;

	m_tileInfo.SetInfo(nLayer, nX, nY);

	m_fLongEast = xStart;
	m_fLongWest = xEnd;

	if (0 == s_nVerticalsCnt)
	{
		s_nVerticalsCnt = c_nNumRows * c_nNumCols;
	}

	if (nullptr == m_pData)
	{
		m_pData = new V3F_T2F[s_nVerticalsCnt];
	}

	Vec3 z(0.0f, 0.0f, 1.0f);
	Vec3 x(1.0f, 0.0f, 0.0f);
	Vec3 y(0.0f, 1.0f, 0.0f);

	Vec3 min;
	Vec3 max;

	for (int j = 0; j < c_nNumRows; j++)
	{
		double ndc_y = ((double)j) / (double)(c_nNumRows - 1);
		double yTemp = yStart + yLength*ndc_y;
		double dLat = MercatorPosYToLatRadian(yTemp);

		if (j == 0)
			m_fLatNorth = dLat;
		else if (j == (c_nNumRows - 1))
			m_fLatSouth = dLat;

		for (int i = 0; i < c_nNumCols; i++)
		{
			double ndc_x = ((double)i) / (double)(c_nNumCols - 1);
			double dLong = (xStart + xLength * ndc_x);

			//计算贴图坐标
			unsigned int iv = j*c_nNumCols + i;
			//Vec3 ndc(((double)i) / (double)(nNumCols - 1), ((double)j) / (double)(nNumRows - 1), 0.0);
			auto & tempPos = m_pData[iv];

			tempPos.texCoords.u = float(ndc_x);
			tempPos.texCoords.v = 1.0f - float(ndc_y);

			auto qy = Quaternion(y, dLong);
			auto qx = Quaternion(x, -dLat);
			qy *= qx;
			tempPos.vertices = qy*z;
			//tempPos.vertices *= 100;
			m_aabb.updateMinMax(tempPos.vertices);
		}
	}

	m_bDirty = true;
	
	if (m_bDirty)
	{
		m_bDirty = false;

		if (0 == m_nVBO)
			glGenBuffers(1, &m_nVBO);

		glBindBuffer(GL_ARRAY_BUFFER, m_nVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(V3F_T2F)*s_nVerticalsCnt, m_pData, GL_STATIC_DRAW);

		if (0 == s_nVEAB)
			glGenBuffers(1, &s_nVEAB);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_nVEAB);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * s_nIndicesCnt, s_pIndices, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

// 	if (m_texture)
// 	{
// 		m_texture = nullptr;
// 	}
	//++m_nSendCount;
	//auto tilePath = GetTilePath(nLayer, nX, nY);
	//CHmsLoadTextureThread::GetInstance()->AddLoadTask(nLayer, nX, nY, HMS_CALLBACK_1(CHmsMercatorEarthTile::SetTileImageCallback, this), (int)this);
}

void CHmsMercatorEarthTile::Draw(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!renderer)
		return;
#endif
	if (m_eDrawState == TitleDrawState::noDraw)
	{
		return;
	}
	else if (m_eDrawState == TitleDrawState::drawChild)
	{
		for (auto & c:m_mapTileChild)
		{
			if (c.second)
			{
				c.second->Draw(renderer, transform, flags);
			}
		}
		return;
	}

	

	if (m_texture)
	{
		GLuint texId = m_texture ? m_texture->getName() : 0;

		if (s_bDrawTexture)
		{
			m_meshCommand.init(m_globalZOrder,
				texId,
				GetGLProgramState(),
				m_stateBlock,
				m_nVBO,
				s_nVEAB,
				GL_TRIANGLES,
				GL_UNSIGNED_SHORT,
				s_nIndicesCnt,
				transform,
				CHmsNode::FLAGS_RENDER_AS_3D);
		}
		else
		{
			m_meshCommand.init(m_globalZOrder,
				0,
				GetGLProgramState(),
				m_stateBlock,
				m_nVBO,
				s_nVEAB,
				GL_LINES,
				GL_UNSIGNED_SHORT,
				s_nIndicesCnt,
				transform,
				CHmsNode::FLAGS_RENDER_AS_3D);
		}
		renderer->addCommand(&m_meshCommand/*, RenderQueue::QUEUE_GROUP::TRANSPARENT_3D*/);
	}
// 	else
// 	{
// 		if (m_nSendCount == m_nRevCount)
// 		{
// 			m_meshCommand.init(m_globalZOrder,
// 				0,
// 				GetGLProgramState(),
// 				m_stateBlock,
// 				m_nVBO,
// 				s_nVEAB,
// 				GL_LINES,
// 				GL_UNSIGNED_SHORT,
// 				s_nIndicesCnt,
// 				transform,
//  				CHmsNode::FLAGS_RENDER_AS_3D);
// 			renderer->addCommand(&m_meshCommand/*, RenderQueue::QUEUE_GROUP::TRANSPARENT_3D*/);
// 		}
// 	}

	if (m_eDrawState == TitleDrawState::drawChildAndSelf)
	{
		for (auto & c : m_mapTileChild)
		{
			if (c.second)
			{
				c.second->Draw(renderer, transform, flags);
			}
		}
	}
}

bool CHmsMercatorEarthTile::IsVisitableByFrustum(HmsAviPf::Frustum * frustum)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!frustum)
		return false;
#endif
#define CHECK_FRUSTUM(x) if (!frustum->isOutOfFrustum(x->vertices))return true;

	CHECK_FRUSTUM(GetCentre());
	CHECK_FRUSTUM(GetNorthWest());
	CHECK_FRUSTUM(GetSouthEast());
	CHECK_FRUSTUM(GetSouthWest());
	CHECK_FRUSTUM(GetNorthEast());
	if (!frustum->isOutOfFrustum(m_aabb))
		return true;
	return false;
}

void CHmsMercatorEarthTile::UpdateDrawState(HmsAviPf::Frustum * frustum, int nLayer)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!frustum)
		return;
#endif
	if (!IsVisitableByFrustum(frustum) && nLayer <= EARTHTILE_BG_GRADE)
	{
		m_eDrawState = TitleDrawState::noDraw;
	}
	else
	{
		m_eDrawState = TitleDrawState::drawSelf;
	}
	return;


	if (IsVisitableByFrustum(frustum))
	{
		m_eDrawState = TitleDrawState::drawSelf;

		if (nLayer > m_tileInfo.nLayer)
		{
			for (int i = 0; i < 4; i++)
			{
				auto itFind = m_mapTileChild.find(i);
				if (itFind != m_mapTileChild.end())
				{
					m_eDrawState = TitleDrawState::drawChildAndSelf;
					auto tile = itFind->second;

					if (ChildState::Child_LOADING == m_vecTileChildState[i])
					{
						if (tile->IsTextureOk())
						{
							m_vecTileChildState[i] = ChildState::Child_OK;
						}
					}	
					tile->UpdateDrawState(frustum, nLayer);
				}
				else
				{
					if (ChildState::Child_NO == m_vecTileChildState[i])
					{
						m_vecTileChildState[i] = ChildState::Child_LOADING;

						CreateChildTile(i);
					}			
				}		
			}
			if (m_vecTileChildState[0] == ChildState::Child_OK &&
				m_vecTileChildState[1] == ChildState::Child_OK &&
				m_vecTileChildState[2] == ChildState::Child_OK &&
				m_vecTileChildState[3] == ChildState::Child_OK)
			{
				m_eDrawState = TitleDrawState::drawChild;
			}
		}
	}
	else
	{
		m_eDrawState = TitleDrawState::noDraw;
	}

#if 0
	if (IsVisitableByFrustum(frustum))
	{
		if (nLayer > m_tileInfo.nLayer)
		{
			for (int i = 0; i < 4; i++)
			{
				auto tile = GetOrCreateChildTile(i);
				tile->UpdateDrawState(frustum, nLayer);
			}
			m_eDrawState = TitleDrawState::drawChild;
		}
		else
		{
			m_eDrawState = TitleDrawState::drawSelf;
		}
	}
	else
	{
		m_eDrawState = TitleDrawState::noDraw;
	}
#endif
}

void CHmsMercatorEarthTile::Visit(Renderer * renderer, const Mat4 & parentTransform, uint32_t parentFlags)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!renderer)
		return;
#endif
	CHmsNode::Visit(renderer, parentTransform, parentFlags);
}

HmsAviPf::V3F_T2F * CHmsMercatorEarthTile::GetNorthWest()
{
	return &m_pData[0];
}

HmsAviPf::V3F_T2F * CHmsMercatorEarthTile::GetNorthEast()
{
	return &m_pData[c_nNumCols - 1];
}

HmsAviPf::V3F_T2F * CHmsMercatorEarthTile::GetSouthWest()
{
	return &m_pData[c_nNumCols*(c_nNumRows-1)];
}

HmsAviPf::V3F_T2F * CHmsMercatorEarthTile::GetSouthEast()
{
	return &m_pData[c_nNumCols*c_nNumRows - 1];
}

HmsAviPf::V3F_T2F * CHmsMercatorEarthTile::GetCentre()
{
	return &m_pData[c_nNumCols*c_nNumRows / 2];
}

CHmsMercatorEarthTile * CHmsMercatorEarthTile::GetOrCreateChildTile(int nChildIndex)
{
	auto itFind = m_mapTileChild.find(nChildIndex);
	if (itFind != m_mapTileChild.end())
	{
		return itFind->second;
	}

	auto tileInfo = m_tileInfo.GetChild(nChildIndex);
	auto tile = Create(tileInfo);
	m_mapTileChild.insert(nChildIndex, tile);

	//return Create(m_nLayer + 1, nChildIndex, nChildIndex);
	return tile;
}

void CHmsMercatorEarthTile::CreateChildTile(int nChildIndex)
{
	auto tileInfo = m_tileInfo.GetChild(nChildIndex);
	auto tile = Create(tileInfo);
	m_mapTileChild.insert(nChildIndex, tile);
}

void CHmsMercatorEarthTile::ReloadTexture()
{
// 	 CHmsLoadTextureThread::GetInstance()->AddLoadTask(m_nLayer, m_nX, m_nY, [=](HmsAviPf::Image *p)
// 		{
// 
// 		}
// 		 , (int)this);

	SendLoadTextureRequest(m_tileInfo.nLayer, m_tileInfo.nX, m_tileInfo.nY, true);
}

#define TILE_CALLBACK_4(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, ##__VA_ARGS__)

void CHmsMercatorEarthTile::TextureLoadCallback(HmsAviPf::Image *p, int nLayer, int nX, int nY)
{
	if (m_nLayer == nLayer && m_nX == nX && m_nY == nY)
	{
		if (p && p->getDataLen() > 0)
		{
			CHmsTextureFactory::GetInstance()->AddTask(p, m_nLayer, m_nX, m_nY, TILE_CALLBACK_4(CHmsMercatorEarthTile::CheckTexture, this));
		}
	}
	else
	{
		if (p)
		{
			delete p;
			p = nullptr;
		}
	}
}

void CHmsMercatorEarthTile::SendLoadTextureRequest(int nLayer, int nX, int nY, bool bAlwaysLoad)
{
	if (!bAlwaysLoad)
	{
		if (m_nLayer == nLayer && m_nX == nX && m_nY == nY)
		{
			return;
		}
	}
	m_nLayer = nLayer;
	m_nX = nX;
	m_nY = nY;

	CHmsLoadTextureThread::GetInstance()->AddLoadTask(nLayer, nX, nY, TILE_CALLBACK_4(CHmsMercatorEarthTile::TextureLoadCallback, this), (long)this);
}

void CHmsMercatorEarthTile::CheckTexture(HmsAviPf::Texture2D *pTexture, int nLayer, int nX, int nY)
{
	if (m_nLayer == nLayer && m_nX == nX && m_nY == nY)
	{
		m_texture = pTexture;
		InitTileInfo3DEarth(m_nLayer, m_nX, m_nY);
	}
	
	return;


	//texture要在主线程才能生成，子线程获取不到_name
// 	if (m_pImage && m_pImage->getDataLen() > 0)
// 	{
// 		if (m_nSendCount == m_nRevCount)
// 		{
// 			auto texture = new  HmsAviPf::Texture2D();
// 			if (texture && texture->initWithImage(m_pImage))
// 			{
// 				texture->autorelease();
// 
// 				m_texture = texture;
// 			}
// 		}
// 		delete m_pImage;
// 		m_pImage = nullptr;
// 	}
}





