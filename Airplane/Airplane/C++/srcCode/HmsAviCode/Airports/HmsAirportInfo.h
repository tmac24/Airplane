#pragma once
#include "base/HmsNode.h"
#include "ui/HmsUiEventInterface.h"
#include "HmsAirportsStuDef.h"
#include "display/HmsLabel.h"
#include "base/RefPtr.h"
#include "ui/HmsUiStretchButton.h"
#include <vector>
#include "HmsAirportInfoTabPage.h"
#include "Database/Hms424Database.h"

using namespace HmsAviPf;

class CHmsAirportInfo : public HmsAviPf::CHmsNode, public HmsUiEventInterface
{
public:
    CHmsAirportInfo();
    ~CHmsAirportInfo();
    CREATE_FUNC_BY_SIZE(CHmsAirportInfo)
    virtual bool Init(const HmsAviPf::Size & size);

    void SetIcaoCode(string code);

private:
    void UpdateAirportInfo(const string &code);

    void GetFrequenciesData(const string &code);
    void UpdateFrequencies(const string &code);

    void GetRunwaysData(const string &code);
    void UpdateRunways(const string &code);

    void GetProceduresData(const string &code);
    void UpdateProcedures(const string &code);

private:
    AirportInfoStu m_airportInfoStu;

    CHmsLabel *m_pLabelTitle;

    CHmsLabel *m_pLabelElevation;
    CHmsLabel *m_pLabelLongestRunway;
    CHmsLabel *m_pLabelMagneticVar;
    CHmsLabel *m_pLabelTransitAlt;

    CHmsLabel *m_pLabelAtis;
    CHmsLabel *m_pLabelClearance;
    CHmsLabel *m_pLabelGround;
    CHmsLabel *m_pLabelTower;

    CHmsAirportInfoTabPage *m_pTabPage;

    std::map<std::string, std::vector<DB_AirportComm> > m_mapFreqDbData;
    map<string, vector<AirportInfoFrequenciesStu> > m_mapFreq;
    CHmsAirportInfoTabPage::TabPageStu *m_pTabPageStuFreq;

    std::map<std::string, AirportInfoRunwayStu> m_mapRunway;
    CHmsAirportInfoTabPage::TabPageStu *m_pTabPageStuRunway;

    map<string, vector<AirportInfoProceduresStu> > m_mapProcedure;
    CHmsAirportInfoTabPage::TabPageStu *m_pTabPageStuProcedure;
};

