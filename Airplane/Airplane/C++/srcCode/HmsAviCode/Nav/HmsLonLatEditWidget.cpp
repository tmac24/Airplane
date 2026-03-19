#include "HmsLonLatEditWidget.h"
#include "HmsNavComm.h"
#include "HmsNavFplLayer.h"

CHmsLonLatEditWidget::CHmsLonLatEditWidget()
: HmsUiEventInterface(this)
{

}

CHmsLonLatEditWidget::~CHmsLonLatEditWidget()
{

}

bool CHmsLonLatEditWidget::Init()
{
    if (!CHmsNode::Init())
    {
        return false;
    }

    Size windowSize = Size(800, 200);
    SetContentSize(windowSize);

    m_pLonDegreeEdit = CHmsLonLatDegreeEdit::Create(Size(800, 95), true);
    m_pLonDegreeEdit->SetAnchorPoint(0, 0);
    m_pLonDegreeEdit->SetPosition(Vec2(0, 105));
    this->AddChild(m_pLonDegreeEdit);

    m_pLatDegreeEdit = CHmsLonLatDegreeEdit::Create(Size(800, 95), false);
    m_pLatDegreeEdit->SetAnchorPoint(0, 0);
    m_pLatDegreeEdit->SetPosition(Vec2(0, 0));
    this->AddChild(m_pLatDegreeEdit);

    this->RegisterAllNodeChild();

    return true;
}

void CHmsLonLatEditWidget::Blur()
{
    m_pLonDegreeEdit->Blur();
    m_pLatDegreeEdit->Blur();
}

void CHmsLonLatEditWidget::ClearText()
{
    m_pLonDegreeEdit->ClearText();
    m_pLatDegreeEdit->ClearText();
}

bool CHmsLonLatEditWidget::IsRightResult()
{
    if (m_pLonDegreeEdit->IsRightResult() && m_pLatDegreeEdit->IsRightResult())
    {
        return true;
    }
    return false;
}

std::string CHmsLonLatEditWidget::GetFormatString()
{
    std::string strLon = m_pLonDegreeEdit->GetFormatString();
    std::string strLat = m_pLatDegreeEdit->GetFormatString();
    std::string ret = strLon + "/" + strLat;
    
    return ret;
}

bool CHmsLonLatEditWidget::OnPressed(const Vec2 & posOrigin)
{
    auto DetectLonLatEditEvent = [=](CHmsLonLatDegreeEdit *pLonLatEdit)
    {
        auto rect = pLonLatEdit->GetRectFromParent();
        if (pLonLatEdit->IsVisible() && rect.containsPoint(posOrigin))
        {
            pLonLatEdit->Focus();

            if (CHmsNavComm::GetInstance()->GetCHmsNavFplLayer())
            {
                CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->ShowOrHideKeyboard(true,
                    [=](char c){

                    if ('\b' == c)
                    {
                        pLonLatEdit->Backspace();
                    }
                    else
                    {
                        pLonLatEdit->AddChar(c);
                    }

                }, HMS_CALLBACK_0(CHmsLonLatEditWidget::Blur, this));
            }
            return true;
        }
        else
        {
            return false;
        }
    };


    Blur();

    if (!DetectLonLatEditEvent(m_pLonDegreeEdit))
    {
        if (!DetectLonLatEditEvent(m_pLatDegreeEdit))
        {
            return false;
        }
    }

    return HmsUiEventInterface::OnPressed(posOrigin);
}
