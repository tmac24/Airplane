#pragma once
#include "HmsAviPf.h"
#include "HmsResizeableLayer.h"
#include "ui/HmsUiEventInterface.h"

class CHmsAvionicsDisplayLayer : public CHmsResizeableLayer, public HmsAviPf::HmsUiEventInterface
{
public:
	CHmsAvionicsDisplayLayer();
	~CHmsAvionicsDisplayLayer();

	CREATE_FUNC(CHmsAvionicsDisplayLayer);

	virtual bool Init() override;

};

