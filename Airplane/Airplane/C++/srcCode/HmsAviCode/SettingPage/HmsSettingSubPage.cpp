#include "HmsSettingSubPage.h"
#include "ui/HmsUiImageButton.h"
#include "ui/HmsUiRadio.h"
#include "Language/HmsLanguageLabel.h"

USING_NS_HMS;

CHmsSettingSubPage::CHmsSettingSubPage()
	:HmsUiEventInterface(this)
	, m_fContentBegin(1000)
	, m_nItemCnt(0)
	, m_bMutex(true)
{

}

CHmsSettingSubPage::~CHmsSettingSubPage()
{

}

CHmsSettingSubPage * CHmsSettingSubPage::Create(const HmsAviPf::Size & size)
{
	CHmsSettingSubPage *ret = new CHmsSettingSubPage();
	if (ret && ret->init(size))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

#define XXXINFO(x) (x*CHmsGlobal::GetDesignScale())

bool CHmsSettingSubPage::init(const HmsAviPf::Size & size)
{
	if (!CHmsNode::Init())
	{
		return false;
	}

	SetContentSize(size);

	auto m_imageBackground = CHmsStretchImageNode::Create("res/BasicFrame/SolidFrame.png");
	if (m_imageBackground)
	{
		m_imageBackground->SetColor(Color3B::BLACK);
		m_imageBackground->SetContentSize(size);
		m_imageBackground->SetAnchorPoint(Vec2(0, 0));
		this->AddChild(m_imageBackground);
	}

	auto posTopLeft = GetUiTopLeft();

	Vec2 pos((40), (-64));
	Size sizeBtn(64, 64);

	m_fContentBegin = posTopLeft.y + pos.y * 2;
	pos += posTopLeft;	

	auto backBtn = CHmsFixSizeUiImageButton::CreateWithImage("res/Btns/backPrev.png", sizeBtn, [=](CHmsUiButtonAbstract * btn, HMS_UI_BUTTON_STATE e){
		if (e == HMS_UI_BUTTON_STATE::btn_normal)
		{
			if (m_layerParallel)
				m_layerParallel->RunScreenAction("settingSubPage", "main");
		}
	});
	if (backBtn)
	{
		auto label = CHmsLanguageLabel::CreateLabel("Sub Page", 48, true)->GetLabel();
		if (label)
		{
			label->SetAnchorPoint(Vec2(0, 0.5));
			label->SetPosition(sizeBtn.width + 20, sizeBtn.height / 2);
			backBtn->SetAnchorPoint(Vec2(0, 0.5));
			backBtn->AddLabel(label);
			backBtn->SetPosition(pos);
			this->AddChild(backBtn);
		}
		m_labelTitle = label;
	}

// 	sizeBtn.width += label->GetContentSize().width + 20;
// 	backBtn->SetContentSize(sizeBtn);
//	backBtn->SetScale((1));

	m_nodeDraw = CHmsDrawNode::Create();
	this->AddChild(m_nodeDraw);

	RegisterAllNodeChild();
	return true;
}

void CHmsSettingSubPage::SetSubPageTitle(const std::string & title)
{
	Size sizeBtn(64, 64);
	CHmsLanguageLabel::SetText(m_labelTitle, title.c_str());
// 	sizeBtn.width += m_labelTitle->GetContentSize().width + 20;
// 	m_labelTitle->getParent()->SetContentSize(sizeBtn);
}

void CHmsSettingSubPage::SetContent(const std::vector<SettingItemContent> & vItemContent)
{
	float fContentBoder = 10;
	float fDesignHeight = (90);
	float fHeight = m_fContentBegin;
	float fHeightLow = fHeight - fDesignHeight;
	float fPageWidth = GetContentSize().width;
	float fContentWidth = fPageWidth - 2 * fContentBoder;

	m_nItemCnt = 0;
	m_nodeDraw->clear();
	RemoveAllChildInterface();

	auto getSelectedBtn = [](const CHmsUiImage2StateButton::HmsUiBtnCallback & callback, bool bSelected)
	{
		auto btn = CHmsUiImage2StateButton::CreateWithImage("res/Btns/checkRightCU.png", "res/Btns/checkRightCS.png", callback);
		if (btn)
		{
			btn->SetToggleBtn(true);
			btn->SetSelected(bSelected);
		}
		return btn;
	};

	auto getRadioBtn = [=](const SettingItemContent & item)
	{
		auto radio = CHmsUiRadio::CreateWithImage("res/Btns/checkRightCU.png", "res/Btns/checkRightCS.png");
		if (radio)
		{
			radio->SetSelected(item.bSelected);
			m_mapRadioGroup.insert(item.strName, radio);
			radio->SetCallbackFunc(HMS_CALLBACK_2(CHmsSettingSubPage::OnRadioClick, this, item.strName, item.nParam));
		}
		return radio;
	};

	for (auto c:vItemContent)
	{
		m_nodeDraw->DrawSolidRect(fContentBoder, fHeight, fContentBoder + fContentWidth, fHeightLow, Color4F(0xC9 / 255.0f, 0xC9 / 255.0f, 0xC9 / 255.0f, 1.0f));

		auto labelContent = CHmsLanguageLabel::CreateLabel(c.strName.c_str(), (38), true)->GetLabel();
		if (labelContent)
		{
			labelContent->SetTextColor(Color4B::BLACK);
			labelContent->SetAnchorPoint(Vec2(0, 0.5f));
			labelContent->SetPosition(fContentBoder + (64), (fHeightLow + fHeight) / 2.0f);
			labelContent->SetTag(88);
			AddContentNode(labelContent);
		}

		auto btn = getRadioBtn(c);
		if (btn)
		{
			btn->SetPosition(fContentBoder - (32) + fContentWidth, (fHeightLow + fHeight) / 2.0f);
			btn->SetAnchorPoint(Vec2(1.0f, 0.5f));
			btn->SetScale((1));
			btn->SetTag(88);
			AddContentNode(btn);
		}

// 		if (m_nItemCnt)
// 		{
// 			m_nodeDraw->DrawSolidRect(fContentBoder + (32), fHeight, fContentBoder - (32) + fContentWidth, fHeight - 1, Color4F::Color4F(0, 0, 0, 1.0f));
// 		}

		m_nItemCnt++;

		fHeight = fHeightLow - 4;
		fHeightLow = fHeight - fDesignHeight;
	}

	RegisterAllNodeChild();
}

void CHmsSettingSubPage::SetContent(SettingItemContent * pItems, int nArrayCnt)
{
	std::vector<SettingItemContent> vItemContent;
	for (int i = 0; i < nArrayCnt; i++)
	{
		vItemContent.push_back(pItems[i]);
	}
	if (nArrayCnt > 0)
	{
		SetContent(vItemContent);
	}
}

bool CHmsSettingSubPage::OnPressed(const Vec2 & posOrigin)
{
	HmsUiEventInterface::OnPressed(posOrigin);
	return true;
}

void CHmsSettingSubPage::OnRadioClick(HmsAviPf::CHmsUiButtonAbstract *sender, HmsAviPf::HMS_UI_BUTTON_STATE state, const std::string & strName, int nParam)
{
	if (state != HMS_UI_BUTTON_STATE::btn_selected) return;

	for (auto c:m_mapRadioGroup)
	{
		if (nullptr == c.second)
			continue;

		if (sender == c.second)
		{
			c.second->SetSelected(true);
			if (m_callback)
				m_callback(strName, nParam);
		}
		else
		{
			c.second->SetSelected(false);
		}
	}
};

void CHmsSettingSubPage::AddContentNode(HmsAviPf::CHmsNode * node)
{
	if (node)
	{
		m_vContectNode.pushBack(node);
		this->AddChild(node);
	}
}

void CHmsSettingSubPage::RemoveAllContentNode()
{
	for (auto c:m_vContectNode)
	{
		if (c)
		{
			c->RemoveFromParent();
		}
	}
	m_vContectNode.clear();
}
