#ifndef KMISSION_H
#define KMISSION_H

#ifdef _SERVER
#include "KCore.h"
#include "KEngine.h"
#include "KLinkArrayTemplate.h"
#include "KTaskFuns.h"
#include "KPlayerDef.h"

#include "KMissionArray.h"
#include "KPlayerChat.h"
#include "KSortScript.h"

//Mission
//每个Mission有自已的独立的全局变量
//为每一个Mission记录玩家列表的统一的机制
//包括最大人数，等等都要有
//加个Chanel，有利于消息发送
//有功能可以编列所有的参加者id
extern int g_PlayerTimerCallBackFun(void * pOwner, char * szScriptFileName, DWORD dwTimerTaskId);
extern int g_MissionTimerCallBackFun(void * pOwner, char * szScriptFileName, DWORD dwTimerTaskId);
#define MAX_TIMER_PERMISSION 4

typedef struct 
{
	unsigned long m_ulPlayerIndex;
	unsigned long m_ulPlayerID;
	unsigned char m_ucPlayerGroup;
	unsigned long m_ulJoinTime;
	int			  m_nParam[MAX_MISSION_PARAM];
}TMissionPlayerInfo;

typedef struct 
{
	unsigned long m_ulNpcIndex;
	unsigned long m_ulNpcID;
	unsigned long m_ucNpcGroup;
	unsigned long m_ulJoinTime;
}TMissionNpcInfo;
//管理玩家集合的模板类

template<class T , unsigned long ulSize>
class _KMissionPlayerArray:public KLinkArrayTemplate<T , ulSize>
{
public:
	unsigned long FindSame(T *pT)
	{
		int nIdx = 0;
		while(1)
		{
			nIdx = m_UseIdx.GetNext(nIdx);
			if (!nIdx)
				break;
			
			if (pT->m_ulPlayerID == m_Data[nIdx].m_ulPlayerID)
			{
				m_Data[nIdx].m_ulPlayerIndex = pT->m_ulPlayerIndex;
				return nIdx;
			}
		}
		return 0;
	};

	int GetParam(unsigned long ulIndex, int nParam) const
	{
		if (ulIndex >= ulSize) 
			return 0;
		return m_Data[ulIndex].m_nParam[nParam];
	};

	BOOL SetParam(unsigned long ulIndex, int nParam, int nValue);
};

template<class T , unsigned long ulSize>
class _KMissionNpcArray:public KLinkArrayTemplate<T , ulSize>
{
public:
	unsigned long FindSame(T *pT)
	{
		int nIdx = 0;
		while(1)
		{
			nIdx = m_UseIdx.GetNext(nIdx);
			if (!nIdx)
				break;
			
			
			if (pT->m_ulNpcID == m_Data[nIdx].m_ulNpcID)
			{
				m_Data[nIdx].m_ulNpcIndex = pT->m_ulNpcIndex;
				return nIdx;
			}
		}
		return 0;
	};
};

typedef _KMissionPlayerArray<TMissionPlayerInfo, MAX_PLAYER> KMissionPlayerArray;
typedef _KMissionNpcArray<TMissionNpcInfo, MAX_NPC> KMissionNpcArray;

//管理Mission时间触发器集合的模板类
typedef KTimerFunArray <KTimerTaskFun, MAX_TIMER_PERMISSION, g_MissionTimerCallBackFun> KMissionTimerArray;

class KMission
{
	char	m_MissionValue[MAX_MISSIONARRAY_VALUE_COUNT][16];
	unsigned long m_ulMissionId;
	bool m_bMissionLadder;
	char m_szMissionName[64];
	int	m_nLadderParam;
	int	m_nGlbLadderParam[MAX_GLBMISSION_PARAM];
	void * m_pOwner;
public:
	KMissionTimerArray	m_cTimerTaskSet;
	KMission()
	{
		m_pOwner = NULL;
		for (int i = 1; i < MAX_TIMER_PERMISSION; i ++)
		{
			KTimerTaskFun * pTimer = (KTimerTaskFun*)m_cTimerTaskSet.GetData(i);
			if (pTimer)
			pTimer->SetOwner(this);
		}
		memset(m_MissionValue, 0, sizeof(m_MissionValue));
		m_ulMissionId = 0;
		m_bMissionLadder = false;
		m_szMissionName[0] = 0;
		m_nLadderParam = 0;
		memset(m_nGlbLadderParam, 0, sizeof(m_nGlbLadderParam));
	};
	BOOL	Activate();
	
	void SetOwner(void * pOwner)
	{
		m_pOwner = pOwner;
	};
	
	void * GetOwner()
	{
		return m_pOwner;
	};
	
	void	Init()
	{
		memset(m_MissionValue, 0, sizeof(m_MissionValue));
		m_ulMissionId = 0;
		m_bMissionLadder = false;
		memset(m_szMissionName, 0, sizeof(m_szMissionName));
		m_nLadderParam = 0;
		memset(m_nGlbLadderParam, 0, sizeof(m_nGlbLadderParam));
	};

	BOOL SetMissionId(unsigned long ulMissionId)
	{
		m_ulMissionId = ulMissionId;
		return TRUE;
	}

	void SetMissionLadder(const char* szName, int nLadderParam, int* nGlbParam)
	{
		m_bMissionLadder=true;
		memcpy(m_szMissionName, szName, strlen(szName));
		m_nLadderParam = nLadderParam;
		for(int i=0;i<MAX_GLBMISSION_PARAM;i++)
			m_nGlbLadderParam[i] = nGlbParam[i];
	}

	unsigned long GetMissionId()
	{
		return m_ulMissionId;
	}
	
	const char* GetMissionName()
	{
		return m_szMissionName;
	}

	int GetMissionLadderParam()
	{
		return m_nLadderParam;
	}
	
	TMissionLadderInfo* GetMin(
		TMissionLadderInfo* const aMSLDList,
		const int n);

	void ListSort(
		TMissionLadderInfo* const aMSLDList,	
		const int n);
		
	void DataCopy(TMissionLadderInfo* Desc, TMissionPlayerInfo* Source);
	unsigned long Msg2Group(const char * strMsg, unsigned char ucGroup, int nChannelID)
	{
		if (!strMsg || !strMsg[0]) 
			return 0;
		int nCount = 0;
		int nIdx = 0;
		while(1)
		{
			nIdx =	m_MissionPlayer.m_UseIdx.GetNext(nIdx);
			if (!nIdx)
				break;
			
			if ((m_MissionPlayer.m_Data[nIdx].m_ucPlayerGroup == ucGroup) && 
				(m_MissionPlayer.m_Data[nIdx].m_nParam[MISSION_PARAM_AVAILABLE] == MISSION_AVAILABLE_VALUE))
			{
				KPlayerChat::SendSystemInfo(1, m_MissionPlayer.m_Data[nIdx].m_ulPlayerIndex, MESSAGE_BROADCAST_ANNOUCE_HEAD, (char *) strMsg, strlen(strMsg), nChannelID);
				nCount ++;
			}
		}
		return nCount;
	}

	unsigned long Msg2All(const char * strMsg, int nChannelID)
	{
		if (!strMsg || !strMsg[0]) 
			return 0;
		int nCount = 0;
		int nIdx = 0;
		while(1)
		{
			nIdx =	m_MissionPlayer.m_UseIdx.GetNext(nIdx);
			if (!nIdx)
				break;
			
			if (m_MissionPlayer.m_Data[nIdx].m_nParam[MISSION_PARAM_AVAILABLE] == MISSION_AVAILABLE_VALUE)
			{
				KPlayerChat::SendSystemInfo(1, m_MissionPlayer.m_Data[nIdx].m_ulPlayerIndex, MESSAGE_BROADCAST_ANNOUCE_HEAD, (char *) strMsg, strlen(strMsg), nChannelID);
				nCount ++;
			}
		}
		return nCount;
	}

	unsigned long GetNextPlayer(unsigned long ulIdx, unsigned long &ulPlayerIndex)
	{
		ulPlayerIndex = 0;
		while(1)
		{
			ulIdx =	m_MissionPlayer.m_UseIdx.GetNext(ulIdx);
			if (ulIdx == 0) 
				return 0;

			if (m_MissionPlayer.m_Data[ulIdx].m_nParam[MISSION_PARAM_AVAILABLE] == MISSION_AVAILABLE_VALUE)
			{
				ulPlayerIndex = m_MissionPlayer.m_Data[ulIdx].m_ulPlayerIndex;
				return ulIdx;
			}
		}
		return 0;
	}

	unsigned long GetNextNpc(unsigned long ulIdx, unsigned long &ulNpcIndex)
	{
		ulNpcIndex = 0;
		while(1)
		{
			ulIdx =	m_MissionNpc.m_UseIdx.GetNext(ulIdx);
			if (ulIdx == 0) 
				return 0;

			ulNpcIndex = m_MissionNpc.m_Data[ulIdx].m_ulNpcIndex;	
			return ulIdx;
		}
		return 0;
	}	

	unsigned long GetPlayerCount() const
	{ 
		return m_MissionPlayer.GetUsedCount();
	};
	
	unsigned long GetNpcCount() const
	{ 
		return m_MissionNpc.GetUsedCount();
	};

	void SetMission(unsigned long ulValueId, const char* szValue)
	{
		if (ulValueId >= MAX_MISSIONARRAY_VALUE_COUNT)
			return ;
		strcpy(m_MissionValue[ulValueId], szValue);
	};

	unsigned long GetGroupPlayerCount (unsigned char ucGroup ) const 
	{
		unsigned long ulGroupPlayerCount = 0;
		int nIdx = 0;
		while(1)
		{
			nIdx = m_MissionPlayer.m_UseIdx.GetNext(nIdx);
			if (!nIdx)
				break;

			if (ucGroup>= 0)
			{
				if ((m_MissionPlayer.m_Data[nIdx].m_ucPlayerGroup == ucGroup) && 
					(m_MissionPlayer.m_Data[nIdx].m_nParam[MISSION_PARAM_AVAILABLE] == MISSION_AVAILABLE_VALUE))
					ulGroupPlayerCount ++;
			}
		}
		return ulGroupPlayerCount;
	};	

	unsigned long GetGroupNpcCount (unsigned char ucGroup ) const 
	{
		unsigned long ulGroupNpcCount = 0;
		int nIdx = 0;
		while(1)
		{
			nIdx = m_MissionNpc.m_UseIdx.GetNext(nIdx);
			if (!nIdx)
				break;

			if (m_MissionNpc.m_Data[nIdx].m_ucNpcGroup == ucGroup)
				ulGroupNpcCount ++;
		}
		return ulGroupNpcCount;
	};		

	int GetMissionValue(unsigned long ulValueId) const
	{
		if (ulValueId >= MAX_MISSIONARRAY_VALUE_COUNT)
			return 0;

		int nResult = 0;
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
	BOOL	ExecuteScript(char * ScriptFileName, char * szFunName, int nParam);
	BOOL	ExecuteScript(DWORD dwScriptId,  char * szFunName, int nParam);
	BOOL StartTimer(unsigned char ucTimerId, unsigned long ulTimerInterval)
	{
		KTimerTaskFun Timer;
		Timer.SetTimer(ulTimerInterval, ucTimerId);
		Timer.SetOwner(this);
		m_cTimerTaskSet.Add(&Timer);
		return TRUE;
	}

	BOOL StopTimer(unsigned char ucTimerId)
	{
		KTimerTaskFun Timer;
		Timer.SetTimer(1, ucTimerId);
		unsigned long ulIndex = 0;
		KTimerTaskFun * pTimer = m_cTimerTaskSet.GetData(&Timer);
		if (pTimer)
			pTimer->CloseTimer();
		
	}

	DWORD GetTimerRestTimer(unsigned char ucTimerId)
	{
		KTimerTaskFun Timer;
		Timer.SetTimer(1, ucTimerId);
		unsigned long ulIndex = 0;
		KTimerTaskFun * pTimer = m_cTimerTaskSet.GetData(&Timer);
		if (pTimer)
			return pTimer->GetRestTime();
		return 0;
	}

	BOOL StopMission();
	
	KMissionPlayerArray m_MissionPlayer;
	KMissionNpcArray m_MissionNpc;
	TMissionLadderInfo m_MissionLadder[MISSION_STATNUM];

	unsigned long	AddPlayer(unsigned long ulPlayerIndex, unsigned long ulPlayerID, unsigned char ucPlayerGroup, int ulJoinTime = 0);
	unsigned long	AddNpc(unsigned long ulNpcIndex, unsigned long ulNpcID, unsigned char ucNpcGroup, int ulJoinTime = 0);
	BOOL			RemovePlayer(unsigned long ulPlayerIndex, unsigned long ulPlayerID);
	BOOL			RemoveNpc(unsigned long ulNpcIndex, unsigned long ulNpcID = 0);
	void 			SetPlayerParam(unsigned long ulIndex, int nParam, int nValue);
	unsigned long GetMissionPlayer_DataIndex(const char* szName, unsigned long ulPlayerIndex, unsigned long ulPlayerID)//
	{
		if (ulPlayerIndex >= MAX_PLAYER)
			return 0;
		TMissionPlayerInfo Info;
		Info.m_ulPlayerIndex = ulPlayerIndex;
		Info.m_ulPlayerID = ulPlayerID;
		return m_MissionPlayer.FindSame(&Info);
	}

	unsigned long GetMissionPlayer_PlayerIndex(unsigned long ulDataIndex)
	{
		if (ulDataIndex > m_MissionPlayer.GetTotalCount())
			return 0;
		return m_MissionPlayer.m_Data[ulDataIndex].m_ulPlayerIndex;		
	}
	
	unsigned long GetMissionPlayer_GroupId(unsigned long ulDataIndex)
	{
		if (ulDataIndex > m_MissionPlayer.GetTotalCount())
			return 0;
		return m_MissionPlayer.m_Data[ulDataIndex].m_ucPlayerGroup;
	}

	unsigned long GetMissionNpc_DataIndex(unsigned long ulNpcIndex, unsigned long ulNpcID)//
	{
		if (ulNpcIndex >= MAX_NPC)
			return 0;
		TMissionNpcInfo Info;
		Info.m_ulNpcIndex = ulNpcIndex;
		Info.m_ulNpcID = ulNpcID;
		return m_MissionNpc.FindSame(&Info);
	}

	unsigned long GetMissionNpc_NpcIndex(unsigned long ulDataIndex)
	{
		if (ulDataIndex > m_MissionNpc.GetTotalCount())
			return 0;
		return m_MissionNpc.m_Data[ulDataIndex].m_ulNpcIndex;		
	}
	
	unsigned long GetMissionNpc_GroupId(unsigned long ulDataIndex)
	{
		if (ulDataIndex > m_MissionNpc.GetTotalCount())
			return 0;
		return m_MissionNpc.m_Data[ulDataIndex].m_ucNpcGroup;
	}
};
#endif
#endif
