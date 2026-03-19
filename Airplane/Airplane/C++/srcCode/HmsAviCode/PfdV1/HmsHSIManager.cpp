#include "HmsHSIManager.h"
#include "../Calculate/CalculateZs.h"

USING_NS_HMS;

#if 0
CHmsHSIManager::CHmsHSIManager(CHmsHSI *hsi)
:m_preSendCD(-100)
{
    this->m_pHsi = hsi;

    m_dataBus = CHmsGlobal::GetInstance()->GetDataBus();
}


CHmsHSIManager::~CHmsHSIManager()
{
}

void CHmsHSIManager::RefreshData()
{
#if 0
    m_insGnss = m_dataBus->GetInsGnssData();
    m_dmeData = m_dataBus->GetDmeData();
    m_vorils = m_dataBus->GetVorilsData();
    m_adfData = m_dataBus->GetAdfData();

    m_aircraftData = m_dataBus->GetAircraftInstrumentData();
#endif
}

float CHmsHSIManager::GetVorQDM()
{
    return QDRToQDM(m_vorils.azimuth);
}

float CHmsHSIManager::GetAdfQDM()
{
    return RBToQDM(m_adfData.azimuth, m_insGnss.yaw);
}

float CHmsHSIManager::GetTrack()
{
    return m_aircraftData->track;
}

bool CHmsHSIManager::VorUsable()
{
    return m_aircraftData->vorBearingValid;
}

bool CHmsHSIManager::AdfUsable()
{
    return m_aircraftData->adfBearingValid;
}

bool CHmsHSIManager::TrackUsable()
{
    return m_aircraftData->trackUsable;
}

bool CHmsHSIManager::IsLocalizer()
{
    return m_aircraftData->hasLocalizer;
}

CHmsHSI::NAV_TOFROM_MODE CHmsHSIManager::GetNavToFromMode(CHmsHSI::NAV_SRC navSrc, const float &course)
{
    if (navSrc == CHmsHSI::NAV_GPS || 
        (navSrc == CHmsHSI::NAV_ADF && !AdfUsable()) ||
        navSrc == CHmsHSI::NAV_VOR && !VorUsable())//GPS时无向背台
    {
        return CHmsHSI::NAV_MODE_NONE;
    }
    else if (navSrc == CHmsHSI::NAV_LOC)//LOC时永远为向台
    {
        return CHmsHSI::NAV_MODE_TO;
    }

    float qdr = GetQDR(navSrc);
    return CalToFromByQDR(course, qdr);
}

float CHmsHSIManager::GetCourceDeviation(CHmsHSI::NAV_SRC navSrc, const float &course)
{
    if (navSrc == CHmsHSI::NAV_ADF && !AdfUsable())
    {
        return 0.0;
    }

    if (navSrc == CHmsHSI::NAV_VOR && !VorUsable())
    {
        return 0.0;
    }

    float deviation = 0.0f;
    switch (navSrc)
    {
    case CHmsHSI::NAV_GPS:
        //暂不处理
        break;
    case CHmsHSI::NAV_ADF:
    case CHmsHSI::NAV_VOR:
        deviation = NumberMap(CalCourceDeviation(course, GetQDR(navSrc)), -10.0, 10.0, -2, 2);
        break;
    case CHmsHSI::NAV_LOC:
        deviation = m_vorils.locDeviation;
        break;
    default:
        break;
    }
    return deviation;
}

float CHmsHSIManager::GetMagHeading()
{
    return m_aircraftData->magneticHeading;
}

float CHmsHSIManager::GetTurnRate()
{
    return m_aircraftData->turnRate;
}

void CHmsHSIManager::rotateNodeOnCenter(HmsAviPf::CHmsNode *target, const float &angle, const HmsAviPf::Vec2 &center, const float &radius)
{
    double rad = ToRadian(angle);
    float offsetX = radius * sin(rad);
    float offsetY = radius * cos(rad);
    Vec2 newPos = center + Vec2(offsetX, offsetY);
    target->SetPosition(newPos);
    target->SetRotation(angle);
}

float CHmsHSIManager::RBToQDM(float fRb, float fMh)
{
    float qdm = fRb + fMh;
    qdm = qdm > 360 ? qdm - 360 : qdm;
    return qdm;
}

float CHmsHSIManager::QDRToQDM(float fQdr)
{
    float qdm = fQdr + 180.0f;
    if (qdm > 360.0f)
    {
        qdm = qdm - 360.0f;
    }
    return qdm;
}

float CHmsHSIManager::QDMToQDR(float fQdm)
{
    float qdr = fQdm - 180.0f;
    if (qdr < 0.0f)
    {
        qdr = qdr + 360.0f;
    }
    return qdr;
}

float CHmsHSIManager::GetQDR(CHmsHSI::NAV_SRC navSrc)
{
    float qdr = -1.0f;
    if (navSrc == CHmsHSI::NAV_GPS)
    {
        //需使用航路和GPS数据进行计算
    }
    else if (navSrc == CHmsHSI::NAV_VOR || navSrc == CHmsHSI::NAV_LOC || navSrc == CHmsHSI::NAV_ADF)
    {
        if (navSrc == CHmsHSI::NAV_VOR || navSrc == CHmsHSI::NAV_LOC)
        {
            qdr = m_vorils.azimuth;
            qdr = qdr < 0 ? qdr + 360 : qdr;
        }
        else
        {
            qdr = QDMToQDR(RBToQDM(m_adfData.azimuth, m_insGnss.yaw));
        }
    }
    return qdr;
}

CHmsHSI::NAV_TOFROM_MODE CHmsHSIManager::CalToFromByQDR(const float &course, const float &qdr)
{
    float differ = fabs(qdr - course);
    if (differ == 90.0f || differ == 270.0f)
    {
        return CHmsHSI::NAV_MODE_NONE;
    }

    //计算背台所处角度范围
    float maxBearing = course + 90;
    float minBearing = course - 90;

    CHmsHSI::NAV_TOFROM_MODE mode = CHmsHSI::NAV_MODE_TO;
    if (minBearing < 0.0f)
    {
        float min1 = minBearing + 360;
        float max1 = 360;
        float min2 = 0;
        float max2 = maxBearing;
        if ((qdr >= min1 && qdr <= max1) || (qdr >= min2 && qdr <= max2))
        {
            mode = CHmsHSI::NAV_MODE_FROM;
        }
    }
    else if (maxBearing > 360.0f)
    {
        float min1 = minBearing;
        float max1 = 360;
        float min2 = 0;
        float max2 = maxBearing - 360.0f;
        if ((qdr >= min1 && qdr <= max1) || (qdr >= min2 && qdr <= max2))
        {
            mode = CHmsHSI::NAV_MODE_FROM;
        }
    }

    if (qdr >= minBearing && qdr <= maxBearing)
    {
        mode = CHmsHSI::NAV_MODE_FROM;
    }

    return mode;
}

float CHmsHSIManager::CalCourceDeviation(const float &course, const float &qdr)
{
    float cd = qdr - course;
    //计算偏差
    float absDiff = fabs(cd);
    if (absDiff > 90.0f && absDiff < 270.0f)
    {
        cd = 180 - absDiff;
    }
    else if (absDiff > 270.0f)
    {
        cd = 360 - absDiff;
    }
    //计算偏差偏移方向
    float calCrs = course;
    float reserveFlag = 1;
    if (calCrs > 180.0f)
    {
        calCrs = calCrs - 180;
        reserveFlag = -1;
    }
    if ((qdr >= 0 && qdr <= calCrs) || (qdr >= calCrs + 180.0f && qdr <= 360.0f))
    {
        cd = fabs(cd) * reserveFlag;
    }
    else if ((qdr >= calCrs && qdr <= calCrs + 180.0f))
    {
        cd = -fabs(cd) * reserveFlag;
    }

    if (cd < -10.0) cd = -10;
    if (cd > 10.0) cd = 10;
    return cd;
}

bool CHmsHSIManager::ContainsPoint(const HmsAviPf::Vec2 &center, const float &radius, const HmsAviPf::Vec2 &point)
{
    float distance = std::sqrt(std::pow(point.x - center.x, 2) + std::pow(point.y - center.y, 2));
    if (distance - radius < 0.0001)
    {
        return true;
    }

    return false;
}

float CHmsHSIManager::CalAngleWithZero(const Vec2 &point1)
{
    double ma_x = point1.x;
    double ma_y = point1.y;
    double tolerate = 0.000001;

    double angle = atan(ma_x / ma_y) * 180 / M_PI;


    if (ma_x == 0 && ma_y > 0)
    {
        return 0;
    }
    else if (ma_x == 0 && ma_y < 0)
    {
        return 180;
    }
    else if (ma_x > 0 && ma_y == 0)
    {
        return 90.0;
    }
    else if (ma_x < 0 && ma_y == 0)
    {
        return 270.0;
    }

    if (ma_x >= 0 && ma_y <= 0)
    {
        angle = 180 - fabs(angle);
    }
    else if (ma_x <= 0 && ma_y <= 0)
    {
        angle += 180.0f;
    }
    else if (ma_x <= 0 && ma_y >= 0)
    {
        angle = 360 - fabs(angle);
    }
    return angle;
}

float CHmsHSIManager::CalTwoPointsAngle(const Vec2 &center, const Vec2 &point1, const Vec2 &point2)
{
    double ma_x = point1.x - center.x;
    double ma_y = point1.y - center.y;
    double mb_x = point2.x - center.x;
    double mb_y = point2.y - center.y;

    double angle1 = CalAngleWithZero(Vec2(ma_x, ma_y));
    double angle2 = CalAngleWithZero(Vec2(mb_x, mb_y));

    float diff = angle2 - angle1;
    if (angle1 > 270.0f && angle2 == 0)
    {
        diff = 360 - angle1;
    }
    if (angle2 > 270.0f && angle1 < 90.0f)
    {
        diff = -360 + diff;
    }
    if (angle1 > 270.0f && angle2 < 90.0f)
    {
        diff = 360 - diff;
    }
    return diff;
}

void CHmsHSIManager::AngleNormalize(float &degree)
{
    float temp = degree;
    int ratio = 1;// degree / 360;
    if (degree > 360.0f)
    {
        temp = degree - int(degree / 360) * 360;
    }
    else if (degree <= 0)
    {
        temp = 360 - degree;
    }
    degree = temp;
}

void CHmsHSIManager::SendToFrom(CHmsHSI::NAV_TOFROM_MODE mode)
{
    CHmsDataSender *sender = CHmsGlobal::GetInstance()->GetDataSender();
    SettingData *settingData = CHmsGlobal::GetInstance()->GetDataBus()->GetSettingData();

    settingData->fromOrTo = mode == CHmsHSI::NAV_MODE_TO ? 0x01 : 0x02;
    sender->SendSettingData(settingData);
}

void CHmsHSIManager::SendCourseDeviation(float fCourseDeviation)
{
    float sendCD = fCourseDeviation;

    if (fabs(m_preSendCD - sendCD) < 0.1)
    {
        return;
    }

    CHmsDataSender *sender = CHmsGlobal::GetInstance()->GetDataSender();
    SettingData* settingData = CHmsGlobal::GetInstance()->GetDataBus()->GetSettingData();
    settingData->dataValid.courseDeviationValid= 0x01;
    settingData->courseDeviation = sendCD;
    sender->SendSettingData(settingData);

    m_preSendCD = sendCD;
}
#else
void CHmsHSIManager::rotateNodeOnCenter(HmsAviPf::CHmsNode *target, const float &angle, const HmsAviPf::Vec2 &center, const float &radius)
{
    double rad = ToRadian(angle);
    float offsetX = radius * sin(rad);
    float offsetY = radius * cos(rad);
    Vec2 newPos = center + Vec2(offsetX, offsetY);
    target->SetPosition(newPos);
    target->SetRotation(angle);
}

float CHmsHSIManager::CalCourceDeviation(const float &course, const float &qdr)
{
    float cd = qdr - course;
    //计算偏差
    float absDiff = fabs(cd);
    if (absDiff > 90.0f && absDiff < 270.0f)
    {
        cd = 180 - absDiff;
    }
    else if (absDiff > 270.0f)
    {
        cd = 360 - absDiff;
    }
    //计算偏差偏移方向
    float calCrs = course;
    float reserveFlag = 1;
    if (calCrs > 180.0f)
    {
        calCrs = calCrs - 180;
        reserveFlag = -1;
    }
    if ((qdr >= 0 && qdr <= calCrs) || (qdr >= calCrs + 180.0f && qdr <= 360.0f))
    {
        cd = fabs(cd) * reserveFlag;
    }
    else if ((qdr >= calCrs && qdr <= calCrs + 180.0f))
    {
        cd = -fabs(cd) * reserveFlag;
    }

    if (cd < -10.0) cd = -10;
    if (cd > 10.0) cd = 10;
    return cd;
}

bool CHmsHSIManager::ContainsPoint(const HmsAviPf::Vec2 &center, const float &radius, const HmsAviPf::Vec2 &point)
{
    float distance = std::sqrt(std::pow(point.x - center.x, 2) + std::pow(point.y - center.y, 2));
    if (distance - radius < 0.0001)
    {
        return true;
    }

    return false;
}

float CHmsHSIManager::CalAngleWithZero(const Vec2 &point1)
{
    double ma_x = point1.x;
    double ma_y = point1.y;
    double tolerate = 0.000001;

    double angle = atan(ma_x / ma_y) * 180 / SIM_Pi;


    if (ma_x == 0 && ma_y > 0)
    {
        return 0;
    }
    else if (ma_x == 0 && ma_y < 0)
    {
        return 180;
    }
    else if (ma_x > 0 && ma_y == 0)
    {
        return 90.0;
    }
    else if (ma_x < 0 && ma_y == 0)
    {
        return 270.0;
    }

    if (ma_x >= 0 && ma_y <= 0)
    {
        angle = 180 - fabs(angle);
    }
    else if (ma_x <= 0 && ma_y <= 0)
    {
        angle += 180.0f;
    }
    else if (ma_x <= 0 && ma_y >= 0)
    {
        angle = 360 - fabs(angle);
    }
    return angle;
}

float CHmsHSIManager::CalTwoPointsAngle(const Vec2 &center, const Vec2 &point1, const Vec2 &point2)
{
    double ma_x = point1.x - center.x;
    double ma_y = point1.y - center.y;
    double mb_x = point2.x - center.x;
    double mb_y = point2.y - center.y;

    double angle1 = CalAngleWithZero(Vec2(ma_x, ma_y));
    double angle2 = CalAngleWithZero(Vec2(mb_x, mb_y));

    float diff = angle2 - angle1;
    if (angle1 > 270.0f && angle2 == 0)
    {
        diff = 360 - angle1;
    }
    if (angle2 > 270.0f && angle1 < 90.0f)
    {
        diff = -360 + diff;
    }
    if (angle1 > 270.0f && angle2 < 90.0f)
    {
        diff = 360 - diff;
    }
    return diff;
}

void CHmsHSIManager::AngleNormalize(float &degree)
{
    float temp = degree;
    int ratio = 1;// degree / 360;
    if (degree > 360.0f)
    {
        temp = degree - int(degree / 360) * 360;
    }
    else if (degree <= 0)
    {
        temp = 360 - degree;
    }
    degree = temp;
}
#endif