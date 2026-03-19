#pragma once

#include "HmsAviPf.h"


class CHmsHudVerticalLine : public HmsAviPf::CHmsImageNode
{
public:
	static CHmsHudVerticalLine * CreateVerticalLine(int nLineWidth = 2);

	CREATE_FUNC(CHmsHudVerticalLine);

	void SetHeight(float fHeight);
};

class CHmsHudHorizontalLine : public HmsAviPf::CHmsImageNode
{
public:
	static CHmsHudHorizontalLine * CreateHorizontalLine(int nLineWidht = 2);

	CREATE_FUNC(CHmsHudHorizontalLine);

	void SetWidth(float fWidth);
};