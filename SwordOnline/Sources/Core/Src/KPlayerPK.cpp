//---------------------------------------------------------------------------
// Sword3 Engine (c) 2003 by Kingsoft
//
// File:	KPlayerPK.cpp
// Date:	2003.07.15
// Code:	边城浪子
// Desc:	PlayerPK Class
//---------------------------------------------------------------------------


#include	"KCore.h"
#include	"KPlayer.h"
#include	"KPlayerDef.h"
#include	"KPlayerPK.h"
#include "../../Engine/Src/Text.h"

#ifndef _SERVER
#include "CoreShell.h"
#endif

#ifdef _SERVER

//-------------------------------------------------------------------------
//	功能：构造函数
//-------------------------------------------------------------------------
KPlayerPK::KPlayerPK()
{
	m_nPlayerIndex = 0;
}

//-------------------------------------------------------------------------
//	功能：初始化
//-------------------------------------------------------------------------
void	KPlayerPK::Init(int nPlayerIdx)
{
	this->m_nPlayerIndex = nPlayerIdx;

	m_nNormalPKFlag			= 0;
	m_btLockPKState			= 0;
	m_nEnmityPKState		= enumPK_ENMITY_STATE_CLOSE;
	m_nEnmityPKAim			= 0;
	m_nEnmityPKTime			= 0;
	m_nEnmityPKLaunchFlag	= 0;
	m_bEnmitySpar			= FALSE;
	m_nExercisePKFlag		= 0;
	m_nExercisePKAim		= 0;
	m_nPKValue				= 0;
	m_nBackFightMode		= enumFightNone;
	m_uNextChangeNormalPK	= 0;
	m_uNextChangeFightPK	= 0;
}

void	KPlayerPK::Active()
{
	EnmityPKCountDown();
}


BOOL	KPlayerPK::CheckSwitchPK(BYTE bFlag, BOOL bCaptainSet)
{
	if ((m_nPKValue >= NpcSet.m_nPKNotSwitchPKWhenLock) && 
		Player[m_nPlayerIndex].GetLockState() && 
		bFlag != enumPKNormal)
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_PK_HIGHT_LOCK_NOT_SWITCH;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
		sMsg.m_lpBuf = (LPVOID)NpcSet.m_nPKNotSwitchPKWhenLock;
		g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		sMsg.m_lpBuf = 0;
		return FALSE;
	}
	if ((!NpcSet.m_nPKWarOpen && bFlag == enumPKWar) || 
		(!NpcSet.m_nPKMurderOpen && bFlag == enumPKMurder) || 
		(!NpcSet.m_nPKTongOpen && bFlag == enumPKTongWar))
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_PKSTATE_NOT_OPEN;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
		g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		return FALSE;
	}
	if (!bCaptainSet && 
		Player[m_nPlayerIndex].m_cTeam.m_nFlag && 
		Player[m_nPlayerIndex].m_cTeam.m_nFigure != TEAM_CAPTAIN && 
		Player[m_nPlayerIndex].m_cTeam.GetPKFollowCaptain())
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_PK_IS_LOCK3;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
		g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		return FALSE;
	}

	if (bFlag == enumPKTongWar && !Player[m_nPlayerIndex].m_cTong.m_nFlag)
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_PK_IS_LOCK2;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
		g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		return FALSE;
	}

	if (bFlag == enumPKTongWar && Player[m_nPlayerIndex].m_cTeam.m_nFlag)
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_PK_IS_LOCK4;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
		g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		return FALSE;
	}

	if ((m_nPKValue >= NpcSet.m_nNotFightWhenHightPK) && bFlag != enumPKNormal)
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_PK_IS_LOCK5;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
		g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		return FALSE;
	}
	if (((Player[m_nPlayerIndex].m_nExp*MAX_PERCENT/Player[m_nPlayerIndex].m_nNextLevelExp) < NpcSet.m_nNotFightExpPercent) && bFlag != enumPKNormal)
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_PK_IS_LOCK6;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
		g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		return FALSE;
	}
	BOOL bPKLocked = FALSE;
	switch (m_btLockPKState)
	{
		case enumPKLogNothing:
			break;
		case enumPKLogAll:
			bPKLocked = TRUE;
			break;
		case enumPKLogTongWar:
			if (bFlag == enumPKTongWar)
				bPKLocked = TRUE;
			break;
		case enumPKLogMurder:
			if (bFlag == enumPKMurder)
				bPKLocked = TRUE;
			break;
		case enumPKLogWar:
			if (bFlag == enumPKWar)
				bPKLocked = TRUE;
			break;
		case enumPKLogNormal:
			if (bFlag == enumPKNormal)
				bPKLocked = TRUE;
			break;
	}
	if (bPKLocked)
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_PK_IS_LOCK1;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
		g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		return FALSE;
	}

	if (!bCaptainSet)
	{
		int nPKState = GetNormalPKState();
		if ((bFlag == enumPKWar || 
			bFlag == enumPKMurder || 
			bFlag == enumPKTongWar) && 
			nPKState == enumPKNormal)
		{
			m_uNextChangeNormalPK = GetTickCount();
			m_uNextChangeFightPK = GetTickCount();
			return TRUE;
		}
		else if ((bFlag == enumPKWar || 
			bFlag == enumPKMurder || 
			bFlag == enumPKTongWar) && 
			nPKState != enumPKNormal)
		{
			if (nPKState != bFlag)
			{
				if (GetTickCount() - m_uNextChangeFightPK > NpcSet.m_nFightPKTimeLong)
				{
					m_uNextChangeFightPK = GetTickCount();
					return TRUE;
				}
				SHOW_MSG_SYNC	sMsg;
				sMsg.ProtocolType = s2c_msgshow;
				sMsg.m_wMsgID = enumMSG_ID_PK_IS_LOCK7;
				sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
				sMsg.m_lpBuf = (LPVOID)NpcSet.m_nFightPKTimeLong;
				g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
				return FALSE;
			}
			return TRUE;
		}
		else if ((nPKState == enumPKWar || 
			nPKState == enumPKMurder || 
			nPKState == enumPKTongWar) && 
			bFlag == enumPKNormal)
		{
			if (Npc[Player[m_nPlayerIndex].m_nIndex].m_FightMode == enumFightNone || 
				(GetTickCount() - m_uNextChangeNormalPK > NpcSet.m_nNormalPKTimeLong))
			{
				m_uNextChangeNormalPK = GetTickCount();
				return TRUE;
			}
			SHOW_MSG_SYNC	sMsg;
			sMsg.ProtocolType = s2c_msgshow;
			sMsg.m_wMsgID = enumMSG_ID_PK_IS_LOCK;
			sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
			sMsg.m_lpBuf = (LPVOID)NpcSet.m_nNormalPKTimeLong;
			g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
			return FALSE;
		}
	}
	return TRUE;
}
//-------------------------------------------------------------------------
//	功能：设定正常PK状态 TRUE 打开，可以砍人  FALSE 关闭，不可以砍人
//-------------------------------------------------------------------------
void	KPlayerPK::SetNormalPKState(BYTE bFlag, BOOL bCaptainSet)
{
	if (!CheckSwitchPK(bFlag, bCaptainSet))
		return;

	if (NpcSet.m_nPKTongOpenCamp)
	{
		if (bFlag == enumPKTongWar)
			Npc[Player[m_nPlayerIndex].m_nIndex].SetCurrentCamp(camp_tongwar);
		else
		{
			if (m_nNormalPKFlag == enumPKTongWar)
				Npc[Player[m_nPlayerIndex].m_nIndex].RestoreCurrentCamp();
		}
	}

	m_nNormalPKFlag = bFlag;
	Npc[Player[m_nPlayerIndex].m_nIndex].m_nPKFlag = bFlag;

	PK_NORMAL_FLAG_SYNC	sFlag;
	sFlag.ProtocolType = s2c_pksyncnormalflag;
	sFlag.m_btFlag = m_nNormalPKFlag;
	g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sFlag, sizeof(PK_NORMAL_FLAG_SYNC));

	int i,nMemberId;
	if (Player[m_nPlayerIndex].m_cTeam.m_nFlag)
	{
		if (Player[m_nPlayerIndex].m_cTeam.m_nFigure == TEAM_CAPTAIN)
		{
			for (i=0; i< MAX_TEAM_MEMBER; i++)
			{
				nMemberId = g_Team[Player[m_nPlayerIndex].m_cTeam.m_nID].m_nMember[i];
				if (nMemberId > 0 && (Player[nMemberId].m_cTeam.GetPKFollowCaptain()))
					Player[nMemberId].m_cPK.SetNormalPKState(bFlag, TRUE);
			}
		}
	}
}


void KPlayerPK::SetLockPKState(BYTE bFlag, BYTE btState)
{
	m_nNormalPKFlag = bFlag;
	Npc[Player[m_nPlayerIndex].m_nIndex].m_nPKFlag = bFlag;
	m_btLockPKState = btState;

	if (bFlag != enumPKNormal)
		m_uNextChangeNormalPK = GetTickCount();

	PK_NORMAL_FLAG_SYNC	sFlag;
	sFlag.ProtocolType = s2c_pksyncnormalflag;
	sFlag.m_btFlag = m_nNormalPKFlag;
	g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sFlag, sizeof(PK_NORMAL_FLAG_SYNC));
}
//-------------------------------------------------------------------------
//	功能：获得正常PK状态 TRUE 打开，可以砍人  FALSE 关闭，不可以砍人
//-------------------------------------------------------------------------
BOOL	KPlayerPK::GetNormalPKState()
{
	return this->m_nNormalPKFlag;
}

//-------------------------------------------------------------------------
//	功能：关闭仇杀PK
//-------------------------------------------------------------------------
void	KPlayerPK::EnmityPKClose(BOOL bIsPKing)
{
	if (m_nEnmityPKState == enumPK_ENMITY_STATE_CLOSE)
		return;

	PK_ENMITY_STATE_SYNC	sState;
	sState.ProtocolType = s2c_pksyncenmitystate;
	sState.m_btState = enumPK_ENMITY_STATE_CLOSE;
	sState.m_bSpar = this->m_bEnmitySpar;
	sState.m_dwNpcID = 0;
	sState.m_wLength = sizeof(PK_ENMITY_STATE_SYNC) - 1 - sizeof(sState.m_szName);
	g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sState, sState.m_wLength + 1);

	if (m_nEnmityPKAim > 0 && m_nEnmityPKAim < MAX_PLAYER)
	{
		sState.m_bSpar = Player[m_nEnmityPKAim].m_cPK.m_bEnmitySpar;
		g_pServer->PackDataToClient(Player[m_nEnmityPKAim].m_nNetConnectIdx, (BYTE*)&sState, sState.m_wLength + 1);

		if(bIsPKing)
		{
			if(Player[m_nEnmityPKAim].m_cPK.m_bEnmitySpar && NpcSet.m_nSparPacific)
				Npc[Player[m_nEnmityPKAim].m_nIndex].m_FightMode = Player[m_nEnmityPKAim].m_cPK.m_nBackFightMode;
			Player[m_nEnmityPKAim].m_cPK.m_bEnmitySpar = FALSE;
			Player[m_nEnmityPKAim].m_cPK.m_nBackFightMode = enumFightNone;
		}
		Player[m_nEnmityPKAim].m_cPK.m_nEnmityPKState = enumPK_ENMITY_STATE_CLOSE;
		Player[m_nEnmityPKAim].m_cPK.m_nEnmityPKAim = 0;
		Player[m_nEnmityPKAim].m_cPK.m_nEnmityPKTime = 0;
	}
	if(bIsPKing)
	{
		if(this->m_bEnmitySpar && NpcSet.m_nSparPacific)
			Npc[Player[m_nPlayerIndex].m_nIndex].m_FightMode = this->m_nBackFightMode;
		this->m_bEnmitySpar = FALSE;
		this->m_nBackFightMode = enumFightNone;
	}
	this->m_nEnmityPKState = enumPK_ENMITY_STATE_CLOSE;
	this->m_nEnmityPKAim = 0;
	this->m_nEnmityPKTime = 0;
	m_nEnmityPKLaunchFlag = 0;
}

//-------------------------------------------------------------------------
//	功能：仇杀PK开始
//-------------------------------------------------------------------------
BOOL	KPlayerPK::EnmityPKOpen(int nAim, BOOL bSpar)
{
	// 出错
	if (nAim <= 0 || nAim >= MAX_PLAYER || Player[nAim].m_nIndex < 1)
		return FALSE;
	// 非战斗模式
	if (!bSpar && ((m_nPKValue >= NpcSet.m_nPKNotSwitchPKWhenLock) && 
		Player[m_nPlayerIndex].GetLockState()))
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_PK_HIGHT_LOCK_NOT_SWITCH;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
		sMsg.m_lpBuf = (LPVOID)NpcSet.m_nPKNotSwitchPKWhenLock;
		g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		sMsg.m_lpBuf = 0;
		return FALSE;
	}
	if (!bSpar && (m_nPKValue >= NpcSet.m_nNotFightWhenHightPK))
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_PK_IS_LOCK5;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
		g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		return FALSE;
	}
	if (!bSpar && ((Player[m_nPlayerIndex].m_nExp*MAX_PERCENT/Player[m_nPlayerIndex].m_nNextLevelExp) < NpcSet.m_nNotEnmityExpPercent))
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_PK_IS_LOCK6;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
		g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		return FALSE;
	}
	if (!bSpar && !Npc[Player[m_nPlayerIndex].m_nIndex].m_FightMode)
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_PK_ERROR_1;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
		g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		return FALSE;
	}
	if (!Npc[Player[nAim].m_nIndex].m_FightMode)
	{
		if(bSpar && NpcSet.m_nSparPacific)
		{
		}
		else
		{
			SHOW_MSG_SYNC	sMsg;
			sMsg.ProtocolType = s2c_msgshow;
			sMsg.m_wMsgID = enumMSG_ID_PK_ERROR_6;
			sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
			g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
			return FALSE;
		}
	}
	// 自己是新手
	if (Npc[Player[m_nPlayerIndex].m_nIndex].m_Camp == camp_begin)
	{
		if(bSpar && NpcSet.m_nSparPacific)
		{
		}
		else
		{
			SHOW_MSG_SYNC	sMsg;
			sMsg.ProtocolType = s2c_msgshow;
			sMsg.m_wMsgID = enumMSG_ID_PK_ERROR_2;
			sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
			g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
			return FALSE;
		}
	}
	// 正在切磋
	if (m_nExercisePKFlag == TRUE)
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_PK_ERROR_3;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
		g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		return FALSE;
	}
	// 对方正在切磋
	if (Player[nAim].m_cPK.GetExercisePKState())
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_PK_ERROR_7;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
		g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		return FALSE;
	}
	// 正在仇杀
	if (m_nEnmityPKState != enumPK_ENMITY_STATE_CLOSE)
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_PK_ERROR_4;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
		g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		return FALSE;
	}
	// 对方是新手
	if (Npc[Player[nAim].m_nIndex].m_Camp == camp_begin)
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_PK_ERROR_5;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
		g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		return FALSE;
	}

	if (Player[m_nPlayerIndex].m_bForbidEnmity)
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_PK_ERROR_8;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
		g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		return FALSE;
	}

	if (Player[nAim].m_bForbidEnmity)
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_PK_ERROR_9;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
		g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		return FALSE;
	}

	Player[nAim].m_cPK.EnmityPKClose(TRUE);
	Player[nAim].m_cPK.m_nEnmityPKState = enumPK_ENMITY_STATE_TIME;
	Player[nAim].m_cPK.m_nEnmityPKAim = m_nPlayerIndex;
	if(bSpar)
		Player[nAim].m_cPK.m_nEnmityPKTime = PK_ANMITY_TIME / 2;
	else
		Player[nAim].m_cPK.m_nEnmityPKTime = PK_ANMITY_TIME;
	Player[nAim].m_cPK.m_nEnmityPKLaunchFlag = 0;
	Player[nAim].m_cPK.m_bEnmitySpar = bSpar;
	if(Player[nAim].m_cPK.m_bEnmitySpar && NpcSet.m_nSparPacific)
		Player[nAim].m_cPK.m_nBackFightMode = Npc[Player[nAim].m_nIndex].m_FightMode;

	m_nEnmityPKState = enumPK_ENMITY_STATE_TIME;
	m_nEnmityPKAim = nAim;
	if(bSpar)
		m_nEnmityPKTime = PK_ANMITY_TIME / 2;
	else
		m_nEnmityPKTime = PK_ANMITY_TIME;
	m_nEnmityPKLaunchFlag = 1;
	m_bEnmitySpar = bSpar;
	if(m_bEnmitySpar && NpcSet.m_nSparPacific)
		m_nBackFightMode = Npc[Player[m_nPlayerIndex].m_nIndex].m_FightMode;

	PK_ENMITY_STATE_SYNC	sState;
	sState.ProtocolType = s2c_pksyncenmitystate;
	sState.m_btState = enumPK_ENMITY_STATE_TIME;
	sState.m_bSpar = bSpar;
	sState.m_bAim = 1;
	sState.m_dwNpcID = Npc[Player[m_nPlayerIndex].m_nIndex].m_dwID;
	strcpy(sState.m_szName, Npc[Player[m_nPlayerIndex].m_nIndex].Name);
	sState.m_wLength = sizeof(PK_ENMITY_STATE_SYNC) - 1 - sizeof(sState.m_szName) + strlen(sState.m_szName);
	g_pServer->PackDataToClient(Player[nAim].m_nNetConnectIdx, (BYTE*)&sState, sState.m_wLength + 1);
	sState.m_bAim = 0;
	sState.m_dwNpcID = Npc[Player[nAim].m_nIndex].m_dwID;
	strcpy(sState.m_szName, Npc[Player[nAim].m_nIndex].Name);
	sState.m_wLength = sizeof(PK_ENMITY_STATE_SYNC) - 1 - sizeof(sState.m_szName) + strlen(sState.m_szName);
	g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sState, sState.m_wLength + 1);

	return TRUE;
}

//-------------------------------------------------------------------------
//	功能：获得仇杀状态，对应 enumPK_ANMITY_STATE
//-------------------------------------------------------------------------
int		KPlayerPK::GetEnmityPKState()
{
	return this->m_nEnmityPKState;
}

//-------------------------------------------------------------------------
//	功能：获得仇杀目标 if 目标==0 不在仇杀状态 else 得到仇杀目标
//-------------------------------------------------------------------------
int		KPlayerPK::GetEnmityPKAim()
{
	return this->m_nEnmityPKAim;
}

//-------------------------------------------------------------------------
//	功能：关闭切磋PK
//-------------------------------------------------------------------------
void	KPlayerPK::ExercisePKClose()
{
	if (m_nExercisePKFlag == FALSE)
		return;

	PK_EXERCISE_STATE_SYNC	sState;
	sState.ProtocolType = s2c_pksyncexercisestate;
	sState.m_btState = 0;
	sState.m_dwNpcID = 0;
	sState.m_wLength = sizeof(PK_EXERCISE_STATE_SYNC) - 1 - sizeof(sState.m_szName);
	g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sState, sState.m_wLength + 1);

	if (m_nExercisePKAim > 0 && m_nExercisePKAim < MAX_PLAYER)
	{
		g_pServer->PackDataToClient(Player[m_nExercisePKAim].m_nNetConnectIdx, (BYTE*)&sState, sState.m_wLength + 1);

		Player[m_nExercisePKAim].m_cPK.m_nExercisePKFlag = FALSE;
		Player[m_nExercisePKAim].m_cPK.m_nExercisePKAim = 0;
	}
	this->m_nExercisePKFlag = FALSE;
	this->m_nExercisePKAim = 0;
}

//-------------------------------------------------------------------------
//	功能：打开切磋PK
//-------------------------------------------------------------------------
BOOL	KPlayerPK::ExercisePKOpen(int nAim)
{
	// 自己是新手
	if (Npc[Player[m_nPlayerIndex].m_nIndex].m_Camp == camp_begin)
		return FALSE;
	// 正在切磋
	if (m_nExercisePKFlag == TRUE)
		return FALSE;
	// 出错
	if (nAim <= 0 || nAim >= MAX_PLAYER || Player[nAim].m_nIndex < 1)
		return FALSE;
	// 对方是新手
	if (Npc[Player[nAim].m_nIndex].m_Camp == camp_begin)
		return FALSE;

	this->EnmityPKClose();
	Player[nAim].m_cPK.EnmityPKClose();
	this->m_nExercisePKFlag = TRUE;
	this->m_nExercisePKAim = nAim;
	Player[nAim].m_cPK.m_nExercisePKFlag = TRUE;
	Player[nAim].m_cPK.m_nExercisePKAim = this->m_nPlayerIndex;

	PK_EXERCISE_STATE_SYNC	sState;
	sState.ProtocolType = s2c_pksyncexercisestate;
	sState.m_btState = 1;

	sState.m_dwNpcID = Npc[Player[m_nPlayerIndex].m_nIndex].m_dwID;
	strcpy(sState.m_szName, Npc[Player[m_nPlayerIndex].m_nIndex].Name);
	sState.m_wLength = sizeof(PK_EXERCISE_STATE_SYNC) - 1 - sizeof(sState.m_szName) + strlen(sState.m_szName);
	g_pServer->PackDataToClient(Player[nAim].m_nNetConnectIdx, (BYTE*)&sState, sState.m_wLength + 1);

	sState.m_dwNpcID = Npc[Player[nAim].m_nIndex].m_dwID;
	strcpy(sState.m_szName, Npc[Player[nAim].m_nIndex].Name);
	sState.m_wLength = sizeof(PK_EXERCISE_STATE_SYNC) - 1 - sizeof(sState.m_szName) + strlen(sState.m_szName);
	g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sState, sState.m_wLength + 1);

	return TRUE;
}

//-------------------------------------------------------------------------
//	功能：获得切磋目标 if 目标==0 不在切磋状态 else 得到切磋目标
//-------------------------------------------------------------------------
int		KPlayerPK::GetExercisePKAim()
{
	return this->m_nExercisePKAim;
}

//-------------------------------------------------------------------------
//	功能：设定PK值
//-------------------------------------------------------------------------
void	KPlayerPK::SetPKValue(int nValue)
{
	this->m_nPKValue = (nValue < 0 ? 0 : nValue);
	if (m_nPKValue > MAX_DEATH_PUNISH_PK_VALUE)
		m_nPKValue = MAX_DEATH_PUNISH_PK_VALUE;

	if (nValue > 0 && 
		(m_nPKValue >= NpcSet.m_nNotFightWhenHightPK || 
		((m_nPKValue >= NpcSet.m_nPKNotSwitchPKWhenLock) && Player[m_nPlayerIndex].GetLockState())) && 
		m_nNormalPKFlag != enumPKNormal)
		this->SetNormalPKState(enumPKNormal);

	PK_VALUE_SYNC	sValue;
	sValue.ProtocolType = s2c_pksyncpkvalue;
	sValue.m_nPKValue = m_nPKValue;
	g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sValue, sizeof(PK_VALUE_SYNC));
}

//-------------------------------------------------------------------------
//	功能：获得PK值
//-------------------------------------------------------------------------
int		KPlayerPK::GetPKValue()
{
	return m_nPKValue;
}

//-------------------------------------------------------------------------
//	功能：增加(或减少)PK值，PK值最多减少到0
//-------------------------------------------------------------------------
void	KPlayerPK::AddPKValue(int nAdd)
{
	m_nPKValue += nAdd;

	if (m_nPKValue < 0)
		m_nPKValue = 0;
	if (m_nPKValue > MAX_DEATH_PUNISH_PK_VALUE)
		m_nPKValue = MAX_DEATH_PUNISH_PK_VALUE;

	if (nAdd > 0 && 
		(m_nPKValue >= NpcSet.m_nNotFightWhenHightPK || 
		((m_nPKValue >= NpcSet.m_nPKNotSwitchPKWhenLock) && Player[m_nPlayerIndex].GetLockState())) && 
		m_nNormalPKFlag != enumPKNormal)
		this->SetNormalPKState(enumPKNormal);

	PK_VALUE_SYNC	sValue;
	sValue.ProtocolType = s2c_pksyncpkvalue;
	sValue.m_nPKValue = m_nPKValue;
	g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sValue, sizeof(PK_VALUE_SYNC));
}

//-------------------------------------------------------------------------
//	功能：关闭仇杀PK和切磋PK
//-------------------------------------------------------------------------
void	KPlayerPK::CloseAll()
{
	ExercisePKClose();
	EnmityPKClose(TRUE);
}

//-------------------------------------------------------------------------
//	功能：仇杀倒计时
//-------------------------------------------------------------------------
void	KPlayerPK::EnmityPKCountDown()
{
	if (m_nEnmityPKState == enumPK_ENMITY_STATE_TIME)
	{
		m_nEnmityPKTime--;
		if (m_nEnmityPKTime > 0)
			return;
		m_nEnmityPKTime = PK_ANMITY_CLOSE_TIME;
		m_nEnmityPKState = enumPK_ENMITY_STATE_PKING;

		if (Player[m_nEnmityPKAim].m_nIndex <= 0)
			return;

		if(this->m_bEnmitySpar && NpcSet.m_nSparPacific)
			Npc[Player[m_nPlayerIndex].m_nIndex].m_FightMode = enumFightActive;

		if(Player[m_nEnmityPKAim].m_cPK.m_bEnmitySpar && NpcSet.m_nSparPacific)
			Npc[Player[m_nEnmityPKAim].m_nIndex].m_FightMode = enumFightActive;

		PK_ENMITY_STATE_SYNC	sState;
		sState.ProtocolType = s2c_pksyncenmitystate;
		sState.m_btState = enumPK_ENMITY_STATE_PKING;
		sState.m_dwNpcID = Npc[Player[m_nEnmityPKAim].m_nIndex].m_dwID;
		strcpy(sState.m_szName, Npc[Player[m_nEnmityPKAim].m_nIndex].Name);
		sState.m_bSpar = m_bEnmitySpar;
		sState.m_wLength = sizeof(PK_ENMITY_STATE_SYNC) - 1 - sizeof(sState.m_szName) + strlen(sState.m_szName);
		g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sState, sState.m_wLength + 1);
	}
	else if (m_nEnmityPKState == enumPK_ENMITY_STATE_PKING)
	{
		m_nEnmityPKTime--;
		if (m_nEnmityPKTime > 0)
			return;
		m_nEnmityPKTime = 0;
		ExercisePKClose();
	}
}

#endif

#ifndef _SERVER

//-------------------------------------------------------------------------
//	功能：初始化
//-------------------------------------------------------------------------
void	KPlayerPK::Init()
{
	//m_nNormalPKFlag			= 0;
	m_nEnmityPKState		= enumPK_ENMITY_STATE_CLOSE;
	m_nEnmityPKAim			= 0;
	m_nEnmityPKTime			= 0;
	m_szEnmityAimName[0]	= 0;
	m_nExercisePKFlag		= 0;
	m_nExercisePKAim		= 0;
	m_szExerciseAimName[0]	= 0;
	//m_nPKValue				= 0;
}

void	KPlayerPK::Active()
{
	EnmityPKCountDown();
}

//-------------------------------------------------------------------------
//	功能：设定正常PK状态 TRUE 打开，可以砍人  FALSE 关闭，不可以砍人
//-------------------------------------------------------------------------
void	KPlayerPK::SetNormalPKState(int bFlag, BOOL bShowMsg/* = TRUE*/)
{
	if (m_nNormalPKFlag == bFlag)
		return;

	m_nNormalPKFlag = bFlag;
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPKFlag = bFlag;
	if (bShowMsg)
	{
		KSystemMessage	sMsg;
		if (bFlag == enumPKNormal)
			sprintf(sMsg.szMessage, MSG_PK_NORMAL_FLAG_CLOSE);
		else if (bFlag == enumPKWar)
			sprintf(sMsg.szMessage, MSG_PK_NORMAL_FLAG_OPEN);
		else if (bFlag == enumPKMurder)
			sprintf(sMsg.szMessage, MSG_PK_NORMAL_FLAG_DS);
		else if (bFlag == enumPKTongWar)
			sprintf(sMsg.szMessage, MSG_PK_NORMAL_FLAG_TONGWAR);
		else
			sprintf(sMsg.szMessage, MSG_PK_NORMAL_FLAG_UNKNOWN);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
	}
}

//-------------------------------------------------------------------------
//	功能：向服务器申请打开、关闭正常PK状态
//-------------------------------------------------------------------------
void	KPlayerPK::ApplySetNormalPKState(BYTE bFlag)
{
//	SetNormalPKState(bFlag, FALSE);
	PK_APPLY_NORMAL_FLAG_COMMAND	sApply;
	sApply.ProtocolType = c2s_pkapplychangenormalflag;
	sApply.m_btFlag = bFlag;
	if (g_pClient)
		g_pClient->SendPackToServer(&sApply, sizeof(PK_APPLY_NORMAL_FLAG_COMMAND));

}

//-------------------------------------------------------------------------
//	功能：获得正常PK状态 TRUE 打开，可以砍人  FALSE 关闭，不可以砍人
//-------------------------------------------------------------------------
BOOL	KPlayerPK::GetNormalPKState()
{
	return this->m_nNormalPKFlag;
}

//-------------------------------------------------------------------------
//	功能：向服务器申请仇杀某人
//-------------------------------------------------------------------------
void	KPlayerPK::ApplyEnmityPK(int nNpcID)
{
	if (m_nEnmityPKState != enumPK_ENMITY_STATE_CLOSE)
	{
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, MSG_PK_ERROR_4);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		return;
	}
	if (!Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_FightMode)
	{
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, MSG_PK_ERROR_1);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		return;
	}
	int		nIdx = NpcSet.SearchID(nNpcID);
	if (nIdx == 0 || Npc[nIdx].m_Kind != kind_player)
		return;

	PK_APPLY_ENMITY_COMMAND	sApply;
	sApply.ProtocolType = c2s_pkapplyenmity;
	sApply.m_dwNpcID = nNpcID;
	sApply.m_bRefuse = FALSE;
	sApply.m_bSpar = FALSE;
	if (g_pClient)
		g_pClient->SendPackToServer(&sApply, sizeof(PK_APPLY_ENMITY_COMMAND));
}

void	KPlayerPK::SparApplyStart(int nNpcID)
{
	if (m_nEnmityPKState != enumPK_ENMITY_STATE_CLOSE)
	{
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, MSG_PK_ERROR_4);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		return;
	}

	int		nNpcIdx = NpcSet.SearchID(nNpcID);
	if (nNpcIdx == 0 || Npc[nNpcIdx].m_Kind != kind_player)
		return;
	SPAR_APPLY_START_COMMAND	sStart;
	sStart.ProtocolType = c2s_sparapplystart;
	sStart.m_dwID = Npc[nNpcIdx].m_dwID;
	if (g_pClient)
		g_pClient->SendPackToServer(&sStart, sizeof(SPAR_APPLY_START_COMMAND));
	
	KSystemMessage	sMsg;
	sprintf(sMsg.szMessage, MSG_SPAR_SEND_APPLY, Npc[nNpcIdx].Name);
	sMsg.eType = SMT_NORMAL;
	sMsg.byConfirmType = SMCT_NONE;
	sMsg.byPriority = 0;
	sMsg.byParamSize = 0;
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
}

void	KPlayerPK::ReplyInvite(int nNpcID, int nResult)
{
	if (m_nEnmityPKState != enumPK_ENMITY_STATE_CLOSE)
	{
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, MSG_PK_ERROR_4);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		return;
	}

	int		nIdx = NpcSet.SearchID(nNpcID);
	if (nIdx == 0 || Npc[nIdx].m_Kind != kind_player)
		return;

	PK_APPLY_ENMITY_COMMAND	sApply;
	sApply.ProtocolType = c2s_pkapplyenmity;
	sApply.m_dwNpcID = nNpcID;
	sApply.m_bRefuse = (nResult == 0);
	sApply.m_bSpar = TRUE;
	if (g_pClient)
		g_pClient->SendPackToServer(&sApply, sizeof(PK_APPLY_ENMITY_COMMAND));
}
//-------------------------------------------------------------------------
//	功能：设定仇杀PK状态
//-------------------------------------------------------------------------
void	KPlayerPK::SetEnmityPKState(int nState, int nNpcID/* = 0*/, char *lpszName/* = NULL*/, BOOL bAim /* = 0*/, BOOL bSpar)
{
	if (nState == enumPK_ENMITY_STATE_CLOSE)
	{
		if (m_nEnmityPKState != enumPK_ENMITY_STATE_CLOSE)
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			if (bSpar)
				sprintf(sMsg.szMessage, MSG_PK_SPAR_ENMITY_CLOSE);
			else
				sprintf(sMsg.szMessage, MSG_PK_ENMITY_CLOSE);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}

		m_nEnmityPKState = enumPK_ENMITY_STATE_CLOSE;
		m_nEnmityPKAim = 0;
		m_nEnmityPKTime = 0;
		m_szEnmityAimName[0] = 0;
	}
	else if (nState == enumPK_ENMITY_STATE_TIME)
	{
		m_nEnmityPKState = enumPK_ENMITY_STATE_CLOSE;
		m_nEnmityPKAim = nNpcID;
		if (bSpar)
			m_nEnmityPKTime = PK_ANMITY_TIME / 2;
		else
			m_nEnmityPKTime = PK_ANMITY_TIME;
		m_szEnmityAimName[0] = 0;
		if (lpszName)
			strcpy(m_szEnmityAimName, lpszName);

		KSystemMessage	sMsg;
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		if (bSpar)
			sprintf(sMsg.szMessage, MSG_PK_SPAR_ENMITY_SUCCESS_1, m_szEnmityAimName);
		else
			sprintf(sMsg.szMessage, MSG_PK_ENMITY_SUCCESS_1, m_szEnmityAimName);
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

		if (bSpar)
			sprintf(sMsg.szMessage, MSG_PK_SPAR_ENMITY_SUCCESS_2);
		else
			sprintf(sMsg.szMessage, MSG_PK_ENMITY_SUCCESS_2);
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
	}
	else	// if (nState == enumPK_ENMITY_STATE_PKING)
	{
		m_nEnmityPKState = enumPK_ENMITY_STATE_PKING;
		m_nEnmityPKTime = 0;
		if (nNpcID > 0)
			m_nEnmityPKAim = nNpcID;
		if (lpszName)
			strcpy(m_szEnmityAimName, lpszName);
		KSystemMessage	sMsg;
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		if (bSpar)
			sprintf(sMsg.szMessage, MSG_PK_SPAR_ENMITY_OPEN);
		else
			sprintf(sMsg.szMessage, MSG_PK_ENMITY_OPEN);
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
	}
}

//-------------------------------------------------------------------------
//	功能：仇杀倒计时
//-------------------------------------------------------------------------
void	KPlayerPK::EnmityPKCountDown()
{
	if (m_nEnmityPKState == enumPK_ENMITY_STATE_TIME)
	{
		m_nEnmityPKTime--;
		if (m_nEnmityPKTime < 0)
			m_nEnmityPKTime = 0;
	}
}

//-------------------------------------------------------------------------
//	功能：设定切磋状态
//-------------------------------------------------------------------------
void	KPlayerPK::SetExercisePKState(int nState, int nNpcID/* = 0*/, char *lpszName/* = NULL*/)
{
	SetEnmityPKState(enumPK_ENMITY_STATE_CLOSE);

	if (nState)
	{
		m_nExercisePKFlag = 1;
		m_nExercisePKAim = nNpcID;
		m_szExerciseAimName[0] = 0;
		if (lpszName)
			strcpy(m_szExerciseAimName, lpszName);
	}
	else
	{
		m_nExercisePKFlag = 0;
		m_nExercisePKAim = 0;
		m_szExerciseAimName[0] = 0;
	}
}

//-------------------------------------------------------------------------
//	功能：设定PK值
//-------------------------------------------------------------------------
void	KPlayerPK::SetPKValue(int nValue)
{
	if (m_nPKValue == nValue)
		return;

	this->m_nPKValue = (nValue < 0 ? 0 : nValue);
	if (m_nPKValue > MAX_DEATH_PUNISH_PK_VALUE)
		m_nPKValue = MAX_DEATH_PUNISH_PK_VALUE;

	KSystemMessage	sMsg;
	sprintf(sMsg.szMessage, MSG_PK_VALUE, m_nPKValue);
	sMsg.eType = SMT_NORMAL;
	sMsg.byConfirmType = SMCT_NONE;
	sMsg.byPriority = 0;
	sMsg.byParamSize = 0;
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

}

#endif













