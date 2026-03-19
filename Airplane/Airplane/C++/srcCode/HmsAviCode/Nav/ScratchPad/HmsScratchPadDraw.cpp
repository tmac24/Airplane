#include "HmsScratchPadDraw.h"


CHmsPadDrawNode::CHmsPadDrawNode()
{

}

CHmsPadDrawNode::~CHmsPadDrawNode()
{

}

void CHmsPadDrawNode::DrawLines(Vec2 * vertexs, int nCnt, float fLineWidth, const Color4B & color /*= Color4B::WHITE*/)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (nCnt < 2 || !vertexs)
#else
	if (nCnt < 2)
#endif	
	{
		return;
	}
	unsigned int nVertexCount = 2 * nCnt;
	ensureCapacity(nVertexCount);
	V2F_C4B_T2F * pPoss = m_pBuffer + m_bufferCount;
	float fHalfWidth = fLineWidth * 0.5f;
	{
		V2F_C4B_T2F * pCurPos = pPoss;
		//0
		{
			Vec2 v0 = vertexs[0];
			Vec2 v1 = vertexs[1];
			Vec2 n1 = (v1 - v0).getPerp().getNormalized();
			*pCurPos++ = { v0 - n1 * fHalfWidth, color, Tex2F(0.0, 0.0) };
			*pCurPos++ = { v0 + n1 * fHalfWidth, color, Tex2F(0.0, 0.0) };
		}
		//>0
		{
			for (int i = 1; i < nCnt; i++)
			{
				Vec2 v0 = vertexs[i - 1];
				Vec2 v1 = vertexs[i];
				Vec2 n1 = (v1 - v0).getPerp().getNormalized();
				*pCurPos++ = { v1 - n1 * fHalfWidth, color, Tex2F(0.0, 0.0) };
				*pCurPos++ = { v1 + n1 * fHalfWidth, color, Tex2F(0.0, 0.0) };
			}
		}
	}
	HmsDrawCtrl fd;
	fd.SetTriangleStrip(m_bufferCount, nVertexCount);
	m_vFanDraw.push_back(HmsDrawCtrl(fd));
	m_bufferCount += nVertexCount;
}


CHmsScratchPadDraw::CHmsScratchPadDraw()
:m_bDealEventAsButton(false)
, m_callbackFunc(nullptr)
, m_bNeedSave(false)
, m_bEnableDraw(false)
#if _NAV_SVS_LOGIC_JUDGE
, m_pImageNode(nullptr)
, m_pDrawNode(nullptr)
#endif
{

}

CHmsScratchPadDraw::~CHmsScratchPadDraw()
{

}

bool CHmsScratchPadDraw::Init()
{
	if (!CHmsUiPanel::Init())
	{
		return false;
	}
	
	m_infoStu.bgType = ScratchPadBgType::TYPE_PIREP;
	m_pImageNode = CHmsImageNode::Create("res/NavImage/pirep.png");
#if _NAV_SVS_LOGIC_JUDGE	
	Size size;
	if (m_pImageNode)
#endif
	{
		m_pImageNode->SetAnchorPoint(Vec2(0, 0));
		this->AddChild(m_pImageNode);
#if _NAV_SVS_LOGIC_JUDGE	
		size = m_pImageNode->GetContentSize();
#else
		auto size = m_pImageNode->GetContentSize();
#endif
		this->SetContentSize(size);
	}

	m_pDrawNode = CHmsPadDrawNode::Create();
#if _NAV_SVS_LOGIC_JUDGE	
	if (m_pDrawNode)
#endif
	{
		m_pDrawNode->SetAnchorPoint(Vec2(0, 0));
		m_pDrawNode->SetContentSize(size);
		this->AddChild(m_pDrawNode);
	}

	m_fixPoint = size / 2;

	return true;
}

void CHmsScratchPadDraw::PushToTrack(const Vec2 &pos)
{
	auto len = m_vecTrackPoints.size();
	if (len > 0)
	{
		if (pos == m_vecTrackPoints.at(len - 1))
		{
			return;
		}
	}
	m_vecTrackPoints.push_back(pos);
}

bool CHmsScratchPadDraw::OnPressed(const Vec2 & posOrigin)
{
	if (m_bDealEventAsButton)
	{
		return true;
	}
	else
	{
		if (m_bEnableDraw)
		{
			m_vecUndoRedo.clear();
			m_vecTrackPoints.clear();

			Vec2 pos = posOrigin;

			PushToTrack(pos);

			m_bNeedSave = true;

			RedrawLines();
		}
		return true;
	}
}

void CHmsScratchPadDraw::OnMove(const Vec2 & posOrigin)
{
	if (m_bDealEventAsButton)
	{

	}
	else
	{
		if (m_bEnableDraw)
		{
			Vec2 pos = posOrigin;

			EdgeLimit(pos);

			PushToTrack(pos);
			m_bNeedSave = true;
			RedrawLines();
		}
	}
}

void CHmsScratchPadDraw::OnReleased(const Vec2 & posOrigin)
{
	if (m_bDealEventAsButton)
	{
		if (m_callbackFunc)
		{
			m_callbackFunc(m_infoStu);
		}
	}
	else
	{
		if (m_bEnableDraw)
		{
			Vec2 pos = posOrigin;

			EdgeLimit(pos);

			PushToTrack(pos);
			m_bNeedSave = true;

			InsertBezier();

			RedrawLines();
		}
	}
}

bool CHmsScratchPadDraw::OnMutiPressed(const std::vector<UiEventPoint> &pos)
{
	if (pos.size() != 2 || m_bDealEventAsButton)
	{
		return false;
	}
	m_mapMultiPoints[pos[0].Id] = pos[0];
	m_mapMultiPoints[pos[1].Id] = pos[1];

	return true;
}

void CHmsScratchPadDraw::OnMutiMove(const std::vector<UiEventPoint> &pos)
{
	if (pos.size() != 2 || m_bDealEventAsButton)
	{
		return;
	}
	auto lastScale = this->GetScale();

	auto lastPoint1 = m_mapMultiPoints[pos[0].Id].uiPoint / lastScale;
	auto lastPoint2 = m_mapMultiPoints[pos[1].Id].uiPoint / lastScale;

	auto curPoint1 = pos[0].uiPoint;
	auto curPoint2 = pos[1].uiPoint;

	auto ratio = (curPoint1 - curPoint2).length() / (lastPoint1 - lastPoint2).length();
	this->SetScale(ratio);
	this->SetPosition(this->GetPoistion() - lastPoint1 * (ratio - lastScale));

	m_mapMultiPoints[pos[0].Id] = pos[0];
	m_mapMultiPoints[pos[1].Id] = pos[1];
}

void CHmsScratchPadDraw::OnMutiReleased(const std::vector<UiEventPoint> &pos)
{
	if (pos.size() != 2 || m_bDealEventAsButton)
	{
		return;
	}
}

void CHmsScratchPadDraw::RedrawLines()
{
#if _NAV_SVS_LOGIC_JUDGE	
	if (!m_pDrawNode)
		return;
#endif
	m_pDrawNode->clear();

	for (auto &stu : m_infoStu.vecLineStu)
	{
		m_pDrawNode->DrawLines(stu.vecPoints.data(), stu.vecPoints.size(), stu.lineWidth, stu.lineColor);
	}
	if (!m_vecTrackPoints.empty())
	{
		auto &vec = m_vecTrackPoints;
		m_pDrawNode->DrawLines(vec.data(), vec.size(), m_infoStu.lineWidth, m_infoStu.lineColor);
	}
}

void CHmsScratchPadDraw::InsertBezier()
{
	const std::vector<Vec2> &vec = m_vecTrackPoints;
	if (vec.size() < 4)
	{
		return;
	}

	OneLineStu stu;
	stu.lineWidth = m_infoStu.lineWidth;
	stu.lineColor = m_infoStu.lineColor;

    stu.vecPoints = vec;

#if 0

#ifdef __vxworks

	stu.vecPoints = vec;

#else
	//控制点的计算
	std::vector<Vec2> controlPoints;
	for (int i = 0; i < vec.size()-1; ++i)
	{
		int n1 = i - 1;
		int n2 = i;
		int n3 = i + 1;
		int n4 = i + 2;
		if (n1 < 0) n1 = 0;
		if (n4 > vec.size() - 1) n4 = vec.size() - 1;

		Vec2 a1 = vec[n2] + (vec[n3] - vec[n1]) / 6.0;
		Vec2 a2 = vec[n3] - (vec[n4] - vec[n2]) / 6.0;
		controlPoints.push_back(a1);
		controlPoints.push_back(a2);	
	}
	for (int i = 0; i < vec.size()-1; ++i)
	{
		CHmsBezier b = CHmsBezier::fromPoints(vec[i], controlPoints[2 * i + 0], controlPoints[2 * i + 1], vec[i + 1]);

		int threshold = 6;
		float one_over_threshold_minus_1 = float(1) / (threshold - 1);
		for (int t = 0; t < threshold-1; ++t)
		{
			auto pt = b.pointAt(t * one_over_threshold_minus_1);
			stu.vecPoints.push_back(pt);
		}
	}
#endif

#endif

	m_infoStu.vecLineStu.push_back(stu);

	m_vecTrackPoints.clear();
}

void CHmsScratchPadDraw::ZoomIn()
{
	ZoomByScale(this->GetScale()*1.1);
}

void CHmsScratchPadDraw::ZoomOut()
{
	ZoomByScale(HMS_MAX(this->GetScale()*0.9, 1.0));
}

void CHmsScratchPadDraw::ZoomByScale(float scale)
{
	auto lastPos = this->GetPoistion();
	auto lastScale = this->GetScale();

	this->SetScale(scale);

	auto newScale = this->GetScale();
	Vec2 newPos = lastPos - m_fixPoint * (newScale - lastScale);
	this->SetPosition(newPos);
}

void CHmsScratchPadDraw::ClearDrawContent()
{
#if _NAV_SVS_LOGIC_JUDGE	
	if (m_pDrawNode)
#endif
	m_pDrawNode->clear();
	m_vecTrackPoints.clear();

	m_infoStu.vecLineStu.clear();

	m_vecUndoRedo.clear();

	m_bNeedSave = true;
}

void CHmsScratchPadDraw::Undo()
{
	if (!m_infoStu.vecLineStu.empty())
	{
		m_vecUndoRedo.push_back(m_infoStu.vecLineStu.back());
		m_infoStu.vecLineStu.pop_back();

		RedrawLines();

		m_bNeedSave = true;
	}
}

void CHmsScratchPadDraw::Redo()
{
	if (!m_vecUndoRedo.empty())
	{
		m_infoStu.vecLineStu.push_back(m_vecUndoRedo.back());
		m_vecUndoRedo.pop_back();

		RedrawLines();

		m_bNeedSave = true;
	}
}

void CHmsScratchPadDraw::EdgeLimit(Vec2 &pos)
{
	auto rect = this->GetRectByOrigin();
	rect.size = rect.size / this->GetScale();
	pos.x = HMS_MIN(pos.x, rect.origin.x + rect.size.width);
	pos.x = HMS_MAX(pos.x, rect.origin.x);

	pos.y = HMS_MIN(pos.y, rect.origin.y + rect.size.height);
	pos.y = HMS_MAX(pos.y, rect.origin.y);
}

void CHmsScratchPadDraw::SetScratchPadBgType(ScratchPadBgType bgType)
{
#if _NAV_SVS_LOGIC_JUDGE	
	if (!m_pImageNode)
		return;
#endif
	ClearDrawContent();

	switch (bgType)
	{
	case ScratchPadBgType::TYPE_PIREP:
		m_pImageNode->InitWithFile("res/NavImage/pirep.png");
		break;
	case ScratchPadBgType::TYPE_ATIS:
		m_pImageNode->InitWithFile("res/NavImage/atis.png");
		break;
	case ScratchPadBgType::TYPE_CRAFT:
		m_pImageNode->InitWithFile("res/NavImage/craft.png");
		break;
	case ScratchPadBgType::TYPE_BLANK:
		m_pImageNode->InitWithFile("res/NavImage/blank.png");
		break;
	default:
		break;
	}
	m_pImageNode->SetAnchorPoint(Vec2(0, 0));

	m_infoStu.bgType = bgType;
}

void CHmsScratchPadDraw::SetScratchPadBgByPath(const std::string &path)
{
#if _NAV_SVS_LOGIC_JUDGE	
	if (!m_pImageNode || !m_pDrawNode)
		return;
#endif
	m_pImageNode->InitWithFile(path);
	m_pImageNode->SetAnchorPoint(Vec2(0, 0));

	auto size = m_pImageNode->GetContentSize();
	this->SetContentSize(size);
	m_pDrawNode->SetContentSize(size);
}

void CHmsScratchPadDraw::SaveDataToDb()
{
	if (m_bNeedSave)
	{
		m_bNeedSave = false;

		if (!m_infoStu.vecLineStu.empty() || m_infoStu.id > 0)
		{
			CHmsScratchPadSql &sql = CHmsScratchPadSql::GetInstance();
			if (m_infoStu.id < 0)
			{
				sql.InsertScratchPadInfoStu(m_infoStu);
			}
			else
			{
				sql.ModScratchPadInfoStu(m_infoStu);
			}

			ClearDrawContent();

			m_infoStu = ScratchPadInfoStu();
		}
	}
}

void CHmsScratchPadDraw::SetScratchPadInfoStu(const ScratchPadInfoStu &stu)
{
	SetScratchPadBgType(stu.bgType);
	m_infoStu = stu;
	RedrawLines();

	m_bNeedSave = false;
}

void CHmsScratchPadDraw::SetDealEventAsButton(bool b)
{
	m_bDealEventAsButton = b;
}

void CHmsScratchPadDraw::SetCallbackFunc(t_callbackFunc func)
{
	m_callbackFunc = func;
}

void CHmsScratchPadDraw::SetLineWidthColor(int lineWidth, Color4B color)
{
	m_infoStu.lineWidth = lineWidth;
	m_infoStu.lineColor = color;
}

void CHmsScratchPadDraw::GetCurLineWidthAndColor(int &lineWidth, Color4B &color)
{
	lineWidth = m_infoStu.lineWidth;
	color = m_infoStu.lineColor;
}

void CHmsScratchPadDraw::SetEnableDraw(bool b)
{
	m_bEnableDraw = b;
}

