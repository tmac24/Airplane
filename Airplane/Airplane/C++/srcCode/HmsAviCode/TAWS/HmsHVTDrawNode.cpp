#include "HmsHVTDrawNode.h"
#include "render/HmsRenderer.h"
#include <iostream>
#include "render/HmsGLProgramState.h"
#include "render/HmsGLStateCache.h"
#include "render/HmsTextureCache.h"
#include "HmsAviDisplayUnit.h"

USING_NS_HMS;

HmsHVTDrawNode::HmsHVTDrawNode()
: m_nColorIndex(0)
, m_pTexture(nullptr)
{
	m_color = Color4F::WHITE;
}

HmsHVTDrawNode::~HmsHVTDrawNode()
{

}

bool HmsHVTDrawNode::Init()
{
	m_blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;

	SetGLProgramState(GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR));
	//SetGLProgramState(GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_NAME_POSITION_LENGTH_TEXTURE_COLOR));
	m_nColorIndex = GetGLProgram()->getUniformLocation("u_color");

	ensureCapacity(128);

	//if (Configuration::getInstance()->supportsShareableVAO())
	if (Renderer::UseGlVao())
	{
		glGenVertexArrays(1, &m_vao);
		GL::bindVAO(m_vao);
		glGenBuffers(1, &m_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(V2F_C4B_T2F)* m_bufferCapacity, m_pBuffer, GL_STREAM_DRAW);
		// vertex
		glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_POSITION);
		glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(V2F_C4B_T2F), (GLvoid *)offsetof(V2F_C4B_T2F, vertices));
		// color
		glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_COLOR);
		glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(V2F_C4B_T2F), (GLvoid *)offsetof(V2F_C4B_T2F, colors));
		// texcood
		glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_TEX_COORD);
		glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(V2F_C4B_T2F), (GLvoid *)offsetof(V2F_C4B_T2F, texCoords));

		GL::bindVAO(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}
	else
	{
		glGenBuffers(1, &m_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(V2F_C4B_T2F)* m_bufferCapacity, m_pBuffer, GL_STREAM_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	//CHECK_GL_ERROR_DEBUG();

	m_bDirty = true;

#if HMS_ENABLE_CACHE_TEXTURE_DATA
	// Need to listen the event only when not use batchnode, because it will use VBO
	auto listener = EventListenerCustom::create(EVENT_RENDERER_RECREATED, [this](EventCustom* event){
		/** listen the event that renderer was recreated on Android/WP8 */
		this->Init();
	});

	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
#endif

	return true;
}

void HmsHVTDrawNode::SetTexturePath(std::string str)
{
	m_pTexture = CHmsAviDisplayUnit::GetInstance()->GetTextureCache()->addImage(str);
	if (m_pTexture)
	{
		Rect rect = Rect::ZERO;
		rect.size = m_pTexture->getContentSize();

		Texture2D::TexParams texP = {
			GL_LINEAR,
			GL_LINEAR,
			GL_REPEAT,
			GL_REPEAT
		};
		m_pTexture->setTexParameters(texP);
	}
}

void HmsHVTDrawNode::Draw(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	if (m_bufferCount)
	{
		m_customCommand.init(m_globalZOrder, transform, flags);
		m_customCommand.func = std::bind(&HmsHVTDrawNode::onDraw, this, transform, flags);
		renderer->addCommand(&m_customCommand);
	}
}

void HmsHVTDrawNode::onDraw(const Mat4 &transform, uint32_t flags)
{
	auto glProgram = GetGLProgram();
	glProgram->use();
	glProgram->setUniformsForBuiltins(transform);
	glProgram->setUniformLocationWith4fv(m_nColorIndex, (GLfloat*)&m_color.r, 1);

	GL::blendFunc(m_blendFunc.src, m_blendFunc.dst);
	if (m_pTexture)
	{
		GL::bindTexture2D(m_pTexture->getName());
	}

	if (m_bDirty)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(V2F_C4B_T2F)*m_bufferCapacity, m_pBuffer, GL_STREAM_DRAW);

		m_bDirty = false;
	}

	if (Renderer::UseGlVao())
	{
		GL::bindVAO(m_vao);
	}
	else
	{
		GL::enableVertexAttribs(GL::VERTEX_ATTRIB_FLAG_POS_COLOR_TEX);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		// vertex
		glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(V2F_C4B_T2F), (GLvoid *)offsetof(V2F_C4B_T2F, vertices));
		// color
		glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(V2F_C4B_T2F), (GLvoid *)offsetof(V2F_C4B_T2F, colors));
		// texcood
		glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(V2F_C4B_T2F), (GLvoid *)offsetof(V2F_C4B_T2F, texCoords));
	}

	for (auto & fd : m_vFanDraw)
	{
		switch (fd.drawType)
		{
		case GL_LINES:
		case GL_LINE_LOOP:
		case GL_LINE_STRIP:
			glLineWidth(fd.fLineWidth);
		default:
			break;
		}
		glDrawArrays(fd.drawType, fd.nStartPos, fd.nDrawCnt);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	if (Renderer::UseGlVao())
	{
		GL::bindVAO(0);
	}
}
#if 0
void HmsHVTDrawNode::DrawLineStrip(const std::vector<Vec2> & vPos, float fLineWidth, const Color4B & color /* = Color4B::WHITE */)
{

}


void HmsHVTDrawNode::DrawLineStrip(Vec2 * verts, int nCount, float fLineWidth, float fStrokeWidth, const Color4B & color /*= Color4B::WHITE*/, const Color4B & colorStroke /*= Color4B::BLACK*/)
{
	unsigned int nVertexCount = 2 * nCount;

	ensureCapacity(nVertexCount * 2);

	V2F_C4B_T2F * pPoss = GetCurrentPosBegin();

	float fHalfWidth = fLineWidth * 0.5f;
	float fHalfStroke = fHalfWidth + fStrokeWidth;

	{
		V2F_C4B_T2F * pCurPos = pPoss + nVertexCount;
		V2F_C4B_T2F * pCurStroke = pPoss;

		Vec2 v0 = verts[0], v1 = verts[1];
		Vec2 n1, n2;
		n1 = (v1 - v0).getPerp().getNormalized();

		*pCurPos++ = { v0 + n1 * fHalfWidth, color, Tex2F(0.0, 0.0) };
		*pCurPos++ = { v0 - n1 * fHalfWidth, color, Tex2F(1.0, 0.0) };

		*pCurStroke++ = { v0 + (n1 * fHalfStroke + n1.getPerp() * fStrokeWidth), colorStroke, Tex2F(0.0, 0.0) };
		*pCurStroke++ = { v0 - (n1 * fHalfStroke + n1.getRPerp() * fStrokeWidth), colorStroke, Tex2F(0.0, 0.0) };

		int nLoopCnt = nCount - 1;
		for (int i = 1; i < nLoopCnt; i++)
		{
			v0 = v1;
			v1 = verts[i + 1];
			n2 = (v1 - v0).getPerp().getNormalized();

			Vec2 offset = (n1 + n2) * (1.0 / (n1.dot(n2) + 1.0));

			*pCurPos++ = { v0 + offset * fHalfWidth, color, Tex2F(0.0, 1.0) };
			*pCurPos++ = { v0 - offset * fHalfWidth, color, Tex2F(1.0, 1.0) };

			*pCurStroke++ = { v0 + offset * fHalfStroke, colorStroke, Tex2F(0.0, 0.0) };
			*pCurStroke++ = { v0 - offset * fHalfStroke, colorStroke, Tex2F(0.0, 0.0) };

			n1 = n2;
		}

		v1 = verts[nCount - 1];
		n2 = (v1 - v0).getPerp().getNormalized();

		*pCurPos++ = { v1 + n2 * fHalfWidth, color, Tex2F(0.0, 0.0) };
		*pCurPos++ = { v1 - n2 * fHalfWidth, color, Tex2F(0.0, 0.0) };

		*pCurStroke++ = { v1 + (n2 * fHalfStroke + n2.getRPerp() * fStrokeWidth), colorStroke, Tex2F(0.0, 0.0) };
		*pCurStroke++ = { v1 - (n2 * fHalfStroke + n2.getPerp() * fStrokeWidth), colorStroke, Tex2F(0.0, 0.0) };
	}

	SetDrawTriangleStrip(nVertexCount);
	SetDrawTriangleStrip(nVertexCount);
}
#endif

