#include "HmsLonLatDegreeEdit.h"
#include "HmsGlobal.h"

CHmsLonLatDegreeEdit::CHmsLonLatDegreeEdit()
: HmsUiEventInterface(this)
, m_bIsLon(true)
, m_degreeMaxLength(3)
, m_bIsPositive(true)
{

}

CHmsLonLatDegreeEdit::~CHmsLonLatDegreeEdit()
{

}

CHmsLonLatDegreeEdit* CHmsLonLatDegreeEdit::Create(const HmsAviPf::Size & size, bool bIsLon)
{
    CHmsLonLatDegreeEdit *pRet = new CHmsLonLatDegreeEdit();
    if (pRet && pRet->Init(size, bIsLon))
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}

bool CHmsLonLatDegreeEdit::Init(const HmsAviPf::Size & windowSize, bool bIsLon)
{
    if (!CHmsNode::Init())
    {
        return false;
    }
    SetContentSize(windowSize);

    SetLonLatMode(bIsLon);

    m_degreeMark = "\xC2\xB0  ";//为了翻译时不影响其他页面，后面加2个空格
    m_minMark = "'  ";
    m_secondMark = "\"  ";

    auto pBgNode = CHmsStretchImageNode::Create("res/airport/search_bg.png", Size(450, windowSize.height));
    pBgNode->SetAnchorPoint(Vec2(0, 0));
    pBgNode->SetPosition(Vec2(0, 0));
    this->AddChild(pBgNode);

    auto CreateLineEdit = [=](Size s, Vec2 pos, std::string &strMark)
    {
        auto pLineEdit = CHmsLineEidt::Create("");
        pLineEdit->SetAnchorPoint(0, 0);
        pLineEdit->SetSize(s);
        pLineEdit->SetPosition(pos);
        pLineEdit->SetTextColor(Color4B::WHITE);
        {
            auto pLabel = CHmsGlobal::CreateUTF8Label("", 32, true);
            pLineEdit->SetTextLabel(pLabel);
        }
        this->AddChild(pLineEdit);

        CHmsLabel *pLabel = nullptr;
        pLabel = CHmsGlobal::CreateLanguageLabel(strMark.c_str(), 32, Color4F::WHITE, true);
        pLabel->SetAnchorPoint(Vec2(0, 0.5));
        pLabel->SetPosition(Vec2(pos.x + s.width, pos.y + s.height * 0.5));
        this->AddChild(pLabel);

        return pLineEdit;
    };

    m_pLineEditDegree = CreateLineEdit(Size(110, 95), Vec2(0, 0), m_degreeMark);
    m_pLineEditMin = CreateLineEdit(Size(90, 95), Vec2(160, 0), m_minMark);
    m_pLineEditSecond = CreateLineEdit(Size(90, 95), Vec2(300, 0), m_secondMark);

    auto CreateButton = [=](Size s, Vec2 pos, std::string str)
    {
        str = " " + str + " ";//为了翻译时不影响其他页面，加空格
        auto pItem = CHmsUiStretchButton::Create("res/BasicFrame/Round4SolidWhite.png", s, Color3B(0x1e, 0x37, 0x4f), Color3B::GRAY);
        pItem->AddText(str.c_str(), 32, Color4B::WHITE, true);
        pItem->SetAnchorPoint(Vec2(0, 0));
        pItem->SetPosition(pos);
        this->AddChild(pItem);
        return pItem;
    };

    auto btnSize = Size(150, 95);
    m_pPositiveBtn = CreateButton(btnSize, Vec2(480, 0), m_positiveMark);
    m_pPositiveBtn->SetOnClickedFunc([=](CHmsUiButtonAbstract * btn)
    {
        m_bIsPositive = true;
        m_pPositiveBtn->Selected();
        m_pNegativeBtn->UnSelected();
    });

    m_pNegativeBtn = CreateButton(btnSize, Vec2(640, 0), m_negativeMark);
    m_pNegativeBtn->SetOnClickedFunc([=](CHmsUiButtonAbstract * btn)
    {
        m_bIsPositive = false;
        m_pNegativeBtn->Selected();
        m_pPositiveBtn->UnSelected();
    });

    m_pPositiveBtn->Selected();

    this->RegisterAllNodeChild();

    return true;
}

void CHmsLonLatDegreeEdit::SetLonLatMode(bool bIsLon)
{
    m_bIsLon = bIsLon;

    if (bIsLon)
    {
        m_degreeMaxLength = 3;
        m_positiveMark = "E";
        m_negativeMark = "W";
    }
    else
    {
        m_degreeMaxLength = 2;
        m_positiveMark = "N";
        m_negativeMark = "S";
    }
}

void CHmsLonLatDegreeEdit::AddChar(char c)
{
    if (c < '0' || c > '9')
    {
        return;
    }
    if (m_degree.length() < m_degreeMaxLength)
    {
        m_degree += c;
    }
    else
    {
        if (m_min.length() < 2)
        {
            m_min += c;
        }
        else
        {
            if (m_second.length() < 2)
            {
                m_second += c;
            }
        }
    }

    Focus();
    UpdateLineEdit();
}

void CHmsLonLatDegreeEdit::Backspace()
{
    if (m_second.length() > 0)
    {
        m_second.pop_back();
    }
    else
    {
        if (m_min.length() > 0)
        {
            m_min.pop_back();
        }
        else
        {
            if (m_degree.length() > 0)
            {
                m_degree.pop_back();
            }
        }
    }

    Focus();
    UpdateLineEdit();
}

bool CHmsLonLatDegreeEdit::IsRightResult()
{
    if (m_degree.length() == m_degreeMaxLength && m_min.length() == 2 && m_second.length() == 2)
    {
        int degree = std::stoi(m_degree);
        int min = std::stoi(m_min);
        int second = std::stoi(m_second);

        if (m_bIsLon)
        {
            if ((degree <= 179 && min <= 59 && second <= 59) || (degree == 180 && min == 0 && second == 0))
            {
                return true;
            }
        }
        else
        {
            if ((degree <= 89 && min <= 59 && second <= 59) || (degree == 90 && min == 0 && second == 0))
            {
                return true;
            }
        }
    }
    return false;
}

double CHmsLonLatDegreeEdit::GetLonLat()
{
    if (IsRightResult())
    {
        double degree = std::stoi(m_degree);
        double min = std::stoi(m_min);
        double second = std::stoi(m_second);

        double ret = degree + min / 60.0 + second / 3600.0;

        if (!m_bIsPositive)
        {
            ret = ret * -1;
        }
        return ret;
    }
    else
    {
        return 0;
    }
}

std::string CHmsLonLatDegreeEdit::GetFormatString()
{
    if (IsRightResult())
    {
        std::string ret = m_degree + "_" + m_min + "_" + m_second;
        if (m_bIsPositive)
        {
            ret += m_positiveMark;
        }
        else
        {
            ret += m_negativeMark;
        }
        return ret;
    }
    else
    {
        if (m_bIsLon)
        {
            return "000_00_00" + m_positiveMark;
        }
        else
        {
            return "00_00_00" + m_positiveMark;
        }
    }
}

void CHmsLonLatDegreeEdit::ClearText()
{
    m_degree = "";
    m_min = "";
    m_second = "";

    UpdateLineEdit();
}

void CHmsLonLatDegreeEdit::UpdateLineEdit()
{
    m_pLineEditDegree->SetText(m_degree);
    m_pLineEditMin->SetText(m_min);
    m_pLineEditSecond->SetText(m_second);
}

void CHmsLonLatDegreeEdit::Focus()
{
    Blur();

    if (m_degree.length() < m_degreeMaxLength)
    {
        m_pLineEditDegree->Focus();
    }
    else
    {
        if (m_min.length() < 2)
        {
            m_pLineEditMin->Focus();
        }
        else
        {
            m_pLineEditSecond->Focus();
        }
    }
}

void CHmsLonLatDegreeEdit::Blur()
{
    m_pLineEditDegree->Blur();
    m_pLineEditMin->Blur();
    m_pLineEditSecond->Blur();
}
