#include "HmsLadderDrawNode.h"
#include "../HmsGlobal.h"
#include "Calculate/CalculateZs.h"
USING_NS_HMS;

#ifndef HMS_ARRAYSIZE
#define HMS_ARRAYSIZE(arr) (sizeof(arr)/sizeof((arr)[0]))
#endif

CHmsLadderDrawNode::CHmsLadderDrawNode()
	: m_fLineWidth(4)
	, m_fLadderHeight(8)
	, m_fPosBegin(30)
	, m_fPosEnd(220)
	, m_fPitchPerHeight(10)
	, m_fLabelMove(15)
{

}


CHmsLadderDrawNode::~CHmsLadderDrawNode()
{
}

void CHmsLadderDrawNode::SetLineArea(float fBegin, float fEnd, float fLineWidth)
{
	m_fPosBegin = fBegin;
	m_fPosEnd = fEnd;
	m_fLineWidth = fLineWidth;
}

void CHmsLadderDrawNode::SetPitchPerHeight(float fPitchPerHeight)
{
	m_fPitchPerHeight = fPitchPerHeight;
}

void CHmsLadderDrawNode::SetLadderScaleHeight(float fHeight)
{
	m_fLadderHeight = fHeight;
}

void CHmsLadderDrawNode::AddSkyLadder(int nPitch)
{
	unsigned int nVertexCount = 6;
	Color4B color(GetColor());

	ensureCapacity(nVertexCount * 2);

	V2F_C4B_T2F * pPoss = GetCurrentPosBegin();

	float fPos = nPitch * m_fPitchPerHeight;
	float fHalfWidth = m_fLineWidth / 2.0f;
	float fHalfUsedWidth = fHalfWidth + 1.0f;

	float fTexXPos = (m_fLineWidth * 2 - 1) / 16.0f;
	float fTexYPos = (m_fLineWidth + 2) / 8;
	Tex2F top(fTexXPos, 0.0);
	Tex2F bottom(fTexXPos, fTexYPos);


	V2F_C4B_T2F a = { Vec2(m_fPosBegin, fPos + fHalfUsedWidth), color, top };
	V2F_C4B_T2F b = { Vec2(m_fPosBegin, fPos - fHalfUsedWidth), color, bottom };
	V2F_C4B_T2F c = { Vec2(m_fPosEnd, fPos + fHalfUsedWidth), color, top };
	V2F_C4B_T2F d = { Vec2(m_fPosEnd - (m_fLineWidth+2), fPos - fHalfUsedWidth), color, bottom };
	V2F_C4B_T2F e = { Vec2(m_fPosEnd, fPos - m_fLadderHeight), color, top };
	V2F_C4B_T2F f = { Vec2(m_fPosEnd - (m_fLineWidth+2), fPos - m_fLadderHeight), color, bottom };


	int nPos = 0;
	//左边
	{
		
		pPoss[nPos++] = a;
		pPoss[nPos++] = b;
		pPoss[nPos++] = c;
		pPoss[nPos++] = d;
		pPoss[nPos++] = e;
		pPoss[nPos++] = f;
		SetDrawTriangleStrip(nVertexCount);
	}

	//右边
	{
		a.vertices.x = -a.vertices.x;
		b.vertices.x = -b.vertices.x;
		c.vertices.x = -c.vertices.x;
		d.vertices.x = -d.vertices.x;
		e.vertices.x = -e.vertices.x;
		f.vertices.x = -f.vertices.x;

		pPoss[nPos++] = a;
		pPoss[nPos++] = b;
		pPoss[nPos++] = c;
		pPoss[nPos++] = d;
		pPoss[nPos++] = e;
		pPoss[nPos++] = f;

		SetDrawTriangleStrip(nVertexCount);
	}

	char strText[20] = { 0 };
	sprintf(strText, "%d", nPitch);
	auto label = GetLabel(strText);
	label->SetAnchorPoint(Vec2(0, 1.0));
	label->SetPosition(d.vertices + Vec2(2, -0));
	this->AddChild(label);

	auto labelRight = GetLabel(strText);
	labelRight->SetAnchorPoint(Vec2(1.0, 1.0));
	labelRight->SetPosition(Vec2(-d.vertices.x, d.vertices.y) + Vec2(-2, -0));
	this->AddChild(labelRight);
}

void CHmsLadderDrawNode::AddGroundLadder(int nPitch)
{
	unsigned int nVertexCount = 4*6;
	Color4B color(GetColor());

	ensureCapacity(nVertexCount * 2);

	V2F_C4B_T2F * pPoss = GetCurrentPosBegin();

	float fPos = nPitch * m_fPitchPerHeight;
	float fHalfWidth = m_fLineWidth / 2.0f;
	float fEndPos = fPos + (m_fPosEnd-m_fPosBegin) *sin(ToRadian(nPitch * 0.5f));
	float fUsedWidth = m_fLineWidth + 2.0f;

	Vec2 pos1(m_fPosBegin, fPos + m_fLadderHeight);
	Vec2 pos2(m_fPosBegin, fPos);
	Vec2 pos3(m_fPosEnd, fEndPos);

	Vec2 n1 = (pos2 - pos1).getPerp().getNormalized();
	Vec2 n2 = (pos3 - pos2).getPerp().getNormalized();
	Vec2 offset = (n1 + n2) * (1.0 / (n1.dot(n2) + 1.0));

	float fTexXPos = (m_fLineWidth * 2 - 1) / 16.0f;
	float fTexYPos = (m_fLineWidth + 2) / 8;
	Tex2F top(fTexXPos, 0.0);
	Tex2F bottom(fTexXPos, fTexYPos);


	V2F_C4B_T2F a = { pos1, color, top };
	V2F_C4B_T2F b = { pos1 + n1 * fUsedWidth, color, bottom };
	V2F_C4B_T2F c = { pos2, color, top };
	V2F_C4B_T2F d = { pos2 + offset * fUsedWidth, color, bottom };
	V2F_C4B_T2F e = { pos3, color, top };
	V2F_C4B_T2F f = { pos3 + n2 * fUsedWidth, color, bottom };

	const float c_fGap = 0.15f;

	float fPosPercent = 0.0f;
	fPosPercent = (1 - c_fGap * 2) / 3;
	V2F_C4B_T2F ce1 = { c.vertices.lerp(e.vertices, fPosPercent), color, top };
	V2F_C4B_T2F df1 = { ce1.vertices + n2 * fUsedWidth, color, bottom };

	fPosPercent = (1 - c_fGap * 2) / 3 + c_fGap;
	V2F_C4B_T2F ce2 = { c.vertices.lerp(e.vertices, fPosPercent), color, top };
	V2F_C4B_T2F df2 = { ce2.vertices + n2 * fUsedWidth, color, bottom };

	fPosPercent = (1 - c_fGap * 2) / 3 * 2 + c_fGap;
	V2F_C4B_T2F ce3 = { c.vertices.lerp(e.vertices, fPosPercent), color, top };
	V2F_C4B_T2F df3 = { ce3.vertices + n2 * fUsedWidth, color, bottom };

	fPosPercent = 1 - (1 - c_fGap * 2) / 3;
	V2F_C4B_T2F ce4 = { c.vertices.lerp(e.vertices, fPosPercent), color, top };
	V2F_C4B_T2F df4 = { ce4.vertices + n2 * fUsedWidth, color, bottom };

	int nPos = 0;

	auto quad2Triangles = [&pPoss, &nPos](const V2F_C4B_T2F & a, const V2F_C4B_T2F & b, const V2F_C4B_T2F & d, const V2F_C4B_T2F & c)
	{
		pPoss[nPos++] = a;
		pPoss[nPos++] = b;
		pPoss[nPos++] = d;
		pPoss[nPos++] = a;//add
		pPoss[nPos++] = d;//add
		pPoss[nPos++] = c;
	};

	//左边
	{
#if 1
		quad2Triangles(a, b, d, c);
		quad2Triangles(c, d, df1, ce1);
		quad2Triangles(df2,ce2,ce3,df3);
		quad2Triangles(df4,ce4,e,f);
		SetDrawTriangles(nVertexCount);
#else
		pPoss[nPos++] = a;
		pPoss[nPos++] = b;
		pPoss[nPos++] = d;
		pPoss[nPos++] = c;

		pPoss[nPos++] = c;
		pPoss[nPos++] = d;
		pPoss[nPos++] = df1;
		pPoss[nPos++] = ce1;

		pPoss[nPos++] = df2;
		pPoss[nPos++] = ce2;
		pPoss[nPos++] = ce3;
		pPoss[nPos++] = df3;

		pPoss[nPos++] = df4;
		pPoss[nPos++] = ce4;
		pPoss[nPos++] = e;
		pPoss[nPos++] = f;
		SetDrawQuads(nVertexCount);
#endif
	}

	//右边
	{
		a.vertices.x = -a.vertices.x;
		b.vertices.x = -b.vertices.x;
		c.vertices.x = -c.vertices.x;
		d.vertices.x = -d.vertices.x;
		ce1.vertices.x = -ce1.vertices.x;
		df1.vertices.x = -df1.vertices.x;
		ce2.vertices.x = -ce2.vertices.x;
		df2.vertices.x = -df2.vertices.x;
		ce3.vertices.x = -ce3.vertices.x;
		df3.vertices.x = -df3.vertices.x;
		ce4.vertices.x = -ce4.vertices.x;
		df4.vertices.x = -df4.vertices.x;
		e.vertices.x = -e.vertices.x;
		f.vertices.x = -f.vertices.x;

#if 1
		quad2Triangles(a, b, d, c);
		quad2Triangles(c, d, df1, ce1);
		quad2Triangles(df2,ce2,ce3,df3);
		quad2Triangles(df4,ce4,e,f);
		SetDrawTriangles(nVertexCount);
#else
		pPoss[nPos++] = a;
		pPoss[nPos++] = b;
		pPoss[nPos++] = d;
		pPoss[nPos++] = c;

		pPoss[nPos++] = c;
		pPoss[nPos++] = d;
		pPoss[nPos++] = df1;
		pPoss[nPos++] = ce1;

		pPoss[nPos++] = df2;
		pPoss[nPos++] = ce2;
		pPoss[nPos++] = ce3;
		pPoss[nPos++] = df3;

		pPoss[nPos++] = df4;
		pPoss[nPos++] = ce4;
		pPoss[nPos++] = e;
		pPoss[nPos++] = f;
		SetDrawQuads(nVertexCount);
#endif
	}

	char strText[20] = { 0 };
	sprintf(strText, "-%d", abs(nPitch));
	auto label = GetLabel(strText);
	label->SetAnchorPoint(Vec2(1.0, 0.5));
	label->SetPosition(Vec2(-m_fPosEnd - 5, e.vertices.y) + Vec2(-2, 7));
	this->AddChild(label);

	auto labelRight = GetLabel(strText);
	labelRight->SetAnchorPoint(Vec2(0.0, 0.5));
	labelRight->SetPosition(Vec2(m_fPosEnd, e.vertices.y) + Vec2(6, 7));
	this->AddChild(labelRight);
}

void CHmsLadderDrawNode::DrawAft()
{
	Vec2 verts[] = {
		{174.5f, 191.875f},
		{186.203f, 191.875f},
		{192.417f, 202.958f},
		{200, 193},
		{207.167f, 202.958f },
		{214.167f, 191.875f},
		{225.417f, 191.875f}
	};

	for (auto & c: verts)
	{
		c.x = 200.0f - c.x;
		c.y = 191.875f - c.y;
	}

	DrawLineStrip(verts, HMS_ARRAYSIZE(verts), m_fLineWidth, Color4B(GetColor()));
}

void CHmsLadderDrawNode::AddSky90()
{
	float fPos = 90 * m_fPitchPerHeight;

	Vec2 verts[] = {
		{ 194.333f, 296.750f },
		{ 197.334f, 215.334f },
		{ 206.167f, 221.167f },
		{ 201.167f, 211.500f },
		{ 222.500f, 206.834f },
		{ 201.834f, 202.334f },
		{ 206.667f, 193.333f },
		{ 197.667f, 198.167f },
		{ 193.167f, 179.167f },
		{ 188.667f, 198.167f },
		{ 179.667f, 193.333f },
		{ 184.500f, 202.334f },
		{ 163.833f, 206.834f },
		{ 185.167f, 211.500f },
		{ 180.167f, 221.167f },
		{ 187.892f, 215.149f }
	};

	for (auto & c : verts)
	{
		c.x = 194.333f - c.x;
		c.y = 206.834f - c.y + fPos;
	}

	DrawLineLoop(verts, HMS_ARRAYSIZE(verts), m_fLineWidth, Color4B(GetColor()));
}

void CHmsLadderDrawNode::AddGround90()
{
	float fPos = -90 * m_fPitchPerHeight;

	DrawRing(Vec2(0, fPos), 35.834f, m_fLineWidth, 50, Color4F(GetColor()));

	Vec2 verts[] = {
		{ 173.167f, 197.167f }, { 244.834f, 197.167f },
		{ 175.255f, 185.090f }, { 242.794f, 185.231f },
		{ 183.745f, 171.747f }, { 233.992f, 171.495f },
		{ 175.255f, 209.247f }, { 242.794f, 209.247f },
		{ 183.175f, 222.004f }, { 235.148f, 222.004f },
		{ 209.000f, 161.333f }, { 209.000f, 092.333f },
	};

	for (auto & c : verts)
	{
		c.x = 209 - c.x;
		c.y = 197.167f - c.y + fPos;
	}

	DrawLines(verts, HMS_ARRAYSIZE(verts), m_fLineWidth, Color4B(GetColor()));
}

HmsAviPf::CHmsLabel * CHmsLadderDrawNode::GetLabel(const char * strText)
{
	auto label = CHmsGlobal::CreateEnglishLabel(strText, 25.0f * 2, Color4F(GetColor()), true);
	label->SetScale(0.5f);
	return label;
}



