#pragma once
#include <vector>
#include "math/Vec4.h"

class warningColorStyle
{
public:
    virtual ~warningColorStyle();
    static warningColorStyle* create(const std::vector<float>& altThresholds, const std::vector<HmsAviPf::Vec4>& colors);
    const std::vector<float>* AltThresholds() const { return &_altThresholds; }
    const std::vector<HmsAviPf::Vec4>* Colors() const { return &_colors; }
    const int ThresholdCnt() const { return _thresholdCnt; }
protected:
    warningColorStyle();
    bool init(const std::vector<float>& altThresholds, const std::vector<HmsAviPf::Vec4>& colors);
    std::vector<float> _altThresholds;
    std::vector<HmsAviPf::Vec4> _colors;
    int _thresholdCnt;
};