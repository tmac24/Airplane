#pragma once

#include "base/HmsNode.h"
#include "base/HmsClippingNode.h"
#include "../HmsGlobal.h"

class CHmsChartAxis : public HmsAviPf::CHmsNode
{
public:
	enum AxisType
	{
		AtLeft,
		AtRight,
		AtTop,
		AtBottom
	};

	CHmsChartAxis();

	static CHmsChartAxis* Create(float width, float height, AxisType type);

	virtual bool Init(float width, float height, AxisType type);

	virtual void Update(float delta) override;

	AxisType Orientation();

	void Resize(const HmsAviPf::Size &size);

	void SetRange(double lower, double upper);

	void MoveRange(double diff);

	void SetTickStep(double step);

	void SetOrientation(AxisType type);

	void SetTickColor(HmsAviPf::Color4B color);

	void SetLabelColor(HmsAviPf::Color4B color);

	void SetNumberFormat(const std::string &formatCode);

	void SetNumberPrecision(int precision);

private:
	AxisType m_axisType;
	float m_tickLength;
	float m_subTickLength;
	double m_tickStep;
	double m_rangeLower;
	double m_rangeUpper;
	std::string m_numFormatCode;
	HmsAviPf::Color4B m_labelColor;
	HmsAviPf::Color4F m_tickColor;

	HmsAviPf::RefPtr<HmsAviPf::CHmsDrawNode>	m_pDrawAxis;
	HmsAviPf::Vector<HmsAviPf::CHmsLabel*>		m_pAxisLabelList;

	bool m_updateFlag;

	void FormatLabelString();

	void UpdateAxisLabel();
	void UpdateAxis();
};
