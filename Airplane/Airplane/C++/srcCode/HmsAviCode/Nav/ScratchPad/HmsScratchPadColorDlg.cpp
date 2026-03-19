#include "HmsScratchPadColorDlg.h"

CHmsScratchPadColorDlg::CHmsScratchPadColorDlg()
: HmsUiEventInterface(this)
, m_callback(nullptr)
, m_bIsDragImageNode(false)
, m_lineWidth(2)
, m_lineColor(Color4B::WHITE)
{

}

CHmsScratchPadColorDlg::~CHmsScratchPadColorDlg()
{

}

CHmsScratchPadColorDlg* CHmsScratchPadColorDlg::Create(const HmsAviPf::Size & size, float trianglePosY)
{
    CHmsScratchPadColorDlg *pRet = new CHmsScratchPadColorDlg();
    if (pRet && pRet->Init(size, trianglePosY))
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

bool CHmsScratchPadColorDlg::Init(const HmsAviPf::Size & windowSize, float trianglePosY)
{
	if (!CHmsNode::Init())
	{
		return false;
	}
	SetContentSize(windowSize);

    m_xScale = windowSize.width / 300;
    m_yScale = windowSize.height / 350;

	const float triangleHeight = 20;

	auto pBgNode = CHmsDrawNode::Create();
#if _NAV_SVS_LOGIC_JUDGE
	if (pBgNode)
#endif
	{
		pBgNode->SetAnchorPoint(Vec2(0, 0));
		pBgNode->SetPosition(0, 0);
		this->AddChild(pBgNode);
		pBgNode->DrawSolidRect(
			Rect(0, 0, windowSize.width - triangleHeight, windowSize.height),
			Color4F(Color3B(0x0e, 0x19, 0x25)));
		{
			std::vector<Vec2> vec;
			vec.push_back(Vec2(windowSize.width - triangleHeight, trianglePosY));
			vec.push_back(Vec2(windowSize.width, trianglePosY + triangleHeight * 0.8));
			vec.push_back(Vec2(windowSize.width - triangleHeight, trianglePosY + triangleHeight * 0.8 * 2));

			pBgNode->DrawTriangles(vec, Color4F(Color3B(0x0e, 0x19, 0x25)));
		}
		{
			std::vector<Vec2> vec;
			vec.push_back(Vec2(0, 0));
			vec.push_back(Vec2(windowSize.width - triangleHeight, 0));
			vec.push_back(Vec2(windowSize.width - triangleHeight, trianglePosY));
			vec.push_back(Vec2(windowSize.width, trianglePosY + triangleHeight * 0.8));
			vec.push_back(Vec2(windowSize.width - triangleHeight, trianglePosY + triangleHeight * 0.8 * 2));
			vec.push_back(Vec2(windowSize.width - triangleHeight, windowSize.height));
			vec.push_back(Vec2(0, windowSize.height));
			pBgNode->DrawLineLoop(vec, 1, Color4B(0x2a, 0x4c, 0x6e, 0xff));
		}
	}

	{
		auto pItem = CHmsUiStretchButton::Create("res/BasicFrame/Round4SolidWhite.png",
			Size(110 * m_xScale, 40 * m_yScale), Color3B(0x1e, 0x37, 0x4f), Color3B::BLACK);
#if _NAV_SVS_LOGIC_JUDGE
		if (pItem)
#endif
		{
			pItem->AddText("Cancel", 32);
			pItem->SetAnchorPoint(Vec2(0, 0));
			pItem->SetPosition(Vec2((windowSize.width - triangleHeight)*0.5 + 15, 30));
			pItem->SetOnClickedFunc(
				[=](CHmsUiButtonAbstract *btn){
				this->SetVisible(false);
			});
			this->AddChild(pItem);
		}
	}
	{
		auto pItem = CHmsUiStretchButton::Create("res/BasicFrame/Round4SolidWhite.png",
            Size(110 * m_xScale, 40 * m_yScale), Color3B(0x1e, 0x37, 0x4f), Color3B::BLACK);
#if _NAV_SVS_LOGIC_JUDGE
		if (pItem)
#endif
		{
			pItem->AddText("Ok", 32);
			pItem->SetAnchorPoint(Vec2(1, 0));
			pItem->SetPosition(Vec2((windowSize.width - triangleHeight)*0.5 - 15, 30));
			pItem->SetOnClickedFunc(
				[=](CHmsUiButtonAbstract *btn){

				this->SetVisible(false);
				if (m_callback)
				{
					m_callback(m_lineWidth, m_lineColor);
				}
			});
			this->AddChild(pItem);
		}
	}

	InitWidget();

	m_imageLogicRange = Vec2(2, 25);

	this->RegisterAllNodeChild();

	OnButtonColorClick(Color4B::WHITE, 0);

	return true;
}

void CHmsScratchPadColorDlg::InitWidget()
{
	auto windowSize = this->GetContentSize();
	
	auto node = CHmsDrawNode::Create();
#if _NAV_SVS_LOGIC_JUDGE
	if (node)
#endif
	{
		std::vector<Vec2> vec;
		vec.push_back(Vec2(30, windowSize.height - 90));
		vec.push_back(Vec2(windowSize.width - 20 - 30, windowSize.height - 90));
		node->DrawLines(vec, 5);
		this->AddChild(node);
	}

	m_imageNode = CHmsImageNode::Create("res/AirportInfo/slidePointer.png");
#if _NAV_SVS_LOGIC_JUDGE
	if (m_imageNode)
#endif
	{
		m_imageNode->SetPosition(Vec2(30, windowSize.height - 65));
		m_imageNode->SetContentSize(Size(100, 100));
		m_imageNode->SetColor(Color3B(0, 71, 157));
		m_imageXRange.x = 80;
		m_imageXRange.y = windowSize.width - 20 - 30;
		this->AddChild(m_imageNode);
	}

	m_lineWidthLabel = CHmsGlobal::CreateLabel("Line Width : 2 px", 32);
#if _NAV_SVS_LOGIC_JUDGE
	if (m_lineWidthLabel)
#endif
	{
		m_lineWidthLabel->SetPosition(Vec2((windowSize.width - 20) / 2, windowSize.height - 40));
		this->AddChild(m_lineWidthLabel);
	}


	Color4B arrayColor[] = {
		Color4B::WHITE,
		Color4B::YELLOW,
		Color4B::BLUE,
		Color4B::GREEN,
		Color4B::RED,
		Color4B::MAGENTA,
		Color4B::BLACK,
		Color4B::ORANGE,
		Color4B::GRAY
	};

	float btnWidth = 55 * m_xScale;
	float space = 30;
	float leftSpace = (windowSize.width - btnWidth*3 - space * 2 - 20) / 2;

	for (int i = 0; i < 9; ++i)
	{
		auto pItem = CHmsUiStretchButton::Create("res/BasicFrame/Round4SolidWhite.png",
			Size(btnWidth, btnWidth), Color3B(arrayColor[i]), Color3B(arrayColor[i]));
#if _NAV_SVS_LOGIC_JUDGE
		if (pItem)
#endif
		{
			pItem->SetAnchorPoint(Vec2(0, 0));
			int row = i / 3;
			int col = i % 3;
			pItem->SetPosition(Vec2(leftSpace + col * (btnWidth + space), windowSize.height - 260 - row * (btnWidth + space)));
			pItem->SetOnClickedFunc(std::bind(&CHmsScratchPadColorDlg::OnButtonColorClick, this, arrayColor[i], i));
			this->AddChild(pItem);

			m_buttonRectMap[i] = pItem->GetRectFromParent();
			m_buttonColorMap[i] = arrayColor[i];
		}
	}

	m_btnSelectDraw = CHmsDrawNode::Create();
#if _NAV_SVS_LOGIC_JUDGE
	if (m_btnSelectDraw)
#endif
	this->AddChild(m_btnSelectDraw);
}

bool CHmsScratchPadColorDlg::OnPressed(const Vec2 & posOrigin)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_imageNode)
		return false;
#endif
	auto rect = m_imageNode->GetRectFromParent();
	if (rect.containsPoint(posOrigin))
	{
		m_bIsDragImageNode = true;

		m_posPressStart = posOrigin;
		m_posChildPressStart = m_imageNode->GetPoistion();
	}
	else
	{
		m_bIsDragImageNode = false;
		HmsUiEventInterface::OnPressed(posOrigin);
	}
	
	return true;
}

void CHmsScratchPadColorDlg::OnMove(const Vec2 & posOrigin)
{
	if (m_bIsDragImageNode)
	{
		UpdateLineWidth(posOrigin);
	}
	else
	{
		HmsUiEventInterface::OnMove(posOrigin);
	}
}

void CHmsScratchPadColorDlg::OnReleased(const Vec2 & posOrigin)
{
	if (m_bIsDragImageNode)
	{
		UpdateLineWidth(posOrigin);
	}
	else
	{
		HmsUiEventInterface::OnReleased(posOrigin);
	}
}

void CHmsScratchPadColorDlg::UpdateLineWidth(const Vec2 & posOrigin)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_imageNode || !m_lineWidthLabel)
		return;
#endif
	auto deltax = posOrigin.x - m_posPressStart.x;
	auto newPosx = m_posChildPressStart.x + deltax;
	newPosx = HMS_MAX(m_imageXRange.x, newPosx);
	newPosx = HMS_MIN(m_imageXRange.y, newPosx);
	m_imageNode->SetPositionX(newPosx);

	m_lineWidth = (m_imageLogicRange.y - m_imageLogicRange.x) / (m_imageXRange.y - m_imageXRange.x) * (newPosx - m_imageXRange.x) 
		+ m_imageLogicRange.x;

	m_lineWidthLabel->SetString(CHmsGlobal::FormatString("Line Width : %d px", m_lineWidth));
}

void CHmsScratchPadColorDlg::OnButtonColorClick(Color4B color, int buttonIndex)
{
	m_lineColor = color;

    float space = 5;
	auto rect = m_buttonRectMap[buttonIndex];
    Vec2 pos1 = rect.origin - Vec2(space, space);
    Vec2 pos2 = Vec2(rect.origin.x + rect.size.width + space, rect.origin.y + rect.size.height + space);

#if _NAV_SVS_LOGIC_JUDGE
	if (!m_btnSelectDraw)
		return;
#endif
	m_btnSelectDraw->clear();
	m_btnSelectDraw->DrawOutterRect(pos1, pos2);
}

void CHmsScratchPadColorDlg::SetLineWidthAndColor(int lineWidth, Color4B color)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_lineWidthLabel || !m_imageNode)
		return;
#endif
	m_lineWidth = lineWidth;
	m_lineWidthLabel->SetString(CHmsGlobal::FormatString("Line Width : %d px", m_lineWidth));
	auto imageX = (m_imageXRange.y - m_imageXRange.x) / (m_imageLogicRange.y - m_imageLogicRange.x) * (lineWidth - m_imageLogicRange.x) 
		+ m_imageXRange.x;
	m_imageNode->SetPositionX(imageX);

	int i = 0;
	for (auto it : m_buttonColorMap)
	{
		if (it.second == color)
		{
			break;
		}
		++i;
	}
	OnButtonColorClick(color, i);
}
