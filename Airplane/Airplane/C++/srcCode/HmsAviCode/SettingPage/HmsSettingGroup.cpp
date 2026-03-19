#include "HmsSettingGroup.h"
#include "HmsGlobal.h"
#include "ui/HmsUiRadio.h"
USING_NS_HMS;
CHmsSettingGroup::CHmsSettingGroup()
	: HmsUiEventInterface(this)
	, m_fWidth(100)
	, m_nItemCnt(0)
{

}

CHmsSettingGroup::~CHmsSettingGroup()
{

}

CHmsSettingGroup * CHmsSettingGroup::Create(const std::string & strGroupName, float fWidth)
{
	CHmsSettingGroup *ret = new CHmsSettingGroup();
	if (ret && ret->Init(strGroupName, fWidth))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

#define XXXINFO(x) (x*CHmsGlobal::GetDesignScale())

bool CHmsSettingGroup::Init(const std::string & strGroupName, float fWidth)
{
	if (!CHmsNode::Init())
	{
		return false;
	}
	m_fWidth = fWidth;
	float fTempHeight = (96);
	float fHeight = 0.0f;
	this->SetAnchorPoint(Vec2(0, 1.0f));

	m_nodeDraw = CHmsDrawNode::Create();
	this->AddChild(m_nodeDraw);

	m_nodeDraw->DrawSolidRect(0, 0, fWidth, -fTempHeight, Color4F(0x36 / 255.0f, 0x36 / 255.0f, 0x36 / 255.0f, 1.0f));
	fHeight += fTempHeight;

	m_labelTitle = CHmsLanguageLabel::CreateLabel(strGroupName.c_str(), (38), true)->GetLabel();
	if (m_labelTitle)
	{
		m_labelTitle->SetTextColor(Color4B::WHITE);
		m_labelTitle->SetAnchorPoint(Vec2(0, 0.5));
		m_labelTitle->SetPosition((32), fHeight / -2.0f);
		m_nodeDraw->AddChild(m_labelTitle);
	}


	m_nodeDraw->SetPositionY(fHeight);
	this->SetContentSize(Size(m_fWidth, fHeight));
	return true;
}

HmsAviPf::CHmsLabel * CHmsSettingGroup::AddItem(const std::string & strItemName, HmsAviPf::CHmsNode * nodeBtn)
{
	auto orgSize = GetContentSize();
	auto fHeight = orgSize.height;
	auto fHeightNew = fHeight + (96);
	m_nodeDraw->DrawSolidRect(0, -fHeight, m_fWidth, -fHeightNew, Color4F(0xC9 / 255.0f, 0xC9 / 255.0f, 0xC9 / 255.0f, 1.0f));

	auto labelContent = CHmsLanguageLabel::CreateLabel(strItemName.c_str(), (38), true)->GetLabel();
	labelContent->SetTextColor(Color4B::BLACK);
	labelContent->SetAnchorPoint(Vec2(0, 0.5));
	labelContent->SetPosition((64), (fHeightNew + fHeight) / -2.0f);
	m_nodeDraw->AddChild(labelContent);

	if (m_nItemCnt)
	{
		m_nodeDraw->DrawSolidRect((32), -fHeight-1, m_fWidth - (32), 1 - fHeight, Color4F(0, 0, 0, 0.75f));
	}

	m_nItemCnt++;

	orgSize.height = fHeightNew;
	m_nodeDraw->SetPositionY(orgSize.height);
	this->SetContentSize(orgSize);

	AddAndUpdateNodeBtns(nodeBtn);

	return labelContent;
}

void CHmsSettingGroup::AddRadioItem(const std::string & name1, const std::string & name2, bool bSelect1, const std::function<void(int)> & cb)
{
	auto createRadioBtn = [=](const std::string & name){
		Color4B unselectColor(0x33, 0x33, 0x33, 0xFF);
		Color4B selectColor(0x00, 0x00, 0xFF, 0xFF);

		auto radio = CHmsUiRadio::CreateWithImage("res/Btns/RadioUnselect.png", "res/Btns/RadioSelect.png");
		if (radio)
		{
			radio->AddText(name.c_str(), 38, unselectColor, true);
			radio->SetTextAlign(CHmsUiRadio::ALIGN_RIGHT);
			radio->SetNormalTextColor(unselectColor);
			radio->SetSelectedTextColor(selectColor);
			radio->SetAnchorPoint(Vec2(0, 0.5f));
			radio->SetScale((1));
		}
		return radio;
	};

	auto orgSize = GetContentSize();
	float fModeUp = (96);
	float fHeight = orgSize.height;
	float fHeightNew = fHeight + fModeUp;
	
	auto radio1 = createRadioBtn(name1);
	radio1->SetPosition(Vec2((64), fModeUp / 2));

	auto radio2 = createRadioBtn(name2);
	radio2->SetPosition(Vec2(m_fWidth / 2 + (16), fModeUp / 2));

	if (bSelect1)
		radio1->SetSelected(true);
	else
		radio2->SetSelected(true);

	auto callback = [=](HmsAviPf::CHmsUiButtonAbstract *sender, HmsAviPf::HMS_UI_BUTTON_STATE state)
	{
		if (state != HMS_UI_BUTTON_STATE::btn_selected) return;

		if (sender == radio1)
		{
			radio2->UnSelected();
			if (cb)
				cb(1);
		}
		if (sender == radio2)
		{
			radio1->UnSelected();
			if (cb)
				cb(2);
		}
	};
	radio2->SetCallbackFunc(callback);
	radio1->SetCallbackFunc(callback);

	m_nodeDraw->DrawSolidRect(0, -fHeight, m_fWidth, -fHeightNew, Color4F(0xC9 / 255.0f, 0xC9 / 255.0f, 0xC9 / 255.0f, 1.0f));

	if (m_nItemCnt)
	{
		m_nodeDraw->DrawSolidRect((32), -fHeight, m_fWidth - (32), 1 - fHeight, Color4F(0, 0, 0, 1.0f));
	}

	m_nItemCnt++;

	orgSize.height = fHeightNew;
	m_nodeDraw->SetPositionY(orgSize.height);
	this->SetContentSize(orgSize);

	AddAndUpdateRadioBtns(radio1, radio2);
}

void CHmsSettingGroup::AddAndUpdateNodeBtns(HmsAviPf::CHmsNode * nodeBtn)
{
	float fModeUp = (96);
	if (nodeBtn)
	{
		nodeBtn->SetAnchorPoint(Vec2(1.0f, 0.5f));
		nodeBtn->SetPosition(Vec2(m_fWidth - (32), fModeUp / 2));
		nodeBtn->SetScale((1));
		this->AddChild(nodeBtn);
	}

	for (auto c : m_vNodeBtn)
	{
		if (c)
		{
			auto pos = c->GetPoistion();
			pos += Vec2(0, fModeUp);
			c->SetPosition(pos);
		}
	}

	if (nodeBtn)
	{
		m_vNodeBtn.pushBack(nodeBtn);
	}
}

void CHmsSettingGroup::AddAndUpdateRadioBtns(HmsAviPf::CHmsNode * node1, HmsAviPf::CHmsNode * node2)
{
	float fModeUp = (96);

	for (auto c : m_vNodeBtn)
	{
		if (c)
		{
			auto pos = c->GetPoistion();
			pos += Vec2(0, fModeUp);
			c->SetPosition(pos);
		}
	}

	if (node1)
	{
		this->AddChild(node1);
		m_vNodeBtn.pushBack(node1);
	}

	if (node2)
	{
		this->AddChild(node2);
		m_vNodeBtn.pushBack(node2);
	}
}
