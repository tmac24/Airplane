#pragma once
#include "base/RefPtr.h"
#include "base/HmsNode.h"
#include "base/HmsCamera.h"
#include "render/HmsTexture2D.h"
#include "display/HmsImageNode.h"
#include "render/HmsFrameBuffer.h"
#include "ui/HmsUiEventInterface.h"
#include "HmsSvsMgr.h"

class HmsSVSFrame;

class HmsSVSViewer : public HmsAviPf::CHmsImageNode, public HmsAviPf::HmsUiEventInterface
{
public:
	HmsSVSViewer();

	static HmsSVSViewer * Create(HmsAviPf::Texture2D *texture);	
};


class HmsSVSFrame : public HmsAviPf::CHmsNode
{
public:
	HmsSVSFrame();
	~HmsSVSFrame();

	static HmsSVSFrame * Create(const HmsAviPf::Size & size);

	//get the texture of the frame
	HmsAviPf::Texture2D * GetTexture();

	//create a image frame node to show the frame
	HmsAviPf::CHmsImageNode * CreateImageFrameNode();

	HmsSVSViewer * CreateSVSViewer();

	void AddSvsNode(HmsAviPf::CHmsNode * node);

	void ResetFrameSize(const HmsAviPf::Size & size);

	virtual void OnEnter() override;
	virtual void OnExit() override;

protected:
	bool Init(const HmsAviPf::Size & size);

private:
	HmsAviPf::RefPtr<HmsAviPf::FrameBuffer>			m_frameBuffer;
	HmsAviPf::RefPtr<HmsAviPf::Camera>				m_camera;
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>			m_frameRoot;	
	HmsAviPf::RefPtr<HmsSvsMgr>						m_svsMgr;
	uint32_t										m_nCameraFlag;
};