#include <HmsAviPf.h>
#include "HmsAdminAreasServer.h"
#include "../HmsGlobal.h"
#include "HmsConfigXMLReader.h"
#include "Language/HmsLanguageMananger.h"
#include "HmsLog.h"
USING_NS_HMS;

CHmsAdminAreasServer::CHmsAdminAreasServer()
	: m_pThread(nullptr)
	, m_bThreadRun(true)
	, m_bImageUpdate(false)
	, m_nThreadLoadingUse(0)
	, m_nCurLayer(0)
{
	
}

CHmsAdminAreasServer::~CHmsAdminAreasServer()
{
	End();
}

CHmsAdminAreasServer * CHmsAdminAreasServer::GetInstance()
{
	static CHmsAdminAreasServer * s_pAdminAreasServer = nullptr;
	if (nullptr == s_pAdminAreasServer)
	{
		s_pAdminAreasServer = new CHmsAdminAreasServer;
		s_pAdminAreasServer->Init();
		s_pAdminAreasServer->Start();
	}
	return s_pAdminAreasServer;
}

void CHmsAdminAreasServer::Init()
{
	auto config = CHmsConfigXMLReader::GetSingleConfig();
	std::vector<HmsAdminAreaDbInfo> vDbInfo;
	

	if (config->GetAdminAreasDbInfo(vDbInfo))
	{
		for (auto c:vDbInfo)
		{
			if (c.bUsed)
			{
                if (c.strDataType == "LandAdmin")
                {
                    if (m_db.ResetDatabase(c.strDbPath))
                    {
                        //get the china land
                        m_db.GetChinaLandAdmin(m_dataChinaAdmin);
                        //get the china text
                    }
                }
				else if (m_db.ResetDatabase(c.strDbPath))
				{
					HmsAdminAreaDescribe describe;
					describe.strName = c.strName;
					describe.strDbPath = c.strDbPath;
					m_db.FillAdminAreasDescribe(describe);
					m_mapAdminAreaDescribe[c.strName] = describe;
				}
			}
		}
	}	
}

void CHmsAdminAreasServer::Start()
{
	if (nullptr == m_pThread)
	{
		m_bThreadRun = true;
		m_pThread = new std::thread(&CHmsAdminAreasServer::ThreadRunning, this);
	}
}

void CHmsAdminAreasServer::End()
{
	m_bThreadRun = false;
	if (m_pThread)
	{
		m_pThread->join();
		delete m_pThread;
		m_pThread = nullptr;
	}
}

void CHmsAdminAreasServer::ThreadRunning()
{
	while (m_bThreadRun)
	{
		bool bDoLoad = false;
		TerrainTiffIndex gti;

		while (0 == m_nThreadLoadingUse)
		{
			//如果暂停加载，线程暂停
#ifdef  __vxworks
			sleep(10);
#else
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
#endif
			if (!m_bThreadRun)
			{
				break;;
			}
		}

		while (m_qLoadAction.size())
		{
			m_mutexThreadLoading.lock();			
			auto action = m_qLoadAction.front();
			m_qLoadAction.pop();
			m_mutexThreadLoading.unlock();
			if (action)
			{
				action();
			}			
			SetUnusedLoading();
		}
	}
}



int CHmsAdminAreasServer::FixAdminAreaDescribeLayer(int nLayer, const HmsAdminAreaDescribe & describe)
{
	int nRetLayer = 0;
	
	if (nLayer > 1 && (describe.nLayerCnt == 3))
	{
		nLayer -= 1;
	}

	if (nLayer >=0 && nLayer < describe.nLayerCnt)
	{
		nRetLayer = nLayer;
	}
	else if ( describe.nLayerCnt > 1)
	{
		nRetLayer = describe.nLayerCnt - 1;
	}

	return nRetLayer;
}

bool CHmsAdminAreasServer::GetGeomDataByDescribe(const HmsAdminAreaDescribe & describe, const HmsAviPf::Rect & rect, int nLayer, std::vector<HmsGeomData *> & m_vGeomData)
{
	bool bRet = false;

	//just fix the chinese and english now
	auto GetFixItemName = [](const std::string & strIsoName, char * strNameCHN, char * strNameTWN, char * strNameOther)
	{
		if (CHmsLanguageMananger::GetInstance()->GetLanguageName() == "CHN")
		{
			if (strIsoName == "CHN")
			{
				return strNameCHN;
			}
			else if (strIsoName == "TWN")
			{
				return strNameTWN;
			}
		}
	
		return strNameOther;
	};

	int nFixLayer = FixAdminAreaDescribeLayer(nLayer, describe);
    if ((int)describe.vLayerInfo.size() > nFixLayer)
	{
		auto & info = describe.vLayerInfo.at(nFixLayer);
		ShapeRange sr;
		sr.SetData(rect.getMinX(), rect.getMaxX(), rect.getMinY(), rect.getMaxY(), info);

		char * strItemName = nullptr;
		switch (nFixLayer)
		{
		case 0:
		{
			strItemName = GetFixItemName(info.strIsoName, "NAME_NONLATIN", "NAME_CHINESE", "NAME_ENGLISH");
			m_db.SetMultUsedIndex(0);
			break;
		}
		case 1:
		{
			strItemName = GetFixItemName(info.strIsoName, "NL_NAME_1", "NL_NAME_1", "NAME_1");
			m_db.SetMultUsedIndex(1);
			break;
		}
		case 2:
		{
			strItemName = GetFixItemName(info.strIsoName, "NL_NAME_2", "NL_NAME_2", "NAME_2");
			m_db.SetMultUsedIndex(1);
			break;
		}
		case 3:
		{
			strItemName = GetFixItemName(info.strIsoName, "NL_NAME_3", "NL_NAME_3", "NAME_3");
			m_db.SetMultUsedIndex(0);
			break;
		}
		default:
			return bRet;
			break;
		}

		m_db.ResetDatabase(describe.strDbPath);
		bRet = m_db.GetRangeGeomData(info.strTableName.c_str(), strItemName, sr, m_vGeomData);
	}

	return bRet;
}

bool CHmsAdminAreasServer::GetGeomData(const HmsAviPf::Rect & rect, int nLayer, std::vector<HmsGeomData *> & m_vGeomData)
{
	bool bRet = false;

	for (auto c:m_mapAdminAreaDescribe)
	{
		auto & describe = c.second;
		if (describe.boundaryRect.intersectsRect(rect))
		{
			if (GetGeomDataByDescribe(describe, rect, nLayer, m_vGeomData))
			{
				bRet = true;
			}
		}
	}

	return bRet;
}

void CHmsAdminAreasServer::OnLoadData(const HmsGeomBoundary & boundary, HmsAdminAreasServerDataCell * dataCell, const HmsAdminAreasDataCallback & callback)
{
	if (dataCell)
	{
		m_nCurLayer = dataCell->nLayer;
		dataCell->eState = ADMIN_AREAS_DATA_STATE::ActionLoading;
		if (boundary.dLeft > 90 && boundary.dRight < -90)
		{
			Rect rectEast(boundary.dLeft, boundary.dBottom, 180.0 - boundary.dLeft, boundary.dTop - boundary.dBottom);
			Rect rectWest(-180, boundary.dBottom, boundary.dRight + 180.0, boundary.dTop - boundary.dBottom);
			GetGeomData(rectEast, dataCell->nLayer, dataCell->vGeomData);
			GetGeomData(rectWest, dataCell->nLayer, dataCell->vGeomData);
		}
		else
		{
			Rect rect(boundary.dLeft, boundary.dBottom, boundary.dRight - boundary.dLeft, boundary.dTop - boundary.dBottom);
			GetGeomData(rect, dataCell->nLayer, dataCell->vGeomData);
		}
		dataCell->eState = ADMIN_AREAS_DATA_STATE::ActionRespond;
	}

	if (callback)
	{
		HMSLOG("HmsLog: Admin Areas Server callback.ItemSize= %d", dataCell->vGeomData.size());
		callback(dataCell);
	}
}

void CHmsAdminAreasServer::SetUsingLoading()
{
	m_nThreadLoadingUse++;
	if (m_nThreadLoadingUse <= 0)
	{
		m_nThreadLoadingUse = 1;
	}
}

void CHmsAdminAreasServer::SetUnusedLoading()
{
	m_nThreadLoadingUse--;
	if (m_nThreadLoadingUse < 0)
	{
		m_nThreadLoadingUse = 0;
	}
}

void CHmsAdminAreasServer::GetDataByAsyn(const HmsGeomBoundary & boundary, HmsAdminAreasServerDataCell * dataCell, const HmsAdminAreasDataCallback & callback)
{
	if (dataCell)
	{
		m_mutexThreadLoading.lock();
		m_qLoadAction.push(HMS_CALLBACK_0(CHmsAdminAreasServer::OnLoadData, this, boundary, dataCell, callback));
		m_mutexThreadLoading.unlock();
		SetUsingLoading();
	}
}








