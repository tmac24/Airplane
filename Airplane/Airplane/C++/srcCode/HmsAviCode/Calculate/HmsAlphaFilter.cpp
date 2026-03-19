#include "HmsAlphaFilter.h"

CHmsAlphaFilter::CHmsAlphaFilter()
{
	m_alphaValue = 0.05;
	m_preValue = 0.0;
}

CHmsAlphaFilter::CHmsAlphaFilter(const double& alphaValue, const double& preValue)
{
    m_alphaValue = alphaValue;
    m_preValue = preValue;
}

CHmsAlphaFilter::~CHmsAlphaFilter()
{

}

void CHmsAlphaFilter::SetInitValue(double value)
{
	m_preValue = value;
}

void CHmsAlphaFilter::SetAlpha(double alphaValue)
{
	m_alphaValue = alphaValue;
}

double CHmsAlphaFilter::GetFilterValue(double curValue)
{
	m_preValue = curValue * m_alphaValue + m_preValue * (1 - m_alphaValue);

	return m_preValue;
}

double CHmsAlphaFilter::GetCicle0360FilterValue(double curValue)
{
	double delta = fmodl(curValue - m_preValue, 360);
	if (delta > 180)
	{
		delta -= 360;
	}
	else if (delta < -180)
	{
		delta += 360;
	}
	m_preValue += delta*m_alphaValue;

	if (m_preValue<0)
	{
		m_preValue += 360;
	}
	else if (m_preValue >=360)
	{
		m_preValue -= 360;
	}
	return m_preValue;
}
