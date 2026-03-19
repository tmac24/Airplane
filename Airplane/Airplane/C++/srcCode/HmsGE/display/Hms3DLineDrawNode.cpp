#include "Hms3DLineDrawNode.h"
#include "render/HmsGLProgram.h"
#include "render/HmsGLProgramState.h"
#include "render/HmsGLStateCache.h"
#include "render/HmsRenderer.h"
#include "render/HmsTexture2D.h"
#include "base/HmsImage.h"
#include "HmsLog.h"

float g_fHmsTrimDistance = 1.0f;

NS_HMS_BEGIN

CHms3DLineDrawNode * CHms3DLineDrawNode::Create(int nPreAssignCapacity)
{
	CHms3DLineDrawNode *ret = new CHms3DLineDrawNode();
	if (ret && ret->Init(nPreAssignCapacity))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

CHms3DLineDrawNode::CHms3DLineDrawNode()
	: m_pBuffer(nullptr)
    , m_bufferCapacity(0)
    , m_bufferCount(0)
	, m_pIndices(nullptr)
    , m_indicesCapacity(0)
    , m_indicesCount(0)
	, m_nVBO(0)
	, m_nVEAB(0)
	, m_bDirty(false)
	, m_fLineWidth(2.0f)
	, m_colorDraw(Color4F::WHITE)
    , m_nColorIndex(0)
    , m_nPointIndex(0)
    , m_nTrimDistance(0)
{
	
}

CHms3DLineDrawNode::~CHms3DLineDrawNode()
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

bool CHms3DLineDrawNode::Init(int nPreAssignCapacity)
{
	if (!CHmsNode::Init())
	{
		return false;
	}

#if 1
    auto glProgram = HmsAviPf::GLProgram::createWithFilenames("Shaders/zs_3dPos.vsh", "Shaders/zs_3dPos.fsh");
    SetGLProgramState(GLProgramState::getOrCreateWithGLProgram(glProgram));
#else
	SetGLProgramState(GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_NAME_POSITION_U_COLOR));
#endif

	ensureBufferCapacity(nPreAssignCapacity);
	ensureIndicesCapacity(nPreAssignCapacity);

	Vec4 uColor(m_colorDraw.r, m_colorDraw.g, m_colorDraw.b, m_colorDraw.a);
    m_uColor = uColor;
	auto glProgramState = GetGLProgramState();
	if (glProgramState)
	{
	glProgramState->setVertexAttribPointer(GLProgram::ATTRIBUTE_NAME_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (GLvoid *)0);
//    glProgramState->setUniformVec4("u_color", m_uColor);
//    glProgramState->setUniformFloat("u_pointSize", m_fLineWidth);
	}
    m_nColorIndex = GetGLProgram()->getUniformLocation("u_color");
    m_nPointIndex = GetGLProgram()->getUniformLocation("u_pointSize");
    m_nTrimDistance = GetGLProgram()->getUniformLocation("u_trimDistance");

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


void CHms3DLineDrawNode::Draw(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	if (m_bufferCount && m_indicesCount)
	{
		m_customCommand.init(m_globalZOrder, transform, flags);
		m_customCommand.func = std::bind(&CHms3DLineDrawNode::OnDraw, this, transform, flags);
		renderer->addCommand(&m_customCommand);
	}
}

void CHms3DLineDrawNode::OnDraw(const Mat4 &transform, uint32_t flags)
{
	auto glProgramState = GetGLProgramState();
	if (!glProgramState)
	{
		return;
	}

	if (m_bDirty)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_nVBO);
		glProgramState->applyAttributes();
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3)*m_bufferCount, m_pBuffer, GL_DYNAMIC_DRAW);

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
    auto glProgram = GetGLProgramState()->getGLProgram();
    glProgram->setUniformLocationWith4fv(m_nColorIndex, (GLfloat*)&m_uColor, 1);
    glProgram->setUniformLocationWith1f(m_nPointIndex, m_fLineWidth);
    glProgram->setUniformLocationWith1f(m_nTrimDistance, g_fHmsTrimDistance);
//     glProgramState->setUniformVec4("u_color", m_uColor);
//     glProgramState->setUniformFloat("u_pointSize", m_fLineWidth);
// 	glProgramState->applyUniforms();

	GL::bindTexture2D(0);

	if (m_stateBlock)
	{
		m_stateBlock->bind();
	}
	glLineWidth(m_fLineWidth);
	glDrawElements(GL_LINES, (GLsizei)m_indicesCount, GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void CHms3DLineDrawNode::Visit(Renderer * renderer, const Mat4 & parentTransform, uint32_t parentFlags)
{
	CHmsNode::Visit(renderer, parentTransform, parentFlags);
}

void CHms3DLineDrawNode::SetLineWidth(float fLineWidth)
{
	m_fLineWidth = fLineWidth;
	UpdatePointSize();
}

void CHms3DLineDrawNode::SetDepthTest(bool bTest)
{
	if (m_stateBlock)
	{
		m_stateBlock->setDepthTest(true);
		m_stateBlock->setDepthWrite(true);
	}
}

void CHms3DLineDrawNode::clear()
{
	m_bufferCount = 0;
	m_indicesCount = 0;
	m_bDirty = true;
}

void CHms3DLineDrawNode::PrintUsedSize()
{
	HMSLOG("HmsLog: 3D Line, bufferCnt=%d, indicesCnt=%d", m_bufferCount, m_indicesCount);
}

void CHms3DLineDrawNode::ensureBufferCapacity(int count)
{
	HMSASSERT(count >= 0, "capacity must be >= 0");

	if (m_bufferCount + count > m_bufferCapacity)
	{
		m_bufferCapacity += MAX(m_bufferCapacity, count);
		m_pBuffer = (Vec3*)realloc(m_pBuffer, m_bufferCapacity*sizeof(Vec3));
	}
}

void CHms3DLineDrawNode::ensureIndicesCapacity(int count)
{
	HMSASSERT(count >= 0, "capacity must be >= 0");

	if (m_indicesCount + count > m_indicesCapacity)
	{
		m_indicesCapacity += MAX(m_indicesCapacity, count);
		m_pIndices = (GLuint*)realloc(m_pIndices, m_indicesCapacity*sizeof(GLuint));
	}
}

Vec3 * CHms3DLineDrawNode::GetCurrentVerticalPosBegin()
{
	return m_pBuffer + m_bufferCount;
}

GLuint * CHms3DLineDrawNode::GetCurrentIndicesPosBegin()
{
	return m_pIndices + m_indicesCount;
}

void HmsAviPf::CHms3DLineDrawNode::UpdatePointSize()
{
	//auto glProgramState = GetGLProgramState();
	//glProgramState->setUniformFloat("u_pointSize", m_fLineWidth);
}

void HmsAviPf::CHms3DLineDrawNode::UpdateUColor()
{
	Vec4 uColor(m_colorDraw.r, m_colorDraw.g, m_colorDraw.b, m_colorDraw.a);
	//auto glProgramState = GetGLProgramState();
	//glProgramState->setUniformVec4("u_color", uColor);
    m_uColor = uColor;
}

void CHms3DLineDrawNode::SetOpacity(GLubyte opacity)
{
	CHmsNode::SetOpacity(opacity);

	m_colorDraw.a = opacity / 255.0f;
	UpdateUColor();
}

void CHms3DLineDrawNode::SetColor(const Color3B& color)
{
	CHmsNode::SetColor(color);

	m_colorDraw.r = color.r / 255.0f;
	m_colorDraw.g = color.g / 255.0f;
	m_colorDraw.b = color.b / 255.0f;

	UpdateUColor();
}

void HmsAviPf::CHms3DLineDrawNode::SetColor(const Color4F & color)
{
	m_colorDraw = color;
	CHmsNode::SetColor(Color3B(color.r*255,color.g*255, color.b*255));

	UpdateUColor();
}

void CHms3DLineDrawNode::DrawLines(const std::vector<Vec3> & vPos)
{
	unsigned int nVertexCount = vPos.size();
	unsigned int nIndicesCount = (nVertexCount >> 1) * 2;

	ensureBufferCapacity(nVertexCount);
	ensureIndicesCapacity(nIndicesCount);

	Vec3 * pPoss = GetCurrentVerticalPosBegin();
	GLuint * pIndex = GetCurrentIndicesPosBegin();
	int nVertexBegin = m_bufferCount;

	unsigned int nPos = 0;
	for (auto & c:vPos)
	{
		pPoss[nPos++] = c;
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

void CHms3DLineDrawNode::DrawLineLoop(const std::vector<Vec3> & vPos)
{
	unsigned int nVertexCount = vPos.size();
	unsigned int nIndicesCount = nVertexCount * 2;

	if (m_pBuffer == nullptr || m_pIndices == nullptr)
	{
		HMSLOG("3D line buffer is null, size=%d,%d, cap=%d,%d", m_bufferCount, m_indicesCount, m_bufferCapacity, m_indicesCapacity);
		return;
	}

	if (nVertexCount < 2)
	{
		return;
	}

	ensureBufferCapacity(nVertexCount);
	ensureIndicesCapacity(nIndicesCount);

	Vec3 * pPoss = GetCurrentVerticalPosBegin();
	GLuint * pIndex = GetCurrentIndicesPosBegin();
	int nVertexBegin = m_bufferCount;
	
	unsigned int nPos = 0;
	for (auto & c : vPos)
	{
		pPoss[nPos++] = c;
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

void CHms3DLineDrawNode::DrawLineStrip(const std::vector<Vec3> & vPos)
{
	unsigned int nVertexCount = vPos.size();
	unsigned int nIndicesCount = (nVertexCount - 1) * 2;

	if (nVertexCount < 2)
	{
		return;
	}

	ensureBufferCapacity(nVertexCount);
	ensureIndicesCapacity(nIndicesCount);

	Vec3 * pPoss = GetCurrentVerticalPosBegin();
	GLuint * pIndex = GetCurrentIndicesPosBegin();
	int nVertexBegin = m_bufferCount;

	unsigned int nPos = 0;
	for (auto & c : vPos)
	{
		pPoss[nPos++] = c;
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