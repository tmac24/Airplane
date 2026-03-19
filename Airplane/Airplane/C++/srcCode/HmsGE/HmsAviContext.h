#pragma once
#include "platform.h"
#include "HmsPlatformConfig.h"
#include <string>
#if HMS_TARGET_PLATFORM == HMS_PLATFORM_VXWORKS
#include "HmsVxEventMgr.h"
#endif
namespace HmsAviPf
{
	class CHmsAviDisplayUnit;
	class CHmsScreen;
}

struct AviContextCtrlWold 
{
	bool bMouseToTouch;
	bool bMouseClick;

	AviContextCtrlWold()
	{
		bMouseToTouch = false;
		bMouseClick = false;
	}
};

class CHmsTouchMgr;

class CHmsAviContext
{
public:
	CHmsAviContext();
	virtual ~CHmsAviContext();

	ESContext * GetEsContext(){ return &m_esContext; }
	HmsAviPf::CHmsAviDisplayUnit * GetDisplayUnit(){ return m_pDU; }

	virtual void InitFrame();

	virtual void InitScreen();

	virtual void DrawScreen(float fDelta);

	virtual void BeforeRuning();

	virtual void MainLoop();

	virtual void OnTouchBegin(unsigned int nCnt, struct EsTouchPoint * touchs);
	virtual void OnTouchMove(unsigned int nCnt, struct EsTouchPoint * touchs);
	virtual void OnTouchEnd(unsigned int nCnt, struct EsTouchPoint * touchs);
	void OnWindowMove(int xPos, int yPos);

	virtual void OnMouseDownFunc(unsigned int, unsigned int, unsigned int);					//mouse id, x, y
	virtual void OnMouseUpFunc(unsigned int, unsigned int, unsigned int);					//mouse id, x, y
	virtual void OnMouseMoveFunc(unsigned int, unsigned int);								//x, y
	virtual void OnMouseWheelFunc(unsigned int, unsigned int, unsigned int);				//x,y,wheelDelta
	virtual void OnMouseWheelHorFunc(unsigned int, unsigned int, unsigned int);				//x,y,wheelDelta

	virtual void OnKeyBtnChange(unsigned int nBtnID, unsigned int nBtnState);
	virtual void OnKeyKnobChange(unsigned int nKnobID, float fPosRatio);

	void BeginInit();
	void EndInit();

	void SwapScreenBuffers();

public:
	void SetWindowsTitle(const char * strTitle);
	void SetGlFlags(unsigned int flags);

protected:
	void SetScreen(HmsAviPf::CHmsScreen * pScreen);
	void SetNextScreen(HmsAviPf::CHmsScreen * pScreen);

private:
	void InitEsContext();

private:
	ESContext			m_esContext;
	AviContextCtrlWold	m_contextCtrlWold;
	int					m_nWinPosX;
	int					m_nWinPosY;
	int 				m_nWinSizeWidth;
	int 				m_nWinSizeHeight;
	std::string			m_strWinTitle;			//窗口名称
	unsigned int		m_flagsWinGL;			//GL窗体属性

	HmsAviPf::CHmsAviDisplayUnit *	 m_pDU;
	CHmsTouchMgr *					 m_pTouchMgr;

protected:
#if HMS_TARGET_PLATFORM == HMS_PLATFORM_VXWORKS
	CHmsVxEventMgr					 m_mgrVxEvent;
#endif

	friend class CHmsAvionicsApp;
};

