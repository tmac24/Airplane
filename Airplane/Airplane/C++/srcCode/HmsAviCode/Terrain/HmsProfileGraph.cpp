#include "HmsProfileGraph.h"
#include "../Calculate/CalculateZs.h"
USING_NS_HMS;

#define MAX_NUM_ALT_THRESHOLD 8 //the max slot number in profile's shader

CHmsProfileGraph::CHmsProfileGraph()
: m_uniformChanged(false)
, m_beginIndex(0)
, m_endIndex(0)
, m_beginPos(0)
, m_count(0)
{

}

CHmsProfileGraph::~CHmsProfileGraph()
{

}

bool CHmsProfileGraph::Init()
{
    if (!CHmsDrawNode::Init())
    {
        return false;
    }
    auto glProgram = HmsAviPf::GLProgram::createWithFilenames("Shaders/zs_profileUni.vsh", "Shaders/zs_profileUni.fsh");
    if (0 == glProgram)
    {
        return false;
    }

    auto state = GLProgramState::getOrCreateWithGLProgram(glProgram);

    _u_sizeAltThresholds = glGetUniformLocation(glProgram->getProgram(), "sizeAltThresholds");
    _u_AltThresholdsPixel = glGetUniformLocation(glProgram->getProgram(), "AltThresholdsPixel");
    _u_colorsUni = glGetUniformLocation(glProgram->getProgram(), "colorsUni");

    this->SetGLProgram(glProgram);
    this->SetGLProgramState(state);

    return true;
}

void CHmsProfileGraph::onDrawShape()
{
    if (m_uniformChanged)
    {
        m_uniformChanged = false;
        auto glProgram = this->GetGLProgram();

        auto altThrCnt = _altThresholdsWorldPixel.size();
        auto pa = _altThresholdsWorldPixel.data();
        auto pc = _altWarningColors.data();

        glProgram->setUniformLocationWith1i(_u_sizeAltThresholds, altThrCnt);
        glProgram->setUniformLocationWith1fv(_u_AltThresholdsPixel, pa, altThrCnt);
        glProgram->setUniformLocationWith4fv(_u_colorsUni, (float*)pc, altThrCnt + 1);
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

void CHmsProfileGraph::SetLowHighColorAltitude(const std::vector<float>& altThresholdsWorldPixel, const std::vector<HmsAviPf::Vec4> altWarningColors)
{
    if (altThresholdsWorldPixel.size() > MAX_NUM_ALT_THRESHOLD)
    {
        //will not acceptable by shader.
        return;
    }

    _altThresholdsWorldPixel = altThresholdsWorldPixel;
    _altWarningColors = altWarningColors;
    m_uniformChanged = true;
}

void CHmsProfileGraph::SetBeginEndIndex(int begin, int end)
{
    m_beginIndex = begin;
    m_endIndex = end;

    m_beginPos = m_beginIndex * 2;
    m_count = (m_endIndex - m_beginIndex + 1) * 2;
}

void CHmsProfileGraph::SetShowSize(const HmsAviPf::Size & size)
{
    SetContentSize(size);
}



