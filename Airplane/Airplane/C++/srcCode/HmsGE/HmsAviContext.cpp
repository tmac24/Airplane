#include "HmsAviContext.h"
#include "HmsAviDisplayUnit.h"
#include "base/HmsCamera.h"
#include "base/HmsEventTouch.h"
#include "HmsTouchMgr.h"
#include "base/HmsScreen.h"
#include "HmsGlobal.h"
#include "HmsPlatform/platform.h"
#include "HmsLog.h"

USING_NS_HMS;

extern "C" void HmsPrint(const char * strFormat, ...)
{
#if 0
	va_list _ArgList;
	va_start(_ArgList, strFormat);
	char strTemp[1024] = { 0 };
	int nStrRetLen = vsprintf(strTemp, strFormat, _ArgList);
	va_end(_ArgList);
	printf("%s\n", strTemp);

    if (nStrRetLen >= HMS_ARRAYSIZE(strTemp))
    {
        HMSLOGERROR("strLen=%d, %s", nStrRetLen, strFormat);
    }
#else
	va_list _ArgList;
	va_start(_ArgList, strFormat);
	vprintf(strFormat, _ArgList);
	va_end(_ArgList);
#endif


}

void HmsEsDraw(ESContext *esContext, float fDelta)
{
	CHmsAviContext * aviContext = (CHmsAviContext*)esContext->userData;
	if (aviContext)
	{
		aviContext->DrawScreen(fDelta);
		//eglSwapBuffers(esContext->eglDisplay, esContext->eglSurface);
		//esSwapBuffers(esContext);
	}
}

void HmsEsUpdate(ESContext * esContext, float fDelta)
{
	CHmsAviContext * aviContext = (CHmsAviContext*)esContext->userData;
	if (aviContext)
	{
		//fDelta = 0;
		aviContext->DrawScreen(fDelta);
//#if (HMS_TARGET_PLATFORM != HMS_PLATFORM_ANDROID)
//		eglSwapBuffers(esContext->eglDisplay, esContext->eglSurface);
//#endif
	}
}

void HmsEsWindowMove(ESContext * esContext, int xpos, int ypos)
{
	CHmsAviContext * aviContext = (CHmsAviContext*)esContext->userData;
	if (aviContext)
		aviContext->OnWindowMove(xpos, ypos);
}

void HmsEsTouchBegin(ESContext * esContext, unsigned int nCnt, struct EsTouchPoint * touchs)
{
	CHmsAviContext * aviContext = (CHmsAviContext*)esContext->userData;
	if (aviContext)
		aviContext->OnTouchBegin(nCnt, touchs);
}

void HmsEsTouchMove(ESContext * esContext, unsigned int nCnt, struct EsTouchPoint * touchs)
{
	CHmsAviContext * aviContext = (CHmsAviContext*)esContext->userData;
	if (aviContext)
		aviContext->OnTouchMove(nCnt, touchs);
}

void HmsEsTouchEnd(ESContext * esContext, unsigned int nCnt, struct EsTouchPoint * touchs)
{
	CHmsAviContext * aviContext = (CHmsAviContext*)esContext->userData;
	if (aviContext)
		aviContext->OnTouchEnd(nCnt, touchs);
}


extern "C" void HmsEsTouchBegin_C(ESContext * esContext, unsigned int nCnt, struct EsTouchPoint * touchs)
{
	HmsEsTouchBegin(esContext, nCnt, touchs);
}

extern "C" void HmsEsTouchMove_C(ESContext * esContext, unsigned int nCnt, struct EsTouchPoint * touchs)
{
	HmsEsTouchMove(esContext, nCnt, touchs);
}

extern "C" void HmsEsTouchEnd_C(ESContext * esContext, unsigned int nCnt, struct EsTouchPoint * touchs)
{
	HmsEsTouchEnd(esContext, nCnt, touchs);
}



//mouse id, x, y
void HmsEsMouseDownFunc(ESContext * esContext, unsigned int btnID, unsigned int x, unsigned int y)
{
	CHmsAviContext * aviContext = (CHmsAviContext*)esContext->userData;
	if (aviContext)
		aviContext->OnMouseDownFunc(btnID, x, y);
}

//mouse id, x, y
void HmsEsMouseUpFunc(ESContext * esContext, unsigned int btnID, unsigned int x, unsigned int y)
{
	CHmsAviContext * aviContext = (CHmsAviContext*)esContext->userData;
	if (aviContext)
		aviContext->OnMouseUpFunc(btnID, x, y);
}

//x, y
void HmsEsMouseMoveFunc(ESContext * esContext, unsigned int x, unsigned int y)
{
	CHmsAviContext * aviContext = (CHmsAviContext*)esContext->userData;
	if (aviContext)
		aviContext->OnMouseMoveFunc(x, y);
}

//x,y,wheelDelta
void HmsEsMouseWheelFunc(ESContext * esContext, unsigned int x, unsigned int y, unsigned int wheelDelta)
{
	CHmsAviContext * aviContext = (CHmsAviContext*)esContext->userData;
	if (aviContext)
		aviContext->OnMouseWheelFunc(x, y, wheelDelta);
}

//x,y,wheelDelta
void HmsEsMouseWheelHorFunc(ESContext * esContext, unsigned int x, unsigned int y, unsigned int wheelDelta)
{
	CHmsAviContext * aviContext = (CHmsAviContext*)esContext->userData;
	if (aviContext)
		aviContext->OnMouseWheelHorFunc(x, y, wheelDelta);
}


CHmsAviContext::CHmsAviContext()
	: m_pDU(nullptr)
	, m_pTouchMgr(nullptr)
{
	memset(&m_esContext, 0, sizeof(m_esContext));
	m_esContext.userData = this;

	m_esContext.surfWidth = 1920;
	m_esContext.surfHeight = 1080;
	m_esContext.fUpdateInterval = 1 / 60.0f;

	m_flagsWinGL = ES_WINDOW_RGB | ES_WINDOW_ALPHA | ES_WINDOW_DEPTH | ES_WINDOW_STENCIL | ES_WINDOW_MULTISAMPLE;

	m_pDU = HmsAviPf::CHmsAviDisplayUnit::GetInstance();
	m_pTouchMgr = new CHmsTouchMgr;

    m_pDU->SetESContext(&m_esContext);

    m_nWinPosX = 0;
    m_nWinPosY = 0;
	m_nWinSizeWidth = m_esContext.surfWidth;
	m_nWinSizeHeight = m_esContext.surfHeight;

	InitEsContext();
}

void CHmsAviContext::InitEsContext()
{
	auto esContext = &m_esContext;
#if (HMS_TARGET_PLATFORM != HMS_PLATFORM_ANDROID)
	esRegisterDrawFunc(esContext, HmsEsDraw);
	esRegisterUpdateFunc(esContext, HmsEsUpdate);
#endif
	esContext->windowMove = HmsEsWindowMove;
	esContext->touchBeginFunc = HmsEsTouchBegin;
	esContext->touchMoveFunc = HmsEsTouchMove;
	esContext->touchEndFunc = HmsEsTouchEnd;

	esContext->mouseDownFunc = HmsEsMouseDownFunc;
	esContext->mouseUpFunc = HmsEsMouseUpFunc;
	esContext->mouseMoveFunc = HmsEsMouseMoveFunc;
	esContext->mouseWheelFunc = HmsEsMouseWheelFunc;
	esContext->mouseWheelHorFunc = HmsEsMouseWheelHorFunc;
}


CHmsAviContext::~CHmsAviContext()
{
	m_esContext.userData = nullptr;
	m_pDU = nullptr;

	if (m_pTouchMgr)
	{
		delete m_pTouchMgr;
		m_pTouchMgr = nullptr;
	}

}

void CHmsAviContext::BeginInit()
{	
	HmsAviPf::Viewport vp;
	vp._left = 0;
	vp._width = (float)m_esContext.viewportWidth;
	vp._bottom = 0;
	vp._height = (float)m_esContext.viewportHeight;
	HmsAviPf::Camera::setDefaultViewport(vp);

	m_pDU->SetViewSize(HmsAviPf::Size((float)m_esContext.surfWidth, (float)m_esContext.surfHeight));
	m_pDU->InitMatrixStack();
	
	m_nWinSizeWidth = m_esContext.surfWidth;
	m_nWinSizeHeight = m_esContext.surfHeight;
}

void CHmsAviContext::SetWindowsTitle(const char * strTitle)
{
	m_strWinTitle = strTitle;
}

void CHmsAviContext::SetGlFlags(unsigned int flags)
{
	m_flagsWinGL = flags;
}

void CHmsAviContext::DrawScreen(float fDelta)
{
	m_pDU->DrawScreen(fDelta);
}

void CHmsAviContext::InitScreen()
{

}

void CHmsAviContext::MainLoop()
{

}

void CHmsAviContext::SetScreen(HmsAviPf::CHmsScreen * pScreen)
{
	m_pDU->RunScreen(pScreen);
}

void CHmsAviContext::SetNextScreen(HmsAviPf::CHmsScreen * pScreen)
{
	m_pDU->SetNextScreen(pScreen);
}

void CHmsAviContext::EndInit()
{
	m_pDU->InitDisplayUnit();

	BeforeRuning();
}

void CHmsAviContext::SwapScreenBuffers()
{
#if (HMS_TARGET_PLATFORM != HMS_PLATFORM_ANDROID) && (HMS_TARGET_PLATFORM != HMS_PLATFORM_IOS)
    esSwapBuffers(&m_esContext);
#endif
}

void CHmsAviContext::BeforeRuning()
{

}

void CHmsAviContext::InitFrame()
{
	m_esContext.surfXPos = 0;
	m_esContext.surfYPos = 0;
	m_esContext.surfWidth = 1920;
	m_esContext.surfHeight = 1080;

	m_flagsWinGL = ES_WINDOW_RGB | ES_WINDOW_ALPHA | ES_WINDOW_DEPTH | ES_WINDOW_STENCIL | ES_WINDOW_MULTISAMPLE;
}

void CHmsAviContext::OnTouchBegin(unsigned int nCnt, struct EsTouchPoint * touchs)
{
	int nID = 0;
	float x = 0;
	int y = 0;

	EventTouch et;

	for (auto i = 0u; i < nCnt; i++)
	{
		nID = touchs[i].nId;
		x = touchs[i].fX - m_nWinPosX;
		y = touchs[i].fY - m_nWinPosY;
		
		auto touch = m_pTouchMgr->GetOrCreateTouch(nID);
		touch->SetTouchInfo(x, y);

		et._touches.push_back(touch);
	}

	et._eventCode = EventTouch::EventCode::BEGAN;
	auto dispatcher = m_pDU->GetEventDispatcher();
	if (dispatcher)
	{
		dispatcher->DispatchEvent(&et);
	}
}

void CHmsAviContext::OnTouchMove(unsigned int nCnt, struct EsTouchPoint * touchs)
{
	int nID = 0;
	float x = 0;
	int y = 0;

	EventTouch et;

	for (auto i = 0u; i < nCnt; i++)
	{
		nID = touchs[i].nId;
		x = touchs[i].fX - m_nWinPosX;
		y = touchs[i].fY - m_nWinPosY;

		auto touch = m_pTouchMgr->GetOrCreateTouch(nID);
		touch->SetTouchInfo(x, y);

		et._touches.push_back(touch);
	}

	et._eventCode = EventTouch::EventCode::MOVED;
	auto dispatcher = m_pDU->GetEventDispatcher();
	if (dispatcher && et._touches.size())
	{
		dispatcher->DispatchEvent(&et);
	}
}

void CHmsAviContext::OnTouchEnd(unsigned int nCnt, struct EsTouchPoint * touchs)
{
	int nID = 0;
	float x = 0;
	int y = 0;

	EventTouch et;
	
	for (auto i = 0u; i < nCnt; i++)
	{
		nID = touchs[i].nId;
		x = touchs[i].fX - m_nWinPosX;
		y = touchs[i].fY - m_nWinPosY;

		auto touch = m_pTouchMgr->GetAndRemoveTouch(nID);
		touch->SetTouchInfo(x, y);

		et._touches.push_back(touch);
	}

	et._eventCode = EventTouch::EventCode::ENDED;
	auto dispatcher = m_pDU->GetEventDispatcher();
	if (dispatcher && et._touches.size())
	{
		dispatcher->DispatchEvent(&et);
	}

	//must release the touch info after the dispatcher
	for (auto &c:et._touches)
	{
		c->release();
	}
}

void CHmsAviContext::OnWindowMove(int xPos, int yPos)
{
	m_nWinPosX = xPos;
	m_nWinPosY = yPos;
}

void CHmsAviContext::OnMouseDownFunc(unsigned int btnID, unsigned int x, unsigned int y)
{
	int nID = 0;
	EventTouch et;


	auto touch = m_pTouchMgr->GetOrCreateTouch(nID);
	touch->ReSetStartCapture();
	touch->SetTouchInfo(x, y);
	et._touches.push_back(touch);

	et._eventCode = EventTouch::EventCode::BEGAN;
	auto dispatcher = m_pDU->GetEventDispatcher();
	if (dispatcher)
	{
		dispatcher->DispatchEvent(&et);
	}

	m_contextCtrlWold.bMouseClick = true;
}

void CHmsAviContext::OnMouseUpFunc(unsigned int btnID, unsigned int x, unsigned int y)
{
	int nID = 0;

	EventTouch et;

	auto touch = m_pTouchMgr->GetOrCreateTouch(nID);
	touch->SetTouchInfo(x, y);

	et._touches.push_back(touch);

	et._eventCode = EventTouch::EventCode::ENDED;
	auto dispatcher = m_pDU->GetEventDispatcher();
	if (dispatcher && et._touches.size())
	{
		dispatcher->DispatchEvent(&et);
	}

	m_contextCtrlWold.bMouseClick = false;
}

void CHmsAviContext::OnMouseMoveFunc(unsigned int x, unsigned int y)
{
	if (m_contextCtrlWold.bMouseClick)
	{
		int nID = 0;
		EventTouch et;

		auto touch = m_pTouchMgr->GetOrCreateTouch(nID);
		touch->SetTouchInfo(x, y);

		et._touches.push_back(touch);

		et._eventCode = EventTouch::EventCode::MOVED;
		auto dispatcher = m_pDU->GetEventDispatcher();
		if (dispatcher && et._touches.size())
		{
			dispatcher->DispatchEvent(&et);
		}
	}
}

void CHmsAviContext::OnMouseWheelFunc(unsigned int x, unsigned int y, unsigned int wheelDelta)
{

}

void CHmsAviContext::OnMouseWheelHorFunc(unsigned int x, unsigned int y, unsigned int wheelDelta)
{

}

void CHmsAviContext::OnKeyBtnChange(unsigned int nBtnID, unsigned int nBtnState)
{
	EventKeyButton ekb(nBtnID, nBtnState);

	auto dispatcher = m_pDU->GetEventDispatcher();
	if (dispatcher)
	{
		dispatcher->DispatchEvent(&ekb);
	}
}

void CHmsAviContext::OnKeyKnobChange(unsigned int nKnobID, float fPosRatio)
{
	EventKeyKnob ekb(nKnobID, fPosRatio);

	auto dispatcher = m_pDU->GetEventDispatcher();
	if (dispatcher)
	{
		dispatcher->DispatchEvent(&ekb);
	}
}


