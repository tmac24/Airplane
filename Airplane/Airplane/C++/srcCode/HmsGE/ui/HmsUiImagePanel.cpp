#include "HmsUiImagePanel.h"

NS_HMS_BEGIN

CHmsUiStretchImagePanel* CHmsUiStretchImagePanel::CreateWithImage(const char * strImagePath, const Size & size)
{
	CHmsUiStretchImagePanel *ret = new CHmsUiStretchImagePanel();
	if (ret && ret->InitWithFile(strImagePath))
	{
		ret->SetContentSize(size);
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

CHmsUiStretchImagePanel::CHmsUiStretchImagePanel()
	: HmsUiEventInterface(this)
{

}

CHmsUiStretchImagePanel::~CHmsUiStretchImagePanel()
{
}

bool CHmsUiStretchImagePanel::Init()
{
	if (!CHmsStretchImageNode::Init())
	{
		return false;
	}
	return true;
}


CHmsUiImagePanel::CHmsUiImagePanel()
	: HmsUiEventInterface(this)
{

}

CHmsUiImagePanel::~CHmsUiImagePanel()
{

}

CHmsUiImagePanel* CHmsUiImagePanel::CreateWithImage(const char * strImagePath)
{
	CHmsUiImagePanel *ret = new CHmsUiImagePanel();
	if (ret && ret->InitWithFile(strImagePath))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

bool CHmsUiImagePanel::Init()
{
	if (!CHmsImageNode::Init())
	{
		return false;
	}
	return true;
}


CHmsUiPanel::CHmsUiPanel()
	: HmsUiEventInterface(this)
	, m_bSwallowEvent(true)
{

}

CHmsUiPanel::~CHmsUiPanel()
{

}

CHmsUiPanel* CHmsUiPanel::Create(const Size & size)
{
	CHmsUiPanel *ret = new CHmsUiPanel();
	if (ret && ret->Init())
	{
		ret->SetContentSize(size);
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

bool CHmsUiPanel::Init()
{
	if (!CHmsNode::Init())
	{
		return false;
	}
	return true;
}

bool CHmsUiPanel::OnPressed(const Vec2 & posOrigin)
{
	bool bRet = HmsUiEventInterface::OnPressed(posOrigin);
	if (m_bSwallowEvent)
	{
		return true;
	}
	return bRet;
}

NS_HMS_END


