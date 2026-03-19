#include "HmsAirportInfo.h"
#include "HmsGlobal.h"
#include "HmsNavImageSql.h"
#include "HmsNavMgr.h"
#include "HmsLayerAirportProgram.h"
#include "ToolBar/HmsAvionicsDisplayMgr.h"
#include "HmsLayerAirports.h"
#include "NavSvs/HmsNavSvsLayer.h"
#include "Nav/HmsNavComm.h"

CHmsAirportInfo::CHmsAirportInfo()
: HmsUiEventInterface(this)
{

	//add by MK init value
	m_pLabelTitle = NULL;

	m_pLabelElevation = NULL;
	m_pLabelLongestRunway = NULL;
	m_pLabelMagneticVar = NULL;
	m_pLabelTransitAlt = NULL;

	m_pLabelAtis = NULL;
	m_pLabelClearance = NULL;
	m_pLabelGround = NULL;
	m_pLabelTower = NULL;
	m_pTabPage = NULL;
		
	m_pTabPageStuFreq = NULL;
	m_pTabPageStuRunway = NULL;
	m_pTabPageStuProcedure = NULL;
}

CHmsAirportInfo::~CHmsAirportInfo()
{

}

bool CHmsAirportInfo::Init(const HmsAviPf::Size & size)
{
    if (!CHmsNode::Init())
    {
        return false;
    }
    SetContentSize(size);
    {
        auto pBgDrawNode = CHmsDrawNode::Create();

		if (pBgDrawNode != NULL)//modefy by MK for check nullptr
		{
			pBgDrawNode->DrawSolidRect(Rect(0, 0, size.width, size.height), Color4F(CHmsAirportConstant::s_airportWhite));
			this->AddChild(pBgDrawNode);
		}        
    }

    auto pPlaneImage = CHmsImageNode::Create("res/AirportInfo/planeIcon.png");
	if (NULL != pPlaneImage)//modefy by MK for check nullptr
	{
		pPlaneImage->SetAnchorPoint(Vec2(0, 0.5));
		this->AddChild(pPlaneImage);
		pPlaneImage->SetPosition(Vec2(50, size.height - 100));
	}
    

    m_pLabelTitle = CHmsGlobal::CreateUTF8Label((const char*)m_airportInfoStu.name.c_str(), CHmsAirportConstant::s_fontSize, true);
	if (NULL != m_pLabelTitle)//modefy by MK for check nullptr
	{
		m_pLabelTitle->SetTextColor(Color4B::BLACK);
		this->AddChild(m_pLabelTitle);
		m_pLabelTitle->SetPosition(Vec2(50 + 350, size.height - 100));
	}
   

    auto pBtn = CHmsUiStretchButton::Create("res/AirportInfo/Round4SolidWhite.png", Size(96, 96)
        , CHmsAirportConstant::s_airportGray, CHmsAirportConstant::s_airportBlue);
	if (NULL != pBtn)//modefy by MK for check nullptr
	{
		this->AddChild(pBtn);
		pBtn->SetAnchorPoint(Vec2(1, 0.5));
		pBtn->SetPosition(Vec2(size.width - 50, size.height - 100));
		pBtn->SetOnClickedFunc([=](CHmsUiButtonAbstract *pBtn)
		{

			if (m_airportInfoStu.icaoCode.empty())
			{
				return;
			}
			auto p = CHmsAvionicsDisplayMgr::GetInstance()->GetNavType(HmsPageType::Page3DView);
			auto pNavSvsLayer = dynamic_cast<CHmsNavSvsLayer*>(p);
			if (pNavSvsLayer)
			{
				double lon, lat, height;
				CHms424Database::GetInstance()->GetAirportLonLatHeightByIdent(lon, lat, height, m_airportInfoStu.icaoCode);
				height = FeetToMeter(height);
				pNavSvsLayer->SetAirportViewMode(lon, lat, height);

				CHmsAvionicsDisplayMgr::GetInstance()->GetToolBarBottom()->SetCurrentMode("3DView");
			}
		});
		auto p3dLabel = CHmsGlobal::CreateLanguageLabel("3DView", 20, Color4F::BLACK, true);
		if (NULL != p3dLabel)//modefy by MK for check nullptr
		{
			p3dLabel->SetAnchorPoint(Vec2(0.5, 0));
			p3dLabel->SetPosition(Vec2(pBtn->GetContentSize().width * 0.5, 3));
			pBtn->AddChild(p3dLabel);
		}
		
		auto p3dImage = CHmsImageNode::Create("res/AirportInfo/3DViewIcon.png");
		if (NULL != p3dImage)//modefy by MK for check nullptr
		{
			p3dImage->SetAnchorPoint(Vec2(0.5, 1));
			p3dImage->SetPosition(Vec2(pBtn->GetContentSize().width * 0.5, pBtn->GetContentSize().height - 10));
			p3dImage->SetColor(Color3B::BLACK);
			pBtn->AddChild(p3dImage);
		}		
	}
    

    {
        auto CreateTitleLabel = [=](const char* text, int &row, int &column){
            if (column == 4)
            {
                column = 0;
                ++row;
            }

            CHmsLabel *pLabel = nullptr;
            int xPos = 0;
            switch (column)
            {
            case 0:
                xPos = 50 + column * ((size.width - 100) / 4.0);
                pLabel = CHmsGlobal::CreateLanguageLabel(text, CHmsAirportConstant::s_fontSize);
                break;
            case 1:
                xPos = 50 + column * ((size.width - 100) / 4.0) - 100;
                pLabel = CHmsGlobal::CreateLanguageLabel(text, CHmsAirportConstant::s_fontSize, true);
                break;
            case 2:
                xPos = 50 + column * ((size.width - 100) / 4.0) - 100;
                pLabel = CHmsGlobal::CreateLanguageLabel(text, CHmsAirportConstant::s_fontSize);
                break;
            case 3:
                xPos = 50 + column * ((size.width - 100) / 4.0) - 300;
                pLabel = CHmsGlobal::CreateLanguageLabel(text, CHmsAirportConstant::s_fontSize, true);
                break;

            default:
                break;
            }

            auto rowHeight = CHmsAirportConstant::s_buttonSize.height - 30;
			if (NULL != pLabel)//modefy by MK for check nullptr
			{
				pLabel->SetAnchorPoint(Vec2(0, 0.5));
				pLabel->SetPosition(Vec2(xPos, size.height * 0.6 + 50 + (4 - row) * rowHeight));
				pLabel->SetTextColor(Color4B::BLACK);
				this->AddChild(pLabel);
			}            

            ++column;

            return pLabel;
        };
        int row = 0;
        int column = 0;
        CHmsLabel *pLabel = nullptr;
        pLabel = CreateTitleLabel("Elevation", row, column); 
        m_pLabelElevation = CreateTitleLabel(m_airportInfoStu.elevation.c_str(), row, column);

        pLabel = CreateTitleLabel("Atis", row, column);
        m_pLabelAtis = CreateTitleLabel(m_airportInfoStu.atis.c_str(), row, column);

        pLabel = CreateTitleLabel("LongestRunway", row, column);
        m_pLabelLongestRunway = CreateTitleLabel(m_airportInfoStu.longestRunway.c_str(), row, column);

        pLabel = CreateTitleLabel("Clearance", row, column);
        m_pLabelClearance = CreateTitleLabel(m_airportInfoStu.clearance.c_str(), row, column);

        pLabel = CreateTitleLabel("MagneticVar", row, column);
        m_pLabelMagneticVar = CreateTitleLabel(m_airportInfoStu.magneticVar.c_str(), row, column);

        pLabel = CreateTitleLabel("Ground", row, column);
        m_pLabelGround = CreateTitleLabel(m_airportInfoStu.ground.c_str(), row, column);

        pLabel = CreateTitleLabel("TransitAlt", row, column);
        m_pLabelTransitAlt = CreateTitleLabel(m_airportInfoStu.transitAlt.c_str(), row, column);

        pLabel = CreateTitleLabel("Tower", row, column);
        m_pLabelTower = CreateTitleLabel(m_airportInfoStu.tower.c_str(), row, column);    
    }

    m_pTabPage = CHmsAirportInfoTabPage::Create(Size(size.width, size.height * 0.55 + 40));
	if (NULL != m_pTabPage)//modefy by MK for check nullptr
	{
		this->AddChild(m_pTabPage);
		m_pTabPageStuFreq = m_pTabPage->AddPage("Frequencies");
		if (NULL != m_pTabPageStuFreq)//modefy by MK for check nullptr
		{
			m_pTabPageStuFreq->pBtn->Selected();
		}
		m_pTabPageStuRunway = m_pTabPage->AddPage("Runways");
		m_pTabPageStuProcedure = m_pTabPage->AddPage("Procedures");
	}
    

    this->RegisterAllNodeChild();
    return true;
}

void CHmsAirportInfo::SetIcaoCode(string code)
{
    UpdateAirportInfo(code);
    UpdateFrequencies(code);
    UpdateRunways(code);
    UpdateProcedures(code);

    m_pTabPageStuFreq->pBtn->Selected();
}

void CHmsAirportInfo::UpdateAirportInfo(const string &code)
{
    auto airportStu = CHms424Database::GetInstance()->GetAirportInfo(code);

    m_airportInfoStu.icaoCode = code;
    m_airportInfoStu.name = airportStu.airportName;

    char tmp[256] = { 0 };

    if (CHmsNavComm::GetInstance()->IsMetric())
    {
        sprintf(tmp, "%dm MSL", (int)FeetToMeter(airportStu.airportElev));
        m_airportInfoStu.elevation = tmp;
        sprintf(tmp, "%dm MSL", (int)FeetToMeter(airportStu.longestRunway));
        m_airportInfoStu.longestRunway = tmp;
    }
    else
    {
        sprintf(tmp, "%d MSL", airportStu.airportElev);
        m_airportInfoStu.elevation = tmp;
        sprintf(tmp, "%d MSL", airportStu.longestRunway);
        m_airportInfoStu.longestRunway = tmp;
    }
    

    sprintf(tmp, "%.5f", airportStu.magneticVar);
    m_airportInfoStu.magneticVar = tmp;
    if (airportStu.transitAlt == -1)
    {
        m_airportInfoStu.transitAlt = "-----";
    }
    else
    {
        if (CHmsNavComm::GetInstance()->IsMetric())
        {
            sprintf(tmp, "%dm MSL", (int)FeetToMeter(airportStu.transitAlt));
        }
        else
        {
            sprintf(tmp, "%d MSL", airportStu.transitAlt);
        }
        
        m_airportInfoStu.transitAlt = tmp;
    }

    auto GetFreqString = [=](std::string type){

        std::string freqTmp = "-----";
        auto iter = m_mapFreqDbData.find(type);
        if (iter != m_mapFreqDbData.end())
        {
            freqTmp = "";
			for (int i = 0; i < (int)iter->second.size() && i < 5; ++i)
            {
                char k[32] = { 0 };
                if (i == 0)
                {
                    sprintf(k, "%.2f", iter->second[i].frequency);
                }
                else
                {
                    sprintf(k, "   %.2f", iter->second[i].frequency);
                }

                freqTmp += k;
            }
            if (iter->second.size() > 5)
            {
                freqTmp += "...";
            }
        }
        return freqTmp;
    };
    m_mapFreqDbData.clear();
    CHms424Database::GetInstance()->GetAirportComm(code, m_mapFreqDbData);

    m_airportInfoStu.atis = GetFreqString("ATI");
    m_airportInfoStu.clearance = GetFreqString("CLD");
    m_airportInfoStu.ground = GetFreqString("GND");
    m_airportInfoStu.tower = GetFreqString("TWR");

    sprintf(tmp, "%s : %s\n%s", airportStu.icaoCode.c_str(), airportStu.airportName.c_str(),
        CHmsNavMathHelper::LonLatToString(airportStu.longitude, airportStu.latitude).c_str());

	if (NULL != m_pLabelTitle)
	{
		m_pLabelTitle->SetString(tmp);
	}
    
	if (NULL != m_pLabelElevation)
	{
		m_pLabelElevation->SetString(m_airportInfoStu.elevation.c_str());
	}
    
	if (NULL != m_pLabelLongestRunway)
	{
		m_pLabelLongestRunway->SetString(m_airportInfoStu.longestRunway.c_str());
	}
    
	if (NULL != m_pLabelMagneticVar)
	{
		m_pLabelMagneticVar->SetString(m_airportInfoStu.magneticVar.c_str());
	}
    
	if (NULL != m_pLabelTransitAlt)
	{
		m_pLabelTransitAlt->SetString(m_airportInfoStu.transitAlt.c_str());
	}

	if (NULL != m_pLabelAtis)
	{
		m_pLabelAtis->SetString(m_airportInfoStu.atis.c_str());
	}
    
	if (NULL != m_pLabelClearance)
	{
		m_pLabelClearance->SetString(m_airportInfoStu.clearance.c_str());
	}
    
	if (NULL != m_pLabelGround)
	{
		m_pLabelGround->SetString(m_airportInfoStu.ground.c_str());
	}
    
	if (NULL != m_pLabelTower)
	{
		m_pLabelTower->SetString(m_airportInfoStu.tower.c_str());
	}
}

void CHmsAirportInfo::GetFrequenciesData(const string &code)
{
    m_mapFreq.clear();
    char tmp[32];
    for (auto &m : m_mapFreqDbData)
    {
        for (auto &c : m.second)
        {
            AirportInfoFrequenciesStu stu;
            stu.type = c.commType;
            stu.name = c.commName;
            stu.describe = "";

            sprintf(tmp, "%.2f", c.frequency);
            stu.freq = tmp;

            m_mapFreq[stu.type].push_back(stu);
        }
    }
}

void CHmsAirportInfo::UpdateFrequencies(const string &code)
{
    GetFrequenciesData(code);


    auto OnRightListBtnClick = [=](CHmsUiButtonAbstract *pBtn, string str){

    };

    auto UpdateRightList = [=](const vector<AirportInfoFrequenciesStu> &vec){

        m_pTabPageStuFreq->pListRight->RemoveAllItem();
        for (auto &c : vec)
        {
            auto itemHeight = CHmsAirportConstant::s_listItemHeight;

            auto pButton = CHmsUiStretchButton::Create("res/AirportInfo/Round4SolidWhite.png",
                Size(m_pTabPageStuFreq->pListRight->GetContentSize().width, itemHeight));

			if (NULL != pButton)
			{
				pButton->SetAnchorPoint(Vec2(0, 1));

				auto str = c.name;
				if (!c.describe.empty())
				{
					str += "\n" + c.describe;
				}
				{
					auto pLabel = CHmsGlobal::CreateUTF8Label(str.c_str(), CHmsAirportConstant::s_fontSize, Color4F::BLACK, true);
					if (NULL != pLabel)
					{
						pLabel->SetAnchorPoint(Vec2(0, 0.5));
						pLabel->SetPosition(Vec2(10, itemHeight * 0.5));
						pButton->AddChild(pLabel);
					}					
				}
				{
					auto pLabel = CHmsGlobal::CreateUTF8Label(c.freq.c_str(), CHmsAirportConstant::s_fontSize, Color4F::BLACK, false);
					if (NULL != pLabel)
					{
						pLabel->SetAnchorPoint(Vec2(1, 0.5));
						pLabel->SetPosition(Vec2(m_pTabPageStuFreq->pListRight->GetContentSize().width - 10, itemHeight * 0.5));
						pButton->AddChild(pLabel);
					}					
				}

				m_pTabPageStuFreq->pListRight->AddWindowItem(pButton);
			}           
        }

    };

    static std::vector<CHmsUiButtonAbstract*> s_vecBtn;

    auto OnLeftListBtnClick = [=](CHmsUiButtonAbstract *pBtn, string str){

        if (!pBtn->IsSelected())
        {
            pBtn->Selected();
            return;
        }
        for (auto &c : s_vecBtn)
        {
            if (c != pBtn)
            {
                c->SetEnableCallback(false);
                c->SetTextColor(Color4B::BLACK);
                c->UnSelected();
                c->SetEnableCallback(true);
            }
        }

        auto iter = m_mapFreq.find(str);
        if (iter != m_mapFreq.end())
        {
            UpdateRightList(iter->second);
        }
    };
    auto UpdateLeftList = [=](const map<string, vector<AirportInfoFrequenciesStu> > &mapFreq){

        s_vecBtn.clear();

		if (NULL != m_pTabPageStuFreq)
		{
			m_pTabPageStuFreq->pListLeft->RemoveAllItem();
			m_pTabPageStuFreq->pListRight->RemoveAllItem();
			for (auto &c : mapFreq)
			{
				auto itemHeight = CHmsAirportConstant::s_listItemHeight;

				auto pButton = CHmsUiStretchButton::Create("res/AirportInfo/Round4SolidWhite.png",
					Size(m_pTabPageStuFreq->pListLeft->GetContentSize().width, itemHeight)
					, Color3B::WHITE, CHmsAirportConstant::s_airportBlue);
				if (NULL != pButton)
				{
					pButton->SetAnchorPoint(Vec2(0, 1));
					pButton->SetOnClickedFunc(std::bind(OnLeftListBtnClick, std::placeholders::_1, c.first));
					pButton->SetToggleBtn(true);

					auto pLabel = CHmsGlobal::CreateUTF8Label(c.first.c_str(), CHmsAirportConstant::s_fontSize, Color4F::BLACK, true);
					if (NULL != pLabel)
					{
						pLabel->SetAnchorPoint(Vec2(0, 0.5));
						pLabel->SetPosition(Vec2(10, itemHeight * 0.5));
						pButton->AddChild(pLabel);
					}					

					auto pImage = CHmsImageNode::Create("res/AirportInfo/RightPointer.png");
					if (NULL != pImage)
					{
						pImage->SetColor(CHmsAirportConstant::s_airportGray);
						pImage->SetAnchorPoint(Vec2(0.5, 0.5));
						pImage->SetPosition(Vec2(m_pTabPageStuFreq->pListLeft->GetContentSize().width - 30, itemHeight * 0.5));
						pButton->AddChild(pImage);
					}					

					m_pTabPageStuFreq->pListLeft->AddWindowItem(pButton);
					s_vecBtn.push_back(pButton);
				}				
			}
		}        
    };
    
    UpdateLeftList(m_mapFreq);
}

void CHmsAirportInfo::GetRunwaysData(const string &code)
{
    std::vector<DB_RunwayInfo> vecRunwayInfo;
    CHms424Database::GetInstance()->GetAirportRunway(code, vecRunwayInfo);
    m_mapRunway.clear();

    char tmp[256] = { 0 };
    for (auto &c : vecRunwayInfo)
    {
        AirportInfoRunwayStu stu;
        stu.runwayIdent = c.runwayIdent;
        if (CHmsNavComm::GetInstance()->IsMetric())
        {
            sprintf(tmp, "%dm x %dm", (int)FeetToMeter(c.rwLength), (int)FeetToMeter(c.rwWidth));
            stu.size = tmp;

            sprintf(tmp, "%d m", (int)FeetToMeter(c.thresholdEle));
            stu.thresholdEle = tmp;
        }
        else
        {
            sprintf(tmp, "%dft x %dft", c.rwLength, c.rwWidth);
            stu.size = tmp;

            sprintf(tmp, "%d ft", c.thresholdEle);
            stu.thresholdEle = tmp;
        }      

        sprintf(tmp, "%.3fM", c.bearing);
        stu.bearing = tmp;

        stu.lonLat = CHmsNavMathHelper::LonLatToString(c.longitude, c.latitude);      

        m_mapRunway[stu.runwayIdent] = stu;
    }
}

void CHmsAirportInfo::UpdateRunways(const string &code)
{
    GetRunwaysData(code);


    auto OnRightListBtnClick = [=](CHmsUiButtonAbstract *pBtn, string str){

    };

    auto CreateRightListButton = [=](const char *textA, const char *textB){

        auto itemHeight = CHmsAirportConstant::s_listItemHeight;

        auto pButton = CHmsUiStretchButton::Create("res/AirportInfo/Round4SolidWhite.png",
            Size(m_pTabPageStuRunway->pListRight->GetContentSize().width, itemHeight));
		if (pButton != NULL)
		{
			pButton->SetAnchorPoint(Vec2(0, 1));

			if (textA)
			{
				auto pLabel = CHmsGlobal::CreateUTF8Label(textA, CHmsAirportConstant::s_fontSize, Color4F::BLACK, true);
				pLabel->SetAnchorPoint(Vec2(0, 0.5));
				pLabel->SetPosition(Vec2(10, itemHeight * 0.5));
				pButton->AddChild(pLabel);
			}
			if (textB)
			{
				auto pLabel = CHmsGlobal::CreateUTF8Label(textB, CHmsAirportConstant::s_fontSize, Color4F::BLACK, false);
				pLabel->SetAnchorPoint(Vec2(1, 0.5));
				pLabel->SetPosition(Vec2(m_pTabPageStuRunway->pListRight->GetContentSize().width - 10, itemHeight * 0.5));
				pButton->AddChild(pLabel);
			}
			m_pTabPageStuRunway->pListRight->AddWindowItem(pButton);
		}
        return pButton;
    };
    auto CreateRightListTitle = [=](const char *text){

        auto itemHeight = CHmsAirportConstant::s_listItemHeight;

        auto node = CHmsNode::Create();
		if (NULL != node)
		{
			node->SetContentSize(Size(m_pTabPageStuRunway->pListRight->GetContentSize().width, itemHeight));
			node->SetAnchorPoint(Vec2(0, 1));

			auto pLabel = CHmsGlobal::CreateUTF8Label(text, CHmsAirportConstant::s_fontSize, Color4F(CHmsAirportConstant::s_airportBlue), true);
			if (NULL != pLabel)
			{
				pLabel->SetContentSize(Size(m_pTabPageStuRunway->pListRight->GetContentSize().width, itemHeight));
				pLabel->SetAnchorPoint(Vec2(0, 0.5));
				pLabel->SetPosition(Vec2(10, itemHeight * 0.5));
				node->AddChild(pLabel);
			}
			m_pTabPageStuRunway->pListRight->AddWindowItem(node);
		}        
    };

    auto UpdateRightList = [=](const AirportInfoRunwayStu &stu){

        m_pTabPageStuRunway->pListRight->RemoveAllItem();
        
        CreateRightListTitle("RUNWAY SIZE");
        CreateRightListButton(stu.size.c_str(), nullptr);
        CreateRightListTitle("HEADING");
        CreateRightListButton(stu.bearing.c_str(), nullptr);
        CreateRightListTitle("POSITION");
        CreateRightListButton(stu.lonLat.c_str(), nullptr);
        CreateRightListTitle("ELEVATION");
        CreateRightListButton(stu.thresholdEle.c_str(), nullptr);
    };

    static std::vector<CHmsUiButtonAbstract*> s_vecBtn;

    auto OnLeftListBtnClick = [=](CHmsUiButtonAbstract *pBtn, string str){

        if (!pBtn->IsSelected())
        {
            pBtn->Selected();
            return;
        }
        for (auto &c : s_vecBtn)
        {
            if (c != pBtn)
            {
                c->SetEnableCallback(false);
                c->SetTextColor(Color4B::BLACK);
                c->UnSelected();
                c->SetEnableCallback(true);
            }
        }

        auto iter = m_mapRunway.find(str);
        if (iter != m_mapRunway.end())
        {
            UpdateRightList(iter->second);
        }
    };
    auto UpdateLeftList = [=](const std::map<std::string, AirportInfoRunwayStu> &mapRunway){

        s_vecBtn.clear();
        m_pTabPageStuRunway->pListLeft->RemoveAllItem();
        m_pTabPageStuRunway->pListRight->RemoveAllItem();
        for (auto &c : mapRunway)
        {
            auto itemHeight = CHmsAirportConstant::s_listItemHeight;

            auto pButton = CHmsUiStretchButton::Create("res/AirportInfo/Round4SolidWhite.png",
                Size(m_pTabPageStuRunway->pListLeft->GetContentSize().width, itemHeight)
                , Color3B::WHITE, CHmsAirportConstant::s_airportBlue);

			if (NULL != pButton)
			{
				pButton->SetAnchorPoint(Vec2(0, 1));
				pButton->SetOnClickedFunc(std::bind(OnLeftListBtnClick, std::placeholders::_1, c.first));
				pButton->SetToggleBtn(true);

				auto pLabel = CHmsGlobal::CreateUTF8Label(c.first.c_str(), CHmsAirportConstant::s_fontSize, Color4F::BLACK, true);
				if (NULL != pLabel)
				{
					pLabel->SetAnchorPoint(Vec2(0, 0.5));
					pLabel->SetPosition(Vec2(10, itemHeight * 0.5));
					pButton->AddChild(pLabel);
				}				

				{
					auto pImage = CHmsImageNode::Create("res/AirportInfo/runway.png");
					if (NULL != pImage)
					{
						pImage->SetAnchorPoint(Vec2(0.5, 0.5));
						pImage->SetPosition(Vec2(m_pTabPageStuRunway->pListLeft->GetContentSize().width - 30 - 70, itemHeight * 0.5));
						pImage->SetColor(Color3B::BLACK);
						pButton->AddChild(pImage);
					}					
				}
				{
					auto pImage = CHmsImageNode::Create("res/AirportInfo/RightPointer.png");
					if (NULL != pImage)
					{
						pImage->SetColor(CHmsAirportConstant::s_airportGray);
						pImage->SetAnchorPoint(Vec2(0.5, 0.5));
						pImage->SetPosition(Vec2(m_pTabPageStuRunway->pListLeft->GetContentSize().width - 30, itemHeight * 0.5));
						pButton->AddChild(pImage);
					}					
				}

				m_pTabPageStuRunway->pListLeft->AddWindowItem(pButton);
				s_vecBtn.push_back(pButton);
			}            
        }
    };

    UpdateLeftList(m_mapRunway);
}

void CHmsAirportInfo::GetProceduresData(const string &code)
{
    m_mapProcedure.clear();

    std::map<int, std::string> typeMap;
    typeMap[1] = "AIRPORT";
    typeMap[5] = "STAR";
    typeMap[10] = "SID";
    typeMap[15] = "ILS";
    typeMap[20] = "VOR";

    for (auto iter = typeMap.begin(); iter != typeMap.end(); ++iter)
    {
        std::vector<StarSidStu> vecStu = CHmsNavImageSql::GetInstance().GetStarSidInfo(code, false, iter->first);
        for (auto &c : vecStu)
        {
            AirportInfoProceduresStu stu;
            stu.type = iter->second;
            stu.name = c.name;
            stu.ssStu = c;
            m_mapProcedure[stu.type].push_back(stu);
        }
    }
#if 0
    {
        AirportInfoProceduresStu stu;
        stu.type = "Approach";
        stu.name = "Austin Approach";
        m_mapProcedure[stu.type].push_back(stu);
    }
    {
        AirportInfoProceduresStu stu;
        stu.type = "Approach";
        stu.name = "Austin Approach2";
        m_mapProcedure[stu.type].push_back(stu);
    }
    {
        AirportInfoProceduresStu stu;
        stu.type = "Departure";
        stu.name = "AEROZ ONE";
        m_mapProcedure[stu.type].push_back(stu);
    }
    {
        AirportInfoProceduresStu stu;
        stu.type = "Departure";
        stu.name = "Austin FIVE";
        m_mapProcedure[stu.type].push_back(stu);
    }
#endif
}

void CHmsAirportInfo::UpdateProcedures(const string &code)
{
    GetProceduresData(code);


    auto OnRightListBtnClick = [=](CHmsUiButtonAbstract *pBtn, const StarSidStu *pData){

        if (pData)
        {
            auto p = CHmsAvionicsDisplayMgr::GetInstance()->GetNavType(HmsPageType::PageAirports);
            auto pAirport = dynamic_cast<CHmsLayerAirports*>(p);
            if (pAirport)
            {
                pAirport->RemoveAllChildInterface();

                auto pProgram = CHmsLayerAirportProgram::GetInstance();
				if (NULL != pProgram)
				{
					pAirport->AddChild(pProgram);
					pAirport->AddChildInterface(pProgram);
					pProgram->SetOnCloseCallback([=](){

						pAirport->RegisterAllNodeChild();
					});

					auto stu = CHmsNavImageSql::GetInstance().GetStarSidInfo(pData->id);
					pProgram->OnEnter();
					pProgram->SetStarSidStu(stu);
				}               
            }
        }
    };

    auto UpdateRightList = [=](const vector<AirportInfoProceduresStu> &vec){

        m_pTabPageStuProcedure->pListRight->RemoveAllItem();
        for (auto &c : vec)
        {
            auto itemHeight = CHmsAirportConstant::s_listItemHeight;

            auto pButton = CHmsUiStretchButton::Create("res/AirportInfo/Round4SolidWhite.png",
                Size(m_pTabPageStuProcedure->pListRight->GetContentSize().width, itemHeight));
			if (NULL != pButton)
			{
				pButton->SetAnchorPoint(Vec2(0, 1));
				pButton->SetOnClickedFunc(std::bind(OnRightListBtnClick, std::placeholders::_1, &c.ssStu));
				{
					auto pLabel = CHmsGlobal::CreateUTF8Label(c.name.c_str(), CHmsAirportConstant::s_fontSize, Color4F::BLACK, true);
					if (NULL != pLabel)
					{
						pLabel->SetMaxLineWidth(pButton->GetContentSize().width - 10);
						pLabel->SetAnchorPoint(Vec2(0, 0.5));
						pLabel->SetPosition(Vec2(10, itemHeight * 0.5));
						pButton->AddChild(pLabel);
					}					
				}

				m_pTabPageStuProcedure->pListRight->AddWindowItem(pButton);
			}           
        }

    };

    static std::vector<CHmsUiButtonAbstract*> s_vecBtn;
    auto OnLeftListBtnClick = [=](CHmsUiButtonAbstract *pBtn, string str){

        if (!pBtn->IsSelected())
        {
            pBtn->Selected();
            return;
        }
        for (auto &c : s_vecBtn)
        {
			if (c != pBtn && NULL != c)
            {
                c->SetEnableCallback(false);
                c->SetTextColor(Color4B::BLACK);
                c->UnSelected();
                c->SetEnableCallback(true);
            }
        }


        auto iter = m_mapProcedure.find(str);
        if (iter != m_mapProcedure.end())
        {
            UpdateRightList(iter->second);
        }
    };
    auto UpdateLeftList = [=](const map<string, vector<AirportInfoProceduresStu> > &mapProcedure){

        s_vecBtn.clear();
        m_pTabPageStuProcedure->pListLeft->RemoveAllItem();
        m_pTabPageStuProcedure->pListRight->RemoveAllItem();
        for (auto &c : mapProcedure)
        {
            auto itemHeight = CHmsAirportConstant::s_listItemHeight;

            auto pButton = CHmsUiStretchButton::Create("res/AirportInfo/Round4SolidWhite.png",
                Size(m_pTabPageStuProcedure->pListLeft->GetContentSize().width, itemHeight)
                , Color3B::WHITE, CHmsAirportConstant::s_airportBlue);
			if (NULL != pButton)
			{
				pButton->SetAnchorPoint(Vec2(0, 1));
				pButton->SetOnClickedFunc(std::bind(OnLeftListBtnClick, std::placeholders::_1, c.first));
				pButton->SetToggleBtn(true);

				auto pLabel = CHmsGlobal::CreateUTF8Label(c.first.c_str(), CHmsAirportConstant::s_fontSize, Color4F::BLACK, true);
				pLabel->SetAnchorPoint(Vec2(0, 0.5));
				pLabel->SetPosition(Vec2(10, itemHeight * 0.5));
				pButton->AddChild(pLabel);

				auto pImage = CHmsImageNode::Create("res/AirportInfo/RightPointer.png");
				pImage->SetColor(CHmsAirportConstant::s_airportGray);
				pImage->SetAnchorPoint(Vec2(0.5, 0.5));
				pImage->SetPosition(Vec2(m_pTabPageStuProcedure->pListLeft->GetContentSize().width - 30, itemHeight * 0.5));
				pButton->AddChild(pImage);

				m_pTabPageStuProcedure->pListLeft->AddWindowItem(pButton);
				s_vecBtn.push_back(pButton);
			}            
        }
    };

    UpdateLeftList(m_mapProcedure);
}
