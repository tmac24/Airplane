#include "HmsLayerHud.h"
#include "HmsGlobal.h"
#include "DataInterface/HmsDataBus.h"
USING_NS_HMS;

CHmsLayerHud::CHmsLayerHud()
: HmsUiEventInterface(this)
, m_pHudAttitude(0)
{
    m_pHudAttitude = nullptr;
    m_pHudAltmeter = nullptr;
    m_pHudAirspeed = nullptr;
    m_pHudHeading = nullptr;
    m_pOverload = nullptr;
    m_drawInvalid = nullptr;
}

CHmsLayerHud::~CHmsLayerHud()
{
}

CHmsLabel*  CHmsLayerHud::GenOverload()
{
    CHmsLabel *pOverload = CHmsLabel::createWithTTF("+1.0", "fonts/msyhbd.ttc", 25.0f);
    if (pOverload)
    {
        pOverload->SetColor(Color3B::GREEN);
    }

    CHmsLabel *pSignMark = CHmsLabel::createWithTTF("G", "fonts/msyhbd.ttc", 25.0f);
    if (pSignMark)
    {
        pSignMark->SetAnchorPoint(Vec2(1.0f, 0.0f));
        pSignMark->SetPosition(-10.0f, 0.0f);
        pSignMark->SetColor(Color3B::GREEN);
        if (pOverload)
        {
            pOverload->AddChild(pSignMark);
        }
    }

    return pOverload;
}

void CHmsLayerHud::InitInvalidDraw(const HmsAviPf::Size & size)
{
    m_drawInvalid = CHmsMultLineDrawNode::Create();
    float fWide = size.width / 2.0f;
    float fHigh = size.height / 2.0f;

    Vec2  vertVec[4];
    vertVec[0].setPoint(-fWide, fHigh);
    vertVec[1].setPoint(fWide, -fHigh);
    vertVec[2].setPoint(-fWide, -fHigh);
    vertVec[3].setPoint(fWide, fHigh);
    m_drawInvalid->DrawLines(vertVec, 4, 4.0f, Color4B::RED);
    m_drawInvalid->SetDrawColor(Color4F::RED);
    m_drawInvalid->SetPosition(Vec2(0, 0));
    if (m_rootContent)
    {
        m_rootContent->AddChild(m_drawInvalid);
    }
}

bool CHmsLayerHud::Init(const HmsAviPf::Size & sizeInit)
{
    if (!CHmsResizeableLayer::Init())
    {
        return false;
    }
    SetContentSize(sizeInit);

    auto size = sizeInit;
    size.width -= 10;
    size.height -= 10;

    m_rootContent = CHmsNode::Create();
    m_rootContent->SetPosition(GetUiCenter());
    this->AddChild(m_rootContent);

    Vec2 centerPos = { 0, 0 };

    /*态势.*/
#if 0
    m_pHudAttitude = CHmsHudAttitude::Create(Size(500, 500));
    m_pHudAttitude->SetAnchorPoint(Vec2(0.5, 0.5f));
    m_pHudAttitude->SetPosition(centerPos);
    //m_pHudAttitude->setScale(0.9f);
    m_rootContent->AddChild(m_pHudAttitude);
#else
    m_pHudAttitude = CHmsHudAttitudeFullDraw::Create(Size(500, 500));
    m_pHudAttitude->SetAnchorPoint(Vec2(0.5, 0.5f));
    m_pHudAttitude->SetPosition(centerPos);
    //m_pHudAttitude->setScale(0.9f);
    m_rootContent->AddChild(m_pHudAttitude);
#endif

    /*高度表.*/
    m_pHudAltmeter = CHmsHudAltMeter::Create(Size(50.0f, 580));
    m_pHudAltmeter->SetAnchorPoint(Vec2(0.0, 0.5f));
    m_pHudAltmeter->SetPosition(centerPos + Vec2(380, 0));
    m_rootContent->AddChild(m_pHudAltmeter);

    /*空速表.*/
    m_pHudAirspeed = CHmsHudAirspeed::Create(Size(50.0f, 580));
    m_pHudAirspeed->SetAnchorPoint(Vec2(0.0, 0.5f));
    m_pHudAirspeed->SetPosition(centerPos + Vec2(-380, 0));
    m_rootContent->AddChild(m_pHudAirspeed);

    /*偏航角.*/
    m_pHudHeading = CHmsHudHeading::Create(Size(366.0f, 30.0f));
    m_pHudHeading->SetAnchorPoint(Vec2(0.5, 0.0f));
    m_pHudHeading->SetPosition(centerPos + Vec2(0.0f, 340.0f));
    m_rootContent->AddChild(m_pHudHeading);

    /*过载量.*/
    m_pOverload = GenOverload();
    m_pOverload->SetAnchorPoint(Vec2(0.0, 0.0f));
    m_pOverload->SetPosition(centerPos + Vec2(-380.0f, 340.0f));
    m_rootContent->AddChild(m_pOverload);

    InitInvalidDraw(Size(380 * 2, 340 * 2));

    //RegisterTouchEvent();
    RegisterAllNodeChild();

    SetScheduleUpdate();
    return true;
}
#define KnotKMH				1.852
void CHmsLayerHud::Update(float delta)
{
    CHmsDataBus *dataBus = CHmsGlobal::GetInstance()->GetDataBus();
    CHmsDuControlData *pDUCtrl = CHmsGlobal::GetInstance()->GetDataBus()->GetDuControlData();
    if (0 == m_pHudAirspeed || 0 == m_pHudAltmeter ||
        0 == m_pHudAttitude || 0 == m_pHudHeading || 0 == dataBus)
    {
        return;
    }
    //TODO

    auto aftData = dataBus->GetData();

    if (aftData->bDeviceConnected)
    {
        m_drawInvalid->SetVisible(false);
        if (dataBus->GetData()->bLatLonValid)
        {
            m_pHudAirspeed->SetUnValid(1);
            m_pHudAltmeter->SetUnValid(1);
        }
        else
        {
            m_pHudAirspeed->SetUnValid(0);
            m_pHudAltmeter->SetUnValid(0);
        }
    }
    else
    {
        m_drawInvalid->SetVisible(true);
        m_pHudAirspeed->SetUnValid(0);
        m_pHudAltmeter->SetUnValid(0);
    }


    m_pHudAirspeed->SetAirSpeed(aftData->groundSpeed*KnotKMH);
    m_pHudAltmeter->SetAltMeter(dataBus->GetMslAltitudeMeter());
    m_pHudAttitude->SetHudPitch(aftData->pitch);
    m_pHudAttitude->SetRoll(aftData->roll);
    //m_pHudHeading->SetHeadingType(false);
    //m_pHudHeading->SetTrackType(false);
    do
    {
        m_pHudHeading->CheckPfdCompassMode(pDUCtrl->GetPfdCompassDataMode());

        float fHeading = 0.0f;
        switch (pDUCtrl->GetPfdCompassDataMode())
        {
        case PFD_COMPASS_DATA_MODE::magneticHeading:
            fHeading = (aftData->magneticHeading);
            break;
        case PFD_COMPASS_DATA_MODE::trueTrack:
            fHeading = (aftData->trueTrack);
            break;
        case PFD_COMPASS_DATA_MODE::trueHeading:
            fHeading = (aftData->trueHeading);
            break;
        case PFD_COMPASS_DATA_MODE::magneticTrack:
            //TODO
            break;
        default:
            break;
        }
        m_pHudHeading->SetHudHeading(fHeading);
    } while (0);

    if (m_pOverload)
    {
        CHmsGlobal::SetLabelFormat(m_pOverload, "%.2f", aftData->GLode);
    }

    bool bIsMetric = dataBus->GetDuControlData()->IsMetricEnable();
    m_pHudAirspeed->MetricSwitch(bIsMetric);
    m_pHudAltmeter->MetricSwitch(bIsMetric);
}

void CHmsLayerHud::ResetLayerSize(const HmsAviPf::Size & size, const HmsAviPf::Vec2 & pos)
{
    CHmsResizeableLayer::ResetLayerSize(size, pos);

    if (m_rootContent)
    {
        m_rootContent->SetPosition(GetUiCenter());
    }
}
