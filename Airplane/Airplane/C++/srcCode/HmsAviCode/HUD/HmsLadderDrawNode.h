#pragma once
#include "HmsAviPf.h"
#include "display/HmsDrawNode.h"
#include "HmsMultLineDrawNode.h"

class CHmsLadderDrawNode : public HmsAviPf::CHmsMultLineDrawNode
{
public:
    CHmsLadderDrawNode();
    ~CHmsLadderDrawNode();

    CREATE_FUNC(CHmsLadderDrawNode);

    void SetLineArea(float fBegin, float fEnd, float fLineWidth);
    void SetPitchPerHeight(float fPitchPerHeight);
    void SetLadderScaleHeight(float fHeight);

    void AddSkyLadder(int nPitch);
    void AddGroundLadder(int nPitch);

public:
    void DrawAft();

    void AddSky90();

    void AddGround90();

protected:
    HmsAviPf::CHmsLabel * GetLabel(const char * strText);

private:
    float	m_fPosBegin;			//开始点
    float	m_fPosEnd;				//结束点
    float	m_fLineWidth;			//线宽度
    float	m_fLadderHeight;
    float	m_fPitchPerHeight;
    float	m_fLabelMove;
};

