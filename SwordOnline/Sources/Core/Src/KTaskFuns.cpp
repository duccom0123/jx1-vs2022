#include "KCore.h"
//#include "MyAssert.H"
#include "KPlayer.h"
#include "KPlayerTask.h"
#include "KTaskFuns.h"
#include "KNpc.h"
#include "KSubWorldSet.h"
#ifndef _SERVER
KTabFile g_StringResourseTabFile;

char* g_GetStringRes(int nStringID, char * szString, int nMaxLen)
{
	char stringid[10];
	itoa(nStringID, stringid, 10);
	g_StringResourseTabFile.GetString(stringid, "STRING", "", szString, 5000);
	return szString;
}
#endif


#ifdef _SERVER
int g_WayPointPriceUnit = 1;//WayPoint表格中价格的单位量，WayPoint价格 = 单位量 * 表格数值
int g_StationPriceUnit = 1;	//Station表格中价格的单位量，Station价格 = 单位量 * 表格数值
int g_DockPriceUnit	= 1;
int g_GetPriceToWayPoint(int nStationId, int nWayPoint)
{
	if (g_WayPointPriceTabFile.GetWidth() < nStationId)	
	{
		_ASSERT(0);
		return 0;
	}
	if (g_WayPointPriceTabFile.GetHeight() < nWayPoint)
	{
		_ASSERT(0);
		return 0;
	}
	if (*(g_pWayPointPriceTab + (nStationId - 1) * g_WayPointPriceTabFile.GetWidth() + nWayPoint - 1) < 0)
	{
		g_WayPointPriceTabFile.GetInteger(nWayPoint + 1, nStationId + 1, 0, (g_pWayPointPriceTab + (nStationId - 1) * g_WayPointPriceTabFile.GetWidth() + nWayPoint - 1));
	}
	return 	g_WayPointPriceUnit * (*(g_pWayPointPriceTab + (nStationId - 1) * g_WayPointPriceTabFile.GetWidth() + nWayPoint - 1));
}

int g_GetPriceToStation(int nStationId, int nNextStationId)
{
	if (nStationId == nNextStationId) return 0;
	if (g_StationPriceTabFile.GetWidth() < nStationId)	
	{
		_ASSERT(0);
		return 0;
	}
	if (g_StationTabFile.GetHeight() < nNextStationId)
	{
		_ASSERT(0);
		return 0;
	}
	if (*(g_pStationPriceTab + (nStationId - 1) * g_StationPriceTabFile.GetWidth() + nNextStationId - 1) < 0)
	{
		int t;
		g_StationPriceTabFile.GetInteger(nNextStationId + 1, nStationId + 1, 0, &t);
		*(g_pStationPriceTab + (nStationId - 1) * g_StationPriceTabFile.GetWidth() + nNextStationId - 1) = t;
	}
	return 	g_StationPriceUnit * (*(g_pStationPriceTab + (nStationId - 1) * g_StationPriceTabFile.GetWidth() + nNextStationId - 1));
}

int g_GetPriceToDock(int nCurDockId, int nNextDockId)
{
	if (nCurDockId == nNextDockId)
	{
		return 0;
	}
	
	if (g_DockPriceTabFile.GetWidth() < nCurDockId)	
	{
		_ASSERT(0);
		return 0;
	}

	if (g_DockTabFile.GetHeight()  - 1< nNextDockId)
	{
		_ASSERT(0);
		return 0;
	}
	
	if (*(g_pDockPriceTab + (nCurDockId - 1) * g_DockPriceTabFile.GetWidth() + nNextDockId - 1) < 0)
	{
		int t;
		g_DockPriceTabFile.GetInteger(nNextDockId + 1, nCurDockId + 1, 0, &t);
		*(g_pDockPriceTab + (nCurDockId - 1) * g_DockPriceTabFile.GetWidth() + nNextDockId - 1) = t;
	}
	return 	g_DockPriceUnit * (*(g_pDockPriceTab + (nCurDockId - 1) * g_DockPriceTabFile.GetWidth() + nNextDockId - 1));


}

KTimerTaskFun::KTimerTaskFun()
{
	m_dwTimeTaskTime = 0;
	m_dwTimerTaskId = 0;
	m_dwIntervalTime = 0;
}

BOOL	KTimerTaskFun::Activate(PF_TimerCallBackFun TimerCallBackFun)
{
	if (m_dwTimeTaskTime && m_dwTimeTaskTime <=  g_SubWorldSet.GetGameTime())
	{
		//自动设置下一个周期时间
		m_dwTimeTaskTime = g_SubWorldSet.GetGameTime() + m_dwIntervalTime; 
		
		if (m_dwTimerTaskId)
		{
			char szTimerScript[MAX_PATH];
			g_TimerTask.GetTimerTaskScript(szTimerScript, m_dwTimerTaskId, MAX_PATH);
			TimerCallBackFun(m_pTimerOwner, szTimerScript, m_dwTimerTaskId);
		}
	}
	return TRUE;
}

BOOL	KTimerTaskFun::SaveTask(KPlayer * pPlayer)
{
	if (!pPlayer)
	{
		return FALSE;
	}
	pPlayer->m_cTask.SetSaveVal(TASKVALUE_TIMERTASK_TAST, "");
	pPlayer->m_cTask.SetSaveVal(TASKVALUE_TIMERTASK_RESTTIME, "");
	
	if (m_dwTimerTaskId)
	{
		int nRestTime = m_dwTimeTaskTime - g_SubWorldSet.GetGameTime();

		if (nRestTime > 0)
		{
			char szSaveVal[16];
			itoa(m_dwTimerTaskId, szSaveVal, 10);
			pPlayer->m_cTask.SetSaveVal(TASKVALUE_TIMERTASK_TAST, szSaveVal);
			itoa(nRestTime, szSaveVal, 10);
			pPlayer->m_cTask.SetSaveVal(TASKVALUE_TIMERTASK_RESTTIME, szSaveVal);
		}
	}
	return TRUE;
}

BOOL	KTimerTaskFun::LoadTask(KPlayer * pPlayer)
{
	if (!pPlayer) 
	{
		return FALSE;
	}
	
	int nTimerTaskId = pPlayer->m_cTask.GetSaveVal(TASKVALUE_TIMERTASK_TAST);
	int nRestTime = pPlayer->m_cTask.GetSaveVal(TASKVALUE_TIMERTASK_RESTTIME);
	m_dwTimerTaskId = 0;
	m_dwTimeTaskTime = 0;
	
	if (nRestTime > 0)
	{
		int nAtTime = nRestTime + g_SubWorldSet.GetGameTime();
		m_dwTimerTaskId = nTimerTaskId;
		m_dwTimeTaskTime = nAtTime;
	}
	return TRUE;
}

BOOL	KTimerTaskFun::SetTimer(size_t ulTime, unsigned short usTimerTaskId)
{
	//如果间隔时间为0，表示中止Timer;
	if (ulTime == 0) 
	{
		CloseTimer();
		return FALSE;
	}

	m_dwIntervalTime = ulTime;
	m_dwTimeTaskTime = g_SubWorldSet.GetGameTime() + ulTime;
	m_dwTimerTaskId = usTimerTaskId;
	return TRUE;
};

void	KTimerTaskFun::CloseTimer()
{
	m_dwTimeTaskTime = 0;
	m_dwTimerTaskId = 0;
	m_dwIntervalTime = 0;
};

unsigned long	KTimerTaskFun::GetRestTime()
{
	DWORD dwGameTime = g_SubWorldSet.GetGameTime();
	return (dwGameTime > m_dwTimeTaskTime) ? 0 : (m_dwTimeTaskTime - dwGameTime);
};


void	KTimerTaskFun::GetTimerTaskScript(char * szScriptFileName, unsigned short usTimerTaskId, size_t nScriptFileLen)
{
	if (!szScriptFileName)
		return ;
	
	sprintf(szScriptFileName, TIMERTASK_SCRIPTFILE, usTimerTaskId);
}

KTimerTaskFun g_TimerTask;

int g_PlayerTimerCallBackFun(void * pOwner, char * szScriptFileName, DWORD dwTimerTaskId)
{
	if (!pOwner)
		return 0;
	KPlayer * pPlayer = (KPlayer * )pOwner;
	pPlayer->ExecuteScript(szScriptFileName, "OnTimer", dwTimerTaskId);
	return 1;
};

#endif 
