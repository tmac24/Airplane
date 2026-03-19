#include "HmsWarningColorStyle.h"



warningColorStyle::warningColorStyle()
{

}

warningColorStyle::~warningColorStyle()
{
    _altThresholds.clear();
    _colors.clear();
}

//************************************
// Method:    create
// FullName:  warningColorStyle::create
// Access:    public static 
// Desc:      
// Returns:   warningColorStyle::warningColorStyle*
// Qualifier:
// Parameter: const std::vector<float> & altThresholds,  altitude threshold
// Parameter: const std::vector<HmsAviPf::Vec4> & colors,  colors in every band divided by the altitude threshold
// Author:    yan.jiang
//************************************
warningColorStyle* warningColorStyle::create(const std::vector<float>& altThresholds, const std::vector<HmsAviPf::Vec4>& colors)
{
    auto pColorstyle = new warningColorStyle();
    auto ret = pColorstyle->init(altThresholds, colors);
    if (false == ret)
    {
        delete pColorstyle;
        return nullptr;
    }
    return pColorstyle;
}

bool warningColorStyle::init(const std::vector<float>& altThresholds, const std::vector<HmsAviPf::Vec4>& colors)
{
    _thresholdCnt = altThresholds.size();
    if (_thresholdCnt + 1 != colors.size())
    {
        return false;
    }

    for (auto& t : altThresholds)
    {
        _altThresholds.push_back(t);
    }

    for (auto& c : colors)
    {
        _colors.push_back(c);
    }

    return true;
}
