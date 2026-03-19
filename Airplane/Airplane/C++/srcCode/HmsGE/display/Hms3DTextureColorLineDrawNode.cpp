#include "Hms3DTextureColorLineDrawNode.h"
#include "render/HmsGLProgram.h"
#include "render/HmsGLProgramState.h"
#include "render/HmsGLStateCache.h"
#include "render/HmsRenderer.h"
#include "render/HmsTexture2D.h"
#include "base/HmsImage.h"
#include "display/HmsImageNode.h"

NS_HMS_BEGIN

CHms3DTextureColorLineDrawNode * CHms3DTextureColorLineDrawNode::Create(int nPreAssignCapacity)
{
	CHms3DTextureColorLineDrawNode *ret = new CHms3DTextureColorLineDrawNode();
	if (ret && ret->Init(nPreAssignCapacity))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

CHms3DTextureColorLineDrawNode::CHms3DTextureColorLineDrawNode()
	: m_pBuffer(nullptr)
	, m_pIndices(nullptr)
	, m_nVBO(0)
	, m_nVEAB(0)
	, m_bDirty(false)
	, m_fLineWidth(2.0f)
{
	m_bufferCapacity = 0;
	m_bufferCount = 0;
	m_indicesCapacity = 0;
	m_indicesCount = 0;
}

CHms3DTextureColorLineDrawNode::~CHms3DTextureColorLineDrawNode()
{
	if (m_pBuffer)
	{
		free(m_pBuffer);
		m_pBuffer = nullptr;
	}

	if (m_pIndices)
	{
		free(m_pIndices);
		m_pIndices = nullptr;
	}

	if (m_nVBO)
	{
		glDeleteBuffers(1, &m_nVBO);
		m_nVBO = 0;
	}
}

bool CHms3DTextureColorLineDrawNode::Init(int nPreAssignCapacity)
{
	if (!CHmsNode::Init())
	{
		return false;
	}

	SetGLProgramState(GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR));//SHADER_3D_POSITION_TEXTURE

	ensureBufferCapacity(nPreAssignCapacity);
	ensureIndicesCapacity(nPreAssignCapacity);

	auto glProgramState = GetGLProgramState();
	if (glProgramState)
	{
		glProgramState->setVertexAttribPointer(GLProgram::ATTRIBUTE_NAME_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(V3F_C4B_T2F), (GLvoid *)offsetof(V3F_C4B_T2F, vertices));
		glProgramState->setVertexAttribPointer(GLProgram::ATTRIBUTE_NAME_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(V3F_C4B_T2F), (GLvoid *)offsetof(V3F_C4B_T2F, colors));
		glProgramState->setVertexAttribPointer(GLProgram::ATTRIBUTE_NAME_TEX_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(V3F_C4B_T2F), (GLvoid *)offsetof(V3F_C4B_T2F, texCoords));
	}
	
	m_stateBlock = RenderState::StateBlock::create();

	m_stateBlock->setCullFace(true);
	m_stateBlock->setCullFaceSide(RenderState::CULL_FACE_SIDE_FRONT);
	//m_stateBlock->setCullFaceSide(RenderState::CULL_FACE_SIDE_FRONT_AND_BACK);
	m_stateBlock->setDepthTest(false);
	m_stateBlock->setDepthWrite(false);
	m_stateBlock->setBlend(true);
	m_stateBlock->setBlendFunc(BlendFunc::ALPHA_PREMULTIPLIED);

	glGenBuffers(1, &m_nVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_nVBO);
	glGenBuffers(1, &m_nVEAB);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_nVEAB);

	m_bDirty = true;

	return true;
}


void CHms3DTextureColorLineDrawNode::Draw(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	if (m_bufferCount && m_indicesCount)
	{
		m_customCommand.init(m_globalZOrder, transform, flags);
		m_customCommand.func = std::bind(&CHms3DTextureColorLineDrawNode::OnDraw, this, transform, flags);
		renderer->addCommand(&m_customCommand);
	}
}

void CHms3DTextureColorLineDrawNode::OnDraw(const Mat4 &transform, uint32_t flags)
{
	auto glProgramState = GetGLProgramState();

	if (m_bDirty)
	{
		if (nullptr == m_texture)
		{
			m_texture = CHmsImageNode::GetDefalutWhiteImageTexture();
		}

		glBindBuffer(GL_ARRAY_BUFFER, m_nVBO);
		glProgramState->applyAttributes();
		glBufferData(GL_ARRAY_BUFFER, sizeof(V3F_C4B_T2F)*m_bufferCount, m_pBuffer, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_nVEAB);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)* m_indicesCount, m_pIndices, GL_DYNAMIC_DRAW);
		m_bDirty = false;

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_nVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_nVEAB);

	glProgramState->applyGLProgram(transform);
	glProgramState->applyAttributes();

	GLuint texId = m_texture ? m_texture->getName() : 0;
	GL::bindTexture2D(texId);

	if (m_stateBlock)
	{
		m_stateBlock->bind();
	}
	glLineWidth(m_fLineWidth);
	glDrawElements(GL_LINES, (GLsizei)m_indicesCount, GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void CHms3DTextureColorLineDrawNode::Visit(Renderer * renderer, const Mat4 & parentTransform, uint32_t parentFlags)
{
	CHmsNode::Visit(renderer, parentTransform, parentFlags);
}

void CHms3DTextureColorLineDrawNode::SetLineWidth(float fLineWidth)
{
	m_fLineWidth = fLineWidth;
}

void CHms3DTextureColorLineDrawNode::SetDepthTest(bool bTest)
{
	if (m_stateBlock)
	{
		m_stateBlock->setDepthTest(true);
		m_stateBlock->setDepthWrite(true);
	}
}

void CHms3DTextureColorLineDrawNode::clear()
{
	m_bufferCount = 0;
	m_indicesCount = 0;
	m_bDirty = true;
}

void CHms3DTextureColorLineDrawNode::ensureBufferCapacity(int count)
{
	HMSASSERT(count >= 0, "capacity must be >= 0");

	if (m_bufferCount + count > m_bufferCapacity)
	{
		m_bufferCapacity += MAX(m_bufferCapacity, count);
		m_pBuffer = (V3F_C4B_T2F*)realloc(m_pBuffer, m_bufferCapacity*sizeof(V3F_C4B_T2F));
	}
}

void CHms3DTextureColorLineDrawNode::ensureIndicesCapacity(int count)
{
	HMSASSERT(count >= 0, "capacity must be >= 0");

	if (m_indicesCount + count > m_indicesCapacity)
	{
		m_indicesCapacity += MAX(m_indicesCapacity, count);
		m_pIndices = (GLuint*)realloc(m_pIndices, m_indicesCapacity*sizeof(GLuint));
	}
}

V3F_C4B_T2F * CHms3DTextureColorLineDrawNode::GetCurrentVerticalPosBegin()
{
	return m_pBuffer + m_bufferCount;
}

GLuint * CHms3DTextureColorLineDrawNode::GetCurrentIndicesPosBegin()
{
	return m_pIndices + m_indicesCount;
}

void CHms3DTextureColorLineDrawNode::DrawLines(const std::vector<Vec3> & vPos, const Color4F & color /*= Color4F::WHITE*/)
{
	unsigned int nVertexCount = vPos.size();
	unsigned int nIndicesCount = (nVertexCount >> 1) * 2;

	ensureBufferCapacity(nVertexCount);
	ensureIndicesCapacity(nIndicesCount);

	V3F_C4B_T2F * pPoss = GetCurrentVerticalPosBegin();
	GLuint * pIndex = GetCurrentIndicesPosBegin();
	int nVertexBegin = m_bufferCount;

	V3F_C4B_T2F temp = { Vec3(), Color4B(color), Tex2F(0, 0) };

	unsigned int nPos = 0;
	for (auto & c:vPos)
	{
		temp.vertices = c;
		pPoss[nPos++] = temp;
	}

	nPos = 0;
	for (unsigned int i = 0; i < nIndicesCount; i++)
	{
		pIndex[nPos++] = nVertexBegin + i;
	}

	m_bufferCount += nVertexCount;
	m_indicesCount += nIndicesCount;
	m_bDirty = true;
}

void CHms3DTextureColorLineDrawNode::DrawLineLoop(const std::vector<Vec3> & vPos, const Color4F & color /*= Color4F::WHITE*/)
{
	unsigned int nVertexCount = vPos.size();
	unsigned int nIndicesCount = nVertexCount * 2;

	if (nVertexCount < 2)
	{
		return;
	}

	ensureBufferCapacity(nVertexCount);
	ensureIndicesCapacity(nIndicesCount);

	V3F_C4B_T2F * pPoss = GetCurrentVerticalPosBegin();
	GLuint * pIndex = GetCurrentIndicesPosBegin();
	int nVertexBegin = m_bufferCount;

	V3F_C4B_T2F temp = { Vec3(), Color4B(color), Tex2F(0, 0) };
	
	unsigned int nPos = 0;
	for (auto & c : vPos)
	{
		temp.vertices = c;
		pPoss[nPos++] = temp;
	}

	nPos = 0;
	pIndex[nPos++] = nVertexBegin + nVertexCount - 1;
	pIndex[nPos++] = nVertexBegin;

	for (unsigned int i = 1; i < nVertexCount; i++)
	{
		pIndex[nPos++] = nVertexBegin + i - 1;
		pIndex[nPos++] = nVertexBegin + i;
	}

	m_bufferCount += nVertexCount;
	m_indicesCount += nIndicesCount;
	m_bDirty = true;
}

void CHms3DTextureColorLineDrawNode::DrawLineStrip(const std::vector<Vec3> & vPos, const Color4F & color /*= Color4F::WHITE*/)
{
	unsigned int nVertexCount = vPos.size();
	unsigned int nIndicesCount = (nVertexCount - 1) * 2;

	if (nVertexCount < 2)
	{
		return;
	}

	ensureBufferCapacity(nVertexCount);
	ensureIndicesCapacity(nIndicesCount);

	V3F_C4B_T2F * pPoss = GetCurrentVerticalPosBegin();
	GLuint * pIndex = GetCurrentIndicesPosBegin();
	int nVertexBegin = m_bufferCount;

	V3F_C4B_T2F temp = { Vec3(), Color4B(color), Tex2F(0, 0) };

	unsigned int nPos = 0;
	for (auto & c : vPos)
	{
		temp.vertices = c;
		pPoss[nPos++] = temp;
	}

	nPos = 0;

	for (unsigned int i = 1; i < nVertexCount; i++)
	{
		pIndex[nPos++] = nVertexBegin + i - 1;
		pIndex[nPos++] = nVertexBegin + i;
	}

	m_bufferCount += nVertexCount;
	m_indicesCount += nIndicesCount;
	m_bDirty = true;
}

NS_HMS_END