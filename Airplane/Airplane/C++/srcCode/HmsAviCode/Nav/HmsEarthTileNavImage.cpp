#include "HmsEarthTileNavImage.h"

#include "render/HmsGLProgram.h"
#include "render/HmsGLProgramState.h"
#include "render/HmsGLStateCache.h"
#include "render/HmsRenderer.h"
#include "render/HmsTexture2D.h"
#include "base/HmsImage.h"
#include "HmsGlobal.h"
USING_NS_HMS;

CHmsEarthTileNavImage * CHmsEarthTileNavImage::Create()
{
	CHmsEarthTileNavImage *ret = new CHmsEarthTileNavImage();
	if (ret && ret->Init())
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

CHmsEarthTileNavImage::CHmsEarthTileNavImage()
{

}

bool CHmsEarthTileNavImage::Init()
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
	m_stateBlock->setDepthTest(false);
	m_stateBlock->setDepthWrite(true);
	m_stateBlock->setBlend(true);
	m_stateBlock->setBlendFunc(BlendFunc::ALPHA_PREMULTIPLIED);

	m_meshCommand.setSkipBatching(true);
	m_meshCommand.setTransparent(true);

	return true;
}

void CHmsEarthTileNavImage::InitTileInfo3DEarth(const NavImageTileStu &tileStu)
{
	if (tileStu.imagePath.empty())
	{
		m_texture = nullptr;
		return;
	}
	m_texture = GetTileTexture(tileStu.imagePath);

	double xStart = MATH_DEG_TO_RAD(tileStu.lonLatLT.x);
	double xEnd = MATH_DEG_TO_RAD(tileStu.lonLatRB.x);
	double xLength = xEnd - xStart;

	double yStart = MATH_DEG_TO_RAD(tileStu.lonLatLT.y);
	double yEnd = MATH_DEG_TO_RAD(tileStu.lonLatRB.y);
	double yLength = yEnd - yStart;

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
		//double dLat = MercatorPosYToLatRadian(yTemp);
		double dLat = yTemp;

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
			auto & tempPos = m_pData[iv];

			tempPos.texCoords.u = float(ndc_x);
			tempPos.texCoords.v = 1.0f - float(ndc_y);

			auto qy = Quaternion(y, dLong);
			auto qx = Quaternion(x, dLat);
			qy *= qx;
			tempPos.vertices = qy*z;
			m_aabb.updateMinMax(tempPos.vertices);
		}
	}

	int a = 0;
	m_bDirty = true;
}

void CHmsEarthTileNavImage::Draw(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	if (m_eDrawState == TitleDrawState::noDraw || m_texture == nullptr)
	{
		return;
	}
	if (nullptr == m_pData)
	{
		return;
	}
	if (m_bDirty)
	{
		if (0 == m_nVBO)
			glGenBuffers(1, &m_nVBO);

		glBindBuffer(GL_ARRAY_BUFFER, m_nVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(V3F_T2F)*s_nVerticalsCnt, m_pData, GL_STATIC_DRAW);

		if (0 == s_nVEAB)
			glGenBuffers(1, &s_nVEAB);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_nVEAB);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)* s_nIndicesCnt, s_pIndices, GL_STATIC_DRAW);
		m_bDirty = false;

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

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
#if _NAV_SVS_LOGIC_JUDGE
	if (!renderer)
		return;
#endif
	renderer->addCommand(&m_meshCommand/*, RenderQueue::QUEUE_GROUP::TRANSPARENT_3D*/);
}

void CHmsEarthTileNavImage::UpdateDrawState(HmsAviPf::Frustum * frustum, int nLayer)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!frustum)
		return;
#endif
	if (!m_texture)
	{
		return;
	}
	if (IsVisitableByFrustum(frustum))
	{
		m_eDrawState = TitleDrawState::drawSelf;
	}
	else
	{
		m_eDrawState = TitleDrawState::noDraw;
	}
}



