#include "BarrierTileDataServer.h"

NS_HMS_BEGIN



BarrierTileDataServer::BarrierTileDataServer()
: m_working(false)
{

}


bool HmsAviPf::BarrierTileDataServer::StartServer()
{
	if (m_working)
	{
		std::cout << "Barrier Tile Data Server is running." << std::endl;
		return true;
	}	

	m_working = true;
	m_pTask = new std::thread(&BarrierTileDataServer::Run, this);

	return true;
}

void HmsAviPf::BarrierTileDataServer::Run()
{
	while (m_working)
	{
		
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}

bool HmsAviPf::BarrierTileDataServer::StopServer()
{
	m_working = false;
	return true;
}

HmsAviPf::BarrierTileDataServer::~BarrierTileDataServer()
{
	if (m_pTask)
	{
		delete(m_pTask);
		m_pTask = nullptr;
	}
}

NS_HMS_END