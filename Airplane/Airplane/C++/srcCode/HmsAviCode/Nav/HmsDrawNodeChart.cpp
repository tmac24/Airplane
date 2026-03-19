#include "HmsDrawNodeChart.h"

HmsAviPf::CHmsDrawNodeChart::CHmsDrawNodeChart()
: m_lowWorldPixel(0)
, m_highWorldPixel(0)
, m_uniformChanged(false)
, m_beginIndex(0)
, m_endIndex(0)
, m_beginPos(0)
, m_count(0)
#if _NAV_SVS_LOGIC_JUDGE
, m_u_fHightColorPos(0)
, m_u_fLowColorPos(0)
#endif
{

}

HmsAviPf::CHmsDrawNodeChart::~CHmsDrawNodeChart()
{

}

bool HmsAviPf::CHmsDrawNodeChart::Init()
{
	if (!CHmsDrawNode::Init())
	{
		return false;
	}
	auto glProgram = HmsAviPf::GLProgram::createWithFilenames("Shaders/zs_profile.vsh", "Shaders/zs_profile.fsh");
	if (0 == glProgram)
	{
		return false;
	}
	auto state = GLProgramState::getOrCreateWithGLProgram(glProgram);
#if _NAV_SVS_LOGIC_JUDGE
	if (!state)
		return false;
#endif
	m_u_fLowColorPos = glGetUniformLocation(glProgram->getProgram(), "u_fLowColorPos");
	m_u_fHightColorPos = glGetUniformLocation(glProgram->getProgram(), "u_fHightColorPos");

	this->SetGLProgram(glProgram);
	this->SetGLProgramState(state);

	return true;
}

void HmsAviPf::CHmsDrawNodeChart::onDrawShape()
{
	if (m_uniformChanged)
	{
		m_uniformChanged = false;
		auto glProgram = this->GetGLProgram();
#if _NAV_SVS_LOGIC_JUDGE
		if (!glProgram)
			return;
#endif
		glProgram->setUniformLocationWith1f(m_u_fLowColorPos, m_lowWorldPixel);
		glProgram->setUniformLocationWith1f(m_u_fHightColorPos, m_highWorldPixel);
	}
// 	for (auto & fd : m_vFanDraw)
// 	{
// 		switch (fd.drawType)
// 		{
// 		case GL_LINES:
// 		case GL_LINE_LOOP:
// 		case GL_LINE_STRIP:
// 			glLineWidth(fd.fLineWidth);
// 		default:
// 			break;
// 		}
// 		if (m_beginPos + m_count <= fd.nStartPos + fd.nDrawCnt)
// 		{
// 			glDrawArrays(fd.drawType, m_beginPos, m_count);
// 		}
// 		else
// 		{
// 			glDrawArrays(fd.drawType, fd.nStartPos, fd.nDrawCnt);
// 		}
// 	}
	CHmsDrawNode::onDrawShape();
}

void HmsAviPf::CHmsDrawNodeChart::SetLowHighColorAltitude(float lowWorldPixel, float highWorldPixel)
{
	m_lowWorldPixel = lowWorldPixel;
	m_highWorldPixel = highWorldPixel;
	m_uniformChanged = true;
}

void HmsAviPf::CHmsDrawNodeChart::SetBeginEndIndex(int begin, int end)
{
	m_beginIndex = begin;
	m_endIndex = end;

	m_beginPos = m_beginIndex * 2;
	m_count = (m_endIndex - m_beginIndex + 1) * 2;
}




