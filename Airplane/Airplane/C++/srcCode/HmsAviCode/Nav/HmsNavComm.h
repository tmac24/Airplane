#pragma once

#include <string>
#include <vector>

struct NavLogTableStu
{
    double angle;
    double totalNM;
    double totalTime;
    std::string totalTimeStr;

    double legNM;
    double legTime;
    std::string legTimeStr;

    double remainNM;
    double remainTime;
    std::string remainNMStr;
    std::string remainTimeStr;

    std::string etaStr;

    NavLogTableStu()
    {
        angle = 0;
        totalNM = 0;
        totalTime = -1;
        totalTimeStr = "---";
        legNM = -1;
        legTime = -1;
        legTimeStr = "---";
        remainNM = -1;
        remainTime = -1;
        remainNMStr = "---";
        remainTimeStr = "---";
        etaStr = "---";
    }
};

class CHmsFrame2DRoot;
class CHmsNavFplLayer;

class CHmsNavComm
{
public:
    static CHmsNavComm* GetInstance();

    void SetCHmsFrame2DRoot(CHmsFrame2DRoot *p){ m_pFrame2DRoot = p; }
    CHmsFrame2DRoot* GetCHmsFrame2DRoot(){ return m_pFrame2DRoot; }

    void SetCHmsNavFplLayer(CHmsNavFplLayer *p){ m_pNavFplLayer = p; }
    CHmsNavFplLayer* GetCHmsNavFplLayer(){ return m_pNavFplLayer; }

    int UpdateTableETA(float groundSpeed, std::vector<NavLogTableStu> &vecNavLogTable);

    bool IsMetric();
    //输入输出都是米制，比如传入30， 米制时返回30km， 英制时返回30nm
    double GetNMOrKM(double value);
    std::string AppendNMOrKMUnit(const char *format, ...);

    void SetEnableTileLoad(bool b){ m_bEnableTileLoad = b; };
    bool GetEnableTileLoad(){ return m_bEnableTileLoad; }

private:
    CHmsNavComm();
    ~CHmsNavComm();
private:
    CHmsFrame2DRoot *m_pFrame2DRoot;
    CHmsNavFplLayer *m_pNavFplLayer;
    bool m_bMetricEnable;
    bool m_bEnableTileLoad;
};

