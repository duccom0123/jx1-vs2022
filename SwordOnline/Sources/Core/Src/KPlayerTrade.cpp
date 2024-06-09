//---------------------------------------------------------------------------
// Sword3 Engine (c) 2003 by Kingsoft
//
// File:	KPlayerTrade.cpp
// Date:	2003.02.17
// Code:	边城浪子
// Desc:	Trade Class
//---------------------------------------------------------------------------

#include	"KCore.h"
#include	"KNpc.h"
#include	"KPlayer.h"
#include	"KPlayerSet.h"
#include	"KPlayerTrade.h"
//#include	"MyAssert.h"
#ifndef _SERVER
#include	"CoreShell.h"
#endif
KTrade::KTrade()
{
	Release();
#ifdef _SERVER
	m_nApplyIdx = -1;
#endif
}

void	KTrade::Release()
{
	m_nIsTrading			= 0;
	m_nTradeDest			= -1;
	m_nTradeState			= 0;
	m_nTradeLock			= 0;
	m_bTradeFolkGame		= FALSE;
#ifdef _SERVER
	m_uTradeOkTime			= 0;
	m_nTradeFolkId			= -1;
#else
	m_nBackEquipMoney		= 0;
	m_nBackRepositoryMoney	= 0;
	m_nTradeDestState		= 0;
	m_nTradeDestLock		= 0;
	m_szDestName[0]			= 0;
#endif
}

BOOL	KTrade::StartTrade(int nDest, BOOL bFolkGame, int nEquipMoney, int nRepositoryMoney)
{
	if (m_nIsTrading)
		return FALSE;
	if (nDest < 0)
		return FALSE;

	m_nIsTrading = 1;
	m_nTradeDest = nDest;
	m_nTradeState = 0;
	m_nTradeLock = 0;
	m_bTradeFolkGame = bFolkGame;
#ifndef _SERVER
	m_nBackEquipMoney = nEquipMoney;
	m_nBackRepositoryMoney = nRepositoryMoney;
	m_nTradeDestState = 0;
	m_nTradeDestLock = 0;
	int nIdx = NpcSet.SearchID(nDest);
	if (nIdx > 0)
		strcpy(m_szDestName, Npc[nIdx].Name);
	else
		m_szDestName[0] = 0;
#endif
	return TRUE;
}

void	KTrade::Lock(BOOL bFlag)
{
	if (bFlag)
	{
		m_nTradeLock = 1;
	}
	else
	{
		m_nTradeLock = 0;
#ifndef _SERVER
		m_nTradeDestLock = 0;
#endif
	}
}

#ifndef _SERVER
void	KTrade::SetTradeState(TRADE_STATE_SYNC *pPress)
{
	if(!pPress)
		return;

	if (pPress->m_btSelfLock)
	{
		if (m_nTradeLock == 0)
		{
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_TRADE_SELF_LOCK);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
	}

	if (pPress->m_btDestLock)
	{
		if (m_nTradeDestLock == 0)
		{
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_TRADE_DEST_LOCK, m_szDestName);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
	}

	this->m_nTradeLock = (pPress->m_btSelfLock != 0);
	this->m_nTradeDestLock = (pPress->m_btDestLock != 0);
	this->m_nTradeState = (pPress->m_btSelfOk != 0);
	this->m_nTradeDestState = (pPress->m_btDestOk != 0);
	// 通知界面按钮状态改变
	CoreDataChanged(GDCNI_TRADE_OPER_DATA, pPress->m_bSelfAskFolkGame, pPress->m_bDestReplyFolkGame);
}
#endif

#ifndef _SERVER
void	KTrade::ReplyInvite(int nDestIdx, int nReply, BOOL bFolkGame)
{
	if (nDestIdx < 0)
		return;
	TRADE_REPLY_START_COMMAND	sReply;
	sReply.ProtocolType = c2s_tradereplystart;
	sReply.m_bDecision = (nReply != 0);
	sReply.m_nDestIdx = nDestIdx;
	sReply.m_bFolkGame = bFolkGame;
	if (g_pClient)
		g_pClient->SendPackToServer(&sReply, sizeof(TRADE_REPLY_START_COMMAND));
}
#endif
