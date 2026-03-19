#include "HmsRouteLineDrawNode.h"
#include "render/HmsGLProgram.h"
#include "render/HmsGLProgramState.h"
#include "render/HmsGLStateCache.h"
#include "render/HmsRenderer.h"
#include "render/HmsTextureCache.h"
#include "HmsAviDisplayUnit.h"

NS_HMS_BEGIN

//#define HMS_PI   3.14159265358979323846
//#define HMS_2PI  (HMS_PI*2)	 
//#define HMS_PI_2 (HMS_PI*0.5)
//#define HMS_PI_4 (HMS_PI*0.25)

inline float ToRadian(float fAngle){ return float(fAngle*HMS_PI / 180.0f); }
inline float ToAngle(float fRadian){ return float(fRadian * 180.0f / HMS_PI); }
inline float SinAngle(float fAngle){ return sin(ToRadian(fAngle)); }
inline float CosAngle(float fAngle){ return cos(ToRadian(fAngle)); }

CHmsRouteLineDrawNode::CHmsRouteLineDrawNode()
	: m_vao(0)
	, m_vbo(0)
	, m_bufferCapacity(0)
	, m_bufferCount(0)
	, m_pBuffer(nullptr)
	, m_bDirty(false)
	, m_pTexture(nullptr)
	, m_nColorIndex(0)
{
	m_blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;
	m_color = Color4F::WHITE;
}


CHmsRouteLineDrawNode::~CHmsRouteLineDrawNode()
{
	free(m_pBuffer);
	m_pBuffer = nullptr;

	glDeleteBuffers(1, &m_vbo);
	m_vbo = 0;

#ifdef HMS_RENDER_USE_VAO
	if (Renderer::UseGlVao())
	{
		GL::bindVAO(0);
		glDeleteVertexArrays(1, &m_vao);
		m_vao = 0;
	}
#endif
}

bool CHmsRouteLineDrawNode::Init()
{
	m_blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;

	SetGLProgramState(GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR));
#if _NAV_SVS_LOGIC_JUDGE
	if (!GetGLProgram())
		return false;
#endif
	m_nColorIndex = GetGLProgram()->getUniformLocation("u_color");

	//SetTexturePath("res/NavImage/navLine.png");

	ensureCapacity(128);
#ifdef HMS_RENDER_USE_VAO
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
#endif
	{
		glGenBuffers(1, &m_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(V2F_C4B_T2F)* m_bufferCapacity, m_pBuffer, GL_STREAM_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	//CHECK_GL_ERROR_DEBUG();

	m_bDirty = true;

#if CC_ENABLE_CACHE_TEXTURE_DATA
	// Need to listen the event only when not use batchnode, because it will use VBO
	auto listener = EventListenerCustom::create(EVENT_RENDERER_RECREATED, [this](EventCustom* event){
		/** listen the event that renderer was recreated on Android/WP8 */
		this->init();
	});

	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
#endif

	return true;
}

void CHmsRouteLineDrawNode::Draw(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!renderer)
		return;
#endif
	if (m_bufferCount)
	{
		m_customCommand.init(m_globalZOrder, transform, flags);
		m_customCommand.func = std::bind(&CHmsRouteLineDrawNode::onDraw, this, transform, flags);
		renderer->addCommand(&m_customCommand);
	}
}

void CHmsRouteLineDrawNode::ensureCapacity(int count)
{
	HMSASSERT(count >= 0, "capacity must be >= 0");

	if (m_bufferCount + count > m_bufferCapacity)
	{
		m_bufferCapacity += MAX(m_bufferCapacity, count);
		m_pBuffer = (V2F_C4B_T2F*)realloc(m_pBuffer, m_bufferCapacity*sizeof(V2F_C4B_T2F));
	}
}

void CHmsRouteLineDrawNode::onDraw(const Mat4 &transform, uint32_t flags)
{
 	auto glProgram = GetGLProgram();
#if _NAV_SVS_LOGIC_JUDGE
	if (!glProgram)
		return;
#endif
 	glProgram->use();
 	glProgram->setUniformsForBuiltins(transform);
	glProgram->setUniformLocationWith4fv(m_nColorIndex, (GLfloat*)&m_color.r, 1);

	GL::blendFunc(m_blendFunc.src, m_blendFunc.dst);
	if (m_pTexture)
	{
		GL::bindTexture2D(m_pTexture->getName());
		//GL::bindTexture2D(0);
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

	//glDisable(GL_MULTISAMPLE);

	// 	HMS_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1, m_bufferCount);
	// 	CHECK_GL_ERROR_DEBUG();
}

void CHmsRouteLineDrawNode::clear()
{
	m_bufferCount = 0;
	m_bDirty = true;
	m_vFanDraw.clear();
}

const BlendFunc& CHmsRouteLineDrawNode::getBlendFunc() const
{
	return m_blendFunc;
}

void CHmsRouteLineDrawNode::setBlendFunc(const BlendFunc &blendFunc)
{
	m_blendFunc = blendFunc;
}


void CHmsRouteLineDrawNode::drawLineAngle(float fStart, float fEnd, float fAngle, float fLineWidth /*= 2.0*/)
{
	float fRadian = ToRadian(fAngle);
	float fSinRadian = sin(fRadian);
	float fCosRadian = cos(fRadian);

	float fWidthRadian = ToRadian(fAngle + 270);
	float fPoxMoveX = fLineWidth / 2.0 * sin(fWidthRadian);
	float fPoxMoveY = fLineWidth / 2.0 * cos(fWidthRadian);
	Vec2 moveUp(fPoxMoveX, fPoxMoveY);
	Vec2 moveDown(-fPoxMoveX, -fPoxMoveY);

	Vec2 posStart(fStart*fSinRadian, fStart*fCosRadian);
	Vec2 posEnd(fEnd * fSinRadian, fEnd * fCosRadian);

	float fTexPos = fabsf(fEnd - fStart) / 16;

	V2F_C4B_T2F a = { posStart + moveUp, Color4B::WHITE, Tex2F(0.0, 0.0) };
	V2F_C4B_T2F b = { posStart + moveDown, Color4B::WHITE, Tex2F(1.0, 0.0) };
	V2F_C4B_T2F c = { posEnd + moveDown, Color4B::WHITE, Tex2F(0.0, fTexPos) };
	V2F_C4B_T2F d = { posEnd + moveUp, Color4B::WHITE, Tex2F(1.0, fTexPos) };


	unsigned int nVertexCount = 4;
	V2F_C4B_T2F * pPoss = m_pBuffer + m_bufferCount;
	ensureCapacity(nVertexCount);
#if _NAV_SVS_LOGIC_JUDGE
	if (!pPoss)
		return;
#endif
	int nPos = 0;
	pPoss[nPos++] = a;
	pPoss[nPos++] = b;
	pPoss[nPos++] = d;
	pPoss[nPos++] = c;

	HmsDrawCtrl fd;
	fd.SetTriangleStrip(m_bufferCount, nVertexCount);
	m_vFanDraw.push_back(HmsDrawCtrl(fd));

	m_bufferCount += nVertexCount;

}

void CHmsRouteLineDrawNode::drawCicleFan(float fAngleStart, float fAngleEnd, float fRadius, float fLineWidth, unsigned int nSegment)
{
	unsigned int nVertexCount = (nSegment + 1) * 2;

	float fRadiusInner = fRadius - fLineWidth / 2.0;
	float fRadiusOutter = fRadius + fLineWidth / 2.0;

	ensureCapacity(nVertexCount);

	float fRadiusStart = ToRadian(fAngleStart);
	float fRadiusEnd = ToRadian(fAngleEnd);
	float fRadianPerSegment = (fRadiusEnd - fRadiusStart) / nSegment;
	

	V2F_C4B_T2F * pPoss = m_pBuffer + m_bufferCount;
#if _NAV_SVS_LOGIC_JUDGE
	if (!pPoss)
		return;
#endif

	int nPos = 0;
	Vec2 tempPos;
	float fTempTexPos = 0;
	float fCurrentRadian = 0.0;
	float fTexInc = ((fRadiusEnd - fRadiusStart) * fRadius) / nSegment / 16;

	for (unsigned int i = 0; i <= nSegment; i++)
	{
		fTempTexPos = fTexInc * i;

		fCurrentRadian = fRadiusStart + fRadianPerSegment * i;
		tempPos.set(sin(fCurrentRadian), cos(fCurrentRadian));

		V2F_C4B_T2F a = { tempPos * fRadiusInner, Color4B::WHITE, Tex2F(0, fTempTexPos) };
		V2F_C4B_T2F b = { tempPos * fRadiusOutter, Color4B::WHITE, Tex2F(1.0, fTempTexPos) };
		pPoss[nPos++] = a;
		pPoss[nPos++] = b;
	}

	HmsDrawCtrl fd;
	fd.SetTriangleStrip(m_bufferCount, nVertexCount);
	m_vFanDraw.push_back(HmsDrawCtrl(fd));

	m_bufferCount += nVertexCount;
}

void CHmsRouteLineDrawNode::drawLine(float fLength, float fLineWidth /*= 2.0*/)
{
	Vec2 moveUp(0, fLineWidth/2.0);
	Vec2 moveDown(0, -fLineWidth/2.0);

	Vec2 posStart(0, 0);
	Vec2 posEnd(fLength, 0);

	float fTexPos = fabsf(fLength) / 16;

	V2F_C4B_T2F a = { posStart + moveUp, Color4B::WHITE, Tex2F(0.0, 0.0) };
	V2F_C4B_T2F b = { posStart + moveDown, Color4B::WHITE, Tex2F(1.0, 0.0) };
	V2F_C4B_T2F c = { posEnd + moveDown, Color4B::WHITE, Tex2F(0.0, fTexPos) };
	V2F_C4B_T2F d = { posEnd + moveUp, Color4B::WHITE, Tex2F(1.0, fTexPos) };


	unsigned int nVertexCount = 4;
	V2F_C4B_T2F * pPoss = m_pBuffer + m_bufferCount;
	ensureCapacity(nVertexCount);

#if _NAV_SVS_LOGIC_JUDGE
	if (!pPoss)
		return;
#endif

	int nPos = 0;
	pPoss[nPos++] = a;
	pPoss[nPos++] = b;
	pPoss[nPos++] = d;
	pPoss[nPos++] = c;

	HmsDrawCtrl fd;
	fd.SetTriangleStrip(m_bufferCount, nVertexCount);
	m_vFanDraw.push_back(HmsDrawCtrl(fd));

	m_bufferCount += nVertexCount;
}

void CHmsRouteLineDrawNode::SetColor(const Color4F & color)
{
	m_color = color;
}

void CHmsRouteLineDrawNode::DrawLines(const std::vector<RouteLineDrawStu> &vecStu)
{
#if 0
	int nCnt = 0;
	for (const auto &stu : vecStu)
	{
		for (const auto &vec : stu.vecArrayVertexs)
		{
			if (vec.size() >= 2)
			{
				nCnt += (vec.size() - 1) * 6;
			}
		}
	}
	unsigned int nVertexCount = (nCnt - 1) * 6;
	ensureCapacity(nVertexCount);
	V2F_C4B_T2F * pPoss = m_pBuffer + m_bufferCount;
	int nLines = 0;
	V2F_C4B_T2F * pCurPos = pPoss;
	for (const auto &stu : vecStu)
	{
		float fHalfWidth = stu.fLineWidth * 0.5f;

		for (const auto &vec : stu.vecArrayVertexs)
		{
			if (vec.size() < 2)
			{
				continue;
			}
			Vec2 lastP1;
			Vec2 lastP2;
			for (int i = 1; i < vec.size(); ++i)
			{
				Vec2 v0 = vec[i - 1];
				Vec2 v1 = vec[i];
				if (v0 == v1)
				{
					continue;
				}
				++nLines;

				Vec2 n1 = (v1 - v0).getPerp().getNormalized();
				Vec2 p3 = v1 + n1 * fHalfWidth;
				Vec2 p4 = v1 - n1 * fHalfWidth;
				if (1 == i)
				{
					Vec2 p1 = v0 + n1 * fHalfWidth;
					Vec2 p2 = v0 - n1 * fHalfWidth;
					lastP1 = p1;
					lastP2 = p2;
				}
				*pCurPos++ = { lastP1, stu.color, Tex2F(0.0, 1.0) };
				*pCurPos++ = { lastP2, stu.color, Tex2F(0.0, 0.0) };
				*pCurPos++ = { p4, stu.color, Tex2F(1.0, 0.0) };

				*pCurPos++ = { lastP1, stu.color, Tex2F(0.0, 1.0) };
				*pCurPos++ = { p4, stu.color, Tex2F(1.0, 0.0) };
				*pCurPos++ = { p3, stu.color, Tex2F(1.0, 1.0) };

				lastP1 = p3;
				lastP2 = p4;
			}
		}
	}
	nVertexCount = (nLines * 6);

	HmsDrawCtrl fd;
	fd.SetTriangles(m_bufferCount, nVertexCount);
	m_vFanDraw.push_back(HmsDrawCtrl(fd));
	m_bufferCount += nVertexCount;

#else

	int nCnt = 0;
	for (const auto &stu : vecStu)
	{
		for (const auto &vec : stu.vecArrayVertexs)
		{
			if (vec.size() >= 2)
			{
				nCnt += (vec.size() - 1) * 6;
			}
		}
	}
    int nVertexCount = (nCnt - 1) * 6;
    if (nVertexCount <= 0)
    {
        return;
    }
	ensureCapacity(nVertexCount);
	V2F_C4B_T2F * pPoss = m_pBuffer + m_bufferCount;
#if _NAV_SVS_LOGIC_JUDGE
	if (!pPoss)
		return;
#endif
	int nLines = 0;
	V2F_C4B_T2F * pCurPos = pPoss;
	for (const auto &stu : vecStu)
	{
		float fHalfWidth = stu.fLineWidth * 0.5f;

		for (const auto &vec : stu.vecArrayVertexs)
		{
			if (vec.size() < 2)
			{
				continue;
			}
			for (int i = 1; i < (int)vec.size(); ++i)
			{
				Vec2 v0 = vec[i - 1];
				Vec2 v1 = vec[i];
				if (v0 == v1)
				{
					continue;
				}
				++nLines;

				Vec2 n1 = (v1 - v0).getPerp().getNormalized();
				*pCurPos++ = { v0 + n1 * fHalfWidth, stu.color, Tex2F(0.0, 1.0) };
				*pCurPos++ = { v0 - n1 * fHalfWidth, stu.color, Tex2F(0.0, 0.0) };
				*pCurPos++ = { v1 - n1 * fHalfWidth, stu.color, Tex2F(1.0, 0.0) };

				*pCurPos++ = { v0 + n1 * fHalfWidth, stu.color, Tex2F(0.0, 1.0) };
				*pCurPos++ = { v1 - n1 * fHalfWidth, stu.color, Tex2F(1.0, 0.0) };
				*pCurPos++ = { v1 + n1 * fHalfWidth, stu.color, Tex2F(1.0, 1.0) };
			}
		}
	}
	nVertexCount = (nLines * 6);

	HmsDrawCtrl fd;
	fd.SetTriangles(m_bufferCount, nVertexCount);
	m_vFanDraw.push_back(HmsDrawCtrl(fd));
	m_bufferCount += nVertexCount;
#endif
}

void CHmsRouteLineDrawNode::DrawLines(Vec2 * vertexs, int nCnt, float fLineWidth, const Color4B & color /*= Color4B::WHITE*/)
{
#if 1
	if (nCnt < 2)
	{
		return;
	}
	unsigned int nVertexCount = (nCnt - 1) * 6;
	ensureCapacity(nVertexCount);
	V2F_C4B_T2F * pPoss = m_pBuffer + m_bufferCount;
#if _NAV_SVS_LOGIC_JUDGE
	if (!pPoss)
		return;
#endif
	float fHalfWidth = fLineWidth * 0.5f;
	int nLines = 0;
	{
		V2F_C4B_T2F * pCurPos = pPoss;
		for (int i = 1; i < nCnt; i++)
		{
			Vec2 v0 = vertexs[i - 1];
			Vec2 v1 = vertexs[i];
			if (v0 == v1)
			{
				continue;
			}
			++nLines;

			Vec2 n1 = (v1 - v0).getPerp().getNormalized();
			*pCurPos++ = { v0 + n1 * fHalfWidth, color, Tex2F(0.0, 1.0) };
			*pCurPos++ = { v0 - n1 * fHalfWidth, color, Tex2F(0.0, 0.0) };
			*pCurPos++ = { v1 - n1 * fHalfWidth, color, Tex2F(1.0, 0.0) };

			*pCurPos++ = { v0 + n1 * fHalfWidth, color, Tex2F(0.0, 1.0) };
			*pCurPos++ = { v1 - n1 * fHalfWidth, color, Tex2F(1.0, 0.0) };
			*pCurPos++ = { v1 + n1 * fHalfWidth, color, Tex2F(1.0, 1.0) };
		}
	}
	nVertexCount = (nLines * 6);

	HmsDrawCtrl fd;
	fd.SetTriangles(m_bufferCount, nVertexCount);
	m_vFanDraw.push_back(HmsDrawCtrl(fd));
	m_bufferCount += nVertexCount;
#endif
#if 0
	if (nCnt < 2)
	{
		return;
	}
	unsigned int nVertexCount = 2 * nCnt;
	ensureCapacity(nVertexCount);
	V2F_C4B_T2F * pPoss = m_pBuffer + m_bufferCount;
	float fHalfWidth = fLineWidth * 0.5f;
	{
		V2F_C4B_T2F * pCurPos = pPoss;
		//0
		{
			Vec2 v0 = vertexs[0];
			Vec2 v1 = vertexs[1];
			Vec2 n1 = (v1 - v0).getPerp().getNormalized();
			*pCurPos++ = { v0 - n1 * fHalfWidth, color, Tex2F(0.0, 1.0) };
			*pCurPos++ = { v0 + n1 * fHalfWidth, color, Tex2F(0.0, 0.0) };
		}
		//>0
		{
			for (int i = 1; i < nCnt; i++)
			{
				Vec2 v0 = vertexs[i-1];
				Vec2 v1 = vertexs[i];
				Vec2 n1 = (v1 - v0).getPerp().getNormalized();
				*pCurPos++ = { v1 - n1 * fHalfWidth, color, Tex2F(1.0, 1.0) };
				*pCurPos++ = { v1 + n1 * fHalfWidth, color, Tex2F(1.0, 0.0) };
			}
		}
	}
	HmsDrawCtrl fd;
	fd.SetTriangleStrip(m_bufferCount, nVertexCount);
	m_vFanDraw.push_back(HmsDrawCtrl(fd));
	m_bufferCount += nVertexCount;
#endif

#if 0
	unsigned int nVertexCount = 2 * nCnt;

	//Color4B color(255, 255, 255, 255);

	ensureCapacity(nVertexCount);

	V2F_C4B_T2F * pPoss = m_pBuffer + m_bufferCount;

	float fHalfWidth = fLineWidth * 0.5f;

	int nLines = nVertexCount / 4;

	{
		V2F_C4B_T2F * pCurPos = pPoss;

		for (int i = 0; i < nLines; i++)
		{
			Vec2 v0 = vertexs[i * 2];
			Vec2 v1 = vertexs[i * 2 + 1];

			Vec2 n1 = (v1 - v0).getPerp().getNormalized();

			
			*pCurPos++ = { v0 - n1 * fHalfWidth, color, Tex2F(0.0, 1.0) };
			*pCurPos++ = { v0 + n1 * fHalfWidth, color, Tex2F(0.0, 0.0) };
			*pCurPos++ = { v1 - n1 * fHalfWidth, color, Tex2F(1.0, 1.0) };
			*pCurPos++ = { v1 + n1 * fHalfWidth, color, Tex2F(1.0, 0.0) };
		}
	}

	nVertexCount = (nLines * 4);

	HmsDrawCtrl fd;
	fd.SetTriangleStrip(m_bufferCount, nVertexCount);
	m_vFanDraw.push_back(HmsDrawCtrl(fd));
	m_bufferCount += nVertexCount;
#endif
}

void CHmsRouteLineDrawNode::DrawDashLines(Vec2 * vertexs, int nCnt, float fLineWidth, const Color4B & color /*= Color4B::WHITE*/)
{
	if (nCnt < 2)
	{
		return;
	}
	unsigned int nVertexCount = (nCnt - 1) * 6;
	ensureCapacity(nVertexCount);
	V2F_C4B_T2F * pPoss = m_pBuffer + m_bufferCount;
#if _NAV_SVS_LOGIC_JUDGE
	if (!pPoss)
		return;
#endif
	float fHalfWidth = fLineWidth * 0.5f;
	int nLines = 0;
	{
		float textureX = 0;

		V2F_C4B_T2F * pCurPos = pPoss;
		for (int i = 1; i < nCnt; i++)
		{
			Vec2 v0 = vertexs[i - 1];
			Vec2 v1 = vertexs[i];
			if (v0 == v1)
			{
				continue;
			}
			++nLines;

			float xStart = textureX;
			float xEnd = 0;
			textureX += (v0 - v1).length() / 32;
			xEnd = textureX;

			Vec2 n1 = (v1 - v0).getPerp().getNormalized();
			*pCurPos++ = { v0 + n1 * fHalfWidth, color, Tex2F(xStart, 1.0) };
			*pCurPos++ = { v0 - n1 * fHalfWidth, color, Tex2F(xStart, 0.0) };
			*pCurPos++ = { v1 - n1 * fHalfWidth, color, Tex2F(xEnd, 0.0) };

			*pCurPos++ = { v0 + n1 * fHalfWidth, color, Tex2F(xStart, 1.0) };
			*pCurPos++ = { v1 - n1 * fHalfWidth, color, Tex2F(xEnd, 0.0) };
			*pCurPos++ = { v1 + n1 * fHalfWidth, color, Tex2F(xEnd, 1.0) };
		}
	}
	nVertexCount = (nLines * 6);

	HmsDrawCtrl fd;
	fd.SetTriangles(m_bufferCount, nVertexCount);
	m_vFanDraw.push_back(HmsDrawCtrl(fd));
	m_bufferCount += nVertexCount;
}

void CHmsRouteLineDrawNode::DrawLinesPrimitive(Vec2 * vertexs, int nCnt, float fLineWidth, const Color4B & color /*= Color4B::WHITE*/)
{
	if (nCnt < 2)
	{
		return;
	}
	unsigned int nVertexCount = 2 * nCnt;
	ensureCapacity(nVertexCount);
	V2F_C4B_T2F * pPoss = m_pBuffer + m_bufferCount;
#if _NAV_SVS_LOGIC_JUDGE
	if (!pPoss)
		return;
#endif

	for (int i = 1; i < nCnt; i++)
	{
		*pPoss++ = { vertexs[i - 1], color, Tex2F(1.0, 1.0) };
		*pPoss++ = { vertexs[i], color, Tex2F(1.0, 0.0) };
	}

	HmsDrawCtrl fd;
	fd.SetLines(m_bufferCount, nVertexCount, fLineWidth);
	m_vFanDraw.push_back(HmsDrawCtrl(fd));
	m_bufferCount += nVertexCount;
}



void CHmsRouteLineDrawNode::DrawRing(const Vec2& center, float fRadius, float fLineWidth /*= 2.0*/, unsigned int nSegment /*= 50*/, const Color4F & color /*= Color4F::WHITE*/)
{
	unsigned int nVertexCount = (nSegment + 1) * 2;

	float fRadianPerSegment = HMS_2PI / nSegment;
	float fSinRadian = 0.0, fCosRadian = 0.0;
	float fCurrentRadian = 0.0;
	float fRadiusInner = fRadius - fLineWidth / 2.0;
	float fRadiusOutter = fRadius + fLineWidth / 2.0;

	ensureCapacity(nVertexCount);

	V2F_C4B_T2F * pPoss = m_pBuffer + m_bufferCount;
#if _NAV_SVS_LOGIC_JUDGE
	if (!pPoss)
		return;
#endif
	int nPos = 0;

	for (unsigned int i = 0; i <= nSegment; i++)
	{
		fCurrentRadian = fRadianPerSegment * i;
		fSinRadian = sin(fCurrentRadian);
		fCosRadian = cos(fCurrentRadian);
		V2F_C4B_T2F a = { Vec2(fSinRadian * fRadiusInner, fCosRadian * fRadiusInner) + center, Color4B(color), Tex2F(0.0, 0.0) };
		V2F_C4B_T2F b = { Vec2(fSinRadian * fRadiusOutter, fCosRadian * fRadiusOutter) + center, Color4B(color), Tex2F(0.0, 1.0) };

		pPoss[nPos++] = a;
		pPoss[nPos++] = b;
	}

	HmsDrawCtrl fd;
	fd.SetTriangleStrip(m_bufferCount, nVertexCount);
	m_vFanDraw.push_back(HmsDrawCtrl(fd));

	m_bufferCount += nVertexCount;
}

void CHmsRouteLineDrawNode::DrawRing(float fRadius, float fLineWidth /*= 2.0*/, unsigned int nSegment /*= 50*/, const Color4F & color /*= Color4F::WHITE*/)
{
	unsigned int nVertexCount = (nSegment + 1) * 2;

	float fRadianPerSegment = HMS_2PI / nSegment;
	float fSinRadian = 0.0, fCosRadian = 0.0;
	float fCurrentRadian = 0.0;
	float fRadiusInner = fRadius - fLineWidth / 2.0;
	float fRadiusOutter = fRadius + fLineWidth / 2.0;

	ensureCapacity(nVertexCount);

	V2F_C4B_T2F * pPoss = m_pBuffer + m_bufferCount;
#if _NAV_SVS_LOGIC_JUDGE
	if (!pPoss)
		return;
#endif
	int nPos = 0;

	for (unsigned int i = 0; i <= nSegment; i++)
	{
		fCurrentRadian = fRadianPerSegment * i;
		fSinRadian = sin(fCurrentRadian);
		fCosRadian = cos(fCurrentRadian);
		V2F_C4B_T2F a = { Vec2(fSinRadian * fRadiusInner, fCosRadian * fRadiusInner), Color4B(color), Tex2F(0.0, 0.0) };
		V2F_C4B_T2F b = { Vec2(fSinRadian * fRadiusOutter, fCosRadian * fRadiusOutter), Color4B(color), Tex2F(0.0, 1.0) };

		pPoss[nPos++] = a;
		pPoss[nPos++] = b;
	}

	HmsDrawCtrl fd;
	fd.SetTriangleStrip(m_bufferCount, nVertexCount);
	m_vFanDraw.push_back(HmsDrawCtrl(fd));

	m_bufferCount += nVertexCount;
}

void CHmsRouteLineDrawNode::SetTexturePath(std::string str)
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


NS_HMS_END