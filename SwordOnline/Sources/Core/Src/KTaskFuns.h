#ifndef KTASKFUNCS_H
#define KTASKFUNCS_H

#include "KSubWorldSet.h"
typedef int (* PF_TimerCallBackFun)(void * pOwner, char * szFileName, DWORD dwTimerTaskId);

#define MAX_TASK_SCRIPTFUNC 100

#define TASKVALUE_SAVEWAYPOINT_BEGIN 101
#define	TASKVALUE_MAXWAYPOINT_COUNT 3

#define TASKVALUE_SAVESTATION_BEGIN  110
#define TASKVALUE_MAXSTATION_COUNT 32

#define TASKVALUE_TIMERTASK_TAST	200
#define TASKVALUE_TIMERTASK_RESTTIME 201

#define TASKVALUE_BASEDATA_PASSWORD 202
#define TASKVALUE_BASEDATA_MATENAME 203

#define TIMERTASK_SCRIPTFILE "\\script\\timertask\\task%02d.lua"
#define MISSIONTASK_SCRIPTFILE "\\script\\missions\\mission%02d.lua"

class KPlayer;

class KTaskFun
{
public:
	virtual BOOL	SaveTask(KPlayer *) = 0;
	virtual BOOL	LoadTask(KPlayer *) = 0;
};

class KTimerTaskFun :public KTaskFun
{
public:
	KTimerTaskFun();
	
	KTimerTaskFun(void * pOwner)
	{
		m_pTimerOwner = pOwner;
	};

	void SetOwner(void * pOwner) { m_pTimerOwner = pOwner;};

	BOOL	SaveTask(KPlayer *);
	BOOL	LoadTask(KPlayer *);
	void *  m_pTimerOwner;
	
	BOOL	SetTimer(size_t ulTime, unsigned short usTimerTaskId);
	void	CloseTimer();
	unsigned long	GetRestTime();
	

	void	GetTimerTaskScript(char * szScriptFileName, unsigned short usTimerTaskId, size_t nScriptFileLen);
	BOOL	Activate(PF_TimerCallBackFun TimerCallBackFun);
	DWORD GetTaskId(){return m_dwTimerTaskId;};
private:
	DWORD	m_dwTimeTaskTime;
	DWORD	m_dwTimerTaskId;
	DWORD	m_dwIntervalTime;
	
	
};


typedef KTaskFun KTask;

extern KTimerTaskFun g_TimerTask;
#endif
