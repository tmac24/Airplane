#pragma once

#include "../Frame2DLayer/HmsFrame2DLayerBase.h"
#include "../HmsRouteLineDrawNode.h"
#include "HUD/HmsMultLineDrawNode.h"
#include "base/HmsActionTimeRef.h"
#include "base/HmsFrustum.h"
#include "DataInterface/HmsAdminAreasServer.h"
#include "display/Hms3DLineDrawNode.h"
#include "display/Hms3DColorLineDrawNode.h"
#include "display/Hms3DTextureColorLineDrawNode.h"

class CHmsFrame2DAftTrackLayer : public CHmsFrame2DLayerBase
{
public:
    CHmsFrame2DAftTrackLayer();
    ~CHmsFrame2DAftTrackLayer();

    static CHmsFrame2DAftTrackLayer* Create(Size layerSize);
    virtual bool Init(Size layerSize);

    virtual void Update(float delta) override;
    virtual void Update2DElements() override;
    virtual void SetEarthLayer(int n) override;

    void ShowTrackLayer();
    void HideTrackLayer();

    void DisplayHistoryTrack(const std::vector<HmsAviPf::Vec3> & vTrackPos);
    void CleanHistoryTrack();

	virtual void SetVisible(bool bVisible) override;

	virtual void SetColor(const Color3B& color) override;

protected:

private:
    RefPtr<CHmsMultLineDrawNode>						m_pTrackLineDrawNode;
	RefPtr<CHmsNode>									m_pRootLabelNode;
#if (USE_COLOR_TEXTRUE == 1)
	RefPtr<HmsAviPf::CHms3DTextureColorLineDrawNode>	m_pTrackLine3DDrawNode;
#else
	RefPtr<HmsAviPf::CHms3DLineDrawNode>				m_pTrackLine3DDrawNode;
    RefPtr<HmsAviPf::CHms3DLineDrawNode>                m_pTrackLine3DHistoryDrawNode;
#endif

    std::vector<HmsAviPf::Vec3>                         m_pos3D;
};



