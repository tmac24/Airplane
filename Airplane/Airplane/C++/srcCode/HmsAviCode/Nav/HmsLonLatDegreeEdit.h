#pragma once

#include "ui/HmsUiStretchButton.h"
#include "display/HmsLineEdit.h"
#include "display/HmsLabel.h"
#include "display/HmsDrawNode.h"
#include <string>

using namespace HmsAviPf;

class CHmsLonLatDegreeEdit : public HmsAviPf::CHmsNode, public HmsUiEventInterface
{
public:
    CHmsLonLatDegreeEdit();
    ~CHmsLonLatDegreeEdit();

    static CHmsLonLatDegreeEdit* Create(const HmsAviPf::Size & size, bool bIsLon);
    bool Init(const HmsAviPf::Size & windowSize, bool bIsLon);

    void AddChar(char c);
    void Backspace();

    bool IsRightResult();
    double GetLonLat();
    std::string GetFormatString();

    void ClearText();

    void UpdateLineEdit();

    void Focus();
    void Blur();

private:
    void SetLonLatMode(bool bIsLon);

private:
    std::string m_degreeMark;
    std::string m_minMark;
    std::string m_secondMark;

    bool m_bIsLon;
    unsigned int m_degreeMaxLength;//经度3位  纬度2位
    std::string m_degree;
    std::string m_min;
    std::string m_second;

    bool m_bIsPositive;
    std::string m_positiveMark;
    std::string m_negativeMark;

    RefPtr<CHmsLineEidt> m_pLineEditDegree;
    RefPtr<CHmsLineEidt> m_pLineEditMin;
    RefPtr<CHmsLineEidt> m_pLineEditSecond;

    RefPtr<CHmsUiStretchButton> m_pPositiveBtn;
    RefPtr<CHmsUiStretchButton> m_pNegativeBtn;
};