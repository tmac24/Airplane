#include "HmsNavSvsCtrlPageRoam.h"
#include "NavSvs/HmsNavSvsModuleMgr.h"
#include "NavSvs/PositionMgr/HmsNavSvsPositionRoam.h"
#include "HmsGlobal.h"
#include "NavSvs/HmsNavSvsMathHelper.h"

CHmsNavSvsCtrlPageRoam::CHmsNavSvsCtrlPageRoam()
{
    m_type = NavSvsCtrlPageType::CTRLPAGE_ROAM;
    m_bPressOnCtrlPoint = false;
    m_bPressChangeAttitude = false;
}

CHmsNavSvsCtrlPageRoam::~CHmsNavSvsCtrlPageRoam()
{

}

bool CHmsNavSvsCtrlPageRoam::Init(const HmsAviPf::Size & size)
{
    if (!CHmsNavSvsCtrlPageBase::Init(size))
    {
        return false;
    }
    SetContentSize(size);

    auto pImage = CHmsImageNode::Create("res/NavSvs/ctrlbg.png");
    pImage->SetAnchorPoint(Vec2(0, 0));
    pImage->SetPosition(50, 50);
    pImage->SetOpacity(100);
    pImage->SetColor(Color3B::BLACK);
    this->AddChild(pImage);
    m_pCtrlPointBgImageNode = pImage;

    pImage = CHmsImageNode::Create("res/NavSvs/ctrlPoint.png");
    pImage->SetAnchorPoint(Vec2(0.5, 0.5));
    pImage->SetPosition(m_pCtrlPointBgImageNode->GetContentSize() * 0.5);
    pImage->SetOpacity(100);
    pImage->SetColor(Color3B::WHITE);
    m_pCtrlPointBgImageNode->AddChild(pImage);
    m_pCtrlPointImageNode = pImage;

    m_pVSlideBar = CHmsVSlideBar::Create(Size(150, size.height - 100));
    m_pVSlideBar->SetAnchorPoint(Vec2(0, 0));
    m_pVSlideBar->SetPosition(Vec2(size.width - 200, 50));
    m_pVSlideBar->SetValueRange(0, 10000);
    this->AddChild(m_pVSlideBar);

    auto pLabel = CHmsLabel::createWithTTF("", "fonts/simsun.ttc", 32);
    pLabel->SetAnchorPoint(Vec2(1, 1));
    pLabel->SetPosition(Vec2(size.width - 250, size.height - 50));
    this->AddChild(pLabel);
    m_pInfoLabel = pLabel;

    this->RemoveAllChildInterface();
    this->RegisterAllNodeChild();

    return true;
}

template<class T>
void FillDisplayData(std::stringstream & s, const char * strTitle, T t, const char * strUnit)
{
	s << strTitle;
	s << t;
	if (strUnit)
	{
		s << strUnit;
	}
	s << '\n';
}

#include "Language/HmsLanguageMananger.h"

void CHmsNavSvsCtrlPageRoam::Update(float delta)
{
    CHmsNavSvsCtrlPageBase::Update(delta);

    auto pPositionMgr = dynamic_cast<CHmsNavSvsPositionRoam*>(CHmsNavSvsModuleMgr::GetInstance()->GetCurPositionMgr());
    if (pPositionMgr)
	{
        m_pVSlideBar->SetCurValue(pPositionMgr->GetHeight());

		std::stringstream s;
		s.setf(std::ios::fixed);
		s.precision(6);

		if (CHmsLanguageMananger::GetInstance()->GetLanguageName() == "CHN")
		{
			FillDisplayData(s, "\xE7\xBB\x8F\xE5\xBA\xA6\x3A\x20", pPositionMgr->GetLon(), nullptr);
			FillDisplayData(s, "\xE7\xBA\xAC\xE5\xBA\xA6\x3A\x20", pPositionMgr->GetLat(), nullptr); 
			s.precision(0);
			FillDisplayData(s, "\xE9\xAB\x98\xE5\xBA\xA6\x3A\x20", pPositionMgr->GetHeight(), "m");
			s.precision(2);
			FillDisplayData(s, "\xE5\x81\x8F\xE8\x88\xAA\x3A\x20", pPositionMgr->GetYaw(), "\302\260");
			FillDisplayData(s, "\xE4\xBF\xAF\xE4\xBB\xB0\x3A\x20", pPositionMgr->GetPitch(), "\302\260");
			FillDisplayData(s, "\xE6\xBB\x9A\xE8\xBD\xAC\x3A\x20", pPositionMgr->GetRoll(), "\302\260");
			FillDisplayData(s, "\xE9\x80\x9F\xE5\xBA\xA6\x3A\x20", pPositionMgr->GetSpeed(), "Km/h");
		}
		else
		{
			FillDisplayData(s, "Longitude ", pPositionMgr->GetLon(), nullptr);
			FillDisplayData(s, "Latitude  ", pPositionMgr->GetLat(), nullptr);
			s.precision(0);
			FillDisplayData(s, "Altitude  ", pPositionMgr->GetHeight(), "m");
			s.precision(2);
			FillDisplayData(s, "Yaw       ", pPositionMgr->GetYaw(), "\302\260");
			FillDisplayData(s, "Pitch     ", pPositionMgr->GetPitch(), "\302\260");
			FillDisplayData(s, "Roll      ", pPositionMgr->GetRoll(), "\302\260");
			FillDisplayData(s, "Speed     ", pPositionMgr->GetSpeed(), "Km/h");
		}
		
        m_pInfoLabel->SetString(s.str());
    }
}

bool CHmsNavSvsCtrlPageRoam::OnPressed(const HmsAviPf::Vec2 & posOrigin)
{
    auto rect = m_pCtrlPointBgImageNode->GetRectFromParent();
    if (rect.containsPoint(posOrigin))
    {
        m_bPressOnCtrlPoint = true;
    }
    else
    {
        m_bPressOnCtrlPoint = false;
        
        if (HmsUiEventInterface::OnPressed(posOrigin))
        {
            m_bPressChangeAttitude = false;
        }
        else
        {
            m_bPressChangeAttitude = true;
            m_pressPos = posOrigin;
        }
    }

    return true;
}

void CHmsNavSvsCtrlPageRoam::OnMove(const HmsAviPf::Vec2 & posOrigin)
{
    if (m_bPressOnCtrlPoint)
    {
        auto pos = posOrigin - m_pCtrlPointBgImageNode->GetPoistion();
        pos.x = SVS_BETWEEN_VALUE(pos.x, 0, m_pCtrlPointBgImageNode->GetContentSize().width);
        pos.y = SVS_BETWEEN_VALUE(pos.y, 0, m_pCtrlPointBgImageNode->GetContentSize().height);

        Vec2 center = m_pCtrlPointBgImageNode->GetContentSize() * 0.5;
        float dis = sqrt(pow(pos.x - center.x, 2) + pow(pos.y - center.y, 2));
        while (dis > 250)
        {
            if (pos.x < center.x)
            {
                pos.x += 1;
            }
            else
            {
                pos.x -= 1;
            }
            if (pos.y < center.y)
            {
                pos.y += 1;
            }
            else
            {
                pos.y -= 1;
            }
            dis = sqrt(pow(pos.x - center.x, 2) + pow(pos.y - center.y, 2));
        }

        m_pCtrlPointImageNode->SetPosition(pos);

        UpdateCtrlData();
    }
    else
    {
        if (m_bPressChangeAttitude)
        {
            auto pPositionMgr = dynamic_cast<CHmsNavSvsPositionRoam*>(CHmsNavSvsModuleMgr::GetInstance()->GetCurPositionMgr());
            if (pPositionMgr)
            {
                auto yaw = pPositionMgr->GetYaw();
                yaw -= (m_pressPos.x - posOrigin.x) / 2560.0 * 360;
                pPositionMgr->SetYaw(yaw);

                auto pitch = pPositionMgr->GetPitch();
                pitch -= (m_pressPos.y - posOrigin.y) / 1440.0 * 180;
                pPositionMgr->SetPitch(pitch);

                m_pressPos = posOrigin;
            }          
        }
        else
        {
            HmsUiEventInterface::OnMove(posOrigin);

            auto pPositionMgr = dynamic_cast<CHmsNavSvsPositionRoam*>(CHmsNavSvsModuleMgr::GetInstance()->GetCurPositionMgr());
            if (pPositionMgr)
            {
                auto height = m_pVSlideBar->GetCurValue();
                pPositionMgr->SetHeight(height);
            }
        }  
    }
}

void CHmsNavSvsCtrlPageRoam::OnReleased(const HmsAviPf::Vec2 & posOrigin)
{
    if (m_bPressOnCtrlPoint)
    {
        auto pos = Vec2(m_pCtrlPointBgImageNode->GetContentSize() * 0.5);
        m_pCtrlPointImageNode->SetPosition(pos);

        UpdateCtrlData();
    }
    else
    {
        if (m_bPressChangeAttitude)
        {

        }
        else
        {
            HmsUiEventInterface::OnReleased(posOrigin);
        }
    }
}

void CHmsNavSvsCtrlPageRoam::OnEnter()
{
    CHmsNavSvsCtrlPageBase::OnEnter();
    SetScheduleUpdate();
}

void CHmsNavSvsCtrlPageRoam::OnExit()
{
    CHmsNavSvsCtrlPageBase::OnExit();
    SetUnSchedule();
}

void CHmsNavSvsCtrlPageRoam::UpdateCtrlData()
{
    auto GetCtrlValue = [](float y, int wh)
    {
        float pitch = 0;
        float yabs = fabsf(y);
        float sign = 1;
        if (yabs != 0)
        {
            sign = y / yabs;
        }
        if (yabs < wh * 0.25)
        {
            pitch = yabs * (0.3f / (wh * 0.25f));
        }
        else
        {
            pitch = (yabs - wh * 0.25f) * (0.7f / (wh * 0.25f)) + 0.3f;
        }
        return pitch * sign;
    };

    auto pos = m_pCtrlPointImageNode->GetPoistion();

    int width = m_pCtrlPointBgImageNode->GetContentSize().width;
    int height = m_pCtrlPointBgImageNode->GetContentSize().height;
    float x = pos.x - width * 0.5f;
    float y = pos.y - height * 0.5f;
    float forwardSpeed = GetCtrlValue(y, height) * 5000;
    float rightSpeed = GetCtrlValue(x, width) * 5000;

    auto pPositionMgr = dynamic_cast<CHmsNavSvsPositionRoam*>(CHmsNavSvsModuleMgr::GetInstance()->GetCurPositionMgr());
    if (pPositionMgr)
    {
        pPositionMgr->SetSpeed(forwardSpeed);
        pPositionMgr->SetRightSpeed(rightSpeed);
    }
}