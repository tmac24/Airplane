#include "HmsHSI.h"
#include "HmsHSIManager.h"
#include "Calculate/CalculateZs.h"
#include "../HmsGlobal.h"

USING_NS_HMS;

#define LABEL_SHOW_MAX_TIME 2000  //选择航道最大显示时间 3000毫秒

CHmsHSI::CHmsHSI()
: HmsUiEventInterface(this)
, m_compassRadius(128.0f)
, m_innerCircleRadius(64.0f)
, m_scaleGap(5.0f)
, m_cdiScaleGap_px(23.0f)
, m_selHdgBugColor(0xFF, 0xFF, 0xFF)
, m_turnRateVectorColor(0xFF, 0x00, 0xFF)
, m_navPointerColor(0xFF, 0x00, 0xFF)
, m_adfPointerColor(0xFF, 0x66, 0x00)
, m_curSrc(NAV_GPS)
, m_curToFromMode(NAV_MODE_NONE)
, m_ctrlMode(CTRL_CHANGE_OBS)
, m_ePfdCompassDataMode(PFD_COMPASS_DATA_MODE::magneticHeading)
, m_turnRate(0)
, m_heading(0)
, m_course(0)
, m_selHeading(0)
, m_crsLabelShowTime(0)
, OnCourseChanged(nullptr)
, OnSelHeadingChanged(nullptr)
, OnNavSrcChanged(nullptr)
, m_bDataUnbelievable(false)
, m_bObsMode(true)
, m_bTrueHeading(false)
, m_bTrack(false)
{
	m_pHsiManager = new CHmsHSIManager(this);

	angleTexts[0] = "N";
	angleTexts[1] = "3";
	angleTexts[2] = "6";
	angleTexts[3] = "E";
	angleTexts[4] = "12";
	angleTexts[5] = "15";
	angleTexts[6] = "S";
	angleTexts[7] = "21";
	angleTexts[8] = "24";
	angleTexts[9] = "W";
	angleTexts[10] = "30"; 
	angleTexts[11] = "33";

	NAV_SRC_STR[0] = "GPS";
	NAV_SRC_STR[1] = "ADF";
	NAV_SRC_STR[2] = "VOR";
	NAV_SRC_STR[3] = "LOC";

	m_crsPointerColor[0] = HmsAviPf::Color3B(0x80, 0x00, 0xFF);
	m_crsPointerColor[1] = HmsAviPf::Color3B(0x00, 0x00, 0xFF);
	m_crsPointerColor[2] = HmsAviPf::Color3B(0x00, 0xFF, 0x00);
	m_crsPointerColor[3] = HmsAviPf::Color3B(0x00, 0xFF, 0x00);
}


CHmsHSI::~CHmsHSI()
{
	if (m_pHsiManager)
	{
		delete m_pHsiManager;
		m_pHsiManager = nullptr;
	}
}

bool CHmsHSI::Init()
{
	if (!CHmsGauge::Init())
	{
		return false;
	}

	this->SetAnchorPoint(0.5, 0.5);

	auto root = CHmsNode::Create();
	this->AddChild(root);
	m_pRoot = root;

	//罗盘
	m_pCompass = CHmsImageNode::Create("res/Pfd/HSI/compass.png");
	if (m_pCompass)
	{
		m_compassRadius = m_pCompass->GetContentSize().width / 2.0;
	}
	Vec2 compassCenter(m_compassRadius, m_compassRadius);
	
	//转弯率指示器
	auto turnRateIndcator = CHmsImageNode::Create("res/Pfd/HSI/compass_bg.png");
	//转弯率矢量线
	m_pTurnRateVector = CHmsDrawNode::Create();

// 	//当前航迹游标
// 	m_pCurTrackBug = CHmsImageNode::Create("res/Pfd/HSI/trackBug.png");
// 	{
// 		m_pCurTrackBug->SetPosition(compassCenter + Vec2(0, m_compassRadius + 6));
// 		m_pCurTrackBug->SetAnchorPoint(Vec2(0.5, 0));
// 
// 		m_pCompass->AddChild(m_pCurTrackBug);
// 	}
// 
// 	//VOR导航指针
// 	m_pVorPointer = CHmsImageNode::Create("res/Pfd/HSI/pointer2.png");
// 	m_pVorPointer->SetColor(m_navPointerColor);
// 	m_pVorPointer->SetPosition(compassCenter);
// 	m_pCompass->AddChild(m_pVorPointer);
// 
// 	//ADF导航指针
// 	m_pAdfPointer = CHmsImageNode::Create("res/Pfd/HSI/pointer1.png");
// 	m_pAdfPointer->SetColor(m_adfPointerColor);
// 	m_pAdfPointer->SetPosition(compassCenter);
// 	m_pCompass->AddChild(m_pAdfPointer);

	//航向指针
	m_pCrsPointer = CHmsNode::Create();
	m_pCrsPointer->SetPosition(compassCenter);
	{
		auto pointer = CHmsImageNode::Create("res/Pfd/HSI/crsPointer.png");
		if (pointer)
		{
			pointer->SetName("pointer");
			pointer->SetColor(m_crsPointerColor[m_curSrc]);
			pointer->SetPosition(0, 0);
		}
		//向台标识
		m_pToIndicator = CHmsImageNode::Create("res/Pfd/HSI/cdiMarkTo.png");
		if (m_pToIndicator)
		{
			m_pToIndicator->SetColor(m_crsPointerColor[m_curSrc]);
			m_pToIndicator->SetPosition(0, m_innerCircleRadius - 10);
		}
		//背台标识
		m_pFromIndicator  = CHmsImageNode::Create("res/Pfd/HSI/cdiMarkFrom.png");
		if (m_pFromIndicator)
		{
			m_pFromIndicator->SetColor(m_crsPointerColor[m_curSrc]);
			m_pFromIndicator->SetPosition(-0.5f, -m_innerCircleRadius + 10);
		}
		//偏离刻度
		auto cdiScale = CHmsDrawNode::Create();
		Vec2 center1(-m_cdiScaleGap_px, 0);
		Vec2 center2(m_cdiScaleGap_px, 0);
		Vec2 center3(-m_cdiScaleGap_px * 2, 0);
		Vec2 center4(m_cdiScaleGap_px * 2, 0);
		cdiScale->DrawRing(center1, 4, 1.5f);
		cdiScale->DrawRing(center2, 4, 1.5f);
		cdiScale->DrawRing(center3, 4, 1.5f);
		cdiScale->DrawRing(center4, 4, 1.5f);
		//偏离杆
		m_pCDIPointer = CHmsImageNode::Create("res/Pfd/HSI/cdiPointer.png");
		if (m_pCDIPointer)
		{
			m_pCDIPointer->SetColor(m_crsPointerColor[m_curSrc]);
			m_pCDIPointer->SetPosition(0, 0);
		}

		if (pointer)
		{
			m_pCrsPointer->AddChild(pointer);
		}
		if (m_pToIndicator)
		{
			m_pCrsPointer->AddChild(m_pToIndicator);
		}
		if (m_pFromIndicator)
		{
			m_pCrsPointer->AddChild(m_pFromIndicator);
		}
		m_pCrsPointer->AddChild(cdiScale);
		if (m_pCDIPointer)
		{
			m_pCrsPointer->AddChild(m_pCDIPointer);
		}
	}
	m_pCompass->AddChild(m_pCrsPointer);

	//导航源
	m_pNavSrcLabel = CHmsGlobal::CreateLabel("GPS", 12, true);
	if (m_pNavSrcLabel)
	{
		m_pNavSrcLabel->SetTextColor(Color4B(m_crsPointerColor[m_curSrc]));
		m_pNavSrcLabel->SetAnchorPoint(Vec2(0.5, 0.5));
		m_pNavSrcLabel->SetPosition(-34, 18);
	}

	//GPS飞行阶段
	m_pGpsPhaseFlight = CHmsGlobal::CreateLabel("", 12, true);
	if (m_pGpsPhaseFlight)
	{
		m_pGpsPhaseFlight->SetTextColor(Color4B(m_crsPointerColor[NAV_GPS]));
		m_pGpsPhaseFlight->SetAnchorPoint(Vec2(0.5, 0.5));
		m_pGpsPhaseFlight->SetPosition(34, 18);
	}

	//OBS模式标签
	m_pOBSLable = CHmsGlobal::CreateLabel("OBS", 12, true);
	if (m_pOBSLable)
	{
		m_pOBSLable->SetTextColor(Color4B(m_crsPointerColor[NAV_GPS]));
		m_pOBSLable->SetAnchorPoint(Vec2(0.5, 0.5));
		m_pOBSLable->SetPosition(34, -18);
	}

	//选择航道标签
	m_pSelCourseLable = CHmsGlobal::CreateLabel("CRS 000", 16, true);
	if (m_pSelCourseLable)
	{
		m_pSelCourseLable->SetAnchorPoint(Vec2(0.5, 0.5));
		m_pSelCourseLable->SetPosition(m_compassRadius, m_compassRadius);
		m_pSelCourseLable->SetVisible(false);
	}

	//选择航向
	m_pSelHdgLable = CHmsGlobal::CreateLabel("HDG 000", 16, true);
	if (m_pSelHdgLable)
	{
		m_pSelHdgLable->SetAnchorPoint(Vec2(0.5, 0.5));
		m_pSelHdgLable->SetPosition(-m_compassRadius, m_compassRadius);
		m_pSelHdgLable->SetVisible(false);
	}

	//选择航向游标
	m_pSelHdgBug = CHmsImageNode::Create("res/Pfd/HSI/hdg_bug.png");
	if (m_pSelHdgBug)
	{
		m_pSelHdgBug->SetColor(m_selHdgBugColor);
		m_pSelHdgBug->SetAnchorPoint(Vec2(0.5, 0));
		m_pSelHdgBug->SetPosition(compassCenter + Vec2(0, m_compassRadius + 2));
		
		m_pCompass->AddChild(m_pSelHdgBug);
	}

	//当前航向box
	auto hdgBox = CHmsImageNode::Create("res/Pfd/HSI/hdg_box.png");
	{
		hdgBox->SetAnchorPoint(Vec2(0.5, 0));
		if (m_pSelHdgBug)
		{
			hdgBox->SetPosition(0, m_compassRadius + m_pSelHdgBug->GetContentSize().height);
		}

		m_pCurHdgLabel = CHmsGlobal::CreateUTF8Label("000\302\260", 18);
		m_pCurHdgLabel->SetAnchorPoint(Vec2(0.5, 1.0));
		m_pCurHdgLabel->SetPosition(hdgBox->GetContentSize().width / 2.0, hdgBox->GetContentSize().height - 2);
		hdgBox->AddChild(m_pCurHdgLabel);

		m_pTrueOrMagneticType = CHmsGlobal::CreateLanguageLabel("M", 18.0, Color4F::WHITE, true);
		m_pTrueOrMagneticType->SetAnchorPoint(0.0, 1.0);
		m_pTrueOrMagneticType->SetPosition(hdgBox->GetContentSize().width + 10.0, hdgBox->GetContentSize().height - 2);
		hdgBox->AddChild(m_pTrueOrMagneticType);

		m_pTrackOrHeadingType = CHmsGlobal::/*CreateLanguageLabel*/CreateLabel("HDG", 18.0, Color4F::WHITE, true);
		m_pTrackOrHeadingType->SetAnchorPoint(1.0, 1.0);
		m_pTrackOrHeadingType->SetPosition(-10.0, hdgBox->GetContentSize().height - 2);
		hdgBox->AddChild(m_pTrackOrHeadingType);
	}

	//飞机标识
	auto aircraft = CHmsImageNode::Create("res/Pfd/HSI/aircraft.png");

	root->AddChild(m_pCompass);
	if (turnRateIndcator)
	{
		root->AddChild(turnRateIndcator);
	}
	root->AddChild(m_pTurnRateVector);
	root->AddChild(hdgBox);
	root->AddChild(aircraft);
	if (m_pNavSrcLabel)
	{
		root->AddChild(m_pNavSrcLabel);
	}
	if (m_pGpsPhaseFlight)
	{
		root->AddChild(m_pGpsPhaseFlight);
	}
	if (m_pOBSLable)
	{
		root->AddChild(m_pOBSLable);
	}
	if (m_pSelCourseLable)
	{
		root->AddChild(m_pSelCourseLable);
	}
	if (m_pSelHdgLable)
	{
		root->AddChild(m_pSelHdgLable);
	}

	SetNavSrc(NAV_GPS);
	ChangeToFromMode(m_curToFromMode);
	UpdateHeading(0.0);
	SetCourse(0);
	ModifyCtrlData(false, m_course);

	this->SetContentSize(Size((m_compassRadius + 52) * 2, (m_compassRadius + 52) * 2));
	root->SetPosition(GetContentSize() / 2.0f);

	m_pFailWin = CHmsDrawNode::Create();
	DrawFailWin(m_pFailWin, hdgBox->GetPoistionX() - hdgBox->GetContentSize().width / 2.0, hdgBox->GetPoistionY() + 10, hdgBox->GetContentSize().width / 2.0, hdgBox->GetContentSize().height + hdgBox->GetPoistionY());
	root->AddChild(m_pFailWin);
// 	m_pVorPointer->SetVisible(false);
// 	m_pAdfPointer->SetVisible(false);
//	m_pCurTrackBug->SetVisible(false);
	m_pCrsPointer->SetVisible(false);
	if (m_pNavSrcLabel)
	{
		m_pNavSrcLabel->SetVisible(false);
	}
	//RegisterTouchEvent(-1);

	this->RegisterAllNodeChild();
	SetScheduleUpdate();
	return true;
}
#include "DataInterface/HmsDataBus.h"
void CHmsHSI::Update(float delta)
{
	if (m_curGaugeMode == CHmsGauge::GaugeMode::GAUGE_MODE_ERROR) return;

	CHmsDataBus *dataBus = CHmsGlobal::GetInstance()->GetDataBus();
	auto aftData = dataBus->GetData();
	FmsCalData & fmsData = CHmsGlobal::GetInstance()->GetDataBus()->GetFmsData();
	CHmsDuControlData *pDUCtrl = CHmsGlobal::GetInstance()->GetDataBus()->GetDuControlData();

	if (m_ePfdCompassDataMode != pDUCtrl->GetPfdCompassDataMode())
	{
		SetPfdCompassDataMode(pDUCtrl->GetPfdCompassDataMode());
	}

	switch (m_ePfdCompassDataMode)
	{
	case PFD_COMPASS_DATA_MODE::magneticHeading:
		UpdateHeading(aftData->magneticHeading);
		break;
	case PFD_COMPASS_DATA_MODE::trueTrack:
		UpdateHeading(aftData->trueTrack);
		break;
	case PFD_COMPASS_DATA_MODE::trueHeading:
		UpdateHeading(aftData->trueHeading);
		break;
	case PFD_COMPASS_DATA_MODE::magneticTrack:
		//TODO
		break;
	default:
		break;
	}

	//SetHeadType(false);
	//SetTrackType(false);
	

	bool bUnbelievable = (aftData->groundSpeed * KnotKMH < 5) ? true : false;
	SetDataUnbelievable(bUnbelievable);

#if 0
	m_pHsiManager->RefreshData();
	bool vorUsable = m_pHsiManager->VorUsable();
	bool adfUsable = m_pHsiManager->AdfUsable();
	bool trackUsable = m_pHsiManager->TrackUsable();
	float vorQDM = m_pHsiManager->GetVorQDM();
	float adfQDM = m_pHsiManager->GetAdfQDM();
	float track = m_pHsiManager->GetTrack();

	m_pVorPointer->SetVisible(vorUsable);
	m_pAdfPointer->SetVisible(adfUsable);
	m_pCurTrackBug->SetVisible(trackUsable);
	if (vorUsable) m_pVorPointer->SetRotation(vorQDM);
	if (adfUsable) m_pAdfPointer->SetRotation(adfQDM);
	if (trackUsable) m_pHsiManager->rotateNodeOnCenter(m_pCurTrackBug, track, Vec2(m_compassRadius, m_compassRadius), m_compassRadius + 6);

	if (!m_pHsiManager->IsLocalizer() && m_curSrc == NAV_LOC)
	{
		SetNavSrc(NAV_VOR);
		if (OnNavSrcChanged) OnNavSrcChanged(NAV_VOR);
	}
	else if (m_pHsiManager->IsLocalizer() && m_curSrc == NAV_VOR)
	{
		SetNavSrc(NAV_LOC);
		if (OnNavSrcChanged) OnNavSrcChanged(NAV_LOC);
	}
	UpdateHeading(m_pHsiManager->GetMagHeading());
	UpdateCDI();
	UpdateTrunRate(m_pHsiManager->GetTurnRate());
#endif

	
	if (pDUCtrl && !pDUCtrl->IsObsMode() && fmsData.changeCourse){
		SetCourse(fmsData.course);
	}else if (pDUCtrl && pDUCtrl->IsObsMode()){
		SetCourse(pDUCtrl->GetCurCourse());
	}

	if (pDUCtrl){
		SetSelHdg(pDUCtrl->GetSelectHeading());
		SetOBSMode(pDUCtrl->IsObsMode());
	}
	
	if (fmsData.valid && m_curSrc == NAV_GPS)
	{
		float cd_px = fmsData.cdiRatio * m_cdiScaleGap_px;//偏离像素
		m_pCDIPointer->SetPositionX(cd_px);

// 		CHmsDuControlData *pDUCtrl = CHmsGlobal::GetInstance()->GetDataBus()->GetDuControlData();
// 		if (pDUCtrl && !pDUCtrl->IsObsMode() && fmsData.changeCourse){
// 			SetCourse(fmsData.course);
// 		}else if (pDUCtrl && pDUCtrl->IsObsMode()){
// 			SetCourse(pDUCtrl->GetCurCourse());
// 		}

		m_pGpsPhaseFlight->SetString(fmsData.phaseOfFlight);
	}
	else
	{
		m_pGpsPhaseFlight->SetString("");
	}

	if (m_crsLabelShowTime > 0)
	{
		m_crsLabelShowTime = m_crsLabelShowTime - delta * 1000;
		if (m_pSelCourseLable && !m_pSelCourseLable->IsVisible())
		{
			m_pSelCourseLable->SetVisible(true);
			m_pSelCourseLable->ForceUpdate();
		}
			
	}
	else
	{
		if (m_pSelCourseLable)
		{
			m_pSelCourseLable->SetVisible(false);
		}
	}

	if (m_hdgLabelShowTime > 0)
	{
		m_hdgLabelShowTime = m_hdgLabelShowTime - delta * 1000;
		if (m_pSelHdgLable && !m_pSelHdgLable->IsVisible())
		{
			m_pSelHdgLable->SetVisible(true);
		}
	}
	else
	{
		if (m_pSelHdgLable)
		{
			m_pSelHdgLable->SetVisible(false);
		}
	}
}

void CHmsHSI::SetGaugeMode(GaugeMode flag)
{
	if (flag == m_curGaugeMode) return;

	m_curGaugeMode = flag;
	if (m_curGaugeMode == CHmsGauge::GaugeMode::GAUGE_MODE_OK)
	{
// 		m_pVorPointer->SetVisible(true);
// 		m_pAdfPointer->SetVisible(true);
//		m_pCurTrackBug->SetVisible(true);
		if (m_pCrsPointer)
		{
			m_pCrsPointer->SetVisible(true);
			m_pCrsPointer->ForceUpdate();
		}
		if (m_pNavSrcLabel)
		{
			m_pNavSrcLabel->SetVisible(true);

			m_pNavSrcLabel->ForceUpdate();
		}
		if (m_pRoot)
		{
			m_pRoot->RemoveChild(m_pFailWin);
			m_pRoot->SetScheduleUpdate();
		}
	}
	else if (m_curGaugeMode == CHmsGauge::GaugeMode::GAUGE_MODE_ERROR)
	{
// 		m_pVorPointer->SetVisible(false);
// 		m_pAdfPointer->SetVisible(false);
//		m_pCurTrackBug->SetVisible(false);
		if (m_pCrsPointer)
		{
			m_pCrsPointer->SetVisible(false);
		}
		if (m_pNavSrcLabel)
		{
			m_pNavSrcLabel->SetVisible(false);
		}
		if (m_pRoot)
		{
			m_pRoot->AddChild(m_pFailWin);
			m_pRoot->SetUnSchedule();
		}
	}
}

void CHmsHSI::SetCourse(const float &fCrs)
{
	if (fabs(fCrs - m_course) > 0.01)
	{
		m_course = fCrs;
		AngleNormalize(m_course);
		if (m_pCrsPointer)
		{
			m_pCrsPointer->SetRotation(m_course);
			CHmsGlobal::SetUTF8LabelFormat(m_pSelCourseLable, "CRS  %03d\302\260", (int)m_course);
			m_crsLabelShowTime = LABEL_SHOW_MAX_TIME;
			m_pCrsPointer->ForceUpdate();
		}
	}
}

void CHmsHSI::SetSelHdg(const float &fSelHdg)
{
	if (fabsf(m_selHeading - fSelHdg) > 0.01)
	{
		m_selHeading = fSelHdg;
		AngleNormalize(m_selHeading);
		if (m_pHsiManager)
		{
			m_pHsiManager->rotateNodeOnCenter(m_pSelHdgBug, m_selHeading, Vec2(m_compassRadius, m_compassRadius), m_compassRadius + 2);
		}

		CHmsGlobal::SetUTF8LabelFormat(m_pSelHdgLable, "HDG  %03d\302\260", int(m_selHeading));
		m_hdgLabelShowTime = LABEL_SHOW_MAX_TIME;
	}
}

void CHmsHSI::SetDataUnbelievable(bool bUnbelievable)
{
	if (m_bDataUnbelievable != bUnbelievable)
	{
		m_bDataUnbelievable = bUnbelievable;

		if (m_pCurHdgLabel)
		{
			if (bUnbelievable)
			{
				m_pCurHdgLabel->SetTextColor(Color4B::YELLOW);
			}
			else
			{
				m_pCurHdgLabel->SetTextColor(Color4B::WHITE);
			}
		}
	}
}

bool CHmsHSI::OnPressed(const HmsAviPf::Vec2 & posOriginOld)
{
	if (m_curGaugeMode == CHmsGauge::GaugeMode::GAUGE_MODE_ERROR) return false;

	Vec2 posOrigin(posOriginOld.x - m_sizeContent.width*0.5f, posOriginOld.y - m_sizeContent.height*0.5f);
	if (!m_pHsiManager)
	{
		return false;
	}
	bool inInnerCircle = m_pHsiManager->ContainsPoint(Vec2(0, 0), m_innerCircleRadius, posOrigin);
	bool inCompassCircle = m_pHsiManager->ContainsPoint(Vec2(0, 0), m_compassRadius, posOrigin);
	bool inMaxCircle = m_pHsiManager->ContainsPoint(Vec2(0, 0), m_sizeContent.height / 2.0f, posOrigin);

	bool ret = false;
	if (inInnerCircle)//如果在内圆弧，为切换导航源
	{
		m_ctrlMode = CTRL_CHANGE_OBS;
		ret = true;
	}
	else if (!inInnerCircle && inCompassCircle)//如果不在内圆弧，但在罗盘范围内，为改变航道
	{
		m_ctrlMode = CTRL_CHANGE_COURSE;
		ret = true;
	}
	else if (!inCompassCircle && inMaxCircle)//不在罗盘范围内，但在矩形高为半径的圆内，为改变选择航向
	{
		m_ctrlMode = CTRL_CHANGE_HDG;
		ret = true;
	}

	m_pressedPos = posOrigin;

	return ret;
}

void CHmsHSI::OnMove(const Vec2 & posOriginOld)
{
	Vec2 posOrigin(posOriginOld.x - m_sizeContent.width*0.5f, posOriginOld.y - m_sizeContent.height*0.5f);
	if (!m_pHsiManager)
	{
		return;
	}
	float rotateAngle = m_pHsiManager->CalTwoPointsAngle(Vec2(0, 0), m_pressedPos, posOrigin);

	if (rotateAngle >= 1 || rotateAngle <= -1)
	{
		m_pressedPos = posOrigin;

		if (m_ctrlMode == CTRL_CHANGE_COURSE)
		{
			float newCourse = m_course + int(rotateAngle);
			ModifyCtrlData(true, newCourse);
			if (OnCourseChanged)
			{
				OnCourseChanged(int(newCourse));
			}
		}
		else if (m_ctrlMode == CTRL_CHANGE_HDG)
		{
			float newSelHeading = m_selHeading + int(rotateAngle);
			ModifySelHeading(newSelHeading);
			if (OnSelHeadingChanged)
			{
				OnSelHeadingChanged(int(newSelHeading));
			}
		}
	}

}

void CHmsHSI::OnReleased(const Vec2 & posOrigin)
{
	if (m_ctrlMode == CTRL_CHANGE_OBS)
	{
		bool bOBS = true;
		CHmsDuControlData *ctrlData = CHmsGlobal::GetInstance()->GetDataBus()->GetDuControlData();
		if (ctrlData)
		{
			bOBS = !ctrlData->IsObsMode();
		}

		ModifyCtrlData(bOBS, m_course);
	}
}

void CHmsHSI::SetNavSrc(NAV_SRC src)
{
	if (src == m_curSrc)
	{
		return;
	}

	m_curSrc = src;

	if (m_pNavSrcLabel)
	{
		m_pNavSrcLabel->SetString(NAV_SRC_STR[m_curSrc]);
	}

	Color3B newColor = m_crsPointerColor[m_curSrc];
	if (m_pCrsPointer)
	{
		m_pCrsPointer->GetChildByName("pointer")->SetColor(newColor);
		m_pCrsPointer->SetColor(newColor);
	}
	if (m_pToIndicator)
	{
		m_pToIndicator->SetColor(newColor);
	}
	if (m_pFromIndicator)
	{
		m_pFromIndicator->SetColor(newColor);
	}
	if (m_pCDIPointer)
	{
		m_pCDIPointer->SetColor(newColor);
	}
	if (m_pNavSrcLabel)
	{
		m_pNavSrcLabel->SetTextColor(Color4B(newColor));
	}
}

void CHmsHSI::ChangeToFromMode(NAV_TOFROM_MODE mode)
{
	if (m_pToIndicator)
	{
		m_pToIndicator->SetVisible(mode == NAV_MODE_TO);
	}
	if (m_pFromIndicator)
	{
		m_pFromIndicator->SetVisible(mode == NAV_MODE_FROM);
	}

	if (m_curToFromMode != mode)
	{
		//m_pHsiManager->SendToFrom(m_curToFromMode);
		m_curToFromMode = mode;
	}
}

void CHmsHSI::UpdateTrunRate(const float &turnRate)
{
	if (fabs(m_turnRate - turnRate) < 0.001)
	{
		return;
	}
	m_turnRate = turnRate;
	float formatFtr = m_turnRate;

	double outRadian = 0.0;
	bool showArrow = false;
	if (formatFtr > 24.0f)
	{
		showArrow = true;
		formatFtr = 24.0f;
		outRadian = ToRadian(formatFtr + 4);
	}
	else if (formatFtr < -24.0f)
	{
		showArrow = true;
		formatFtr = -24.0f;
		outRadian = ToRadian(formatFtr - 4);
	}
	if (!m_pTurnRateVector)
	{
		return;
	}
	m_pTurnRateVector->clear();
	if (showArrow)
	{
		double innerRadian = ToRadian(formatFtr);
		std::vector<Vec2> vPos;
		vPos.push_back(Vec2(sin(innerRadian) * (m_compassRadius - 3), cos(innerRadian) * (m_compassRadius - 3)));
		vPos.push_back(Vec2(sin(innerRadian) * (m_compassRadius + 7), cos(innerRadian) * (m_compassRadius + 7)));
		vPos.push_back(Vec2(sin(outRadian) * (m_compassRadius + 2), cos(outRadian) * (m_compassRadius + 2)));
		m_pTurnRateVector->DrawTriangles(vPos, Color4F(m_turnRateVectorColor));
	}

	m_pTurnRateVector->DrawArc(0, formatFtr, m_compassRadius + 2, 3.0f, 50u, Color4F(m_turnRateVectorColor));
}

void CHmsHSI::UpdateHeading(const float &fHeading)
{
	if (fabs(m_heading - fHeading) < 0.001)
	{
		return;
	}

	m_heading = fHeading;
	CHmsGlobal::SetUTF8LabelFormat(m_pCurHdgLabel, "%03d\302\260", (int)round(fHeading));
	if (m_pCompass)
	{
		m_pCompass->SetRotation(-fHeading);
	}
}

void CHmsHSI::UpdateCDI()
{
#if 0
	NAV_TOFROM_MODE toOrFrom = m_pHsiManager->GetNavToFromMode(m_curSrc, m_course);
	ChangeToFromMode(toOrFrom);

	float deviation = m_pHsiManager->GetCourceDeviation(m_curSrc, m_course);
	float cd_px = deviation * m_cdiScaleGap_px;//偏离像素
	m_pCDIPointer->SetPositionX(cd_px);

	m_pHsiManager->SendCourseDeviation(deviation);
#endif
}

void CHmsHSI::SetOBSMode(bool isOBS)
{
	//if (m_bObsMode == isOBS)
	//	return;

	if (m_pOBSLable)
	{
		m_pOBSLable->SetVisible(isOBS);
		m_bObsMode = isOBS;
	}
}

void CHmsHSI::SetPfdCompassDataMode(PFD_COMPASS_DATA_MODE eMode)
{
	m_ePfdCompassDataMode = eMode;
	switch (m_ePfdCompassDataMode)
	{
	case PFD_COMPASS_DATA_MODE::trueTrack:
		SetTrueOrMagneticType(true);
		SetTrackOrHeadingType(true);
		break;
	case PFD_COMPASS_DATA_MODE::trueHeading:
		SetTrueOrMagneticType(true);
		SetTrackOrHeadingType(false);
		break;
	case PFD_COMPASS_DATA_MODE::magneticTrack:
		SetTrueOrMagneticType(false);
		SetTrackOrHeadingType(true);
		break;
	case PFD_COMPASS_DATA_MODE::magneticHeading:
		SetTrueOrMagneticType(false);
		SetTrackOrHeadingType(false);
		break;
	default:
		break;
	}
}

void CHmsHSI::AngleNormalize(float &degree)
{
	float temp = degree;
	int ratio = 1;// degree / 360;
	if (degree > 360.0f)
	{
		temp = degree - int(degree / 360) * 360;
	}
	else if (degree </*=*/ 0)
	{
		temp = 360 - degree;
	}
	degree = temp;
}

void CHmsHSI::ModifyCtrlData(bool bOBs, float fCourse)
{
	CHmsDuControlData *ctrlData = CHmsGlobal::GetInstance()->GetDataBus()->GetDuControlData();
	ctrlData->SetIsOBSMode(bOBs);
	ctrlData->SetCurCourse(fCourse);
}

void CHmsHSI::ModifySelHeading(float fHeading)
{
	CHmsDuControlData *ctrlData = CHmsGlobal::GetInstance()->GetDataBus()->GetDuControlData();
	if (ctrlData)
	{
		ctrlData->SetSelectHeading(fHeading);
	}
}

void CHmsHSI::SetTrueOrMagneticType(bool bTrueHead)
{
	if (m_pTrueOrMagneticType)
	{
		m_pTrueOrMagneticType->SetString(bTrueHead ? "T" : "M");
		m_bTrueHeading = bTrueHead;
	}
}

void CHmsHSI::SetTrackOrHeadingType(bool bTrack)
{
	if (m_pTrackOrHeadingType)
	{
		m_pTrackOrHeadingType->SetString(bTrack ? "TRK" : "HDG");
		m_bTrack = bTrack;
	}
}