#pragma once
#include "../HmsGlobal.h"

class CHmsNavInfo : public HmsAviPf::CHmsNode
{
public:
    CHmsNavInfo();
    ~CHmsNavInfo();

    CREATE_FUNC(CHmsNavInfo);

    virtual bool Init() override;

    void SetPointerIconColor(const HmsAviPf::Color3B &color);
    void SetPointerIconPath(const std::string &path);
    void SetNavSrc(const std::string &navSrc);
    void SetInfo1(const std::string &info1);
    void SetInfo2(const std::string &info2);
    void SetNoData();

protected:
    HmsAviPf::Color3B m_pointerIconColor;//导航指针示例图标颜色
    std::string m_pointerIconPath;//导航指针示例图标路径
    std::string m_navSrc;//导航源名称
    std::string m_info1;//电台识别码
    std::string m_info2;//距离 单位海里

    HmsAviPf::RefPtr<HmsAviPf::CHmsImageNode>	m_pBoxNode;
    HmsAviPf::RefPtr<HmsAviPf::CHmsImageNode>	m_pPointerIcon;
    HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>		m_pNavSrcLabel;//导航源名称
    HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>		m_pNavSrc;
    HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>		m_pInfo1;
    HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>		m_pInfo2;

    void UnfoldBox(bool unfold);
};

