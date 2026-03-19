#include "HmsSearchShape.h"
#include "Language/HmsLanguageMananger.h"
#include "HmsFrame2DRoot.h"
#include "HmsNavFplLayer.h"

CHmsSearchShape::CHmsSearchShape()
: HmsUiEventInterface(this)
, m_callbackFunc(nullptr)
, m_pCurInputLineEdit(nullptr)
{

}

CHmsSearchShape::~CHmsSearchShape()
{

}

bool CHmsSearchShape::Init(const HmsAviPf::Size & windowSize)
{
	if (!CHmsNode::Init())
	{
		return false;
	}
	SetContentSize(windowSize);

	auto pBgNode = CHmsDrawNode::Create();
#if _NAV_SVS_LOGIC_JUDGE
	if (!pBgNode)
		return false;
#endif
	pBgNode->SetAnchorPoint(Vec2(0, 0));
	pBgNode->SetPosition(0, 0);
	this->AddChild(pBgNode);
	pBgNode->DrawSolidRect(Rect(Vec2(0, 0), windowSize), Color4F(Color3B(0x0e, 0x19, 0x25)));
	//画边框
	{
		std::vector<Vec2> vec;
		vec.push_back(Vec2(0, 0));
		vec.push_back(Vec2(windowSize.width, 0));
		vec.push_back(Vec2(windowSize.width, windowSize.height));
		vec.push_back(Vec2(0, windowSize.height));
		vec.push_back(Vec2(0, 0));
		pBgNode->DrawLineLoop(vec, 1, Color4B(0x2a, 0x4c, 0x6e, 0xff));
	}

	auto CreateButton = [=](Size s, Vec2 pos, const char *imagePath, const char *imageSelect)
	{
		auto pItem = CHmsUiButton::CreateWithImage(imagePath, imageSelect);
#if _NAV_SVS_LOGIC_JUDGE
		if (pItem)
#endif
		{
			pItem->SetAnchorPoint(Vec2(0, 0));
			pItem->SetPosition(pos);
			this->AddChild(pItem);
		}
		return pItem;
	};

	auto btnSize = Size(70, 70);
    m_pBtnNull = CreateButton(btnSize, Vec2(10, windowSize.height - windowSize.height / 6 * 0 - btnSize.height - 30),
		"res/NavImage/SearchMode/null_normal.png", "res/NavImage/SearchMode/null_select.png");
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pBtnNull)
		return false;
#endif
	m_pBtnNull->SetToggleBtn(true);
	m_pBtnNull->SetOnClickedFunc(std::bind(&CHmsSearchShape::OnBtnClick, this, SHAPE_NULL));

    m_pBtnSpiral = CreateButton(btnSize, Vec2(10, windowSize.height - windowSize.height / 6 * 1 - btnSize.height - 30),
		"res/NavImage/SearchMode/Circle.png", "res/NavImage/SearchMode/CircleSelect.png");
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pBtnSpiral)
		return false;
#endif
	m_pBtnSpiral->SetToggleBtn(true);
	m_pBtnSpiral->SetOnClickedFunc(std::bind(&CHmsSearchShape::OnBtnClick, this, SHAPE_SPIRAL));

    m_pBtnRectSpiral = CreateButton(btnSize, Vec2(10, windowSize.height - windowSize.height / 6 * 2 - btnSize.height - 30),
		"res/NavImage/SearchMode/Rect.png", "res/NavImage/SearchMode/RectSelect.png");
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pBtnRectSpiral)
		return false;
#endif
	m_pBtnRectSpiral->SetToggleBtn(true);
	m_pBtnRectSpiral->SetOnClickedFunc(std::bind(&CHmsSearchShape::OnBtnClick, this, SHAPE_RECT_SPIRAL));

    m_pBtnRect = CreateButton(btnSize, Vec2(10, windowSize.height - windowSize.height / 6 * 3 - btnSize.height - 30),
		"res/NavImage/SearchMode/Snake.png", "res/NavImage/SearchMode/SnakeSelect.png");
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pBtnRect)
		return false;
#endif
	m_pBtnRect->SetToggleBtn(true);
	m_pBtnRect->SetOnClickedFunc(std::bind(&CHmsSearchShape::OnBtnClick, this, SHAPE_RECT));

	auto CreateLineEdit = [=](bool bUnit, Size s, Vec2 pos, const char * labelText)
	{
		auto pItem = CHmsLineEidt::Create("res/airport/search_bg.png");
#if _NAV_SVS_LOGIC_JUDGE
		if (pItem)
#endif
		{
			pItem->SetAnchorPoint(0, 0);
			pItem->SetSize(s);
			pItem->SetPosition(pos);
			pItem->SetTextColor(Color4B::WHITE);
			this->AddChild(pItem);
			{
				auto pLabel = CHmsGlobal::CreateUTF8Label("", 32, true);
#if _NAV_SVS_LOGIC_JUDGE
				if (pLabel)
#endif
				pItem->SetTextLabel(pLabel);
			}

			auto pLabel = CHmsGlobal::CreateLanguageLabel(labelText, 26);
#if _NAV_SVS_LOGIC_JUDGE
			if (pLabel)
#endif
			{
				pLabel->SetAnchorPoint(0, 0.5);
				pLabel->SetPosition(Vec2(-130, s.height * 0.5));
				pLabel->SetTextColor(Color4B::WHITE);
				pItem->AddChild(pLabel);
			}

			if (bUnit)
			{
				pLabel = CHmsGlobal::CreateUTF8Label("", 32, true);
#if _NAV_SVS_LOGIC_JUDGE
				if (pLabel)
#endif
				{
					pLabel->SetAnchorPoint(Vec2(0, 0.5));
					pLabel->SetPosition(Vec2(s.width + 10, s.height * 0.5));
					pItem->AddChild(pLabel);
					if (CHmsNavComm::GetInstance()->IsMetric())
					{
						pLabel->SetString("km");
					}
					else
					{
						pLabel->SetString("nm");
					}
				}
			}

			m_mapLineEdit.insert(std::make_pair(pItem, 0));
		}
		return pItem;
	};

    float leftSpace = btnSize.width + 20 + 150;
    auto editSize = Size(windowSize.width - leftSpace - 10 - 150, 95);
	float bottomSpace = 130;
	
	m_pLineEditWidth = CreateLineEdit(true, editSize, Vec2(leftSpace, (editSize.height + 5) * 5 + bottomSpace), "Width");
    m_pLineEditHeight = CreateLineEdit(true, editSize, Vec2(leftSpace, (editSize.height + 5) * 4 + bottomSpace), "Height");
    m_pLineEditRadius = CreateLineEdit(true, editSize, Vec2(leftSpace, (editSize.height + 5) * 4 + bottomSpace), "Radius");
    m_pLineEditSpace = CreateLineEdit(true, editSize, Vec2(leftSpace, (editSize.height + 5) * 3 + bottomSpace), "Space");
    m_pLineEditAngle = CreateLineEdit(false, editSize, Vec2(leftSpace, (editSize.height + 5) * 2 + bottomSpace), "Angle");
    m_pLineEditPosInSide = CreateLineEdit(false, editSize, Vec2(leftSpace, (editSize.height + 5) * 1 + bottomSpace), "PosInSide");

	auto CreateStretchButton = [=](Size s, Vec2 pos, const char * text)
	{
		auto pItem = CHmsUiStretchButton::Create("res/BasicFrame/Round4SolidWhite.png", s, Color3B(0x1e, 0x37, 0x4f), Color3B::GRAY);
#if _NAV_SVS_LOGIC_JUDGE
		if (pItem)
#endif
		{
			pItem->AddText(text, 32, Color4B::WHITE, true);
			pItem->SetAnchorPoint(Vec2(0, 0));
			pItem->SetPosition(pos);
			this->AddChild(pItem);
		}
		return pItem;
	};
	btnSize = Size(260, 95);
	CHmsUiStretchButton *pItem = nullptr;
	pItem = CreateStretchButton(btnSize, Vec2(windowSize.width / 2 - btnSize.width - 10, 10), "Enter");
#if _NAV_SVS_LOGIC_JUDGE
	if (!pItem)
		return false;
#endif
	pItem->SetTextColor(Color4B::GREEN);
	pItem->SetOnClickedFunc([=](CHmsUiButtonAbstract* btn)
	{
		if (GetAltitudeStu())
		{
			CloseDlg();
			if (m_callbackFunc)
			{
				m_callbackFunc(m_shapeStu);
			}
		}	
	});
	pItem = CreateStretchButton(btnSize, Vec2(windowSize.width / 2 + 10, 10), "Cancel");
#if _NAV_SVS_LOGIC_JUDGE
	if (!pItem)
		return false;
#endif
	pItem->SetTextColor(Color4B::GRAY);
	pItem->SetOnClickedFunc([=](CHmsUiButtonAbstract* btn)
	{
		CloseDlg();
	});

	this->RegisterAllNodeChild();

	return true;
}

void CHmsSearchShape::LineEditBlur()
{
	for (auto &c : m_mapLineEdit)
	{
		if (c.first)
		{
			c.first->Blur();
		}
	}
}

void CHmsSearchShape::ShowDlg(SearchShapeStu shapeStu, std::function<void(SearchShapeStu)> func)
{
	this->SetVisible(true);
	m_shapeStu = shapeStu;
	m_callbackFunc = func;
	UpdateLineEdit();
}

void CHmsSearchShape::CloseDlg()
{
	LineEditBlur();
	this->SetVisible(false);
    if (CHmsNavComm::GetInstance()->GetCHmsNavFplLayer())
	{
        CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->ShowOrHideKeyboard(false,
			HMS_CALLBACK_1(CHmsSearchShape::OnKeyboardCallback, this),
			HMS_CALLBACK_0(CHmsSearchShape::LineEditBlur, this));
	}
}

bool CHmsSearchShape::OnPressed(const Vec2 & posOrigin)
{
	LineEditBlur();
	m_pCurInputLineEdit = nullptr;
	for (auto &c : m_mapLineEdit)
	{
		auto rect = c.first->GetRectFromParent();
		if (c.first->IsVisible() && rect.containsPoint(posOrigin))
		{
			c.first->Focus();
			m_pCurInputLineEdit = c.first;

            if (CHmsNavComm::GetInstance()->GetCHmsNavFplLayer())
			{
                CHmsNavComm::GetInstance()->GetCHmsNavFplLayer()->ShowOrHideKeyboard(true,
					HMS_CALLBACK_1(CHmsSearchShape::OnKeyboardCallback, this),
					HMS_CALLBACK_0(CHmsSearchShape::LineEditBlur, this));
			}
			break;
		}
	}
	if (!m_pCurInputLineEdit)
	{
		HmsUiEventInterface::OnPressed(posOrigin);
	}

	return true;
}

void CHmsSearchShape::OnKeyboardCallback(char c)
{
	if (m_pCurInputLineEdit)
	{
		auto str = m_pCurInputLineEdit->GetText();
		if ('\b' == c)
		{
			if (!str.empty())
			{
				str.pop_back();
			}
		}
		else
		{
			str.push_back(c);
		}
		m_pCurInputLineEdit->SetText(str);
	}	
}

void CHmsSearchShape::OnBtnClick(SearchShapeType type)
{
	SelectBtn(type);
}

void CHmsSearchShape::SelectBtn(SearchShapeType type)
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pBtnNull 
		|| !m_pBtnSpiral 
		|| !m_pBtnRectSpiral 
		|| !m_pBtnRect
		|| !m_pLineEditRadius
		|| !m_pLineEditSpace
		|| !m_pLineEditAngle
		|| !m_pLineEditWidth
		|| !m_pLineEditHeight
		|| !m_pLineEditPosInSide)
		return;
#endif
	if (m_pBtnNull->IsSelected())
	{
		m_pBtnNull->Selected();
	}
	if (m_pBtnSpiral->IsSelected())
	{
		m_pBtnSpiral->Selected();
	}
	if (m_pBtnRectSpiral->IsSelected())
	{
		m_pBtnRectSpiral->Selected();
	}
	if (m_pBtnRect->IsSelected())
	{
		m_pBtnRect->Selected();
	}
	for (auto &c : m_mapLineEdit)
	{
		c.first->SetVisible(false);
	}

	if (type == SHAPE_NULL)
	{
		m_pBtnNull->Selected();
	}
	else if (type == SHAPE_SPIRAL)
	{
		m_pBtnSpiral->Selected();
		m_pLineEditRadius->SetVisible(true);
		m_pLineEditSpace->SetVisible(true);
		m_pLineEditAngle->SetVisible(true);
	}
	else if (type == SHAPE_RECT_SPIRAL)
	{
		m_pBtnRectSpiral->Selected();
		m_pLineEditRadius->SetVisible(true);
		m_pLineEditSpace->SetVisible(true);
		m_pLineEditAngle->SetVisible(true);
	}
	else if (type == SHAPE_RECT)
	{
		m_pBtnRect->Selected();
		m_pLineEditWidth->SetVisible(true);
		m_pLineEditHeight->SetVisible(true);
		m_pLineEditSpace->SetVisible(true);
		m_pLineEditAngle->SetVisible(true);
		m_pLineEditPosInSide->SetVisible(true);
	}
}

SearchShapeType CHmsSearchShape::GetButtonSelectState()
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pBtnNull
		|| !m_pBtnSpiral
		|| !m_pBtnRectSpiral
		|| !m_pBtnRect)
		return SHAPE_NULL;
#endif
	if (m_pBtnNull->IsSelected())
	{
		return SHAPE_NULL;
	}
	else if (m_pBtnSpiral->IsSelected())
	{
		return SHAPE_SPIRAL;
	}
	else if (m_pBtnRectSpiral->IsSelected())
	{
		return SHAPE_RECT_SPIRAL;
	}
	else if (m_pBtnRect->IsSelected())
	{
		return SHAPE_RECT;
	}
	else
	{
		return SHAPE_NULL;
	}
}

void CHmsSearchShape::UpdateLineEdit()
{
	SelectBtn(SHAPE_NULL);

	for (auto &c : m_mapLineEdit)
	{
		c.first->SetText("");
	}

	SelectBtn(m_shapeStu.type);
	
    auto GetValueStr = [](float value)
    {
        std::stringstream ss;
        if (CHmsNavComm::GetInstance()->IsMetric())
        {
            ss << KnotToKMH(value);
        }
        else
        {
            ss << value;
        }
        return ss.str();
    };

	std::string str;
    str = GetValueStr(m_shapeStu.width);
#if _NAV_SVS_LOGIC_JUDGE
	if (m_pLineEditWidth)
#endif
    m_pLineEditWidth->SetText(str);

    str = GetValueStr(m_shapeStu.height);
#if _NAV_SVS_LOGIC_JUDGE
	if (m_pLineEditHeight)
#endif
    m_pLineEditHeight->SetText(str);

    str = GetValueStr(m_shapeStu.radius);
#if _NAV_SVS_LOGIC_JUDGE
	if (m_pLineEditRadius)
#endif
    m_pLineEditRadius->SetText(str);

    str = GetValueStr(m_shapeStu.space);
#if _NAV_SVS_LOGIC_JUDGE
	if (m_pLineEditSpace)
#endif
    m_pLineEditSpace->SetText(str);

    std::stringstream ss;
	ss.str("");
	ss << m_shapeStu.angle;
#if _NAV_SVS_LOGIC_JUDGE
	if (m_pLineEditAngle)
#endif
	m_pLineEditAngle->SetText(ss.str());

	ss.str("");
	ss << m_shapeStu.posInSide;
#if _NAV_SVS_LOGIC_JUDGE
	if (m_pLineEditPosInSide)
#endif
	m_pLineEditPosInSide->SetText(ss.str());
}

bool CHmsSearchShape::GetAltitudeStu()
{
    auto GetValueFromStr = [](std::string &str)
    {
        double value = atof(str.c_str());
        if (CHmsNavComm::GetInstance()->IsMetric())
        {
            return KMHToKnot(value);
        }
        else
        {
            return value;
        }
    };


	SearchShapeStu shapeStu;
	auto type = GetButtonSelectState();
	shapeStu.type = type;

#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pLineEditWidth
		|| !m_pLineEditHeight
		|| !m_pLineEditRadius
		|| !m_pLineEditSpace
		|| !m_pLineEditAngle
		|| !m_pLineEditPosInSide)
		return false;
#endif
	auto strWidth = m_pLineEditWidth->GetText();
	auto strHeight = m_pLineEditHeight->GetText();
	auto strRadius = m_pLineEditRadius->GetText();
	auto strSpace = m_pLineEditSpace->GetText();
	auto strAngle = m_pLineEditAngle->GetText();
	auto strPosInSide = m_pLineEditPosInSide->GetText();
	if (type == SHAPE_SPIRAL)
	{
        shapeStu.radius = GetValueFromStr(strRadius);
        shapeStu.space = GetValueFromStr(strSpace);
		shapeStu.angle = atof(strAngle.c_str());
		if (shapeStu.radius <= 0 || shapeStu.space <= 0)
		{
			return false;
		}
	}
	else if (type == SHAPE_RECT_SPIRAL)
	{
        shapeStu.radius = GetValueFromStr(strRadius);
        shapeStu.space = GetValueFromStr(strSpace);
		shapeStu.angle = atof(strAngle.c_str());
		if (shapeStu.radius <= 0 || shapeStu.space <= 0)
		{
			return false;
		}
	}
	else if (type == SHAPE_RECT)
	{
        shapeStu.width = GetValueFromStr(strWidth);
        shapeStu.height = GetValueFromStr(strHeight);
        shapeStu.space = GetValueFromStr(strSpace);
		shapeStu.angle = atof(strAngle.c_str());
		shapeStu.posInSide = atof(strPosInSide.c_str());
		if (shapeStu.width <= 0 || 
			shapeStu.height <= 0 || 
			shapeStu.space <= 0 || 
			shapeStu.posInSide < 0 || 
			shapeStu.posInSide > 1)
		{
			return false;
		}
	}
	
	m_shapeStu = shapeStu;

	return true;
}

void CHmsSearchShape::UpdateAllChildPos(CHmsNode *pNode)
{
	if (pNode)
	{
		pNode->ForceUpdate();

		auto &vec = pNode->GetChildren();
		for (auto &c : vec)
		{
			UpdateAllChildPos(c);
		}
	}
}




