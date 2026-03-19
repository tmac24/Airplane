#include "HmsLayerPfdV1.h"
#include "Calculate/CalculateZs.h"
#include "HmsGlobal.h"
#include "DataInterface/HmsDataBus.h"
#include "base/HmsClippingNode.h"
#include "ui/HmsUiImagePanel.h"
USING_NS_HMS;

CHmsLayerPfdV1::CHmsLayerPfdV1()
: HmsUiEventInterface(this)
, m_fSkyInitHeight(300.0f)
{
}


CHmsLayerPfdV1::~CHmsLayerPfdV1()
{
}

bool CHmsLayerPfdV1::Init(const HmsAviPf::Size & size)
{
    if (!CHmsResizeableLayer::Init())
    {
        return false;
    }

    this->SetContentSize(size);

    auto panel = CHmsUiPanel::Create(size);
    panel->SetSwallowEvent(false);
    panel->SetAnchorPoint(0.5f, 0.5f);
    //m_nodePfdRoot = CHmsNode::Create();
    m_nodePfdRoot = panel;
    m_nodePfdRoot->SetPosition(GetUiCenter() + Vec2(0, 0));
    m_nodePfdRoot->SetScale(1.65f);
    this->AddChild(m_nodePfdRoot);

    if (1) //Init the pfd node
    {
        auto fSidePosOffset = 315;

        Vec2 basicPos(size.width / 2.0f, size.height / 2.0f + 440);

        m_pNodeAirspeed = CHmsAirspeed::Create();
        auto halfAirspeedHeight = m_pNodeAirspeed->GetContentSize().height / 2.0f;
        m_pNodeAirspeed->SetPosition(basicPos + Vec2(-fSidePosOffset, -m_fSkyInitHeight + halfAirspeedHeight + 10));
        std::vector<CHmsAirspeed::AirspeedMarking> list;
        list.push_back(CHmsAirspeed::AirspeedMarking(0, 64, Color4B::RED));
        list.push_back(CHmsAirspeed::AirspeedMarking(64, 125, Color4B::WHITE));
        list.push_back(CHmsAirspeed::AirspeedMarking(88, 194, Color4B::GREEN));
        list.push_back(CHmsAirspeed::AirspeedMarking(194, 265, Color4B::YELLOW));
        list.push_back(CHmsAirspeed::AirspeedMarking(265, 999, Color4B::RED));
        //m_pNodeAirspeed->InitAirspeedMarking(list);

        m_pNodeAltimeter = CHmsAltimeter::Create();
        auto halfAltimterHeight = m_pNodeAltimeter->GetContentSize().height / 2.0f;
        m_pNodeAltimeter->SetPosition(basicPos + Vec2(fSidePosOffset - 80, -m_fSkyInitHeight + halfAltimterHeight + 10));

        m_pNodeAttitude = CHmsAttitude::Create();
        m_pNodeAttitude->SetPosition(basicPos + Vec2(0, -m_fSkyInitHeight - 40));
        auto attitudeWidth = m_pNodeAttitude->GetContentSize().width;
        auto attitudeHeight = m_pNodeAttitude->GetContentSize().height;

        m_pNodeVSpeed = CHmsVSpeed::Create();
        m_pNodeVSpeed->SetPosition(basicPos + Vec2(fSidePosOffset + 40, -m_fSkyInitHeight));

        m_pNodeHSI = CHmsHSI::Create();
        m_pNodeHSI->SetPosition(basicPos + Vec2(0, -680));
        Size hsiContent = m_pNodeHSI->GetContentSize();
        m_pNavInfoLeft = CHmsNavInfoLeft::Create();
        m_pNavInfoLeft->SetPosition(m_pNodeHSI->GetPoistion() + Vec2(-hsiContent.width / 2.8f, -hsiContent.height / 2.4f));
        m_pNavInfoLeft->SetNavSrc("ADF");
        m_pNavInfoLeft->SetPointerIconPath("res/Pfd/HSI/symbol_adf.png");
        m_pNavInfoLeft->SetPointerIconColor(Color3B(0xFF, 0x66, 0x00));
        m_pNavInfoLeft->SetInfo1("RB   264");
        m_pNavInfoLeft->SetInfo2("QDM  264");

        m_pNavInfoRight = CHmsNavInfoRight::Create();
        m_pNavInfoRight->SetPosition(m_pNodeHSI->GetPoistion() + Vec2(hsiContent.width / 2.8f, -hsiContent.height / 2.4f));
        m_pNavInfoRight->SetNavSrc("NAV1");
        m_pNavInfoRight->SetPointerIconPath("res/Pfd/HSI/symbol_nav.png");
        m_pNavInfoRight->SetPointerIconColor(Color3B(0xFF, 0x00, 0xFF));
        m_pNavInfoRight->SetInfo1("330  JPM");
        m_pNavInfoRight->SetInfo2("9.9 nm");

        m_pHsiBoxHdg = CHmsHSIBox::Create();
        m_pHsiBoxHdg->SetPosition(m_pNodeHSI->GetPoistion() + Vec2(-hsiContent.width / 2.4f, hsiContent.height / 2.0f));
        m_pHsiBoxHdg->SetTitle("HDG");

        m_pHsiBoxCrs = CHmsHSIBox::Create();
        m_pHsiBoxCrs->SetPosition(m_pNodeHSI->GetPoistion() + Vec2(hsiContent.width / 2.4f, hsiContent.height / 2.0f));
        m_pHsiBoxCrs->SetTitle("CRS");
        //m_pHsiBoxCrs->OnValueChanged = CC_CALLBACK_1(CHmsLayerPfd::OnCourseChanged, this);

        m_nodePfdRoot->AddChild(m_pNodeAttitude);
        m_nodePfdRoot->AddChild(m_pNodeAirspeed);
        m_nodePfdRoot->AddChild(m_pNodeVSpeed);
        m_nodePfdRoot->AddChild(m_pNodeAltimeter);
        m_nodePfdRoot->AddChild(m_pNodeHSI);
    }

    panel->RegisterAllNodeChild();
    RegisterAllNodeChild();
    SetScheduleUpdate();

    return true;
}

void CHmsLayerPfdV1::Update(float delta)
{
    CHmsDataBus *dataBus = CHmsGlobal::GetInstance()->GetDataBus();
    HmsAftData * aftData = dataBus->GetData();
    bool bIsMetric = dataBus->GetDuControlData()->IsMetricEnable();

    if (aftData->bDeviceConnected)
    {
        if (aftData->bLatLonValid)
        {
            m_pNodeAirspeed->SetGaugeMode(CHmsGauge::GaugeMode::GAUGE_MODE_OK);
            m_pNodeAltimeter->SetGaugeMode(CHmsGauge::GaugeMode::GAUGE_MODE_OK);
            m_pNodeVSpeed->SetGaugeMode(CHmsGauge::GaugeMode::GAUGE_MODE_OK);
        }
        else
        {
            m_pNodeAirspeed->SetGaugeMode(CHmsGauge::GaugeMode::GAUGE_MODE_ERROR);
            m_pNodeAltimeter->SetGaugeMode(CHmsGauge::GaugeMode::GAUGE_MODE_ERROR);
            m_pNodeVSpeed->SetGaugeMode(CHmsGauge::GaugeMode::GAUGE_MODE_ERROR);
        }

        m_pNodeAttitude->SetGaugeMode(CHmsGauge::GaugeMode::GAUGE_MODE_OK);
        m_pNodeHSI->SetGaugeMode(CHmsGauge::GaugeMode::GAUGE_MODE_OK);
    }
    else
    {
        m_pNodeAttitude->SetGaugeMode(CHmsGauge::GaugeMode::GAUGE_MODE_ERROR);
        m_pNodeAirspeed->SetGaugeMode(CHmsGauge::GaugeMode::GAUGE_MODE_ERROR);
        m_pNodeAltimeter->SetGaugeMode(CHmsGauge::GaugeMode::GAUGE_MODE_ERROR);
        m_pNodeVSpeed->SetGaugeMode(CHmsGauge::GaugeMode::GAUGE_MODE_ERROR);
        m_pNodeHSI->SetGaugeMode(CHmsGauge::GaugeMode::GAUGE_MODE_ERROR);
    }

    m_pNodeAirspeed->MetricSwitch(bIsMetric);
    m_pNodeAltimeter->MetricSwitch(bIsMetric);

    //auto aftData = dataBus->GetData();

#if 0
    CHmsDataBus *dataBus = CHmsGlobal::GetInstance()->GetDataBus();
    InsGnssData insGnss = dataBus->GetInsGnssData();
    AdcData adcData = dataBus->GetAdcData();
    DmeData dmeData = dataBus->GetDmeData();
    VorilsData vorils = dataBus->GetVorilsData();
    AdfData adfData = dataBus->GetAdfData();
    AircraftInstrumentData *aircraft = dataBus->GetAircraftInstrumentData();

    m_pGaugeAoa->SetGaugeMode((CHmsGauge::GaugeMode)aircraft->aoaError);
    m_pGaugeAoa->SetValue(aircraft->aoa);

    m_pGaugeOat->SetGaugeMode((CHmsGauge::GaugeMode)aircraft->oatError);
    m_pGaugeOat->SetValue(aircraft->oat);

    m_pNodeRadioAlt->SetGaugeMode((CHmsGauge::GaugeMode)aircraft->radioHeightError);
    m_pNodeRadioAlt->SetValue(aircraft->radioHeight);

    m_pNodeAttitude->SetGaugeMode((CHmsGauge::GaugeMode)aircraft->attitudeError);
    m_pNodeAirspeed->SetGaugeMode((CHmsGauge::GaugeMode)aircraft->airspeedError);
    m_pNodeAltimeter->SetGaugeMode((CHmsGauge::GaugeMode)aircraft->altimeterError);
    m_pNodeVSpeed->SetGaugeMode((CHmsGauge::GaugeMode)aircraft->verticalSpeedError);
    m_pNodeHSI->SetGaugeMode((CHmsGauge::GaugeMode)aircraft->hsiError);

    //更新指点信标数据
    m_pNodeMBI->SetBeaconValid(aircraft->omFlag, aircraft->mmFlag, aircraft->imFlag);

    //更新nav方位数据
    if (aircraft->hasVor && aircraft->vorBearingValid)
    {
        int qdm = aircraft->vorQDR + 180;
        if (qdm > 360)
        {
            qdm = qdm - 360;
        }
        std::string info1 = CHmsGlobal::FormatString("%d\302\260  %s", qdm, vorils.mmrCode.c_str());
        std::string info2 = CHmsGlobal::FormatString("%.2f  nm", dmeData.distance);
        m_pNavInfoRight->SetInfo1(info1);
        m_pNavInfoRight->SetInfo2(info2);
    }
    else
    {
        m_pNavInfoRight->SetNoData();
    }

    //更新ADF方位数据
    if (aircraft->hasAdf && aircraft->adfBearingValid)
    {
        int rb = aircraft->adfRB;
        int qdm = (int)(rb + aircraft->magneticHeading);
        if (qdm > 360)
        {
            qdm = qdm - 360;
        }
        std::string info1 = CHmsGlobal::FormatString("RB     %d\302\260", rb);
        std::string info2 = CHmsGlobal::FormatString("QDM %d\302\260", qdm);
        m_pNavInfoLeft->SetInfo1(info1);
        m_pNavInfoLeft->SetInfo2(info2);
    }
    else
    {
        m_pNavInfoLeft->SetNoData();
    }

    //更新风速风向数据
    if (aircraft->windError == GaugeFailFlag::GAUGE_OK)
    {
        m_pNodeWind->SetData(int(aircraft->windDirection), int(aircraft->windSpeed));
    }
    else
    {
        m_pNodeWind->SetNoData();
    }

    m_pNodeGlideslope->SetVisible(aircraft->hasGlideslope);
#endif
}

void CHmsLayerPfdV1::SetVisible(bool visible)
{
    CHmsResizeableLayer::SetVisible(visible);
    if (m_pNodeHSI)
        m_pNodeHSI->EnableUi(visible);
}

void CHmsLayerPfdV1::ResetLayerSize(const HmsAviPf::Size & size, const HmsAviPf::Vec2 & pos)
{
    CHmsResizeableLayer::ResetLayerSize(size, pos);

    if (size.height < 1300)
    {
        m_nodePfdRoot->SetScale(1.45f);
    }
    else
    {
        m_nodePfdRoot->SetScale(1.65f);
    }

    if (m_nodePfdRoot)
    {
        m_nodePfdRoot->SetPosition(GetUiCenter() + Vec2(0, 0));
    }
}

void CHmsLayerPfdV1::InitBackground()
{
    Color4B skyColor(0, 0x65, 0xCC, 0xFF);
    Color4B groundColor(0x65, 0x33, 0, 0xFF);

    auto size = this->GetContentSize();

    auto clipNode = CHmsClippingNode::create();
    auto stencilNode = CHmsDrawNode::Create();
    stencilNode->DrawSolidRect(2, -2, size.width - 2, -size.width + 2, Color4F::RED);
    clipNode->setStencil(stencilNode);
    clipNode->SetPosition(GetUiTopLeft());

    float sideLength = size.height + size.height;

    m_pNodeSkyGroundRotation = CHmsNode::Create();
    m_pNodeSkyGroundRotation->SetPosition(size.width / 2.0f, -m_fSkyInitHeight);

    m_pNodeSkyGround = CHmsDrawNode::Create();

    m_pNodeSkyGround->DrawSolidRect(-sideLength / 2.0f, 0, sideLength / 2.0f, sideLength, Color4F(skyColor));
    m_pNodeSkyGround->DrawSolidRect(-sideLength / 2.0f, -sideLength, sideLength / 2.0f, 0, Color4F(groundColor));

    m_pNodeSkyGroundRotation->AddChild(m_pNodeSkyGround);
    clipNode->AddChild(m_pNodeSkyGroundRotation);
    this->AddChild(clipNode);
}
#if 0
void CHmsLayerPfd::OnPitchChanged(float pitch_px)
{
    m_pNodeSkyGround->SetPositionY(pitch_px);
}

void CHmsLayerPfd::OnRollChanged(float roll_angle)
{
    m_pNodeSkyGroundRotation->setRotation(roll_angle);
}

void CHmsLayerPfd::OnCourseChanged(int course)
{
    m_pHsiBoxCrs->SetValue(course);
    m_pNodeHSI->SetCourse(course);

    SettingData* settingData = CHmsGlobal::GetInstance()->GetDataBus()->GetSettingData();
    settingData->dataValid.selectCourse1Valid = 0x01;
    settingData->selectCourse1 = m_pHsiBoxCrs->GetValue();
    SendSettingData(settingData);
}

void CHmsLayerPfd::OnSelHeadingChanged(int heading)
{
    m_pHsiBoxHdg->SetValue(heading);
    m_pNodeHSI->SetSelHdg(heading);

    SettingData* settingData = CHmsGlobal::GetInstance()->GetDataBus()->GetSettingData();
    settingData->dataValid.selectHeadingValid = 0x01;
    settingData->selectHeading = m_pHsiBoxHdg->GetValue();
    SendSettingData(settingData);
}

void CHmsLayerPfd::OnNavSrcChanged(CHmsHSI::NAV_SRC navSrc)
{
    m_pNodeGlideslope->SetVisible(navSrc == CHmsHSI::NAV_LOC);

    SettingData *settingData = CHmsGlobal::GetInstance()->GetDataBus()->GetSettingData();
    settingData->flightPhase = navSrc == CHmsHSI::NAV_LOC ? 0x02 : 0x01;
    SendSettingData(settingData);
}

void CHmsLayerPfd::OnMetricSelectedChanged(bool bSelected)
{
    m_pNodeAltimeter->MetricSwitch(bSelected);

    SettingData *settingData = CHmsGlobal::GetInstance()->GetDataBus()->GetSettingData();
    settingData->metricState = bSelected ? 2 : 1;
    SendSettingData(settingData);
}

void CHmsLayerPfd::OnHPASelectedChanged(bool bSelected)
{
    if (bSelected)
        m_pNodeAltimeter->SetBaroUnit(CHmsAltimeter::UNIT_HPA);
    else
        m_pNodeAltimeter->SetBaroUnit(CHmsAltimeter::UNIT_INHG);

    SettingData *settingData = CHmsGlobal::GetInstance()->GetDataBus()->GetSettingData();
    settingData->baroUnit = bSelected ? 2 : 1;
    SendSettingData(settingData);
}

void CHmsLayerPfd::OnStdClicked(bool bSelected)
{
    m_pNodeAltimeter->SetBarometric(29.92f);
    m_pNodeAltimeter->SetBaroRef(CHmsAltimeter::REF_STD);

    BaroSettingData *baroSettingData = CHmsGlobal::GetInstance()->GetDataBus()->GetBaroSettringData();
    baroSettingData->baroRef = 0x01;
    baroSettingData->baroSet = 29.92f;
    baroSettingData->dataValid = 0x01;
    SendBaroSettingData(baroSettingData);

    SettingData *settingData = CHmsGlobal::GetInstance()->GetDataBus()->GetSettingData();
    settingData->dataValid.baroSetValid = 0x01;
    settingData->baroRef = 0x01;
    settingData->baroSet = 29.92f;
    SendSettingData(settingData);
}

void CHmsLayerPfd::OnKnobSelHeadingInnerChanged(float fValue)
{
    int value = m_pHsiBoxHdg->GetValue();
    value = fValue > 0 ? value + 1 : value - 1;
    OnSelHeadingChanged(value);
}

void CHmsLayerPfd::OnKnobSelHeadingOuterChanged(float fValue)
{
    int value = m_pHsiBoxHdg->GetValue();
    value = fValue > 0 ? value + 10 : value - 10;
    OnSelHeadingChanged(value);
}

void CHmsLayerPfd::OnKnobSelCourseInnerChanged(float fValue)
{
    int value = m_pHsiBoxCrs->GetValue();
    value = fValue > 0 ? value + 1 : value - 1;
    OnCourseChanged(value);
}

void CHmsLayerPfd::OnKnobSelCourseOuterChanged(float fValue)
{
    int value = m_pHsiBoxCrs->GetValue();
    value = fValue > 0 ? value + 10 : value - 10;
    OnCourseChanged(value);
}

void CHmsLayerPfd::OnKnobSelAltitudeInnerChanged(float fValue)
{
    float value = m_pNodeAltimeter->GetSelectedAlt();
    value = fValue > 0 ? value + 10 : value - 10;
    m_pNodeAltimeter->SetSelectedAlt(value);

    SettingData *settingData = CHmsGlobal::GetInstance()->GetDataBus()->GetSettingData();
    settingData->dataValid.selectHeightValid = 0x01;
    settingData->selectHeight = FeetToMeter(m_pNodeAltimeter->GetSelectedAlt());
    SendSettingData(settingData);
}

void CHmsLayerPfd::OnKnobSelAltitudeOuterChanged(float fValue)
{
    float value = m_pNodeAltimeter->GetSelectedAlt();
    value = fValue > 0 ? value + 100 : value - 100;
    m_pNodeAltimeter->SetSelectedAlt(value);

    SettingData *settingData = CHmsGlobal::GetInstance()->GetDataBus()->GetSettingData();
    settingData->dataValid.selectHeightValid = 0x01;
    settingData->selectHeight = FeetToMeter(m_pNodeAltimeter->GetSelectedAlt());
    SendSettingData(settingData);
}

void CHmsLayerPfd::OnKnobSetBaroInnerChanged(float fValue)
{
    float value = m_pNodeAltimeter->GetBarometric();
    value = fValue > 0 ? value + 0.01 : value - 0.01;
    m_pNodeAltimeter->SetBarometric(value);

    BaroSettingData *baroSettingData = CHmsGlobal::GetInstance()->GetDataBus()->GetBaroSettringData();
    baroSettingData->baroSet = m_pNodeAltimeter->GetBarometric();
    baroSettingData->dataValid = 0x01;
    SendBaroSettingData(baroSettingData);

    SettingData *settingData = CHmsGlobal::GetInstance()->GetDataBus()->GetSettingData();
    settingData->dataValid.baroSetValid = 0x01;
    settingData->baroSet = m_pNodeAltimeter->GetBarometric();
    SendSettingData(settingData);
}

void CHmsLayerPfd::OnKnobSetBaroOuterChanged(float fValue)
{
    float value = m_pNodeAltimeter->GetBarometric();
    value = fValue > 0 ? value + 0.1 : value - 0.1;
    m_pNodeAltimeter->SetBarometric(value);

    BaroSettingData *baroSettingData = CHmsGlobal::GetInstance()->GetDataBus()->GetBaroSettringData();
    baroSettingData->baroSet = m_pNodeAltimeter->GetBarometric();
    baroSettingData->dataValid = 0x01;
    SendBaroSettingData(baroSettingData);

    SettingData *settingData = CHmsGlobal::GetInstance()->GetDataBus()->GetSettingData();
    settingData->dataValid.baroSetValid = 0x01;
    settingData->baroSet = m_pNodeAltimeter->GetBarometric();
    SendSettingData(settingData);
}

void CHmsLayerPfd::SendBaroSettingData(BaroSettingData* data)
{
    CHmsDataSender *sender = CHmsGlobal::GetInstance()->GetDataSender();
    sender->SendBaroSettingData(data);
}

void CHmsLayerPfd::SendSettingData(SettingData* data)
{
    CHmsDataSender *sender = CHmsGlobal::GetInstance()->GetDataSender();
    sender->SendSettingData(data);
}
#endif
