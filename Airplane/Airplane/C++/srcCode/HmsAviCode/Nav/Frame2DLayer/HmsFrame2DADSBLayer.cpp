#include "HmsFrame2DADSBLayer.h"
#include "../HmsFrame2DRoot.h"
#include "DataInterface/HmsDataBus.h"

CHmsFrame2DADSBLayer::CHmsFrame2DADSBLayer()
: CHmsFrame2DLayerBase(this, NavFrame2DLayerType::NavFrame2D_ADSB)
{

}

CHmsFrame2DADSBLayer::~CHmsFrame2DADSBLayer()
{
}

CHmsFrame2DADSBLayer* CHmsFrame2DADSBLayer::Create(Size layerSize)
{
    CHmsFrame2DADSBLayer *ret = new (std::nothrow) CHmsFrame2DADSBLayer();
    if (ret && ret->Init(layerSize))
    {
        ret->autorelease();
        return ret;
    }
    HMS_SAFE_DELETE(ret);
    return nullptr;
}

bool CHmsFrame2DADSBLayer::Init(Size layerSize)
{
    if (!CHmsNode::Init())
    {
        return false;
    }
    this->SetContentSize(layerSize);

    return true;
}

void CHmsFrame2DADSBLayer::Update(float delta)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!CHmsGlobal::GetInstance()->GetDataBus())
		return;
#endif
    auto pMap = CHmsGlobal::GetInstance()->GetDataBus()->GetADSBData();
#if _NAV_SVS_LOGIC_JUDGE
	if (!pMap)
		return;
#endif
    if (pMap->size() > m_vecStu.size())
    {
        int count = pMap->size() - m_vecStu.size();
        for (int i = 0; i < count; ++i)
        {
            NavADSBLayerNode stu;

            stu.pParentNode = CHmsNode::Create();
#if _NAV_SVS_LOGIC_JUDGE
			if (!stu.pParentNode)
				continue;
#endif
            stu.pParentNode->SetAnchorPoint(Vec2(0.5, 0.5));
            stu.pParentNode->SetContentSize(Size(150, 50));
            this->AddChild(stu.pParentNode);

            stu.pImageNode = CHmsImageNode::Create("res/airport/plane.png");
#if _NAV_SVS_LOGIC_JUDGE
			if (!stu.pImageNode)
				continue;
#endif
            stu.pImageNode->SetColor(Color3B::ORANGE);
            stu.pImageNode->SetAnchorPoint(Vec2(0.5f, 0.5f));
            stu.pImageNode->SetPosition(stu.pParentNode->GetContentSize() * 0.5);
            stu.pParentNode->AddChild(stu.pImageNode);

            stu.pLabel = CHmsGlobal::CreateUTF8Label("1", 20);
#if _NAV_SVS_LOGIC_JUDGE
			if (!stu.pLabel)
				continue;
#endif
            stu.pLabel->SetAnchorPoint(Vec2(0, 0.5));
            stu.pLabel->SetTextColor(Color4B::BLACK);
            stu.pLabel->SetPosition(Vec2(stu.pParentNode->GetContentSize().width * 0.5 + 30, stu.pParentNode->GetContentSize().height * 0.5));

            stu.pBgImage = CHmsStretchImageNode::Create("res/BasicFrame/Round4SolidWhite.png");
#if _NAV_SVS_LOGIC_JUDGE
			if (!stu.pBgImage)
				continue;
#endif
            stu.pBgImage->SetAnchorPoint(Vec2(0, 0.5));
            stu.pBgImage->SetPosition(stu.pLabel->GetPoistion() - Vec2(10, 0));
            stu.pBgImage->SetContentSize(Size(130, stu.pLabel->GetContentSize().height));
            stu.pBgImage->SetColor(Color3B(0x9d, 0xb0, 0xfa));
            stu.pBgImage->SetOpacity(200);
            stu.pParentNode->AddChild(stu.pBgImage);

            stu.pParentNode->AddChild(stu.pLabel);

            m_vecStu.push_back(stu);
        }
    }

#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pFrame2DRoot)
		return;
#endif
    HmsAviPf::Frustum *pFrust = m_pFrame2DRoot->GetFrustum();
    int index = 0;
    for (auto &adsb : *pMap)
    {
        NavADSBLayerNode &stu = m_vecStu[index];
        stu.bInUse = true;
        stu.earthPos = m_pFrame2DRoot->LonLatToEarthPoint(Vec2(adsb.second.longitude, adsb.second.latitude));
    
        bool bVisible = false;
        if (pFrust)
        {
            bVisible = !pFrust->isOutOfFrustum(stu.earthPos);
        }
#if _NAV_SVS_LOGIC_JUDGE
		if (stu.pImageNode)
#endif
        stu.pImageNode->SetRotation(adsb.second.trackHeading);
#if _NAV_SVS_LOGIC_JUDGE
		if (stu.pLabel)
#endif
        stu.pLabel->SetString(adsb.second.callsign);

#if _NAV_SVS_LOGIC_JUDGE
		if (stu.pParentNode)
#endif
		{
			stu.pParentNode->SetPosition(m_pFrame2DRoot->EarthPointToFramePoint(stu.earthPos));
			stu.pParentNode->SetVisible(bVisible);
		}

        ++index;
    }
    for (int i = index; i < (int)m_vecStu.size(); ++i)
    {
        NavADSBLayerNode &stu = m_vecStu[i];
        stu.bInUse = false;
#if _NAV_SVS_LOGIC_JUDGE
		if (stu.pParentNode)
#endif
        stu.pParentNode->SetVisible(false);
    }
}

void CHmsFrame2DADSBLayer::Update2DElements()
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pFrame2DRoot)
		return;
#endif
    HmsAviPf::Frustum *pFrust = m_pFrame2DRoot->GetFrustum();

    for (int i = 0; i < (int)m_vecStu.size(); ++i)
    {
        NavADSBLayerNode &stu = m_vecStu[i];
        if (!stu.bInUse)
        {
            continue;
        }

        bool bVisible = false;
        if (pFrust)
        {
            bVisible = !pFrust->isOutOfFrustum(stu.earthPos);
        }
#if _NAV_SVS_LOGIC_JUDGE
		if (stu.pParentNode)
#endif
		{
			stu.pParentNode->SetPosition(m_pFrame2DRoot->EarthPointToFramePoint(stu.earthPos));
			stu.pParentNode->SetVisible(bVisible);
		}
    }
}
