#include "Hms3DColorLineDrawNode.h"
#include "render/HmsGLProgram.h"
#include "render/HmsGLProgramState.h"
#include "render/HmsGLStateCache.h"
#include "render/HmsRenderer.h"
#include "render/HmsTexture2D.h"
#include "base/HmsImage.h"

NS_HMS_BEGIN

CHms3DColorLineDrawNode * CHms3DColorLineDrawNode::Create(int nPreAssignCapacity)
{
	CHms3DColorLineDrawNode *ret = new CHms3DColorLineDrawNode();
	if (ret && ret->Init(nPreAssignCapacity))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

CHms3DColorLineDrawNode::CHms3DColorLineDrawNode()
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

CHms3DColorLineDrawNode::~CHms3DColorLineDrawNode()
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

bool CHms3DColorLineDrawNode::Init(int nPreAssignCapacity)
{
	if (!CHmsNode::Init())
	{
		return false;
	}

	SetGLProgramState(GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_NAME_POSITION_COLOR));//SHADER_3D_POSITION_TEXTURE

	ensureBufferCapacity(nPreAssignCapacity);
	ensureIndicesCapacity(nPreAssignCapacity);

	auto glProgramState = GetGLProgramState();
	glProgramState->setVertexAttribPointer(GLProgram::ATTRIBUTE_NAME_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(V3F_C4B), (GLvoid *)offsetof(V3F_C4B, vertices));
	glProgramState->setVertexAttribPointer(GLProgram::ATTRIBUTE_NAME_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(V3F_C4B), (GLvoid *)offsetof(V3F_C4B, colors));
	//glProgramState->setUniformVec4("u_color", Vec4(1, 1, 1, 1));

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


void CHms3DColorLineDrawNode::Draw(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	if (m_bufferCount && m_indicesCount)
	{
		m_customCommand.init(m_globalZOrder, transform, flags);
		m_customCommand.func = std::bind(&CHms3DColorLineDrawNode::OnDraw, this, transform, flags);
		renderer->addCommand(&m_customCommand);
	}
}

void CHms3DColorLineDrawNode::OnDraw(const Mat4 &transform, uint32_t flags)
{
	auto glProgramState = GetGLProgramState();

	if (m_bDirty)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_nVBO);
		glProgramState->applyAttributes();
		glBufferData(GL_ARRAY_BUFFER, sizeof(V3F_C4B)*m_bufferCount, m_pBuffer, GL_DYNAMIC_DRAW);

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

void CHms3DColorLineDrawNode::Visit(Renderer * renderer, const Mat4 & parentTransform, uint32_t parentFlags)
{
	CHmsNode::Visit(renderer, parentTransform, parentFlags);
}

void CHms3DColorLineDrawNode::SetLineWidth(float fLineWidth)
{
	m_fLineWidth = fLineWidth;
}

void CHms3DColorLineDrawNode::SetDepthTest(bool bTest)
{
	if (m_stateBlock)
	{
		m_stateBlock->setDepthTest(true);
		m_stateBlock->setDepthWrite(true);
	}
}

void CHms3DColorLineDrawNode::clear()
{
	m_bufferCount = 0;
	m_indicesCount = 0;
	m_bDirty = true;
}

void CHms3DColorLineDrawNode::ensureBufferCapacity(int count)
{
	HMSASSERT(count >= 0, "capacity must be >= 0");

	if (m_bufferCount + count > m_bufferCapacity)
	{
		m_bufferCapacity += MAX(m_bufferCapacity, count);
		m_pBuffer = (V3F_C4B*)realloc(m_pBuffer, m_bufferCapacity*sizeof(V3F_C4B));
	}
}

void CHms3DColorLineDrawNode::ensureIndicesCapacity(int count)
{
	HMSASSERT(count >= 0, "capacity must be >= 0");

	if (m_indicesCount + count > m_indicesCapacity)
	{
		m_indicesCapacity += MAX(m_indicesCapacity, count);
		m_pIndices = (GLuint*)realloc(m_pIndices, m_indicesCapacity*sizeof(GLuint));
	}
}

V3F_C4B * CHms3DColorLineDrawNode::GetCurrentVerticalPosBegin()
{
	return m_pBuffer + m_bufferCount;
}

GLuint * CHms3DColorLineDrawNode::GetCurrentIndicesPosBegin()
{
	return m_pIndices + m_indicesCount;
}

void CHms3DColorLineDrawNode::DrawLines(const std::vector<Vec3> & vPos, const Color4F & color /*= Color4F::WHITE*/)
{
	unsigned int nVertexCount = vPos.size();
	unsigned int nIndicesCount = (nVertexCount >> 1) * 2;

	ensureBufferCapacity(nVertexCount);
	ensureIndicesCapacity(nIndicesCount);

	V3F_C4B * pPoss = GetCurrentVerticalPosBegin();
	GLuint * pIndex = GetCurrentIndicesPosBegin();
	int nVertexBegin = m_bufferCount;

	V3F_C4B temp = { Vec3(), Color4B(color) };

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

void CHms3DColorLineDrawNode::DrawLineLoop(const std::vector<Vec3> & vPos, const Color4F & color /*= Color4F::WHITE*/)
{
	unsigned int nVertexCount = vPos.size();
	unsigned int nIndicesCount = nVertexCount * 2;

	if (nVertexCount < 2)
	{
		return;
	}

	ensureBufferCapacity(nVertexCount);
	ensureIndicesCapacity(nIndicesCount);

	V3F_C4B * pPoss = GetCurrentVerticalPosBegin();
	GLuint * pIndex = GetCurrentIndicesPosBegin();
	int nVertexBegin = m_bufferCount;

	V3F_C4B temp = { Vec3(), Color4B(color) };
	
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

void CHms3DColorLineDrawNode::DrawLineStrip(const std::vector<Vec3> & vPos, const Color4F & color /*= Color4F::WHITE*/)
{
	unsigned int nVertexCount = vPos.size();
	unsigned int nIndicesCount = (nVertexCount - 1) * 2;

	if (nVertexCount < 2)
	{
		return;
	}

	ensureBufferCapacity(nVertexCount);
	ensureIndicesCapacity(nIndicesCount);

	V3F_C4B * pPoss = GetCurrentVerticalPosBegin();
	GLuint * pIndex = GetCurrentIndicesPosBegin();
	int nVertexBegin = m_bufferCount;

	V3F_C4B temp = { Vec3(), Color4B(color) };

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