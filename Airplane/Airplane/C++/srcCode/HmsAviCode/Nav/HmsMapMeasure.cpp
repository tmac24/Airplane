#include "HmsMapMeasure.h"
#include "HmsNavComm.h"

CHmsMapMeasure::CHmsMapMeasure()
{
}


CHmsMapMeasure::~CHmsMapMeasure()
{
}

bool CHmsMapMeasure::Init()
{
	if (!CHmsNode::Init())
	{
		return false;
	}

	m_drawNode = CHmsDrawNode::Create();
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_drawNode)
		return false;
#endif
	this->AddChild(m_drawNode);

	m_labelUp = CHmsGlobal::CreateLabel("UP", 18);
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_labelUp)
		return false;
#endif
	m_labelUp->SetColor(Color3B::BLACK);
	m_labelDown = CHmsGlobal::CreateLabel("DOWN", 18);
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_labelDown)
		return false;
#endif
	m_labelDown->SetColor(Color3B::BLACK);
#if 1
	m_drawNode->AddChild(m_labelUp);
	m_drawNode->AddChild(m_labelDown);
#else
	this->AddChild(m_labelUp);
	this->AddChild(m_labelDown);
#endif

	CloseMeasure();
	return true;
}

void CHmsMapMeasure::UpdateLayer()
{
	
}

void CHmsMapMeasure::CloseMeasure()
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_drawNode)
		return;
#endif
	if (m_drawNode->IsVisible())
	{
		m_drawNode->SetVisible(false);
	}
}

void CHmsMapMeasure::MeasurePos(const Vec2 &touchBegin, const Vec2 &touchEnd, const Vec2 &earthBegin, const Vec2 &earthEnd)
{
	auto ecBegin = earthBegin;
	auto ecEnd = earthEnd;

	auto posBegin = touchBegin;
	auto posEnd = touchEnd;

#if _NAV_SVS_LOGIC_JUDGE
	if (!m_drawNode)
		return;
#endif
	m_drawNode->clear();
	if (!m_drawNode->IsVisible())
	{
		m_drawNode->SetVisible(true);
	}
	Vec2 n1 = (posEnd - posBegin).getPerp().getNormalized();
	Vec2 nD = (posEnd - posBegin).getNormalized();
	auto angle = MATH_RAD_TO_DEG(atan2(n1.y, n1.x));

	std::vector<Vec2> vPos;
	//中线
	vPos.push_back(posBegin);
	vPos.push_back(posEnd);

	auto handle = 25.0f;
	auto leftTop = posBegin + n1*handle;
	auto leftBottom = posBegin - n1*handle;
	auto rightTop = posEnd + n1*handle;
	auto rightBottom = posEnd - n1*handle;

	auto outter = 20.0f;
	auto outTopLeft = leftTop + n1*handle;
	auto outTopRight = rightTop + n1*handle;
	auto outRightTop = rightTop + nD*handle;
	auto outRightBottom = rightBottom + nD*handle;
	auto outBottomLeft = leftBottom - n1*handle;
	auto outBottomRight = rightBottom - n1*handle;
	auto outLeftTop = leftTop - nD*handle;
	auto outLeftBottom = leftBottom - nD*handle;

	//左竖线
	vPos.push_back(outTopLeft);
	vPos.push_back(outBottomLeft);

	//右竖线
	vPos.push_back(outTopRight);
	vPos.push_back(outBottomRight);

	//左上横线
	vPos.push_back(outLeftTop);
	vPos.push_back(leftTop);

	//右上横线
	vPos.push_back(rightTop);
	vPos.push_back(outRightTop);

	//左下横线
	vPos.push_back(outLeftBottom);
	vPos.push_back(leftBottom);

	//右下横线
	vPos.push_back(outRightBottom);
	vPos.push_back(rightBottom);

	m_drawNode->DrawLines(vPos, 2.0f, Color4B::BLACK);

	vPos.clear();
	vPos.push_back(leftTop);
	vPos.push_back(rightTop);
	vPos.push_back(leftBottom);
	vPos.push_back(rightBottom);
	m_drawNode->DrawDashLines(vPos, 2.0f, 5.0f, 2.0f, Color4B::BLACK);

	m_drawNode->DrawTriangleStrip(vPos, Color4F(0.0f, 0.0f, 0.0f, 0.4f));

	auto posCenter = posBegin.getMidpoint(posEnd);
	auto posUp = posCenter + n1 * 37;
	auto posDown = posCenter + n1*-37;

	auto distance = CalculateDistanceNM(ecBegin.x, ecBegin.y, ecEnd.x, ecEnd.y);
	auto angleDeg = CalculateAngle(ecBegin.x, ecBegin.y, ecEnd.x, ecEnd.y);
	auto angleDeg2 = CalculateAngle(ecEnd.x, ecEnd.y, ecBegin.x, ecBegin.y);

    std::string strShow;
    if (CHmsNavComm::GetInstance()->IsMetric())
    {
        double disKM = NmToKilometer(distance);
        strShow = CHmsGlobal::FormatString("%.0f\xC2\xB0M -> %.1fkm <- %.0f\xC2\xB0M", ToAngle(angleDeg2), disKM, ToAngle(angleDeg));
    }
    else
    {
        strShow = CHmsGlobal::FormatString("%.0f\xC2\xB0M -> %.1fnm <- %.0f\xC2\xB0M", ToAngle(angleDeg2), distance, ToAngle(angleDeg));
    }

	if (m_labelUp)
	{
		m_labelUp->SetPosition(posUp);
		m_labelUp->SetRotation(90 - angle);
		CHmsGlobal::SetLabelText(m_labelUp, strShow.c_str());
	}

	if (m_labelDown)
	{
		m_labelDown->SetPosition(posDown);
		m_labelDown->SetRotation(270 - angle);
		CHmsGlobal::SetLabelText(m_labelDown, strShow.c_str());
	}
}
