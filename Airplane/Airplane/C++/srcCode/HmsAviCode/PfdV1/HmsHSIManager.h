#pragma once

#include "HmsHSI.h"

class CHmsDataBus;

#if 0
class CHmsHSIManager
{
public:
    CHmsHSIManager(CHmsHSI *hsi);
    ~CHmsHSIManager();

    void RefreshData();

    float GetVorQDM();

    float GetAdfQDM();

    float GetTrack();

    bool VorUsable();

    bool AdfUsable();

    bool TrackUsable();

    bool IsLocalizer();//是否为仪表降落信标

    CHmsHSI::NAV_TOFROM_MODE GetNavToFromMode(CHmsHSI::NAV_SRC navSrc, const float &course);//根据导航源获取向背台模式 course预选航道

    //获取航道偏差 返回值范围 -2~2
    float GetCourceDeviation(CHmsHSI::NAV_SRC navSrc, const float &course);

    float GetMagHeading();//获取磁航向

    float GetTurnRate();//获取转弯率

    void SendToFrom(CHmsHSI::NAV_TOFROM_MODE mode);//发送向背台标识

    void SendCourseDeviation(float fCourseDeviation);//发送航道偏离 偏离值在-2~2之间

    //以指定点为中心旋转节点
    //target 目标节点 angle旋转角度 center指定的中心点 radius旋转半径
    void rotateNodeOnCenter(HmsAviPf::CHmsNode *target, const float &angle, const HmsAviPf::Vec2 &center, const float &radius);

    //判断一个圆中是否包含一个点 center圆心 radius圆半径 point所需判断的点
    bool ContainsPoint(const HmsAviPf::Vec2 &center, const float &radius, const HmsAviPf::Vec2 &point);

    //计算两个点与中心点之间构成的顺时针夹角
    float CalTwoPointsAngle(const HmsAviPf::Vec2 &center, const HmsAviPf::Vec2 &point1, const HmsAviPf::Vec2 &point2);

    float CalAngleWithZero(const HmsAviPf::Vec2 &point1);
    //角度标准化，如361°->360°，-18°->342°
    void AngleNormalize(float &degree);
private:
    CHmsHSI *m_pHsi;
    CHmsDataBus *m_dataBus;

    InsGnssData m_insGnss;
    DmeData m_dmeData;
    VorilsData m_vorils;
    AdfData m_adfData;
    AircraftInstrumentData *m_aircraftData;


    float m_preSendCD;//上次发送的偏差

    //RB转QDM角度 fRb电台相对方位角 fMh磁航向
    float RBToQDM(float fRb, float fMh);

    //QDR转QDM角度 fQdr飞机磁方位角
    float QDRToQDM(float fQdr);

    //QDM转QDR角度 fQdm电台磁方位角
    float QDMToQDR(float fQdm);

    //获取相应导航源的QDR角
    float GetQDR(CHmsHSI::NAV_SRC navSrc);

    //根据qdr角度确定向背台模式 course选择航道 qdr飞机磁方位角
    CHmsHSI::NAV_TOFROM_MODE CalToFromByQDR(const float &course, const float &qdr);

    //根据qdr计算航偏,返回航偏角度 返回值范围-10~10
    float CalCourceDeviation(const float &course, const float &qdr);
};
#else
class CHmsHSIManager
{
public:
    CHmsHSIManager(CHmsHSI *hsi){}
    ~CHmsHSIManager(){}

    //以指定点为中心旋转节点
    //target 目标节点 angle旋转角度 center指定的中心点 radius旋转半径
    void rotateNodeOnCenter(HmsAviPf::CHmsNode *target, const float &angle, const HmsAviPf::Vec2 &center, const float &radius);

    //判断一个圆中是否包含一个点 center圆心 radius圆半径 point所需判断的点
    bool ContainsPoint(const HmsAviPf::Vec2 &center, const float &radius, const HmsAviPf::Vec2 &point);

    //计算两个点与中心点之间构成的顺时针夹角
    float CalTwoPointsAngle(const HmsAviPf::Vec2 &center, const HmsAviPf::Vec2 &point1, const HmsAviPf::Vec2 &point2);

    float CalAngleWithZero(const HmsAviPf::Vec2 &point1);
    //角度标准化，如361°->360°，-18°->342°
    void AngleNormalize(float &degree);

    //根据qdr计算航偏,返回航偏角度 返回值范围-10~10
    float CalCourceDeviation(const float &course, const float &qdr);

    bool IsLocalizer(){ return false; }//是否为仪表降落信标
};
#endif