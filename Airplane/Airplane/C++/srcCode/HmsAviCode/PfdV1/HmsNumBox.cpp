#include "HmsNumBox.h"
#include "base/HmsClippingNode.h"
USING_NS_HMS;

CHmsNumBox::CHmsNumBox()
:HmsUiEventInterface(this)
, m_boxWidth(70)
, m_boxHeight(26)
, m_boxColor(0, 0, 0, 60)
, m_scaleLineWidth(10.0f)
, m_scaleGap_px(26.0f)
, m_scaleValueStep(10.0f)
, m_curValue(0.0f)
, m_minValue(0.0f)
, m_maxValue(500.0f)
, m_valueFormat("%03d")
, m_scrollIncreament(0.1f)
, OnValueChanged(nullptr)
, m_isPressed(false)
{
}


CHmsNumBox::~CHmsNumBox()
{
}

CHmsNumBox* CHmsNumBox::Create(float width, float height, float minValue, float maxValue, float valueStep)
{
    CHmsNumBox *pRet = new(std::nothrow) CHmsNumBox();
    if (pRet && pRet->init(width, height, minValue, maxValue, valueStep))
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

bool CHmsNumBox::init(float width, float height, float minValue, float maxValue, float valueStep)
{
    if (!CHmsNode::Init())
    {
        return false;
    }
    m_boxWidth = width;
    m_boxHeight = height;
    m_minValue = minValue;
    m_maxValue = maxValue;
    m_scaleValueStep = valueStep;
    this->SetContentSize(Size(width, height));

    //当前值
    auto CurValueBox = CHmsDrawNode::Create();
    CurValueBox->SetPosition(0, 0);
    {
        Color4B selSpeedTxtColor(0xFF, 0, 0xCC, 0xFF);//洋红色

        CurValueBox->DrawSolidRect(0, 0, width, height, Color4F(m_boxColor));

        m_pCurValueLabel = CHmsGlobal::CreateLabel("000", 22);
        if (m_pCurValueLabel)
        {
            m_pCurValueLabel->SetTextColor(selSpeedTxtColor);
            m_pCurValueLabel->SetAnchorPoint(Vec2(0.5, 0.5));
            m_pCurValueLabel->SetPosition(width / 2.0, height / 2.0);

            CurValueBox->AddChild(m_pCurValueLabel);
        }
    }

    float scaleHeight = 140;
    float scaleWidth = 66;
    //刻度尺
    m_pScrollBoxNode = CHmsDrawNode::Create();
    auto scaleBoxClipNode = CHmsClippingNode::create();
    m_pScrollBoxNode->SetPosition(width + 10, height / 2.0f);
    {
        m_pScrollBoxNode->DrawSolidRect(0, scaleHeight / 2.0f, scaleWidth, -scaleHeight / 2.0f, Color4F(m_boxColor));

        auto stencilNode = CHmsDrawNode::Create();
        stencilNode->DrawSolidRect(0, scaleHeight / 2.0f, scaleWidth, -scaleHeight / 2.0f, Color4F::WHITE);
        scaleBoxClipNode->setStencil(stencilNode);

        //刻度条
        m_pScaleBar = CHmsNode::Create();

        m_pScaleLines = CHmsDrawNode::Create();
        //刻度条上刻度线，从最小值位置初始化15条刻度线
        for (int i = 0; i < 15; i++)
        {
            m_pScaleLines->DrawHorizontalLine(0, m_scaleLineWidth, i * m_scaleGap_px);
        }
        m_pScaleBar->AddChild(m_pScaleLines, -1);

        //初始化10个刻度值
        for (int i = 0; i < 10; i++)
        {
            int value = int(i * 2 * m_scaleValueStep);
            std::string scaleText = CHmsGlobal::FormatString("%d", value);
            auto label = CHmsGlobal::CreateLabel(scaleText.c_str(), 16);
            if (label)
            {
                label->SetAnchorPoint(Vec2(0, 0.5f));
                label->SetTextColor(Color4B::WHITE);
                label->SetPosition(m_scaleLineWidth + 2, i * 2 * m_scaleGap_px);
                m_pScaleBar->AddChild(label);
                m_labelMap[value] = label;
            }
        }

        scaleBoxClipNode->AddChild(m_pScaleBar);
        m_pScrollBoxNode->AddChild(scaleBoxClipNode);
    }
    auto boxArrow = CHmsImageNode::Create("res/Pfd/boxArrow.png");
    if (boxArrow)
    {
        boxArrow->SetAnchorPoint(Vec2(0, 0.5));
        boxArrow->SetColor(Color3B::GREEN);
        m_pScrollBoxNode->AddChild(boxArrow);
    }

    m_pScrollBoxNode->SetVisible(false);
    this->AddChild(m_pScrollBoxNode);
    this->AddChild(CurValueBox);
    //RegisterTouchEvent(1);
    return true;
}

void CHmsNumBox::SetValueFormat(const std::string format)
{
    m_valueFormat = format;
    std::string scaleText = CHmsGlobal::FormatString(m_valueFormat.c_str(), m_curValue);
    if (m_pCurValueLabel)
    {
        m_pCurValueLabel->SetString(scaleText);
    }
}

void CHmsNumBox::Update(float delta)
{
    if (m_isPressed && m_timeLast.GetTimeInterval() > 0.5)
    {
        if (!m_pScrollBoxNode->IsVisible())
        {
            m_pScrollBoxNode->SetVisible(true);
        }
        if (m_pCurValueLabel)
        {
            m_pCurValueLabel->SetTextColor(Color4B(0x00, 0xFF, 0xFF, 0xFF));
        }

        float temp = m_curValue + m_scrollIncreament;
        temp = temp < m_minValue ? m_minValue : temp;
        temp = temp > m_maxValue ? m_maxValue : temp;
        ScrollScaleBar(temp);

        m_curValue = temp;
        std::string valueText = CHmsGlobal::FormatString(m_valueFormat.c_str(), (int)m_curValue);
        if (m_pCurValueLabel)
        {
            m_pCurValueLabel->SetString(valueText);
        }

        if (OnValueChanged)
        {
            OnValueChanged(int(m_curValue));
        }

        printLog("m_curSpeed=%f\n", m_curValue);
    }
}

bool CHmsNumBox::OnPressed(const Vec2 & posOrigin)
{
    m_timeLast.Start();
    m_isPressed = true;
    m_scrollIncreament = 0.0f;
    this->SetScheduleUpdate();
    return true;
}

void CHmsNumBox::OnReleased(const Vec2 & posOrigin)
{
    m_isPressed = false;
    if (m_pCurValueLabel)
    {
        m_pCurValueLabel->SetTextColor(Color4B(0xFF, 0, 0xCC, 0xFF));
    }
    m_pScrollBoxNode->SetVisible(false);
    this->SetUnSchedule();
}

void CHmsNumBox::OnMove(const Vec2 & posOrigin)
{
    float interval = posOrigin.y - m_boxHeight / 2.0f;
    float isNegative = interval <= 0;
    float absInterval = fabs(interval);
    if (absInterval > 0 && absInterval <= 10)
    {
        m_scrollIncreament = 0.1f;
    }
    else if (absInterval > 10 && absInterval <= 40)
    {
        m_scrollIncreament = 0.01 * m_scaleValueStep;
    }
    else if (absInterval > 40 && absInterval <= 70)
    {
        m_scrollIncreament = 0.1* m_scaleValueStep;
    }
    else if (absInterval > 70)
    {
        m_scrollIncreament = m_scaleValueStep;
    }

    if (isNegative)
    {
        m_scrollIncreament = -m_scrollIncreament;
    }
}

void CHmsNumBox::printLog(const char * strFormat, ...)
{
    va_list _ArgList;
    va_start(_ArgList, strFormat);
    char strTemp[1024] = { 0 };
    vsprintf(strTemp, strFormat, _ArgList);
    CCLOG("%s\n", strTemp);
}

void CHmsNumBox::ScrollScaleBar(const float &fValue)
{
    float scrollPx = -ValueToPosY(fValue);
    m_pScaleBar->SetPositionY(scrollPx);

    bool downToUp = fValue - m_curValue < 0;
    RefreshScaleLinePos(downToUp);
    RefreshLabelPos(downToUp, fValue);
}

void CHmsNumBox::RefreshScaleLinePos(bool downToUp)
{
    float barY = m_pScaleBar->GetPoistionY();
    float lineY = m_pScaleLines->GetPoistionY();

    float newPosY = 0;
    if (downToUp)
    {
        if (lineY > 0 && barY + lineY > -m_scaleGap_px * 8.0f)
        {
            newPosY = lineY - m_scaleGap_px * 1;
        }
        else
        {
            return;
        }
    }
    else
    {
        if (barY + lineY < -m_scaleGap_px * 8)
        {
            newPosY = lineY + m_scaleGap_px * 1;
        }
        else
        {
            return;
        }
    }

    m_pScaleLines->SetPositionY(newPosY);
    RefreshScaleLinePos(downToUp);
}

void CHmsNumBox::RefreshLabelPos(bool downToUp, const float &fCurSpeed)
{
    std::map<int, HmsAviPf::CHmsLabel*>::iterator beginItem = m_labelMap.begin();
    std::map<int, HmsAviPf::CHmsLabel*>::iterator endItem = m_labelMap.end();
    endItem--;

    int beginValue = beginItem->first;
    HmsAviPf::CHmsLabel* beginLabel = beginItem->second;

    int endValue = endItem->first;
    HmsAviPf::CHmsLabel* endLabel = endItem->second;

    int removeKey = 0;
    int newKey = 0;
    std::string newScaleStr = "";
    HmsAviPf::CHmsLabel* targetLabel = nullptr;
    float newPosY = 0;
    if (downToUp)
    {
        if (beginValue != 0 && fCurSpeed - endValue < -7 * m_scaleValueStep / 2)
        {
            removeKey = endValue;
            newKey = removeKey - m_scaleValueStep * 2 * 10;
            newScaleStr = CHmsGlobal::FormatString("%d", newKey);
            newPosY = endLabel->GetPoistionY() - m_scaleGap_px * 2 * 10;
            targetLabel = endLabel;
        }
        else
        {
            return;
        }
    }
    else
    {
        if (/*endValue != (int)m_maxSpeed &&*/ fCurSpeed - beginValue > 7 * m_scaleValueStep / 2)
        {
            removeKey = beginValue;
            newKey = removeKey + m_scaleValueStep * 2 * 10;
            newScaleStr = CHmsGlobal::FormatString("%d", newKey);
            newPosY = beginLabel->GetPoistionY() + m_scaleGap_px * 2 * 10;
            targetLabel = beginLabel;
        }
        else
        {
            return;
        }
    }

    m_labelMap.erase(removeKey);
    m_labelMap[newKey] = targetLabel;
    targetLabel->SetString(newScaleStr);
    targetLabel->SetPositionY(newPosY);
    RefreshLabelPos(downToUp, fCurSpeed);
}

float CHmsNumBox::ValueToPosY(const float &fSpeed)
{
    float unit = m_scaleGap_px / m_scaleValueStep;
    return fSpeed * unit;
}
