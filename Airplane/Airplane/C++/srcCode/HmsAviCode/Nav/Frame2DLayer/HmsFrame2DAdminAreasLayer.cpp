#include "HmsFrame2DAdminAreasLayer.h"
#include "../HmsFrame2DRoot.h"
#include "Calculate/CalculateZs.h"
#include "HmsGlobal.h"
#include "HmsLog.h"

CHmsFrame2DAdminAreasLayer::CHmsFrame2DAdminAreasLayer()
: CHmsFrame2DLayerBase(this, NavFrame2DLayerType::NavFrame2D_Admin)
, m_bDraw3D(true)
, m_nLayerLimit(5)
{
    m_updateTime = 0;
	m_dataCell.eState = ADMIN_AREAS_DATA_STATE::ActionNone;
	m_dataCell.nLayer = 0;
	m_fReloadDataScale = 0.4f;
}

CHmsFrame2DAdminAreasLayer::~CHmsFrame2DAdminAreasLayer()
{
}

CHmsFrame2DAdminAreasLayer* CHmsFrame2DAdminAreasLayer::Create(Size layerSize)
{
    CHmsFrame2DAdminAreasLayer *ret = new (std::nothrow) CHmsFrame2DAdminAreasLayer();
    if (ret && ret->Init(layerSize))
    {
        ret->autorelease();
        return ret;
    }
    HMS_SAFE_DELETE(ret);
    return nullptr;
}

bool CHmsFrame2DAdminAreasLayer::Init(Size layerSize)
{
    if (!CHmsNode::Init())
    {
        return false;
    }
    this->SetContentSize(layerSize);

    m_pAdminAreaLineDrawNode = CHmsMultLineDrawNode::Create();

    if (m_pAdminAreaLineDrawNode)
    {
        m_pAdminAreaLineDrawNode->SetAnchorPoint(0, 0);
        m_pAdminAreaLineDrawNode->SetContentSize(this->GetContentSize());
        m_pAdminAreaLineDrawNode->SetPosition(0, 0);
        m_pAdminAreaLineDrawNode->SetDrawColor(Color4F::WHITE);
        this->AddChild(m_pAdminAreaLineDrawNode);
    }

	m_pRootLabelNode = CHmsNode::Create();
    if (m_pRootLabelNode)
    {
        m_pRootLabelNode->SetAnchorPoint(0, 0);
        m_pRootLabelNode->SetPosition(0, 0);
        this->AddChild(m_pRootLabelNode);
    }


	m_pAdminAreaLine3DDrawNode = CHms3DLineDrawNode::Create(2000000);
    if (m_pAdminAreaLine3DDrawNode)
    {
        m_pAdminAreaLine3DDrawNode->SetColor(Color4F::WHITE);
        m_pAdminAreaLine3DDrawNode->SetLineWidth(4.0f);
    }
    m_pAdminChinaAdmin3DDrawNode = CHms3DLineDrawNode::Create(100000);
    if (m_pAdminChinaAdmin3DDrawNode)
    {
        m_pAdminChinaAdmin3DDrawNode->SetColor(Color4F::GREEN);
        m_pAdminChinaAdmin3DDrawNode->SetLineWidth(5.0f);

        
        //m_pAdminChinaAdmin3DDrawNode->DrawLines();
    }

	auto navEarthFrame = CHmsGlobal::GetInstance()->GetNavEarthFramePointer();
    if (navEarthFrame)
    {
        navEarthFrame->AttachToEarthDrawNode(m_pAdminAreaLine3DDrawNode);
        navEarthFrame->AttachToEarthDrawNode(m_pAdminChinaAdmin3DDrawNode);
    }

    UpdateChinaAdmin3D();

    return true;
}

void CHmsFrame2DAdminAreasLayer::UpdateChinaAdmin3D()
{
    std::vector<std::vector<HmsAviPf::Vec3>> vecArray;

    auto FillShapeData = [&](const std::vector<GeomPosition> & vPosition, int nBegin, int nEnd)
    {
        nEnd = HMS_MIN(nEnd, (int)vPosition.size());
        int nTotal = nEnd - nBegin;
        int nIncrease = 1;
    
        Vec2 tempPos;
        std::vector<HmsAviPf::Vec3> vecEarthPos;

        for (int i = nBegin; i < nEnd; i += nIncrease)
        {
            auto & pos = vPosition.at(i);
            tempPos.setPoint(pos.dX, pos.dY);
   
            Vec3 posEarth = CHmsNavMathHelper::LonLatToEarthPoint(tempPos);
            vecEarthPos.push_back(posEarth);
        };

        if (vecEarthPos.size() > 2)
        {
            vecArray.push_back(vecEarthPos);
        }
    };

    auto data = CHmsAdminAreasServer::GetInstance()->GetChinaAmdin();
    for ( auto & c: data)
    {
        // the area
        auto it = c.vIndex.begin();
        auto itNext = it;

        while (it != c.vIndex.end())
        {
            itNext = it + 1;

            if (itNext != c.vIndex.end())
            {
                FillShapeData(c.vPosition, *it, *itNext);
            }
            else
            {
                FillShapeData(c.vPosition, *it, c.vPosition.size());
            }
            it = itNext;
        }
    }

    for (auto & c: vecArray)
    {
        m_pAdminChinaAdmin3DDrawNode->DrawLineLoop(c);
    }
}

void CHmsFrame2DAdminAreasLayer::Update(float delta)
{
	if (m_earthLayer <= m_nLayerLimit)
    {
        return;
    }
    m_updateTime += delta;

	if (m_dataCell.eState == ADMIN_AREAS_DATA_STATE::ActionUsed)
	{
		//HMSLOG("HmsLog: Admin Areas Action Used Begin");
#if _NAV_SVS_LOGIC_JUDGE
		if (!m_pRootLabelNode)
			return;
#endif
		m_pRootLabelNode->SetVisible(true);
		FillAdminAreaGroupData();
		//HMSLOG("HmsLog: Admin Areas Data Filled End");
		if (m_bDraw3D)
		{
			Update3DElements();
			//HMSLOG("HmsLog: Admin Areas Data 3D Update End");
		}
		else
		{
			Update2DElements();
			//HMSLOG("HmsLog: Admin Areas Data 2D Update End");
		}
		CleanDataCell();
		return;
	}

	if (m_dataCell.eState != ADMIN_AREAS_DATA_STATE::ActionNone)
	{
		return;
	}

    if (m_updateTime > 2)
    {
        m_updateTime = 0;

		int nTempLayer = 0;

		if (m_earthLayer <=6)
		{
			nTempLayer = 0;
		}
		else if (m_earthLayer <= 7)
		{
			nTempLayer = 1;
		}
		else if (m_earthLayer <= 8)
		{
			nTempLayer = 2;
		}
		else
		{
			nTempLayer = 3;
		}

		//wait the async action callback 

        Vec3 pos, posRef;
#if _NAV_SVS_LOGIC_JUDGE
		if (!m_pFrame2DRoot)
			return;
#endif
        if (m_pFrame2DRoot->Frame2DPointToEarthPoint(this->GetContentSize() * 0.5, pos))
        {
            Vec2 lonLatCenter = CHmsFrame2DInterface::EarthPointToLonLat(pos);

			//the center move
			auto lonCenterOffset = fabsf(lonLatCenter.x - m_lastLonLat.x);
			auto latCenterOffset = fabsf(lonLatCenter.y - m_lastLonLat.y);

			auto lonOffset = 5;
			auto latOffset = 5;

			//use the 1/5 position of frame, to calculate the half screen lonlat offset 
			if (m_pFrame2DRoot->Frame2DPointToEarthPoint(this->GetContentSize() * 0.5f * 0.8f, posRef))
			{
				Vec2 lonLatRef = CHmsFrame2DInterface::EarthPointToLonLat(posRef);
				//area change
				float fScale = 5 * (1 + m_fReloadDataScale);
				lonOffset = fabsf(lonLatCenter.x - lonLatRef.x) * fScale;
				latOffset = fabsf(lonLatCenter.y - lonLatRef.y) * fScale;
			}

			if ((lonCenterOffset > m_lastOffset.x) || (latCenterOffset > m_lastOffset.y) || (nTempLayer != m_dataCell.nLayer))
            {
				m_dataCell.nLayer = nTempLayer;

				HmsGeomBoundary boundary;
				//Fix the boundary	
				boundary.dLeft = lonLatCenter.x - lonOffset;
				boundary.dRight = lonLatCenter.x + lonOffset;
				boundary.dTop = lonLatCenter.y + latOffset;
				boundary.dBottom = lonLatCenter.y - latOffset;
				
				CHmsAdminAreasServer::GetInstance()->GetDataByAsyn(boundary, &m_dataCell, HMS_CALLBACK_1(CHmsFrame2DAdminAreasLayer::OnAmdinAreasDataLoaded, this));

                m_lastLonLat = lonLatCenter;
				m_lastOffset.setPoint(lonOffset, latOffset);
				m_lastOffset *= m_fReloadDataScale;

				HMSLOG("Load Admin Areas %d with layer=%d", m_dataCell.nLayer, m_earthLayer);
            }   
        }
    }
}

void CHmsFrame2DAdminAreasLayer::Update2DElements()
{
	CHmsTime time;
	time.RecordCurrentTime();

	if (m_bDraw3D)
	{
		Update3DLabel();
		return;
	}

#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pRootLabelNode || !m_pAdminAreaLineDrawNode || !m_pFrame2DRoot)
		return;
#endif

	if (m_earthLayer <= m_nLayerLimit)
    {
        m_pAdminAreaLineDrawNode->clear();
		m_pRootLabelNode->SetVisible(false);
        return;
    }

	m_pRootLabelNode->SetVisible(true);

    HmsAviPf::Frustum *pFrust = m_pFrame2DRoot->GetFrustum();
    for (auto &group : m_vecAdminAreaGroup)
    {
		bool bVisible = false;
		if (pFrust)
		{
			bVisible = !pFrust->isOutOfFrustum(group.posEarthLabel);
		}
		group.isLableVisable = bVisible;
		group.pos2DLabel = m_pFrame2DRoot->EarthPointToFramePoint(group.posEarthLabel);

        for (auto & shape : group.vecEarthShape)
        {
            for (int k = 0; k < (int)shape.vecPos2D.size(); ++k)
            {
                bool bVisible = false;
                if (pFrust)
                {
					bVisible = !pFrust->isOutOfFrustum(shape.vecPosEarth.at(k));
                }
				shape.vecIsVisible[k] = bVisible;
				shape.vecPos2D[k] = m_pFrame2DRoot->EarthPointToFramePoint(shape.vecPosEarth[k]);
            }
        }
    }

	printf("Amdini Areas calc Time:%f\n", time.GetElapsed());

    ReDrawLines();

	printf("Amdini Areas Time:%f\n", time.GetElapsed());
}

void CHmsFrame2DAdminAreasLayer::SetEarthLayer(int n)
{
    if (m_earthLayer != n)
    {
        m_earthLayer = n;

        SetNeedUpdate2DEles(true);
    }
}

void CHmsFrame2DAdminAreasLayer::Update3DLabel()
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pRootLabelNode || !m_pAdminAreaLineDrawNode || !m_pAdminAreaLine3DDrawNode || !m_pFrame2DRoot)
		return;
#endif

	if (m_earthLayer <= m_nLayerLimit)
	{
		m_pAdminAreaLineDrawNode->clear();
		m_pRootLabelNode->SetVisible(false);
		m_pAdminAreaLine3DDrawNode->SetVisible(false);
		return;
	}

	m_pRootLabelNode->SetVisible(true);
	m_pAdminAreaLine3DDrawNode->SetVisible(true);

	HmsAviPf::Frustum *pFrust = m_pFrame2DRoot->GetFrustum();
	for (auto &group : m_vecAdminAreaGroup)
	{
		bool bVisible = false;
		if (pFrust)
		{
			bVisible = !pFrust->isOutOfFrustum(group.posEarthLabel);
		}
		group.isLableVisable = bVisible;
		group.pos2DLabel = m_pFrame2DRoot->EarthPointToFramePoint(group.posEarthLabel);

		if (group.pAreaLabel)
		{
			group.pAreaLabel->SetVisible(group.isLableVisable);
			group.pAreaLabel->SetPosition(group.pos2DLabel);
		}
	}
}

void CHmsFrame2DAdminAreasLayer::Update3DElements()
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pAdminAreaLine3DDrawNode)
		return;
#endif

	m_pAdminAreaLine3DDrawNode->clear();
	//HMSLOG("Admin Areas Clear");
	for (auto &group : m_vecAdminAreaGroup)
	{
		//update the label position
		if (group.pAreaLabel)
		{
			group.pAreaLabel->SetVisible(group.isLableVisable);
			group.pAreaLabel->SetPosition(group.pos2DLabel);
		}

		for (auto &airspaceStu : group.vecEarthShape)
		{
			//m_pAdminAreaLine3DDrawNode->DrawLines(airspaceStu.vecPosEarth);
			//m_pAdminAreaLine3DDrawNode->PrintUsedSize();
			//HMSLOG("posCnt=%d", airspaceStu.vecPosEarth.size());
			m_pAdminAreaLine3DDrawNode->DrawLineLoop(airspaceStu.vecPosEarth);
		}
		
	}
	m_pAdminAreaLine3DDrawNode->PrintUsedSize();
	Update3DLabel();
	//HMSLOG("HmsLog: Amdin Areas 3D Label End");
}

void CHmsFrame2DAdminAreasLayer::PushPointToStu(AdminAreaDrawStu &stu, const std::vector<Vec2> &vec2DPos, const std::vector<bool> &vecVisible)
{
	std::vector<Vec2> vecLines;
	for (std::vector<Vec2>::size_type k = 0; k < vec2DPos.size(); ++k)
	{
		if (vecVisible[k])
		{
			vecLines.push_back(vec2DPos[k]);
		}
		else
		{
			if (vecLines.size() > 0)
			{
				stu.vecArrayVertexs.push_back(vecLines);
				vecLines.clear();
			}
		}
	}
	if (vecLines.size() > 0)
	{
		stu.vecArrayVertexs.push_back(vecLines);
		vecLines.clear();
	}
}

void CHmsFrame2DAdminAreasLayer::ReDrawLines()
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pAdminAreaLineDrawNode)
		return;
#endif
    m_pAdminAreaLineDrawNode->clear();
	//std::vector<AdminAreaDrawStu> vecStu;

    //==============amdin area
    for (auto &group : m_vecAdminAreaGroup)
    {
		//update the label position
		if (group.pAreaLabel)
		{
			group.pAreaLabel->SetVisible(group.isLableVisable);
			group.pAreaLabel->SetPosition(group.pos2DLabel);
		}

        for (auto &airspaceStu : group.vecEarthShape)
        {
			AdminAreaDrawStu stu;
            stu.fLineWidth = 2;
            stu.color = Color4B::WHITE;

            const std::vector<Vec2> &vec2DPos = airspaceStu.vecPos2D;
            const std::vector<bool> &vecVisible = airspaceStu.vecIsVisible;

            PushPointToStu(stu, vec2DPos, vecVisible);

            //vecStu.push_back(stu);

            for (auto &c : stu.vecArrayVertexs)
            {
                m_pAdminAreaLineDrawNode->DrawLineStrip(c, stu.fLineWidth, stu.color);
            }
        }
    }
}

void CHmsFrame2DAdminAreasLayer::CleanDataCell()
{
	m_dataCell.eState = ADMIN_AREAS_DATA_STATE::ActionNone;

	for (auto c:m_dataCell.vGeomData)
	{
		if (c)
		{
			delete c;
		}
		c = nullptr;
	}
	m_dataCell.vGeomData.clear();
}

void CHmsFrame2DAdminAreasLayer::ShowAdminAreasLayer()
{
    this->SetVisible(true);
}

void CHmsFrame2DAdminAreasLayer::HideAdminAreasLayer()
{
    this->SetVisible(false);
}

void CHmsFrame2DAdminAreasLayer::SetVisible(bool bVisible)
{
	CHmsFrame2DLayerBase::SetVisible(bVisible);

	if (m_pAdminAreaLine3DDrawNode)
	{
		m_pAdminAreaLine3DDrawNode->SetVisible(bVisible);
	}
}

void CHmsFrame2DAdminAreasLayer::SetColor(const Color3B& color)
{
	if (m_pAdminAreaLine3DDrawNode)
	{
		m_pAdminAreaLine3DDrawNode->SetColor(color);
	}
}

int GetSkipPosByReduceSize(int nSize)
{
#define INCREASE_BASE  50
	static int s_nIncreasePosNum[] = { 
		INCREASE_BASE, INCREASE_BASE << 2, INCREASE_BASE << 4, 
		INCREASE_BASE << 6, INCREASE_BASE << 8, INCREASE_BASE << 10, 
		INCREASE_BASE << 12, INCREASE_BASE << 14, INCREASE_BASE << 16 
	};

	int nIncrease = 1;

	for (auto c : s_nIncreasePosNum)
	{
		if (c > nSize)
			break;
		nIncrease++;
	}
	return nIncrease;
}

void CHmsFrame2DAdminAreasLayer::OnAmdinAreasDataLoaded(HmsAdminAreasServerDataCell * dataCell)
{
	if (nullptr == dataCell)
	{
		return;
	}

	auto FillShapeData = [&](std::vector<AdminAreaShape> & vecEarthShape, const std::vector<GeomPosition> & vPosition, int nBegin, int nEnd)
	{

		AdminAreaShape  shape;
		nEnd = HMS_MIN(nEnd, (int)vPosition.size());
		int nTotal = nEnd - nBegin;
		//int nIncrease = GetSkipPosByReduceSize(nTotal);
        int nIncrease = 1;

		Vec2 tempPos;

		for (int i = nBegin; i < nEnd; i += nIncrease)
		{
			shape.vecIsVisible.push_back(false);
			auto & pos = vPosition.at(i);
			tempPos.setPoint(pos.dX, pos.dY);
			shape.vecPos2D.push_back(tempPos);	//the pos 2d just temp used for calculate geom center position
			//shape.vecPos2D.push_back(Vec2());
			Vec3 posEarth = CHmsNavMathHelper::LonLatToEarthPoint(tempPos);
			shape.vecPosEarth.push_back(posEarth);
		};

		if (shape.vecPosEarth.size() > 2)
		{
			vecEarthShape.push_back(shape);
		}
	};


	m_vecAdminAreaGroupTemp.clear();
	for (auto & c:dataCell->vGeomData)
	{
		AdminAreaGroupStu stu;
		
		// the label
#if 0
		auto dX = (c->header.dMinX + c->header.dMaxX) / 2;
		auto dY = (c->header.dMinY + c->header.dMaxY) / 2;
		Vec2 posCenter(dX, dY);
#else
        Vec2 posCenter(c->centralPos.dX, c->centralPos.dY);
#endif
		stu.strNameLabel = c->strName;
		stu.isLableVisable = false;

		// the area
		auto it = c->vIndex.begin();
		auto itNext = it;

		while (it != c->vIndex.end())
		{
			itNext = it + 1;

			if (itNext != c->vIndex.end())
			{
				FillShapeData(stu.vecEarthShape, c->vPosition, *it, *itNext);
			}
			else
			{
				FillShapeData(stu.vecEarthShape, c->vPosition, *it, c->vPosition.size());
			}
			it = itNext;
		}

		//stu.EstimateGeoeCenter(posCenter);
		stu.posEarthLabel = CHmsNavMathHelper::LonLatToEarthPoint(posCenter);

		m_vecAdminAreaGroupTemp.push_back(stu);
	}

	dataCell->eState = ADMIN_AREAS_DATA_STATE::ActionUsed;
}

void CHmsFrame2DAdminAreasLayer::FillAdminAreaGroupData()
{
	if (!m_pRootLabelNode)
		return;

	for (auto &c : m_vecAdminAreaGroup)
	{
		if (c.pAreaLabel)
		{
			c.pAreaLabel->RemoveFromParent();
		}
		c.vecEarthShape.clear();
	}
	m_vecAdminAreaGroup.clear();
	m_pRootLabelNode->RemoveAllChildren();

	m_vecAdminAreaGroup.swap(m_vecAdminAreaGroupTemp);

	CHmsLabel * label = nullptr;
	for (auto &c : m_vecAdminAreaGroup)
	{
		if (c.pAreaLabel)
		{
			c.pAreaLabel->RemoveFromParent();
            c.pAreaLabel = nullptr;
		}

		label = CHmsGlobal::CreateLabel(c.strNameLabel.c_str(), 23, Color4F::WHITE);
		if (label)
		{
			c.pAreaLabel = label;
			auto sizeLabel = label->GetContentSize();
			auto pBgImage = CHmsStretchImageNode::Create("res/BasicFrame/Round4SolidWhite.png");
			if (pBgImage)
			{
				pBgImage->SetAnchorPoint(Vec2(0, 0));
				pBgImage->SetPosition(-10, 0);
				pBgImage->SetContentSize(sizeLabel + Size(20, 0));
				pBgImage->SetColor(Color3B(0x00, 0x00, 0x00));
				pBgImage->SetOpacity(0x77);
				pBgImage->SetLocalZOrder(-1);
				c.pAreaLabel->AddChild(pBgImage);
			}
			m_pRootLabelNode->AddChild(c.pAreaLabel);
		}
	}
	m_pRootLabelNode->GetParent()->ForceUpdate();
}

struct GeoeShapeCenterTempInfo
{
	int		nCnt;
	Vec2	pos;
#if _NAV_SVS_LOGIC_JUDGE
	GeoeShapeCenterTempInfo()
	{
		nCnt = 0;
	}
#endif
};

bool AdminAreaGroupStu::EstimateGeoeCenter(HmsAviPf::Vec2 & posCenter)
{
	if (vecEarthShape.size() < 0)
	{
		return false;
	}

	std::vector<GeoeShapeCenterTempInfo> vTempInfo;
	vTempInfo.reserve(vecEarthShape.size());

	auto fillData = [&vTempInfo](std::vector<HmsAviPf::Vec2> & vPos)
	{
		const int c_nCalcPos = 8;
		static float s_fWeight = 1.0f / c_nCalcPos;

		GeoeShapeCenterTempInfo info;
		info.nCnt = vPos.size();
		
		if (info.nCnt > 0)
		{
			int nMoveInc = info.nCnt / c_nCalcPos;
			Vec2 tempPos;
			for (int i = 0; i < c_nCalcPos; i++)
			{
				tempPos += vPos.at(i*nMoveInc);
			}
			info.pos = tempPos * s_fWeight;
			vTempInfo.push_back(info);
		}
	};

	bool bRet = false;
	int nCntTotal = 0;

	for (auto & c:vecEarthShape)
	{
		if (c.vecPos2D.size() >= 50)
		{
			nCntTotal += c.vecPos2D.size();
			fillData(c.vecPos2D);
		}
	}

	if (nCntTotal > 0)
	{
		Vec2 tempPos;
		for (auto c:vTempInfo)
		{
			tempPos = c.pos * (c.nCnt / (float)nCntTotal);
		}
		posCenter = tempPos;
		bRet = true;
	}

	return bRet;
}
