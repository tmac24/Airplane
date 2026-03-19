#pragma once

#include "display/HmsDrawNode.h"
#include "render/HmsGLProgram.h"
#include "render/HmsGLProgramState.h"

class CHmsProfileGraph : public HmsAviPf::CHmsDrawNode
{
public:
    CHmsProfileGraph();
    ~CHmsProfileGraph();

    CREATE_FUNC(CHmsProfileGraph)

    virtual bool Init() override;

    virtual void onDrawShape() override;

    //void SetLowHighColorAltitude(float lowWorldPixel, float highWorldPixel);
    void SetLowHighColorAltitude(const std::vector<float>& altThresholdsWorldPixel, const std::vector<HmsAviPf::Vec4> altWarningColors);
    void SetBeginEndIndex(int begin, int end);

    void SetShowSize(const HmsAviPf::Size & size);

private:

    int _u_sizeAltThresholds;
    int _u_AltThresholdsPixel;
    int _u_colorsUni;

    std::vector<float>          _altThresholdsWorldPixel;
    std::vector<HmsAviPf::Vec4> _altWarningColors;

    bool m_uniformChanged;

    int m_beginIndex;
    int m_endIndex;

    int m_beginPos;
    int m_count;
};