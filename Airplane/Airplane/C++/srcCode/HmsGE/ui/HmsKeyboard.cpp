#include "HmsKeyboard.h"


CHmsKeyboard::CHmsKeyboard()
: HmsUiEventInterface(this)
, m_onKeyPressFunc(nullptr)
, m_onKeybdHideFunc(nullptr)
{
}

CHmsKeyboard::~CHmsKeyboard()
{
}

bool CHmsKeyboard::Init()
{
	if (!CHmsStretchImageNode::InitWithFile("res/airport/keyboard_bg.png"))
	{
		return false;
	}
    this->SetContentSize(Size(2560, 860));

	CreateItems();

	return true;
}

bool CHmsKeyboard::Init(const HmsAviPf::Size & size)
{
	if (!CHmsStretchImageNode::InitWithFile("res/airport/keyboard_bg.png"))
	{
		return false;
	}

    this->SetContentSize(Size(2560, 860));
    this->SetScale(size.width / 2560);

	CreateItems();
	return true;
}

void CHmsKeyboard::CreateItems()
{
	float height = this->GetContentSize().height;
	float width  = this->GetContentSize().width;
	float margin = 200.0f;
	float itemHspan = 10.f;
	float itemwidth = (width - margin * 2.0f - 9.0f * itemHspan) / 10.0f;
	float itemVspan = (height - 4.0f * itemwidth) / 5.0f;
	float offheigth = itemVspan;
	{
		char array1[] = { '1', '2', '3', '4', '5', '6', '7', '8', '9', '0' };
		for (int i = 0; i < sizeof(array1); ++i)
		{
			m_mapRect[array1[i]].rect = Rect(margin + (itemwidth + itemHspan)* i, height - offheigth, itemwidth, itemwidth - 3.0f);
			m_mapRect[array1[i]].pNormalImg = "res/airport/key_number_normal.png";
			m_mapRect[array1[i]].pSelectImg = "res/airport/key_number_click.png";
			m_mapRect[array1[i]].pressChar = array1[i];
		}
		offheigth += (itemwidth - 3.0f + itemVspan);
	}
	{
		char array1[] = { 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P' };
		for (int i = 0; i < sizeof(array1); ++i)
		{
			m_mapRect[array1[i]].rect = Rect(margin + (itemwidth + itemHspan) * i, height - offheigth, itemwidth, itemwidth);
			m_mapRect[array1[i]].pNormalImg = "res/airport/key_char_normal.png";
			m_mapRect[array1[i]].pSelectImg = "res/airport/key_char_click.png";
			m_mapRect[array1[i]].pressChar = array1[i];
		}
		offheigth += (itemwidth + itemVspan);
	}
	{
		char array1[] = { 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L' };
		for (int i = 0; i < sizeof(array1); ++i)
		{
			m_mapRect[array1[i]].rect = Rect(margin + itemwidth * 0.5f + (itemwidth + itemHspan) * i + 10, height - offheigth, itemwidth, itemwidth);
			m_mapRect[array1[i]].pNormalImg = "res/airport/key_char_normal.png";
			m_mapRect[array1[i]].pSelectImg = "res/airport/key_char_click.png";
			m_mapRect[array1[i]].pressChar = array1[i];
		}
		offheigth += (itemwidth + itemVspan);
	}
	{
		char array1[] = { '.', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '/' };
		for (int i = 0; i < sizeof(array1); ++i)
		{
			m_mapRect[array1[i]].rect = Rect(margin + itemwidth * 0.5f + (itemwidth + itemHspan) * i + 10, height - offheigth, itemwidth, itemwidth);
			m_mapRect[array1[i]].pNormalImg = "res/airport/key_char_normal.png";
			m_mapRect[array1[i]].pSelectImg = "res/airport/key_char_click.png";
			m_mapRect[array1[i]].pressChar = array1[i];
		}
	}
	for (auto iter = m_mapRect.begin(); iter != m_mapRect.end(); ++iter)
	{
		AddKeyItem(iter->first, iter->second, Color4B::WHITE);
	}
	//close
	float swidth = itemwidth + 30;
	float xpos = margin + itemwidth * 0.5f - swidth - itemHspan + 10;
	float ypos = width - xpos - swidth - itemHspan;
	{
		KeyItemStu stu;
		stu.rect = Rect(xpos, height - offheigth, swidth, itemwidth);
		stu.pNormalImg = "res/airport/key_char_normal.png";
		stu.pSelectImg = "res/airport/key_char_normal.png";
		stu.pressChar = 0;
		AddKeyItem(0, stu, Color4B(0x06, 0x9f, 0xe0, 0xff), "res/airport/closeKeyboardBig.png");
	}
	//space
	{
		KeyItemStu stu;
		stu.rect = Rect(ypos + 17, height - offheigth, swidth, itemwidth);
        stu.pNormalImg = "res/airport/key_char_normal.png";
        stu.pSelectImg = "res/airport/key_char_click.png";
        stu.pressChar = 0x20;
        AddKeyItem("Sp", stu, Color4B::WHITE);
	}
	//
	{
		offheigth -= (itemwidth + itemVspan);
		KeyItemStu stu;
		stu.rect = Rect(xpos, height - offheigth, swidth, itemwidth);
		stu.pNormalImg = "res/airport/key_char_normal.png";
		stu.pSelectImg = "res/airport/key_char_click.png";
		stu.pressChar = '_';
		AddKeyItem("_", stu, Color4B::WHITE);
	}
	//backspace
	{
		KeyItemStu stu;
		stu.rect = Rect(ypos + 17, height - offheigth, swidth, itemwidth);
        stu.pNormalImg = (itemwidth < 70.0f) ? "res/airport/key_char_normal.png" : "res/airport/key_char_normal.png";
        stu.pSelectImg = (itemwidth < 70.0f) ? "res/airport/key_char_normal.png" : "res/airport/key_char_normal.png";
        stu.pressChar = '\b';
        AddKeyItem(0, stu, Color4B::WHITE, "res/airport/delCharBig.png");
	}

	this->RegisterAllNodeChild();
}

void CHmsKeyboard::AddKeyItem(char c, KeyItemStu &stu, Color4B color, std::string icon)
{
	char arr[2] = { 0 };
	arr[0] = c;
	std::string str = arr;
    AddKeyItem(str, stu, color, icon);
}

void CHmsKeyboard::AddKeyItem(std::string str, KeyItemStu &stu, Color4B color, std::string icon)
{
	auto keyItem = CHmsKeyItem::CreateWithImage(stu.pNormalImg, stu.pSelectImg, stu.rect.size, nullptr);
	keyItem->SetAnchorPoint(Vec2(0.0, 1.0));
	keyItem->SetPosition(stu.rect.origin);
	keyItem->SetPressChar(stu.pressChar);
	keyItem->SetText(str.c_str(), /*36*/(int)(stu.rect.size.height * 0.6f), color);

	keyItem->SetCallbackFunc([=](char c){
		if (c == 0)
		{
			this->SetVisible(false);
		}
		else
		{
			if (m_onKeyPressFunc)
			{
				m_onKeyPressFunc(c);
			}
		}
	});

    if (!icon.empty())
    {
        auto pIcon = CHmsImageNode::Create(icon.c_str());
        keyItem->AddChild(pIcon);
        pIcon->SetPosition(stu.rect.size * 0.5);
    }

	this->AddChild(keyItem);
}

void CHmsKeyboard::SetKeyPressCallback(const HmsOnKeyPress &func)
{
	m_onKeyPressFunc = func;
}

void CHmsKeyboard::SetKeybdHideCallback(const HmsOnKbdHide &func)
{
	m_onKeybdHideFunc = func;
}

void CHmsKeyboard::SetVisible(bool bVisible)
{
	CHmsStretchImageNode::SetVisible(bVisible);

	if (!bVisible)
	{
		if (m_onKeybdHideFunc)
		{
			m_onKeybdHideFunc();
		}
	}
}

bool CHmsKeyboard::OnPressed(const Vec2 & posOrigin)
{
	HmsUiEventInterface::OnPressed(posOrigin);

	return true;
}
