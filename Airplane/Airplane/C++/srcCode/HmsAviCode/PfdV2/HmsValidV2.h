#pragma once
#include "HmsAviPf.h"
#include "display/HmsDrawNode.h"

class CHmsValidV2 : public HmsAviPf::CHmsNode
{
public:
	CHmsValidV2();
	~CHmsValidV2();

	virtual bool Init() override;
	
	virtual HmsAviPf::CHmsNode* GenValid(HmsAviPf::Size size);

	virtual bool ValidSwitch(bool bValid);

protected:
	HmsAviPf::Color4F brushColor;
	HmsAviPf::Color4B borderColor;
	HmsAviPf::Color4B crossColor;

	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>		 m_pUnValid;

	bool    m_bUnValid;
};

