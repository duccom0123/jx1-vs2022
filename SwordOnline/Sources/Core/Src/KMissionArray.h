#ifndef KMISSIONARRAY_H
#define KMISSIONARRAY_H

#include "KTaskFuns.h"
#include "KMission.h"

#define MAX_MISSIONARRAY_VALUE_COUNT 10
//管理Mission的集合

template <class T, size_t ulSize>
class KMissionArray : public KLinkArrayTemplate<T,  ulSize>
{
	char	m_MissionValue[MAX_MISSIONARRAY_VALUE_COUNT][16];
public:
	BOOL	Activate()
	{
		unsigned long ulActiveCount = GetUsedCount();
		if (ulActiveCount == 0)
			return FALSE;
		
		int nIdx = 0;		
		for (int i = 0; i < ulActiveCount ; i ++)
		{
			nIdx = m_UseIdx.GetNext(nIdx);
			_ASSERT(nIdx);
			if (!nIdx)
			{
				break;
			}
			m_Data[nIdx].Activate();
		}
		return TRUE;
	};
	
	void SetMission(unsigned long ulValueId, const char* szValue)
	{
		if (ulValueId >= MAX_MISSIONARRAY_VALUE_COUNT)
			return ;
		strcpy(m_MissionValue[ulValueId], szValue);
	};

	int GetMissionValue(unsigned long ulValueId) const
	{
		if (ulValueId >= MAX_MISSIONARRAY_VALUE_COUNT)
			return 0;
		int nResult =0;
		if (m_MissionValue[ulValueId][0])
			nResult = atoi(m_MissionValue[ulValueId]);
		return nResult;
	};
	
	const char* GetMissionString(unsigned long ulValueId) const
	{
		if (ulValueId >= MAX_MISSIONARRAY_VALUE_COUNT)
			return "";
		return m_MissionValue[ulValueId];
	};
	unsigned long FindSame(T *pMission)
	{
		int nIdx = 0;
		while(1)
		{
			nIdx = m_UseIdx.GetNext(nIdx);
			if (!nIdx)
				break;
			
			if (pMission->GetMissionId() == m_Data[nIdx].GetMissionId())
				return nIdx;
		}
		return 0;
	};
	BOOL RemovePlayer(unsigned long ulPlayerIndex, unsigned long ulPlayerID)
	{
		unsigned long ulActiveCount = GetUsedCount();
		if (ulActiveCount == 0)
			return FALSE;
		
		int nIdx = 0;		
		for (int i = 0; i < ulActiveCount ; i ++)
		{
			nIdx = m_UseIdx.GetNext(nIdx);
			_ASSERT(nIdx);
			if (!nIdx)
			{
				break;
			}
			m_Data[nIdx].RemovePlayer(ulPlayerIndex, ulPlayerID);
		}
		return TRUE;
	}
};


//管理定时触发器的集合
template <class T, size_t ulSize, PF_TimerCallBackFun CallFun>
class KTimerFunArray :public KLinkArrayTemplate < T, ulSize>
{
public:
	char m_szScriptFile[MAX_PATH];
	BOOL	Activate()
	{
		unsigned long ulActiveCount = GetUsedCount();
		if (ulActiveCount == 0)
			return FALSE;
		
		int nIdx = 0;		
		for (int i = 0; i < ulActiveCount ; i ++)
		{
			nIdx = m_UseIdx.GetNext(nIdx);
		//	_ASSERT(nIdx);
			if (!nIdx)
			{
				break;
			}
			m_Data[nIdx].Activate(CallFun);
		}
		return TRUE;
	};

	unsigned long FindSame(T *pTask)
	{
		int nIdx = 0;
		while(1)
		{
			nIdx = m_UseIdx.GetNext(nIdx);
			if (!nIdx)
				break;

			if (pTask->GetTaskId() == m_Data[nIdx].GetTaskId())
				return nIdx;
		}
		return 0;
	};

};
#endif
