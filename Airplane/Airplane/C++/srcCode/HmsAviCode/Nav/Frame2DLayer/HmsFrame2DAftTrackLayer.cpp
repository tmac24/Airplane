#include "HmsFrame2DAftTrackLayer.h"
#include "../HmsFrame2DRoot.h"
#include "Calculate/CalculateZs.h"
#include "HmsGlobal.h"
#include "HmsLog.h"
#include "DataInterface/HmsTrackDataServer.h"

CHmsFrame2DAftTrackLayer::CHmsFrame2DAftTrackLayer()
: CHmsFrame2DLayerBase(this, NavFrame2DLayerType::NavFrame2D_AftTrackLayer)
{

}

CHmsFrame2DAftTrackLayer::~CHmsFrame2DAftTrackLayer()
{
}

CHmsFrame2DAftTrackLayer* CHmsFrame2DAftTrackLayer::Create(Size layerSize)
{
    CHmsFrame2DAftTrackLayer *ret = new (std::nothrow) CHmsFrame2DAftTrackLayer();
    if (ret && ret->Init(layerSize))
    {
        ret->autorelease();
        return ret;
    }
    HMS_SAFE_DELETE(ret);
    return nullptr;
}

bool CHmsFrame2DAftTrackLayer::Init(Size layerSize)
{
    if (!CHmsNode::Init())
    {
        return false;
    }
    this->SetContentSize(layerSize);

    m_pTrackLineDrawNode = CHmsMultLineDrawNode::Create();
    m_pTrackLineDrawNode->SetAnchorPoint(0, 0);
    m_pTrackLineDrawNode->SetContentSize(this->GetContentSize());
    m_pTrackLineDrawNode->SetPosition(0, 0);
    m_pTrackLineDrawNode->SetDrawColor(Color4F::WHITE);
    this->AddChild(m_pTrackLineDrawNode);

    m_pRootLabelNode = CHmsNode::Create();
    m_pRootLabelNode->SetAnchorPoint(0, 0);
    m_pRootLabelNode->SetPosition(0, 0);
    this->AddChild(m_pRootLabelNode);

#if (USE_COLOR_TEXTRUE == 1)
    m_pTrackLine3DDrawNode = CHms3DTextureColorLineDrawNode::Create(7200*10);
#else
    m_pTrackLine3DDrawNode = CHms3DLineDrawNode::Create(7200 * 10);
    m_pTrackLine3DDrawNode->SetColor(Color4F(0.0f, 0.0f, 1.0f*0.9f, 1.0f));

    m_pTrackLine3DHistoryDrawNode = CHms3DLineDrawNode::Create(7200 * 10);
    m_pTrackLine3DHistoryDrawNode->SetColor(Color4F::MAGENTA);
    m_pTrackLine3DHistoryDrawNode->SetLineWidth(5.0f);
#endif
    m_pTrackLine3DDrawNode->SetLineWidth(5.0f);
    auto navEarthFrame = CHmsGlobal::GetInstance()->GetNavEarthFramePointer();
    navEarthFrame->AttachToEarthDrawNode(m_pTrackLine3DDrawNode);
    navEarthFrame->AttachToEarthDrawNode(m_pTrackLine3DHistoryDrawNode);

    m_pos3D.reserve(3600 * 10);

    return true;
}

void CHmsFrame2DAftTrackLayer::Update(float delta)
{
    auto trackData = CHmsGlobal::GetInstance()->GetTrackDataServer()->GetCurrentTrackData();
    if (trackData->IsDataDirty())
    {
        m_pTrackLine3DDrawNode->clear();
        auto & trackCell = trackData->GetTrackPosList();

        m_pos3D.clear();
        for (auto & c : trackCell)
        {
            m_pos3D.push_back(c.pos3D);
        }

        //m_pAdminAreaLine3DDrawNode->DrawLines(m_pos3D);
        m_pTrackLine3DDrawNode->DrawLineStrip(m_pos3D);
        trackData->SwallowDirty();
    }
}

void CHmsFrame2DAftTrackLayer::Update2DElements()
{
    m_pRootLabelNode->SetVisible(true);


}

void CHmsFrame2DAftTrackLayer::SetEarthLayer(int n)
{
    if (m_earthLayer != n)
    {
        m_earthLayer = n;

        SetNeedUpdate2DEles(true);
    }
}

void CHmsFrame2DAftTrackLayer::ShowTrackLayer()
{
    this->SetVisible(true);
}

void CHmsFrame2DAftTrackLayer::HideTrackLayer()
{
    this->SetVisible(false);
}

void CHmsFrame2DAftTrackLayer::DisplayHistoryTrack(const std::vector<HmsAviPf::Vec3> & vTrackPos)
{
    if (m_pTrackLine3DHistoryDrawNode)
    {
        m_pTrackLine3DHistoryDrawNode->SetVisible(true);
        m_pTrackLine3DHistoryDrawNode->clear();

        m_pTrackLine3DHistoryDrawNode->DrawLineStrip(vTrackPos);
    }
}

void CHmsFrame2DAftTrackLayer::CleanHistoryTrack()
{
    if (m_pTrackLine3DHistoryDrawNode)
    {
        m_pTrackLine3DHistoryDrawNode->SetVisible(false);
    }
}

void CHmsFrame2DAftTrackLayer::SetVisible(bool bVisible)
{
    CHmsFrame2DLayerBase::SetVisible(bVisible);

    if (m_pTrackLine3DDrawNode)
    {
        m_pTrackLine3DDrawNode->SetVisible(bVisible);
    }
}

void CHmsFrame2DAftTrackLayer::SetColor(const Color3B& color)
{
    if (m_pTrackLine3DDrawNode)
    {
        m_pTrackLine3DDrawNode->SetColor(color);
    }
}