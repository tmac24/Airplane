#pragma once
#include "HmsScreenLayout.h"
#include "HmsMultLayer.h"
#include "base/HmsTimer.h"


class CHmsScreenLayer : public HmsAviPf::CHmsNode
{
public:
	CHmsScreenLayer();
	~CHmsScreenLayer();

	CREATE_FUNC(CHmsScreenLayer);

	virtual bool Init() override;

	void ScreenLeftRightChange();

	HmsAviPf::CHmsNode * GetRootNode(){ return m_nodeRoot.get(); }
	HmsAviPf::CHmsNode * GetContentNode(){ return m_nodeContent.get(); }

	//the bottom of the soft key is up and down
	void OnSoftKeysChange(int nBtnID, int nBtnState);

protected:
	//设置右边菜单屏幕
	void SetRightScreenMenu(CHmsScreenLayout * node);
	//设置左边菜单屏幕
	void SetLeftScreenMenu(CHmsScreenLayout * node);
	//设置全菜单屏幕
	void SetFullScreenMenu(CHmsScreenLayout * node);
	//将显示节点设置为
	void SetScreenSmall(CHmsScreenLayout * node, bool bLeft, int nIndex);
	//设置半屏
	void SetScreenMiddle(CHmsScreenLayout * node, bool bLeft, int nIndex);
	//设置全屏
	void SetScreenNormal(CHmsScreenLayout * node, bool bLeft);

    void SetScreenFull(CHmsScreenLayout * node);

	void SetScreenNormalMid(CHmsScreenLayout * node, bool bLeft);
	//设置跨屏
	void SetScreenStride(CHmsScreenLayout * node, int nIndex);

	void AddScreenLayoutToScene(CHmsScreenLayout * node);
	void AddScreenLayoutToMenu(CHmsScreenLayout * node);

	CHmsMultLayer * CreateMultScreen(Hms_Screen_Layout_Type eType);

	void CleanOldScreenContent();

	void ScreenChangeStart();
	void ScreenChangeEnd();
	void ScreenChangeOver();

private:
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>		m_nodeContent;			//is the child of root
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>		m_nodeRoot;				//the root node of the layer

	bool								m_bCurrentStride;		//record is stride screen
	CHmsTime							m_timeChangeMenu;		//record the last time of menu change

	int 								m_nScreenChangeBack;

	friend class CHmsScreenMgr;
};

