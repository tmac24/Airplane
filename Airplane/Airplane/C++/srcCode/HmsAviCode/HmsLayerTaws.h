#pragma once
#include "HmsScreenLayout.h"
#include "HmsGlobal.h"
#include "ui/HmsUiEventInterface.h"
#include "../HmsGE/display/HmsDrawNode.h"
#include "../HmsGE/base/HmsClippingNode.h"
#include "base/HmsTimer.h"
#include "TAWS/SimAircraft.h"
#include "TAWS/TawsInitParam.h"
#include "TAWS/PlaneState.h"
#include "TAWS/CollisionManager.h"
#include "TAWS/HmsHVTLayer.h"
#include "NavSvs/mathd/Vec3d.h"
#include "Terrain/HmsProfileView.h"
#include "HUD/HmsMultLineDrawNode.h"
#include "TAWS/RunwayTileDataMgr.h"
#include <functional>
#include "Warning/HmsWarningColorStyle.h"
#include "TAWS/HmsTerrainLegend.h"

enum class HMS_TAWS_MODE
{
    Center,
    Arc,
};


class CHmsLayerTaws : public CHmsScreenLayout, public HmsAviPf::HmsUiEventInterface
{
public:
    CREATE_FUNC(CHmsLayerTaws);

    CHmsLayerTaws();
    ~CHmsLayerTaws();

    virtual bool Init() override;
    void UpdateBarrier(const float& delta, const float& fHeading, const HmsAviPf::Vec2d& curLonLat, const float& curAlt, const HmsAviPf::Vec3d& ENS);
    HmsAviPf::Vec2d convertLonLatToPixelPosOnBarrLayer(const HmsAviPf::Vec2d& barrLonLat, const HmsAviPf::Vec2d& barrierLayerOriginalLonLat, const double& pixelsPerLon);
    void UpdateBarrierLayer(const HmsAviPf::Vec2d &curLonLat);
    void UpdateCollisionPoint(const float& fHeading, const HmsAviPf::Vec2d& curLonLat);
    virtual void OnEnter() override;
    virtual void OnExit() override;
    virtual void Update(float delta) override;
    void SimTrackDirEastNorthSkySpeed(float delta, PlaneState& planeState);
    int GetPlaneStateParam(const float& delta, PlaneState& planeState);
    virtual void ResetViewer() override;
    void UpdateGeoMapData(const HmsAviPf::Vec2 & pos, float fHeading);
    void ResizeLayer(const HmsAviPf::Size & size);
    void ZoomIn();
    void ZoomOut();
    static std::string generateFormatStringForFloatNumber(const double& floatPointNumber);
    void SetModeString(std::string strMode);
    void SetMode(HMS_TAWS_MODE eMode);
    void SetShowCompass(bool bShow);
    void SetProfileView(CHmsProfileView * pProfileView);
    void RedrawDistanceCircle(float fRange, bool bUseMask);
    enum enTawsColorTheme{
        EN_TAWS_THEME_0,
        EN_TAWS_THEME_1,
        EN_TAWS_THEME_NA,
    };
    void SetTawsColorStyle(const enTawsColorTheme& theme);
private:
    void UpdateRangeInfo();
    void SetRange(float fRangeKm);
    void UpdateShowContent(const HmsAviPf::Size & size);
    bool validateLonLat(const HmsAviPf::Vec2d &wgsPosNormalized);
private:
    void LoadInitParam();
    union UnionDisplayCommand
    {
        UnionDisplayCommand(const unsigned int& v) :valU32(v){};
        unsigned int valU32;
        struct
        {//注意不同系统下的大小端
            unsigned char mode : 1;//LSB
            unsigned char customRangeEnable : 1;
            unsigned char scope : 3;
            unsigned char : 3;
            unsigned char customRange : 8;
            unsigned char selftestmode : 8;
            unsigned char : 8;//MSB
        }valBit;
        bool operator==(const UnionDisplayCommand& udc){
            return (udc.valBit.mode == this->valBit.mode
                && udc.valBit.customRangeEnable == this->valBit.customRangeEnable
                && udc.valBit.scope == this->valBit.scope
                && udc.valBit.customRange == this->valBit.customRange
                );
        }
    };
    HmsAviPf::RefPtr<HmsAviPf::CHmsImageNode>		    m_map;
    HmsAviPf::RefPtr<HmsAviPf::CHmsNode>			    m_collidePointNodeMask;
    HmsAviPf::RefPtr<HmsAviPf::CHmsImageNode>		    m_collidePointNode;
    HmsAviPf::RefPtr<HmsAviPf::CHmsClippingNode>	    m_nodeClipping;
    HmsAviPf::RefPtr<HmsAviPf::CHmsDrawNode>		    m_nodeShowRect;
    HmsAviPf::RefPtr<HmsAviPf::CHmsNode>				m_nodeCompassRoot;
    HmsAviPf::RefPtr<HmsAviPf::CHmsClippingNode>		m_nodeCompassClipping;
    HmsAviPf::RefPtr<HmsAviPf::CHmsDrawNode>			m_nodeCompassMask;
    HmsAviPf::RefPtr<HmsAviPf::CHmsMultLineDrawNode>	m_nodeCompassRangeCircle;
    HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>				m_labelCompassRange;
    HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>				m_labelCompassRangeMin;
    HmsAviPf::RefPtr<HmsAviPf::CHmsNode>			    m_nodeMapRoot;
    HmsAviPf::RefPtr<CHmsTerrainLegend>                 m_nodeLegend;
    HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>			    m_nodeDebugLabel;
    HmsAviPf::RefPtr<HmsAviPf::CHmsLabel>			    m_nodeColliDistLabel;//碰撞距离显示label
    HmsAviPf::RefPtr<HmsAviPf::CHmsNode>			    m_nodeMapRootMap;
    HmsAviPf::RefPtr<HmsAviPf::CHmsNode>			    m_nodeBarrierLayer;
    //HmsAviPf::RefPtr<HmsHVTLayer>					    m_nodeHVTLayer;//high voltage towers layer
    HmsAviPf::RefPtr<HmsAviPf::CHmsNode>			    m_nodeBtnIn;
    HmsAviPf::RefPtr<HmsAviPf::CHmsNode>			    m_nodeBtnOut;
    HmsAviPf::RefPtr<HmsAviPf::CHmsNode>			    m_nodeBtnSwitchMode;
    HmsAviPf::Vec2									    m_mapCenter;
    HmsAviPf::Rect									    m_mapRect;
    HmsAviPf::Vec2									    m_mapDstOffset;
    float											    m_fRangeKm;
    float											    m_fRangeRadius;		//范围表示的半径 pixel
    HMS_TAWS_MODE									    m_eCurMode;
    CollideInfo                                         m_CurrCollideInfo;
    HmsAviPf::InitParam								    m_initParam;
    PlaneState                                          m_sim_plane_state;
    HmsAviPf::RefPtr<SimAircraft>                       m_pSimAircraft;
    CHmsProfileView *                                   m_pProfileView;
    HmsAviPf::GLProgram*                                m_glprogram;
    std::vector<warningColorStyle*>                     m_warningColorStylePtrs;
};

