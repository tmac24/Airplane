#pragma once
#include "../HmsGlobal.h"
#include "display/HmsDrawNode.h"

class CHmsWind : public HmsAviPf::CHmsNode
{
public:
    CHmsWind();
    ~CHmsWind();

    CREATE_FUNC(CHmsWind);

    virtual bool Init() override;

    void SetNoData();

    //设置风数据 winDir风向 winSpd风速
    void SetData(const int &windDir, const int &windSpd);

private:
    float m_boxWidth;
    float m_boxHeight;

    HmsAviPf::RefPtr<HmsAviPf::CHmsDrawNode>	m_pBoxNode;
    HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>		m_pNoDataLabel;//无风数据显示标签
    HmsAviPf::RefPtr<HmsAviPf::CHmsNode>		m_pWindDataNode;
    HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>		m_pWindDirLabel;//数据显示
    HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>		m_pWindSpdLabel;
    HmsAviPf::RefPtr<HmsAviPf::CHmsImageNode>	m_pWindDirIcon;//风向图标
};

