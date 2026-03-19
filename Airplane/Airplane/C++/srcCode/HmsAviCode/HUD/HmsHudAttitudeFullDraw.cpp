#include "HmsHudAttitudeFullDraw.h"
#include "HmsLadderDrawNode.h"
#include "HmsPfdBankAftSymbolDrawNode.h"
USING_NS_HMS;


#define FLOAT_ISEQUAL(VAL1,VAL2,COMVAL) ((fabsf(VAL1 - VAL2) < COMVAL) ? true : false)

#define MATH_PI						(3.1415926f)
#define DEGREETORAD(degree)        (MATH_PI/180.0f*degree)
#define FLOAT_CONST1				(1e-2)
#ifndef HMS_ARRAYSIZE
#define HMS_ARRAYSIZE(arr) (sizeof(arr)/sizeof((arr)[0]))
#endif

CHmsHudAttitudeFullDraw * CHmsHudAttitudeFullDraw::Create(HmsAviPf::Size size)
{
	CHmsHudAttitudeFullDraw *ret = new CHmsHudAttitudeFullDraw();
	if (ret && ret->InitWithSize(size))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

CHmsHudAttitudeFullDraw::CHmsHudAttitudeFullDraw()
: m_fPitchValue(-720.0f)
, m_fRollValue(-720.0f)
, m_CmmdValue(0.0f)
, m_FivePixels(100.0f)
, m_fHeightPerPitch(1.0f)
{
}

CHmsHudAttitudeFullDraw::~CHmsHudAttitudeFullDraw()
{
}

bool CHmsHudAttitudeFullDraw::InitWithSize(HmsAviPf::Size size)
{
	if (!CHmsNode::Init())
	{
		return false;
	}
	SetContentSize(size);
	SetAnchorPoint(Vec2(0.5f, 0.5f));
	SetColor(Color3B::GREEN);

	m_nodeRoot = CHmsNode::Create();
	m_nodeRoot->SetPosition(size / 2);
	AddChild(m_nodeRoot);

	GenHudAttitude();
	SetHudPitch(0.0f);
	return true;
}

void CHmsHudAttitudeFullDraw::GenHudAttitude()
{
	HmsAviPf::Vec2 centerPos = { m_sizeContent.width / 2.0f, m_sizeContent.height / 2.0f };

	m_fHeightPerPitch = m_sizeContent.height / 36.00f;
	auto clipAttitude = CHmsClippingNode::create();
	clipAttitude->SetPosition(0, 0);

	auto attStencil = CHmsDrawNode::Create();
	attStencil->DrawSolidRect(-centerPos.x, -centerPos.y, centerPos.x, centerPos.y, Color4F::WHITE);
	//attStencil->DrawSolidRect(-centerPos, centerPos, Color4F::WHITE);
	clipAttitude->setStencil(attStencil);

	
	float fBgHalfWidth = 420.0f;
	float fBgHalfHeigth = m_fHeightPerPitch * (90.0f + 20.0f);

#if 0
	Color4B colorSky(0, 153, 255, 255);			//天蓝色
	Color4B colorGround(92, 41, 0, 255);		//褐色
	Color4B colorLine(255, 255, 255, 255);

 	bg->drawSolidRect(Vec2(-fBgHalfWidth, 0), Vec2(fBgHalfWidth, fBgHalfHeigth), Color4F(colorSky));
 	bg->drawSolidRect(Vec2(-fBgHalfWidth, 0), Vec2(fBgHalfWidth, -fBgHalfHeigth), Color4F(colorGround));
#else
	Color4B colorLine(GetColor());
#endif

	float fLadderInsideBegin = 36;
#if 0
	auto bg = DrawNode::create();
	bg->setLineWidth(3.0f);
	bg->drawLine(Vec2(-fBgHalfWidth, 0), Vec2(-fLadderInsideBegin, 0), Color4F(colorLine));
	bg->drawLine(Vec2(fLadderInsideBegin, 0), Vec2(fBgHalfWidth, 0), Color4F(colorLine));
#else
	auto bg = CHmsMultLineDrawNode::Create();
	bg->DrawHorizontalLine(-fBgHalfWidth, -fLadderInsideBegin, 0, 4.0f, Color4F(colorLine));
	bg->DrawHorizontalLine(fLadderInsideBegin, fBgHalfWidth, 0, 4.0f, Color4F(colorLine));
#endif

	auto ladder = CHmsLadderDrawNode::Create();
	ladder->SetColor(GetColor());
	ladder->SetLineArea(fLadderInsideBegin, centerPos.x * 0.55f, 4);
	ladder->SetPitchPerHeight(m_fHeightPerPitch);
	ladder->SetLadderScaleHeight(12);

	for (int i = 5; i < 90; i += 5)
	{
		ladder->AddSkyLadder(i);
		ladder->AddGroundLadder(-i);
	}

	ladder->AddSky90();
	ladder->AddGround90();

	auto aircaftSymbal = CreateAircraftSymbal(4);

	m_nodeLadderRoll = CHmsNode::Create();
	m_nodeLadderPitch = CHmsNode::Create();
	m_nodeLadderPitch->AddChild(bg);
	m_nodeLadderPitch->AddChild(ladder);
	m_nodeLadderRoll->AddChild(m_nodeLadderPitch);
	clipAttitude->AddChild(m_nodeLadderRoll);
	

	//InitPfdHeadingBox(m_nodePdf, clipAttitude->GetPoistion() + Vec2(0, 436 / 2.0));
	m_nodeRoot->AddChild(clipAttitude);
	m_nodeRoot->AddChild(aircaftSymbal);
	
}


HmsAviPf::CHmsNode * CHmsHudAttitudeFullDraw::CreateAircraftSymbal(float fLineWidth)
{
	{
		auto symbol = CHmsPfdBankAftSymbolDrawNode::Create();
		symbol->SetColor(GetColor());
		m_nodeBankScale = symbol;

		float fBankRadius = GetContentSize().height / 2.0 + 80;

		if (symbol)
		{
			symbol->InitAftSymbol(32*2, fLineWidth);
			symbol->InitBankScale(fBankRadius, 30, fLineWidth);

			auto bankArrow = CHmsMultLineDrawNode::Create();
			m_nodeBankPointer = bankArrow;
#if 0
			{
				fLineWidth = 3.0f;
				Vec2  verts[] =
				{
					{ 22.208f, 27.156f },
					{ 18.605f, 35.113f },
					{ 45.396f, 35.113f },
					{ 41.792f, 27.156f },
				};

				for (auto & c : verts)
				{
					c.x = c.x - 32;
					c.y = c.y - fBankRadius;
				}

				bankArrow->DrawLineLoop(verts, HMS_ARRAYSIZE(verts), fLineWidth, Color4B(GetColor()));
			};

			{
				Vec2  verts[] =
				{

					{ 32.000f, 05.535f },
					{ 24.736f, 21.574f },
					{ 39.264f, 21.574f },
				};

				for (auto & c : verts)
				{
					c.x = c.x - 32;
					c.y = c.y - fBankRadius;
				}

				bankArrow->DrawLineLoop(verts, HMS_ARRAYSIZE(verts), fLineWidth, Color4B(GetColor()));
			};
#else
			Vec2  verts[] =
			{
				{ 0.0f, -(fBankRadius + 2) },
				{ 7.80f, -(fBankRadius + 2 + 5 + 13.588f) },
				{ -7.80f, -(fBankRadius + 2 + 5 + 13.588f) },
			};
			bankArrow->DrawLineLoop(verts, HMS_ARRAYSIZE(verts), fLineWidth, Color4B(GetColor()));
#endif

			symbol->AddChild(bankArrow);
		}
		return symbol;
	}
}

void CHmsHudAttitudeFullDraw::SetHudPitch(float fpitch)
{
 	float fValidPitch = HMS_MIN(HMS_MAX(fpitch, -90.0f), 90.0f);
 	if (!FLOAT_ISEQUAL(m_fPitchValue, fValidPitch, FLOAT_CONST1))
 	{
		m_fPitchValue = fValidPitch;
		m_nodeLadderPitch->SetPositionY(-m_fPitchValue * m_fHeightPerPitch);
 	}
}

void CHmsHudAttitudeFullDraw::SetRoll(float fRoll)
{
	if (!FLOAT_ISEQUAL(m_fRollValue, fRoll, FLOAT_CONST1))
	{
		m_fRollValue = fRoll;
		m_nodeLadderRoll->SetRotation(-m_fRollValue);
		m_nodeBankPointer->SetRotation(-m_fRollValue);
	}
}