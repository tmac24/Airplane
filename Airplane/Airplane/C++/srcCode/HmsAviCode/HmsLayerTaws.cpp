#include "HmsLayerTaws.h"
#include "render/HmsGLProgram.h"
#include "render/HmsGLProgramState.h"
#include "DataInterface/TerrainServer/HmsGeoMapLoadManager.h"
#include "HmsGlobal.h"
#include "ui/HmsUiStretchButton.h"
#include "Calculate/CalculateZs.h"
#include "../HmsGE/render/HmsMaterial.h"
#include <time.h>
#include <iostream>
#include "DataInterface/TerrainServer/HmsTerrainServer.h"
#include "TAWS/TileDataMgrBase.h"
#include "TAWS/BarrierTileDataMgr.h"
#include "TAWS/RunwayTileDataMgr.h"
#include "HmsConfigXMLReader.h"
#include "TAWS/BarrierTileDataServer.h"
#include "DataInterface/HmsDataBus.h"
#include "Terrain/HmsProfileView.h"

#define TAWS_COMPASS_LINE_WIDTH  2.0f

USING_NS_HMS;

#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_WIN32)
extern float g_fps;//temp test
extern int g_nCyc;//temp test
#endif


CHmsLayerTaws::CHmsLayerTaws()
: HmsUiEventInterface(this)
, m_eCurMode(HMS_TAWS_MODE::Center)
, m_nodeBtnIn(nullptr)
, m_nodeBtnOut(nullptr)
, m_nodeBtnSwitchMode(nullptr)
, m_nodeLegend(nullptr)
, m_nodeBarrierLayer(nullptr)
, m_nodeShowRect(nullptr)
, m_nodeClipping(nullptr)
, m_nodeDebugLabel(nullptr)
, m_pSimAircraft(nullptr)
, m_pProfileView(nullptr)
, m_fRangeKm(0.0f)
, m_fRangeRadius(0.0f)
{

}

CHmsLayerTaws::~CHmsLayerTaws()
{
    if (m_initParam.taws_data_input_mode == DataInputType_Sim)
    {
        m_pSimAircraft->Stop();
    }
    for (auto& p : m_warningColorStylePtrs)
    {
        delete p;
    }
    m_warningColorStylePtrs.clear();
}

bool CHmsLayerTaws::Init()
{
    LoadInitParam();

    //注册碰撞信息刷新时的回调
    CollisionManager::getInstance()->registerColliInfoRefreshCallback(
        [this](const CollideInfo& ci){
        m_CurrCollideInfo = ci;
    });

    CollisionManager::getInstance()->SetSafeDistance(m_initParam.lowerSafeDistance, m_initParam.upperSafeDistance, m_initParam.leftRightSafeDistance);
    CollisionManager::getInstance()->SetDebug(m_initParam.show_DebugMsg);

    InitLayoutType(Hms_Screen_Layout_Type::Layout_Middle);

    m_pSimAircraft = SimAircraft::Create(m_initParam);

    m_mapCenter = Vec2(103.622f, 30.99f);
    m_mapRect.origin = m_mapCenter - Vec2(1, 1);
    m_mapRect.size.setSize(2, 2);

    m_fRangeKm = 10;
    m_fRangeRadius = 260;

    auto loadMnger = CHmsGlobal::GetInstance()->GetGeoMapLoadManager();
    loadMnger->SetMapCenter(m_mapCenter);

    m_nodeMapRoot = CHmsNode::Create();
    m_nodeMapRootMap = CHmsNode::Create();

    if (/*loadMnger->IsImageUpdate()*/1)
    {
        HmsAviPf::Vec2 mapCenter;
        HmsAviPf::Rect mapRect;
        HmsAviPf::Vec2 mapDstOffset;
        float fLonLatOffset = 1;
        auto image = loadMnger->GetImage(mapCenter, mapRect, mapDstOffset, fLonLatOffset);
        if (image)
        {
            auto texture = new (std::nothrow) Texture2D();
            if (texture && texture->initWithImage(image))
            {
                m_map = CHmsImageNode::CreateWithTexture(texture);
                m_glprogram = HmsAviPf::GLProgram::createWithFilenames("Shaders/zs_2dMapTawsUni.vsh", "Shaders/zs_2dMapTawsUni.fsh");
                if (!m_glprogram)
                {
                    return false;
                }

                //Add both glprogram int cache during the initialization phase, otherwise, replacing glprogram at runtime will crash.

                m_map->SetGLProgram(m_glprogram);
            }
        }
    }
    loadMnger->Start();
    loadMnger->SetLayer(3);
    m_map->SetPosition(Vec2(0, 0));

    m_nodeClipping = CHmsClippingNode::create();
    m_nodeShowRect = CHmsDrawNode::Create();
    m_nodeClipping->setStencil(m_nodeShowRect);

    this->AddChild(m_nodeClipping);

    m_nodeCompassRoot = CHmsNode::Create();
    do
    {
        m_nodeCompassMask = CHmsDrawNode::Create();

        m_nodeCompassRangeCircle = CHmsMultLineDrawNode::Create();
        m_nodeCompassRangeCircle->SetDrawColor(Color4F::WHITE);

        m_nodeCompassClipping = CHmsClippingNode::create();
#if 1
        m_nodeCompassClipping->setStencil(m_nodeCompassMask);
#else
        m_nodeCompassClipping->setStencil(CHmsDrawNode::Create());
        m_nodeCompassRoot->AddChild(m_nodeCompassMask);
#endif
        m_nodeCompassClipping->AddChild(m_nodeCompassRangeCircle);
        do
        {
            auto CreateCompassLabel = [=](char * strName, float fRotation)
            {
                auto n = CHmsGlobal::CreateLabel(strName, 32, true);
                n->SetRotation(fRotation);
                float fAnchorY = -20 / n->GetContentSize().height;
                n->SetAnchorPoint(0.5f, fAnchorY);
                m_nodeCompassRangeCircle->AddChild(n);
            };

            CreateCompassLabel((char*)"N", 0);
            CreateCompassLabel((char*)"E", 90);
            CreateCompassLabel((char*)"S", 180);
            CreateCompassLabel((char*)"W", 270);
        } while (0);

        std::string strRange = CHmsGlobal::FormatString("%.0f km", m_fRangeKm);
        m_labelCompassRange = CHmsGlobal::CreateLabel(strRange.c_str(), 28);
		if (m_labelCompassRange)
		{
			m_labelCompassRange->SetAnchorPoint(Vec2(0.5f, 0.5f));
			m_labelCompassRange->SetPosition(145, 145);
		}

        strRange = CHmsGlobal::FormatString("%.0f km", m_fRangeKm / 2.0f);
        m_labelCompassRangeMin = CHmsGlobal::CreateLabel(strRange.c_str(), 28);
		if (m_labelCompassRangeMin)
		{
			m_labelCompassRangeMin->SetAnchorPoint(Vec2(0.5f, 0.5f));
			m_labelCompassRangeMin->SetPosition(50, 50);
		}

        auto aftTriangle = CHmsImageNode::Create("res/AftIcon/aftTriangle.png");
		if (aftTriangle)
		{
			aftTriangle->SetAnchorPoint(Vec2(0.5f, 1.0f));
		}


        m_nodeCompassRoot->AddChild(m_nodeCompassClipping);
		if (aftTriangle)
		{
			m_nodeCompassRoot->AddChild(aftTriangle);
		}
		if (m_labelCompassRange)
		{
			m_nodeCompassRoot->AddChild(m_labelCompassRange);
		}

		if (m_labelCompassRangeMin)
		{
			m_nodeCompassRoot->AddChild(m_labelCompassRangeMin);
		}

        //m_nodeCompassRangeCircle add dynamic
    } while (0);

    m_nodeClipping->AddChild(m_nodeMapRoot);
    m_nodeMapRoot->AddChild(m_nodeMapRootMap);
    m_nodeMapRootMap->AddChild(m_map);
    m_nodeMapRoot->AddChild(m_nodeCompassRoot);

    //     if (m_initParam.show_compass)
    //     {
    //         m_nodeCompass = CHmsImageNode::Create("res/Terrain/EwdScaleJy1.png");
    //         m_nodeMapRoot->AddChild(m_nodeCompass);
    //     }

    m_collidePointNodeMask = CHmsNode::Create();
    m_collidePointNode = CHmsImageNode::Create("res/AftIcon/collisionPoint5.png");
	if (m_collidePointNode)
	{
		m_collidePointNode->SetScale(0.45f);
		m_collidePointNode->SetAnchorPoint(Vec2(0.5f, 0.5f));
		m_collidePointNodeMask->AddChild(m_collidePointNode);
	}

    m_nodeMapRoot->AddChild(m_collidePointNodeMask);

    m_collidePointNodeMask->SetLocalZOrder(-10);
    m_nodeMapRootMap->SetLocalZOrder(-20);

    m_nodeLegend = CHmsTerrainLegend::Create(HmsAviPf::Size(100 * 1.5f, 132 * 1.5f));
    if (m_nodeLegend)
    {
        m_nodeLegend->SetAnchorPoint(Vec2(0, 0));
        m_nodeLegend->SetPosition(Vec2(16, 16));
        m_nodeLegend->SetScale(1.0);
        this->AddChild(m_nodeLegend);
    }



    auto config = CHmsConfigXMLReader::GetSingleConfig();


    if (!config->GetTawsColorStyleInfo("TAWS_WarningColorConfig/Style", m_warningColorStylePtrs))
    {
        //read failed and set the warning color
        //warning color style init
        //style 0
        std::vector<float> alts0{ -30, -300 };
        std::vector<HmsAviPf::Vec4> colors0{
            HmsAviPf::Vec4(128.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 1.0f),
            HmsAviPf::Vec4(128.0f / 255.0f, 128.0f / 255.0f, 0.0f / 255.0f, 1.0f),
            HmsAviPf::Vec4(0.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 1.0f)
        };

        //style 1
        std::vector<float> alts1{ 152.0f, 0.0f, -76.0f, -152.0f, -457.0f };
        std::vector<HmsAviPf::Vec4> colors1{
            HmsAviPf::Vec4(128.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 1.0f),
            HmsAviPf::Vec4(128.0f / 255.0f, 128.0f / 255.0f, 0.0f / 255.0f, 1.0f),
            HmsAviPf::Vec4(64.0f / 255.0f, 64.0f / 255.0f, 0.0f / 255.0f, 1.0f),
			HmsAviPf::Vec4(0.0f / 255.0f, 128.0f / 255.0f, 0.0f / 255.0f, 1.0f),
			HmsAviPf::Vec4(0.0f / 255.0f, 41.0f / 255.0f, 0.0f / 255.0f, 1.0f),
			HmsAviPf::Vec4(0.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 1.0f)
        };

        auto pst0 = warningColorStyle::create(alts0, colors0);//style 0
        auto pst1 = warningColorStyle::create(alts1, colors1);//style 1
        if (pst0)
        {
            m_warningColorStylePtrs.push_back(pst0);
        }
        if (pst1)
        {
            m_warningColorStylePtrs.push_back(pst1);
        }
    }

    int nTerrainMultiColor = config->GetConfigInfo("FunctionConfig/TerrainMultiColor", 0);
    if (m_warningColorStylePtrs.size() < 2)
    {
        nTerrainMultiColor = 0;
    }
    if (nTerrainMultiColor == 0)
    {
        SetTawsColorStyle(enTawsColorTheme::EN_TAWS_THEME_0);
        m_nodeLegend->ReGenerate(*m_warningColorStylePtrs[0]);
    }
    else
    {
        SetTawsColorStyle(enTawsColorTheme::EN_TAWS_THEME_1);
        m_nodeLegend->ReGenerate(*m_warningColorStylePtrs[1]);
    }

    m_nodeDebugLabel = CHmsLabel::createWithTTF("103.656700 30.555500", "fonts/msyhbd.ttc", 16);
	if (m_nodeDebugLabel)
	{
		m_nodeDebugLabel->SetTextColor(Color4B(0x61, 0xE4, 0x86, 0xFF));
		m_nodeDebugLabel->SetAnchorPoint(0, 1);
		m_nodeDebugLabel->SetPosition(Vec2(this->GetContentSize().width - 45 - m_nodeDebugLabel->GetContentSize().width, this->GetContentSize().height - 135));
		this->AddChild(m_nodeDebugLabel);
		if (m_initParam.show_DebugMsg)
		{
			m_nodeDebugLabel->SetVisible(true);
		}
		else
		{
			m_nodeDebugLabel->SetVisible(false);
		}
	}
   

    Vec2 posMagText = Vec2(this->GetContentSize().width / 2, this->GetContentSize().height - 20);
    Vec2 posMagValue = posMagText + Vec2(10, 0);

    m_nodeBarrierLayer = CHmsNode::Create();

    m_nodeBarrierLayer->SetColor(Color3B(0, 255, 0));
    m_nodeBarrierLayer->SetAnchorPoint(0.5f, 0.5f);
    m_nodeBarrierLayer->SetPosition(0, 0);
    m_nodeBarrierLayer->SetLocalZOrder(-15);
    m_nodeMapRoot->AddChild(m_nodeBarrierLayer);

#if 0//hvt
    m_nodeHVTLayer = HmsHVTLayer::Create();
    m_nodeMapRoot->AddChild(m_nodeHVTLayer);
#endif

    RegisterAllNodeChild();
    RegisterTouchEvent(2);

    //BarrierTileDataServer* ptds = new BarrierTileDataServer();
    //ptds->StartServer();

    //m_pSimAircraft = SimAircraft::Create(m_initParam);
    //m_pSimAircraft->Start();

    return true;
}

void CHmsLayerTaws::LoadInitParam()
{
    auto config = CHmsConfigXMLReader::GetSingleConfig();
    auto strSvsPath_ = config->GetConfigInfo("ResInfo/SvsPath", "F:/SvsData/");
    //m_initParam.taws_data_input_mode = (enDataInputType)stoi(config->GetConfigInfo("TAWS_Init_Param/taws_data_input_mode", "1"));
    m_initParam.taws_data_input_mode = enDataInputType::DataInputType_UDP;
    m_initParam.show_compass = stoi(config->GetConfigInfo("TAWS_Init_Param/show_compass", "1")) ? true : false;
    m_initParam.show_DebugMsg = stoi(config->GetConfigInfo("TAWS_Init_Param/show_DebugMsg", "0")) ? true : false;
    m_initParam.show_ComFps = stoi(config->GetConfigInfo("TAWS_Init_Param/show_ComFps", "0")) ? true : false;
    m_initParam.sim_trackDir_EastNorstSkySpeed = stoi(config->GetConfigInfo("TAWS_Init_Param/Sim_Track_Direction", "1")) ? true : false;
    m_initParam.lon = stod(config->GetConfigInfo("TAWS_Init_Param/lon", "103.95"));
    m_initParam.lat = stod(config->GetConfigInfo("TAWS_Init_Param/lat", "30.425"));
    m_initParam.alt = stod(config->GetConfigInfo("TAWS_Init_Param/alt", "2600"));
    m_initParam.vs = stod(config->GetConfigInfo("TAWS_Init_Param/vs", "0"));
    m_initParam.gs = stod(config->GetConfigInfo("TAWS_Init_Param/gs", "145"));
    m_initParam.hdg = stod(config->GetConfigInfo("TAWS_Init_Param/hdg", "0"));
    m_initParam.trk = stod(config->GetConfigInfo("TAWS_Init_Param/trk", "0"));
    m_initParam.lowerSafeDistance = stod(config->GetConfigInfo("TAWS_Init_Param/LowerSafeDistance", "0"));
    m_initParam.upperSafeDistance = stod(config->GetConfigInfo("TAWS_Init_Param/UpperSafeDistance", "0"));
    m_initParam.leftRightSafeDistance = stod(config->GetConfigInfo("TAWS_Init_Param/LeftRightSafeDistance", "0"));

    memset(&m_sim_plane_state, 0, sizeof(m_sim_plane_state));
    m_sim_plane_state.curLon = m_initParam.lon;
    m_sim_plane_state.curLat = m_initParam.lat;
    m_sim_plane_state.fCurAlt = m_initParam.alt;
    m_sim_plane_state.curVSpeed = m_initParam.vs;
    m_sim_plane_state.groundSpeedKnot = m_initParam.gs;
    m_sim_plane_state.curTrueHeading = m_initParam.hdg;
    m_sim_plane_state.curMagHeading = m_initParam.hdg;
    m_sim_plane_state.trackdir = m_initParam.trk;
}

void CHmsLayerTaws::UpdateBarrier(const float& delta, const float& fHeading, const HmsAviPf::Vec2d& curLonLat, const float& curAlt, const Vec3d& ENS)
{
    static float s_m_fRangeKm = m_fRangeKm;
    static HMS_TAWS_MODE s_m_eCurMode = m_eCurMode;

    static Vec2d s_barrierLayerOriginalLonLat = curLonLat;//BarrierLayer origin

    BarrierTileDataMgr::getInstance()->UpdateBlocks(Vec2(curLonLat.x, curLonLat.y));
    m_nodeBarrierLayer->SetRotation(-fHeading);

    auto kmPerLon = CHmsGlobal::GetKmPerLongitude(curLonLat.y);
    auto lonPerKm = 1 / kmPerLon;
    auto lon = lonPerKm * m_fRangeKm;
    double pixelsPerLon = m_fRangeRadius / lon;

    m_nodeBarrierLayer->SetContentSize(Size(pixelsPerLon, pixelsPerLon));
    Vec2d vecDelta = curLonLat - s_barrierLayerOriginalLonLat;
    Vec2d vecNewAp = vecDelta * pixelsPerLon;
    auto scy = m_map->GetScaleY();
    vecNewAp.y *= scy;
    m_nodeBarrierLayer->SetAnchorPoint(Vec2(vecNewAp.x, vecNewAp.y) / pixelsPerLon);

    float fCurAlt = curAlt;//meter

    auto ccnt = m_nodeBarrierLayer->GetChildrenCount();
    auto children = m_nodeBarrierLayer->GetChildren();
    for (auto c : children)
    {
        c->SetRotation(fHeading);//make barrier icon always up
        BarrierParameters* barrinfo = static_cast<BarrierParameters*>(c->GetUserData());
        if (!barrinfo)
        {
            continue;
        }
        //set barrier icon alarm color
        auto barrTopAlt = barrinfo->altitude + (barrinfo->height ? barrinfo->height : BARRIER_DEFAULT_HEIGHT);
        if (fCurAlt - m_initParam.lowerSafeDistance < barrTopAlt)
            //if (fCurAlt < barrTopAlt)
        {
            c->SetColor(Color3B(255, 0, 0));
        }
        else
        {
            c->SetColor(Color3B(255, 255, 0));
        }
    }

    static float s_m_map_scaley = m_map->GetScaleY();
    auto scaley = m_map->GetScaleY();
    // range changed, mode changed, the barrier node's pixel position should be reset
    if (fabs(s_m_fRangeKm - m_fRangeKm) > FLT_EPSILON
        || s_m_eCurMode != m_eCurMode
        || fabs(s_m_map_scaley - scaley) > FLT_EPSILON)
    {
        for (auto c : children)
        {
            BarrierParameters* barrinfo = static_cast<BarrierParameters*>(c->GetUserData());
            Vec2d vecBarrLonLat = Vec2d(barrinfo->longitude, barrinfo->latitude);
            auto pixelPosBarrier = convertLonLatToPixelPosOnBarrLayer(vecBarrLonLat, s_barrierLayerOriginalLonLat, pixelsPerLon);

            c->SetPosition(Vec2(pixelPosBarrier.x, pixelPosBarrier.y));
        }

        s_m_fRangeKm = m_fRangeKm;
        s_m_eCurMode = m_eCurMode;
        s_m_map_scaley = scaley;
    }

    static int s_heartbeat = 0;
    int heartbeat = BarrierTileDataMgr::getInstance()->GetHeartBeat();

    if (s_heartbeat != heartbeat)//barrier data changed
    {
        s_barrierLayerOriginalLonLat = curLonLat;
        s_heartbeat = heartbeat;

        //refresh the m_nodeBarrierLayer	
        UpdateBarrierLayer(curLonLat);
    }
}

HmsAviPf::Vec2d CHmsLayerTaws::convertLonLatToPixelPosOnBarrLayer(const Vec2d& barrLonLat, const Vec2d& barrierLayerOriginalLonLat, const double& pixelsPerLon)
{
    Vec2d vecDelta = barrLonLat - barrierLayerOriginalLonLat;//Barrier pos relative to the current position of the aircraft

    auto scy = m_map->GetScaleY();
    Vec2d pixelPosBarrier = vecDelta * pixelsPerLon;
    pixelPosBarrier.y *= scy;
    return pixelPosBarrier;
}

void CHmsLayerTaws::UpdateBarrierLayer(const Vec2d &curLonLat)
{
    auto ccount1 = m_nodeBarrierLayer->GetChildrenCount();
    m_nodeBarrierLayer->RemoveAllChildrenWithCleanup(true);
    auto ccount2 = m_nodeBarrierLayer->GetChildrenCount();

    static Vec2d barrierLayerOriginalLonLat = curLonLat;

    auto blocks = BarrierTileDataMgr::getInstance()->GetRenderBlocks();

    auto kmPerLon = CHmsGlobal::GetKmPerLongitude(curLonLat.y);
    barrierLayerOriginalLonLat = curLonLat;

    auto lonPerKm = 1 / kmPerLon;
    auto lon = lonPerKm * m_fRangeKm;
    auto pixelsPerLon = m_fRangeRadius / lon;

    for (auto block : blocks)
    {
        for (auto barrierInfo : block->GetBarrierInfos())
        {
            Vec2d vecBarrLonLat = Vec2d(barrierInfo->longitude, barrierInfo->latitude);

            CHmsImageNode* BarrNode;
            if (barrierInfo->lightingType[0] != 'N' && barrierInfo->lightingType[0] != 'Q')
            {
                BarrNode = CHmsImageNode::Create("res/AftIcon/Obstacle_with_Light_border.png");
            }
            else
            {
                BarrNode = CHmsImageNode::Create("res/AftIcon/Obstacle_border.png");
            }

			if (BarrNode)
			{
				BarrNode->SetUserData(barrierInfo);
				BarrNode->SetColor(Color3B(255, 255, 0));
				auto pixelPosBarrier = convertLonLatToPixelPosOnBarrLayer(vecBarrLonLat, barrierLayerOriginalLonLat, pixelsPerLon);
				BarrNode->SetPosition(Vec2(pixelPosBarrier.x, pixelPosBarrier.y));
				BarrNode->SetScale(0.15f);
				m_nodeBarrierLayer->AddChild(BarrNode);
			}
        }
    }
}

void CHmsLayerTaws::UpdateCollisionPoint(const float& fHeading, const Vec2d& curLonLat)
{
	if (!m_CurrCollideInfo.bExistCollision && m_collidePointNode)
    {
        m_collidePointNode->SetVisible(false);
        return;
    }

    auto kmPerLon = CHmsGlobal::GetKmPerLongitude(curLonLat.y);
    auto lonPerKm = 1 / kmPerLon;
    auto lon = lonPerKm * m_fRangeKm;
    auto pixelsPerLon = m_fRangeRadius / lon;
    auto scy = m_map->GetScaleY();

    auto vecDelta = m_CurrCollideInfo.vecCollidePos - curLonLat;

    auto pixelPosCollide = vecDelta * pixelsPerLon;
    pixelPosCollide.y *= scy;

	if (m_collidePointNode)
	{
		m_collidePointNode->SetPosition(Vec2(pixelPosCollide.x, pixelPosCollide.y));
		m_collidePointNode->SetVisible(true);
	}


    m_collidePointNodeMask->SetRotation(-fHeading);

}

#include "DataInterface/HmsDataBus.h"

bool CHmsLayerTaws::validateLonLat(const HmsAviPf::Vec2d &wgsPosNormalized)
{
    return wgsPosNormalized.x >= -180
        && wgsPosNormalized.x <= 180
        && wgsPosNormalized.y >= -90
        && wgsPosNormalized.y <= 90;
}

void CHmsLayerTaws::Update(float delta)
{
    PlaneState planeState;
    //Get flight state variables
    if (0 != GetPlaneStateParam(delta, planeState))
    {
        std::cout << "update plane state error" << std::endl;
        return;
    }

    auto vali = validateLonLat(HmsAviPf::Vec2d(planeState.curLon, planeState.curLat));
    if (!vali)
    {
        //若为非法经纬度，则置为默认位置(0,0)
        planeState.curLon = 0.0;
        planeState.curLat = 0.0;
    }

    auto posLonLat = Vec2d(planeState.curLon, planeState.curLat);

    //DEM Map
    auto state = m_map->GetGLProgramState();
    state->setUniformFloat("fCurAlt", planeState.fCurAlt);
    UpdateGeoMapData(Vec2((float)posLonLat.x, (float)posLonLat.y), planeState.curTrueHeading);

    //障碍物视图
    UpdateBarrier(delta, planeState.curTrueHeading, posLonLat, planeState.fCurAlt, planeState.EastNorthSkySpeed);

    //碰撞点视图
    UpdateCollisionPoint(planeState.curTrueHeading, posLonLat);

    //Runway nearest
    //m_outputInfo = RunwayTileDataMgr::getInstance()->CalcNearestRunwayInfo(planeState.curLon, planeState.curLat);

    if (m_nodeCompassRangeCircle)
    {
        //if 使用磁航向显示
        m_nodeCompassRangeCircle->SetRotation(-planeState.curMagHeading);
        //else 使用真航向显示
        //m_nodeCompassRangeCircle->SetRotation(-planeState.curTrueHeading);
    }

#if 0//hvt
    //high voltage wire
    auto kmPerLon = CHmsGlobal::GetKmPerLongitude(planeState.curLat);
    auto kmPerLat = CHmsGlobal::GetKmPerLatitude();
    //auto lon = m_fRangeKm / kmPerLon;
    //double pixelsPerLon = m_fRangeRadius / lon;
    double pixelsPerLon = m_fRangeRadius * kmPerLon / m_fRangeKm;
    double pixelsPerLat = m_fRangeRadius * kmPerLat / m_fRangeKm;

    static Vec2d s_hvtLayerOriginalLonLat = Vec2d(planeState.curLon, planeState.curLat);
    static bool hvtdirty = true;
    if (hvtdirty)
    {
        hvtdirty = false;
        m_nodeHVTLayer->UpdateData(s_hvtLayerOriginalLonLat, pixelsPerLon, pixelsPerLat);
    }
    m_nodeHVTLayer->UpdateDisplay(planeState.curTrueHeading, Vec2d(planeState.curLon, planeState.curLat), pixelsPerLon, pixelsPerLat);
#endif

#if 0
    if (m_pCollideInfoOutput && m_pCollideInfoOutput->bExistCollision)
    {
        m_outputInfo.collideDist = (float)HMS_MIN(65535.00, m_pCollideInfoOutput->fCollideDistance);
    }
    else
    {
        m_outputInfo.collideDist = 65535.000;
    }
#endif

    if (m_pProfileView)
    {
        m_pProfileView->SetHeading(planeState.curTrueHeading);
        m_pProfileView->SetLocation(planeState.curLon, planeState.curLat);
        m_pProfileView->SetAftAltMeter(planeState.fCurAlt);
    }

    if (m_nodeDebugLabel)
    {
        if (m_initParam.show_DebugMsg)
        {
            Vec2d horizontalVelocity(planeState.EastNorthSkySpeed.x, planeState.EastNorthSkySpeed.y);
            auto hSpeed = horizontalVelocity.getLength();
            auto tmplat1 = (int)planeState.curLat;
            auto tmplon1 = (int)planeState.curLon;
            auto tmplat2 = (planeState.curLat - tmplat1)*60.0;
            auto tmplon2 = (planeState.curLon - tmplon1)*60.0;


            m_nodeDebugLabel->SetVisible(true);
            std::string msg1 =
                to_string(planeState.curLat) + std::string(" ") + to_string(planeState.curLon) + std::string("\n")
                + to_string(tmplat1) + std::string("*") + to_string(tmplat2) + std::string("'")
                + to_string(tmplon1) + std::string("*") + to_string(tmplon2) + std::string("'")
                + std::string("\nAlt ") + to_string((int)planeState.fCurAlt) + std::string(" M,  ") + to_string((int)MeterToFeet(planeState.fCurAlt)) + std::string(" ft")
                + std::string("\nVS ") + to_string(planeState.curVSpeed) + std::string(" ft/min")
                + std::string("\nTrueHDG ") + to_string(planeState.curTrueHeading)
                + std::string("\nMagHDG ") + to_string(planeState.curMagHeading)
                + std::string("\nTRK ") + to_string(planeState.trackdir)
                + std::string("\nGS ") + to_string((int)KnotToKMH(planeState.groundSpeedKnot)) + std::string(" Km/h ") + to_string((int)planeState.groundSpeedKnot) + std::string(" Knot")

                + std::string("\nRA ") + to_string(planeState.AltitudeRadio)
                + std::string("\nDpCmd ") + to_string(planeState.DispCommond)
                + std::string("\nMagC ") + to_string(planeState.MagnetismCourse)
                + std::string("\nRedAT ") + to_string(planeState.RedArlarmTimeSet)
                + std::string("\nYellowAT ") + to_string(planeState.YellowArlarmTimeSet)
                + std::string("\nTime ") + to_string(planeState.Time)
                + std::string("\nVCtol ") + to_string(planeState.VideoContor)
                + std::string("\nES ") + to_string(planeState.EastNorthSkySpeed.x)
                + std::string("\nNS ") + to_string(planeState.EastNorthSkySpeed.y)
                + std::string("\nSS ") + to_string(planeState.EastNorthSkySpeed.z)
                + std::string("\nHS ") + to_string(hSpeed)
                //+ std::string("\nSpare1 ") + to_string(planeState.Spare1)
                + std::string("\nisCusCLLen") + to_string(planeState.isCustomLowColliPointHeight)
                //+ std::string("\nSpare2 ") + to_string(planeState.Spare2)
                + std::string("\nCusCLLen ") + to_string(planeState.customLowColliPointHeight)

                //+ std::string("\nCollDst ") + to_string(m_outputInfo.collideDist)

                //+ std::string("\nnstICAO ") + std::string(m_outputInfo.nearestICAO_id)
                //+ std::string("\nnstRwId ") + std::string(m_outputInfo.nearestRunway_id)
                //+ std::string("\nnstRW ") + to_string(m_outputInfo.nearestRunwayDist)
                //+ std::string("\nnstAP ") + to_string(m_outputInfo.nearestAptDist)
                //+ std::string("\nnstRWAlt ") + to_string(m_outputInfo.nearestRunwayAlt)
#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_WIN32)
                // + std::string("\nnstRWAng ") + to_string(m_outputInfo.nearestRunwayAngle)
                + std::string("\nnCyc ") + to_string(g_nCyc);
#else
                ;
            //+ std::string("\nnstRWAng ") + to_string(m_outputInfo.nearestRunwayAngle);
#endif
            m_nodeDebugLabel->SetString(msg1);
        }
        else
        {
            m_nodeDebugLabel->SetVisible(false);
        }
    }
}


//************************************
// Method:    GetPlaneStateParam
// FullName:  CHmsLayerTaws::GetPlaneStateParam
// Access:    public 
// Desc:      
// Returns:   int
// Qualifier:
// Parameter: PlaneState & planeState
// Author:    yan.jiang
//************************************
int CHmsLayerTaws::GetPlaneStateParam(const float& delta, PlaneState& planeState)
{
    if (m_initParam.taws_data_input_mode == DataInputType_Sim)
    {
        auto craftstate = m_pSimAircraft->GetAircraftState();

        m_sim_plane_state.curLon = craftstate.curLon;
        m_sim_plane_state.curLat = craftstate.curLat;

        m_sim_plane_state.curVSpeed = craftstate.curVSpeed;//feet/mim	
        m_sim_plane_state.fCurAlt = craftstate.fCurAlt;

        planeState = m_sim_plane_state;

        //reset position
        auto config = CHmsConfigXMLReader::GetSingleConfig();
        auto initLon = stod(config->GetConfigInfo("TAWS_Init_Param/lon", "103.4533"));
        auto initLat = stod(config->GetConfigInfo("TAWS_Init_Param/lat", "30.5496"));
        auto distKm = CalculateDistanceKM(initLon, initLat, m_sim_plane_state.curLon, m_sim_plane_state.curLat);
        if (distKm > 20)//>20km reset plane
        {
            m_pSimAircraft->InitWithState(m_initParam);
        }
    }
    else if (m_initParam.taws_data_input_mode == DataInputType_UDP)
    {
        CHmsDataBus *dataBus = CHmsGlobal::GetInstance()->GetDataBus();
        if (dataBus == nullptr)
        {
            return -1;
        }

        auto aftData = dataBus->GetData();
        planeState.fCurAlt = dataBus->GetMslAltitudeMeter();
        planeState.curLon = aftData->longitude;
        planeState.curLat = aftData->latitude;
        planeState.curTrueHeading = aftData->trueHeading;
        planeState.curMagHeading = aftData->magneticHeading;
        planeState.curVSpeed = aftData->verticalSpeed;
        planeState.groundSpeedKnot = aftData->groundSpeed;
        planeState.EastNorthSkySpeed.x = aftData->speedEast;
        planeState.EastNorthSkySpeed.y = aftData->speedNorth;
        planeState.EastNorthSkySpeed.z = aftData->speedSkyUp;
        planeState.trackdir = aftData->trueTrack;
        planeState.AltitudeRadio = aftData->heightAboveTerrainMeter;
    }
    else
    {
        std::cout << "error data input mode." << std::endl;
        return -1;//error, inpossible here
    }

    return 0;
}


void CHmsLayerTaws::ResetViewer()
{
    ResizeLayer(GetSize());
}

void CHmsLayerTaws::OnEnter()
{
    CHmsNode::OnEnter();
    SetScheduleUpdate();//jy to do
    CHmsGlobal::GetInstance()->GetGeoMapLoadManager()->SetUsingLoading();
    if (m_initParam.taws_data_input_mode == DataInputType_Sim)
    {
        m_pSimAircraft->Start();
    }
}

void CHmsLayerTaws::OnExit()
{
    CHmsNode::OnExit();
    SetUnSchedule();// jy todo
    CHmsGlobal::GetInstance()->GetGeoMapLoadManager()->SetUnusedLoading();
}

void CHmsLayerTaws::UpdateGeoMapData(const HmsAviPf::Vec2 & pos, float fHeading)
{
    auto loadMnger = CHmsGlobal::GetInstance()->GetGeoMapLoadManager();
    loadMnger->SetMapCenter(pos);

    if (loadMnger->IsImageUpdate())
    {
        HmsAviPf::Vec2 mapCenter;
        HmsAviPf::Rect mapRect;
        HmsAviPf::Vec2 mapDstOffset;
        float fLonLatOffset = 1;
        auto image = loadMnger->GetImage(mapCenter, mapRect, mapDstOffset, fLonLatOffset);
        if (image)
        {
            auto texture = new (std::nothrow) Texture2D();
            if (texture && texture->initWithImage(image))
            {
                //image->saveToFile("D:\\123sz.png");
                m_map->SetTexture(texture);
            }
            HMS_SAFE_RELEASE(texture);

            if (m_pProfileView)
            {
                m_pProfileView->SetTerrainData(image, mapCenter, mapRect, mapDstOffset, fLonLatOffset);
            }

            m_mapCenter = mapCenter;
            m_mapRect = mapRect;
            m_mapDstOffset = mapDstOffset;

            //scale the the x and the y at the some range, and then use the x range to show distance 
            m_map->SetScaleY(m_mapDstOffset.y / m_mapDstOffset.x);

            //auto scale =  m_fRangeKm / (m_mapDstOffset.x / m_map->getContentSize().width) / m_fRangeRadius;
            // 			auto d1 = m_mapDstOffset.x / m_map->getContentSize().width;
            // 			auto d2 = m_fRangeKm / d1;
            // 			auto d3 = m_fRangeRadius / d2 ;
            auto scale = (m_mapDstOffset.x * m_fRangeRadius) / (m_fRangeKm * m_map->GetContentSize().width);
            m_nodeMapRootMap->SetScale(scale);
        }
    }

    if (m_mapRect.containsPoint(pos))
    {
        auto relatePos = pos - m_mapRect.origin;
        Vec2 ap(relatePos.x / m_mapRect.size.width, relatePos.y / m_mapRect.size.height);
        //std::cout <<"pos:"<< pos << "relatePos:" << relatePos << "ap:" << ap << std::endl;
        m_map->SetVisible(true);
        m_map->SetAnchorPoint(ap);
        //auto state = m_map->GetGLProgramState();
        //state->setUniformVec2("v_curPlanePos", Vec2(ap.x, 1.0f - ap.y));
        //set reach point after 30 seconds.
        //speed

        //state->setUniformFloat("fHeading", fHeading - 90);//convert

        //CHmsMicroSecondTime tm;
        //double ctime = tm.GetTimeCurrent();
        //std::cout << "ctime=" << fixed << ctime << std::endl;
        //ctime -= int(ctime);
        //state->setUniformFloat("fTime", ctime);

        //state->setUniformFloat("fTime", );
        //state->setUniformVec2("v_curPlanePos", Vec2(0.5f,0.5f));
        m_map->SetRotation(-fHeading);
    }
    else
    {
        m_map->SetVisible(false);
    }
}


void CHmsLayerTaws::ResizeLayer(const HmsAviPf::Size & size)
{
    SetContentSize(size);

    Vec2 posClipping = size / 2.0F;
    if (m_nodeClipping)
    {
        m_nodeClipping->SetPosition(posClipping);
    }

    if (m_nodeShowRect)
    {
        m_nodeShowRect->clear();
        m_nodeShowRect->DrawSolidRect(-posClipping.x, -posClipping.y, posClipping.x, posClipping.y, Color4F::WHITE);
    }

    UpdateShowContent(size);

    if (m_nodeBtnOut)
    {
        m_nodeBtnOut->SetPosition(Vec2(size.width - 10, 10));
    }

    if (m_nodeBtnIn)
    {
        m_nodeBtnIn->SetPosition(Vec2(size.width - 10 - m_nodeBtnOut->GetContentSize().width - 10, 10));
    }

    if (m_nodeBtnSwitchMode)
    {
        m_nodeBtnSwitchMode->SetPosition(Vec2(size.width - 10, 10 + m_nodeBtnOut->GetContentSize().height + 10));
    }

    if (m_nodeLegend)
    {
        m_nodeLegend->SetPosition(Vec2(16, 16));
    }

    if (m_nodeDebugLabel)
    {
        m_nodeDebugLabel->SetPosition(Vec2(this->GetContentSize().width - 30 - m_nodeDebugLabel->GetContentSize().width, this->GetContentSize().height - 140));
    }

    if (m_nodeColliDistLabel)
    {
        m_nodeColliDistLabel->SetPosition(Vec2(40, this->GetContentSize().height - 40));
    }

    Vec2 posMagText = Vec2(this->GetContentSize().width / 2, this->GetContentSize().height - 60);
    Vec2 posMagValue = posMagText + Vec2(10, 0.0);
}

void CHmsLayerTaws::UpdateShowContent(const HmsAviPf::Size & size)
{
    if (!m_nodeMapRoot)
    {
        return;
    }

    float radius = 0.0;

    switch (m_eCurMode)
    {
    case HMS_TAWS_MODE::Center:
        m_nodeMapRoot->SetPosition(Vec2(0, 0));
        radius = fmin(size.width, size.height) / 2.0F - 126;
        //         if (m_nodeCompass)
        //         {
        //             double compass_scale = radius / (m_nodeCompass->GetContentSize().width / 2.0F);
        //             m_nodeCompass->SetScale(compass_scale);
        //         }
        m_fRangeRadius = radius;
        RedrawDistanceCircle(radius, false);

        //         if (m_labelCompassRange)
        //         {
        //             Vec2 handle(0, radius);
        //             auto pos = handle.rotateByAngle(Vec2(), (float)ToRadian(-45.0F));
        //             m_labelCompassRange->SetPosition(pos);
        //         }
        // 
        //         if (m_labelCompassRangeMin)
        //         {
        //             Vec2 handle(0, radius / 2.0F);
        //             auto pos = handle.rotateByAngle(Vec2(), (float)ToRadian(-45.0F));
        //             m_labelCompassRangeMin->SetPosition(pos);
        //         }
        break;

    case HMS_TAWS_MODE::Arc:
        m_nodeMapRoot->SetPosition(Vec2(0, 45 - size.height / 2.0F));
        radius = fmin(size.width, size.height) - 45 - 126;
        //         if (m_nodeCompass)
        //         {
        //             double compass_scale = radius / (m_nodeCompass->GetContentSize().width / 2.0F);
        //             m_nodeCompass->SetScale((float)compass_scale);
        //         }
        m_fRangeRadius = radius;
        RedrawDistanceCircle(radius, true);

        //         if (m_labelCompassRange)
        //         {
        //             Vec2 handle(0, radius + 30);
        //             auto pos = handle.rotateByAngle(Vec2(), (float)ToRadian(-30.0F));
        //             m_labelCompassRange->SetPosition(pos);
        //         }
        // 
        //         if (m_labelCompassRangeMin)
        //         {
        //             Vec2 handle(0, radius / 2.0F + 20);
        //             auto pos = handle.rotateByAngle(Vec2(), (float)ToRadian(-30.0F));
        //             m_labelCompassRangeMin->SetPosition(pos);
        //         }
        break;
    }

    if (m_nodeMapRootMap && m_map)
    {
        auto scale = (m_mapDstOffset.x * m_fRangeRadius) / (m_fRangeKm * m_map->GetContentSize().width);
        m_nodeMapRootMap->SetScale(scale);
    }
}

void CHmsLayerTaws::UpdateRangeInfo()
{
    auto lat = CHmsGlobal::GetInstance()->GetDataBus()->GetData()->latitude;

    double dPerKm = CHmsGlobal::GetKmPerLongitude(lat);
    double dWgcAngle = m_fRangeKm * 1.5F / dPerKm;

    int nIndex = 0;
    double dAngeIndex = 5.0;

    while (nIndex < 8 && dAngeIndex > dWgcAngle)
    {
        auto temp = dAngeIndex / 2.0;
        if (dAngeIndex > dWgcAngle && temp < dWgcAngle)
        {
            if ((dWgcAngle / dAngeIndex) < (temp / dWgcAngle * 0.7F))
            {
                nIndex++;
            }

            break;
        }
        dAngeIndex = temp;
        nIndex++;
    };

    CHmsGlobal::GetInstance()->GetGeoMapLoadManager()->SetLayer(nIndex);
}

void CHmsLayerTaws::ZoomIn()
{
    auto tempRangeKm = m_fRangeKm / 2;
    if (tempRangeKm < 0.625)
    {
        tempRangeKm = 0.625;
    }
    SetRange(tempRangeKm);
}

void CHmsLayerTaws::ZoomOut()
{
    auto tempRangeKm = m_fRangeKm * 2;
    if (tempRangeKm >= 160)
    {
        tempRangeKm = 160;
    }
    SetRange(tempRangeKm);
}

//************************************
// Method:    generateFormatStringForFloatNumber
// FullName:  CHmsLayerTaws::generateFormatStringForFloatNumber
// Access:    public static 
// Desc:      得到小数对应的格式化输出字符串,例如  0.625  得到 "%0.3f" , 5.0 得到 "%0.0f"
// Returns:   std::string
// Qualifier:
// Parameter: const double & floatPointNumber
// Author:    yan.jiang
//************************************
std::string CHmsLayerTaws::generateFormatStringForFloatNumber(const double& floatPointNumber)
{
    char buf[64];
    auto pn = floatPointNumber - (int)floatPointNumber;
    int len = 0;
    if (pn != 0)//yes, no need to use abs > 0.00000000000001
    {
        sprintf(buf, "%g", pn);
        len = strlen(buf) - 2;
    }
    std::string str;
    str = "%." + std::to_string(len) + std::string("f");
    return str;
}

void CHmsLayerTaws::SetRange(float fRangeKm)
{
    if (fabs(fRangeKm - m_fRangeKm) < FLT_EPSILON)
    {
        return;
    }
    m_fRangeKm = fRangeKm;

    auto fmtstr = CHmsLayerTaws::generateFormatStringForFloatNumber(m_fRangeKm) + " km";
    std::string strRange = CHmsGlobal::FormatString(fmtstr.c_str(), m_fRangeKm);
	if (m_labelCompassRange)
	{
		CHmsGlobal::SetLabelText(m_labelCompassRange, strRange.c_str());
	}

    fmtstr = CHmsLayerTaws::generateFormatStringForFloatNumber(m_fRangeKm / 2.0F) + " km";
    strRange = CHmsGlobal::FormatString(fmtstr.c_str(), m_fRangeKm / 2.0F);

	if (m_labelCompassRangeMin)
	{
		CHmsGlobal::SetLabelText(m_labelCompassRangeMin, strRange.c_str());
	}

    UpdateRangeInfo();
    if (m_nodeMapRootMap && m_map)
    {
        auto scale = (m_mapDstOffset.x * m_fRangeRadius) / (m_fRangeKm * m_map->GetContentSize().width);
        m_nodeMapRootMap->SetScale(scale);
    }

    if (m_pProfileView)
    {
        m_pProfileView->SetDistanceKm(m_fRangeKm);
    }
}

void CHmsLayerTaws::SetModeString(std::string strMode)
{
    if (strMode == "center")
    {
        SetMode(HMS_TAWS_MODE::Center);
    }
    else if (strMode == "arc")
    {
        SetMode(HMS_TAWS_MODE::Arc);
    }
}

void CHmsLayerTaws::SetMode(HMS_TAWS_MODE eMode)
{
    if (eMode != m_eCurMode)
    {
        m_eCurMode = eMode;
        auto size = GetContentSize();
        UpdateShowContent(size);
    }
}

void CHmsLayerTaws::SetShowCompass(bool bShow)
{
    //not implemented
}

void CHmsLayerTaws::SetProfileView(CHmsProfileView * pProfileView)
{
    m_pProfileView = pProfileView;
    if (m_pProfileView)
    {
        m_pProfileView->SetDistanceKm(m_fRangeKm);
    }
}

void CHmsLayerTaws::SetTawsColorStyle(const enTawsColorTheme& theme)
{
    int styleIdx = theme;
    if ((int)m_warningColorStylePtrs.size() <= styleIdx)
    {
        std::cout << "warning color style setting error." << std::endl;
        return;
    }

    auto state = m_map->GetGLProgramState();
    auto tcnt = m_warningColorStylePtrs[styleIdx]->ThresholdCnt();
    auto atss = m_warningColorStylePtrs[styleIdx]->AltThresholds();
    auto clrs = m_warningColorStylePtrs[styleIdx]->Colors();
    auto pa = atss->data();
    auto pc = clrs->data();

    state->setUniformInt("sizeAltThresholds", tcnt);
    state->setUniformFloatv("AltThresholds", tcnt, pa); // pointer类型的，需要自己保存数据内存， setUniformFloatv不会拷贝
    state->setUniformVec4v("colorsUni", tcnt + 1, pc);  // pointer类型的，需要自己保存数据内存， setUniformVec4v不会拷贝

    m_nodeLegend->ReGenerate(*m_warningColorStylePtrs[styleIdx]);//图例更新
}

void CHmsLayerTaws::RedrawDistanceCircle(float fRangeRadius, bool bUseMask)
{
    auto GetRotatePosition = [](float fRadius, float fAngleRotate)
    {
        Vec2 handle(0, fRadius);
        auto pos = handle.rotateByAngle(Vec2(), (float)ToRadian(fAngleRotate));
        return pos;
    };

    float fHalfRangeRasius = fRangeRadius * 0.5f;

    if (m_nodeCompassRangeCircle)
    {
        m_nodeCompassRangeCircle->clear();
        m_nodeCompassRangeCircle->DrawRing(fRangeRadius, TAWS_COMPASS_LINE_WIDTH, 75);
        m_nodeCompassRangeCircle->DrawRing(fHalfRangeRasius, TAWS_COMPASS_LINE_WIDTH, 60);

        m_nodeCompassRangeCircle->DrawLineAngle(fRangeRadius, fRangeRadius - 10, 0, TAWS_COMPASS_LINE_WIDTH);
        m_nodeCompassRangeCircle->DrawLineAngle(fRangeRadius, fRangeRadius - 10, 90, TAWS_COMPASS_LINE_WIDTH);
        m_nodeCompassRangeCircle->DrawLineAngle(fRangeRadius, fRangeRadius - 10, 180, TAWS_COMPASS_LINE_WIDTH);
        m_nodeCompassRangeCircle->DrawLineAngle(fRangeRadius, fRangeRadius - 10, 270, TAWS_COMPASS_LINE_WIDTH);

        auto child = m_nodeCompassRangeCircle->GetChildren();
        for (auto c : child)
        {
            auto contentHeight = c->GetContentSize().height;
            float fAnchorY = -(fRangeRadius - contentHeight - 10) / contentHeight;
            c->SetAnchorPoint(0.5f, fAnchorY);
        }
    }

    float fAngleRot = -45.0f;
    Vec2 posRangeText;
    Vec2 posRangeMinText;

    m_nodeCompassMask->clear();
    if (bUseMask)
    {
        m_nodeCompassClipping->setInverted(false);
        m_nodeCompassMask->DrawFan(-30, 30, fRangeRadius + 2);
        fAngleRot = -35.0f;

        posRangeText = GetRotatePosition(fRangeRadius, fAngleRot + 2);
        posRangeMinText = GetRotatePosition(fRangeRadius / 2.0f, fAngleRot);
    }
    else
    {
        posRangeText = GetRotatePosition(fRangeRadius, fAngleRot);
        posRangeMinText = GetRotatePosition(fRangeRadius / 2.0f, fAngleRot);
        m_nodeCompassClipping->setInverted(true);
        //m_nodeCompassMask->DrawDisk(fRangeRadius + 2);

        m_nodeCompassMask->DrawSolidRect(Rect(posRangeText + Vec2(-40, -30), Size(80, 60)), Color4F::RED);
        m_nodeCompassMask->DrawSolidRect(Rect(posRangeMinText + Vec2(-40, -30), Size(80, 60)), Color4F::RED);
    }

    if (m_labelCompassRange)
    {
        m_labelCompassRange->SetPosition(posRangeText);
    }

    if (m_labelCompassRangeMin)
    {
        m_labelCompassRangeMin->SetPosition(posRangeMinText);
    }
}

