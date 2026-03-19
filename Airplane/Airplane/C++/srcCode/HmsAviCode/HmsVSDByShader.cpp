#include "HmsVSDByShader.h"
#include "HmsGlobal.h"
#include "render/HmsGLProgram.h"
#include "render/HmsGLProgramState.h"
#include "render/HmsGLStateCache.h"
#include "render/HmsRenderer.h"
#include "render/HmsTexture2D.h"
#include "Calculate/CalculateZs.h"
USING_NS_HMS;

CHmsVSDByShader::CHmsVSDByShader()
	:m_vao(0)
	, m_vbo(0)
	, m_bufferCapacity(0)
	, m_bufferCount(0)
	, m_pBuffer(nullptr)
	, m_bDirty(false)
	, m_nTextureUniform(0)
	, m_nCenterPosUniform(0)
	, m_nHeadingSinCosUniform(0)
	, m_nShowRangeUniform(0)
	, m_nMapRangeInPicUniform(0)
	, m_nLevelInGLPosUniform(0)
	, m_nShowRangeZeroPosUniform(0)
	, m_nCurAltUniform(0)
	, m_fLonLatOffset(1.0f)
	, m_fZeroPosStart(20)
	, m_fCurAlt(0)
	, m_fCurMapRange(10)
{
	m_blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;

	m_vCenterPos = Vec2(0.5, 0.5);
	m_vHeadingSinCos = Vec2(0, 1);
	m_vShowRange = Vec2(700, 300);
	m_vMapPicRange = Vec2(0.1f, 0.1f);			//0-1
	m_vLevelPosAndHeight = Vec2(0, 300);
	m_vCenterPos = Vec2(0.1f, 0.3f);
}


CHmsVSDByShader::~CHmsVSDByShader()
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

bool CHmsVSDByShader::Init()
{
	m_blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;

	auto glProgram = GLProgram::createWithFilenames("Shaders/zs_VSD.vsh", "Shaders/zs_VSD.fsh");
	if (0 == glProgram)
	{
		return false;
	}
	auto state = GLProgramState::getOrCreateWithGLProgram(glProgram);
	SetGLProgramState(state);

	m_nTextureUniform = glGetUniformLocation(glProgram->getProgram(), "HMS_Texture0");
	m_nCenterPosUniform = glGetUniformLocation(glProgram->getProgram(), "u_centerPos");
	m_nHeadingSinCosUniform = glGetUniformLocation(glProgram->getProgram(), "u_HeadingSinCos");
	m_nShowRangeUniform = glGetUniformLocation(glProgram->getProgram(), "u_showRange");
	m_nMapRangeInPicUniform = glGetUniformLocation(glProgram->getProgram(), "u_range");
	m_nLevelInGLPosUniform = glGetUniformLocation(glProgram->getProgram(), "u_levelPosYHeight");
	m_nShowRangeZeroPosUniform = glGetUniformLocation(glProgram->getProgram(), "u_fShowRangeStart");
	m_nCurAltUniform = glGetUniformLocation(glProgram->getProgram(), "u_fCurAlt");

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

void CHmsVSDByShader::Draw(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	if (m_bufferCount)
	{
		m_customCommand.init(m_globalZOrder, transform, flags);
		m_customCommand.func = HMS_CALLBACK_0(CHmsVSDByShader::onDraw, this, transform, flags);
		renderer->addCommand(&m_customCommand);
	}
}

void CHmsVSDByShader::ensureCapacity(int count)
{
	HMSASSERT(count >= 0, "capacity must be >= 0");

	if (m_bufferCount + count > m_bufferCapacity)
	{
		m_bufferCapacity += MAX(m_bufferCapacity, count);
		m_pBuffer = (V2F_C4B_T2F*)realloc(m_pBuffer, m_bufferCapacity*sizeof(V2F_C4B_T2F));
	}
}

void CHmsVSDByShader::onDraw(const Mat4 &transform, uint32_t flags)
{
	auto glProgram = GetGLProgram();
	glProgram->use();
	glProgram->setUniformsForBuiltins(transform);

	if (m_textureImage)
	{
		GL::bindTexture2D(m_textureImage->getName());
	}
	else
	{
		GL::bindTexture2D(0);
	}

	auto SetUnifromVec2 = [=](int nLocation, const Vec2 & pos)
	{
		glProgram->setUniformLocationWith2f(nLocation, pos.x, pos.y);
	};

	glUniform1i(m_nTextureUniform, 0);
	SetUnifromVec2(m_nCenterPosUniform, m_vCenterPos);
	SetUnifromVec2(m_nHeadingSinCosUniform, m_vHeadingSinCos);
	SetUnifromVec2(m_nShowRangeUniform, m_vShowRange);
	SetUnifromVec2(m_nMapRangeInPicUniform, m_vMapPicRange);
	SetUnifromVec2(m_nLevelInGLPosUniform, m_vLevelPosAndHeight);

	glProgram->setUniformLocationWith1f(m_nCurAltUniform, m_fCurAlt);
	glProgram->setUniformLocationWith1f(m_nShowRangeZeroPosUniform, m_fZeroPosStart);

	GL::blendFunc(m_blendFunc.src, m_blendFunc.dst);
	glEnable(GL_BLEND);
	//glEnable(GL_MULTISAMPLE);

	if (m_bDirty)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(V2F_C4B_T2F)*m_bufferCapacity, m_pBuffer, GL_STREAM_DRAW);

		m_bDirty = false;
	}
	//if (Configuration::getInstance()->supportsShareableVAO())
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

// 	HMS_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1, m_bufferCount);
// 	CHECK_GL_ERROR_DEBUG();
}

void CHmsVSDByShader::onDrawShape()
{
	for (auto & fd : m_vFanDraw)
	{
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

void CHmsVSDByShader::clear()
{
	m_bufferCount = 0;
	m_bDirty = true;
	m_vFanDraw.clear();
}

const BlendFunc& CHmsVSDByShader::getBlendFunc() const
{
	return m_blendFunc;
}

void CHmsVSDByShader::setBlendFunc(const BlendFunc &blendFunc)
{
	m_blendFunc = blendFunc;
}

HmsAviPf::V2F_C4B_T2F * CHmsVSDByShader::GetCurrentPosBegin()
{
	return m_pBuffer + m_bufferCount;
}

void CHmsVSDByShader::SetDrawTriangleStrip(int nVertexCount)
{
	HmsDrawCtrl fd;
	fd.SetTriangleStrip(m_bufferCount, nVertexCount);
	m_vFanDraw.push_back(HmsDrawCtrl(fd));

	m_bufferCount += nVertexCount;
}

// void CHmsVSDByShader::SetDrawQuadStrip(int nVertexCount)
// {
// 	HmsDrawCtrl fd;
// 	fd.SetQuadStrip(m_bufferCount, nVertexCount);
// 	m_vFanDraw.push_back(HmsDrawCtrl(fd));
// 
// 	m_bufferCount += nVertexCount;
// }

// void CHmsVSDByShader::SetDrawQuads(int nVertexCount)
// {
// 	HmsDrawCtrl fd;
// 	fd.SetQuads(m_bufferCount, nVertexCount);
// 	m_vFanDraw.push_back(HmsDrawCtrl(fd));
// 
// 	m_bufferCount += nVertexCount;
// }

int CHmsVSDByShader::GetDrawIndex()
{
	return m_vFanDraw.size() - 1;
}

void CHmsVSDByShader::DrawIndex(int nIndex)
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

void CHmsVSDByShader::DrawSolidRectWithSegment(float x1, float y1, float x2, float y2, int nSegment, const HmsAviPf::Color4F & color /*= HmsAviPf::Color4F::WHITE*/)
{
	unsigned int nVertexCount = 2 * (nSegment+1);

	float fWidth = x2 - x1;
	float fWidthMove = fWidth / nSegment;

	ensureCapacity(nVertexCount);

	V2F_C4B_T2F a = { Vec2(x1, y1), Color4B(color), Tex2F(0.0, 0.0) };
	V2F_C4B_T2F b = { Vec2(x1, y2), Color4B(color), Tex2F(0.0, 0.0) };


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

	m_vShowRange.setPoint(x2 - x1, y2 - y1);
}

void CHmsVSDByShader::SetTexture(HmsAviPf::Texture2D * texture, const HmsAviPf::Vec2 & mapCenter, const HmsAviPf::Rect & mapRect, const HmsAviPf::Vec2 & mapDstOffset, float fLonLatOffset)
{
	m_textureImage = texture;
	m_mapCenter = mapCenter;
	m_mapRect = mapRect;
	m_mapDstOffset = mapDstOffset;
	m_fLonLatOffset = fLonLatOffset * 3;

	UpdateRangeRelation(); 
}

void CHmsVSDByShader::SetLevelPosAndHeight(float posY, float height)
{
	m_vLevelPosAndHeight.setPoint(posY, height);
}

void CHmsVSDByShader::SetFlightData(const HmsAviPf::Vec2 & wgcPos, float fCurAlt, float fCurHeading)
{
	float fHeadingRad = ToRadian(fCurHeading);
	m_vHeadingSinCos.setPoint(sin(fHeadingRad), cos(fHeadingRad));
	m_fCurAlt = fCurAlt;
	m_curCenter = wgcPos;

	auto delta = m_curCenter - m_mapCenter;
	delta = delta/m_fLonLatOffset;
	m_vCenterPos = Vec2(0.5, 0.5) + Vec2(delta.x, -delta.y);
}

void CHmsVSDByShader::SetZeroStartPos(float fPos)
{
	m_fZeroPosStart = fPos;
}

void CHmsVSDByShader::SetCurMapRange(float fRange)
{
	m_fCurMapRange = fRange;

	UpdateRangeRelation();
}

void CHmsVSDByShader::UpdateRangeRelation()
{
	m_vMapPicRange = Vec2(m_fCurMapRange /m_mapDstOffset.x, m_fCurMapRange/ m_mapDstOffset.y);
	
	
}
