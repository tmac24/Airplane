#pragma once

#include "ui/HmsUiStretchButton.h"
#include "ui/HmsUiEventInterface.h"
#include "display/HmsDrawNode.h"
#include "display/HmsImageNode.h"
#include "../../HmsGlobal.h"
#include <map>
#include "HmsStandardDef.h"

using namespace HmsAviPf;

class CHmsScratchPadColorDlg : public HmsAviPf::CHmsNode, public HmsUiEventInterface
{
public:
	CHmsScratchPadColorDlg();
	~CHmsScratchPadColorDlg();

    static CHmsScratchPadColorDlg* Create(const HmsAviPf::Size & size, float trianglePosY = 550);

    virtual bool Init(const HmsAviPf::Size & windowSize, float trianglePosY);

	typedef std::function<void(int,Color4B)> t_callbackFunc;

	void SetCallback(t_callbackFunc func){ m_callback = func; }

	void SetLineWidthAndColor(int lineWidth, Color4B color);

protected:
	virtual bool OnPressed(const Vec2 & posOrigin);

	virtual void OnMove(const Vec2 & posOrigin);

	virtual void OnReleased(const Vec2 & posOrigin);

private:

	void InitWidget();

	void UpdateLineWidth(const Vec2 & posOrigin);

	void OnButtonColorClick(Color4B color, int buttonIndex);

private:

	t_callbackFunc m_callback;

	RefPtr<CHmsImageNode> m_imageNode;
	RefPtr<CHmsLabel> m_lineWidthLabel;
	bool m_bIsDragImageNode;

	RefPtr<CHmsDrawNode> m_btnSelectDraw;

	Vec2 m_posPressStart;
	Vec2 m_posChildPressStart;
	Vec2 m_imageXRange;
	Vec2 m_imageLogicRange;

	int m_lineWidth;
	Color4B m_lineColor;

	std::map<int, Rect> m_buttonRectMap;
	std::map<int, Color4B> m_buttonColorMap;

    float m_xScale;
    float m_yScale;
};

