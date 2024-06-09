//#include <objbase.h>
//#include <crtdbg.h>
#include "KCore.h"
#include "KPlayer.h"
#include "KNpc.h"
#include "KNpcSet.h"
#include "KSubWorld.h"
#include "GameDataDef.h"
#include "SkillDef.h"
#include "KProtocolProcess.h"
#ifdef _SERVER
//#include "KNetServer.h"
//#include "../MultiServer/Heaven/Interface/iServer.h"
#include "KSubWorldSet.h"
#include "CoreServerShell.h"
#endif
#include "KPlayerSet.h"
#include "Text.h"
#include "Cryptography/EDOneTimePad.h"
#include "CoreUseNameDef.h"
#include "../../Engine/Src/Text.h"
#include <shlwapi.h>
#define		PLAYER_FIRST_LUCKY				0

KPlayerSet PlayerSet;

KPlayerSet::KPlayerSet()
{
#ifdef _SERVER
	m_ulNextSaveTime = 0;
	m_ulDelayTimePerSave = 0;
	m_ulMaxSaveTimePerPlayer = 0;
#endif
#ifndef _SERVER
	memset(m_szNationalEmblemPic, 0, sizeof(m_szNationalEmblemPic));
	memset(m_szFortuneRankPic, 0, sizeof(m_szFortuneRankPic));
	memset(m_szViprankPic, 0, sizeof(m_szViprankPic));
	memset(m_szTranlifePic, 0, sizeof(m_szTranlifePic));
#endif
}

KPlayerSet::~KPlayerSet()
{
}

BOOL	KPlayerSet::Init()
{
	int i = 0;
	
#ifdef _SERVER
	m_ulNextSaveTime = 0;
	m_ulMaxSaveTimePerPlayer = 60 * 20 * 30;
	m_ulDelayTimePerSave = m_ulMaxSaveTimePerPlayer / MAX_PLAYER;
#endif

	// �Ż����ұ�
	m_FreeIdx.Init(MAX_PLAYER);
	m_UseIdx.Init(MAX_PLAYER);

	// ��ʼʱ���е�����Ԫ�ض�Ϊ��
	for (i = MAX_PLAYER - 1; i > 0; i--)
	{
		m_FreeIdx.Insert(i);
	}

	if ( !m_cLevelAdd.Init() )
		return FALSE;
	if ( !m_cMagicLevelExp.Init() )
		return FALSE;
	if ( !m_cLeadExp.Init() )
		return FALSE;
	for (i = 0; i < MAX_PLAYER; i++)
	{
		Player[i].Release();
		Player[i].SetPlayerIndex(i);
#ifdef _SERVER
		Player[i].m_cPK.Init(i);
		Player[i].m_cTask.Init(i);
		Player[i].m_cRoom.Init(i);
#else
		Player[i].m_cAI.Init();
#endif
		Player[i].m_cTong.Init(i);
		Player[i].m_ItemList.Init(i);
		Player[i].m_Node.m_nIndex = i;
	}
	if ( !m_cNewPlayerAttribute.Init() )
		return FALSE;
	if ( !m_cPlayerStamina.Init())
		return FALSE;
#ifdef _SERVER

	KTabFile	cPKParam;

	memset(m_sPKPunishParam, 0, sizeof(m_sPKPunishParam));
//	g_SetFilePath("\\");
	if (!cPKParam.Load(defPK_PUNISH_FILE))
		return FALSE;
	for (i = 0; i < MAX_DEATH_PUNISH_PK_VALUE + 1; i++)
	{
		cPKParam.GetInteger(i + 2, 2, 1, &m_sPKPunishParam[i].m_nExpP);
		cPKParam.GetInteger(i + 2, 3, 1, &m_sPKPunishParam[i].m_nExpV);
		cPKParam.GetInteger(i + 2, 4, 1, &m_sPKPunishParam[i].m_nMoney);
		cPKParam.GetInteger(i + 2, 5, 1, &m_sPKPunishParam[i].m_nItem);
		cPKParam.GetInteger(i + 2, 6, 1, &m_sPKPunishParam[i].m_nEquip);
		cPKParam.GetInteger(i + 2, 7, 1, &m_sPKPunishParam[i].m_nAbradeP);
	}
#endif
#ifndef _SERVER
	char Buff[MAX_PATH];
	for (i=0; i<10;i++)
	{
		if(i<MAX_TONG_NATIONALEMBLEM)
		{
			sprintf(Buff, "Spr_%d", i);
			g_GameSetting.GetString("NationalEmblem", Buff, "", m_szNationalEmblemPic[i], sizeof(m_szNationalEmblemPic[i]));	
		}
		if(i<(MAX_ITEM_LEVEL+1))
		{
			sprintf(Buff, "Spr_%d", i);
			g_GameSetting.GetString("FortuneRank", Buff, "", m_szFortuneRankPic[i], sizeof(m_szFortuneRankPic[i]));		
		}
		if(i<(MAX_TRANSLIFE_VALUE+1))
		{
			itoa(i, Buff, 10);
			g_GameSetting.GetString("TransLife", Buff, "", m_szTranlifePic[i], sizeof(m_szTranlifePic[i]));
		}
		if(i<(MAX_VIPRANK_VALUE+1))
		{
			sprintf(Buff, "Spr_%d", i);
			g_GameSetting.GetString("VipRank", Buff, "", m_szViprankPic[i], sizeof(m_szViprankPic[i]));
		}
	}
#endif
	return TRUE;
}

int	KPlayerSet::FindFree()
{
	return m_FreeIdx.GetNext(0);
}

int KPlayerSet::FindSame(DWORD dwID)
{
	int nUseIdx = 0;

	nUseIdx = m_UseIdx.GetNext(0);
	while(nUseIdx)
	{
		if (Player[nUseIdx].m_dwID == dwID)
			return nUseIdx;
		nUseIdx = m_UseIdx.GetNext(nUseIdx);
	}
	return 0;
}

int		KPlayerSet::GetFirstPlayer()
{
	m_nListCurIdx = m_UseIdx.GetNext(0);
	return m_nListCurIdx;
}

int	KPlayerSet::GetNextPlayer()
{
	if ( !m_nListCurIdx )
		return 0;
	m_nListCurIdx = m_UseIdx.GetNext(m_nListCurIdx);
	return m_nListCurIdx;
}

#ifdef _SERVER
int KPlayerSet::Add(LPSTR szPlayerID, void* pGuid)
{
	if (!pGuid || !szPlayerID || !szPlayerID[0])
		return 0;

	int i = 0;

	DWORD dwID = g_FileName2Id(szPlayerID);

/*	i = FindSame(dwID);
	if (i)
		return 0;*/

	i = FindFree();

	if (i)
	{
		Player[i].m_dwID = dwID;
		Player[i].m_nNetConnectIdx = -1;
		Player[i].m_dwLoginTime = g_SubWorldSet.GetGameTime();
		memcpy(&Player[i].m_Guid, pGuid, sizeof(GUID));
		Player[i].SetPlayerIndex(i);
		m_FreeIdx.Remove(i);
		m_UseIdx.Insert(i);
		return i;
	}
	return 0;
}

int	KPlayerSet::Broadcasting(char* pMessage, int nLen)
{
	if ( !pMessage || nLen <= 0 || nLen >= MAX_SENTENCE_LENGTH)
		return 0;
	if (!g_pServer)
		return 0;
	g_pServer->PreparePackSink();
	KPlayerChat::SendSystemInfo(0, 0, MESSAGE_SYSTEM_ANNOUCE_HEAD, pMessage, nLen);
	g_pServer->SendPackToClient(-1);
	return 1;
}

void KPlayerSet::PrepareRemove(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;

	int nSubWorld = Npc[Player[nIndex].m_nIndex].m_SubWorldIndex;
	if (nSubWorld >= 0)
	{
		SubWorld[nSubWorld].m_MissionArray.RemovePlayer(nIndex, Player[nIndex].m_dwID);
	}

	Player[nIndex].ExecuteScript(LOGOUT_SCRIPT, "OnLogout", 0);

//	g_DebugLog("SERVER:Player[%s] has been removed!", Npc[Player[nIndex].m_nIndex].Name);
	// ֪ͨ��������Լ�������
	// �����ӣ��뿪����
	PLAYER_APPLY_LEAVE_TEAM	sLeaveTeam;
	sLeaveTeam.ProtocolType = c2s_teamapplyleave;
	sLeaveTeam.bMySelf = FALSE;
	Player[nIndex].LeaveTeam((BYTE*)&sLeaveTeam);
	// ����������ڽ��У�ȡ������
	TRADE_DECISION_COMMAND	sTrade;
	sTrade.ProtocolType = c2s_tradedecision;
	sTrade.m_btDecision = 0;
	sTrade.m_btFolkGame = 0;
	Player[nIndex].TradeDecision((BYTE*)&sTrade);
	if (Player[nIndex].m_PTrade.nTrade)
		Player[nIndex].SendTradeCancel();
	if(Player[nIndex].m_cRoom.m_nFlag)
		Player[nIndex].m_cRoom.LeaveRoom();
	// PK״̬ȡ��
	Player[nIndex].m_cPK.CloseAll();

//	Player[nIndex].Save();
	Player[nIndex].WaitForRemove();
}

void KPlayerSet::PrepareLoginFailed(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;

//	g_DebugLog("SERVER:Player[%s] has been removed!", Npc[Player[nIndex].m_nIndex].Name);
	// ֪ͨ��������Լ�������
	// �����ӣ��뿪����
	PLAYER_APPLY_LEAVE_TEAM	sLeaveTeam;
	sLeaveTeam.ProtocolType = c2s_teamapplyleave;
	sLeaveTeam.bMySelf = FALSE;
	Player[nIndex].LeaveTeam((BYTE*)&sLeaveTeam);
	// ����������ڽ��У�ȡ������
	TRADE_DECISION_COMMAND	sTrade;
	sTrade.ProtocolType = c2s_tradedecision;
	sTrade.m_btDecision = 0;
	sTrade.m_btFolkGame = 0;
	Player[nIndex].TradeDecision((BYTE*)&sTrade);
	// PK״̬ȡ��
	Player[nIndex].m_cPK.CloseAll();
	// ���״̬����
	Player[nIndex].m_cTong.Clear();

	// ����TimeOut����Player
	Player[nIndex].m_nNetConnectIdx = -1;
	Player[nIndex].m_dwLoginTime = 0;
}

void KPlayerSet::PrepareExchange(int i)
{
	if (!Player[i].m_bExchangeServer || !Npc[Player[i].m_nIndex].m_bExchangeServer)
		return;

	// ���PK״̬
	Player[i].m_cPK.CloseAll();

	// ���
	PLAYER_APPLY_LEAVE_TEAM	sLeaveTeam;
	sLeaveTeam.ProtocolType = c2s_teamapplyleave;
	sLeaveTeam.bMySelf = FALSE;
	Player[i].LeaveTeam((BYTE*)&sLeaveTeam);

	// ����������ڽ��У�ȡ������
	TRADE_DECISION_COMMAND	sTrade;
	sTrade.ProtocolType = c2s_tradedecision;
	sTrade.m_btDecision = 0;
	sTrade.m_btFolkGame = 0;
	Player[i].TradeDecision((BYTE*)&sTrade);

	Player[i].Save();
}


void KPlayerSet::RemoveQuiting(int nIndex)
{
	if (Player[nIndex].m_nNetConnectIdx == -1 || Player[nIndex].m_dwID == 0 || Player[nIndex].m_byLixian != LIXIAN_IDLE)
		return;

	if (Player[nIndex].IsWaitingRemove())
	{
		if (Player[nIndex].m_nIndex > 0)	// have npc
		{
			int nRegion = Npc[Player[nIndex].m_nIndex].m_RegionIndex;
			int nSubWorld = Npc[Player[nIndex].m_nIndex].m_SubWorldIndex;
			
			if (nSubWorld >= 0 && nRegion >= 0)
			{
				SubWorld[nSubWorld].RemovePlayer(nRegion, nIndex);
				SubWorld[nSubWorld].m_Region[nRegion].RemoveNpc(Player[nIndex].m_nIndex);
				SubWorld[nSubWorld].m_Region[nRegion].DecRef(Npc[Player[nIndex].m_nIndex].m_MapX, Npc[Player[nIndex].m_nIndex].m_MapY, obj_npc);
			}	
			NpcSet.Remove(Player[nIndex].m_nIndex);
		}
		// ���״̬����
		Player[nIndex].m_cTong.Clear();

		Player[nIndex].m_ItemList.RemoveAll();
		Player[nIndex].m_dwID = 0;
		Player[nIndex].m_nIndex = 0;
		Player[nIndex].m_nNetConnectIdx = -1;
		Player[nIndex].Release();
		
		m_FreeIdx.Insert(nIndex);
		m_UseIdx.Remove(nIndex);
	}
}	

void KPlayerSet::RemoveLoginTimeOut(int nIndex)
{
	if (Player[nIndex].IsLoginTimeOut())
	{
		if (Player[nIndex].m_nIndex > 0)
		{
			int nRegion = Npc[Player[nIndex].m_nIndex].m_RegionIndex;
			int nSubWorld = Npc[Player[nIndex].m_nIndex].m_SubWorldIndex;
		
		if (nSubWorld >= 0)
			SubWorld[nSubWorld].RemovePlayer(nRegion, nIndex);
		
			if (nSubWorld >= 0 && nRegion >= 0)
			{
				SubWorld[nSubWorld].m_Region[nRegion].RemoveNpc(Player[nIndex].m_nIndex);
				SubWorld[nSubWorld].m_Region[nRegion].DecRef(Npc[Player[nIndex].m_nIndex].m_MapX, Npc[Player[nIndex].m_nIndex].m_MapY, obj_npc);
			}
			NpcSet.Remove(Player[nIndex].m_nIndex);
		}		
		Player[nIndex].m_ItemList.RemoveAll();
		Player[nIndex].m_dwID = 0;
		Player[nIndex].m_nIndex = 0;
		Player[nIndex].m_nNetConnectIdx = -1;
		Player[nIndex].LoginTimeOut();
		
		m_FreeIdx.Insert(nIndex);
		m_UseIdx.Remove(nIndex);
	}
}

void KPlayerSet::RemoveExchanging(int nIndex)
{
	if (Player[nIndex].m_nNetConnectIdx == -1)
		return;

	if (Player[nIndex].IsExchangingServer())
	{
		int nRegion = Npc[Player[nIndex].m_nIndex].m_RegionIndex;
		int nSubWorld = Npc[Player[nIndex].m_nIndex].m_SubWorldIndex;
		
		if (nSubWorld >= 0 && nRegion >= 0)
		{
			SubWorld[nSubWorld].RemovePlayer(nRegion, nIndex);
			SubWorld[nSubWorld].m_Region[nRegion].RemoveNpc(Player[nIndex].m_nIndex);
			SubWorld[nSubWorld].m_Region[nRegion].DecRef(Npc[Player[nIndex].m_nIndex].m_MapX, Npc[Player[nIndex].m_nIndex].m_MapY, obj_npc);
		}
		
		if (Player[nIndex].m_nIndex > 0)
			NpcSet.Remove(Player[nIndex].m_nIndex);
		
		Player[nIndex].m_ItemList.RemoveAll();
		Player[nIndex].m_dwID = 0;
		Player[nIndex].m_nIndex = 0;
		Player[nIndex].m_nNetConnectIdx = -1;
		Player[nIndex].Release();
		
		m_FreeIdx.Insert(nIndex);
		m_UseIdx.Remove(nIndex);
	}
}
#endif

/*
int KPlayerSet::FindClient(int nClient)
{
	int i = 0;

	i = m_UseIdx.GetNext(0);
	while(i)
	{
		if (Player[i].m_nNetConnectIdx == nClient)
			return i;
		i = m_UseIdx.GetNext(i);
	}
	return 0;
}
*/

#ifdef _SERVER
void KPlayerSet::ProcessClientMessage(int nIndex, const char* pChar, int nSize)
{
	if (nIndex <= 0 && nIndex >= MAX_PLAYER)
		return;

	int i = Player[nIndex].m_nNetConnectIdx;

	if (i >= 0)
	{
//		_ASSERT(g_CoreServerShell.CheckProtocolSize(pChar, nSize));
		if (Player[nIndex].m_dwID && !Player[nIndex].m_bExchangeServer)
			g_ProtocolProcess.ProcessNetMsg(nIndex, (BYTE*)pChar);
	}
}
#endif

//---------------------------------------------------------------------------
//	���ܣ����캯��
//---------------------------------------------------------------------------
KLevelAdd::KLevelAdd()
{
	memset(m_nLevelExp, 0, sizeof(int) * MAX_LEVEL);
	memset(m_nLevelExpAdd, 0, sizeof(int) * MAX_TRANSLIFE_VALUE * MAX_LEVEL);
	memset(m_nLifePerLevel, 0, sizeof(int) * series_num);
	memset(m_nManaPerLevel, 0, sizeof(int) * series_num);
	memset(m_nStaminaMalePerLevel, 0, sizeof(int) * series_num);
	memset(m_nStaminaFemalePerLevel, 0, sizeof(int) * series_num);
	memset(m_nLifePerVitality, 0, sizeof(int) * series_num);
	memset(m_nStaminaPerVitality, 0, sizeof(int) * series_num);
	memset(m_nManaPerEnergy, 0, sizeof(int) * series_num);
	memset(m_nStaminaMaleBase, 0, sizeof(int) * series_num);
	memset(m_nStaminaFemaleBase, 0, sizeof(int) * series_num);
}

//---------------------------------------------------------------------------
//	���ܣ���ʼ��
//---------------------------------------------------------------------------
BOOL	KLevelAdd::Init()
{
	int			i, j;
	KTabFile	LevelExp;
	if ( !LevelExp.Load(PLAYER_LEVEL_EXP_FILE) )
		return FALSE;
	for (i = 0; i < MAX_TRANSLIFE_VALUE; i ++)
	{
		for (j = 0; j < MAX_LEVEL; j++)
		{
			if(i == 0)
				LevelExp.GetInteger(j + 2, 2, 0, &m_nLevelExp[j]);
			LevelExp.GetInteger(j + 2, 3+i, 0, &m_nLevelExpAdd[i][j]);
		}
	}
	KTabFile	LevelAdd;
	if ( !LevelAdd.Load(PLAYER_LEVEL_ADD_FILE) )
		return FALSE;
	for (i = 0; i < series_num; i++)
	{
		LevelAdd.GetInteger(i + 2, 2, 0, &m_nLifePerLevel[i]);
		LevelAdd.GetInteger(i + 2, 3, 0, &m_nStaminaMalePerLevel[i]);
		LevelAdd.GetInteger(i + 2, 4, 0, &m_nStaminaFemalePerLevel[i]);
		LevelAdd.GetInteger(i + 2, 5, 0, &m_nManaPerLevel[i]);
		LevelAdd.GetInteger(i + 2, 6, 0, &m_nLifePerVitality[i]);
		LevelAdd.GetInteger(i + 2, 7, 0, &m_nStaminaPerVitality[i]);
		LevelAdd.GetInteger(i + 2, 8, 0, &m_nManaPerEnergy[i]);
		LevelAdd.GetInteger(i + 2, 9, 0, &m_nLeadExpShare[i]);
		LevelAdd.GetInteger(i + 2, 10, 0, &m_nFireResistPerLevel[i]);
		LevelAdd.GetInteger(i + 2, 11, 0, &m_nColdResistPerLevel[i]);
		LevelAdd.GetInteger(i + 2, 12, 0, &m_nPoisonResistPerLevel[i]);
		LevelAdd.GetInteger(i + 2, 13, 0, &m_nLightResistPerLevel[i]);
		LevelAdd.GetInteger(i + 2, 14, 0, &m_nPhysicsResistPerLevel[i]);
		LevelAdd.GetInteger(i + 2, 15, 0, &m_nStaminaMaleBase[i]);
		LevelAdd.GetInteger(i + 2, 16, 0, &m_nStaminaFemaleBase[i]);
	}

	return TRUE;
}

//---------------------------------------------------------------------------
//	���ܣ����ĳ�ȼ�����������
//	������nLevel  Ŀ��ȼ�
//---------------------------------------------------------------------------
int		KLevelAdd::GetLevelExp(int nLevel, int nTranslife)
{
	if (nLevel < 1 || nLevel > MAX_LEVEL)
		return 0;
	return m_nLevelExp[nLevel - 1] + m_nLevelExpAdd[nTranslife][nLevel - 1];
}

//---------------------------------------------------------------------------
//	���ܣ����ÿ��ϵ������������
//---------------------------------------------------------------------------
int		KLevelAdd::GetLifePerLevel(int nSeries)
{
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	return m_nLifePerLevel[nSeries];
}

//---------------------------------------------------------------------------
//	���ܣ����ÿ��ϵ������������
//---------------------------------------------------------------------------
int		KLevelAdd::GetStaminaPerLevel(int nSex, int nSeries)
{
	if (nSex < 0 || nSex > 1)
		return 0;
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	if (nSex)
		return m_nStaminaFemalePerLevel[nSeries];
	else
		return m_nStaminaMalePerLevel[nSeries];
}

//---------------------------------------------------------------------------
//	���ܣ����ÿ��ϵ������������
//---------------------------------------------------------------------------
int		KLevelAdd::GetManaPerLevel(int nSeries)
{
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	return m_nManaPerLevel[nSeries];
}

//---------------------------------------------------------------------------
//	���ܣ�ÿ��ϵ����������һ�������������
//---------------------------------------------------------------------------
int		KLevelAdd::GetLifePerVitality(int nSeries)
{
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	return m_nLifePerVitality[nSeries];
}

//---------------------------------------------------------------------------
//	���ܣ�ÿ��ϵ����������һ�������������
//---------------------------------------------------------------------------
int		KLevelAdd::GetStaminaPerVitality(int nSeries)
{
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	return m_nStaminaPerVitality[nSeries];
}

//---------------------------------------------------------------------------
//	���ܣ�ÿ��ϵ����������һ�������������
//---------------------------------------------------------------------------
int		KLevelAdd::GetManaPerEnergy(int nSeries)
{
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	return m_nManaPerEnergy[nSeries];
}

//---------------------------------------------------------------------------
//	���ܣ�ÿ��ϵ����������һ�������������
//---------------------------------------------------------------------------
int		KLevelAdd::GetLeadExpShare(int nSeries)
{
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	return m_nLeadExpShare[nSeries];
}

//---------------------------------------------------------------------------
//	���ܣ�ÿ��ϵĳ���ȼ��Ļ�������
//---------------------------------------------------------------------------
int		KLevelAdd::GetFireResist(int nSeries, int nLevel)
{
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	if (nLevel <= 0 || nLevel > MAX_LEVEL)
		return 0;
	return (m_nFireResistPerLevel[nSeries] * nLevel / 100);
}

//---------------------------------------------------------------------------
//	���ܣ�ÿ��ϵĳ���ȼ��Ļ���������
//---------------------------------------------------------------------------
int		KLevelAdd::GetColdResist(int nSeries, int nLevel)
{
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	if (nLevel <= 0 || nLevel > MAX_LEVEL)
		return 0;
	return (m_nColdResistPerLevel[nSeries] * nLevel / 100);
}

//---------------------------------------------------------------------------
//	���ܣ�ÿ��ϵĳ���ȼ��Ļ���������
//---------------------------------------------------------------------------
int		KLevelAdd::GetPoisonResist(int nSeries, int nLevel)
{
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	if (nLevel <= 0 || nLevel > MAX_LEVEL)
		return 0;
	return (m_nPoisonResistPerLevel[nSeries] * nLevel / 100);
}

//---------------------------------------------------------------------------
//	���ܣ�ÿ��ϵĳ���ȼ��Ļ����翹��
//---------------------------------------------------------------------------
int		KLevelAdd::GetLightResist(int nSeries, int nLevel)
{
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	if (nLevel <= 0 || nLevel > MAX_LEVEL)
		return 0;
	return (m_nLightResistPerLevel[nSeries] * nLevel / 100);
}

//---------------------------------------------------------------------------
//	���ܣ�ÿ��ϵĳ���ȼ��Ļ�����������
//---------------------------------------------------------------------------
int		KLevelAdd::GetPhysicsResist(int nSeries, int nLevel)
{
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	if (nLevel <= 0 || nLevel > MAX_LEVEL)
		return 0;
	return (m_nPhysicsResistPerLevel[nSeries] * nLevel / 100);
}

int		KLevelAdd::GetStaminaBase(int nSex, int nSeries)
{
	if (nSex < 0 || nSex > 1)
		return 0;
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	if (nSex)
		return m_nStaminaFemaleBase[nSeries];
	else
		return m_nStaminaMaleBase[nSeries];
}


//---------------------------------------------------------------------------
//	���ܣ����캯��
//---------------------------------------------------------------------------
KMagicLevelExp::KMagicLevelExp()
{
}

//---------------------------------------------------------------------------
//	���ܣ���ʼ��
//---------------------------------------------------------------------------
BOOL	KMagicLevelExp::Init()
{
	if ( !m_MagicLevelExpFile.Load(PLAYER_MAGIC_LEVEL_EXP_FILE) )
		return FALSE;
		
	return TRUE;
}

int		KMagicLevelExp::GetNextExp(int nSkillId, int nLevel)
{
	if (nSkillId <= 0 && nSkillId > MAX_SKILL)
		return 0;
	if (nLevel <= 0 && nLevel > MAX_TRAIN_SKILLEXPLEVEL)
		return 0;	
		
	int nExp = 0;
	char nRow[16];
	char nCol[8];
	itoa(nSkillId, nRow, 10);
	sprintf(nCol, "LEVEL%d", nLevel);
	m_MagicLevelExpFile.GetInteger(nRow, nCol, 0, &nExp);
	
	return nExp;
}
//---------------------------------------------------------------------------
//	���ܣ����캯��
//---------------------------------------------------------------------------
KTeamLeadExp::KTeamLeadExp()
{
	for (int i = 0; i < MAX_LEAD_LEVEL; i++)
	{
		m_sPerLevel[i].m_dwExp = 0;
		m_sPerLevel[i].m_dwMemNum = 1;
	}
}

//---------------------------------------------------------------------------
//	���ܣ���ʼ������ȡͳ��������
//---------------------------------------------------------------------------
BOOL	KTeamLeadExp::Init()
{
	int			i;
	KTabFile	LevelExp;

	if ( !LevelExp.Load(PLAYER_LEVEL_LEAD_EXP_FILE) )
		return FALSE;
	for (i = 0; i < MAX_LEAD_LEVEL; i++)
	{
		LevelExp.GetInteger(i + 2, 2, 0, (int*)&m_sPerLevel[i].m_dwExp);
		LevelExp.GetInteger(i + 2, 3, 1, (int*)&m_sPerLevel[i].m_dwMemNum);
	}

	return TRUE;
}

//---------------------------------------------------------------------------
//	���ܣ����뾭���õȼ���
//---------------------------------------------------------------------------
int		KTeamLeadExp::GetLevel(DWORD dwExp, int nCurLeadLevel)
{
	if (dwExp <= 0)
		return 1;
	if (nCurLeadLevel > 0)
	{
		if (nCurLeadLevel >= MAX_LEAD_LEVEL)
			return MAX_LEAD_LEVEL;
		if (dwExp < m_sPerLevel[nCurLeadLevel - 1].m_dwExp)
			return nCurLeadLevel;
		if (dwExp < m_sPerLevel[nCurLeadLevel].m_dwExp)
			return nCurLeadLevel + 1;
	}
	for (int i = 0; i < MAX_LEAD_LEVEL; i++)
	{
		if (dwExp < m_sPerLevel[i].m_dwExp)
		{
			return i + 1;
		}
	}
	return MAX_LEAD_LEVEL;
}

//---------------------------------------------------------------------------
//	���ܣ����뾭���ÿɴ���Ա��
//---------------------------------------------------------------------------
int		KTeamLeadExp::GetMemNumFromExp(DWORD dwExp)
{
	int nGetLevel;
	nGetLevel = GetLevel(dwExp);
	return m_sPerLevel[nGetLevel - 1].m_dwMemNum;
}

//---------------------------------------------------------------------------
//	���ܣ�����ȼ���ÿɴ���Ա��
//---------------------------------------------------------------------------
int		KTeamLeadExp::GetMemNumFromLevel(int nLevel)
{
	if (1 <= nLevel && nLevel <= MAX_LEAD_LEVEL)
		return m_sPerLevel[nLevel - 1].m_dwMemNum;
	return 1;
}

//---------------------------------------------------------------------------
//	���ܣ�����ȼ�����������辭��ֵ
//---------------------------------------------------------------------------
int		KTeamLeadExp::GetLevelExp(int nLevel)
{
	if (1 <= nLevel && nLevel <= MAX_LEAD_LEVEL)
		return this->m_sPerLevel[nLevel - 1].m_dwExp;
	return 0;
}

#ifdef _SERVER
void	KPlayerSet::Activate()
{
}
#endif

KNewPlayerAttribute::KNewPlayerAttribute()
{
	memset(m_nStrength, 0, sizeof(m_nStrength));
	memset(m_nDexterity, 0, sizeof(m_nDexterity));
	memset(m_nVitality, 0, sizeof(m_nVitality));
	memset(m_nEngergy, 0, sizeof(m_nEngergy));
	memset(m_nLucky, 0, sizeof(m_nLucky));
}

BOOL	KNewPlayerAttribute::Init()
{
	char		szSeries[series_num][16] = {"metal", "wood", "water", "fire", "earth"};
	KIniFile	AttributeFile;

	if ( !AttributeFile.Load(BASE_ATTRIBUTE_FILE_NAME) )
		return FALSE;

	for (int i = 0; i < series_num; i++)
	{
		AttributeFile.GetInteger(szSeries[i], "Strength", 0, &m_nStrength[i]);
		AttributeFile.GetInteger(szSeries[i], "Dexterity", 0, &m_nDexterity[i]);
		AttributeFile.GetInteger(szSeries[i], "Vitality", 0, &m_nVitality[i]);
		AttributeFile.GetInteger(szSeries[i], "Engergy", 0, &m_nEngergy[i]);
		AttributeFile.GetInteger(szSeries[i], "Lucky", PLAYER_FIRST_LUCKY, &m_nLucky[i]);
	}

	return TRUE;
}


KPlayerStamina::KPlayerStamina()
{
	m_nNormalAdd = 0;
	m_nExerciseRunSub = 0;
	m_nFightRunSub = 0;
	m_nKillRunSub = 0;
	m_nTongWarRunSub = 0;
	m_nSitAdd = 0;
}

BOOL	KPlayerStamina::Init()
{
	KIniFile	StaminaFile;

	if ( !StaminaFile.Load(PLAYER_STAMINA_FILE_NAME) )
		return FALSE;

	StaminaFile.GetInteger("stamina", "NormalAdd", 1, &m_nNormalAdd);
	StaminaFile.GetInteger("stamina", "ExerciseRunSub", 1, &m_nExerciseRunSub);
	StaminaFile.GetInteger("stamina", "FightRunSub", 1, &m_nFightRunSub);
	StaminaFile.GetInteger("stamina", "KillRunSub", 18, &m_nKillRunSub);
	StaminaFile.GetInteger("stamina", "TongWarRunSub", 8, &m_nTongWarRunSub);
	StaminaFile.GetInteger("stamina", "SitAdd", 10, &m_nSitAdd);

	return TRUE;
}

#ifdef _SERVER
BOOL	KPlayerSet::GetPlayerName(int nIndex, char* szName)
{
//	int i = FindClient(nClient);

	int i = nIndex;

	if (!szName)
		return FALSE;

	if (i <= 0 || i >= MAX_PLAYER)
	{
		szName[0] = 0;
		return FALSE;
	}
	strcpy(szName, Player[i].Name);
	return TRUE;
}
#endif

#ifdef _SERVER
BOOL	KPlayerSet::GetPlayerAccount(int i, char* szName)
{
//	int i = FindClient(nClient);
	if (!szName)
		return FALSE;

	if (i <= 0 || i >= MAX_PLAYER)
	{
		szName[0] = 0;
		return FALSE;
	}

	strcpy(szName, Player[i].AccountName);
	return TRUE;
}
#endif


#ifdef _SERVER
int		KPlayerSet::AttachPlayer(const unsigned long lnID, GUID* pGuid)
{
	if (lnID >= MAX_PLAYER || NULL == pGuid)
		return 0;

	int nUseIdx = m_UseIdx.GetPrev(0);
	while(nUseIdx)
	{
		if (Player[nUseIdx].m_nNetConnectIdx == -1)
		{
			if (0 == memcmp(&Player[nUseIdx].m_Guid, pGuid, sizeof(GUID)))
			{
				Player[nUseIdx].m_nNetConnectIdx = lnID;
				Player[nUseIdx].m_ulLastSaveTime = g_SubWorldSet.m_nLoopRate;
///				Player[nUseIdx].m_dwLoginTime = -1;
				return nUseIdx;
			}
//			else
//			{
//				g_DebugLog("[error]Attach to a connected player named %s", Player[nUseIdx].Name);
//				return 0;
//			}
		}
		nUseIdx = m_UseIdx.GetPrev(nUseIdx);
	}
	return 0;
}
#endif

#ifdef _SERVER
int		KPlayerSet::GetPlayerIndexByGuid(GUID* pGuid)
{
	int nUseIdx = m_UseIdx.GetNext(0);
	while(nUseIdx)
	{
		if (0 == memcmp(&Player[nUseIdx].m_Guid, pGuid, sizeof(GUID)))
		{
			if (Player[nUseIdx].m_nNetConnectIdx != -1)
			{
				return nUseIdx;
			}
			else
			{
				g_DebugLog("[error]Find Guid to a disconnect player");
				return 0;
			}
		}
		nUseIdx = m_UseIdx.GetNext(nUseIdx);
	}
	return 0;
}
#endif

#ifdef _SERVER
void KPlayerSet::AutoSave()
{
	unsigned long uTime = g_SubWorldSet.GetGameTime();

	if (uTime >= m_ulNextSaveTime)
	{
		int nUseIdx = m_UseIdx.GetNext(0);
		while(nUseIdx)
		{
			if (Player[nUseIdx].CanSave() && uTime - Player[nUseIdx].m_ulLastSaveTime >= m_ulMaxSaveTimePerPlayer)
			{
				if (Player[nUseIdx].Save())
				{
					Player[nUseIdx].m_uMustSave = SAVE_REQUEST;
//					Player[nUseIdx].m_ulLastSaveTime = uTime;
					m_ulNextSaveTime += m_ulDelayTimePerSave;
				}
				break;
			}
			nUseIdx = m_UseIdx.GetNext(nUseIdx);
		}
	}
}

int KPlayerSet::ExecuteScript(int nPlayerIndex, char* szScriptName, char* szFuncName, int nParam)
{
	DWORD dwScriptId = g_FileName2Id(szScriptName);
	return ExecuteScript(nPlayerIndex, dwScriptId, szFuncName, nParam);
}

int KPlayerSet::ExecuteScript(int nPlayerIndex, DWORD dwScriptId, char* szFuncName, int nParam)
{
	if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
	{
		try
		{
			KLuaScript * pScript = (KLuaScript* )g_GetScript(dwScriptId);
			if (pScript)
			{	
				Lua_PushNumber(pScript->m_LuaState, nPlayerIndex);
				pScript->SetGlobalName(SCRIPT_PLAYERINDEX);
				
				Lua_PushNumber(pScript->m_LuaState, Player[nPlayerIndex].m_dwID);
				pScript->SetGlobalName(SCRIPT_PLAYERID);
				
				Lua_PushNumber(pScript->m_LuaState, Npc[Player[nPlayerIndex].m_nIndex].m_SubWorldIndex);
				pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);

				int nTopIndex = 0;
				int nReturn = 0;
				pScript->SafeCallBegin(&nTopIndex);
				pScript->CallFunction(szFuncName, 1, "d", nParam);
				nTopIndex = Lua_GetTopIndex(pScript->m_LuaState);
				nReturn = (int) Lua_ValueToNumber(pScript->m_LuaState, nTopIndex);
				pScript->SafeCallEnd(nTopIndex);

				return nReturn;
			}
			return 0;
		}
		catch(...)
		{
			printf("-->Error Execute: %u [%s]\n", dwScriptId, szFuncName);
			return 0;
		}
	}
	return 0;
}
#endif