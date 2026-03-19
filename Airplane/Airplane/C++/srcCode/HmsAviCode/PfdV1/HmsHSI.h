#pragma once
#include "../HmsGlobal.h"
#include "HmsGauge.h"
#include "ui/HmsUiEventInterface.h"
#include "DataInterface/HmsDataBus.h"

class CHmsHSIManager;
class CHmsHSI : public CHmsGauge, public HmsAviPf::HmsUiEventInterface
{
public:
    enum NAV_SRC{ NAV_GPS = 0, NAV_ADF = 1, NAV_VOR = 2, NAV_LOC = 3 };//导航模式
    enum NAV_TOFROM_MODE{ NAV_MODE_TO, NAV_MODE_FROM, NAV_MODE_NONE };//向背台模式 NAV_MODE_TO向台 NAV_MODE_FROM背台 NAV_MODE_NONE无数据
    enum CTRL_MODE{ CTRL_CHANGE_OBS, CTRL_CHANGE_COURSE, CTRL_CHANGE_HDG };//控制模式 CTRL_CHANGE_OBS OBS控制 CTRL_CHANGE_COURSE改变预选航道 CTRL_CHANGE_HDG改变航向

    typedef std::function<void(int)> OnCourseChangedCallback;
    typedef std::function<void(int)> OnSelHeadingChangedCallback;
    typedef std::function<void(NAV_SRC)> OnNavSrcChangedCallback;
    CHmsHSI();
    ~CHmsHSI();

    CREATE_FUNC(CHmsHSI)

        virtual bool Init() override;
    virtual void Update(float delta) override;

    void SetGaugeMode(GaugeMode flag) override;

    void SetCourse(const float &fCrs);			//设置选择航道
    void SetSelHdg(const float &fSelHdg);		//设置选择航向
    void SetDataUnbelievable(bool bUnbelievable);
    void SetOBSMode(bool isOBS);					//设置OBS模式
    void SetPfdCompassDataMode(PFD_COMPASS_DATA_MODE eMode);

    virtual bool OnPressed(const HmsAviPf::Vec2 & posOrigin) override;
    virtual void OnMove(const HmsAviPf::Vec2 & posOrigin) override;
    virtual void OnReleased(const HmsAviPf::Vec2 & posOrigin) override;

    OnCourseChangedCallback OnCourseChanged;
    OnSelHeadingChangedCallback OnSelHeadingChanged;
    OnNavSrcChangedCallback OnNavSrcChanged;
private:
    CHmsHSIManager *m_pHsiManager;
    float m_compassRadius;//罗盘半径
    float m_innerCircleRadius;//内圆弧半径
    float m_scaleGap;// 罗盘刻度间隔角度
    float m_cdiScaleGap_px;//偏航刻度间隔
    std::string angleTexts[12];//{ "N", "3", "6", "E", "12", "15", "S", "21", "24", "W", "30", "33" };
    std::string NAV_SRC_STR[4];//{"GPS","ADF","VOR","LOC"}

    float m_turnRate;//当前转弯率
    float m_heading;//当前航向
    float m_course;//当前选择航道
    float m_selHeading;	//当前选择航向
    NAV_SRC m_curSrc;//当前导航模式，初始化时GPS
    NAV_TOFROM_MODE m_curToFromMode;//当前向背台模式，初始化为none
    CTRL_MODE m_ctrlMode;//当前控制模式
    PFD_COMPASS_DATA_MODE	m_ePfdCompassDataMode;
    HmsAviPf::Vec2 m_pressedPos;
    float m_crsLabelShowTime;	//选择航道标签显示时间
    float m_hdgLabelShowTime;	//选择航向标签显示时间
    bool  m_bDataUnbelievable;	//
    bool  m_bObsMode;
    bool  m_bTrueHeading;
    bool  m_bTrack;

    HmsAviPf::Color3B m_selHdgBugColor;
    HmsAviPf::Color3B m_turnRateVectorColor;//转弯率趋势适量线颜色
    HmsAviPf::Color3B m_navPointerColor;
    HmsAviPf::Color3B m_adfPointerColor;
    HmsAviPf::Color3B m_crsPointerColor[4];//航道指针颜色三种 CPS：紫色 NAV：绿色 ADF：蓝色 LOC：绿色

    HmsAviPf::RefPtr<HmsAviPf::CHmsNode>	  m_pRoot;
    HmsAviPf::RefPtr<HmsAviPf::CHmsImageNode> m_pCompass;//罗盘
    //	HmsAviPf::RefPtr<HmsAviPf::CHmsImageNode> m_pCurTrackBug;//当前航迹
    HmsAviPf::RefPtr<HmsAviPf::CHmsDrawNode>  m_pTurnRateVector;//转弯率矢量线
    HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>	  m_pCurHdgLabel;		//当前航向显示标签
    HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>	  m_pTrueOrMagneticType;
    HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>	  m_pTrackOrHeadingType;
    HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>	  m_pNavSrcLabel;		//导航源显示标签
    HmsAviPf::RefPtr<HmsAviPf::CHmsNode>	  m_pCrsPointer;		//航道指针
    HmsAviPf::RefPtr<HmsAviPf::CHmsImageNode> m_pCDIPointer;			//偏航指针
    HmsAviPf::RefPtr<HmsAviPf::CHmsImageNode> m_pToIndicator;		//向台指示器
    HmsAviPf::RefPtr<HmsAviPf::CHmsImageNode> m_pFromIndicator;		//背台指示器
    // 	HmsAviPf::RefPtr<HmsAviPf::CHmsImageNode> m_pAdfPointer;//ADF指针
    // 	HmsAviPf::RefPtr<HmsAviPf::CHmsImageNode> m_pVorPointer;//Nav指针
    HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>  m_pGpsPhaseFlight;		//飞行阶段
    HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>  m_pOBSLable;			//OBS标签
    HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>  m_pSelCourseLable;		//选择航道角度
    HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>  m_pSelHdgLable;		//选择航道角度
    HmsAviPf::RefPtr<HmsAviPf::CHmsImageNode> m_pSelHdgBug;			//选择航向
    HmsAviPf::RefPtr<HmsAviPf::CHmsDrawNode>  m_pFailWin;		//故障窗体

    void SetNavSrc(NAV_SRC src);				//设置导航源
    void ChangeToFromMode(NAV_TOFROM_MODE mode);	//设置向台/背台模式
    void UpdateTrunRate(const float &turnRate);//刷新转弯率 单位：度/秒
    void UpdateHeading(const float &fHeading);		//设置当前磁航向
    void UpdateCDI();//刷新偏航指示器
    void AngleNormalize(float &degree);				//角度标准化
    void ModifyCtrlData(bool bOBs, float fCourse);							//modify HmsDataBus ctrl data
    void ModifySelHeading(float fHeading);

    void SetTrueOrMagneticType(bool bTrueHead);
    void SetTrackOrHeadingType(bool bTrack);
};

