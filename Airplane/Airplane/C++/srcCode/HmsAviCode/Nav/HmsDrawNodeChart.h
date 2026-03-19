#pragma once

#include "display/HmsDrawNode.h"
#include "render/HmsGLProgram.h"
#include "render/HmsGLProgramState.h"
#include "HmsStandardDef.h"

NS_HMS_BEGIN

class CHmsDrawNodeChart : public CHmsDrawNode
{
public:
	CHmsDrawNodeChart();
	~CHmsDrawNodeChart();

	CREATE_FUNC(CHmsDrawNodeChart)

	virtual bool Init() override;

	virtual void onDrawShape() override;

	void SetLowHighColorAltitude(float lowWorldPixel, float highWorldPixel);

	void SetBeginEndIndex(int begin, int end);

private:
	int m_u_fHightColorPos;
	int m_u_fLowColorPos;

	float m_lowWorldPixel;
	float m_highWorldPixel;
	bool m_uniformChanged;

	int m_beginIndex;
	int m_endIndex;

	int m_beginPos;
	int m_count;
};

NS_HMS_END