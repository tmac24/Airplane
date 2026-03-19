#pragma once
#include "../HmsGlobal.h"
#include "math/HmsMath.h"
#include "math/HmsGeometry.h"
#include "base/HmsNode.h"
#include "base/HmsClippingNode.h"
#include "display/HmsImageNode.h"
#include "display/HmsStretchImageNode.h"
#include "display/HmsLabel.h"
#include "display/HmsDrawNode.h"
#include "display/HmsLightDrawNode.h"
#include "ui/HmsUiEventInterface.h"
#include "DataInterface/HmsDataBus.h"
#include "HmsValidV2.h"

class CHmsHSIManager;
class CHmsHSIV2 : public CHmsValidV2, public HmsAviPf::HmsUiEventInterface
{
public:
	enum PRESS_MODIFY
	{
		MOD_UNKNOW =0x00,
		MOD_OBS,
		MOD_CRS,
		MOD_SELECT,
	};

	CHmsHSIV2();
	~CHmsHSIV2();

	CREATE_FUNC(CHmsHSIV2)

	static CHmsHSIV2* Create(HmsAviPf::Size size);

	bool InitWithSize(HmsAviPf::Size size);

	void SetHeading(float fHeading);
	void SetTrueOrMagneticType(bool bTrueHead);
	void SetPfdCompassDataMode(PFD_COMPASS_DATA_MODE eMode);
	void CheckPfdCompassMode(PFD_COMPASS_DATA_MODE eMode);

	void SetTrackOrHeadingType(bool bTrack);

	void SetVor(float fVorValue);
	void SetSelectHead(float fSelValue);

	void SetOffset(float fOffset);
	void SetOffsetRatio(float Ratio);
	void SetMaxOffset(float fMaxOffset);
	void SetOBSMode(bool bOBs);

	virtual bool ValidSwitch(bool bValid);

	virtual bool OnPressed(const Vec2 & posOrigin) override;
	virtual void OnMove(const Vec2 & posOrigin) override;
	virtual void OnReleased(const Vec2 & posOrigin) override;
	virtual void Update(float delta) override;

protected:
	HmsAviPf::CHmsNode* GenOutterTick(float fRadius);
	HmsAviPf::CHmsNode* GenCircleDisk(float fRadius);
	HmsAviPf::CHmsNode* GenPlaneMarker(HmsAviPf::Size size);
	HmsAviPf::CHmsNode* GenCrsPointer(float fRadius, Color4B color, Color4B borderColor);
	HmsAviPf::CHmsNode* GenOffsetPoint(float fRadius);
	HmsAviPf::CHmsNode* GenOffsetStaff(HmsAviPf::Size size, float fBorderWide, Color4B color, Color4B borderColor, bool bAutoScale);
	HmsAviPf::CHmsNode* GenDisplayText(HmsAviPf::Size size);
	HmsAviPf::CHmsNode* GenCrsHintText();
	HmsAviPf::CHmsNode* GenHdgHintText();
	HmsAviPf::CHmsNode* GenOBSHintText();
	HmsAviPf::CHmsNode* GenSelectHead(HmsAviPf::Size size, float fRadius, Color4B color);

	void ReDrawStaff(float fHeight);
	void DrawSrcTick(
		HmsAviPf::CHmsLightDrawNode *pDrawNode,
		Vec2 &center,
		float fAngle, 
		float fInnRadius, 
		float fOutRadius, 
		float fLineWidth,
		Color4B color);
	void ModifyCtrlData(bool bOBs, float fCourse);
	void ModifySelHeading(float fHeading);

private:
	bool  m_bTrueHeading;
	bool  m_bTrack;
	bool  m_bAutoScale;
	bool  m_bObsMode;

	float m_MidCrcRadius;
	float m_BorderWide;
	float m_CurHeading;
	float m_CurCrsValue;
	float m_CurSelValue;
	float m_CurOffset;
	float m_MaxOffset;
	float m_DialRadius;
	float m_CrsShowTime;	//选择航道标签显示时间
	float m_HdgShowTime;	//选择航向标签显示时间

	Color4B m_StaffColor;
	Color4B m_StaffBorder;

	PRESS_MODIFY			m_ModType;
	PFD_COMPASS_DATA_MODE	m_ePfdCompassDataMode;

	HmsAviPf::Vec2			 m_OffsetScope;
	HmsAviPf::Vec2           m_PressPoint;
	//HmsAviPf::RefPtr<HmsAviPf::CHmsNode>  m_pBackGround;
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>  m_pUnValid;
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>  m_pCricleDisk;
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>  m_pCrsPointer;
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>  m_pSelectHead;
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>  m_pOffsetStaff;
	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel> m_pHeadingText;
	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel> m_pHeadingType;
	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel> m_pTrackType;
	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel> m_pCrsHintText;
	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel> m_pHdgHintText;
	HmsAviPf::RefPtr<HmsAviPf::CHmsLabel> m_pOBSHintText;

	CHmsHSIManager          *m_pHsiManager;
};

