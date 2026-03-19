#include "HmsAvionicsApp.h"
#include "HmsLog.h"

#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_VXWORKS)
#include "platform_vxworks.h"
#endif

#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_LINUX)
#include "platform_linux.h"
#endif

#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_WIN32)
#include "platform_win.inl"
#endif

CHmsAvionicsApp::CHmsAvionicsApp(CHmsAviContext * pContext)
	: m_pContext(pContext)
{
	if (nullptr == m_pContext)
	{
		m_pContext = new CHmsAviContext();
	}
}


CHmsAvionicsApp::~CHmsAvionicsApp()
{
	if (m_pContext)
	{
#if (HMS_TARGET_PLATFORM != HMS_PLATFORM_ANDROID) && (HMS_TARGET_PLATFORM != HMS_PLATFORM_IOS)
		esRelaseWindowContext(m_pContext->GetEsContext());
#endif
		delete m_pContext;
		m_pContext;
	}
}

bool CHmsAvionicsApp::Init()
{
	bool bRet = false;
	if (m_pContext)
	{
		m_pContext->InitFrame();
		ESContext * esContext = m_pContext->GetEsContext();
		m_pContext->BeginInit();
#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_ANDROID) || (HMS_TARGET_PLATFORM == HMS_PLATFORM_IOS)
		bRet = true;
#else
        bRet = esCreateWindow(esContext, m_pContext->m_strWinTitle.c_str(), m_pContext->m_flagsWinGL) ? true : false;
#endif
	}
	return bRet;
}

int CHmsAvionicsApp::Exec()
{
	if (m_pContext)
	{
		m_pContext->InitScreen();
		m_pContext->EndInit();
		HMSLOG("end App init");
#if (HMS_TARGET_PLATFORM != HMS_PLATFORM_ANDROID) && (HMS_TARGET_PLATFORM != HMS_PLATFORM_IOS)
		WinLoop(m_pContext->GetEsContext());
#endif
	}

	return 1;
}
