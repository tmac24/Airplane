#include "HmsDataBus_Old.h"
#include <string>
#include "HmsGlobal.h"


CHmsDataBus_Old::CHmsDataBus_Old()
	: m_nSvsType(1)
{
	memset(&m_data, 0, sizeof(m_data));
	m_data.pos.dLatitude = 30.0f;
	m_data.pos.dLongitude = 110.0f;
	m_data.att.fHeading = 100.0f;

	memset(&m_dataInstruFailed, 0, sizeof(m_dataInstruFailed));
}


CHmsDataBus_Old::~CHmsDataBus_Old()
{
}

void CHmsDataBus_Old::AddAftStatusMsg(const HmsAftStatusMsg & msg)
{
	if (msg.eAftMsgType == AftMsgType::Failed)
	{
		bool bFailed(msg.nMsgStatus ? true : false);
		switch (msg.nMsgID)
		{
		case AFT_STATUS_MSG_INSTU_SPD:
			m_dataInstruFailed.bAirspeed = bFailed; 
			break;
		case AFT_STATUS_MSG_INSTU_ALT:
			m_dataInstruFailed.bAltitude = bFailed;
			break;
		case AFT_STATUS_MSG_INSTU_ATT:
			m_dataInstruFailed.bAttitude = bFailed;
			break;
		case AFT_STATUS_MSG_INSTU_TRN:
			m_dataInstruFailed.bTurnCoord = bFailed;
			break;
		case AFT_STATUS_MSG_INSTU_VS:
			m_dataInstruFailed.bVerticalSpd = bFailed;
			break;
		case AFT_STATUS_MSG_INSTU_HSI:
			m_dataInstruFailed.bHsi = bFailed;
			break;
		default:
			break;
		}
	}
	else
	{
		CHmsCSLocker locker(CHmsGlobal::GetInstance()->GetHmsCS());
		m_qAftStatusMsg.push(msg);
	}
}

bool CHmsDataBus_Old::GetAftStatusMsg(HmsAftStatusMsg & msg)
{
	if (m_qAftStatusMsg.size())
	{
		CHmsCSLocker locker(CHmsGlobal::GetInstance()->GetHmsCS());
		msg = m_qAftStatusMsg.front();
		m_qAftStatusMsg.pop();
		return true;
	}
	return false;
}

void CHmsDataBus_Old::SetSvsInfo(int nType, const std::string & strPath)
{
	m_nSvsType = nType;
	m_strSvsVideoPath = strPath;
}
