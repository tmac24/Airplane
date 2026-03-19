#include "HmsPfdBankAftSymbolDrawNode.h"
#include "../HmsGlobal.h"
#include "Calculate/CalculateZs.h"
USING_NS_HMS;

#ifndef HMS_ARRAYSIZE
#define HMS_ARRAYSIZE(arr) (sizeof(arr)/sizeof((arr)[0]))
#endif

CHmsPfdBankAftSymbolDrawNode::CHmsPfdBankAftSymbolDrawNode()
	: m_nIndexAftSymbol(-1)
	, m_nIndexBankScale0(-1)
	, m_nIndexBankScale10to20(-1)
	, m_nIndexBankScale30(-1)
	, m_nIndexBankScale45to60(-1)
	, m_nIndexBankScale90To180(-1)
	, m_fBank(0)
{
}


CHmsPfdBankAftSymbolDrawNode::~CHmsPfdBankAftSymbolDrawNode()
{
}

bool CHmsPfdBankAftSymbolDrawNode::Init()
{
	if (!CHmsMultLineDrawNode::Init())
	{
		return false;
	}

	return true;
}

void CHmsPfdBankAftSymbolDrawNode::DrawBgIndex(const HmsDrawIndex & nIndex)
{
	//DrawIndex(nIndex - 1);
	for (int i = 0; i < nIndex.nIndexCnt; i++)
	{
		DrawIndex(nIndex.nIndex+i);
	}
}

void CHmsPfdBankAftSymbolDrawNode::onDrawShape()
{
	DrawBgIndex(m_nIndexAftSymbol);

	DrawBgIndex(m_nIndexBankScale0);
	DrawBgIndex(m_nIndexBankScale10to20);
	DrawBgIndex(m_nIndexBankScale30);
	if (m_fBank > 30)
	{
		DrawBgIndex(m_nIndexBankScale45to60);
	}

	if (m_fBank > 60)
	{
		DrawBgIndex(m_nIndexBankScale90To180);
	}
}

void CHmsPfdBankAftSymbolDrawNode::InitAftSymbol(float fAftWidth, float fLineWidth)
{
	Vec2 verts[] = {
		{ 174.5f, 191.875f },
		{ 186.203f, 191.875f },
		{ 192.417f, 202.958f },
		{ 200, 193 },
		{ 207.167f, 202.958f },
		{ 214.167f, 191.875f },
		{ 225.417f, 191.875f }
	};

	for (auto & c : verts)
	{
		c.x = 200.0f - c.x;
		c.y = 191.875f - c.y;
	}

	float fRate = fAftWidth / (225.417f - 174.5f);

	for (auto & c:verts)
	{
		c.x *= fRate;
		c.y *= fRate;
	}

	//DrawLineStrip(verts, HMS_ARRAYSIZE(verts), fLineWidth+1, nStrokeWidth);
	//DrawLineStrip(verts, HMS_ARRAYSIZE(verts), fLineWidth, nStrokeWidth, HmsAviPf::Color4B::WHITE, HmsAviPf::Color4B(127, 127, 127, 255));
	DrawLineStrip(verts, HMS_ARRAYSIZE(verts), fLineWidth, Color4B(GetColor()));

	m_nIndexAftSymbol = this->GetDrawIndex();
}

void CHmsPfdBankAftSymbolDrawNode::InitBankScale(float fRadius, float fMaxLineLength, float fLineWidth)
{
	int nBankOuterPos = fRadius;

	Color4B color(GetColor());

	Vec2 line0[2];
	line0->setPoint(0, -nBankOuterPos);
	line0[1].setPoint(0, -(nBankOuterPos - fMaxLineLength));
	DrawLines(line0, 2, fLineWidth + 1.0f, color);
	m_nIndexBankScale0 = GetDrawIndex();

	{
		std::vector<Vec2>		m_vBankVerts;
		int AngleArray[] = { -20, -10, 10, 20 };
		for (auto c : AngleArray)
		{
			float fRadian = ToRadian(c);

			auto v1 = Vec2(0, -nBankOuterPos).rotateByAngle(Vec2(), fRadian);
			auto v2 = Vec2(0, -(nBankOuterPos - fMaxLineLength*0.55f)).rotateByAngle(Vec2(), fRadian);

			m_vBankVerts.push_back(v1);
			m_vBankVerts.push_back(v2);
		}
		
		DrawLines(m_vBankVerts.data(), m_vBankVerts.size(), fLineWidth, color);
		m_nIndexBankScale10to20 = HmsDrawIndex(GetDrawIndex(), HMS_ARRAYSIZE(AngleArray));
	};

	{
		std::vector<Vec2>		m_vBankVerts;
		int AngleArray[] = { -30, 30 };
		for (auto c : AngleArray)
		{
			float fRadian = ToRadian(c);

			auto v1 = Vec2(0, -nBankOuterPos).rotateByAngle(Vec2(), fRadian);
			auto v2 = Vec2(0, -(nBankOuterPos - fMaxLineLength*0.8f)).rotateByAngle(Vec2(), fRadian);

			m_vBankVerts.push_back(v1);
			m_vBankVerts.push_back(v2);
		}

		DrawLines(m_vBankVerts.data(), m_vBankVerts.size(), fLineWidth + 1, color);
		m_nIndexBankScale30 = HmsDrawIndex(GetDrawIndex(), HMS_ARRAYSIZE(AngleArray));
	};

	{
		std::vector<Vec2>		m_vBankVerts;
		int AngleArray[] = { -60, -45, 45, 60 };
		for (auto c : AngleArray)
		{
			float fRadian = ToRadian(c);

			auto v1 = Vec2(0, -nBankOuterPos).rotateByAngle(Vec2(), fRadian);
			auto v2 = Vec2(0, -(nBankOuterPos - fMaxLineLength*0.6f)).rotateByAngle(Vec2(), fRadian);

			m_vBankVerts.push_back(v1);
			m_vBankVerts.push_back(v2);
		}

		DrawLines(m_vBankVerts.data(), m_vBankVerts.size(), fLineWidth + 1, color);
		m_nIndexBankScale45to60 = HmsDrawIndex(GetDrawIndex(), HMS_ARRAYSIZE(AngleArray));
	};

	{
		std::vector<Vec2>		m_vBankVerts;
		int AngleArray[] = { -135, -90, 90, 135 };
		for (auto c : AngleArray)
		{
			float fRadian = ToRadian(c);

			auto v1 = Vec2(0, -nBankOuterPos).rotateByAngle(Vec2(), fRadian);
			auto v2 = Vec2(0, -(nBankOuterPos - 14)).rotateByAngle(Vec2(), fRadian);

			m_vBankVerts.push_back(v1);
			m_vBankVerts.push_back(v2);
		}

		DrawLines(m_vBankVerts.data(), m_vBankVerts.size(), fLineWidth, color);
		m_nIndexBankScale90To180 = HmsDrawIndex(GetDrawIndex(), HMS_ARRAYSIZE(AngleArray));
	};
}

void CHmsPfdBankAftSymbolDrawNode::SetBank(float fBank)
{
	m_fBank = abs(fBank);
}
