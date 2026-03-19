#pragma once
#include "../HmsGlobal.h"
#include "HmsAviPf.h"
#include "display/HmsDrawNode.h"
#include "base/HmsClippingNode.h"
#include <list>

/*最大空速值.*/
#define MAX_AIRSPEED        (999.0f) //千米每小时.

typedef enum
{
	MARK_V1 = 0x00,
	MARK_V2,
	MARK_VR,
	MARK_VREF,
	MARK_SEL,
}HmsMarkType;

typedef struct _HmsAirspeedMark
{
	float vMin;
	float vMax;
	float vWid;
	HmsAviPf::Color4F color;
}HmsAirspeedMark;

class CHmsHudSpeedBar : public HmsAviPf::CHmsNode
{
public:
	CHmsHudSpeedBar();
	~CHmsHudSpeedBar();

	CREATE_FUNC(CHmsHudSpeedBar)

	static CHmsHudSpeedBar* Create(HmsAviPf::Size size);
	bool InitWithSize(HmsAviPf::Size size);

	void SetMarkValue(HmsMarkType mType, float fValue);
	void SetValue(float curAirspeed, float preAirspeed);

	void AddMaskNode(HmsAviPf::CHmsNode * node);

protected:
	void GenSpeedBar();
	HmsAviPf::CHmsNode* GenTextLine(const char *szValue, bool bVisible);
	void CreateMark(
		HmsAviPf::CHmsNode *pParent, 
		HmsAviPf::CHmsImageNode **curNode, 
		const char *szpng);
	HmsAviPf::CHmsImageNode* GetMarkPointer(HmsMarkType mtype, float **fValP);
	HmsAviPf::CHmsNode* GetValue(bool bFront, float *pOut);
	void SetTickLabelValue(HmsAviPf::CHmsNode *pLabelNode, bool bFront, float fVlaue);
	void DrawSpeedMark(float fmin);
	void SetMarkPosition(HmsMarkType mtype, float frefmin, float frefmax);

protected:
	std::list<HmsAviPf::CHmsNode*>		m_pTickLabel;//刻度值集.
	std::vector<HmsAirspeedMark>	m_SpeedBarStyle;//空速条样式.
	HmsAviPf::CHmsNode*					m_pTextNode;//刻度文本节点集.
	HmsAviPf::CHmsDrawNode  *		m_pSpeedBar;//空速条.
	HmsAviPf::CHmsImageNode *				m_pV1Mark;//V1速度指示游标.
	HmsAviPf::CHmsImageNode *				m_pV2Mark;//V2速度指示游标.
	HmsAviPf::CHmsImageNode *				m_pVrMark;//VR速度指示游标.
	HmsAviPf::CHmsImageNode *				m_pVrefMark;//VREF速度指示游标.
	HmsAviPf::CHmsImageNode *				m_pSelSpeedMark;//选择空速游标.

	HmsAviPf::CHmsClippingNode *			m_pClipText;

	float							m_V1Value;//V1指示值.
	float							m_V2Value;//V2指示值.
	float							m_VrValue;//VR指示值.
	float							m_VrefValue;//VREF指示值.
	float							m_SelSpeed;//选择空速值.
	float							m_AirSpeed;//记录指示空速.
	float							m_tickPixels;//多少像素表示1千米每小时.
	float							m_tickSpan;  //空速刻度间隔值.
	unsigned int					m_tickNumber;//刻度数量.
};

