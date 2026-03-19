#include "HmsInputAltitude.h"
#include "Language/HmsLanguageMananger.h"
#include "HmsFrame2DRoot.h"
#include "HmsNavFplLayer.h"
#include "HmsNavComm.h"

CHmsInputAltitude::CHmsInputAltitude()
: HmsUiEventInterface(this)
, m_callbackFunc(nullptr)
{

}

CHmsInputAltitude::~CHmsInputAltitude()
{

}

bool CHmsInputAltitude::Init(const HmsAviPf::Size & windowSize)
{
    if (!CHmsNode::Init())
    {
        return false;
    }
    SetContentSize(windowSize);

    auto pBgNode = CHmsDrawNode::Create();
#if _NAV_SVS_LOGIC_JUDGE
	if (!pBgNode)
		return false;
#endif
    pBgNode->SetAnchorPoint(Vec2(0, 0));
    pBgNode->SetPosition(0, 0);
    this->AddChild(pBgNode);
    pBgNode->DrawSolidRect(Rect(Vec2(0, 0), windowSize), Color4F(Color3B(0x0e, 0x19, 0x25)));
    //画边框
    {
        std::vector<Vec2> vec;
        vec.push_back(Vec2(0, 0));
        vec.push_back(Vec2(windowSize.width, 0));
        vec.push_back(Vec2(windowSize.width, windowSize.height));
        vec.push_back(Vec2(0, windowSize.height));
        vec.push_back(Vec2(0, 0));
        pBgNode->DrawLineLoop(vec, 1, Color4B(0x2a, 0x4c, 0x6e, 0xff));
    }

    auto CreateButton = [=](Size s, Vec2 pos, const char * text)
    {
        auto pItem = CHmsUiStretchButton::Create("res/BasicFrame/Round4SolidWhite.png", s, Color3B(0x1e, 0x37, 0x4f), Color3B::GRAY);
#if _NAV_SVS_LOGIC_JUDGE
		if (pItem)
#endif
		{
			pItem->AddText(text, 32, Color4B::WHITE, true);
			pItem->SetAnchorPoint(Vec2(0, 0));
			pItem->SetPosition(pos);
			this->AddChild(pItem);
		}
        return pItem;
    };

    auto btnSize = Size(260, 95);
    float topPos = windowSize.height - btnSize.height - 10;
    CHmsUiStretchButton *pItem = nullptr;
    m_pBtnBlow = CreateButton(btnSize, Vec2(btnSize.width * 0 + 10, topPos), "Below");
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pBtnBlow)
		return false;
#endif
    m_pBtnBlow->SetToggleBtn(true);
    m_pBtnBlow->SetOnClickedFunc(std::bind(&CHmsInputAltitude::OnBtnClick, this, BUTTON_BLOW));
    m_pBtnEqual = CreateButton(btnSize, Vec2(btnSize.width * 1 + 10, topPos), "Equal");
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pBtnEqual)
		return false;
#endif
    m_pBtnEqual->SetToggleBtn(true);
    m_pBtnEqual->SetOnClickedFunc(std::bind(&CHmsInputAltitude::OnBtnClick, this, BUTTON_EQUAL));
    m_pBtnAbove = CreateButton(btnSize, Vec2(btnSize.width * 2 + 10, topPos), "Above");
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pBtnAbove)
		return false;
#endif
    m_pBtnAbove->SetToggleBtn(true);
    m_pBtnAbove->SetOnClickedFunc(std::bind(&CHmsInputAltitude::OnBtnClick, this, BUTTON_ABOVE));
    m_pBtnBetween = CreateButton(btnSize, Vec2(btnSize.width * 3 + 10, topPos), "Between");
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pBtnBetween)
		return false;
#endif
    m_pBtnBetween->SetToggleBtn(true);
    m_pBtnBetween->SetOnClickedFunc(std::bind(&CHmsInputAltitude::OnBtnClick, this, BUTTON_BETWEEN));

    m_pLineEditLeft = CHmsLineEidt::Create("res/airport/search_bg.png");
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pLineEditLeft)
		return false;
#endif
    m_pLineEditLeft->SetAnchorPoint(0, 0);
    m_pLineEditLeft->SetSize(Size((windowSize.width - btnSize.width * 4 - 40) * 0.5, btnSize.height));
    m_pLineEditLeft->SetPosition(btnSize.width * 4 + 20, topPos);
    m_pLineEditLeft->SetTextColor(Color4B::WHITE);
    this->AddChild(m_pLineEditLeft);
    {
        auto pLabel = CHmsGlobal::CreateUTF8Label("", 32, true);
        m_pLineEditLeft->SetTextLabel(pLabel);
    }

    m_pLineEditRight = CHmsLineEidt::Create("res/airport/search_bg.png");
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pLineEditRight)
		return false;
#endif
    m_pLineEditRight->SetAnchorPoint(1, 0);
    m_pLineEditRight->SetSize(Size((windowSize.width - btnSize.width * 4 - 40) * 0.5, btnSize.height));
    m_pLineEditRight->SetPosition(windowSize.width - 10, topPos);
    m_pLineEditRight->SetTextColor(Color4B::WHITE);
    this->AddChild(m_pLineEditRight);
    {
        auto pLabel = CHmsGlobal::CreateUTF8Label("", 32, true);
        m_pLineEditRight->SetTextLabel(pLabel);
    }

    btnSize = Size(260, 95);
    pItem = CreateButton(btnSize, Vec2(windowSize.width * 0.5 - btnSize.width - 10, 10), "Enter");
#if _NAV_SVS_LOGIC_JUDGE
	if (!pItem)
		return false;
#endif
    pItem->SetTextColor(Color4B::GREEN);
    pItem->SetOnClickedFunc([=](CHmsUiButtonAbstract* btn)
    {
        CloseDlg();
        GetAltitudeStu();
        if (m_callbackFunc)
        {
            m_callbackFunc(m_altStu);
        }
    });
    pItem = CreateButton(btnSize, Vec2(windowSize.width * 0.5 + 10, 10), "Cancel");
#if _NAV_SVS_LOGIC_JUDGE
	if (!pItem)
		return false;
#endif
    pItem->SetTextColor(Color4B::GRAY);
    pItem->SetOnClickedFunc([=](CHmsUiButtonAbstract* btn)
    {
        CloseDlg();
    });

    this->RegisterAllNodeChild();

    return true;
}

void CHmsInputAltitude::LineEditBlur()
{
    if (m_pLineEditLeft)
    {
        m_pLineEditLeft->Blur();
    }
    if (m_pLineEditRight)
    {
        m_pLineEditRight->Blur();
    }
}

void CHmsInputAltitude::ShowDlg(const FmcAltitude *pAltStu, std::function<void(FmcAltitude altStu[2])> func)
{
    this->SetVisible(true);
    m_altStu[0] = pAltStu[0];
    m_altStu[1] = pAltStu[1];
    m_callbackFunc = func;
    UpdateLineEdit();
}

void CHmsInputAltitude::CloseDlg()
{
    LineEditBlur();
    this->SetVisible(false);
    if (CHmsNavComm::GetInstance()->GetCHmsNavFplLayer())
    {
        CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->ShowOrHideKeyboard(false,
            HMS_CALLBACK_1(CHmsInputAltitude::OnKeyboardCallbackLeft, this),
            HMS_CALLBACK_0(CHmsInputAltitude::LineEditBlur, this));
    }
}

bool CHmsInputAltitude::OnPressed(const Vec2 & posOrigin)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pLineEditLeft || !m_pLineEditRight)
		return false;
#endif
    LineEditBlur();

    auto rect = m_pLineEditLeft->GetRectFromParent();
    if (rect.containsPoint(posOrigin))
    {
        m_pLineEditLeft->Focus();

        if (CHmsNavComm::GetInstance()->GetCHmsNavFplLayer())
        {
            CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->ShowOrHideKeyboard(true,
                HMS_CALLBACK_1(CHmsInputAltitude::OnKeyboardCallbackLeft, this),
                HMS_CALLBACK_0(CHmsInputAltitude::LineEditBlur, this));
        }
    }
    else
    {
        rect = m_pLineEditRight->GetRectFromParent();
        if (rect.containsPoint(posOrigin))
        {
            m_pLineEditRight->Focus();

            if (CHmsNavComm::GetInstance()->GetCHmsNavFplLayer())
            {
                CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->ShowOrHideKeyboard(true,
                    HMS_CALLBACK_1(CHmsInputAltitude::OnKeyboardCallbackRight, this),
                    HMS_CALLBACK_0(CHmsInputAltitude::LineEditBlur, this));
            }
        }
        else
        {
            HmsUiEventInterface::OnPressed(posOrigin);
        }
    }

    return true;
}

void CHmsInputAltitude::OnKeyboardCallbackLeft(char c)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pLineEditLeft)
		return;
#endif
    auto str = m_pLineEditLeft->GetText();
    if ('\b' == c)
    {
        if (!str.empty())
        {
            str.pop_back();
        }
    }
    else
    {
        str.push_back(c);
    }
    m_pLineEditLeft->SetText(str);
}

void CHmsInputAltitude::OnKeyboardCallbackRight(char c)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pLineEditRight)
		return;
#endif
    auto str = m_pLineEditRight->GetText();
    if ('\b' == c)
    {
        if (!str.empty())
        {
            str.pop_back();
        }
    }
    else
    {
        str.push_back(c);
    }
    m_pLineEditRight->SetText(str);
}

void CHmsInputAltitude::OnBtnClick(ButtonType type)
{
    SelectBtn(type);
}

void CHmsInputAltitude::SelectBtn(ButtonType type)
{
	if (m_pBtnBlow && m_pBtnBlow->IsSelected())
    {
        m_pBtnBlow->Selected();
    }
	if (m_pBtnEqual && m_pBtnEqual->IsSelected())
    {
        m_pBtnEqual->Selected();
    }
	if (m_pBtnAbove && m_pBtnAbove->IsSelected())
    {
        m_pBtnAbove->Selected();
    }
	if (m_pBtnBetween&& m_pBtnBetween->IsSelected())
    {
        m_pBtnBetween->Selected();
    }
	if (m_pLineEditLeft)
		m_pLineEditLeft->SetVisible(false);
	if (m_pLineEditRight)
		m_pLineEditRight->SetVisible(false);
    if (type == BUTTON_BLOW)
    {
		if (m_pBtnBlow)
			m_pBtnBlow->Selected();
		if (m_pLineEditLeft)
			m_pLineEditLeft->SetVisible(true);
	}
	else if (type == BUTTON_EQUAL)
	{
		if (m_pBtnEqual)
			m_pBtnEqual->Selected();
		if (m_pLineEditLeft)
			m_pLineEditLeft->SetVisible(true);
	}
	else if (type == BUTTON_ABOVE)
	{
		if (m_pBtnAbove)
			m_pBtnAbove->Selected();
		if (m_pLineEditLeft)
			m_pLineEditLeft->SetVisible(true);
	}
	else
	{
		if (m_pBtnBetween)
			m_pBtnBetween->Selected();
		if (m_pLineEditLeft)
			m_pLineEditLeft->SetVisible(true);
		if (m_pLineEditRight)
			m_pLineEditRight->SetVisible(true);
    }
}

CHmsInputAltitude::ButtonType CHmsInputAltitude::GetButtonSelectState()
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pBtnBlow || !m_pBtnEqual || !m_pBtnAbove)
		return BUTTON_BETWEEN;
#endif
    if (m_pBtnBlow->IsSelected())
    {
        return BUTTON_BLOW;
    }
    else if (m_pBtnEqual->IsSelected())
    {
        return BUTTON_EQUAL;
    }
    else if (m_pBtnAbove->IsSelected())
    {
        return BUTTON_ABOVE;
    }
    else
    {
        return BUTTON_BETWEEN;
    }
}

void CHmsInputAltitude::UpdateLineEdit()
{
    auto GetValue = [](int value)
    {
        if (CHmsNavComm::GetInstance()->IsMetric())
        {
            double tmp = value;
            tmp = floor(FeetToMeter(tmp));
            return (int)tmp;
        }
        else
        {
            return value;
        }
    };

#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pLineEditLeft || !m_pLineEditRight)
		return;
#endif

	SelectBtn(BUTTON_EQUAL);
	m_pLineEditLeft->SetText("");
	m_pLineEditRight->SetText("");

    if (m_altStu[0].nAltitude != -1 && m_altStu[1].nAltitude != -1)
    {
        SelectBtn(BUTTON_BETWEEN);
        std::stringstream ss;
        ss << GetValue(m_altStu[1].nAltitude);
        m_pLineEditLeft->SetText(ss.str());

        ss.str("");
        ss << GetValue(m_altStu[0].nAltitude);
        m_pLineEditRight->SetText(ss.str());
    }
    else if (m_altStu[0].nAltitude != -1)
    {
        if (m_altStu[0].uLimit.bitType.bBelow)
        {
            SelectBtn(BUTTON_BLOW);
        }
        else if (m_altStu[0].uLimit.bitType.bAbove)
        {
            SelectBtn(BUTTON_ABOVE);
        }
        else
        {
            SelectBtn(BUTTON_EQUAL);
        }
        std::stringstream ss;
        ss << GetValue(m_altStu[0].nAltitude);
        m_pLineEditLeft->SetText(ss.str());
    }
}

void CHmsInputAltitude::GetAltitudeStu()
{
    auto GetValue = [](const std::string &str)
    {
        if (CHmsNavComm::GetInstance()->IsMetric())
        {
            double tmp = atoi(str.c_str());
            tmp = ceil(MeterToFeet(tmp));
            return (int)tmp;
        }
        else
        {
            return atoi(str.c_str());
        }
    };
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pLineEditLeft || !m_pLineEditRight)
		return;
#endif
    FmcAltitude altStu[2];
    auto type = GetButtonSelectState();
    auto strLeft = m_pLineEditLeft->GetText();
    auto strRight = m_pLineEditRight->GetText();
    if (type == BUTTON_BLOW)
    {
        if (strLeft.size() > 0)
        {
            altStu[0].uLimit.bitType.bBelow = 1;
            altStu[0].nAltitude = GetValue(strLeft);
        }
    }
    else if (type == BUTTON_EQUAL)
    {
        if (strLeft.size() > 0)
        {
            altStu[0].uLimit.bitType.bAt = 1;
            altStu[0].nAltitude = GetValue(strLeft);
        }
    }
    else if (type == BUTTON_ABOVE)
    {
        if (strLeft.size() > 0)
        {
            altStu[0].uLimit.bitType.bAbove = 1;
            altStu[0].nAltitude = GetValue(strLeft);
        }
    }
    else
    {
        if (strLeft.size() > 0 && strRight.size() > 0)
        {
            altStu[1].uLimit.bitType.bAbove = 1;
            altStu[1].nAltitude = GetValue(strLeft);

            altStu[0].uLimit.bitType.bBelow = 1;
            altStu[0].nAltitude = GetValue(strRight);
        }
    }
    m_altStu[0] = altStu[0];
    m_altStu[1] = altStu[1];
}




