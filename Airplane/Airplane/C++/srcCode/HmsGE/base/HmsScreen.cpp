#include "base/HmsScreen.h"
#include "base/HmsCamera.h"
#include "render/HmsRenderer.h"
#include "render/HmsFrameBuffer.h"
#include "HmsAviDisplayUnit.h"

NS_HMS_BEGIN

CHmsScreen * CHmsScreen::Create()
{
	CHmsScreen *ret = new CHmsScreen();
	if (ret && ret->Init())
	{
		ret->autorelease();
		return ret;
	}
	else
	{
		HMS_SAFE_DELETE(ret);
		return nullptr;
	}
}

CHmsScreen * CHmsScreen::CreateWithSize(const Size & size)
{
	CHmsScreen *ret = new  CHmsScreen();
	if (ret && ret->InitWithSize(size))
	{
		ret->autorelease();
		return ret;
	}
	else
	{
		HMS_SAFE_DELETE(ret);
		return nullptr;
	}
}

static bool camera_cmp(const Camera* a, const Camera* b)
{
	return a->getRenderOrder() < b->getRenderOrder();
}

const std::vector<Camera*> & CHmsScreen::GetCameras()
{
	if (m_bDirtyCamera)
	{
		stable_sort(m_vCameras.begin(), m_vCameras.end(), camera_cmp);
		m_bDirtyCamera = false;
	}
	return m_vCameras;
}

void CHmsScreen::Render(Renderer * renderer)
{
	auto du = CHmsAviDisplayUnit::GetInstance();
	const auto & transform = GetNodeToParentTransform();
	int ccc = GetCameras().size();
	for (const auto & c : GetCameras())
	{
		if (!c->IsVisible())
		{
			continue;
		}

		Camera::s_pCameraVisiting = c;

		du->PushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
		du->LoadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION, c->getViewProjectionMatrix());
		c->apply();
		c->clearBackground();
		if (c->m_pRenderRootSpecific)
			c->m_pRenderRootSpecific->Visit(renderer, transform, 0);
		else
			Visit(renderer, transform, 0);
		renderer->render();
		du->PopMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
	}

	Camera::s_pCameraVisiting = nullptr;
	FrameBuffer::applyDefaultFBO();
}

CHmsScreen::CHmsScreen()
	: m_cameraDefault(nullptr)
	, m_bDirtyCamera(true)
{
	m_ignoreAnchorPointForPosition = true;
	SetAnchorPoint(Vec2(0.5f, 0.5f));
	
	m_cameraDefault = Camera::create();
	AddChild(m_cameraDefault);
	Camera::s_pCameraVisiting = nullptr;

#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_VXWORKS)
 	m_screenPointer = CHmsScreenPointer::Create();
	if (m_screenPointer)
	{
		AddChild(m_screenPointer);
		m_screenPointer->SetVisible(false);
	}
#endif

#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_LINUX)
	m_screenPointer = CHmsScreenPointer::Create();
	if (m_screenPointer)
	{
		AddChild(m_screenPointer);
		m_screenPointer->SetVisible(false);
	}
#endif
}

CHmsScreen::~CHmsScreen()
{

}

bool CHmsScreen::Init()
{
	auto size = CHmsAviDisplayUnit::GetInstance()->GetViewSize();
	return InitWithSize(size);
}

bool CHmsScreen::InitWithSize(const Size & size)
{
	return true;
}

NS_HMS_END
