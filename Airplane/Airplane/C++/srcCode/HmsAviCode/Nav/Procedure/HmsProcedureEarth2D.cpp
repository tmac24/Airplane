#include "HmsProcedureEarth2D.h"

CHmsProcedureEarth2D::CHmsProcedureEarth2D()
:CHmsFrame2DInterface(this)
{
}

CHmsProcedureEarth2D::~CHmsProcedureEarth2D()
{
}

CHmsProcedureEarth2D* CHmsProcedureEarth2D::Create(HmsAviPf::Camera *pCamera, Size layerSize)
{
	CHmsProcedureEarth2D *ret = new (std::nothrow) CHmsProcedureEarth2D();
	if (ret && ret->Init(pCamera, layerSize) && pCamera)
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

bool CHmsProcedureEarth2D::Init(HmsAviPf::Camera *pCamera, Size layerSize)
{
#if _NAV_SVS_LOGIC_JUDGE
	if(!pCamera)
		return false;
#endif
	if (!CHmsFrame2DInterface::Init(pCamera, layerSize))
	{
		return false;
	}

	m_pRouteLineDrawNode = CHmsRouteLineDrawNode::Create();
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pRouteLineDrawNode)
		return false;
#endif
	m_pRouteLineDrawNode->SetAnchorPoint(0, 0);
	m_pRouteLineDrawNode->SetContentSize(this->GetContentSize());
	m_pRouteLineDrawNode->SetPosition(0, 0);
	m_pRouteLineDrawNode->SetTexturePath("res/NavImage/navLine.png");
	this->AddChild(m_pRouteLineDrawNode);

	return true;
}

void CHmsProcedureEarth2D::OnClickOnEarth(const HmsAviPf::Vec2 & pos)
{

}

void CHmsProcedureEarth2D::Update(float delta)
{
	if (m_bNeedUpdate2DEles)
	{
		Update2DElements();
	}
}

void CHmsProcedureEarth2D::Update2DElements()
{
	if (0 == m_fNearPct || !m_pCamera)
	{
		return;
	}
	auto frust = m_pCamera->GetCameraFrustum(m_fNearPct);
#if _NAV_SVS_LOGIC_JUDGE
	if (!frust)
		return;
#endif
	for (auto &v : m_vecArray)
	{
		for (auto &m : v.mapRoute)
		{
			for (auto &e : m.second)
			{
				WptNodeStu2D &stu = e;
				if (stu.pNode)
				{
					auto visible = !frust->isOutOfFrustum(stu.earthPos);
					stu.pNode->SetVisible(visible);
					if (visible)
					{
						auto pos = EarthPointToFramePoint(stu.earthPos);
						stu.pNode->SetPosition(pos);
					}
				}

				for (int k = 0; k < (int)stu.vecInsertEarthPos.size(); ++k)
				{
					stu.vecInsertLonLatIsVisible[k] = !frust->isOutOfFrustum(stu.vecInsertEarthPos.at(k));
				}
			}
		}
	}

	CalInsertData2DPos();
	ReDrawLines();

	SetNeedUpdate2DEles(false);
}

void CHmsProcedureEarth2D::SetEarthLayer(int n)
{
	m_earthLayer = n;
}

CHmsMapMeasure* CHmsProcedureEarth2D::GetMapMeasure()
{
	return nullptr;
}

void CHmsProcedureEarth2D::SetRouteData(std::vector<PreviewRouteStu> &vec)
{
	ClearAllRouteNode();

	m_vecArray = vec;
	if (m_vecArray.empty())
	{
		return;
	}
	m_mapColor.clear();
	m_colorIndex = 0;

	if (m_vecArray.size() == 1)
	{
		for (auto &v : m_vecArray)
		{
			for (auto &m : v.mapRoute)
			{
				m_mapColor.insert(std::make_pair(m.first, GetColor(true)));
				for (auto &e : m.second)
				{
					Vec3 earthPos = LonLatToEarthPoint(Vec2(e.brief.lon, e.brief.lat));
					Vec2 pos = EarthPointToFramePoint(earthPos);

					auto pItem = CHmsImageNode::Create(CHmsWptInfoDlg::GetImagePathByWptType(e.brief.wType));
#if _NAV_SVS_LOGIC_JUDGE
					if (pItem)
#endif
					{
						pItem->SetScale(0.5);
						pItem->SetAnchorPoint(Vec2(0.5, 0.5));
						pItem->SetPosition(pos);
						this->AddChild(pItem);


						auto bgImage = CHmsStretchImageNode::Create("res/BasicFrame/Round4SolidWhite.png");
#if _NAV_SVS_LOGIC_JUDGE
						if (bgImage)
#endif
						{
							bgImage->SetAnchorPoint(Vec2(0, 0.5));
							bgImage->SetPosition(Vec2(30, 15));
							if (e.brief.ident.size() > 15)
							{
								bgImage->SetContentSize(Size(e.brief.ident.size() * 14, 40));
							}
							else
							{
								bgImage->SetContentSize(Size(e.brief.ident.size() * 18, 40));
							}
							bgImage->SetColor(Color3B(m_mapColor.at(m.first)));
							bgImage->SetOpacity(125);
							pItem->AddChild(bgImage);
						}

						auto titleLabel = CHmsGlobal::CreateLabel(e.brief.ident.c_str(), 12);
#if _NAV_SVS_LOGIC_JUDGE
						if (titleLabel)
#endif
						{
							titleLabel->SetScale(2);
							titleLabel->SetAnchorPoint(0, 0.5);
							titleLabel->SetPosition(Vec2(30, 15));
							titleLabel->SetTextColor(Color4B::BLACK);
							pItem->AddChild(titleLabel);
						}

						e.earthPos = earthPos;
						e.pNode = pItem;
					}
				}
			}
		}
	}
	else
	{
		for (auto &v : m_vecArray)
		{
			m_mapColor.insert(std::make_pair(v.programName, GetColor(true)));
			for (auto &m : v.mapRoute)
			{
				for (auto &e : m.second)
				{
					Vec3 earthPos = LonLatToEarthPoint(Vec2(e.brief.lon, e.brief.lat));
					Vec2 pos = EarthPointToFramePoint(earthPos);

					auto pItem = CHmsImageNode::Create(CHmsWptInfoDlg::GetImagePathByWptType(e.brief.wType));
#if _NAV_SVS_LOGIC_JUDGE
					if (pItem)
#endif
					{
						pItem->SetScale(0.5);
						pItem->SetAnchorPoint(Vec2(0.5, 0.5));
						pItem->SetPosition(pos);
						this->AddChild(pItem);

						e.earthPos = earthPos;
						e.pNode = pItem;
					}
				}
			}
		}
		//多个程序的时候，添加程序标题
		for (auto &v : m_vecArray)
		{
			for (auto &m : v.mapRoute)
			{
				if (m.second.empty())
				{
					continue;
				}
				//取每个程序的第一条航路

				auto bgImage = CHmsStretchImageNode::Create("res/BasicFrame/Round4SolidWhite.png");
#if _NAV_SVS_LOGIC_JUDGE
				if (bgImage)
#endif
				{
					bgImage->SetAnchorPoint(Vec2(0, 0.5));
					bgImage->SetPosition(Vec2(30, 15));
					bgImage->SetContentSize(Size(150, 40));
					bgImage->SetOpacity(200);
					bgImage->SetColor(Color3B(m_mapColor.at(v.programName)));

					auto titleLabel = CHmsGlobal::CreateLabel(v.programName.c_str(), 16);
#if _NAV_SVS_LOGIC_JUDGE
					if (titleLabel)
#endif
					{
						titleLabel->SetScale(2);
						titleLabel->SetAnchorPoint(0, 0.5);
						titleLabel->SetPosition(Vec2(30, 15));
						titleLabel->SetTextColor(Color4B::BLACK);
						if (v.proType == ROUTE_TYPE_DEP_SID)
						{
							m.second[m.second.size() - 1].pNode->AddChild(bgImage);
							m.second[m.second.size() - 1].pNode->AddChild(titleLabel);
						}
						else
						{
							m.second[0].pNode->AddChild(bgImage);
							m.second[0].pNode->AddChild(titleLabel);
						}
					}
				}

				break;
			}
		}
	}

	CalInsertLonLat();
	SetNeedUpdate2DEles(true);
}

void CHmsProcedureEarth2D::CalInsertLonLat()
{
	HmsAviPf::Frustum *pFrust = nullptr;
	if (m_fNearPct != 0)
	{
		pFrust = m_pCamera->GetCameraFrustum(m_fNearPct);
	}

	auto ProcessInsert = [=](WptNodeStu2D *pLastStu, WptNodeStu2D *pCurStu)
	{
		if (pLastStu == nullptr || pCurStu == nullptr)
		{
			return;
		}
		pCurStu->vecInsertLonLatIsVisible.clear();
		pCurStu->vecInsertEarthPos.clear();
		pCurStu->vecInsert2DPos.clear();
		
		float dis = 2;
		std::vector<Vec2> vecLonLat;
		InsertLonLat(Vec2(pLastStu->brief.lon, pLastStu->brief.lat), Vec2(pCurStu->brief.lon, pCurStu->brief.lat), vecLonLat, dis);
        for (int i = 0; i < (int)vecLonLat.size(); ++i)
		{
			auto earthPos = LonLatToEarthPoint(vecLonLat[i]);
			if (pFrust)
			{
				pCurStu->vecInsertLonLatIsVisible.push_back(!pFrust->isOutOfFrustum(earthPos));
			}
			else
			{
				pCurStu->vecInsertLonLatIsVisible.push_back(true);
			}
			pCurStu->vecInsertEarthPos.push_back(earthPos);
			pCurStu->vecInsert2DPos.push_back(EarthPointToFramePoint(earthPos));
		}
	};

	WptNodeStu2D *pLastStu = nullptr;
	WptNodeStu2D *pCurStu = nullptr;

	for (auto &v : m_vecArray)
	{
		for (auto &m : v.mapRoute)
		{
			auto &vec = m.second;
			if (vec.size() < 2)
			{
				continue;
			}
			pLastStu = &vec[0];
			for (int i = 1; i < (int)vec.size(); ++i)
			{
				pCurStu = &vec[i];

				ProcessInsert(pLastStu, pCurStu);

				pLastStu = pCurStu;
				pCurStu = nullptr;
			}
		}
	}
}

void CHmsProcedureEarth2D::CalInsertData2DPos()
{
	for (auto &v : m_vecArray)
	{
		for (auto &m : v.mapRoute)
		{
			for (auto &e : m.second)
			{
				for (int k = 0; k < (int)e.vecInsertEarthPos.size(); ++k)
				{
					e.vecInsert2DPos[k] = EarthPointToFramePoint(e.vecInsertEarthPos[k]);
				}
			}
		}
	}
}

void CHmsProcedureEarth2D::ClearAllRouteNode()
{
	for (auto &v : m_vecArray)
	{
		for (auto &m : v.mapRoute)
		{
			for (auto &e : m.second)
			{
				if (e.pNode)
				{
					e.pNode->RemoveFromParent();
				}
			}
		}
	}
	m_vecArray.clear();

	SetNeedUpdate2DEles(true);
}

void CHmsProcedureEarth2D::ReDrawLines()
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pRouteLineDrawNode)
		return;
#endif
	m_pRouteLineDrawNode->clear();
	
	std::vector<Vec2> vecLines;
	for (auto &v : m_vecArray)
	{
		for (auto &m : v.mapRoute)
		{
			if (m.second.size() < 2)
			{
				continue;
			}
			for (auto &e : m.second)
			{
				vecLines.clear();

				WptNodeStu2D &wptNode = e;
#if _NAV_SVS_LOGIC_JUDGE
				if (wptNode.pNode)
#endif
				wptNode.pNode->SetColor(Color3B::WHITE);
				
				const std::vector<Vec2> &vec2DPos = wptNode.vecInsert2DPos;
				const std::vector<bool> &vecVisible = wptNode.vecInsertLonLatIsVisible;

                for (int k = 0; k < (int)vec2DPos.size(); ++k)
				{
					if (vecVisible[k])
					{
						vecLines.push_back(vec2DPos[k]);
					}
				}
				if (m_vecArray.size() == 1)
				{
					m_pRouteLineDrawNode->DrawLines(vecLines.data(), vecLines.size(), 8, m_mapColor.at(m.first));
				}
				else
				{
					m_pRouteLineDrawNode->DrawLines(vecLines.data(), vecLines.size(), 8, m_mapColor.at(v.programName));
				}
			}
		}
	}
}

HmsAviPf::Color4B CHmsProcedureEarth2D::GetColor(bool bAutoInc)
{
	Color4B ret = Color4B::GREEN;
	switch (m_colorIndex)
	{
	case 0:  ret = Color4B(0xff, 0x7e, 0x74, 0xff); break;
	case 1:  ret = Color4B(0xf8, 0xbc, 0x6c, 0xff); break;
	case 2:  ret = Color4B(0x73, 0xb5, 0xe4, 0xff); break;
	case 3:  ret = Color4B(0xff, 0xf8, 0x68, 0xff); break;
	case 4:  ret = Color4B(0xd2, 0xf0, 0xc8, 0xff); break;
	case 5:  ret = Color4B(0xfd, 0xce, 0xf2, 0xff); break;
	case 6:  ret = Color4B(0xc6, 0xc4, 0xdf, 0xff); break;
	case 7:  ret = Color4B(0xfb, 0xb7, 0x69, 0xff); break;
	case 8:  ret = Color4B(0xbc, 0xe1, 0x71, 0xff); break;
	case 9:  ret = Color4B(0xc1, 0x85, 0xcd, 0xff); break;
	case 10:  ret = Color4B(0x92, 0xda, 0xca, 0xff); break;
	default:
		break;
	}
	if (bAutoInc)
	{
		++m_colorIndex;
	}
	if (m_colorIndex > 10)
	{
		m_colorIndex = 0;
	}
	return ret;
}


