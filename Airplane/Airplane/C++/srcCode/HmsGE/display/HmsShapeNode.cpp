#include "HmsShapeNode.h"
#include "HmsBezier.h"
#include "HmsAviDisplayUnit.h"
#include "render/HmsGLProgramCache.h"
#include "render/HmsGLProgram.h"
#include "render/HmsGLStateCache.h"
#include <algorithm>
NS_HMS_BEGIN


CHmsShapeNode::CHmsShapeNode()
	: _currentStencilEnabled(GL_FALSE)
	, _currentStencilWriteMask(~0)
	, _currentStencilFunc(GL_ALWAYS)
	, _currentStencilRef(0)
	, _currentStencilValueMask(~0)
	, _currentStencilFail(GL_KEEP)
	, _currentStencilPassDepthFail(GL_KEEP)
	, _currentStencilPassDepthPass(GL_KEEP)
	, _currentDepthWriteMask(GL_TRUE)
	, _currentAlphaTestEnabled(GL_FALSE)
	, _currentAlphaTestFunc(GL_ALWAYS)
	, _currentAlphaTestRef(1)
	, _mask_layer(0x80)
{
}


CHmsShapeNode::~CHmsShapeNode()
{
}

void CHmsShapeNode::OnStencilBegin()
{
	_currentStencilEnabled = glIsEnabled(GL_STENCIL_TEST);
	glGetIntegerv(GL_STENCIL_WRITEMASK, (GLint *)&_currentStencilWriteMask);
	glGetIntegerv(GL_STENCIL_FUNC, (GLint *)&_currentStencilFunc);
	glGetIntegerv(GL_STENCIL_REF, &_currentStencilRef);
	glGetIntegerv(GL_STENCIL_VALUE_MASK, (GLint *)&_currentStencilValueMask);
	glGetIntegerv(GL_STENCIL_FAIL, (GLint *)&_currentStencilFail);
	glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL, (GLint *)&_currentStencilPassDepthFail);
	glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS, (GLint *)&_currentStencilPassDepthPass);

	glEnable(GL_STENCIL_TEST);

	// check for OpenGL error while enabling stencil test

	_mask_layer_le = 0x80 & _currentStencilValueMask;
	//glStencilMask(_mask_layer);
	//RenderState::StateBlock::_defaultState->setStencilWrite(mask_layer);

	glGetBooleanv(GL_DEPTH_WRITEMASK, &_currentDepthWriteMask);

// 	glDepthMask(GL_FALSE);
// 	RenderState::StateBlock::_defaultState->setDepthWrite(false);
}

void CHmsShapeNode::OnStencilEnd()
{
	///////////////////////////////////
	// CLEANUP
	glStencilFunc(_currentStencilFunc, _currentStencilRef, _currentStencilValueMask);

	glStencilOp(_currentStencilFail, _currentStencilPassDepthFail, _currentStencilPassDepthPass);

	glStencilMask(_currentStencilWriteMask);
	if (!_currentStencilEnabled)
	{
		glDisable(GL_STENCIL_TEST);
	}
}

void CHmsShapeNode::drawFullScreenQuadClearStencil()
{
	auto du = CHmsAviDisplayUnit::GetInstance();

	du->PushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	du->LoadIdentityMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);

	du->PushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
	du->LoadIdentityMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);

	Vec2 vertices[] = {
		Vec2(-1.0f, -1.0f),
		Vec2(1.0f, -1.0f),
		Vec2(1.0f, 1.0f),
		Vec2(-1.0f, 1.0f)
	};

	auto glProgram = GLProgramCache::getInstance()->getGLProgram(GLProgram::SHADER_NAME_POSITION_U_COLOR);

	if (glProgram)
	{
		int colorLocation = glProgram->getUniformLocation("u_color");

		Color4F color(1, 1, 1, 1);

		glProgram->use();
		glProgram->setUniformsForBuiltins();
		glProgram->setUniformLocationWith4fv(colorLocation, (GLfloat*)&color.r, 1);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	GL::enableVertexAttribs(GL::VERTEX_ATTRIB_FLAG_POSITION);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, vertices);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	du->PopMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
	du->PopMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}

void CHmsShapeNode::drawFullScreenQuadFillShape()
{
	auto du = CHmsAviDisplayUnit::GetInstance();
	HMSASSERT(nullptr != director, "Director is null when setting matrix stack");

	du->PushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	du->LoadIdentityMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);

	du->PushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
	du->LoadIdentityMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);

	Vec2 vertices[] = {
		Vec2(-1.0f, -1.0f),
		Vec2(1.0f, -1.0f),
		Vec2(1.0f, 1.0f),
		Vec2(-1.0f, 1.0f)
	};

	auto glProgram = GLProgramCache::getInstance()->getGLProgram(GLProgram::SHADER_NAME_POSITION_U_COLOR);

	if (glProgram)
	{
		int colorLocation = glProgram->getUniformLocation("u_color");

		Color4F color(1, 1, 1, 1);

		glProgram->use();
		glProgram->setUniformsForBuiltins();
		glProgram->setUniformLocationWith4fv(colorLocation, (GLfloat*)&color.r, 1);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	GL::enableVertexAttribs(GL::VERTEX_ATTRIB_FLAG_POSITION);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, vertices);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	du->PopMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
	du->PopMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}

void CHmsShapeNode::onDrawShape()
{
	int nMaxIndex = GetDrawIndex();
	glEnable(GL_STENCIL_TEST);

	OnStencilBegin();
	int nSkipIndex = -1;
	for (int i = 0; i <= nMaxIndex; i++)
	{
		if (nSkipIndex == i)
		{
			continue;
		}
		nSkipIndex = OnPrintShapeIndex(i);
	}
	OnStencilEnd();
}

// static GLuint s_cosshatched[] =
// {
// 	0x81818181, 0xc0c0c0c0, 0x60606060, 0x30303030, 0x18181818, 0x0c0c0c0c, 0x06060606, 0x03030303,
// 	0x81818181, 0xc0c0c0c0, 0x60606060, 0x30303030, 0x18181818, 0x0c0c0c0c, 0x06060606, 0x03030303,
// 	0x81818181, 0xc0c0c0c0, 0x60606060, 0x30303030, 0x18181818, 0x0c0c0c0c, 0x06060606, 0x03030303,
// 	0x81818181, 0xc0c0c0c0, 0x60606060, 0x30303030, 0x18181818, 0x0c0c0c0c, 0x06060606, 0x03030303,
// };
#include "HmsGL.h"

int CHmsShapeNode::OnPrintShapeIndex(int nShapeIndex)
{
	int nSkipNext = -1;
	auto itFind = m_mapShapeArea.find(nShapeIndex);
	if (itFind != m_mapShapeArea.end())
	{
		nSkipNext = itFind->second.nRectIndex;

		//清空数据位
		glStencilFunc(GL_NEVER, _mask_layer, _mask_layer);
		glStencilOp(GL_ZERO, GL_ZERO, GL_REPLACE);
		auto & rect = itFind->second.rect;

		DrawIndex(itFind->second.nRectIndex);

		//绘制显示区域的蒙版
		glColorMask(false, false, false, false);
		glStencilFunc(GL_ALWAYS, _mask_layer, _mask_layer);
		glStencilOp(GL_KEEP, GL_INVERT, GL_INVERT);
		DrawIndex(nShapeIndex);
		glColorMask(true, true, true, true);

		//绘制显示区域
		glStencilFunc(GL_EQUAL, _mask_layer_le, _mask_layer_le);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		
		DrawIndex(nSkipNext);

		//还原标识位
		glStencilFunc(GL_ALWAYS, _mask_layer, _mask_layer);
	}
	else
	{
#if 1
		DrawIndex(nShapeIndex);
#else
		//清空数据位置
		glStencilFunc(GL_NEVER, _mask_layer, _mask_layer);
		glStencilOp(GL_ZERO, GL_ZERO, GL_REPLACE);
		DrawIndex(nShapeIndex);

		//绘制显示区域的蒙版
		glColorMask(false, false, false, false);
		glStencilFunc(GL_ALWAYS, _mask_layer, _mask_layer);
		glStencilOp(GL_KEEP, GL_INVERT, GL_INVERT);
 		DrawIndex(nShapeIndex);
 		DrawIndex(nShapeIndex);
		glColorMask(true, true, true, true);

		//显示区域上色
		glStencilFunc(GL_EQUAL, _mask_layer_le, _mask_layer_le);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		DrawIndex(nShapeIndex);

		//还原蒙版的标识位
// 		GLuint invertMaskLe = ~_mask_layer_le;
// 		glColorMask(false, false, false, false);
// 		glStencilFunc(GL_EQUAL, invertMaskLe, invertMaskLe);
// 		glStencilOp(GL_KEEP, GL_INVERT, GL_INVERT);
// 		DrawIndex(nShapeIndex);
// 		glColorMask(true, true, true, true);


		glStencilFunc(GL_ALWAYS, _mask_layer, _mask_layer);
#endif
	}

	return nSkipNext;
}

void CHmsShapeNode::DrawShape(const std::vector<Vec2> & vShape, const Color4B & color)
{
	if (0 == vShape.size())
	{
		return;
	}
	unsigned int nVertexCount = vShape.size();

	ensureCapacity(nVertexCount);

	V2F_C4B_T2F * pPoss = GetCurrentPosBegin();

	ShapeDrawInfo sdi;
	sdi.color = color;
	Vec2 min = vShape.at(0);
	Vec2 max = min;

	for (unsigned int i = 0; i < nVertexCount; i++)
	{
		auto & v = vShape.at(i);
		pPoss[i].vertices = v;
		pPoss[i].colors = color;
		pPoss[i].texCoords = Tex2F(0.0, 0.0);

		if (min.x > v.x)min.x = v.x;
		if (min.y > v.y)min.y = v.y;
		if (max.x < v.x)max.x = v.x;
		if (max.y < v.y)max.y = v.y;
	}

	//sdi.rect.setRect(min.x - 4, min.y - 4, max.x - min.x + 8, max.y - min.y + 8);
	sdi.rect.setRect(min.x-1, min.y-1, max.x - min.x+2, max.y - min.y+2);
	SetDrawTriangleFan(nVertexCount);
	int nDraw = GetDrawIndex();
	DrawSolidRect(sdi.rect, Color4F(color));
	sdi.nRectIndex = GetDrawIndex();
	m_mapShapeArea.insert(std::pair<int, ShapeDrawInfo>(nDraw, sdi));
}

void CHmsShapeNode::DrawPainterPath(const CHmsPainterPath & path, float fLineWidth, const Color4B & color /*= Color4B::WHITE*/)
{
	auto vE = path.GetPathElements();
	GLfloat curveInverseScale = 1.0f;

	std::vector<Vec2>	vArrayPos;
	vArrayPos.reserve(100);

	auto GetPos = [=](int nPos)
	{
		auto & e = vE->at(nPos);
		return Vec2(float(e.x), float(e.y));
	};

	for (unsigned int i = 0; i < vE->size(); i++)
	{
		switch (vE->at(i).type)
		{
		case CHmsPainterPath::MoveToElement:
			vArrayPos.push_back(GetPos(i));
			break;
		case CHmsPainterPath::LineToElement:
			vArrayPos.push_back(GetPos(i));
			break;
		case CHmsPainterPath::CurveToElement:
		{
			CHmsBezier b = CHmsBezier::fromPoints(GetPos(i - 1),
				GetPos(i),
				GetPos(i + 1),
				GetPos(i + 2));
			auto bounds = b.bounds();
			// threshold based on same algorithm as in qtriangulatingstroker.cpp
			int threshold = HMS_MIN(64, (int)(HMS_MAX(bounds.size.width, bounds.size.height) * 3.14f / (curveInverseScale * 6)));
			if (threshold < 3) threshold = 3;
			float one_over_threshold_minus_1 = float(1) / (threshold - 1);
			for (int t = 0; t < threshold; ++t) {
				auto pt = b.pointAt(t * one_over_threshold_minus_1);
				vArrayPos.push_back(pt);
			}
			i += 2;
		}
		break;
		case CHmsPainterPath::CurveToDataElement:
			break;
		default:
			break;
		}
	}

	DrawLineStrip(vArrayPos.data(), vArrayPos.size(), fLineWidth, color);
}

void CHmsShapeNode::FillPainterPath(const CHmsPainterPath & path, const Color4B & color /*= Color4B::GREEN*/)
{
	auto vE = path.GetPathElements();
	GLfloat curveInverseScale = 1.0f;

	std::vector<Vec2>	vArrayPos;
	vArrayPos.reserve(100);

	auto GetPos = [=](int nPos)
	{
		auto & e = vE->at(nPos);
		return Vec2(float(e.x), float(e.y));
	};

	for (unsigned int i = 0; i < vE->size(); i++)
	{
		switch (vE->at(i).type)
		{
		case CHmsPainterPath::MoveToElement:
			vArrayPos.push_back(GetPos(i));
			break;
		case CHmsPainterPath::LineToElement:
			vArrayPos.push_back(GetPos(i));
			break;
		case CHmsPainterPath::CurveToElement:
		{
			CHmsBezier b = CHmsBezier::fromPoints(GetPos(i - 1),
				GetPos(i),
				GetPos(i + 1),
				GetPos(i + 2));
			auto bounds = b.bounds();
			// threshold based on same algorithm as in qtriangulatingstroker.cpp
			int threshold = HMS_MIN(64, (int)(HMS_MAX(bounds.size.width, bounds.size.height) * 3.14f / (curveInverseScale * 6)));
			if (threshold < 3) threshold = 3;
			float one_over_threshold_minus_1 = float(1) / (threshold - 1);
			for (int t = 0; t < threshold; ++t) {
				auto pt = b.pointAt(t * one_over_threshold_minus_1);
				vArrayPos.push_back(pt);
			}
			i += 2;
		}
		break;
		case CHmsPainterPath::CurveToDataElement:
			break;
		default:
			break;
		}
	}

	DrawShape(vArrayPos, color);
}

void CHmsShapeNode::DrawAndFillPainterPath(const CHmsPainterPath & path, float fLineWidth, const Color4B & color /*= Color4B::WHITE*/, const Color4B & colorFill /*= Color4B::GREEN*/)
{
	auto vE = path.GetPathElements();
	GLfloat curveInverseScale = 1.0f;

	std::vector<Vec2>	vArrayPos;
	vArrayPos.reserve(100);

	auto GetPos = [=](int nPos)
	{
		auto & e = vE->at(nPos);
		return Vec2(float(e.x), float(e.y));
	};

	for (unsigned int i = 0; i < vE->size(); i++)
	{
		switch (vE->at(i).type)
		{
		case CHmsPainterPath::MoveToElement:
			vArrayPos.push_back(GetPos(i));
			break;
		case CHmsPainterPath::LineToElement:
			vArrayPos.push_back(GetPos(i));
			break;
		case CHmsPainterPath::CurveToElement:
		{
			CHmsBezier b = CHmsBezier::fromPoints(GetPos(i - 1),
				GetPos(i),
				GetPos(i + 1),
				GetPos(i + 2));
			auto bounds = b.bounds();
			// threshold based on same algorithm as in qtriangulatingstroker.cpp
			int threshold = HMS_MIN(64, (int)(HMS_MAX(bounds.size.width, bounds.size.height) * 3.14f / (curveInverseScale * 6)));
			if (threshold < 3) threshold = 3;
			float one_over_threshold_minus_1 = float(1) / (threshold - 1);
			for (int t = 0; t < threshold; ++t) {
				auto pt = b.pointAt(t * one_over_threshold_minus_1);
				vArrayPos.push_back(pt);
			}
			i += 2;
		}
		break;
		case CHmsPainterPath::CurveToDataElement:
			break;
		default:
			break;
		}
	}

	DrawShape(vArrayPos, colorFill);
	DrawLineStrip(vArrayPos.data(), vArrayPos.size(), fLineWidth, color);
}

NS_HMS_END