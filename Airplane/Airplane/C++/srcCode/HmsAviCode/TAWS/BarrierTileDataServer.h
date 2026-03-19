#pragma once
#include "HmsAviMacros.h"
#include <iostream>
#include <thread>

NS_HMS_BEGIN

class BarrierTileDataServer
{
public:
	BarrierTileDataServer();
	~BarrierTileDataServer();
	virtual bool StartServer();
	virtual bool StopServer();
	virtual void Run();
private:
	bool m_working;
	std::thread *m_pTask;

};

NS_HMS_END