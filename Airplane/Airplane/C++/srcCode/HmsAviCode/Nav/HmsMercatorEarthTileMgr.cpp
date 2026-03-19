#include "HmsMercatorEarthTileMgr.h"
#include "Calculate/CalculateZs.h"
#include "HmsNavComm.h"
#include "stdio.h"
USING_NS_HMS;


CHmsMercatorEarthTileMgr::CHmsMercatorEarthTileMgr()
	: m_sizeScreen(1280, 1080)
	, m_nLayer(3)
	, m_fShowRadianHorizontal(60)
	, m_fShowRadianVertical(60)
	, m_nMaxLayer(15)
	, m_nLastLayer(0)
	, m_layer_hCount(15)
	, m_layer_vCount(15)
	, m_layer_bgGrade(4)
	, m_frame2DInterface(nullptr)
{

}

CHmsMercatorEarthTileMgr::~CHmsMercatorEarthTileMgr()
{

}

void CHmsMercatorEarthTileMgr::UpdateShowRadian(float vertical, float horizontal)
{
	m_fShowRadianHorizontal = horizontal;
	m_fShowRadianVertical = vertical;
}

void CHmsMercatorEarthTileMgr::SetScreenSize(const HmsAviPf::Size & size)
{
	m_sizeScreen = size;
}

bool CHmsMercatorEarthTileMgr::Init()
{
	if (!CHmsNode::Init())
	{
		return false;
	}

	int nLayer = m_layer_bgGrade;
	int nLayerCnt = 1 << nLayer;
	for (int i = 0; i < nLayerCnt; i++)
	{
		for (int j = 0; j < nLayerCnt; j++)
		{
			auto tile = CHmsMercatorEarthTile::Create(nLayer, i, j);
#if _NAV_SVS_LOGIC_JUDGE
			if (tile)
#endif
			{
				tile->InitTileInfo3DEarth(nLayer, i, j);
				tile->SendLoadTextureRequest(nLayer, i, j, true);
				tile->SetPosition3D(Vec3(0, 0, 0));
				//this->AddEarthNode(tile);
				m_rootTile.pushBack(tile);
				//return true;
			}
		}
	}

	for (int i = 0; i < m_layer_hCount; i++)
	{
		std::list<CHmsMercatorEarthTile*> list;
		for (int j = 0; j < m_layer_vCount; j++)
		{
			auto tile = CHmsMercatorEarthTile::Create(m_layer_bgGrade+1, i, j);
#if _NAV_SVS_LOGIC_JUDGE
			if (tile)
#endif
			{
				tile->InitTileInfo3DEarth(m_layer_bgGrade + 1, i, j);
				tile->SendLoadTextureRequest(m_layer_bgGrade + 1, i, j, true);
				tile->SetPosition3D(Vec3(0, 0, 0));
				tile->retain();
				list.push_back(tile);
			}
		}
		m_listLayers.push_back(list);
	}

	m_pNavImageTileMgr = CHmsNavImageTileMgr::Create();
	//m_rootTile.pushBack(m_pNavImageTileMgr->m_pNavImageTile);

	m_pSkyboxMgr = CHmsEarthSkyBoxMgr::Create();

	return true;
}

Vec2 CHmsMercatorEarthTileMgr::GetScreenCenterXY(int nLayer)
{
	Vec2 ret;
	Vec2 center;
	center.x = m_sizeScreen.width / 2;
	center.y = m_sizeScreen.height / 2;
	Vec3 pos;
	Vec2 temp;
	if (m_frame2DInterface && m_frame2DInterface->Frame2DPointToEarthPoint(center, pos))
	{
		Vec2 lonLat = CHmsFrame2DInterface::EarthPointToLonLat(pos);

		//lonLat.setX(lonLat.x() * D_TO_R);
		//lonLat.setY(lonLat.y() * D_TO_R);

		int nLayerCnt = 1 << nLayer;
		int nLayerHalfCnt = 1 << (nLayer - 1);

		for (int x = 0; x<nLayerCnt; ++x)
		{
			double xStart = (x - nLayerHalfCnt)*180.0 / nLayerHalfCnt;
			if (xStart > lonLat.x)
			{
				ret.x = x;
				break;
			}
		}
		for (int y = 0; y<nLayerCnt; ++y)
		{
#define ZS_PI 3.14159265358979323846
			double yStart = (nLayerHalfCnt - y)*ZS_PI / nLayerHalfCnt;
			temp.x = lonLat.x;
			temp.y = MATH_RAD_TO_DEG(CHmsMercatorEarthTile::MercatorPosYToLatRadian(yStart));
			if (fabsf(temp.y) > 80)
			{
				continue;
			}
			temp = m_frame2DInterface->EarthPointToFramePoint(CHmsFrame2DInterface::LonLatToEarthPoint(temp));

			if (temp.y < center.y)
			{
				ret.y = y;
				break;
			}
		}
	}
	return ret;
}

void CHmsMercatorEarthTileMgr::Visit(HmsAviPf::Renderer * renderer, const HmsAviPf::Mat4 & parentTransform, uint32_t parentFlags)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!renderer)
		return;
#endif
    HmsAviPf::Mat4 mat4Transform;

	CHmsTextureFactory::GetInstance()->LoadTexture();

	if (m_pSkyboxMgr)
	{
        m_pSkyboxMgr->Visit(renderer, mat4Transform, parentFlags);
	}
	

	for (auto & c:m_rootTile)
	{
        c->Visit(renderer, mat4Transform, parentFlags);
	}

	if (m_nLayer > m_layer_bgGrade)
	{
        if (CHmsNavComm::GetInstance()->GetEnableTileLoad())
        {
            auto screenCenter = GetScreenCenterXY(m_nLayer);
            bool bMove = false;
            int deltax = 0;
            int deltay = 0;
            if (m_vLastCenter != screenCenter)
            {
                if (m_vLastCenter.x != 0)
                {
                    deltax = screenCenter.x - m_vLastCenter.x;
                    deltay = screenCenter.y - m_vLastCenter.y;
                }
                m_vLastCenter = screenCenter;

                bMove = true;
            }
            std::list< std::list<CHmsMercatorEarthTile*> > &list = m_listLayers;
            if (m_nLastLayer == m_nLayer)
            {
                if (deltax > 0)
                {
                    int len = deltax;
                    LeftColToRight(list, len);
                }
                else if (deltax < 0)
                {
                    int len = fabsf(deltax);
                    RightColToLeft(list, len);
                }

                if (deltay > 0)
                {
                    int len = deltay;
                    TopRowToBottom(list, len);
                }
                else if (deltay < 0)
                {
                    int len = fabsf(deltay);
                    BottomRowToTop(list, len);
                }
            }
            else
            {
                std::list< std::list<CHmsMercatorEarthTile*> > templist = m_listLayers;
                m_listLayers.clear();

                int typeCount = 0;
                while (!templist.empty())
                {
                    switch (typeCount % 4)
                    {
                    case 0:
                        GetListFrontRow(templist, m_listLayers);
                        break;
                    case 1:
                        GetListBackRow(templist, m_listLayers);
                        break;
                    case 2:
                        GetListLeftCol(templist, m_listLayers);
                        break;
                    case 3:
                        GetListRightCol(templist, m_listLayers);
                        break;
                    default:
                        break;
                    }
                    ++typeCount;
                }
            }

            int nLayerCnt = 1 << m_nLayer;
            int startx = screenCenter.x - m_layer_hCount / 2;
            int starty = screenCenter.y - m_layer_vCount / 2;
            if (starty < 0) starty = 0;
            if (starty + m_layer_vCount > nLayerCnt) starty = nLayerCnt - m_layer_vCount;

            int i = 0;
            for (auto colit = list.begin(); colit != list.end(); ++colit, ++i)
            {
                int j = 0;
                std::list<CHmsMercatorEarthTile*> &colList = *colit;
                for (auto rowit = colList.begin(); rowit != colList.end(); ++rowit, ++j)
                {
                    CHmsMercatorEarthTile *pTile = *rowit;
#if _NAV_SVS_LOGIC_JUDGE
					if (pTile)
#endif
					{
						if (bMove)
						{
							int x = startx + i;
							if (x < 0)
							{
								x += nLayerCnt;
							}
							int y = starty + j;

							pTile->SendLoadTextureRequest(m_nLayer, x, y, false);
						}
						//pTile->UpdateDrawState(m_frustum, nLayer);
						//pTile->CheckTexture();
						pTile->Visit(renderer, mat4Transform, parentFlags);
					}
                }
            }
        }
        else
        {
            std::list< std::list<CHmsMercatorEarthTile*> > &list = m_listLayers;
            for (auto colit = list.begin(); colit != list.end(); ++colit)
            {
                std::list<CHmsMercatorEarthTile*> &colList = *colit;
                for (auto rowit = colList.begin(); rowit != colList.end(); ++rowit)
                {
                    CHmsMercatorEarthTile *pTile = *rowit;
#if _NAV_SVS_LOGIC_JUDGE
					if (pTile)
#endif
                    pTile->Visit(renderer, mat4Transform, parentFlags);
                }
            }
        }
		
	}
	m_nLastLayer = m_nLayer;

	//画航图
#if _NAV_SVS_LOGIC_JUDGE
	if (m_pNavImageTileMgr && m_pNavImageTileMgr->m_pNavImageTile)
#endif
    m_pNavImageTileMgr->m_pNavImageTile->Visit(renderer, mat4Transform, parentFlags);

	//draw other child
	for (auto c:m_vOther3DChild)
	{
		c->Visit(renderer, mat4Transform, parentFlags);
	}
}

void CHmsMercatorEarthTileMgr::EarthClipping(HmsAviPf::Frustum * frustum)
{
	for (auto & c : m_rootTile)
	{
		//c->UpdateDrawState(frustum, m_nLayer);
		c->UpdateDrawState(frustum, m_layer_bgGrade);
	}
#if _NAV_SVS_LOGIC_JUDGE
	if (m_pNavImageTileMgr && m_pNavImageTileMgr->m_pNavImageTile)
#endif
	m_pNavImageTileMgr->m_pNavImageTile->UpdateDrawState(frustum, 5);
}

int CHmsMercatorEarthTileMgr::SetSeeAbleRadian(float fRadian)
{
	//int nTileNeed = floor(m_sizeScreen.width / 256);
	int nTileNeed = ceilf(m_sizeScreen.width / 256);
	auto arcLength = fRadian;
	auto tileLength = arcLength / nTileNeed;
	float fTileRadian = (float)SIM_Pi;
	for (int i = 1; i <= m_nMaxLayer; i++)
	{
		float fPct = tileLength / fTileRadian;
		m_nLayer = i;
		if (fPct > 0.125f)
		{
			break;
		}
		fTileRadian /= 2;
	}
	int a = 0;

	CHmsMapTileSql &mapTileSql = CHmsMapTileSql::GetInstance();
	m_nLayer = mapTileSql.DetectLayerEdge(m_nLayer);

	return m_nLayer;
}

void CHmsMercatorEarthTileMgr::ReloadEarthTile()
{
	CHmsMapTileSql &mapTileSql = CHmsMapTileSql::GetInstance();
	m_nLayer = mapTileSql.DetectLayerEdge(m_nLayer);


	auto screenCenter = GetScreenCenterXY(m_nLayer);
	int nLayerCnt = 1 << m_nLayer;
	int startx = screenCenter.x - m_layer_hCount / 2;
	int starty = screenCenter.y - m_layer_vCount / 2;
	if (starty < 0) starty = 0;
	if (starty + m_layer_vCount > nLayerCnt) starty = nLayerCnt - m_layer_vCount;

	int i = 0;
	for (auto colit = m_listLayers.begin(); colit != m_listLayers.end(); ++colit, ++i)
	{
		int j = 0;
		std::list<CHmsMercatorEarthTile*> &colList = *colit;
		for (auto rowit = colList.begin(); rowit != colList.end(); ++rowit, ++j)
		{
			CHmsMercatorEarthTile *pTile = *rowit;
			int x = startx + i;
			if (x < 0)
			{
				x += nLayerCnt;
			}
			int y = starty + j;

#if _NAV_SVS_LOGIC_JUDGE
			if (pTile)
#endif
			pTile->SendLoadTextureRequest(m_nLayer, x, y, true);
		}
	}
	m_vLastCenter = screenCenter;
	m_nLastLayer = m_nLayer;


// 	for (auto & list : m_listLayers)
// 	{
// 		for (auto & c:list)
// 		{
// 			c->ReloadTexture();
// 		}
// 	}

	for (auto c : m_rootTile)
	{
		c->ReloadTexture();
	}
}

void CHmsMercatorEarthTileMgr::AddOther3DLayer(HmsAviPf::CHmsNode * node)
{
	m_vOther3DChild.pushBack(node);
}

void CHmsMercatorEarthTileMgr::LeftColToRight(std::list< std::list<CHmsMercatorEarthTile*> > &list, const int &count)
{
	for (int i = 0; i < count; ++i)
	{
		std::list<CHmsMercatorEarthTile*> temp = list.front();
		list.push_back(temp);
		list.pop_front();
	}
}

void CHmsMercatorEarthTileMgr::RightColToLeft(std::list< std::list<CHmsMercatorEarthTile*> > &list, const int &count)
{
	for (int i = 0; i < count; ++i)
	{
		std::list<CHmsMercatorEarthTile*> temp = list.back();
		list.push_front(temp);
		list.pop_back();
	}
}

void CHmsMercatorEarthTileMgr::TopRowToBottom(std::list< std::list<CHmsMercatorEarthTile*> > &list, const int &count)
{
	for (auto it = list.begin(); it != list.end(); ++it)
	{
		std::list<CHmsMercatorEarthTile*> &temp = *it;
		for (int k = 0; k < count; ++k)
		{
			auto t = temp.front();
			temp.push_back(t);
			temp.pop_front();
		}
	}
}

void CHmsMercatorEarthTileMgr::BottomRowToTop(std::list< std::list<CHmsMercatorEarthTile*> > &list, const int &count)
{
	for (auto it = list.begin(); it != list.end(); ++it)
	{
		std::list<CHmsMercatorEarthTile*> &temp = *it;
		for (int k = 0; k < count; ++k)
		{
			auto t = temp.back();
			temp.push_front(t);
			temp.pop_back();
		}
	}
}

void CHmsMercatorEarthTileMgr::PushToList(CHmsMercatorEarthTile *pTile, std::list< std::list<CHmsMercatorEarthTile*> > &retList)
{
	if (retList.empty())
	{
		retList.resize(1);
	}
	if ((int)retList.back().size() >= m_layer_vCount)
	{
		retList.resize(retList.size() + 1);
	}
	retList.back().push_back(pTile);
}

void CHmsMercatorEarthTileMgr::GetListFrontRow(std::list< std::list<CHmsMercatorEarthTile*> > &list, 
	std::list< std::list<CHmsMercatorEarthTile*> > &retList)
{
	for (auto colit = list.begin(); colit != list.end(); ++colit)
	{
		std::list<CHmsMercatorEarthTile*> &colList = *colit;
		for (auto rowit = colList.begin(); rowit != colList.end(); ++rowit)
		{
			CHmsMercatorEarthTile *pTile = *rowit;
			PushToList(pTile, retList);
			colList.pop_front();
			break;
		}
	}
	for (auto colit = list.begin(); colit != list.end(); ++colit)
	{
		if (colit->empty())
		{
			list.clear();
			return;
		}
	}
}

void CHmsMercatorEarthTileMgr::GetListBackRow(std::list< std::list<CHmsMercatorEarthTile*> > &list, 
	std::list< std::list<CHmsMercatorEarthTile*> > &retList)
{
	for (auto colit = list.begin(); colit != list.end(); ++colit)
	{
		std::list<CHmsMercatorEarthTile*> &colList = *colit;
		for (auto rowit = colList.end(); rowit != colList.begin(); --rowit)
		{
			--rowit;
			CHmsMercatorEarthTile *pTile = *rowit;
			PushToList(pTile, retList);
			colList.pop_back();
			break;
		}
	}
	for (auto colit = list.begin(); colit != list.end(); ++colit)
	{
		if (colit->empty())
		{
			list.clear();
			return;
		}
	}
}

void CHmsMercatorEarthTileMgr::GetListLeftCol(std::list< std::list<CHmsMercatorEarthTile*> > &list, 
	std::list< std::list<CHmsMercatorEarthTile*> > &retList)
{
	for (auto colit = list.begin(); colit != list.end(); ++colit)
	{
		std::list<CHmsMercatorEarthTile*> &colList = *colit;
		for (auto rowit = colList.begin(); rowit != colList.end(); ++rowit)
		{
			CHmsMercatorEarthTile *pTile = *rowit;
			PushToList(pTile, retList);
		}
		list.pop_front();
		break;
	}
}

void CHmsMercatorEarthTileMgr::GetListRightCol(std::list< std::list<CHmsMercatorEarthTile*> > &list, 
	std::list< std::list<CHmsMercatorEarthTile*> > &retList)
{
	for (auto colit = list.end(); colit != list.begin(); --colit)
	{
		--colit;
		std::list<CHmsMercatorEarthTile*> &colList = *colit;
		for (auto rowit = colList.begin(); rowit != colList.end(); ++rowit)
		{
			CHmsMercatorEarthTile *pTile = *rowit;
			PushToList(pTile, retList);
		}
		list.pop_back();
		break;
	}
}
