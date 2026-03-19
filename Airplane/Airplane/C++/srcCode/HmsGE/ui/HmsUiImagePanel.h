#pragma once
#include "display/HmsStretchImageNode.h"
#include "display/HmsImageNode.h"
#include "base/RefPtr.h"
#include "HmsUiEventInterface.h"

NS_HMS_BEGIN

class CHmsUiStretchImagePanel : public CHmsStretchImageNode, public HmsUiEventInterface
{
public:
	CHmsUiStretchImagePanel();
	~CHmsUiStretchImagePanel();

	static CHmsUiStretchImagePanel* CreateWithImage(const char * strImagePath, const Size & size);
	CREATE_FUNC(CHmsStretchImageNode);

protected:
	virtual bool Init() override;
};

class CHmsUiImagePanel : public CHmsImageNode, public HmsUiEventInterface
{
public:
	CHmsUiImagePanel();
	~CHmsUiImagePanel();

	static CHmsUiImagePanel* CreateWithImage(const char * strImagePath);
	CREATE_FUNC(CHmsUiImagePanel);

protected:
	virtual bool Init() override;
};

class CHmsUiPanel : public CHmsNode, public HmsUiEventInterface
{
public:
	CHmsUiPanel();
	~CHmsUiPanel();

	CREATE_FUNC(CHmsUiPanel);
	static CHmsUiPanel* Create(const Size & size);

	void SetSwallowEvent(bool bSwallow){ m_bSwallowEvent = bSwallow; }
protected:
	virtual bool Init() override;

	virtual bool OnPressed(const Vec2 & posOrigin);

private:
	bool	m_bSwallowEvent;
};

NS_HMS_END