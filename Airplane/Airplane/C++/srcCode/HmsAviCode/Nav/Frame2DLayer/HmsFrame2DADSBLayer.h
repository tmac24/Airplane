#pragma once

#include "HmsFrame2DLayerBase.h"
#include "base/HmsFrustum.h"
#include "display/HmsStretchImageNode.h"

struct NavADSBLayerNode
{
    CHmsNode *pParentNode;
    CHmsImageNode *pImageNode;
    CHmsLabel *pLabel;
    CHmsStretchImageNode *pBgImage;
    Vec3 earthPos;
    bool bInUse;

    NavADSBLayerNode()
    {
        pParentNode = nullptr;
        pImageNode = nullptr;
        pLabel = nullptr;
        pBgImage = nullptr;
        bInUse = false;
    }
};

class CHmsFrame2DADSBLayer : public CHmsFrame2DLayerBase
{
public:
    CHmsFrame2DADSBLayer();
    ~CHmsFrame2DADSBLayer();

    static CHmsFrame2DADSBLayer* Create(Size layerSize);
    virtual bool Init(Size layerSize);

    virtual void Update(float delta) override;
    virtual void Update2DElements() override;

private:
    std::vector<NavADSBLayerNode> m_vecStu;
};



