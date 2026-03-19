#include "HmsGlobal.h"
#include "HmsAvionicsDisplayLayer.h"
USING_NS_HMS;

CHmsAvionicsDisplayLayer::CHmsAvionicsDisplayLayer()
	: HmsUiEventInterface(this)
{
}


CHmsAvionicsDisplayLayer::~CHmsAvionicsDisplayLayer()
{

}

bool CHmsAvionicsDisplayLayer::Init()
{
	if (!CHmsNode::Init())
	{
		return false;
	}

	
	//HmsUiEventInterface::RegisterTouchEvent();
	HmsUiEventInterface::RegisterMutiTouchEvent(-1);

	return true;
}
