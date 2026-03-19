#include "HmsSVSFrame.h"
#include "render/HmsTextureCache.h"
#include "display/HmsImageNode.h"
#include "display/HmsStretchImageNode.h"
#include "math/HmsMathBase.h"
#include "HmsGlobal.h"

USING_NS_HMS;

HmsSVSFrame * HmsSVSFrame::Create(const HmsAviPf::Size & size)
{
	HmsSVSFrame *ret = new HmsSVSFrame();
	if (ret && ret->Init(size))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

HmsSVSFrame::HmsSVSFrame()
{
	m_nCameraFlag = (uint32_t)CameraFlag::DEFAULT;
}

HmsSVSFrame::~HmsSVSFrame()
{

}

bool HmsSVSFrame::Init(const HmsAviPf::Size & size)
{
	if (!CHmsNode::Init())
	{
		return false;
	}

	auto & fboSize = size;
	auto fbo = FrameBuffer::create(1, fboSize.width, fboSize.height);
	auto rt = RenderTarget::create(fboSize.width, fboSize.height);
	auto rtDS = RenderTargetDepthStencil::create(fboSize.width, fboSize.height);
	fbo->attachRenderTarget(rt);
	fbo->attachDepthStencilTarget(rtDS);	
	fbo->setClearColor(Color4F(0.68f, 0.88f, 1.00000000f, 1.0));	
	m_frameBuffer = fbo;
	
	auto camera = Camera::create();
	camera->setCameraFlag(CameraFlag(m_nCameraFlag));
	camera->setFrameBufferObject(fbo);
	camera->setDepth(-1);
	m_camera = camera;
	
	m_frameRoot = CHmsNode::Create();
	m_frameRoot->SetCameraMask(m_nCameraFlag);
	m_camera->setSpecificRenderRoot(m_frameRoot);

	m_svsMgr = HmsSvsMgr::Create();
    m_svsMgr->SetSvsDrawSize(size);
	AddSvsNode(m_svsMgr);

	SetCameraMask(m_nCameraFlag);

	return true;
}

HmsAviPf::Texture2D * HmsSVSFrame::GetTexture()
{
	HmsAviPf::Texture2D * tex = nullptr;

	if (m_frameBuffer)
	{
		tex = m_frameBuffer->getRenderTarget()->getTexture();
	}

	return tex;
}

HmsAviPf::CHmsImageNode * HmsSVSFrame::CreateImageFrameNode()
{
	HmsAviPf::CHmsImageNode * node = nullptr;

	if (m_frameBuffer)
	{
		node = CHmsImageNode::CreateWithTexture(m_frameBuffer->getRenderTarget()->getTexture());
	}
	return node;
}

HmsSVSViewer * HmsSVSFrame::CreateSVSViewer()
{
	return HmsSVSViewer::Create(m_frameBuffer->getRenderTarget()->getTexture());
}

void HmsSVSFrame::AddSvsNode(HmsAviPf::CHmsNode * node)
{
	if (node)
	{
		node->SetCameraMask(m_nCameraFlag);

		m_frameRoot->AddChild(node);
	}
}

void HmsSVSFrame::ResetFrameSize(const HmsAviPf::Size & size)
{
	if (m_svsMgr)
	{
		m_svsMgr->SetDyamicSize(size);
	}
}

void HmsSVSFrame::OnEnter()
{
	if (m_frameRoot)
	{
		m_frameRoot->OnEnter();
	}

	if (m_camera)
	{
		auto screen = GetScreen();
		m_camera->setScreen(screen);
		m_camera->OnEnter();
	}

	m_bRunning = true;
}

void HmsSVSFrame::OnExit()
{
	if (m_frameRoot)
	{
		m_frameRoot->OnExit();
	}

	if (m_camera)
	{
		m_camera->OnExit();
	}

	m_bRunning = false;
}

HmsSVSViewer * HmsSVSViewer::Create(Texture2D *texture)
{
	HmsSVSViewer *ret = new HmsSVSViewer();
	if (ret && ret->InitWithTexture(texture))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

HmsSVSViewer::HmsSVSViewer() : HmsUiEventInterface(this)
{

}
