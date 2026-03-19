#include "HmsLayerAirports.h"
#include "HmsLayerAirportProgram.h"
#include "../HmsNavMgr.h"

CHmsLayerAirports::CHmsLayerAirports()
: HmsUiEventInterface(this)
, m_navDb(CHmsNavImageSql::GetInstance())
, m_bIsFavorite(false)
, m_toolHeight(0)
, m_itemHeight(70)
, m_bottomSpace(100)
, m_margin(10)
{
	m_pAirportInfo = nullptr;
}


CHmsLayerAirports::~CHmsLayerAirports()
{
}

bool CHmsLayerAirports::Init(const HmsAviPf::Size & size)
{
	if (!CHmsNode::Init())
	{
		return false;
	}

	SetContentSize(size);

    CHmsLayerAirportProgram::GetInstance()->Init(size);

    m_itemHeight = CHmsAirportConstant::s_listItemHeight;

    auto & layerSize = size;
    {
        auto pBgDrawNode = CHmsDrawNode::Create();
		if (pBgDrawNode)
		{
			pBgDrawNode->SetPosition(m_margin, m_toolHeight);
			pBgDrawNode->DrawSolidRect(Rect(0, 0, layerSize.width*0.3, layerSize.height - m_toolHeight - m_margin), Color4F(CHmsAirportConstant::s_airportWhite));
			pBgDrawNode->DrawVerticalLine(layerSize.width*0.3, 0, layerSize.height, 1, Color4F(CHmsAirportConstant::s_airportGray));
			this->AddChild(pBgDrawNode);
		}        
    }
#if 0
	{
		CHmsStretchImageNode *pItem = CHmsStretchImageNode::Create("res/airport/lightblue_bg.png");
		pItem->SetAnchorPoint(Vec2(0, 0));
		pItem->SetPosition(m_margin, m_toolHeight);
		pItem->SetContentSize(Size(layerSize.width*0.3, layerSize.height - m_toolHeight - m_margin));

		this->AddChild(pItem);
	}
	{
		CHmsStretchImageNode *pItem = CHmsStretchImageNode::Create("res/airport/blue_bg.png");
		pItem->SetAnchorPoint(Vec2(0, 0));
		pItem->SetPosition(m_margin + layerSize.width*0.3, m_toolHeight);
		pItem->SetContentSize(Size(layerSize.width*0.7 - m_margin - m_margin, layerSize.height - m_toolHeight - m_margin));

		this->AddChild(pItem);
	}
#endif

    m_pAirportInfo = CHmsAirportInfo::Create(Size(layerSize.width*0.7 - m_margin - m_margin, layerSize.height - m_toolHeight - m_margin));
	if (m_pAirportInfo)
	{
		m_pAirportInfo->SetAnchorPoint(Vec2(0, 0));
		m_pAirportInfo->SetPosition(m_margin + layerSize.width*0.3, m_toolHeight);
		this->AddChild(m_pAirportInfo);
	}    

	InitLeft();
	//InitRight();
	//InitBottom();

	{
        m_pKeyBoard = CHmsKeyboard::Create(Size(size.width, size.height * 0.45));
		if (m_pKeyBoard)
		{
			m_pKeyBoard->SetAnchorPoint(Vec2(0, 0));
			m_pKeyBoard->SetPosition(0, 0);
			m_pKeyBoard->SetVisible(false);
			this->AddChild(m_pKeyBoard);
		}		
	}

	InitEvent();

	return true;
}

void CHmsLayerAirports::Update(float delta)
{

}

void CHmsLayerAirports::InitEvent()
{
	this->RegisterAllNodeChild();

	m_pKeyBoard->SetKeyPressCallback([=](char c){
		if ('\b' == c)
		{
			m_pLineEdit->Backspace();
		}
		else
		{
			m_pLineEdit->AddChar(c);
		}

		RefreshAirportList();
	});

	m_pBtnAll->SetOnBtnSelectFunc([=](CHmsMutexButton *btn){

		m_bIsFavorite = false;
		m_pBtnFavorite->SetUnSelect();

		RefreshAirportList();
	});
	m_pBtnFavorite->SetOnBtnSelectFunc([=](CHmsMutexButton * btn){

		m_bIsFavorite = true;
		m_pBtnAll->SetUnSelect();

		RefreshAirportList();
	});
	//
	m_pBtnAll->SetSelect();
}

void CHmsLayerAirports::InitLeft()
{
	auto layerSize = this->GetContentSize();
	{
		m_pLineEdit = CHmsLineEidt::Create("res/airport/search_bg.png");
		if (m_pLineEdit)
		{
			m_pLineEdit->SetAnchorPoint(0, 0);
			m_pLineEdit->SetPosition(m_margin + m_margin, layerSize.height - m_margin - m_itemHeight + m_margin);
			m_pLineEdit->SetTextColor(Color4B::WHITE);
			m_pLineEdit->SetSize(Size(layerSize.width*0.3 - m_margin * 2, m_itemHeight - m_margin * 2));
			m_pLineEdit->SetMarginRight(60);
			m_pLineEdit->Focus();

			auto pLabel = CHmsGlobal::CreateUTF8Label("", CHmsAirportConstant::s_fontSize, true);
			m_pLineEdit->SetTextLabel(pLabel);

			this->AddChild(m_pLineEdit);
		}		

		auto keyboradBtn = CHmsUiImage2StateButton::CreateWithImage("res/AirportInfo/search.png", "res/AirportInfo/search.png");
		if (keyboradBtn)
		{
			keyboradBtn->SetCallbackFunc([=](CHmsUiImage2StateButton * btn, HMS_UI_BUTTON_STATE eState){
				switch (eState)
				{
				case HMS_UI_BUTTON_STATE::btn_normal:
					btn->SetScale(1.0f);
					break;
				case HMS_UI_BUTTON_STATE::btn_selected:
					btn->SetScale(1.1f);
					break;
				}
			});
			keyboradBtn->SetAnchorPoint(Vec2(0.5, 0.5));
			keyboradBtn->SetPosition(m_margin + layerSize.width*0.3 - 10 - 50, layerSize.height - m_margin - m_itemHeight / 2);
			this->AddChild(keyboradBtn);
		}		
	}
	{
		m_pListBoxLeft = CHmsListBox::CreateWithImage(NULL, NULL, "res/airport/lightblue_selbox.png", NULL, NULL);
		if (m_pListBoxLeft)
		{
			m_pListBoxLeft->SetBoxSize(Size(layerSize.width*0.3, layerSize.height - m_margin - m_itemHeight - m_bottomSpace - m_toolHeight));
			m_pListBoxLeft->SetItemTextAlignLeft(false);
			m_pListBoxLeft->SetItemNormalTextColor(Color4B(0x19, 0x29, 0x3f, 0xff));
			m_pListBoxLeft->SetAnchorPoint(Vec2(0, 0));
			m_pListBoxLeft->SetPosition(m_margin, m_toolHeight + m_bottomSpace);
			m_pListBoxLeft->SetItemHeight(CHmsAirportConstant::s_listItemHeight);
			m_pListBoxLeft->SetBacktopButtonVisible(false);
			this->AddChild(m_pListBoxLeft);

			m_pListBoxLeft->SetItemSelectChangeCallBack([=](CHmsListBoxItem *pItem, int index, bool selected){

				if (selected && m_pKeyBoard)
				{
					m_pKeyBoard->SetVisible(false);
				}
				if (selected)
				{
					AirportSelectChanged(pItem, index, selected);
				}
			});
		}
		


		for (int i = 0; i < 50; ++i)
		{
			CHmsListBoxItem *pItem = CHmsListBoxItem::CreateWithImage(NULL, "res/airport/lightblue_selbox.png", NULL, NULL);
			if (pItem)
			{
				pItem->SetText("text", CHmsAirportConstant::s_fontSize, Color4B(0x19, 0x29, 0x3f, 0xff), true);
				m_vecLeftListItemPool.pushBack(pItem);
			}            
		}
	}
	{
		m_pBtnAll = CHmsMutexButton::CreateWithImage("res/airport/btn_normal.png", "res/airport/btn_select.png");
		if (m_pBtnAll)
		{
			m_pBtnAll->SetAnchorPoint(Vec2(1, 0));
			m_pBtnAll->SetPosition(layerSize.width * 0.3 * 0.5 - 30, m_toolHeight + m_margin * 2);
			m_pBtnAll->SetContentSize(CHmsAirportConstant::s_buttonSize);
			m_pBtnAll->SetText("All", CHmsAirportConstant::s_fontSize, Color4B::WHITE, true);
			this->AddChild(m_pBtnAll);
		}		

		m_pBtnFavorite = CHmsMutexButton::CreateWithImage("res/airport/btn_normal.png", "res/airport/btn_select.png");
		if (m_pBtnFavorite)
		{
			m_pBtnFavorite->SetAnchorPoint(Vec2(0, 0));
			m_pBtnFavorite->SetPosition(layerSize.width * 0.3 * 0.5 + 30, m_toolHeight + m_margin * 2);
			m_pBtnFavorite->SetContentSize(CHmsAirportConstant::s_buttonSize);
			m_pBtnFavorite->SetText("Favorite", CHmsAirportConstant::s_fontSize, Color4B::WHITE, true);
			this->AddChild(m_pBtnFavorite);
		}		
	}
}

CHmsListBoxItem* CHmsLayerAirports::CreateRightListItem()
{
	CHmsListBoxItem *pItem = CHmsListBoxItem::CreateWithImage(NULL,
		"res/airport/blue_selbox.png",
		"res/airport/star_normal.png",
		"res/airport/star_checked.png");
	if (pItem)
	{
		pItem->SetText("text", 16);
	}

	return pItem;
}

void CHmsLayerAirports::InitRight()
{
	for (int i = 0; i < 30; ++i)
	{
		auto pItem = CreateRightListItem();
		if (pItem)
		{
			m_vecRightListItemPool.pushBack(pItem);
		}		
	}

	auto layerSize = this->GetContentSize();
	m_pListBoxRight = CHmsListBox::CreateWithImage(
		NULL, 
		NULL, 
		"res/airport/blue_selbox.png",
		"res/airport/star_normal.png", 
		"res/airport/star_checked.png");

	m_pListBoxRight->SetBoxSize(Size(layerSize.width*0.7 - 25 - m_margin * 2, layerSize.height - m_margin - m_toolHeight));
	m_pListBoxRight->SetAnchorPoint(Vec2(0, 0));
	m_pListBoxRight->SetPosition(m_margin + layerSize.width*0.3 + 25, m_toolHeight);
	this->AddChild(m_pListBoxRight);

	m_pListBoxRight->SetItemSelectChangeCallBack([=](CHmsListBoxItem *pItem, int index, bool selected){
		
		if (selected && m_pKeyBoard)
		{
			m_pKeyBoard->SetVisible(false);
		}
		if (!selected)
		{
			return;
		}
		if (index < 0 || index >= (int)m_vecStarSidStu.size())
		{
			return;
		}
		auto p = CHmsNavMgr::GetInstance()->ShowNavType(HmsNavType::NavAirportProgram);
		auto pProgram = dynamic_cast<CHmsLayerAirportProgram*>(p);
		if (pProgram)
		{
			pProgram->SetStarSidStu(m_vecStarSidStu[index]);
		}	
	});
	m_pListBoxRight->SetItemCheckChangeCallBack([=](CHmsListBoxItem *pItem, int index, bool checked){
		
		if (index < 0 || index >= (int)m_vecStarSidStu.size())
		{
			return;
		}
		StarSidStu &stu = m_vecStarSidStu.at(index);
		stu.isFavorite = checked;
		m_navDb.ModProgramFavorite(stu);

	});
}

void CHmsLayerAirports::InitBottom()
{
	auto mgr = CHmsNavMgr::GetInstance()->GetBtnMgr();
	mgr->CreateOrSelectGroup(this);

	auto CreateButton = [mgr,this]
		(int index, const char* str, const char * text, std::function<void(CHmsUiButtonAbstract*, HMS_UI_BUTTON_STATE)> func)
	{
		auto layerSize = this->GetContentSize();

		auto pToolBtn = CHmsNavMenuButton::CreateWithImage(str, text);
		if (pToolBtn)
		{
			pToolBtn->SetAnchorPoint(Vec2(0.5, 0.5));
			pToolBtn->SetPosition(layerSize.width / 8 / 2 + index*layerSize.width / 8, m_toolHeight / 2);
			pToolBtn->SetCallbackFunc(func);
			this->AddChild(pToolBtn);

			mgr->AddBtn(pToolBtn);
		}
		return pToolBtn;
	};

	auto CreateButtonMove = [mgr, this, CreateButton]
		(int index, const char* str, const char * text, std::function<void(CHmsUiButtonAbstract*, HMS_UI_BUTTON_STATE)> func)
	{
		auto pToolBtn = CreateButton(index, str, text, func); 

		if (pToolBtn)
		{
			auto icon = pToolBtn->GetIconNode();
			if (icon)
			{
				icon->SetPosition(icon->GetPoistion() + Vec2(0, -5));
			}
		}
		
		return pToolBtn;
	};

#define AP_CALLBACK(funcName) std::bind(&CHmsLayerAirports::funcName, this, std::placeholders::_1, std::placeholders::_2)  
	int index = 0;
	CreateButton(index++, "res/airport/map.png", "Map", AP_CALLBACK(ToolBtnMapClick));
	CreateButton(index++, "res/airport/X-TAB.png", "X-TAB", AP_CALLBACK(ToolBtnXTabClick));
	CreateButton(index++, "res/airport/favorite.png", "Favorite", AP_CALLBACK(ToolBtnFavoriteClick));
	CreateButtonMove(index++, "res/airport/arrow_left.png", "", AP_CALLBACK(ToolBtnArrowLeftClick));
	CreateButtonMove(index++, "res/airport/arrow_right.png", "", AP_CALLBACK(ToolBtnArrowRightClick));
	CreateButtonMove(index++, "res/airport/arrow_up.png", "", AP_CALLBACK(ToolBtnArrowUpClick));
	CreateButtonMove(index++, "res/airport/arrow_down.png", "", AP_CALLBACK(ToolBtnArrowDownClick));
	CreateButtonMove(index++, "res/airport/Enter.png", "", AP_CALLBACK(ToolBtnEnterClick));
}

void CHmsLayerAirports::ToolBtnMapClick(CHmsUiButtonAbstract *pBtn, HMS_UI_BUTTON_STATE state)
{
	ChangeBtnColor(pBtn, state);
	if (state == HmsAviPf::HMS_UI_BUTTON_STATE::btn_normal)
	{
		CHmsNavMgr::GetInstance()->ShowNavType(HmsNavType::NavMap);
	}
}

void CHmsLayerAirports::ToolBtnXTabClick(CHmsUiButtonAbstract *pBtn, HMS_UI_BUTTON_STATE state)
{
	ChangeBtnColor(pBtn, state);
	if (state != HmsAviPf::HMS_UI_BUTTON_STATE::btn_selected)
	{
		return;
	}
	if (m_pBtnAll->IsSelected())
	{
		m_pBtnFavorite->SetSelect();
	}
	else
	{
		m_pBtnAll->SetSelect();
	}
}

void CHmsLayerAirports::ToolBtnFavoriteClick(CHmsUiButtonAbstract *pBtn, HMS_UI_BUTTON_STATE state)
{
	ChangeBtnColor(pBtn, state);
	if (state != HmsAviPf::HMS_UI_BUTTON_STATE::btn_selected)
	{
		return;
	}
	int index = m_pListBoxRight->GetCurSelectIndex();
	if (index >= 0)
	{
		auto item = m_pListBoxRight->GetItem(index);
		if (item->IsChecked())
		{
			item->SetUnChecked();
		}
		else
		{
			item->SetChecked();
		}
	}
}

void CHmsLayerAirports::ToolBtnArrowLeftClick(CHmsUiButtonAbstract *pBtn, HMS_UI_BUTTON_STATE state)
{
	ChangeBtnColor(pBtn, state);
	if (state != HmsAviPf::HMS_UI_BUTTON_STATE::btn_selected)
	{
		return;
	}
	int areaIndex = GetCurOperateArea();
	if (3 == areaIndex)
	{
		m_pListBoxRight->SelectItem(-1, false);
	}
	else if (2 == areaIndex)
	{
		m_pListBoxLeft->SelectItem(-1);
		m_pLineEdit->Focus();
	}
	else if (1 == areaIndex)
	{
		if (!m_pLineEdit->HasFocus())
		{
			m_pLineEdit->Focus();
		}
		m_pLineEdit->Backspace();
		RefreshAirportList();
	}
}

void CHmsLayerAirports::ToolBtnArrowRightClick(CHmsUiButtonAbstract *pBtn, HMS_UI_BUTTON_STATE state)
{
	ChangeBtnColor(pBtn, state);
	if (state != HmsAviPf::HMS_UI_BUTTON_STATE::btn_selected)
	{
		return;
	}
	int areaIndex = GetCurOperateArea();
	if (3 == areaIndex)
	{
		
	}
	else if (2 == areaIndex)
	{
		m_pListBoxRight->SelectNext(false);
	}
	else if (1 == areaIndex)
	{
		if (!m_pLineEdit->HasFocus())
		{
			m_pLineEdit->Focus();
		}
		auto str = m_pLineEdit->GetText();
		RefreshLineEditData("", str, false);
	}
}

void CHmsLayerAirports::ToolBtnArrowUpClick(CHmsUiButtonAbstract *pBtn, HMS_UI_BUTTON_STATE state)
{
	ChangeBtnColor(pBtn, state);
	if (state != HmsAviPf::HMS_UI_BUTTON_STATE::btn_selected)
	{
		return;
	}
	int areaIndex = GetCurOperateArea();
	if (3 == areaIndex)
	{
		m_pListBoxRight->SelectPrevious(false);
	}
	else if (2 == areaIndex)
	{
		m_pListBoxLeft->SelectPrevious();
	}
	else if (1 == areaIndex)
	{
		if (!m_pLineEdit->HasFocus())
		{
			m_pLineEdit->Focus();
		}
		auto str = m_pLineEdit->GetText();
		if (str.size() > 1)
		{
			RefreshLineEditData(str, str.substr(0, str.length() - 1), true);
		}
		else
		{
			RefreshLineEditData(str, str, true);
		}
	}
}

void CHmsLayerAirports::ToolBtnArrowDownClick(CHmsUiButtonAbstract *pBtn, HMS_UI_BUTTON_STATE state)
{
	ChangeBtnColor(pBtn, state);
	if (state != HmsAviPf::HMS_UI_BUTTON_STATE::btn_selected)
	{
		return;
	}
	int areaIndex = GetCurOperateArea();
	if (3 == areaIndex)
	{
		m_pListBoxRight->SelectNext(false);
	}
	else if (2 == areaIndex)
	{
		m_pListBoxLeft->SelectNext();
	}
	else if (1 == areaIndex)
	{
		if (!m_pLineEdit->HasFocus())
		{
			m_pLineEdit->Focus();
		}
		auto str = m_pLineEdit->GetText();
		if (str.size() > 1)
		{
			RefreshLineEditData(str, str.substr(0, str.length() - 1), false);
		}
		else
		{
			RefreshLineEditData(str, str, false);
		}
	}
}

void CHmsLayerAirports::ToolBtnEnterClick(CHmsUiButtonAbstract *pBtn, HMS_UI_BUTTON_STATE state)
{
	ChangeBtnColor(pBtn, state);
	if (state != HmsAviPf::HMS_UI_BUTTON_STATE::btn_normal)
	{
		return;
	}

    CHmsNavMgr::GetInstance()->ShowNavType(HmsNavType::NavAirportProgramPreview);
#if 0
	int areaIndex = GetCurOperateArea();
	if (3 == areaIndex)
	{
		m_pListBoxRight->SelectItem(m_pListBoxRight->GetCurSelectIndex());
	}
	else if (2 == areaIndex)
	{
		m_pListBoxRight->SelectNext(false);
	}
	else if (1 == areaIndex)
	{
		m_pLineEdit->Blur();
		m_pListBoxLeft->SelectNext();
	}
#endif
}

bool CHmsLayerAirports::OnPressed(const Vec2 & posOrigin)
{
	auto rect = m_pLineEdit->GetRectFromParent();
	if (rect.containsPoint(posOrigin))
	{
		m_pLineEdit->Focus();
		m_pKeyBoard->SetVisible(true);
		return true;
	}
	else
	{
		m_pLineEdit->Blur();
        return HmsUiEventInterface::OnPressed(posOrigin);
	}
}

void CHmsLayerAirports::AirportSelectChanged(CHmsListBoxItem *pItem, int index, bool selected)
{
	std::string str = pItem->GetText();
	int pos = str.find(":");
	if (pos != std::string::npos)
	{
		//m_pListBoxRight->Clear();
		m_vecStarSidStu.clear();
		int curPoolIndex = 0;

		std::string icaoCode = str.substr(0, pos);

        m_pAirportInfo->SetIcaoCode(icaoCode);
        return;

		std::map<int, std::string> typeMap;
		typeMap[1] = "AIRPORT";
		typeMap[5] = "STAR";
		typeMap[10] = "SID";
		typeMap[15] = "ILS";
		typeMap[20] = "VOR";

		for (auto iter = typeMap.begin(); iter != typeMap.end(); ++iter)
		{
			std::vector<StarSidStu> vecStu = m_navDb.GetStarSidInfo(icaoCode, false, iter->first);
			if (!vecStu.empty())
			{
				{
					auto temp = m_pListBoxRight->AppendItem("", false, false);
					temp->SetTextColor(Color4B::WHITE);
					temp->SetSelectable(false);
					temp->SetText(iter->second.c_str(), 26);

					StarSidStu stu;
					m_vecStarSidStu.push_back(stu);
					m_vecStarSidStu.insert(m_vecStarSidStu.end(), vecStu.begin(), vecStu.end());
				}
				for (int i = 0; i < (int)vecStu.size(); ++i)
				{
					if (curPoolIndex >= m_vecRightListItemPool.size())
					{
						auto pItem = CreateRightListItem();
						m_vecRightListItemPool.pushBack(pItem);
					}
					auto item = m_vecRightListItemPool.at(curPoolIndex);
					++curPoolIndex;

					m_pListBoxRight->AppendItem(item, vecStu.at(i).name.c_str());
					if (vecStu.at(i).isFavorite)
					{
						item->SetChecked(false);
					}
				}
			}
		}		
	}
}

void CHmsLayerAirports::RefreshAirportList()
{
	std::vector<std::string> vecRet = m_navDb.GetAirportName(m_pLineEdit->GetText(), m_bIsFavorite);
	auto len = m_vecLeftListItemPool.size();
    if ((int)vecRet.size() >= len)
	{
		vecRet.resize(len);
	}
	m_pListBoxLeft->Clear();
	for (int i = 0; i < (int)vecRet.size(); ++i)
	{
		m_pListBoxLeft->AppendItem(m_vecLeftListItemPool.at(i), vecRet.at(i).c_str(), false, CHmsAirportConstant::s_fontSize, true);
	}

	//m_pListBoxRight->Clear();
}

int CHmsLayerAirports::GetCurOperateArea()
{
	bool b2 = m_pListBoxLeft->GetCurSelectIndex() >= 0 ? true : false;
	bool b3 = m_pListBoxRight->GetCurSelectIndex() >= 0 ? true : false;
	if (b3)
	{
		return 3;
	}
	if (b2 && !b3)
	{
		return 2;
	}
	else
	{
		return 1;
	}
}

void CHmsLayerAirports::RefreshLineEditData(std::string curText, std::string needSearchText, bool bUp)
{
	if (needSearchText.length() >= 4)
	{
		return;
	}
	std::vector<std::string> vecRet = m_navDb.GetAirportName(needSearchText, m_bIsFavorite, true);
	if (vecRet.empty())
	{
		return;
	}
	std::string editStr;
	if (curText.empty())
	{
		editStr = vecRet[0];
	} 
	else
	{
		auto iter = std::find_if(vecRet.begin(), vecRet.end(),
			[=](const std::string strA){

			if (strA.find(curText) == 0)
			{
				return true;
			}
			else
			{
				return false;
			}
		});
		if (iter == vecRet.end())
		{
			editStr = vecRet[0];
		}
		else
		{
			int index = std::distance(vecRet.begin(), iter);
			for (int i = 0; i < (int)vecRet.size(); ++i)
			{
				if (bUp)
				{
					--index;
                    if (index < 0) index = (int)vecRet.size() - 1;
				}
				else
				{
					++index;
					if (index >= (int)vecRet.size()) index = 0;
				}
				editStr = vecRet[index];
				if (editStr.find(curText) != 0)
				{
					break;
				}
			}
		}
	}
	
	if (curText.empty())
	{
		m_pLineEdit->SetText(editStr.substr(0, needSearchText.length() + 1));
	}
	else
	{
		m_pLineEdit->SetText(editStr.substr(0, curText.length()));
	}

	RefreshAirportList();
}

void CHmsLayerAirports::ChangeBtnColor(CHmsUiButtonAbstract *pBtn, HMS_UI_BUTTON_STATE state)
{
	auto pNode = dynamic_cast<CHmsImageNode*>(pBtn);
	switch (state)
	{
	case HmsAviPf::HMS_UI_BUTTON_STATE::btn_normal:
		if (pNode)
		{
			pNode->SetColor(Color3B::WHITE);
			pNode->SetScale(1.0f);
		}
		break;
	case HmsAviPf::HMS_UI_BUTTON_STATE::btn_selected:
		if (pNode)
		{
			pNode->SetColor(Color3B(0x00, 0xa0, 0xe9));
			pNode->SetScale(1.1f);
		}
		break;
	case HmsAviPf::HMS_UI_BUTTON_STATE::btn_disable:
		break;
	default:
		break;
	}
}



