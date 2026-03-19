#pragma once

#include <math.h>
#include <vector>

//Alpha滤波器
class CHmsAlphaFilter
{
public:
	CHmsAlphaFilter();
    CHmsAlphaFilter(const double& alphaValue, const double& preValue = 0.0);

    ~CHmsAlphaFilter();

	void SetAlpha(double alphaValue);
	
	void SetInitValue(double value);

	double GetFilterValue(double curValue);

	double GetCicle0360FilterValue(double curValue);
private:
	double m_alphaValue;//α值
	double m_preValue;
};