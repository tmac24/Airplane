#include "HmsHudLine.h"



CHmsHudVerticalLine * CHmsHudVerticalLine::CreateVerticalLine(int nLineWidth /*= 2*/)
{
	CHmsHudVerticalLine * line = CHmsHudVerticalLine::Create();
	switch (nLineWidth)
	{
	case 1: 
		line->SetTexture("res/Hud/verticalLine1.png");
		break;
	case 2:
		line->SetTexture("res/Hud/verticalLine2.png");
		break;
	case 3:
		line->SetTexture("res/Hud/verticalLine3.png");
		break;
	case 4:
		line->SetTexture("res/Hud/verticalLine4.png");
		break;
	default:
		break;
	}

	return line;
}

void CHmsHudVerticalLine::SetHeight(float fHeight)
{
	auto scaleY = fHeight / this->GetTextureRect().size.height;
	SetScaleY(scaleY);
}

CHmsHudHorizontalLine * CHmsHudHorizontalLine::CreateHorizontalLine(int nLineWidht /*= 2*/)
{
	CHmsHudHorizontalLine * line = CHmsHudHorizontalLine::Create();
	switch (nLineWidht)
	{
	case 1:
		line->SetTexture("res/Hud/horizontalLine1.png");
		break;
	case 2:
		line->SetTexture("res/Hud/horizontalLine2.png");
		break;
	case 3:
		line->SetTexture("res/Hud/horizontalLine3.png");
		break;
	case 4:
		line->SetTexture("res/Hud/horizontalLine4.png");
		break;
	default:
		break;
	}

	return line;
}

void CHmsHudHorizontalLine::SetWidth(float fWidth)
{
	auto scaleX = fWidth / this->GetTextureRect().size.width;
	SetScaleX(scaleX);
}
