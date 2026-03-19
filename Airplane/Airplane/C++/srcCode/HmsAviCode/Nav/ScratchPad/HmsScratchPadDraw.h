#pragma once

#include "ui/HmsUiImagePanel.h"
#include "display/HmsDrawNode.h"
#include "ui/HmsUiEventInterface.h"
#include "ui/HmsUiImagePanel.h"
#include "display/HmsBezier.h"
#include "math/HmsMath.h"

#include "../HmsRouteLineDrawNode.h"
#include "HmsScratchPadSql.h"


using namespace HmsAviPf;

class CHmsPadDrawNode : public CHmsDrawNode
{
public:
	CHmsPadDrawNode();
	~CHmsPadDrawNode();

	CREATE_FUNC(CHmsPadDrawNode);

	void DrawLines(Vec2 * vertexs, int nCnt, float fLineWidth, const Color4B & color = Color4B::WHITE);

private:

};


class CHmsScratchPadDraw : public CHmsUiPanel
{
public:
	CHmsScratchPadDraw();
	~CHmsScratchPadDraw();

	CREATE_FUNC(CHmsScratchPadDraw);

	void ZoomIn();
	void ZoomOut();
	void ZoomByScale(float scale);
	void ClearDrawContent();
	void Undo();
	void Redo();

	void SetEnableDraw(bool b);

	void SaveDataToDb();

	void SetScratchPadInfoStu(const ScratchPadInfoStu &stu);

	void SetLineWidthColor(int lineWidth, Color4B color);

	void SetDealEventAsButton(bool b);

	typedef std::function<void(ScratchPadInfoStu stu)> t_callbackFunc;
	void SetCallbackFunc(t_callbackFunc func);

	void GetCurLineWidthAndColor(int &lineWidth, Color4B &color);

	void SetScratchPadBgByPath(const std::string &path);

protected:
	virtual bool Init();

	virtual bool OnPressed(const Vec2 & posOrigin);

	virtual void OnMove(const Vec2 & posOrigin);

	virtual void OnReleased(const Vec2 & posOrigin);

	virtual bool OnMutiPressed(const std::vector<UiEventPoint> &pos) override;
	virtual void OnMutiMove(const std::vector<UiEventPoint> &pos) override;
	virtual void OnMutiReleased(const std::vector<UiEventPoint> &pos) override;

private:

	void RedrawLines();

	void InsertBezier();

	void EdgeLimit(Vec2 &pos);

	void SetScratchPadBgType(ScratchPadBgType bgType);
    void PushToTrack(const Vec2 &pos);

private:
	std::vector<Vec2> m_vecTrackPoints;
	std::vector< OneLineStu > m_vecUndoRedo;

	RefPtr<CHmsImageNode> m_pImageNode;
	RefPtr<CHmsPadDrawNode> m_pDrawNode;

	Vec2 m_fixPoint;

	ScratchPadInfoStu m_infoStu;

	bool m_bDealEventAsButton;
	t_callbackFunc m_callbackFunc;

	bool m_bNeedSave;

	std::map<int, UiEventPoint> m_mapMultiPoints;

	bool m_bEnableDraw;
};
