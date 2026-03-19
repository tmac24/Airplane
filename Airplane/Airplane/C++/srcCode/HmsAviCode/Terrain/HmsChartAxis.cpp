#include "HmsChartAxis.h"
#include "../Calculate/CalculateZs.h"


USING_NS_HMS;

CHmsChartAxis::CHmsChartAxis()
{
	m_axisType = AtLeft;
	m_tickLength = 10.0f;
	m_subTickLength = 7.0f;
	m_tickStep = 0.5;
	m_rangeLower = -1.0;
	m_rangeUpper = 1.0;

	m_tickColor = Color4F::WHITE;
	m_labelColor = Color4B::WHITE;
	m_numFormatCode = "%.1f";
	m_updateFlag = false;
}

CHmsChartAxis* CHmsChartAxis::Create(float width, float height, AxisType type)
{
	CHmsChartAxis *pRet = new CHmsChartAxis();
	if (pRet && pRet->Init(width, height, type))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = nullptr;
		return nullptr;
	}
}

bool CHmsChartAxis::Init(float width, float height, AxisType type)
{
	if (!CHmsNode::Init())
	{
		return false;
	}

	m_axisType = type;
	this->SetContentSize(Size(width, height));

// 	m_pNodeClipping = CHmsClippingNode::create();
// 	m_pNodeStencil = CHmsDrawNode::Create();
// 	m_pNodeStencil->DrawSolidRect(0, 0, width*2, height*2, Color4F::WHITE);
// 	m_pNodeClipping->setStencil(m_pNodeStencil);

	m_pDrawAxis = CHmsDrawNode::Create();
	this->AddChild(m_pDrawAxis);

	//this->AddChild(m_pNodeClipping);

	UpdateAxisLabel();
	UpdateAxis();

	this->SetScheduleUpdate();

	return true;
}

void CHmsChartAxis::Update(float delta)
{
	if (m_updateFlag)
	{
		UpdateAxisLabel();
		UpdateAxis();
		m_updateFlag = false;
	}
}

CHmsChartAxis::AxisType CHmsChartAxis::Orientation()
{
	return m_axisType;
}

void CHmsChartAxis::Resize(const HmsAviPf::Size &size)
{
	this->SetContentSize(size);

	//m_pNodeStencil->DrawSolidRect(0, 0, size.width, size.height, Color4F::WHITE);

	m_updateFlag = true;
}

void CHmsChartAxis::SetRange(double lower, double upper)
{
	m_rangeLower = lower;
	m_rangeUpper = upper;

	m_updateFlag = true;
}

void CHmsChartAxis::MoveRange(double diff)
{
	m_rangeLower += diff;
	m_rangeUpper += diff;

	m_updateFlag = true;
}

void CHmsChartAxis::SetTickStep(double step)
{
	m_tickStep = step;

	m_updateFlag = true;
}

void CHmsChartAxis::SetOrientation(AxisType type)
{
	m_axisType = type;

	m_updateFlag = true;
}

void CHmsChartAxis::SetTickColor(HmsAviPf::Color4B color)
{
	m_tickColor = Color4F(color);

	m_updateFlag = true;
}

void CHmsChartAxis::SetLabelColor(HmsAviPf::Color4B color)
{
	m_labelColor = color;

	m_updateFlag = true;
}

void CHmsChartAxis::SetNumberFormat(const std::string &formatCode)
{
	m_numFormatCode = formatCode;

	m_updateFlag = true;
}

void CHmsChartAxis::SetNumberPrecision(int precision)
{
	m_numFormatCode = "%." + CHmsGlobal::FormatString("%df", precision);

	m_updateFlag = true;
}

void CHmsChartAxis::UpdateAxisLabel()
{
	double range = m_rangeUpper - m_rangeLower;
	int tickCount = std::ceil(range / m_tickStep) + 3;

	if (tickCount == m_pAxisLabelList.size()) return;

	if (m_pAxisLabelList.size() < tickCount)
	{
		int addCount = tickCount - m_pAxisLabelList.size();
		while (addCount)
		{
			auto axisLabel = CHmsGlobal::CreateLabel("", 26);
			if (axisLabel)
			{
				m_pAxisLabelList.pushBack(axisLabel);
				this->AddChild(axisLabel);
			}
			addCount--;
		}
	}
	else
	{
		int removeCount = m_pAxisLabelList.size() - tickCount;
		while (removeCount)
		{
			auto axisLabel = m_pAxisLabelList.back();
			this->RemoveChild(axisLabel);
			m_pAxisLabelList.popBack();
			removeCount--;
		}
	}
}

void CHmsChartAxis::UpdateAxis()
{
	m_pDrawAxis->clear();

	double range = m_rangeUpper - m_rangeLower;
	double rangeToPixel = 0.0;

	double firstTick = int(m_rangeLower / m_tickStep) * m_tickStep;
	double offset = m_rangeLower - firstTick;
	for (int i = 0; i < m_pAxisLabelList.size(); i++)
	{
		int posIndex = i - 1;

		auto axisLabel = m_pAxisLabelList.at(i);
		axisLabel->SetVisible(false);
		axisLabel->SetTextColor(m_labelColor);
		CHmsGlobal::SetLabelFormat(axisLabel, m_numFormatCode.c_str(), posIndex * m_tickStep + firstTick);

		if (m_axisType == AtLeft || m_axisType == AtRight)
		{
			rangeToPixel = m_sizeContent.height / range;

			Vec2 anchorPoint = Vec2(1, 0.5);
			float x1_subTick = m_sizeContent.width - m_subTickLength;
			float x2_subTick = m_sizeContent.width;
			float x1_tick = m_sizeContent.width - m_tickLength;
			float x2_tick = m_sizeContent.width;
			if (m_axisType == AtRight)
			{
				x1_subTick = 0;
				x2_subTick = m_subTickLength;
				x1_tick = 0;
				x2_tick = m_tickLength;
				Vec2 anchorPoint = Vec2(0, 0.5);
			}

			float y_subTick = ((posIndex + 0.5) * m_tickStep - offset) * rangeToPixel;
			float y_tick = (posIndex * m_tickStep - offset) * rangeToPixel;

			if (y_subTick >= 0 && y_subTick <= m_sizeContent.height)
			{
				m_pDrawAxis->DrawHorizontalLine(x1_subTick, x2_subTick, y_subTick, 2.0, m_tickColor);
			}

			if (y_tick >= 0 && y_tick <= m_sizeContent.height)
			{
				m_pDrawAxis->DrawHorizontalLine(x1_tick, x2_tick, y_tick, 2.0, m_tickColor);
				
			}

			if (y_tick >= 0 && y_tick + axisLabel->GetContentSize().height / 2.0f <= m_sizeContent.height)
			{
				axisLabel->SetVisible(true);
				axisLabel->SetAnchorPoint(anchorPoint);
				axisLabel->SetPosition(x1_tick, y_tick);
			}
		}

		if (m_axisType == AtTop || m_axisType == AtBottom)
		{
			rangeToPixel = m_sizeContent.width / range;
			Vec2 anchorPoint = Vec2(0.5, 0);
			float y1_subTick = 0;
			float y2_subTick = m_subTickLength;
			float y1_tick = 0;
			float y2_tick = m_tickLength;
			if (m_axisType == AtBottom)
			{
				y1_subTick = m_sizeContent.height - m_subTickLength;
				y2_subTick = m_sizeContent.height;
				y1_tick = m_sizeContent.height - m_tickLength;
				y2_tick = m_sizeContent.height;
				anchorPoint = Vec2(0.5, 1);
			}

			float x_subTick = ((posIndex + 0.5) * m_tickStep - offset) * rangeToPixel;
			float x_tick = (posIndex * m_tickStep - offset) * rangeToPixel;

			if (x_subTick >= 0 && x_subTick <= m_sizeContent.width)
			{
				m_pDrawAxis->DrawVerticalLine(x_subTick, y1_subTick, y2_subTick, 2.0, m_tickColor);
			}
			
			if (x_tick >= 0 && x_tick <= m_sizeContent.width)
			{
				m_pDrawAxis->DrawVerticalLine(x_tick, y1_tick, y2_tick, 2.0, m_tickColor);
			}

			if (x_tick >= 0 && x_tick + axisLabel->GetContentSize().width / 2.0f <= m_sizeContent.width)
			{
				axisLabel->SetVisible(true);
				axisLabel->SetAnchorPoint(anchorPoint);
				axisLabel->SetPosition(x_tick, y1_tick);
			}
		}
	}
}


