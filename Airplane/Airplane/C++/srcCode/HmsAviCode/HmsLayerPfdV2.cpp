#include "HmsLayerPfdV2.h"
#include "HmsGlobal.h"
#include "../HmsGE/display/HmsDrawNode.h"
#include "PfdV2/HmsAirspeedV2.h"
#include "PfdV2/HmsAltmeterV2.h"
#include "PfdV2/HmsVerticalV2.h"
#include "PfdV2/HmsAttitudeV2.h"
#include "PfdV2/HmsHSIV2.h"
#include "DataInterface/HmsDataBus.h"
#include "ui/HmsUiImagePanel.h"
#include "Calculate/CalculateZs.h"

USING_NS_HMS;

CHmsLayerPFDV2::CHmsLayerPFDV2()
	: HmsUiEventInterface(this)
{
	
}

CHmsLayerPFDV2::~CHmsLayerPFDV2()
{
}

bool CHmsLayerPFDV2::Init(const HmsAviPf::Size & size)
{
	if (!CHmsResizeableLayer::Init())
	{
		return false;
	}

	SetContentSize(size);

	auto panel = CHmsUiPanel::Create(Size(2560, 1600));
	panel->SetSwallowEvent(false);
	m_nodeRoot = panel;

	m_nodeRoot->SetAnchorPoint(Vec2(0.5f, 0.5f));

	m_nodeRoot->SetPosition(size / 2.0f);
	this->AddChild(m_nodeRoot);
	
	m_pAirspeed = CHmsAirspeedV2::Create(Size(144, 486));
	m_pAirspeed->SetPosition(Vec2(736, 717));
	m_nodeRoot->AddChild(m_pAirspeed);

	m_pAltmeter = CHmsAltmeterV2::Create(Size(144, 486));
	m_pAltmeter->SetPosition(Vec2(1635, 717));
	m_nodeRoot->AddChild(m_pAltmeter);

	m_pVertical = CHmsVerticalV2::Create(Size(98, 329));
	m_pVertical->SetPosition(Vec2(1789, 796));
	m_nodeRoot->AddChild(m_pVertical);

	m_pAttitude = CHmsAttitudeV2::Create(Size(540, 486));
	m_pAttitude->SetAnchorPoint(0.5, 0.5);
	m_pAttitude->SetPosition(Vec2(1278, 960));
	m_nodeRoot->AddChild(m_pAttitude);

	m_pHSI = CHmsHSIV2::Create(Size(480, 480));
	m_pHSI->SetAnchorPoint(0.5, 0.5);
	m_pHSI->SetPosition(Vec2(1278, 350));
	m_nodeRoot->AddChild(m_pHSI);

	panel->RegisterAllNodeChild();

	RegisterAllNodeChild();
	this->SetScheduleUpdate();
	return true;
}


void CHmsLayerPFDV2::Update(float delta)
{
	CHmsDataBus *dataBus = CHmsGlobal::GetInstance()->GetDataBus();
	CHmsDuControlData *pDUCtrl = CHmsGlobal::GetInstance()->GetDataBus()->GetDuControlData();
	if (0 == m_pAirspeed || 0 == m_pAltmeter || 0 == m_pHSI ||
		0 == m_pAttitude || 0 == m_pVertical || 0 == dataBus)
	{
		return;
	}

	auto aftData = dataBus->GetData();

	bool bValid = (aftData->bDeviceConnected) ? false : true;
	if (aftData->bDeviceConnected)
	{
		m_pAttitude->ValidSwitch(bValid);
		m_pHSI->ValidSwitch(bValid);

		bValid = !aftData->bLatLonValid;
		do
		{
			m_pAirspeed->ValidSwitch(bValid);
			m_pAltmeter->ValidSwitch(bValid);
			m_pVertical->ValidSwitch(bValid);
		} while (0);
	}
	else
	{
	m_pAirspeed->ValidSwitch(bValid);
	m_pAltmeter->ValidSwitch(bValid);
	m_pVertical->ValidSwitch(bValid);
	m_pAttitude->ValidSwitch(bValid);
	m_pHSI->ValidSwitch(bValid);
	}

	m_pAirspeed->SetValue(aftData->groundSpeed*KnotKMH);
	m_pAltmeter->SetValue(dataBus->GetMslAltitudeMeter());
	m_pAttitude->SetPitch(aftData->pitch);
	m_pAttitude->SetRoll(aftData->roll);
	m_pVertical->SetValue(FeetToMeter(aftData->verticalSpeed));

	FmsCalData fmsData = dataBus->GetFmsData();
	CHmsDuControlData *pDuCtrl = dataBus->GetDuControlData();
	m_pHSI->SetOffsetRatio(fmsData.cdiRatio);
	//m_pHSI->SetHeadType(false);
	//m_pHSI->SetTrackType(false);
	do 
	{
		m_pHSI->CheckPfdCompassMode(pDUCtrl->GetPfdCompassDataMode());

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
		m_pHSI->SetHeading(fHeading);
	} while (0);
	
	if (pDuCtrl && !pDuCtrl->IsObsMode() && fmsData.changeCourse){
		m_pHSI->SetVor(fmsData.course);
	}else if (pDuCtrl && pDuCtrl->IsObsMode()){
		m_pHSI->SetVor(dataBus->GetDuControlData()->GetCurCourse());
	}

	if (pDuCtrl){
		m_pHSI->SetSelectHead(pDuCtrl->GetSelectHeading());
		m_pHSI->SetOBSMode(pDuCtrl->IsObsMode());
	}

	bool bIsMetric = dataBus->GetDuControlData()->IsMetricEnable();
	m_pAirspeed->MetricSwitch(bIsMetric);
	m_pAltmeter->MetricSwitch(bIsMetric);
}

void CHmsLayerPFDV2::ResetLayerSize(const HmsAviPf::Size & size, const HmsAviPf::Vec2 & pos)
{
	if (m_nodeRoot)
	{
		m_nodeRoot->SetPosition(size / 2.0f);
	}

    if (size.height < 1300)
    {
        m_nodeRoot->SetScale(0.85f);
    }
    else
    {
        m_nodeRoot->SetScale(1.0f);
    }
}