//---------------------------------------------------------------------------
// Sword3 Engine (c) 2002 by Kingsoft
//
// File:	KPlayerTask.h
// Date:	2002.10.05
// Code:	边城浪子
// Desc:	PlayerTask Class
//---------------------------------------------------------------------------
#include 	"GameDataDef.h"

#ifndef KPLAYERTASK_H
#define KPLAYERTASK_H


#ifdef _SERVER
#define TASKGLOBALVALUENUM 5000
extern int		g_TaskGlobalValue[TASKGLOBALVALUENUM];

extern KTabFile g_WayPointTabFile;
extern KTabFile g_StationTabFile;
extern KTabFile g_DockTabFile;

extern KTabFile g_StationPriceTabFile;
extern KTabFile g_WayPointPriceTabFile;
extern KTabFile g_DockPriceTabFile;

extern int	*g_pStationPriceTab;
extern int  *g_pWayPointPriceTab;
extern int  *g_pDockPriceTab;

#endif


class KPlayerTask
{	friend class KPlayer;
private:
	int			m_nPlayerIndex;
public:
	char		szSave[MAX_TASK][16];					// 用于记录任务是否完成，须保存到数据库
	char		szClear[MAX_TEMP_TASK][16];
										// 用于记录任务过程中的中间步骤的完成情况，不保存到数据库，玩家下线后次数据清空
public:
	KPlayerTask();									// 构造函数
	void		Release();							// 清空
	void		ClearTempVar();						// 清除临时过程控制变量
				// 得到任务完成情况
	int			GetClearVal(int nNo);				// 得到临时过程控制变量值
	const char*		GetClearStr(int nNo);				// 得到临时过程控制变量值
	void		SetClearVal(int nNo, char* szVal);		// 设定临时过程控制变量值
	void		SetSaveVal(int nTaskID, char* szTaskVal, BOOL bSync = FALSE);
	void		SetSaveVal(int nTaskID, int nTaskVal, BOOL bSync = FALSE);
	int			GetSaveVal(int nNo);
	const char*		GetSaveStr(int nNo);
	
	BOOL	SetValue(int nIndex, int nValue);			
	int		GetValue(int nIndex);							
	void		Init(int nPlayerIdx);
#ifdef _SERVER
	BOOL		SyncTaskValue(int nTaskID);
#endif
	
};




#endif
