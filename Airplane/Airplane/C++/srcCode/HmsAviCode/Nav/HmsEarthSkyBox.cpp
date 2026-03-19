#include "HmsEarthSkyBox.h"

#include "render/HmsGLProgram.h"
#include "render/HmsGLProgramState.h"
#include "render/HmsGLStateCache.h"
#include "render/HmsRenderer.h"
#include "render/HmsTexture2D.h"
#include "base/HmsImage.h"
#include "HmsGlobal.h"

GLuint CHmsEarthSkyBox::s_nVEAB = 0;
GLushort * CHmsEarthSkyBox::s_pIndices = nullptr;
GLuint CHmsEarthSkyBox::s_nIndicesCnt = 6;
GLuint CHmsEarthSkyBox::s_nVerticalsCnt = 4;

CHmsEarthSkyBox::CHmsEarthSkyBox()
: m_pData(nullptr)
, m_nVBO(0)
, m_bDirty(false)
{
	if (nullptr == s_pIndices)
	{
		s_pIndices = new GLushort[s_nIndicesCnt];
#if _NAV_SVS_LOGIC_JUDGE
		if (s_pIndices && s_nIndicesCnt >= 6)
#endif
		{
			s_pIndices[0] = 0;
			s_pIndices[1] = 1;
			s_pIndices[2] = 2;
			s_pIndices[3] = 0;
			s_pIndices[4] = 2;
			s_pIndices[5] = 3;
		}
	}
}

CHmsEarthSkyBox::~CHmsEarthSkyBox()
{

}


bool CHmsEarthSkyBox::Init()
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

	m_stateBlock->setDepthTest(true);
	m_stateBlock->setDepthWrite(true);
	m_stateBlock->setBlend(true);
	m_stateBlock->setBlendFunc(BlendFunc::ALPHA_PREMULTIPLIED);

	m_meshCommand.setSkipBatching(true);
	m_meshCommand.setTransparent(true);

	return true;
}

void CHmsEarthSkyBox::Draw(HmsAviPf::Renderer *renderer, const HmsAviPf::Mat4& transform, uint32_t flags)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!renderer)
		return;
#endif
	GLuint texId = m_texture ? m_texture->getName() : 0;

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

	renderer->addCommand(&m_meshCommand/*, RenderQueue::QUEUE_GROUP::TRANSPARENT_3D*/);
}

void CHmsEarthSkyBox::InitVertices(SkyBoxType t)
{
	if (nullptr == m_pData)
	{
		m_pData = new V3F_T2F[s_nVerticalsCnt];
#if _NAV_SVS_LOGIC_JUDGE
		if (!m_pData)
			return;
#endif
	}

	static const float coords[6][4][3] = {
		{ { +1, -1, -1 }, { -1, -1, -1 }, { -1, +1, -1 }, { +1, +1, -1 } },
		{ { +1, +1, -1 }, { -1, +1, -1 }, { -1, +1, +1 }, { +1, +1, +1 } },
		{ { +1, -1, +1 }, { +1, -1, -1 }, { +1, +1, -1 }, { +1, +1, +1 } },
		{ { -1, -1, -1 }, { -1, -1, +1 }, { -1, +1, +1 }, { -1, +1, -1 } },
		{ { +1, -1, +1 }, { -1, -1, +1 }, { -1, -1, -1 }, { +1, -1, -1 } },
		{ { -1, -1, +1 }, { +1, -1, +1 }, { +1, +1, +1 }, { -1, +1, +1 } }
	};

	int i = (int)t;
#if _NAV_SVS_LOGIC_JUDGE
	for (unsigned int j = 0; j < s_nVerticalsCnt; ++j)  //此处界限值建议使用new的实际个数，预防遍历时的内存错误
#else
	for (unsigned int j = 0; j < 4; ++j)
#endif	
	{
		auto & tempPos = m_pData[j];

		tempPos.texCoords.u = (j == 0 || j == 3);
		tempPos.texCoords.v = (j == 2 || j == 3);

		float dis = 2;

		tempPos.vertices.x = coords[i][j][0] * dis;
		tempPos.vertices.y = coords[i][j][1] * dis;
		tempPos.vertices.z = coords[i][j][2] * dis;
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
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)* s_nIndicesCnt, s_pIndices, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	std::string str = CHmsGlobal::FormatString("res/NavImage/skybox/%d.png", i);
	m_texture = CHmsMercatorEarthTile::GetTileTexture(str);
}



CHmsEarthSkyBoxMgr::CHmsEarthSkyBoxMgr()
{

}

CHmsEarthSkyBoxMgr::~CHmsEarthSkyBoxMgr()
{

}

bool CHmsEarthSkyBoxMgr::Init()
{
	if (!CHmsNode::Init())
	{
		return false;
	}

	for (int i = 0; i < 6; ++i)
	{
		auto tile = CHmsEarthSkyBox::Create();
#if _NAV_SVS_LOGIC_JUDGE
		if (!tile)
			continue;
#endif
		tile->InitVertices((SkyBoxType)i);

		m_rootTile.pushBack(tile);
	}

	return true;
}

void CHmsEarthSkyBoxMgr::Visit(HmsAviPf::Renderer * renderer, const HmsAviPf::Mat4 & parentTransform, uint32_t parentFlags)
{
	for (auto & c : m_rootTile)
	{
		c->Visit(renderer, parentTransform, parentFlags);
	}
}
