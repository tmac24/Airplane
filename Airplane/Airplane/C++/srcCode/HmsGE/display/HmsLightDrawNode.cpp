#include "HmsLightDrawNode.h"
#include "render/HmsGLProgram.h"
#include "render/HmsGLProgramState.h"
#include "render/HmsGLStateCache.h"
#include "render/HmsTextureCache.h"
#include "render/HmsTexture2D.h"
#include "render/HmsRenderer.h"
#include "HmsAviDisplayUnit.h"
#include "base/HmsCamera.h"

NS_HMS_BEGIN

CHmsLightDrawNode::CHmsLightDrawNode()
	:m_vao(0)
	, m_vbo(0)
	, m_bufferCapacity(0)
	, m_bufferCount(0)
	, m_pBuffer(nullptr)
	, m_bDirty(false)
	, m_pTexture(nullptr)
	, m_nColorIndex(0)
	, m_nTexHigh(8)
	, m_nTexWide(16)
	, m_fOneHigh(0.125f)
	, m_fOneWide(0.0625f)
{
	m_blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;
}


CHmsLightDrawNode::~CHmsLightDrawNode()
{
	free(m_pBuffer);
	m_pBuffer = nullptr;

	glDeleteBuffers(1, &m_vbo);
	m_vbo = 0;

	if (Renderer::UseGlVao())
	{
		GL::bindVAO(0);
		glDeleteVertexArrays(1, &m_vao);
		m_vao = 0;
	}
}

bool CHmsLightDrawNode::Init()
{
	m_blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;

	SetGLProgramState(GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_NAME_POSITION_TEXTURE_U_COLOR));
	m_nColorIndex = GetGLProgram()->getUniformLocation("u_color");

	m_pTexture = CHmsAviDisplayUnit::GetInstance()->GetTextureCache()->addImage("res/1to6horLine.png");
	if (m_pTexture)
	{
		Rect rect = Rect::ZERO;
		rect.size = m_pTexture->getContentSize();

		Texture2D::TexParams texP = {
			GL_LINEAR,/*GL_NEAREST*/
			GL_LINEAR,
			GL_REPEAT,
			GL_REPEAT
		};

		m_pTexture->setTexParameters(texP);
		m_nTexWide = m_pTexture->getPixelsWide();
		m_nTexHigh = m_pTexture->getPixelsHigh();
		m_fOneHigh = 1.0f / float(m_nTexHigh);
		m_fOneWide = 1.0f / float(m_nTexWide);
	}

	ensureCapacity(128);

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

void CHmsLightDrawNode::Draw(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	if (m_bufferCount)
	{
		m_customCommand.init(m_globalZOrder, transform, flags);
		m_customCommand.func = std::bind(&CHmsLightDrawNode::onDraw, this, transform, flags);
		renderer->addCommand(&m_customCommand);
	}
}

void CHmsLightDrawNode::ensureCapacity(int count)
{
	HMSASSERT(count >= 0, "capacity must be >= 0");

	if (m_bufferCount + count > m_bufferCapacity)
	{
		m_bufferCapacity += MAX(m_bufferCapacity, count);
		m_pBuffer = (V2F_C4B_T2F*)realloc(m_pBuffer, m_bufferCapacity*sizeof(V2F_C4B_T2F));
	}
}

void CHmsLightDrawNode::onDraw(const Mat4 &transform, uint32_t flags)
{
	auto glProgram = GetGLProgram();
	glProgram->use();
	glProgram->setUniformsForBuiltins(transform);
	//glProgram->setUniformLocationWith4fv(m_nColorIndex, (GLfloat*)&m_color.r, 1);//only one color.

	glEnable(GL_BLEND);
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

	onDrawShape();
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	if (Renderer::UseGlVao())
	{
		GL::bindVAO(0);
	}

	//glDisable(GL_MULTISAMPLE);
	//glDisable(0x809D);
	GL::bindTexture2D(0u);
	glDisable(GL_BLEND);

 	//CC_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1, m_bufferCount);
 	//CHECK_GL_ERROR_DEBUG();
}

void CHmsLightDrawNode::onDrawShape()
{
	auto glProgram = GetGLProgram();
	for (auto & fd : m_vFanDraw)
	{
		glProgram->setUniformLocationWith4fv(m_nColorIndex, (GLfloat*)&fd.fColor.r, 1);
		//glDrawArrays(GL_TRIANGLE_FAN, fd.nStartPos, fd.nDrawCnt);
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
}

void CHmsLightDrawNode::clear()
{
	m_bufferCount = 0;
	m_bDirty = true;
	m_vFanDraw.clear();
}

const BlendFunc& CHmsLightDrawNode::getBlendFunc() const
{
	return m_blendFunc;
}

void CHmsLightDrawNode::setBlendFunc(const BlendFunc &blendFunc)
{
	m_blendFunc = blendFunc;
}

void CHmsLightDrawNode::DrawFan(const Vec2& center, float fAngleBegin, float fAngleEnd, float fRadius, unsigned int nSegments, const Color4F & color)
{
	unsigned int nVertexCount = nSegments + 2;

	float fRadianPerSegment = HMS_2PI / nSegments;
	float fSinRadian = 0.0, fCosRadian = 0.0;
	float fCurrentRadian = 0.0;
	float fSegmentInc = (fAngleEnd - fAngleBegin) / nSegments;
	float fTexX = 7.0f * m_fOneWide;//宽度为5中心U.
	float fTexY = 3.5f * m_fOneHigh;//宽度为5中心V.

	ensureCapacity(nVertexCount);

	V2F_C4B_T2F * pPoss = m_pBuffer + m_bufferCount;
	V2F_C4B_T2F centerFan = { center, Color4B(color), Tex2F(fTexX, fTexY) };
	int nPos = 0;
	pPoss[nPos++] = centerFan;

	std::vector<Vec2> borderLineLeft, borderLineRight;
	borderLineLeft.push_back(center);
	borderLineRight.push_back(center);

	for (unsigned int i = 0; i <= nSegments; i++)
	{
		fCurrentRadian = ANGLE_TO_RADIAN(fAngleBegin + fSegmentInc * i);
		fSinRadian = sin(fCurrentRadian);
		fCosRadian = cos(fCurrentRadian);
		if (i == 0){
			borderLineLeft.push_back(Vec2(fSinRadian * fRadius, fCosRadian * fRadius) + center);
		}
		else if (i == nSegments){
			borderLineRight.push_back(Vec2(fSinRadian * fRadius, fCosRadian * fRadius) + center);
		}

		V2F_C4B_T2F a = { Vec2(fSinRadian * fRadius, fCosRadian * fRadius) + center, Color4B(color), Tex2F(fSinRadian*m_fOneWide + fTexX, fCosRadian*m_fOneHigh + fTexY) };
		pPoss[nPos++] = a;
	}

	HmsDrawCtrl fd;
	fd.SetColor(color);
	fd.SetTriangleFan(m_bufferCount, nVertexCount);
	m_vFanDraw.push_back(HmsDrawCtrl(fd));

	m_bufferCount += nVertexCount;

	if (fabsf(fAngleEnd - fAngleBegin) < 360.0)
	{
		DrawLineStrip(borderLineLeft, 2.0f, COLOR4F_4B(color));
		DrawLineStrip(borderLineRight, 2.0f, COLOR4F_4B(color));
	}
	
	DrawFanBorder(center, fAngleBegin, fAngleEnd, fRadius, 2.0f, nSegments, color);
}

void CHmsLightDrawNode::DrawFan(float fAngleBegin, float fAngleEnd, float fRadius, unsigned int nSegments, const Color4F & color)
{
	DrawFan(Vec2(0.0, 0.0), fAngleBegin, fAngleEnd, fRadius, nSegments, color);
}

void CHmsLightDrawNode::DrawFanBorder(const Vec2& center, float fAngleBegin, float fAngleEnd, float fRadius, float fBorder /*= 2.0f*/, unsigned int nSegments /*= 50*/, const Color4F & color /*= Color4F::WHITE*/)
{
	unsigned int nVertexCount = (nSegments + 1) * 2;

	float fSinRadian = 0.0, fCosRadian = 0.0;
	float fCurrentRadian = 0.0;
	float fSegmentInc = (fAngleEnd - fAngleBegin) / nSegments;

	ensureCapacity(nVertexCount);

	Tex2F top, bottom;
	float fHalfUsedWidth = GetTextureCoord(fBorder, top, bottom);

	V2F_C4B_T2F * pPoss = m_pBuffer + m_bufferCount;
	int nPos = 0;
	float fRadiusOutter = fRadius + fHalfUsedWidth;
	float fRadiusInner = fRadius - fHalfUsedWidth;

	for (unsigned int i = 0; i <= nSegments; i++)
	{
		fCurrentRadian = ANGLE_TO_RADIAN(fAngleBegin + fSegmentInc * i);
		fSinRadian = sin(fCurrentRadian);
		fCosRadian = cos(fCurrentRadian);

		pPoss[nPos++] = { Vec2(fSinRadian * fRadiusOutter, fCosRadian * fRadiusOutter) + center, Color4B(color), top };
		pPoss[nPos++] = { Vec2(fSinRadian * fRadiusInner, fCosRadian * fRadiusInner) + center, Color4B(color), bottom };
	}

	HmsDrawCtrl fd;
	fd.SetColor(color);
	fd.SetTriangleStrip(m_bufferCount, nVertexCount);
	m_vFanDraw.push_back(HmsDrawCtrl(fd));

	m_bufferCount += nVertexCount;
}

void CHmsLightDrawNode::DrawFanBorder(float fAngleBegin, float fAngleEnd, float fRadius, float fBorder, unsigned int nSegments /*= 50*/, const Color4F & color /*= Color4F::WHITE*/)
{
	DrawFanBorder(Vec2(0.0, 0.0), fAngleBegin, fAngleEnd, fRadius, fBorder, nSegments, color);
}

void CHmsLightDrawNode::DrawRing(float fRadius, float fLineWidth /*= 2.0*/, unsigned int nSegment /*= 50*/, const Color4F & color /*= Color4F::WHITE*/)
{
	DrawRing(Vec2(0.0, 0.0), fRadius, fLineWidth, nSegment, color);
}

void CHmsLightDrawNode::DrawRing(const Vec2& center, float fRadius, float fLineWidth /*= 2.0*/, unsigned int nSegment /*= 50*/, const Color4F & color /*= Color4F::WHITE*/)
{
	DrawFanBorder(center, 0.0, 360.0, fRadius, fLineWidth, nSegment, color);
}

void CHmsLightDrawNode::DrawDisk(float fRadius, unsigned int nSegment /*= 50*/, const Color4F & color /*= Color4F::WHITE*/)
{
	DrawFan(Vec2(0.0, 0.0), 0.0, 360.0, fRadius, nSegment, color);
}

void CHmsLightDrawNode::DrawDisk(const Vec2& center, float fRadius, unsigned int nSegment /*= 50*/, const Color4F & color /*= Color4F::WHITE*/)
{
	DrawFan(center, 0.0, 360.0, fRadius, nSegment, color);
}

void CHmsLightDrawNode::DrawDashLine(const Vec2 & posStart, float fAngle, float fLineLength, float fDotLength, float fBankLength, float fLineWidth /*= 2.0*/, const Color4F & color /*= Color4F::WHITE*/)
{
	unsigned int nVertexCount = 4 *  (unsigned int)ceil(fLineLength / (fDotLength + fBankLength));
	ensureCapacity(nVertexCount);

	Tex2F top, bottom;
	float fHalfUsedWidth = GetTextureCoord(fLineWidth, top, bottom);

	float fRadian = ANGLE_TO_RADIAN(fAngle);
	float fSinRadian = sin(fRadian);
	float fCosRadian = cos(fRadian);

	float fWidthRadian = ANGLE_TO_RADIAN(fAngle + 270);
	float fPoxMoveX = fHalfUsedWidth * sin(fWidthRadian);
	float fPoxMoveY = fHalfUsedWidth * cos(fWidthRadian);
	Vec2 moveUp(fPoxMoveX, fPoxMoveY);
	Vec2 moveDown(-fPoxMoveX, -fPoxMoveY);

	V2F_C4B_T2F * pPoss = m_pBuffer + m_bufferCount;
	
	int nPos = 0;
	float fCurrentLength = 0;
	float fRemainLength = fLineLength;
	float fTempDrawLength = fDotLength;
	float fInc = fDotLength + fBankLength;

	while (fRemainLength > 0.0f)
	{
		//获取画线长度
		if (fRemainLength < fLineLength)
		{
			fTempDrawLength = fDotLength;
		}

		Vec2 posBegin(fSinRadian * fCurrentLength, fCosRadian * fCurrentLength);
		posBegin += posStart;

		V2F_C4B_T2F a = { posBegin + moveUp, Color4B(color), top };
		V2F_C4B_T2F b = { posBegin + moveDown, Color4B(color), bottom };

		Vec2 posEnd(fSinRadian * (fCurrentLength + fTempDrawLength), fCosRadian * (fCurrentLength + fTempDrawLength));
		posEnd += posStart;

		V2F_C4B_T2F c = { posEnd + moveUp, Color4B(color), top };
		V2F_C4B_T2F d = { posEnd + moveDown, Color4B(color), bottom };
		pPoss[nPos++] = a;
		pPoss[nPos++] = b;
		pPoss[nPos++] = c;
		pPoss[nPos++] = d;

		//自增
		fRemainLength -= fInc;
		fCurrentLength += fInc;

		SetDrawTriangleStrip(4, color);
	}
}

void CHmsLightDrawNode::DrawDashCicle(const Vec2& center, float fRadius, float fDashRate /*= 0.5*/, float fLineWidth /*= 2.0*/, unsigned int nSegment /*= 50*/, const Color4F & color /*= Color4F::WHITE*/)
{
	unsigned int nVertexCount = (nSegment + 1) * 2 * 2;
	ensureCapacity(nVertexCount);

	Tex2F top, bottom;
	float fHalfUsedWidth = GetTextureCoord(fLineWidth, top, bottom);
	float fRadianPerSegment = HMS_2PI / nSegment;
	float fSinRadian = 0.0, fCosRadian = 0.0;
	float fCurrentRadian = 0.0;
	float fRadiusInner  = fRadius - fHalfUsedWidth;
	float fRadiusOutter = fRadius + fHalfUsedWidth;

	
	V2F_C4B_T2F * pPoss = m_pBuffer + m_bufferCount;
	int nPos = 0;

	for (unsigned int i = 0; i <= nSegment; i++)
	{
		fCurrentRadian = fRadianPerSegment * i;
		fSinRadian = sin(fCurrentRadian);
		fCosRadian = cos(fCurrentRadian);
		V2F_C4B_T2F a = { Vec2(fSinRadian * fRadiusOutter, fCosRadian * fRadiusOutter) + center, Color4B(color), top };
		V2F_C4B_T2F b = { Vec2(fSinRadian * fRadiusInner, fCosRadian * fRadiusInner) + center, Color4B(color), bottom };

		pPoss[nPos++] = a;
		pPoss[nPos++] = b;

		fCurrentRadian = fCurrentRadian + fRadianPerSegment * fDashRate;
		fSinRadian = sin(fCurrentRadian);
		fCosRadian = cos(fCurrentRadian);
		V2F_C4B_T2F c = { Vec2(fSinRadian * fRadiusOutter, fCosRadian * fRadiusOutter) + center, Color4B(color), top };
		V2F_C4B_T2F d = { Vec2(fSinRadian * fRadiusInner, fCosRadian * fRadiusInner) + center, Color4B(color), bottom };

		pPoss[nPos++] = c; 
		pPoss[nPos++] = d;

		SetDrawTriangleStrip(4, color);
	}
}

void CHmsLightDrawNode::DrawDashCicle(float fRadius, float fDashRate /*= 0.5*/, float fLineWidth /*= 2.0*/, unsigned int nSegment /*= 50*/, const Color4F & color /*= Color4F::WHITE*/)
{
	DrawDashCicle(Vec2(0.0, 0.0), fRadius, fDashRate, fLineWidth, nSegment, color);
}

void CHmsLightDrawNode::DrawOutterRect(float x1, float y1, float x2, float y2, float fLineWidth /*= 2.0*/, const Color4F & color /*= Color4F::WHITE*/)
{
	unsigned int nVertexCount = 5 * 2;
	ensureCapacity(nVertexCount);

	Tex2F top, bottom;
	GetTextureCoord(fLineWidth, top, bottom);

	V2F_C4B_T2F * pPoss = m_pBuffer + m_bufferCount;
	int nPos = 0;
	float fUseWidth = HMS_MIN(fLineWidth, MAX_LINE_WIDTH) + 2.0f;

	//左上角
	pPoss[nPos++] = { Vec2(x1, y1), Color4B(color), top };
	pPoss[nPos++] = { Vec2(x1 - fUseWidth, y1 - fUseWidth), Color4B(color), bottom };

	//右上角
	pPoss[nPos++] = { Vec2(x2, y1), Color4B(color), top };
	pPoss[nPos++] = { Vec2(x2 + fUseWidth, y1 - fUseWidth), Color4B(color), bottom };

	//右下角
	pPoss[nPos++] = { Vec2(x2, y2), Color4B(color), top };
	pPoss[nPos++] = { Vec2(x2 + fUseWidth, y2 + fUseWidth), Color4B(color), bottom };

	//左下角
	pPoss[nPos++] = { Vec2(x1, y2), Color4B(color), top };
	pPoss[nPos++] = { Vec2(x1 - fUseWidth, y2 + fUseWidth), Color4B(color), bottom };

	pPoss[nPos++] = { Vec2(x1, y1), Color4B(color), top };
	pPoss[nPos++] = { Vec2(x1 - fUseWidth, y1 - fUseWidth), Color4B(color), bottom };

	HmsDrawCtrl fd;
	fd.SetColor(color);
	fd.SetTriangleStrip(m_bufferCount, nVertexCount);
	m_vFanDraw.push_back(HmsDrawCtrl(fd));

	m_bufferCount += nVertexCount;
}

void CHmsLightDrawNode::DrawOutterRect(float x1, float y1, float x2, float y2, float fLineWidthH, float fLineWidthV, const Color4F & color /*= Color4F::WHITE*/)
{
	unsigned int nVertexCount = 5 * 2;
	ensureCapacity(nVertexCount);

	Tex2F top, bottom;
	GetTextureCoord(fLineWidthH, top, bottom);

	V2F_C4B_T2F * pPoss = m_pBuffer + m_bufferCount;
	int nPos = 0;

	V2F_C4B_T2F a = { Vec2(x1, y1), Color4B(color), top };
	fLineWidthV = HMS_MIN(fLineWidthV, MAX_LINE_WIDTH) + 2;
	fLineWidthH = HMS_MIN(fLineWidthH, MAX_LINE_WIDTH) + 2;;

	//左上角
	pPoss[nPos++] = a;
	a.vertices = Vec2(x1 - fLineWidthV, y1 - fLineWidthH); a.texCoords = bottom; pPoss[nPos++] = a;

	//右上角
	a.vertices = Vec2(x2, y1); a.texCoords = top; pPoss[nPos++] = a;
	a.vertices = Vec2(x2 + fLineWidthV, y1 - fLineWidthH); a.texCoords = bottom; pPoss[nPos++] = a;

	//右下角
	a.vertices = Vec2(x2, y2); a.texCoords = top; pPoss[nPos++] = a;
	a.vertices = Vec2(x2 + fLineWidthV, y2 + fLineWidthH); a.texCoords = bottom; pPoss[nPos++] = a;

	//左下角
	a.vertices = Vec2(x1, y2); a.texCoords = top; pPoss[nPos++] = a;
	a.vertices = Vec2(x1 - fLineWidthV, y2 + fLineWidthH); a.texCoords = bottom; pPoss[nPos++] = a;

	a.vertices = Vec2(x1, y1); a.texCoords = top; pPoss[nPos++] = a;
	a.vertices = Vec2(x1 - fLineWidthV, y1 - fLineWidthH); a.texCoords = bottom; pPoss[nPos++] = a;

	HmsDrawCtrl fd;
	fd.SetColor(color);
	fd.SetTriangleStrip(m_bufferCount, nVertexCount);
	m_vFanDraw.push_back(HmsDrawCtrl(fd));

	m_bufferCount += nVertexCount;
}

void CHmsLightDrawNode::DrawOutterRect(const Vec2 & pos1, const Vec2 & pos2, float fLineWidth /*= 2.0*/, const Color4F & color /*= Color4F::WHITE*/)
{
	DrawOutterRect(pos1.x, pos1.y, pos2.x, pos2.y, fLineWidth, color);
}

void CHmsLightDrawNode::DrawInnerRect(float x1, float y1, float x2, float y2, float fLineWidth /*= 2.0*/, const Color4F & color /*= Color4F::WHITE*/)
{
	unsigned int nVertexCount = 5 * 2;
	ensureCapacity(nVertexCount);

	Tex2F top, bottom;
	GetTextureCoord(fLineWidth, top, bottom);
	fLineWidth = HMS_MIN(fLineWidth, MAX_LINE_WIDTH);

	V2F_C4B_T2F * pPoss = m_pBuffer + m_bufferCount;
	int nPos = 0;

	//左上角
	pPoss[nPos++] = { Vec2(x1 + fLineWidth, y1 + fLineWidth), Color4B(color), top };
	pPoss[nPos++] = { Vec2(x1 - 2.0, y1 - 2.0), Color4B(color), bottom };//减去2是因为贴图两端各一透明像素.

	//右上角
	pPoss[nPos++] = { Vec2(x2 - fLineWidth, y1 + fLineWidth), Color4B(color), top };
	pPoss[nPos++] = { Vec2(x2 + 2.0, y1 - 2.0), Color4B(color), bottom };

	//右下角
	pPoss[nPos++] = { Vec2(x2 - fLineWidth, y2 - fLineWidth), Color4B(color), top };
	pPoss[nPos++] = { Vec2(x2 + 2.0, y2 + 2.0), Color4B(color), bottom };

	//左下角
	pPoss[nPos++] = { Vec2(x1 + fLineWidth, y2 - fLineWidth), Color4B(color), top };
	pPoss[nPos++] = { Vec2(x1 - 2.0, y2 + 2.0), Color4B(color), bottom };

	pPoss[nPos++] = { Vec2(x1 + fLineWidth, y1 + fLineWidth), Color4B(color), top };
	pPoss[nPos++] = { Vec2(x1 - 2.0, y1 - 2.0), Color4B(color), bottom };

	HmsDrawCtrl fd;
	fd.SetColor(color);
	fd.SetTriangleStrip(m_bufferCount, nVertexCount);
	m_vFanDraw.push_back(HmsDrawCtrl(fd));

	m_bufferCount += nVertexCount;
}

void CHmsLightDrawNode::DrawVerticalLine(float x, float y1, float y2, float fLineWidth /*= 2.0*/, const Color4F & color /*= Color4F::WHITE*/)
{
	unsigned int nVertexCount = 2 * 2;
	ensureCapacity(nVertexCount);

	Tex2F top, bottom;
	float fHalfUseLine = GetTextureCoord(fLineWidth, top, bottom);

	V2F_C4B_T2F * pPoss = m_pBuffer + m_bufferCount;
	int nPos = 0;

	//左上角
	pPoss[nPos++] = { Vec2(x - fHalfUseLine, y1 - 1.0), Color4B(color), top };
	pPoss[nPos++] = { Vec2(x + fHalfUseLine, y1 - 1.0), Color4B(color), bottom };

	//右上角
	pPoss[nPos++] = { Vec2(x - fHalfUseLine, y2 + 1.0), Color4B(color), top };
	pPoss[nPos++] = { Vec2(x + fHalfUseLine, y2 + 1.0), Color4B(color), bottom };

	HmsDrawCtrl fd;
	fd.SetColor(color);
	fd.SetTriangleStrip(m_bufferCount, nVertexCount);
	m_vFanDraw.push_back(HmsDrawCtrl(fd));

	m_bufferCount += nVertexCount;
}

void CHmsLightDrawNode::DrawHorizontalLine(float x1, float x2, float y, float fLineWidth /*= 2.0*/, const Color4F & color /*= Color4F::WHITE*/)
{
	unsigned int nVertexCount = 2 * 2;
	ensureCapacity(nVertexCount);

	Tex2F top, bottom;
	float fHalfUseLine = GetTextureCoord(fLineWidth, top, bottom);

	V2F_C4B_T2F * pPoss = m_pBuffer + m_bufferCount;
	int nPos = 0;

	//左上角
	pPoss[nPos++] = { Vec2(x1/* - 1.0*/, y - fHalfUseLine), Color4B(color), top };
	pPoss[nPos++] = { Vec2(x1/* - 1.0*/, y + fHalfUseLine), Color4B(color), bottom };
	//右上角
	pPoss[nPos++] = { Vec2(x2/* + 1.0*/, y - fHalfUseLine), Color4B(color), top };
	pPoss[nPos++] = { Vec2(x2/* + 1.0*/, y + fHalfUseLine), Color4B(color), bottom };

	HmsDrawCtrl fd;
	fd.SetColor(color);
	fd.SetTriangleStrip(m_bufferCount, nVertexCount);
	m_vFanDraw.push_back(HmsDrawCtrl(fd));

	m_bufferCount += nVertexCount;
}

void CHmsLightDrawNode::DrawSolidRect(float x1, float y1, float x2, float y2, const Color4F & color, bool bBorder)
{
	unsigned int nVertexCount =  2 * 2;
	ensureCapacity(nVertexCount);

	V2F_C4B_T2F * pPoss = m_pBuffer + m_bufferCount;
	int nPos = 0;
	GLfloat fXoffset = float(m_nTexWide - 2) * m_fOneWide;

	/*左顶点.*/
	V2F_C4B_T2F vex = { Vec2(x1, y1), Color4B(color), Tex2F(fXoffset, float(m_nTexHigh - 3)* m_fOneHigh) };
	pPoss[nPos++] = vex;
	/*左下角.*/
	vex = { Vec2(x1, y2), Color4B(color), Tex2F(fXoffset, float(m_nTexHigh - 3)* m_fOneHigh) };
	pPoss[nPos++] = vex;
	/*内框右上角.*/
	vex = { Vec2(x2, y1), Color4B(color), Tex2F(fXoffset, float(m_nTexHigh - 2)* m_fOneHigh) };
	pPoss[nPos++] = vex;
	/*内框右下角.*/
	vex = { Vec2(x2, y2), Color4B(color), Tex2F(fXoffset, float(m_nTexHigh - 2) * m_fOneHigh) };
	pPoss[nPos++] = vex;

	HmsDrawCtrl fd;
	fd.SetColor(color);
	fd.SetTriangleStrip(m_bufferCount, nVertexCount);
	m_vFanDraw.push_back(HmsDrawCtrl(fd));

	m_bufferCount += nVertexCount;

	if (bBorder)
	{
		std::vector<Vec2> vVectors;
		vVectors.push_back(Vec2(x1, y1));
		vVectors.push_back(Vec2(x1, y2));
		vVectors.push_back(Vec2(x2, y2));
		vVectors.push_back(Vec2(x2, y1));
		DrawLineLoop(vVectors, 2.0f, Color4B(color));
	}
}

void CHmsLightDrawNode::DrawSolidRect(const Rect & rect, const Color4F & color, bool bBorder)
{
	DrawSolidRect(rect.getMinX(), rect.getMaxY(), rect.getMaxX(), rect.getMinY(), color, bBorder);
}

void CHmsLightDrawNode::DrawRoundRect(float x1, float y1, float x2, float y2, float fLineWidth, float fRadius /*= 5.0f*/, unsigned int nSegment /*= 10*/, const Color4F & color /*= Color4F::WHITE*/)
{
	unsigned int nVertexCount = (nSegment + 1) * 2 * 4 + 2;

	Tex2F top, bottom;
	float fHalfUsedWidth = GetTextureCoord(fLineWidth, top, bottom);
	float fRadianPerSegment = HMS_2PI / (nSegment * 4);
	float fSinRadian = 0.0, fCosRadian = 0.0;
	float fCurrentRadian = 0.0;
	GLfloat fXoffset = float(m_nTexWide - 3) * m_fOneWide;
	float fRadiusOutter = fRadius + fHalfUsedWidth;
	float fRadiusInner  = fRadius - fHalfUsedWidth;

	ensureCapacity(nVertexCount);

	V2F_C4B_T2F * pPoss = m_pBuffer + m_bufferCount;
	float x1x2 = x2 - x1;
	float y1y2 = y2 - y1;
	Vec2 center(x1 + x1x2 / 2.0, y1 + y1y2 / 2.0);
	V2F_C4B_T2F centerPos = { center, Color4B(color), Tex2F(fXoffset, 0.5) };
	int nPos = 0;
	//pPoss[nPos++] = centerPos;

	struct pp
	{
		float x;
		float y;
	};

	pp offset[] = {
		{ fabsf(x1x2) / 2.0f - fRadius, fabsf(y1y2) / 2.0f - fRadius },
		{ fabsf(x1x2) / 2.0f - fRadius, -fabsf(y1y2) / 2.0f + fRadius },
		{ -fabsf(x1x2) / 2.0f + fRadius, -fabsf(y1y2) / 2.0f + fRadius },
		{ -fabsf(x1x2) / 2.0f + fRadius, fabsf(y1y2) / 2.0f - fRadius }
	};


	for (int seg = 0; seg < 4; seg++)
	{
		float & xOffset = offset[seg].x;
		float & yOffset = offset[seg].y;

		for (unsigned int i = 0; i <= nSegment; i++)
		{
			fCurrentRadian = fRadianPerSegment * i + seg * HMS_PI_2;
			fSinRadian = sin(fCurrentRadian);
			fCosRadian = cos(fCurrentRadian);

			pPoss[nPos++] = { Vec2(fSinRadian * fRadiusOutter + xOffset, fCosRadian * fRadiusOutter + yOffset) + center, Color4B(color), top };
			pPoss[nPos++] = { Vec2(fSinRadian * fRadiusInner + xOffset, fCosRadian * fRadiusInner + yOffset) + center, Color4B(color), bottom };
		}
	}
	pPoss[nPos++] = pPoss[0];
	pPoss[nPos++] = pPoss[1];

	HmsDrawCtrl fd;
	fd.SetColor(color);
	fd.SetTriangleStrip(m_bufferCount, nVertexCount);
	m_vFanDraw.push_back(HmsDrawCtrl(fd));

	m_bufferCount += nVertexCount;
}

void CHmsLightDrawNode::DrawRoundSolidRect(float x1, float y1, float x2, float y2, float fRadius /*= 5.0f*/, unsigned int nSegment /*= 10*/, const Color4F & color /*= Color4F::WHITE*/)
{
	unsigned int nVertexCount = (nSegment + 1) * 4 + 2;

	float fRadianPerSegment = HMS_2PI / (nSegment * 4);
	float fSinRadian = 0.0, fCosRadian = 0.0;
	float fCurrentRadian = 0.0;
	GLfloat fXoffset = float(m_nTexWide - 3) * m_fOneWide;

	ensureCapacity(nVertexCount);

	V2F_C4B_T2F * pPoss = m_pBuffer + m_bufferCount;
	float x1x2 = x2 - x1;
	float y1y2 = y2 - y1;
	Vec2 center(x1 + x1x2 / 2.0, y1 + y1y2 / 2.0);
	V2F_C4B_T2F centerPos = { center, Color4B(color), Tex2F(fXoffset, 0.5) };
	int nPos = 0;
	pPoss[nPos++] = centerPos;

	struct pp
	{
		float x;
		float y;
	};

	pp offset[] = {
		{fabsf(x1x2) / 2.0f - fRadius, fabsf(y1y2) / 2.0f - fRadius},
		{fabsf(x1x2) / 2.0f - fRadius, -fabsf(y1y2) / 2.0f + fRadius},
		{-fabsf(x1x2) / 2.0f + fRadius, -fabsf(y1y2) / 2.0f + fRadius},
		{-fabsf(x1x2) / 2.0f + fRadius, fabsf(y1y2) / 2.0f - fRadius}
	};

	
	for (int seg = 0; seg < 4; seg++)
	{
		float & xOffset = offset[seg].x;
		float & yOffset = offset[seg].y;
		
		for (unsigned int i = 0; i <= nSegment; i++)
		{
			fCurrentRadian = fRadianPerSegment * i + seg * HMS_PI_2;
			fSinRadian = sin(fCurrentRadian);
			fCosRadian = cos(fCurrentRadian);

			V2F_C4B_T2F a = { Vec2(fSinRadian * fRadius + xOffset, fCosRadian * fRadius + yOffset) + center, Color4B(color), Tex2F(fXoffset, 0.5) };
			pPoss[nPos++] = a;
		}
	}
	pPoss[nPos++] = pPoss[1];

	HmsDrawCtrl fd;
	fd.SetColor(color);
	fd.SetTriangleFan(m_bufferCount, nVertexCount);
	m_vFanDraw.push_back(HmsDrawCtrl(fd));

	m_bufferCount += nVertexCount;

	DrawRoundRect(x1, y1, x2, y2, 2.0f, fRadius, 4U, color);
}

void CHmsLightDrawNode::DrawRoundSolidRect(const Rect & rect, float fRadius /*= 5.0f*/, unsigned int nSegment /*= 10*/, const Color4F & color /*= Color4F::WHITE*/)
{
	DrawRoundSolidRect(rect.getMinX(), rect.getMaxY(), rect.getMaxX(), rect.getMinY(), fRadius, nSegment, color);
}

void CHmsLightDrawNode::DrawLineAngle(const Vec2 & center, float fStart, float fEnd, float fAngle, float fLineWidth /*= 2.0*/, const Color4F & color /*= Color4F::WHITE*/)
{
	Tex2F top, bottom;
	float fHalfUsedWidth = GetTextureCoord(fLineWidth, top, bottom);
	float fRadian = ANGLE_TO_RADIAN(fAngle);
	float fSinRadian = sin(fRadian);
	float fCosRadian = cos(fRadian);

	float fWidthRadian = ANGLE_TO_RADIAN(fAngle + 270);
	float fPoxMoveX = fHalfUsedWidth * sin(fWidthRadian);
	float fPoxMoveY = fHalfUsedWidth * cos(fWidthRadian);

	Vec2 moveUp(fPoxMoveX, fPoxMoveY);
	Vec2 moveDown(-fPoxMoveX, -fPoxMoveY);

	Vec2 posStart(fStart*fSinRadian, fStart*fCosRadian);
	Vec2 posEnd(fEnd * fSinRadian, fEnd * fCosRadian);

	posStart += center;
	posEnd += center;

	auto color4b = Color4B(color);

	V2F_C4B_T2F a = { posStart + moveUp, color4b, top };
	V2F_C4B_T2F b = { posStart + moveDown, color4b, bottom };
	V2F_C4B_T2F c = { posEnd + moveUp, color4b, top };
	V2F_C4B_T2F d = { posEnd + moveDown, color4b, bottom };

	unsigned int nVertexCount = 4;
	ensureCapacity(nVertexCount);
	V2F_C4B_T2F * pPoss = m_pBuffer + m_bufferCount;

	int nPos = 0;
	pPoss[nPos++] = a;
	pPoss[nPos++] = b;
	pPoss[nPos++] = c;
	pPoss[nPos++] = d;

	HmsDrawCtrl fd;
	fd.SetColor(color);
	fd.SetTriangleStrip(m_bufferCount, nVertexCount);
	m_vFanDraw.push_back(HmsDrawCtrl(fd));

	m_bufferCount += nVertexCount;
}

void CHmsLightDrawNode::DrawLineAngle(float fStart, float fEnd, float fAngle, float fLineWidth /*= 2.0*/, const Color4F & color /*= Color4F::WHITE*/)
{
	DrawLineAngle(Vec2(0.0, 0.0), fStart, fEnd, fAngle, fLineWidth, color);
}

V2F_C4B_T2F * CHmsLightDrawNode::GetCurrentPosBegin()
{
	return m_pBuffer + m_bufferCount;
}

float CHmsLightDrawNode::GetTextureCoord(float fLineWidth, Tex2F &top, Tex2F &bottom)
{
	float fLnWidth = HMS_MIN(fLineWidth, MAX_LINE_WIDTH); //目前最大仅支持6像素宽度.
	float fTexXPos = (fLnWidth * 2 - 1) / float(m_nTexWide);
	float fTexYPos = (fLnWidth + 2) / float(m_nTexHigh);
	top.u = fTexXPos;
	top.v = 0.0;
	bottom.u = fTexXPos;
	bottom.v = fTexYPos;

	return (fLnWidth * 0.5) + 1.0;
}

void CHmsLightDrawNode::SetDrawTriangleStrip(int nVertexCount, Color4F color)
{
	HmsDrawCtrl fd;
	fd.fColor = color;
	fd.SetTriangleStrip(m_bufferCount, nVertexCount);
	m_vFanDraw.push_back(HmsDrawCtrl(fd));

	m_bufferCount += nVertexCount;
}

void CHmsLightDrawNode::SetDrawLineLoop(int nVertexCount, float fLineWidth)
{
	HmsDrawCtrl fd;
	fd.SetLineLoop(m_bufferCount, nVertexCount, fLineWidth);
	m_vFanDraw.push_back(HmsDrawCtrl(fd));

	m_bufferCount += nVertexCount;
}

void CHmsLightDrawNode::SetDrawPoints(int nVertexCount, float lineWidth)
{
	HmsDrawCtrl fd;
	fd.SetPoints(m_bufferCount, nVertexCount, lineWidth);
	m_vFanDraw.push_back(HmsDrawCtrl(fd));

	m_bufferCount += nVertexCount;
}

void CHmsLightDrawNode::SetDrawLines(int nVertexCount, float lineWidth)
{
	HmsDrawCtrl fd;
	fd.SetLines(m_bufferCount, nVertexCount, lineWidth);
	m_vFanDraw.push_back(HmsDrawCtrl(fd));

	m_bufferCount += nVertexCount;
}

void CHmsLightDrawNode::SetDrawLineStrip(int nVertexCount, GLfloat lineWidth)
{
	HmsDrawCtrl fd;
	fd.SetLineStrip(m_bufferCount, nVertexCount, lineWidth);
	m_vFanDraw.push_back(HmsDrawCtrl(fd));

	m_bufferCount += nVertexCount;
}

void CHmsLightDrawNode::SetDrawTriangles(int nVertexCount)
{
	HmsDrawCtrl fd;
	fd.SetTriangles(m_bufferCount, nVertexCount);
	m_vFanDraw.push_back(HmsDrawCtrl(fd));

	m_bufferCount += nVertexCount;
}

void CHmsLightDrawNode::SetDrawTriangleFan(int nVertexCount)
{
	HmsDrawCtrl fd;
	fd.SetTriangleFan(m_bufferCount, nVertexCount);
	m_vFanDraw.push_back(HmsDrawCtrl(fd));

	m_bufferCount += nVertexCount;
}


void CHmsLightDrawNode::DrawLineLoop(Vec2 * verts, int nCount, float fLineWidth, const Color4B & color)
{
	unsigned int nVertexCount = 2 * (nCount + 1);
	ensureCapacity(nVertexCount);

	Tex2F top, bottom;
	float fHalfWidthDraw = GetTextureCoord(fLineWidth, top, bottom);

	V2F_C4B_T2F * pPoss = GetCurrentPosBegin();
	{
		V2F_C4B_T2F * pCurPos = pPoss;

		for (int i = 0; i <= nCount; i++)
		{
			Vec2 v0 = verts[(i - 1 + nCount) % nCount];
			Vec2 v1 = verts[i%nCount];
			Vec2 v2 = verts[(i + 1) % nCount];

			Vec2 n1 = (v1 - v0).getPerp().getNormalized();
			Vec2 n2 = (v2 - v1).getPerp().getNormalized();
			Vec2 offset = (n1 + n2) * (1.0 / (n1.dot(n2) + 1.0));

			*pCurPos++ = { v1 + offset * fHalfWidthDraw, color, top };
			*pCurPos++ = { v1 - offset * fHalfWidthDraw, color, bottom };
		}
	}

	SetDrawTriangleStrip(nVertexCount, Color4F(color.r*ONETENTH, color.g*ONETENTH, color.b*ONETENTH, color.a*ONETENTH));
}

void CHmsLightDrawNode::DrawLineLoop(Vec2 * verts, int nCount, float fLineWidth, float fStrokeWidth, const Color4B & color /*= Color4B::WHITE*/, const Color4B & colorStroke /*= Color4B::BLACK*/)
{
	unsigned int nVertexCount = 2 * (nCount + 1);
	ensureCapacity(nVertexCount);

	Tex2F top, bottom;
	V2F_C4B_T2F * pPoss = GetCurrentPosBegin();

	float fHalfWidth = fLineWidth * 0.5f;
	float fHalfWidthDraw = GetTextureCoord(fLineWidth, top, bottom);;
	float fHalfStrokeWidth = fHalfWidth + fStrokeWidth;
	{
		V2F_C4B_T2F * pCurStroke = pPoss;
		V2F_C4B_T2F * pCurPos = pPoss + nVertexCount;

		for (int i = 0; i <= nCount; i++)
		{
			Vec2 v0 = verts[(i - 1 + nCount) % nCount];
			Vec2 v1 = verts[i%nCount];
			Vec2 v2 = verts[(i + 1) % nCount];

			Vec2 n1 = (v1 - v0).getPerp().getNormalized();
			Vec2 n2 = (v2 - v1).getPerp().getNormalized();
			Vec2 offset = (n1 + n2) * (1.0 / (n1.dot(n2) + 1.0));

			*pCurPos++ = { v1 + offset * fHalfWidthDraw, color, top };
			*pCurPos++ = { v1 - offset * fHalfWidthDraw, color, bottom };

			*pCurStroke++ = { v1 + offset * fHalfStrokeWidth, colorStroke, top };
			*pCurStroke++ = { v1 - offset * fHalfStrokeWidth, colorStroke, bottom };
		}
	}

	SetDrawTriangleStrip(nVertexCount);
	SetDrawTriangleStrip(nVertexCount);
}

void CHmsLightDrawNode::DrawLineLoop(const std::vector<Vec2> & vPos, float fLineWidth, const Color4B & color /*= Color4B::WHITE*/)
{
	DrawLineLoop((Vec2*)&vPos.at(0), (int)vPos.size(), fLineWidth, color);
}

void CHmsLightDrawNode::DrawLineStrip(Vec2 * verts, int nCount, float fLineWidth, const Color4B & color)
{
	unsigned int nVertexCount = 2 * nCount;
	ensureCapacity(nVertexCount);

	Tex2F top, bottom;
	float fHalfUsedWidth = GetTextureCoord(fLineWidth, top, bottom);
	
	V2F_C4B_T2F * pPoss = GetCurrentPosBegin();

	{
		V2F_C4B_T2F * pCurPos = pPoss;

		Vec2 v0 = verts[0], v1 = verts[1];
		Vec2 n1, n2;
		n1 = (v1 - v0).getPerp().getNormalized();

		*pCurPos++ = { v0 + n1 * fHalfUsedWidth, color, top };
		*pCurPos++ = { v0 - n1 * fHalfUsedWidth, color, bottom };

		int nLoopCnt = nCount - 1;
		for (int i = 1; i < nLoopCnt; i++)
		{
			v0 = v1;
			v1 = verts[i + 1];
			n2 = (v1 - v0).getPerp().getNormalized();

			Vec2 offset = (n1 + n2) * (1.0 / (n1.dot(n2) + 1.0));

			*pCurPos++ = { v0 + offset * fHalfUsedWidth, color, top };
			*pCurPos++ = { v0 - offset * fHalfUsedWidth, color, bottom };

			n1 = n2;
		}

		v1 = verts[nCount - 1];
		n2 = (v1 - v0).getPerp().getNormalized();

		*pCurPos++ = { v1 + n2 * fHalfUsedWidth, color, top };
		*pCurPos++ = { v1 - n2 * fHalfUsedWidth, color, bottom };

	}

	SetDrawTriangleStrip(nVertexCount, Color4F(color.r*ONETENTH, color.g*ONETENTH, color.b*ONETENTH, color.a*ONETENTH));
}

void CHmsLightDrawNode::DrawLineStrip(Vec2 * verts, int nCount, float fLineWidth, float fStrokeWidth, const Color4B & color /*= Color4B::WHITE*/, const Color4B & colorStroke /*= Color4B::BLACK*/)
{
	unsigned int nVertexCount = 2 * nCount;
	ensureCapacity(nVertexCount*2);

	Tex2F top, bottom;
	float fHalfUsedWidth = GetTextureCoord(fLineWidth, top, bottom);

	V2F_C4B_T2F * pPoss = GetCurrentPosBegin();

	float fHalfWidth = fLineWidth * 0.5f;
	float fHalfStroke = fHalfWidth + fStrokeWidth + 1;

	{
		V2F_C4B_T2F * pCurPos = pPoss + nVertexCount;
		V2F_C4B_T2F * pCurStroke = pPoss;

		Vec2 v0 = verts[0], v1 = verts[1];
		Vec2 n1, n2;
		n1 = (v1 - v0).getPerp().getNormalized();

		*pCurPos++ = { v0 + n1 * fHalfUsedWidth, color, top };
		*pCurPos++ = { v0 - n1 * fHalfUsedWidth, color, bottom };

		*pCurStroke++ = { v0 + (n1 * fHalfStroke + n1.getPerp() * fStrokeWidth), colorStroke, top };
		*pCurStroke++ = { v0 - (n1 * fHalfStroke + n1.getRPerp() * fStrokeWidth), colorStroke, bottom };

		int nLoopCnt = nCount - 1;
		for (int i = 1; i < nLoopCnt; i++)
		{
			v0 = v1;
			v1 = verts[i + 1];
			n2 = (v1 - v0).getPerp().getNormalized();

			Vec2 offset = (n1 + n2) * (1.0 / (n1.dot(n2) + 1.0));

			*pCurPos++ = { v0 + offset * fHalfUsedWidth, color, top };
			*pCurPos++ = { v0 - offset * fHalfUsedWidth, color, bottom };

			*pCurStroke++ = { v0 + offset * fHalfUsedWidth, colorStroke, top };
			*pCurStroke++ = { v0 - offset * fHalfUsedWidth, colorStroke, bottom };

			n1 = n2;
		}

		v1 = verts[nCount - 1];
		n2 = (v1 - v0).getPerp().getNormalized();

		*pCurPos++ = { v1 + n2 * fHalfUsedWidth, color, top };
		*pCurPos++ = { v1 - n2 * fHalfUsedWidth, color, bottom };

		*pCurStroke++ = { v1 + (n2 * fHalfStroke + n2.getRPerp() * fStrokeWidth), colorStroke, top };
		*pCurStroke++ = { v1 - (n2 * fHalfStroke + n2.getPerp() * fStrokeWidth), colorStroke, bottom };
	}

 	SetDrawTriangleStrip(nVertexCount);
 	SetDrawTriangleStrip(nVertexCount);
}

void CHmsLightDrawNode::DrawLineStrip(const std::vector<Vec2> & vPos, float fLineWidth, const Color4B & color /*= Color4B::WHITE*/)
{
	DrawLineStrip((Vec2*)&vPos.at(0), (int)vPos.size(), fLineWidth, color);
}

void CHmsLightDrawNode::DrawLines(Vec2 * vertexs, int nCnt, float fLineWidth, const Color4B & color)
{
	unsigned int nVertexCount = 2 * nCnt;
	ensureCapacity(nVertexCount);

	Tex2F top, bottom;
	float fHalfUsedWidth = GetTextureCoord(fLineWidth, top, bottom);

	V2F_C4B_T2F * pPoss = GetCurrentPosBegin();
	int nLines = nVertexCount / 4;

	{
		V2F_C4B_T2F * pCurPos = pPoss;

		for (int i = 0; i < nLines; i++)
		{
			Vec2 v0 = vertexs[i*2];
			Vec2 v1 = vertexs[i * 2 + 1];

			Vec2 n1 = (v1 - v0).getPerp().getNormalized();

			*pCurPos++ = { v0 + n1 * fHalfUsedWidth, color, top };
			*pCurPos++ = { v0 - n1 * fHalfUsedWidth, color, bottom };
			*pCurPos++ = { v1 - n1 * fHalfUsedWidth, color, bottom };
			*pCurPos++ = { v1 + n1 * fHalfUsedWidth, color, top };

			SetDrawTriangleFan(4);
		}
	}
}

void CHmsLightDrawNode::DrawLines(Vec2 * vertexs, int nCnt, float fLineWidth, float fStrokeWidth, const Color4B & color /*= Color4B::WHITE*/, const Color4B & colorStroke /*= Color4B::BLACK*/)
{
	unsigned int nVertexCount = 2 * nCnt;
	ensureCapacity(nVertexCount * 2);

	V2F_C4B_T2F * pPoss = GetCurrentPosBegin();

	Tex2F top, bottom;
	float fHalfUsedWidth = GetTextureCoord(fLineWidth, top, bottom);
	float fHalfWidth = fLineWidth * 0.5f;
	float fHalfStrokeWidth = fHalfWidth + fStrokeWidth + 1;
	int nLines = nVertexCount / 4;

	{
		V2F_C4B_T2F * pCurPos = pPoss;

		for (int i = 0; i < nLines; i++)
		{
			Vec2 v0 = vertexs[i * 2];
			Vec2 v1 = vertexs[i * 2 + 1];

			Vec2 n1 = (v1 - v0).getPerp().getNormalized();

			//draw line stroke
			*pCurPos++ = { v0 + n1 * fHalfStrokeWidth + n1.getPerp()*fStrokeWidth, colorStroke, top };
			*pCurPos++ = { v0 - n1 * fHalfStrokeWidth + n1.getPerp()*fStrokeWidth, colorStroke, bottom };
			*pCurPos++ = { v1 - n1 * fHalfStrokeWidth + n1.getRPerp()*fStrokeWidth, colorStroke, top };
			*pCurPos++ = { v1 + n1 * fHalfStrokeWidth + n1.getRPerp()*fStrokeWidth, colorStroke, bottom };
			SetDrawTriangleFan(4);

			//draw line
			*pCurPos++ = { v0 + n1 * fHalfUsedWidth, color, top };
			*pCurPos++ = { v0 - n1 * fHalfUsedWidth, color, bottom };
			*pCurPos++ = { v1 - n1 * fHalfUsedWidth, color, top };
			*pCurPos++ = { v1 + n1 * fHalfUsedWidth, color, bottom };
			SetDrawTriangleFan(4);
		}
	}
}

void CHmsLightDrawNode::DrawLines(const std::vector<Vec2> & vPos, float fLineWidth, const Color4B & color /*= Color4B::WHITE*/)
{
	DrawLines((Vec2*)vPos.data(), vPos.size(), fLineWidth, color);
}

int CHmsLightDrawNode::GetDrawIndex()
{
	return m_vFanDraw.size() - 1;
}

void CHmsLightDrawNode::DrawIndex(int nIndex)
{
	if (nIndex >= 0 && nIndex <= int(m_vFanDraw.size()))
	{
		auto & fd = m_vFanDraw[nIndex];

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
}

void CHmsLightDrawNode::DrawSolidRectWithSegment(float x1, float y1, float x2, float y2, int nSegment, const Color4F & color /*= Color4F::WHITE*/)
{
	unsigned int nVertexCount = 2 * (nSegment + 1);

	float fWidth = x2 - x1;
	float fWidthMove = fWidth / nSegment;
	GLfloat fXoffset = float(m_nTexWide - 2) * m_fOneWide;

	ensureCapacity(nVertexCount);

	V2F_C4B_T2F a = { Vec2(x1, y1), Color4B(color), Tex2F(fXoffset, 0.5) };
	V2F_C4B_T2F b = { Vec2(x1, y2), Color4B(color), Tex2F(fXoffset, 0.5) };


	V2F_C4B_T2F * pPoss = GetCurrentPosBegin();
	Vec2 move = Vec2(fWidthMove, 0);
	{
		V2F_C4B_T2F * pCurPos = pPoss;

		for (int i = 0; i <= nSegment; i++)
		{
			a.vertices += move;
			b.vertices += move;
			*pCurPos++ = a;
			*pCurPos++ = b;
		}
	}

	SetDrawTriangleStrip(nVertexCount);
}

void CHmsLightDrawNode::DrawTriangles(const std::vector<Vec2> & vPos, const Color4F & color, bool bBorder)
{
	unsigned int nVertexCount = vPos.size();

	if (nVertexCount >= 3)
	{
		ensureCapacity(nVertexCount);

		V2F_C4B_T2F * pPoss = m_pBuffer + m_bufferCount;
		int nPos = 0;
		GLfloat fXoffset = float(m_nTexWide - 2) * m_fOneWide;

		for (unsigned int i = 0; i < nVertexCount; i++)
		{
			pPoss[nPos++] = { vPos.at(i), Color4B(color), Tex2F(fXoffset, 0.5) };
		}

		HmsDrawCtrl fd;
		fd.SetColor(color);
		fd.SetTriangles(m_bufferCount, nVertexCount);
		m_vFanDraw.push_back(HmsDrawCtrl(fd));
		m_bufferCount += nVertexCount;

		if (bBorder)
		{
			DrawLineLoop(vPos, 1.0f, Color4B(color));
		}
	}
}

void CHmsLightDrawNode::DrawTriangles(const Vec2 & center, const std::vector<Vec2> & vPos, const Color4F & color /*= Color4F::WHITE*/)
{
	unsigned int nVertexCount = vPos.size();

	if (nVertexCount >= 3)
	{
		ensureCapacity(nVertexCount);

		std::vector<Vec2> vVectors;
		GLfloat fXoffset = float(m_nTexWide - 2) * m_fOneWide;
		V2F_C4B_T2F * pPoss = m_pBuffer + m_bufferCount;
		int nPos = 0;
		
		for (unsigned int i = 0; i < nVertexCount; i++)
		{
			pPoss[nPos++] = { vPos.at(i) + center, Color4B(color), Tex2F(fXoffset, 0.5) };
			vVectors.push_back(vPos.at(i) + center);
		}

		HmsDrawCtrl fd;
		fd.SetColor(color);
		fd.SetTriangles(m_bufferCount, nVertexCount);
		m_vFanDraw.push_back(HmsDrawCtrl(fd));
		m_bufferCount += nVertexCount;

		DrawLineLoop(vVectors, 2.0f, Color4B(color));
	}
}

void CHmsLightDrawNode::DrawQuad(const Vec2 & leftTop, const Vec2 & rightBottom, const Color4F & color, bool bBorder)
{
	unsigned int nVertexCount = 4;

	ensureCapacity(nVertexCount);

	V2F_C4B_T2F * pPoss = m_pBuffer + m_bufferCount;
	int nPos = 0;
	GLfloat fXoffset = float(m_nTexWide - 2) * m_fOneWide;

	pPoss[nPos++] = { leftTop, Color4B(color), Tex2F(fXoffset, 0.5) };
	pPoss[nPos++] = { Vec2(leftTop.x, rightBottom.y), Color4B(color), Tex2F(fXoffset, 0.5) };
	pPoss[nPos++] = { rightBottom, Color4B(color), Tex2F(fXoffset, 0.5) };
	pPoss[nPos++] = { Vec2(rightBottom.x, leftTop.y), Color4B(color), Tex2F(fXoffset, 0.5) };

	HmsDrawCtrl fd;
	fd.SetColor(color);
	fd.SetTriangleFan(m_bufferCount, nVertexCount);
	m_vFanDraw.push_back(HmsDrawCtrl(fd));
	m_bufferCount += nVertexCount;

	if (bBorder)
	{
		std::vector<Vec2> vVectors;
		vVectors.push_back(leftTop);
		vVectors.push_back(Vec2(rightBottom.x, leftTop.y));
		vVectors.push_back(rightBottom);
		vVectors.push_back(Vec2(leftTop.x, rightBottom.y));
		DrawLineLoop(vVectors, 2.0f, Color4B(color));
	}
}

void CHmsLightDrawNode::DrawQuad(const Vec2 & center, const Vec2 & leftTop, const Vec2 & rightBottom, const Color4F & color, bool bBorder)
{
	unsigned int nVertexCount = 4;

	ensureCapacity(nVertexCount);

	V2F_C4B_T2F * pPoss = m_pBuffer + m_bufferCount;
	int nPos = 0;
	GLfloat fXoffset = float(m_nTexWide - 2) * m_fOneWide;
	
	pPoss[nPos++] = { leftTop + center, Color4B(color), Tex2F(fXoffset, 0.5) };
	pPoss[nPos++] = { Vec2(leftTop.x, rightBottom.y) + center, Color4B(color), Tex2F(fXoffset, 0.5) };
	pPoss[nPos++] = { rightBottom + center, Color4B(color), Tex2F(fXoffset, 0.5) };
	pPoss[nPos++] = { Vec2(rightBottom.x, leftTop.y) + center, Color4B(color), Tex2F(fXoffset, 0.5) };

	HmsDrawCtrl fd;
	fd.SetColor(color);
	fd.SetTriangleFan(m_bufferCount, nVertexCount);
	m_vFanDraw.push_back(HmsDrawCtrl(fd));
	m_bufferCount += nVertexCount;

	if (bBorder)
	{
		std::vector<Vec2> vVectors;
		vVectors.push_back(leftTop + center);
		vVectors.push_back(Vec2(leftTop.x, rightBottom.y) + center);
		vVectors.push_back(rightBottom + center);
		vVectors.push_back(Vec2(rightBottom.x, leftTop.y) + center);
		DrawLineLoop(vVectors, 2.0f, Color4B(color));
	}
}

void CHmsLightDrawNode::DrawDashLines(const std::vector<Vec2> & vPos, float fLineWidth, float fSolidLength, float fDashLength, const Color4B & color /*= Color4B::WHITE*/)
{
	float fOneDashLength = fDashLength + fSolidLength;
	float fDashRate = fDashLength / fOneDashLength;
	float fSolidRate = 1 - fDashRate;
	for (int i = 1; i < (int)vPos.size(); i += 2)
	{
		auto & pos1 = vPos.at(i - 1);
		auto & pos2 = vPos.at(i);
		auto n = pos2 - pos1;
		auto length = n.getLength();
		n = n.getNormalized() * fOneDashLength;

		float fSegmentCnt = length / fOneDashLength;
		int nSegmentCnt = floor(fSegmentCnt);
		
		std::vector<Vec2>	vLines;

		float fOverflow = fSegmentCnt - nSegmentCnt;
		if (fOverflow > fDashRate)
		{
			vLines.push_back(pos1);
			float fPos = (fOverflow - fDashRate) / 2;
			vLines.push_back(pos1 + fPos*n);

			fPos += fDashRate;
			for (int i = 0; i < nSegmentCnt; i++)
			{
				vLines.push_back(pos1 + fPos*n);
				fPos += fSolidRate;
				vLines.push_back(pos1 + fPos*n);
				fPos += fDashRate;
			}
			vLines.push_back(pos1 + fPos*n);
			fPos += (fOverflow - fDashRate) / 2;
			vLines.push_back(pos1 + fPos*n);
		}
		else
		{
			float fPos = fOverflow / 2;
			for (int i = 0; i < nSegmentCnt; i++)
			{
				vLines.push_back(pos1 + fPos*n);
				fPos += fSolidRate;
				vLines.push_back(pos1 + fPos*n);
				fPos += fDashRate;
			}
		}

		DrawLines(vLines, fLineWidth, color);
	}
}

void CHmsLightDrawNode::DrawArc(const Vec2& center, float fAngleBegin, float fAngleEnd, float fRadius, float fLineWidth /*= 2.0*/, unsigned int nSegments /*= 50*/, const Color4F & color /*= Color4F::WHITE*/)
{
	if (fAngleEnd == fAngleBegin)
		return;

	DrawFanBorder(center, fAngleBegin, fAngleEnd, fRadius, fLineWidth, nSegments, color);
}

void CHmsLightDrawNode::DrawArc(float fAngleBegin, float fAngleEnd, float fRadius, float fLineWidth /*= 2.0*/, unsigned int nSegments /*= 50*/, const Color4F & color /*= Color4F::WHITE*/)
{
	DrawFanBorder(Vec2(0, 0), fAngleBegin, fAngleEnd, fRadius, fLineWidth, nSegments, color);
}

NS_HMS_END