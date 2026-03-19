#include "HmsNavSvsObjectMgr.h"
#include "render/HmsRenderer.h"
#include "render/HmsGLStateCache.h"
#include "NavSvs/HmsNavSvsMgr.h"
#include "Calculate/CalculateZs.h"

static void CHmsSvsObjectPutPipeValue(SvsFlyPipeDataStu *pStu, double lon, double lat, double height)
{
    pStu->lonLat.x = lon;
    pStu->lonLat.y = lat;
    pStu->height = height;
}

static void CHmsSvsObjectMgrSetFlyPipeTestData(CHmsNavSvsObjectFlyPipe *pFlyPipe)
{
    std::vector<Vec2> vecAllData;
    Vec2 *pLastLonLat = nullptr;

    std::vector<Vec2> vecLonlat;
    {
        Vec2 lonlatStart = { 103.90594456f, 30.44512153f };
        Vec2 lonlatEnd = { 104.00894165f, 30.67689652f };
        vecLonlat.push_back(lonlatStart);
        vecLonlat.push_back(lonlatEnd);
    }
    {
        Vec2 lonlatStart = { 104.00894165f, 30.67689652f };
        Vec2 lonlatEnd = { 103.38546800f, 31.01645600f };
        vecLonlat.push_back(lonlatStart);
        vecLonlat.push_back(lonlatEnd);
    }
    for (int i = 0; i < (int)vecLonlat.size() - 1; i += 2)
    {
        Vec2 *pLonlatStart = &vecLonlat[i];
        Vec2 *pLonlatEnd = &vecLonlat[i + 1];
        std::vector<Vec2> vec;
        CHmsNavSvsMathHelper::InsertLonLat(pLonlatStart, pLonlatEnd, vec, 1);
        auto iter = vec.begin();
        if (pLastLonLat && pLastLonLat->equals(vec.front()))
        {
            ++iter;
        }
        vecAllData.insert(vecAllData.begin(), iter, vec.end());

        pLastLonLat = &vecAllData.back();
    }
    int vecSize = vecAllData.size();
    SvsFlyPipeDataStu *pStu = (SvsFlyPipeDataStu*)malloc(vecSize * sizeof(SvsFlyPipeDataStu));
    for (int i = 0; i < vecSize; ++i)
    {
        Vec2 *pPos = &vecAllData[i];
        CHmsSvsObjectPutPipeValue(&pStu[i], pPos->x, pPos->y, 2000);
    }
    pFlyPipe->SetData(pStu, vecSize, 0.3f);

    free(pStu);
}

static void CHmsSvsObjectMgrPutRunway(SvsLogicRunwayStu *pStu, const char * ident, float lon, float lat, float height, float bearing)
{
    memset(pStu->ident, 0, sizeof(pStu->ident));
    strcpy(pStu->ident, ident);
    pStu->lonLat.x = lon;
    pStu->lonLat.y = lat;
    pStu->height = FeetToMeter(height);
    pStu->bearing = bearing; // HmsMagneticHeadingToTrueHeading(lon, lat, pStu->height / 1000.0, bearing);
}

static void CHmsSvsObjectMgrSetRunwayTestData(CHmsNavSvsObjectRunway *pRunway)
{
    int vecSize = 4;
    SvsRunwayDataStu *pStu = (SvsRunwayDataStu*)malloc(vecSize * sizeof(SvsRunwayDataStu));
    int index = 0;
    {
        CHmsSvsObjectMgrPutRunway(&pStu[index].runwayOne, "02L", 103.93998888888889f, 30.56346388888889f, 1617.0f, 24.00000035762787f);
        CHmsSvsObjectMgrPutRunway(&pStu[index].runwayTwo, "20R", 103.9540138888889f, 30.59358333333333f, 1625.0f, 204.00000303983688f);
        pStu[index].width = FeetToMeter(148);
        pStu[index].length = FeetToMeter(11811);
        ++index;
    }
    {
        CHmsSvsObjectMgrPutRunway(&pStu[index].runwayOne, "02R", 103.93666944444445f, 30.51949722222222f, 1681.0f, 24.00000035762787f);
        CHmsSvsObjectMgrPutRunway(&pStu[index].runwayTwo, "20L", 103.95068611111111f, 30.54962777777778f, 1629.0f, 204.00000303983688f);
        pStu[index].width = FeetToMeter(197);
        pStu[index].length = FeetToMeter(11811);
        ++index;
    }
    {
        CHmsSvsObjectMgrPutRunway(&pStu[index].runwayOne, "36L", 116.57419166666666f, 40.07345f, 107.0f, 359.00000534951687f);
        CHmsSvsObjectMgrPutRunway(&pStu[index].runwayTwo, "18R", 116.56967222222222f, 40.10206111111111f, 115.0f, 179.00000266730785f);
        pStu[index].width = FeetToMeter(164);
        pStu[index].length = FeetToMeter(10499);
        ++index;
    }
    {
        CHmsSvsObjectMgrPutRunway(&pStu[index].runwayOne, "36R", 116.60016666666667f, 40.05546666666666f, 98.0f, 359.00000534951687f);
        CHmsSvsObjectMgrPutRunway(&pStu[index].runwayTwo, "18L", 116.59482222222222f, 40.08945277777778f, 110.0f, 179.00000266730785f);
        pStu[index].width = FeetToMeter(197.0);
        pStu[index].length = FeetToMeter(12467.0);
        ++index;
    }

    pRunway->SetData(pStu, vecSize);
    free(pStu);
}

CHmsNavSvsObjectMgr::CHmsNavSvsObjectMgr()
{
    m_pGlProgram = GLProgram::createWithFilenames("Shaders/zs_navsvsobject.vsh", "Shaders/zs_navsvsobject.fsh");
    if (NULL == m_pGlProgram)
    {
        return;
    }
    SetGLProgram(m_pGlProgram);
    m_pGlProgram->use();

    m_locUniformColor = m_pGlProgram->getUniformLocation("u_color");
    m_locUniformMVP = m_pGlProgram->getUniformLocation("HMS_MVPMatrix");
    m_locUniformRelativeOrigin = m_pGlProgram->getUniformLocation("u_relativeOrigin");

    m_pSvsMgr = CHmsNavSvsMgr::GetInstance();
    CHmsNavSvsObjectBase::s_pSvsMgr = m_pSvsMgr;
    CHmsNavSvsObjectBase::s_pSvsObjectMgr = this;

    auto pObjectRunway = new CHmsNavSvsObjectRunway;
    CHmsSvsObjectMgrSetRunwayTestData(pObjectRunway);
    m_vecObject.push_back(pObjectRunway);

    auto pObjectFlyPipe = new CHmsNavSvsObjectFlyPipe;
    CHmsSvsObjectMgrSetFlyPipeTestData(pObjectFlyPipe);
    m_vecObject.push_back(pObjectFlyPipe);

    auto pObjectRouteTrack = new CHmsNavSvsObjectRouteTrack;
    m_vecObject.push_back(pObjectRouteTrack);

    auto pObjectADSBMgr = new CHmsNavSvsObjectADSBMgr;
    m_vecObject.push_back(pObjectADSBMgr);

    auto pObjectPlane = new CHmsNavSvsObjectPlane;
    m_vecObject.push_back(pObjectPlane);
}

CHmsNavSvsObjectMgr::~CHmsNavSvsObjectMgr()
{

}

CHmsNavSvsObjectMgr* CHmsNavSvsObjectMgr::GetInstance()
{
    static CHmsNavSvsObjectMgr* s_pIns = nullptr;
    if (!s_pIns)
    {
        s_pIns = new CHmsNavSvsObjectMgr;
    }
    return s_pIns;
}

void CHmsNavSvsObjectMgr::Update(float delta)
{
    for (auto &c : m_vecObject)
    {
        if (c->IsEnable())
        {
            c->Update(delta);
        }    
    }
}

void CHmsNavSvsObjectMgr::OnDraw()
{
    for (auto &c : m_vecObject)
    {
        if (c->IsEnable())
        {
            m_pGlProgram->use();
            m_pGlProgram->setUniformLocationWith3f(m_locUniformRelativeOrigin,
                m_pSvsMgr->m_relativeOrigin.x, m_pSvsMgr->m_relativeOrigin.y, m_pSvsMgr->m_relativeOrigin.z);

            c->OnDraw();
        }    
    }
}

void CHmsNavSvsObjectMgr::SetNavSvsObjectEnable(NavSvsObjectType type, bool bEnable)
{
    for (auto &c : m_vecObject)
    {
        if (c->GetNavSvsObjectType() == type)
        {
            c->SetEnable(bEnable);
            break;
        }
    }
}

CHmsNavSvsObjectBase* CHmsNavSvsObjectMgr::GetNavSvsObject(NavSvsObjectType type)
{
    for (auto &c : m_vecObject)
    {
        if (c->GetNavSvsObjectType() == type)
        {
            return c;
        }
    }
    return nullptr;
}
