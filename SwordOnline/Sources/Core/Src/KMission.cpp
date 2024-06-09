#include "KCore.h"
#include "KMission.h"
#include "KPlayer.h"
#include "KNpc.h"
#include "KSubWorld.h"
#include "KSubWorldSet.h"
#include "KRegion.h"


#ifdef _SERVER

BOOL KMissionPlayerArray::SetParam(unsigned long ulIndex, int nParam, int nValue)
{
	if (ulIndex >= 1200)
		return FALSE;
	
	m_Data[ulIndex].m_nParam[nParam] =  nValue;

	if (nParam == MISSION_PARAM_AVAILABLE)
	{
		Npc[Player[m_Data[ulIndex].m_ulPlayerIndex].m_nIndex].m_nMissionGroup = nValue==MISSION_AVAILABLE_VALUE?(int)m_Data[ulIndex].m_ucPlayerGroup:-1;
		Player[m_Data[ulIndex].m_ulPlayerIndex].SendMSGroup();
	}
	return TRUE;
};

TMissionLadderInfo* KMission::GetMin(
					TMissionLadderInfo* const aMSLDList,
					const int n)
{
	TMissionLadderInfo* tmpData;
	tmpData = &aMSLDList[0];
	//int j = 0;
	//while(true)
	{
		for(int i=0;i<n;++i)
		{
			if(aMSLDList[i].Name[0] == '\0')
				return &aMSLDList[i];

			if(tmpData->nParam[m_nLadderParam] > aMSLDList[i].nParam[m_nLadderParam])
			{
				tmpData = &aMSLDList[i];
				break;
			}
			if (i == n)break;
		}
		

	}
	return tmpData;
}

void KMission::DataCopy(TMissionLadderInfo* Desc, TMissionPlayerInfo* Source)
{
	strcpy(Desc->Name, Player[Source->m_ulPlayerIndex].Name);
	Desc->ucGroup = Source->m_ucPlayerGroup;
	for(int i =0;i<MAX_MISSION_PARAM;i++)
		Desc->nParam[i] = Source->m_nParam[i];
}

void KMission::ListSort(
					TMissionLadderInfo* const aMSLDList,
					const int n)
{
	int i,j;
	for(i=0;i<n;++i)
	{
		TMissionLadderInfo* tmpData = &aMSLDList[i];
		for(j=i+1;j<n;++j)
		{
			if( tmpData->nParam[m_nLadderParam] < aMSLDList[j].nParam[m_nLadderParam] )
			{
				TMissionLadderInfo tmp;
				memcpy(&tmp, &aMSLDList[i], sizeof(aMSLDList[i]));
				*tmpData = aMSLDList[j];
				memcpy(&aMSLDList[j], &tmp, sizeof(tmp));
			}
		}
	}
}

BOOL KMission::Activate()
{
	m_cTimerTaskSet.Activate();
	
	return TRUE;
}

BOOL	KMission::ExecuteScript(char * ScriptFileName, char * szFunName, int nParam)
{
	if (!ScriptFileName || !ScriptFileName[0] || !szFunName  || !szFunName[0]) return FALSE;
	return ExecuteScript(g_FileName2Id(ScriptFileName), szFunName, nParam);	
}

BOOL	KMission::ExecuteScript(DWORD dwScriptId,  char * szFunName, int nParam)
{
	try
	{
		KLuaScript * pScript = (KLuaScript* )g_GetScript(dwScriptId);
		if (pScript)
		{
			KSubWorld * pSubWorld = (KSubWorld*)GetOwner();
			
			if (pSubWorld)
			{
				Lua_PushNumber(pScript->m_LuaState, pSubWorld->m_nIndex);
				pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);
			}
			
			int nTopIndex = 0;
			
			pScript->SafeCallBegin(&nTopIndex);
			pScript->CallFunction(szFunName,0, "d", nParam);
			pScript->SafeCallEnd(nTopIndex);
		}
		return TRUE;
	}
	catch(...)
	{
		printf("-->Error Execute: %u [%s]\n", dwScriptId, szFunName);
		return FALSE;
	}
	return TRUE;
}

BOOL KMission::StopMission()
{
	unsigned long nTargetIndex = 0;
	int nIdx = 0;
	nIdx = GetNextNpc(nIdx, nTargetIndex);
	while (nIdx)
	{	
		Npc[nTargetIndex].m_nMissionGroup = -1;
		if (Npc[nTargetIndex].m_RegionIndex >= 0)
		{
			SubWorld[Npc[nTargetIndex].m_SubWorldIndex].m_Region[Npc[nTargetIndex].m_RegionIndex].RemoveNpc(nTargetIndex);
			SubWorld[Npc[nTargetIndex].m_SubWorldIndex].m_Region[Npc[nTargetIndex].m_RegionIndex].DecRef(Npc[nTargetIndex].m_MapX, Npc[nTargetIndex].m_MapY, obj_npc);
		}
		NpcSet.Remove(nTargetIndex);
		nIdx = GetNextNpc(nIdx, nTargetIndex);
	}

	nTargetIndex = 0;
	nIdx = 0;
	nIdx = GetNextPlayer(nIdx, nTargetIndex);
	while (nIdx)
	{	
		Npc[Player[nTargetIndex].m_nIndex].m_nMissionGroup = -1;
		Player[nTargetIndex].SendMSGroup();
		nIdx = GetNextPlayer(nIdx, nTargetIndex);
	}
	Init();
	m_cTimerTaskSet.Clear();
	m_MissionPlayer.Clear();
	m_MissionNpc.Clear();
	return TRUE;
}

unsigned long	KMission::AddPlayer(unsigned long ulPlayerIndex, unsigned long ulPlayerID, unsigned char ucPlayerGroup, int ulJoinTime/* = 0*/)
{
	if (ulPlayerIndex >= MAX_PLAYER|| ulPlayerID == 0)
		return 0;
	
	unsigned long i = 0;
	TMissionPlayerInfo * pInfo = m_MissionPlayer.FindFree(i);
	
	if (!i || !pInfo)
		return 0;

	pInfo->m_ulPlayerIndex = ulPlayerIndex;
	pInfo->m_ulPlayerID = ulPlayerID;
	pInfo->m_ucPlayerGroup = ucPlayerGroup;
	memset(pInfo->m_nParam, 0, sizeof(pInfo->m_nParam));
	
	if (!ulJoinTime)
		pInfo->m_ulJoinTime = g_SubWorldSet.GetGameTime();
	else
		pInfo->m_ulJoinTime = ulJoinTime;

	m_MissionPlayer.m_FreeIdx.Remove(i);
	m_MissionPlayer.m_UseIdx.Insert(i);
	m_MissionPlayer.m_ulFreeSize --;
	m_MissionPlayer.SetParam(i, MISSION_PARAM_AVAILABLE, MISSION_AVAILABLE_VALUE);
	return i;
};

unsigned long	KMission::AddNpc(unsigned long ulNpcIndex, unsigned long ulNpcID, unsigned char ucNpcGroup, int ulJoinTime/* = 0*/)
{
	if (ulNpcIndex >= MAX_NPC|| ulNpcID == 0)
		return 0;
	
	unsigned long i = 0;
	TMissionNpcInfo * pInfo = m_MissionNpc.FindFree(i);
	
	if (!i || !pInfo)
		return 0;
	pInfo->m_ulNpcIndex = ulNpcIndex;
	pInfo->m_ulNpcID = ulNpcID;
	pInfo->m_ucNpcGroup = ucNpcGroup;
	
	if (!ulJoinTime)
		pInfo->m_ulJoinTime = g_SubWorldSet.GetGameTime();
	else
		pInfo->m_ulJoinTime = ulJoinTime;
	m_MissionNpc.m_FreeIdx.Remove(i);
	m_MissionNpc.m_UseIdx.Insert(i);
	m_MissionNpc.m_ulFreeSize --;
	
	Npc[ulNpcIndex].m_nMissionGroup = (int)ucNpcGroup;
	return i;
};

BOOL	KMission::RemovePlayer(unsigned long ulPlayerIndex, unsigned long ulPlayerID)
{
	if (ulPlayerIndex >= MAX_PLAYER)
		return FALSE;
	TMissionPlayerInfo Info;
	Info.m_ulPlayerIndex = ulPlayerIndex;
	Info.m_ulPlayerID = ulPlayerID;
	//if (m_MissionPlayer.Remove(&Info))
	if (m_MissionPlayer.FindSame(&Info))
	{
		Npc[Player[ulPlayerIndex].m_nIndex].m_nMissionGroup = -1;
		if (m_ulMissionId)
		{
			char szScript[MAX_PATH];
			sprintf(szScript, MISSIONTASK_SCRIPTFILE, m_ulMissionId);
			if (szScript[0])
			{
				ExecuteScript(szScript, "OnLeave", ulPlayerIndex);
			}
		}
	}
	return TRUE;
};

BOOL	KMission::RemoveNpc(unsigned long ulNpcIndex, unsigned long ulNpcID/* = 0*/)
{
	if (ulNpcIndex >= MAX_NPC)
		return 0;
	TMissionNpcInfo Info;
	Info.m_ulNpcIndex = ulNpcIndex;
	Info.m_ulNpcID = ulNpcID;
	if (m_MissionNpc.Remove(&Info))
	{
		if (Npc[ulNpcIndex].m_RegionIndex >= 0)
		{
			SubWorld[Npc[ulNpcIndex].m_SubWorldIndex].m_Region[Npc[ulNpcIndex].m_RegionIndex].RemoveNpc(ulNpcIndex);
			SubWorld[Npc[ulNpcIndex].m_SubWorldIndex].m_Region[Npc[ulNpcIndex].m_RegionIndex].DecRef(Npc[ulNpcIndex].m_MapX, Npc[ulNpcIndex].m_MapY, obj_npc);
		}
		NpcSet.Remove(ulNpcIndex);
	}
	return TRUE;
};

void KMission::SetPlayerParam(unsigned long ulIndex, int nParam, int nValue)
{
	m_MissionPlayer.SetParam(ulIndex, nParam, nValue);

	if (m_bMissionLadder)
	{
		if(m_nLadderParam==nParam)
		{
			TMissionLadderSelfInfo tmpDataSelf;
			memset(&tmpDataSelf, 0, sizeof(tmpDataSelf));
			memset(m_MissionLadder, 0, sizeof(m_MissionLadder));
			int nIdx = 0,i=0;
			unsigned long nPlayerIndex = 0;
			nIdx = GetNextPlayer(nIdx, nPlayerIndex);
			while (nIdx)
			{	
				TMissionLadderInfo* tmpData = GetMin(m_MissionLadder, MISSION_STATNUM);
				if (tmpData->nParam[m_nLadderParam] < m_MissionPlayer.m_Data[nIdx].m_nParam[m_nLadderParam])
				{
					DataCopy(tmpData, &m_MissionPlayer.m_Data[nIdx]);
				}
				nIdx = GetNextPlayer(nIdx, nPlayerIndex);
			}
			ListSort(m_MissionLadder, MISSION_STATNUM);

			memcpy(tmpDataSelf.szMissionName, m_szMissionName, strlen(m_szMissionName));
			tmpDataSelf.nGlbParam[0]=this->GetGroupPlayerCount(m_nGlbLadderParam[0]);
			tmpDataSelf.nGlbParam[1]=this->GetGroupPlayerCount(m_nGlbLadderParam[1]);
			tmpDataSelf.nGlbParam[2]=this->GetTimerRestTimer(m_nGlbLadderParam[2]);

			nIdx = 0;
			nPlayerIndex = 0;
			nIdx = GetNextPlayer(nIdx, nPlayerIndex);
			while (nIdx)
			{	
				tmpDataSelf.ucGroup = m_MissionPlayer.m_Data[nIdx].m_ucPlayerGroup;
				for (i =0;i<MAX_MISSION_PARAM;i++)
					tmpDataSelf.nParam[i] = m_MissionPlayer.m_Data[nIdx].m_nParam[i];
				Player[nPlayerIndex].SendMSRank(&tmpDataSelf,m_MissionLadder);
				nIdx = GetNextPlayer(nIdx, nPlayerIndex);
			}
		}
	}
};
	
int g_MissionTimerCallBackFun(void * pOwner, char * szScriptFile, DWORD dwTimerTaskId)
{
	if (!pOwner) return FALSE;
	KMission *pMission = (KMission*)pOwner;
	if (szScriptFile)
	{
		pMission->ExecuteScript(szScriptFile, "OnMissionTimer", dwTimerTaskId);
		printf("-->Run %s\n", szScriptFile);
	}
	else
		printf("-->Error Execute: Can't find script[%u]", dwTimerTaskId);
	return 1;
}

#endif
