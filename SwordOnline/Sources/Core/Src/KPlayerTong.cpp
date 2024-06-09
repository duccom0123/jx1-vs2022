//---------------------------------------------------------------------------
// Sword3 Engine (c) 2003 by Kingsoft
//
// File:	KPlayerTong.cpp
// Date:	2003.08.12
// Code:	±ﬂ≥«¿À◊”
// Desc:	KPlayerTong Class
//---------------------------------------------------------------------------

#include	"KCore.h"
#include	"KNpc.h"
#include	"KItem.h"
#include	"KPlayer.h"
#include	"KItemList.h"
#include	"KItemSet.h"
#include	"KTongData.h"
#include	"KPlayerTong.h"
#include	"KTongProtocol.h"
#ifndef _SERVER
#include	"CoreShell.h"
#endif

#define		defTONG_PREFIX_LEVEL			1
#define		defFuncShowNormalMsg(str)		\
	{										\
		KSystemMessage	sMsg;				\
		sMsg.eType = SMT_NORMAL;			\
		sMsg.byConfirmType = SMCT_NONE;		\
		sMsg.byPriority = 0;				\
		sMsg.byParamSize = 0;				\
		sprintf(sMsg.szMessage, str);		\
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);\
	}

//-------------------------------------------------------------------------
//	π¶ƒ‹£∫≥ı ºªØ
//-------------------------------------------------------------------------
void	KPlayerTong::Init(int nPlayerIdx)
{
	m_nPlayerIndex = nPlayerIdx;

	Clear();
}

//-------------------------------------------------------------------------
//	π¶ƒ‹£∫«Âø’
//-------------------------------------------------------------------------
void	KPlayerTong::Clear()
{
	m_nFlag				= 0;
	m_nFigure			= enumTONG_FIGURE_MEMBER;
	m_nCamp				= 0;
	m_dwMemberNum		= 0;	
	m_btManagerNum		= 0;
	m_btDirectorNum		= 0;
	m_dwLeaveTime		= 0;
	m_btLevel			= 0;
	m_dwTotalEff		= 0;
	m_dwTongNameID		= 0;
	m_szName[0]			= 0;
	m_szAgname[0]		= 0;
	m_szMasterName[0]	= 0;
	m_nApplyTo			= 0;
	m_dwMoney			= 0;
	m_nTongJiyuParam	= 0;
	m_szTongJiyuNotify[0]= 0;
	m_nJoinTm			= 0;
	m_nSaveEff		= 0;
	m_bRecruit			= TRUE;
	m_nTongParam		= 0;
#ifdef _SERVER
	m_nTongNationalEmblem = 0;
#endif
}

#ifndef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫…Í«Î¥¥Ω®∞Ôª·
//-------------------------------------------------------------------------
int KPlayerTong::ApplyCreateTong(int nCamp, char *lpszTongName)
{
	defFuncShowNormalMsg(MSG_TONG_APPLY_CREATE);
	// ∞Ôª·√˚Œ Ã‚
	if (!lpszTongName || !lpszTongName[0] || strlen(lpszTongName) > defTONG_NAME_LENGTH_12)
		return enumTONG_CREATE_ERROR_ID1;
	if (nCamp != camp_justice && nCamp != camp_evil && nCamp != camp_balance)
		return enumTONG_CREATE_ERROR_ID2;
	if (m_nFlag)
		return enumTONG_CREATE_ERROR_ID3;
	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentCamp != camp_free)
		return enumTONG_CREATE_ERROR_ID4;
	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Level < TongData.m_sTongParam.m_nLevel)
		return enumTONG_CREATE_ERROR_ID5;
	if ((int)Player[CLIENT_PLAYER_INDEX].m_nLeadLevel < TongData.m_sTongParam.m_nLeadLevel)
		return enumTONG_CREATE_ERROR_ID6;
	if (Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipmentMoney() < TongData.m_sTongParam.m_nMoney)
		return enumTONG_CREATE_ERROR_ID7;
	if (Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag)
		return enumTONG_CREATE_ERROR_ID8;
	if (Player[CLIENT_PLAYER_INDEX].m_ItemList.CountCommonItem(NATURE_NORMAL, TongData.m_sTongParam.m_nItemGenre, TongData.m_sTongParam.m_nItemDetail) <= 0)
		return enumTONG_CREATE_ERROR_ID9;
	if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
		return enumTONG_CREATE_ERROR_ID10;
	if (Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(TASKVALUE_STATTASK_REPUTE) < TongData.m_sTongParam.m_nRepute)
		return enumTONG_CREATE_ERROR_ID11;
	if (Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(TASKVALUE_STATTASK_FUYUAN) < TongData.m_sTongParam.m_nFuYuan)
		return enumTONG_CREATE_ERROR_ID12;

	TONG_APPLY_CREATE_COMMAND	sApply;
	sApply.ProtocolType = c2s_extendtong;
	sApply.m_wLength = sizeof(TONG_APPLY_CREATE_COMMAND) - 1;
	sApply.m_btMsgId = enumTONG_COMMAND_ID_APPLY_CREATE;
	sApply.m_btCamp = (BYTE)nCamp;
	sApply.m_btLevel = defTONG_PREFIX_LEVEL;
	strcpy(sApply.m_szName, lpszTongName);

	if (g_pClient)
		g_pClient->SendPackToServer(&sApply, sApply.m_wLength + 1);

	return -1;
}

void KPlayerTong::ForceCreateTong(BYTE* pMsg)
{
	if (!pMsg)
		return;

	TONG_CREATE_SYNC	*pCreat = (TONG_CREATE_SYNC*)pMsg;

	defFuncShowNormalMsg(MSG_TONG_APPLY_CREATE);

	if (!pCreat->m_szName || !pCreat->m_szName[0] || strlen(pCreat->m_szName) > defTONG_NAME_LENGTH_12)
		return;
	if (pCreat->m_btCamp != camp_justice && pCreat->m_btCamp != camp_evil && pCreat->m_btCamp != camp_balance)
		return;
	if (m_nFlag)
		return;

	TONG_APPLY_CREATE_COMMAND	sApply;
	sApply.ProtocolType = c2s_extendtong;
	sApply.m_wLength = sizeof(TONG_APPLY_CREATE_COMMAND) - 1;
	sApply.m_btMsgId = enumTONG_COMMAND_ID_FORCE_CREATE;
	sApply.m_btCamp = (BYTE)pCreat->m_btCamp;
	sApply.m_btLevel = defTONG_PREFIX_LEVEL;
	strcpy(sApply.m_szName, pCreat->m_szName);

	if (g_pClient)
		g_pClient->SendPackToServer(&sApply, sApply.m_wLength + 1);
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫…Í«Îº”»Î∞Ôª·
//-------------------------------------------------------------------------
BOOL	KPlayerTong::ApplyAddTong(DWORD dwNpcID)
{
	defFuncShowNormalMsg(MSG_TONG_APPLY_ADD);
	// “—æ≠ «∞Ôª·≥…‘±
	if (m_nFlag)
	{
		defFuncShowNormalMsg(MSG_TONG_APPLY_ADD_ERROR1);
		return FALSE;
	}
	// ◊‘º∫µƒ’Û”™Œ Ã‚
	if (Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag)
	{
		defFuncShowNormalMsg(MSG_TONG_APPLY_ADD_ERROR3);
		return FALSE;
	}

	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentCamp != camp_free)
	{
		defFuncShowNormalMsg(MSG_TONG_APPLY_ADD_ERROR2);
		return FALSE;
	}

	// Ωª“◊π˝≥Ã÷–
	if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
	{
		return FALSE;
	}
	// ◊È∂”≤ªƒ‹Ω®∞Ôª·

	if (Player[CLIENT_PLAYER_INDEX].m_cTong.m_dwLeaveTime && 
		Player[CLIENT_PLAYER_INDEX].m_cTong.m_dwLeaveTime > KSG_GetCurSec())
	{
		KSystemMessage	sMsg;				
		sMsg.eType = SMT_NORMAL;		
		sMsg.byConfirmType = SMCT_NONE;		
		sMsg.byPriority = 0;				
		sMsg.byParamSize = 0;				
		sprintf(sMsg.szMessage, MSG_TONG_APPLY_ADD_ERROR4, Player[CLIENT_PLAYER_INDEX].m_cTong.m_dwLeaveTime - KSG_GetCurSec());		
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		return FALSE;
	}

	TONG_APPLY_ADD_COMMAND	sApply;
	sApply.ProtocolType = c2s_extendtong;
	sApply.m_wLength = sizeof(TONG_APPLY_ADD_COMMAND) - 1;
	sApply.m_btMsgId = enumTONG_COMMAND_ID_APPLY_ADD;
	sApply.m_dwNpcID = dwNpcID;

	if (g_pClient)
		g_pClient->SendPackToServer(&sApply, sizeof(TONG_APPLY_ADD_COMMAND));

	return TRUE;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫≈–∂œ¥¥Ω®∞Ôª·Ãıº˛ «∑Ò≥…¡¢
//-------------------------------------------------------------------------
int		KPlayerTong::CheckCreateCondition(int nCamp, char *lpszTongName)
{
	if (Player[m_nPlayerIndex].m_nIndex <= 0)
		return enumTONG_CREATE_ERROR_ID1;
	// ∞Ôª·√˚Œ Ã‚
	if (!lpszTongName || !lpszTongName[0] || strlen(lpszTongName) > defTONG_NAME_LENGTH_12)
		return enumTONG_CREATE_ERROR_ID3;
	// ∞Ôª·’Û”™Œ Ã‚
	if (nCamp != camp_justice && nCamp != camp_evil && nCamp != camp_balance)
		return enumTONG_CREATE_ERROR_ID4;
	// “—æ≠ «∞Ôª·≥…‘±
	if (m_nFlag)
		return enumTONG_CREATE_ERROR_ID5;
	// ◊‘º∫µƒ’Û”™Œ Ã‚
	if (Npc[Player[m_nPlayerIndex].m_nIndex].m_CurrentCamp != camp_free ||
		Npc[Player[m_nPlayerIndex].m_nIndex].m_Camp != camp_free)
		return enumTONG_CREATE_ERROR_ID6;
	// µ»º∂Œ Ã‚
	if (Npc[Player[m_nPlayerIndex].m_nIndex].m_Level < TongData.m_sTongParam.m_nLevel ||
		(int)Player[m_nPlayerIndex].m_nLeadLevel < TongData.m_sTongParam.m_nLeadLevel)
		return enumTONG_CREATE_ERROR_ID7;
	// «ÆŒ Ã‚
	if (Player[m_nPlayerIndex].m_ItemList.GetEquipmentMoney() < TongData.m_sTongParam.m_nMoney)
		return enumTONG_CREATE_ERROR_ID8;
	// ◊È∂”≤ªƒ‹Ω®∞Ôª·
	if (Player[m_nPlayerIndex].m_cTeam.m_nFlag)
		return enumTONG_CREATE_ERROR_ID9;
	// Ωª“◊π˝≥Ã÷–
	if (Player[m_nPlayerIndex].CheckTrading())
		return enumTONG_CREATE_ERROR_ID10;
	if (Player[m_nPlayerIndex].m_cTask.GetSaveVal(TASKVALUE_STATTASK_REPUTE) < TongData.m_sTongParam.m_nRepute)
		return enumTONG_CREATE_ERROR_ID11;
	if (Player[m_nPlayerIndex].m_cTask.GetSaveVal(TASKVALUE_STATTASK_FUYUAN) < TongData.m_sTongParam.m_nFuYuan)
		return enumTONG_CREATE_ERROR_ID12;
	return 0;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫µ√µΩrelayÕ®÷™£¨∞Ôª·¥¥Ω®≥…π¶£¨¥¶¿Ìœ‡”¶ ˝æ›
//-------------------------------------------------------------------------
BOOL	KPlayerTong::Create(int nCamp, char *lpszTongName)
{
//	if (!CheckCreateCondition(nCamp, lpszTongName))
//		return FALSE;

	m_nFlag			= 1;
	m_nFigure		= enumTONG_FIGURE_MASTER;
	m_nCamp			= nCamp;
	m_btLevel		= defTONG_PREFIX_LEVEL;
	m_szAgname[0]	= 0;
	m_dwMoney		= 0;
	strcpy(m_szName, lpszTongName);
	strcpy(m_szMasterName, Npc[Player[m_nPlayerIndex].m_nIndex].Name);
	m_dwTongNameID	= g_FileName2Id(m_szName);

	Npc[Player[m_nPlayerIndex].m_nIndex].SetCamp(m_nCamp);
	Npc[Player[m_nPlayerIndex].m_nIndex].SetCurrentCamp(m_nCamp);
	Player[m_nPlayerIndex].m_ItemList.DecMoney(TongData.m_sTongParam.m_nMoney);
	Player[m_nPlayerIndex].m_ItemList.RemoveCommonItem(TongData.m_sTongParam.m_nItemCount, NATURE_NORMAL,
		TongData.m_sTongParam.m_nItemGenre, 
		TongData.m_sTongParam.m_nItemDetail);
	// Õ®÷™øÕªß∂À
	TONG_CREATE_SYNC	sCreate;
	sCreate.ProtocolType = s2c_tongcreate;
	sCreate.m_btCamp = nCamp;
	if (strlen(lpszTongName) < sizeof(sCreate.m_szName))
		strcpy(sCreate.m_szName, lpszTongName);
	else
	{
		memcpy(sCreate.m_szName, lpszTongName, sizeof(sCreate.m_szName) - 1);
		sCreate.m_szName[sizeof(sCreate.m_szName) - 1] = 0;
	}
	if (g_pServer)
		g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, &sCreate, sizeof(TONG_CREATE_SYNC));

	return TRUE;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫µ√µΩ∑˛ŒÒ∆˜Õ®÷™¥¥Ω®∞Ôª·
//-------------------------------------------------------------------------
void	KPlayerTong::Create(TONG_CREATE_SYNC *psCreate)
{
	if (!psCreate)
		return;

	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetCamp(psCreate->m_btCamp);
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetCurrentCamp(psCreate->m_btCamp);

	m_nFlag			= 1;
	m_nFigure		= enumTONG_FIGURE_MASTER;
	m_nCamp			= psCreate->m_btCamp;
	m_szAgname[0]	= 0;
	memset(m_szName, 0, sizeof(m_szName));
	memcpy(m_szName, psCreate->m_szName, sizeof(psCreate->m_szName));
	strcpy(m_szMasterName, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
	m_dwTongNameID	= g_FileName2Id(m_szName);

	// Õ®÷™ΩÁ√Ê∞Ôª·Ω®¡¢≥…π¶
	defFuncShowNormalMsg(MSG_TONG_CREATE_SUCCESS);

	// Õ®÷™∞Ôª·∆µµ¿
	CoreDataChanged(GDCNI_PLAYER_BASE_INFO, 0, 0);

}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫◊™∑¢º”»Î∞Ôª·…Í«Î∏¯∂‘∑ΩøÕªß∂À
//-------------------------------------------------------------------------
BOOL	KPlayerTong::TransferAddApply(DWORD dwNpcID)
{
	// “—æ≠ «∞Ôª·≥…‘±
	if (m_nFlag)
		return FALSE;
	// ◊‘º∫µƒ’Û”™Œ Ã‚
	if (Npc[Player[m_nPlayerIndex].m_nIndex].m_CurrentCamp != camp_free ||
		Npc[Player[m_nPlayerIndex].m_nIndex].m_Camp != camp_free)
		return FALSE;
	// Ωª“◊π˝≥Ã÷–
	if (Player[m_nPlayerIndex].CheckTrading())
		return FALSE;
	// ◊È∂”≤ªƒ‹º”»Î∞Ôª·
	if (Player[m_nPlayerIndex].m_cTeam.m_nFlag)
		return FALSE;

	int	nTarget = Player[m_nPlayerIndex].FindAroundPlayer(dwNpcID);
	if (nTarget == -1)
		return FALSE;
	if (!Player[nTarget].m_cTong.CheckAcceptAddApplyCondition())
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_TONG_APPLY_ADD_ERROR5;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
		g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		return FALSE;
	}

	int nLevel = Npc[Player[m_nPlayerIndex].m_nIndex].m_byTranslife * MAX_LEVEL + Npc[Player[m_nPlayerIndex].m_nIndex].m_Level;
	int nAcceptLv = KSG_StringGetValue(3,Player[nTarget].m_cTong.m_nTongJiyuParam,3);
	if(nAcceptLv && (nLevel> nAcceptLv))
	{
		Player[m_nPlayerIndex].m_cTong.JoinTong(Player[nTarget].m_cTong.m_szName);
		return TRUE;
	}

	m_nApplyTo = nTarget;

	// Õ®÷™øÕªß∂À
	TONG_APPLY_ADD_SYNC	sAdd;
	sAdd.ProtocolType = s2c_extendtong;
	sAdd.m_btMsgId = enumTONG_SYNC_ID_TRANSFER_ADD_APPLY;
	sAdd.m_nPlayerIdx = m_nPlayerIndex;
	strcpy(sAdd.m_szName, Npc[Player[m_nPlayerIndex].m_nIndex].Name);
	sAdd.m_wLength = sizeof(TONG_APPLY_ADD_SYNC) - 1 - sizeof(sAdd.m_szName) + strlen(sAdd.m_szName);
	if (g_pServer)
		g_pServer->PackDataToClient(Player[nTarget].m_nNetConnectIdx, &sAdd, sAdd.m_wLength + 1);

	return TRUE;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫≈–∂œ «∑Òø…“‘◊™∑¢±»Àµƒº”»Î∞Ôª·…Í«Î
//-------------------------------------------------------------------------
BOOL	KPlayerTong::CheckAcceptAddApplyCondition()
{
	if (!m_nFlag || !m_bRecruit || m_nFigure == enumTONG_FIGURE_MEMBER)
		return FALSE;

	return TRUE;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫ «∑ÒΩ” ‹≥…‘± bFlag == TRUE Ω” ‹ == FALSE ≤ªΩ” ‹
//-------------------------------------------------------------------------
void	KPlayerTong::AcceptMember(int nPlayerIdx, DWORD dwNameID, BOOL bFlag)
{
	if (nPlayerIdx <= 0)
		return;

	TONG_ACCEPT_MEMBER_COMMAND	sAccept;
	sAccept.ProtocolType	= c2s_extendtong;
	sAccept.m_wLength		= sizeof(TONG_ACCEPT_MEMBER_COMMAND) - 1;
	sAccept.m_btMsgId		= enumTONG_COMMAND_ID_ACCEPT_ADD;
	sAccept.m_nPlayerIdx	= nPlayerIdx;
	sAccept.m_dwNameID		= dwNameID;
	sAccept.m_btFlag		= (bFlag != 0);
	if (g_pClient)
		g_pClient->SendPackToServer(&sAccept, sAccept.m_wLength + 1);
}

void	KPlayerTong::ReplyMember(int nPlayerIdx, DWORD dwNameID, BOOL bFlag)
{
	if (nPlayerIdx <= 0)
		return;

	TONG_ACCEPT_MEMBER_COMMAND	sAccept;
	sAccept.ProtocolType	= c2s_extendtong;
	sAccept.m_wLength		= sizeof(TONG_ACCEPT_MEMBER_COMMAND) - 1;
	sAccept.m_btMsgId		= enumTONG_COMMAND_ID_ACCEPT_REPLY_ADD;
	sAccept.m_nPlayerIdx	= nPlayerIdx;
	sAccept.m_dwNameID		= dwNameID;
	sAccept.m_btFlag		= (bFlag != 0);

	if (g_pClient)
		g_pClient->SendPackToServer(&sAccept, sAccept.m_wLength + 1);
}
#endif

#ifndef _SERVER
void	KPlayerTong::JoinTongReply(int nNpcIdx)
{
	if (nNpcIdx < 0 || nNpcIdx >= MAX_NPC)
		return;
	TONG_JOIN_REPLY	sJoin;
	sJoin.ProtocolType = c2s_jointongreply;
	sJoin.m_dwID = Npc[nNpcIdx].m_dwID;
	if (g_pClient)
		g_pClient->SendPackToServer(&sJoin, sizeof(TONG_JOIN_REPLY));
	
	KSystemMessage	sMsg;
	sprintf(sMsg.szMessage, MSG_TONG_SEND_APPLY, Npc[nNpcIdx].Name);
	sMsg.eType = SMT_NORMAL;
	sMsg.byConfirmType = SMCT_NONE;
	sMsg.byPriority = 0;
	sMsg.byParamSize = 0;
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
}

#endif


#ifndef _SERVER
void	KPlayerTong::s2cJoinTong(BYTE* pMsg)
{
	if (!pMsg)
		return;
	S2C_JOIN_TONG	*pJoin = (S2C_JOIN_TONG*)pMsg;

	JOIN_TONG_SYNC	sApply;
	sApply.ProtocolType = c2s_extendtong;
	sApply.m_btMsgId = enumTONG_COMMAND_ID_JOIN_TONG;
	strcpy(sApply.m_cTongName, pJoin->m_cTongName);
	sApply.m_wLength = sizeof(JOIN_TONG_SYNC) - 1;

	if (g_pClient)
		g_pClient->SendPackToServer(&sApply, sizeof(JOIN_TONG_SYNC));
}
//---------------------------------------------------------------------
//	π¶ƒ‹£∫ ’µΩ∑˛ŒÒ∆˜Õ®÷™”–»À…Í«ÎΩª“◊
//---------------------------------------------------------------------
void	KPlayerTong::s2cJoinTongReply(BYTE* pMsg)
{
	if (!pMsg)
		return;

	S2C_TONG_JOIN_REPLY	*pJoin = (S2C_TONG_JOIN_REPLY*)pMsg;
	int		nNpcIdx;
	nNpcIdx = NpcSet.SearchID(pJoin->m_dwID);
	if (nNpcIdx == 0)
		return;
	
	KSystemMessage	sMsg;
	KUiPlayerItem	sPlayer;
	
	strcpy(sPlayer.Name, Npc[nNpcIdx].Name);
	sPlayer.nIndex = pJoin->m_Index;
	sPlayer.uId = 0;
	sPlayer.nData = 0;
	
	sprintf(sMsg.szMessage, MSG_TONG_REV_APPLY, Npc[nNpcIdx].Name);
	sMsg.eType = SMT_CLIQUE;
	sMsg.byConfirmType = SMCT_UI_TONG_JOIN_APPLY;
	sMsg.byPriority = 3;
	sMsg.byParamSize = sizeof(KUiPlayerItem);
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, (int)&sPlayer);
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫∑¢œ˚œ¢Õ®÷™æ‹æ¯ƒ≥»À…Í«Î
//-------------------------------------------------------------------------
void	KPlayerTong::SendRefuseMessage(int nPlayerIdx, DWORD dwNameID)
{
	if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER)
		return;
	if (Player[nPlayerIdx].m_cTong.m_nApplyTo != m_nPlayerIndex ||
		Player[nPlayerIdx].m_nIndex <= 0 ||
		Player[m_nPlayerIndex].m_nIndex <= 0)
		return;
	if (g_FileName2Id(Npc[Player[nPlayerIdx].m_nIndex].Name) != dwNameID)
		return;
	int nLength = strlen(Npc[Player[m_nPlayerIndex].m_nIndex].Name);
	SHOW_MSG_SYNC	sMsg;
	sMsg.ProtocolType = s2c_msgshow;
	sMsg.m_wMsgID = enumMSG_ID_TONG_REFUSE_ADD;
	sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID) + nLength;
	sMsg.m_lpBuf = new BYTE[sMsg.m_wLength + 1];

	memcpy(sMsg.m_lpBuf, &sMsg, sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID));
	memcpy((char*)sMsg.m_lpBuf + sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID), Npc[Player[m_nPlayerIndex].m_nIndex].Name, nLength);

	if (g_pServer)
		g_pServer->PackDataToClient(Player[nPlayerIdx].m_nNetConnectIdx, sMsg.m_lpBuf, sMsg.m_wLength + 1);
}
#endif

void	KPlayerTong::GetTongName(char *lpszGetName)
{
	if (!lpszGetName)
		return;
	if (!m_nFlag)
	{
		lpszGetName[0] = 0;
		return;
	}

	strcpy(lpszGetName, m_szName);
}

void	KPlayerTong::GetTongAgname(char *lpszGetAgname)
{
	if (!lpszGetAgname)
		return;
	if (!m_nFlag)
	{
		lpszGetAgname[0] = 0;
		return;
	}

	strcpy(lpszGetAgname, m_szAgname);
}

#ifdef _SERVER
void	KPlayerTong::JoinTong(char* cTongName)
{
	if (Player[m_nPlayerIndex].CheckTrading())
		return;

	S2C_JOIN_TONG	sJoin;
	sJoin.ProtocolType = s2c_jointong;
	strcpy(sJoin.m_cTongName, cTongName);
	if (g_pServer)
		g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sJoin, sizeof(S2C_JOIN_TONG));
}
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫ ’µΩøÕªß∂À…Í«Îø™ ºΩª“◊
//-------------------------------------------------------------------------
void	KPlayerTong::JoinTongReply(BYTE* pProtocol)
{
	if (Player[m_nPlayerIndex].CheckTrading())
		return;
	
	if (!CheckAcceptAddApplyCondition())
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_TONG_APPLY_ADD_ERROR6;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
		g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		return;
	}

	TONG_JOIN_REPLY	*pJoin = (TONG_JOIN_REPLY*)pProtocol;
	
	int		nPlayer;
	nPlayer = Player[m_nPlayerIndex].FindAroundPlayer(pJoin->m_dwID);
	if (nPlayer < 0)
		return;

	m_nApplyTo = nPlayer;
	S2C_TONG_JOIN_REPLY	sJoin;
	sJoin.ProtocolType = s2c_jointongreply;
	sJoin.m_Index = m_nPlayerIndex;
	sJoin.m_dwID = Npc[Player[m_nPlayerIndex].m_nIndex].m_dwID;
	if (g_pServer)
		g_pServer->PackDataToClient(Player[nPlayer].m_nNetConnectIdx, (BYTE*)&sJoin, sizeof(S2C_TONG_JOIN_REPLY));
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫≈–∂œ±»Àº”»Î◊‘º∫∞Ôª·Ãıº˛ «∑Ò≥…¡¢
//-------------------------------------------------------------------------
BOOL	KPlayerTong::CheckJoinTong()
{
//--------------------- ◊‘º∫µƒÃıº˛ -----------------------
	if (m_nFlag)
		return FALSE;

	// ◊‘º∫µƒ’Û”™Œ Ã‚
	if (Npc[Player[m_nPlayerIndex].m_nIndex].m_CurrentCamp != camp_free ||
		Npc[Player[m_nPlayerIndex].m_nIndex].m_Camp != camp_free)
		return FALSE;
	// Ωª“◊π˝≥Ã÷–
	if (Player[m_nPlayerIndex].CheckTrading())
		return FALSE;
	// ◊È∂”≤ªƒ‹º”»Î∞Ôª·
	if (Player[m_nPlayerIndex].m_cTeam.m_nFlag)
		return FALSE;

	return TRUE;
}

BOOL	KPlayerTong::CheckAddCondition(int nPlayerIdx)
{
//--------------------- ◊‘º∫µƒÃıº˛ -----------------------
	if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER)
		return FALSE;
	if (!m_nFlag || m_nFigure == enumTONG_FIGURE_MEMBER)
		return FALSE;

//--------------------- ∂‘∑ΩµƒÃıº˛ -----------------------
	//  «∑Ò…Í«Î¡À
	if (Player[nPlayerIdx].m_cTong.m_nApplyTo != this->m_nPlayerIndex)
		return FALSE;
	// “—æ≠ «∞Ôª·≥…‘±
	if (Player[nPlayerIdx].m_cTong.m_nFlag)
		return FALSE;
	// ◊‘º∫µƒ’Û”™Œ Ã‚
	if (Npc[Player[nPlayerIdx].m_nIndex].m_CurrentCamp != camp_free ||
		Npc[Player[nPlayerIdx].m_nIndex].m_Camp != camp_free)
		return FALSE;
	// Ωª“◊π˝≥Ã÷–
	if (Player[nPlayerIdx].CheckTrading())
		return FALSE;
	// ◊È∂”≤ªƒ‹º”»Î∞Ôª·
	if (Player[nPlayerIdx].m_cTeam.m_nFlag)
		return FALSE;

	return TRUE;
}

BOOL	KPlayerTong::CheckAddConditionReply(int nPlayerIdx)
{
//--------------------- ◊‘º∫µƒÃıº˛ -----------------------
	if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER)
		return FALSE;
	if (!m_nFlag || m_nFigure == enumTONG_FIGURE_MEMBER)
		return FALSE;

//--------------------- ∂‘∑ΩµƒÃıº˛ -----------------------
	//  «∑Ò…Í«Î¡À
	if (this->m_nApplyTo != nPlayerIdx)
		return FALSE;

	// “—æ≠ «∞Ôª·≥…‘±
	if (Player[nPlayerIdx].m_cTong.m_nFlag)
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_TONG_APPLY_ADD_ERROR1;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
		g_pServer->PackDataToClient(Player[nPlayerIdx].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		return FALSE;
	}
	// ◊‘º∫µƒ’Û”™Œ Ã‚
	if (Npc[Player[nPlayerIdx].m_nIndex].m_CurrentCamp != camp_free ||
		Npc[Player[nPlayerIdx].m_nIndex].m_Camp != camp_free)
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_TONG_APPLY_ADD_ERROR2;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
		g_pServer->PackDataToClient(Player[nPlayerIdx].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		return FALSE;
	}
	// Ωª“◊π˝≥Ã÷–
	if (Player[nPlayerIdx].CheckTrading())
		return FALSE;
	// ◊È∂”≤ªƒ‹º”»Î∞Ôª·
	if (Player[nPlayerIdx].m_cTeam.m_nFlag)
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_TONG_APPLY_ADD_ERROR3;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
		g_pServer->PackDataToClient(Player[nPlayerIdx].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		return FALSE;
	}
	if (Player[nPlayerIdx].m_cTong.m_dwLeaveTime && 
		Player[nPlayerIdx].m_cTong.m_dwLeaveTime > KSG_GetCurSec())
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_TONG_APPLY_ADD_ERROR7;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
		g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		return FALSE;
	}

	return TRUE;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫º”»Î∞Ôª·£¨≥…Œ™∆’Õ®∞Ô÷⁄
//-------------------------------------------------------------------------
BOOL	KPlayerTong::AddTong(int nCamp, int nMemberNum, DWORD dwMoney, int nLevel, DWORD dwTotalEff, BOOL bRecruit, int nTongParam, int nTongJiyuParam, char *lpszTongName, char *lpszMasterName, char *lpszAgname)
{
	if (!lpszTongName || !lpszMasterName || !lpszAgname)
		return FALSE;
	// ∞Ôª·’Û”™Œ Ã‚
	if (nCamp != camp_justice && nCamp != camp_evil && nCamp != camp_balance)
		return FALSE;

	m_nFlag		= 1;
	m_nFigure	= enumTONG_FIGURE_MEMBER;
	m_nCamp		= nCamp;
	m_dwMemberNum= nMemberNum;
	m_dwMoney	= dwMoney;
	m_btLevel	= nLevel;
	m_dwTotalEff = dwTotalEff;
	m_bRecruit	= bRecruit;
	m_nTongParam = nTongParam;
	m_nTongJiyuParam = nTongJiyuParam;
	m_nTongNationalEmblem = KSG_StringGetValue(1,m_nTongJiyuParam,5);
	strcpy(this->m_szName, lpszTongName);
	strcpy(this->m_szMasterName, lpszMasterName);
	strcpy(this->m_szAgname, lpszAgname);
	m_dwTongNameID	= g_FileName2Id(m_szName);

	Npc[Player[m_nPlayerIndex].m_nIndex].SetCamp(m_nCamp);
	Npc[Player[m_nPlayerIndex].m_nIndex].SetCurrentCamp(m_nCamp);
	Player[m_nPlayerIndex].m_cTong.m_dwLeaveTime = 0;
	// Õ®÷™øÕªß∂À
	TONG_ADD_SYNC	sAdd;
	sAdd.ProtocolType = s2c_extendtong;
	sAdd.m_wLength = sizeof(sAdd) - 1;
	sAdd.m_btMsgId = enumTONG_SYNC_ID_ADD;
	sAdd.m_btCamp = this->m_nCamp;
	strcpy(sAdd.m_szTongName, m_szName);
	strcpy(sAdd.m_szAgname, m_szAgname);
	strcpy(sAdd.m_szMaster, m_szMasterName);

	if (g_pServer)
		g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, &sAdd, sAdd.m_wLength + 1);

	return TRUE;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫º”»Î∞Ôª·£¨≥…Œ™∆’Õ®∞Ô÷⁄
//-------------------------------------------------------------------------
BOOL	KPlayerTong::AddTong(int nCamp, char *lpszTongName, char *lpszTongAgname, char *lpszMaster)
{
	if (!lpszTongName || !lpszTongName[0] || strlen(lpszTongName) > defTONG_NAME_LENGTH_12)
		return FALSE;
	// ∞Ôª·’Û”™Œ Ã‚
	if (nCamp != camp_justice && nCamp != camp_evil && nCamp != camp_balance)
		return FALSE;

	m_nFlag		= 1;
	m_nFigure	= enumTONG_FIGURE_MEMBER;
	m_nCamp		= nCamp;
	strcpy(m_szName, lpszTongName);
	if (!lpszTongAgname)
		this->m_szAgname[0] = 0;
	else
		strcpy(m_szAgname, lpszTongAgname);
	if (!lpszMaster)
		this->m_szMasterName[0] = 0;
	else
		strcpy(m_szMasterName, lpszMaster);
	m_dwTongNameID	= g_FileName2Id(m_szName);

	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetCamp(m_nCamp);
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetCurrentCamp(m_nCamp);

	defFuncShowNormalMsg(MSG_TONG_ADD_SUCCESS);

	return TRUE;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫…Í«Î»Œ√¸
//-------------------------------------------------------------------------
BOOL	KPlayerTong::ApplyInstate(int nCurFigure, int nCurPos, int nNewFigure, int nNewPos, char *lpszName)
{
	if (!lpszName)
		return FALSE;
	if (!m_nFlag)
		return FALSE;
	if (nCurFigure < 0 || nCurFigure >= enumTONG_FIGURE_NUM ||
		nNewFigure < 0 || nNewFigure >= enumTONG_FIGURE_NUM)
		return FALSE;
	if (nCurPos < 0 || nNewPos < 0)
		return FALSE;
	if (nCurFigure == nNewFigure && nCurPos == nNewPos)
		return FALSE;

	switch (m_nFigure)
	{
	case enumTONG_FIGURE_MEMBER:
		// ∞Ô÷⁄√ª”–»Œ√¸»®¡¶
		return FALSE;
	case enumTONG_FIGURE_MANAGER:
		// ∂”≥§√ª”–»Œ√¸»®¡¶
		return FALSE;
	case enumTONG_FIGURE_DIRECTOR:
		// ≥§¿œ÷ª”–∂‘∂”≥§°¢∞Ô÷⁄µƒ»Œ√¸»®¡¶
		if ((nCurFigure != enumTONG_FIGURE_MANAGER && nCurFigure != enumTONG_FIGURE_MEMBER) ||
			(nNewFigure != enumTONG_FIGURE_MANAGER && nNewFigure != enumTONG_FIGURE_MEMBER))
			return FALSE;
		if (nCurFigure == enumTONG_FIGURE_MANAGER && nCurPos >= defTONG_MAX_MANAGER)
			return FALSE;
		if (nNewFigure == enumTONG_FIGURE_MANAGER && nNewPos >= defTONG_MAX_MANAGER)
			return FALSE;
		break;
	case enumTONG_FIGURE_MASTER:
		// ∞Ô÷˜”–∂‘≥§¿œ°¢∂”≥§°¢∞Ô÷⁄µƒ»Œ√¸»®¡¶
		if (nCurFigure == enumTONG_FIGURE_MASTER || nNewFigure == enumTONG_FIGURE_MASTER)
			return FALSE;
		if (nCurFigure == enumTONG_FIGURE_MANAGER && nCurPos >= defTONG_MAX_MANAGER)
			return FALSE;
		if (nCurFigure == enumTONG_FIGURE_DIRECTOR && nCurPos >= defTONG_MAX_DIRECTOR)
			return FALSE;
		if (nNewFigure == enumTONG_FIGURE_MANAGER && nNewPos >= defTONG_MAX_MANAGER)
			return FALSE;
		if (nNewFigure == enumTONG_FIGURE_DIRECTOR && nNewPos >= defTONG_MAX_DIRECTOR)
			return FALSE;
		break;
	default:
		return FALSE;
	}

	TONG_APPLY_INSTATE_COMMAND	sApply;

	if (strlen(lpszName) >= sizeof(sApply.m_szName))
		return FALSE;
	sApply.ProtocolType = c2s_extendtong;
	sApply.m_btMsgId = enumTONG_COMMAND_ID_APPLY_INSTATE;
	sApply.m_dwTongNameID = g_FileName2Id(this->m_szName);
	sApply.m_btCurFigure = nCurFigure;
	sApply.m_btCurPos = nCurPos;
	sApply.m_btNewFigure = nNewFigure;
	sApply.m_btNewPos = nNewPos;
	memset(sApply.m_szName, 0, sizeof(sApply.m_szName));
	strcpy(sApply.m_szName, lpszName);
	sApply.m_wLength = sizeof(sApply) - 1;

	if (g_pClient)
		g_pClient->SendPackToServer(&sApply, sApply.m_wLength + 1);

	return TRUE;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫…Í«ÎÃﬂ»À
//-------------------------------------------------------------------------
BOOL	KPlayerTong::ApplyKick(int nCurFigure, int nCurPos, char *lpszName)
{
	KSystemMessage	sMsg;
	if (!lpszName || !lpszName[0] || strlen(lpszName) >= 32)
		return FALSE;
	if (!m_nFlag)
	{
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		sprintf(sMsg.szMessage, MSG_TONG_KICK_ERROR1);
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		return FALSE;
	}
	if (m_nFigure != enumTONG_FIGURE_MASTER && m_nFigure != enumTONG_FIGURE_DIRECTOR)
	{
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		sprintf(sMsg.szMessage, MSG_TONG_KICK_ERROR2);
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		return FALSE;
	}
	if (nCurFigure != enumTONG_FIGURE_MANAGER && nCurFigure != enumTONG_FIGURE_MEMBER)
	{
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		sprintf(sMsg.szMessage, MSG_TONG_KICK_ERROR3);
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		return FALSE;
	}
	if (nCurFigure == enumTONG_FIGURE_MANAGER && (nCurPos < 0 || nCurPos >= defTONG_MAX_MANAGER))
		return FALSE;

	TONG_APPLY_KICK_COMMAND	sKick;
	sKick.ProtocolType		= c2s_extendtong;
	sKick.m_wLength			= sizeof(sKick) - 1;
	sKick.m_btMsgId			= enumTONG_COMMAND_ID_APPLY_KICK;
	sKick.m_btFigure		= nCurFigure;
	sKick.m_btPos			= nCurPos;
	sKick.m_dwTongNameID	= g_FileName2Id(this->m_szName);
	strcpy(sKick.m_szName, lpszName);

	if (g_pClient)
		g_pClient->SendPackToServer(&sKick, sKick.m_wLength + 1);

	return TRUE;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫…Í«Î¥´Œª
//-------------------------------------------------------------------------
BOOL	KPlayerTong::ApplyChangeMaster(int nCurFigure, int nPos, char *lpszName)
{
	if (!lpszName || !lpszName[0] || strlen(lpszName) >= 32)
		return FALSE;
	if (!m_nFlag)
		return FALSE;
	if (m_nFigure != enumTONG_FIGURE_MASTER)
		return FALSE;
	TONG_APPLY_CHANGE_MASTER_COMMAND	sChange;
	sChange.ProtocolType	= c2s_extendtong;
	sChange.m_wLength		= sizeof(sChange) - 1;
	sChange.m_btMsgId		= enumTONG_COMMAND_ID_APPLY_CHANGE_MASTER;
	sChange.m_dwTongNameID	= g_FileName2Id(this->m_szName);
	sChange.m_btFigure		= nCurFigure;
	sChange.m_btPos			= nPos;
	strcpy(sChange.m_szName, lpszName);
	if (g_pClient)
		g_pClient->SendPackToServer(&sChange, sChange.m_wLength + 1);

	return TRUE;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫…Í«Î¿Îø™∞Ôª·
//-------------------------------------------------------------------------
BOOL	KPlayerTong::ApplyLeave()
{
	if (!m_nFlag)
		return FALSE;
	if (m_nFigure == enumTONG_FIGURE_MASTER)
	{
		defFuncShowNormalMsg(MSG_TONG_CANNOT_LEAVE1);
		return FALSE;
	}
	if (m_nFigure == enumTONG_FIGURE_DIRECTOR)
	{
		defFuncShowNormalMsg(MSG_TONG_CANNOT_LEAVE2);
		return FALSE;
	}
	if (Player[CLIENT_PLAYER_INDEX].m_ItemList.GetMoney(room_equipment) < TongData.m_sTongParam.m_nLeaveMoney)
	{
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, MSG_TONG_NO_MONEY);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		return FALSE;
	}

	TONG_APPLY_LEAVE_COMMAND	sLeave;
	sLeave.ProtocolType		= c2s_extendtong;
	sLeave.m_wLength		= sizeof(sLeave) - 1;
	sLeave.m_btMsgId		= enumTONG_COMMAND_ID_APPLY_LEAVE;
	sLeave.m_btFigure		= m_nFigure;
	sLeave.m_btPos			= 0;
	sLeave.m_dwTongNameID	= g_FileName2Id(this->m_szName);
	strcpy(sLeave.m_szName, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);

	if (g_pClient)
		g_pClient->SendPackToServer(&sLeave, sLeave.m_wLength + 1);

	return TRUE;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫…Í«ÎªÒµ√∞Ôª·–≈œ¢
//-------------------------------------------------------------------------
BOOL	KPlayerTong::ApplyInfo(int nInfoID, int nParam1, int nParam2, int nParam3, char *lpszName/*=NULL*/)
{
	if (nInfoID < 0 || nInfoID >= enumTONG_APPLY_INFO_ID_NUM)
		return FALSE;

	TONG_APPLY_INFO_COMMAND	sInfo;
	sInfo.ProtocolType = c2s_extendtong;
	sInfo.m_btMsgId = enumTONG_COMMAND_ID_APPLY_INFO;
	sInfo.m_btInfoID = nInfoID;
	sInfo.m_nParam1 = nParam1;
	sInfo.m_nParam2 = nParam2;
	sInfo.m_nParam3 = nParam3;

	switch (nInfoID)
	{
	case enumTONG_APPLY_INFO_ID_SELF:
		sInfo.m_wLength = sizeof(sInfo) - 1 - sizeof(sInfo.m_szBuf);
		if (g_pClient)
			g_pClient->SendPackToServer(&sInfo, sInfo.m_wLength + 1);
		break;

	case enumTONG_APPLY_INFO_ID_MASTER:
		break;

	case enumTONG_APPLY_INFO_ID_DIRECTOR:
		break;

	case enumTONG_APPLY_INFO_ID_MANAGER:
		if (!m_nFlag)
			break;

		sInfo.m_wLength = sizeof(sInfo) - 1 - sizeof(sInfo.m_szBuf);
		if (g_pClient)
			g_pClient->SendPackToServer(&sInfo, sInfo.m_wLength + 1);

		break;

	case enumTONG_APPLY_INFO_ID_MEMBER:
		if (!m_nFlag)
			break;

		sInfo.m_wLength = sizeof(sInfo) - 1 - sizeof(sInfo.m_szBuf);
		if (g_pClient)
			g_pClient->SendPackToServer(&sInfo, sInfo.m_wLength + 1);

		break;

	case enumTONG_APPLY_INFO_ID_ONE:
		if (!lpszName || !lpszName[0])
			break;
		if (strlen(lpszName) >= 32)
			break;
		strcpy(sInfo.m_szBuf, lpszName);
		sInfo.m_wLength = sizeof(sInfo) - 1 - sizeof(sInfo.m_szBuf) + strlen(lpszName);
		if (g_pClient)
			g_pClient->SendPackToServer(&sInfo, sInfo.m_wLength + 1);
		break;
	case enumTONG_APPLY_INFO_ID_TONG_HEAD:
		if (nParam1 <= 0 || nParam1 >= MAX_NPC)
			break;
		if (nParam1 != Player[CLIENT_PLAYER_INDEX].m_nIndex && !Npc[nParam1].m_bTongFlag)
			break;
		sInfo.m_nParam1 = Npc[nParam1].m_dwID;
		sInfo.m_wLength = sizeof(sInfo) - 1 - sizeof(sInfo.m_szBuf);
		if (g_pClient)
			g_pClient->SendPackToServer(&sInfo, sInfo.m_wLength + 1);
		break;
	}
	return TRUE;
}
#endif

DWORD	KPlayerTong::GetTongNameID()
{
	return (m_nFlag ? m_dwTongNameID : 0);
//	if (!m_nFlag)
//		return 0;
//	if (m_szName[0])
//		return 0;
//	return g_FileName2Id(m_szName);
}

//-------------------------------------------------------------------------
//	π¶ƒ‹£∫ «∑Ò”–»®¡¶≤È—Ø∂”≥§–≈œ¢
//-------------------------------------------------------------------------
BOOL	KPlayerTong::CanGetManagerInfo(DWORD dwTongNameID)
{
	if (!m_nFlag)
		return FALSE;
	if (dwTongNameID != g_FileName2Id(this->m_szName))
		return FALSE;
	if (m_nFigure <= enumTONG_FIGURE_MASTER && m_nFigure >= enumTONG_FIGURE_MEMBER)
		return TRUE;
	return FALSE;
}

//-------------------------------------------------------------------------
//	π¶ƒ‹£∫ «∑Ò”–»®¡¶≤È—Ø∞Ô÷⁄–≈œ¢
//-------------------------------------------------------------------------
BOOL	KPlayerTong::CanGetMemberInfo(DWORD dwTongNameID)
{
	if (!m_nFlag)
		return FALSE;
	if (dwTongNameID != g_FileName2Id(this->m_szName))
		return FALSE;
	if (m_nFigure <= enumTONG_FIGURE_MASTER && m_nFigure >= enumTONG_FIGURE_MEMBER)
		return TRUE;
	return TRUE;
}

#ifdef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫∏¯øÕªß∂À∑¢ÀÕ◊‘º∫‘⁄∞Ôª·÷–µƒ–≈œ¢
//-------------------------------------------------------------------------
void	KPlayerTong::SendSelfInfo()
{
	TONG_SELF_INFO_SYNC	sInfo;
	sInfo.ProtocolType = s2c_extendtong;
	sInfo.m_wLength = sizeof(sInfo) - 1;
	sInfo.m_btMsgId = enumTONG_SYNC_ID_SELF_INFO;
	sInfo.m_btJoinFlag = this->m_nFlag;
	sInfo.m_btFigure = this->m_nFigure;
	sInfo.m_btCamp = this->m_nCamp;
	sInfo.m_btLevel = this->m_btLevel;
	sInfo.m_btDirectorNum = this->m_btDirectorNum;
	sInfo.m_btManagerNum = this->m_btManagerNum;
	sInfo.m_dwMemberNum = this->m_dwMemberNum;
	sInfo.m_dwMoney = this->m_dwMoney;
	sInfo.m_dwTotalEff = this->m_dwTotalEff;
	sInfo.m_bRecruit = this->m_bRecruit;
	sInfo.m_nTongParam = this->m_nTongParam;
	sInfo.m_nTongJiyuParam = this->m_nTongJiyuParam;
	strcpy(sInfo.m_szTongJiyuNotify, this->m_szTongJiyuNotify);
	strcpy(sInfo.m_szMasterName, this->m_szMasterName);
	strcpy(sInfo.m_szAgname, this->m_szAgname);
	strcpy(sInfo.m_szTongName, this->m_szName);
	if (g_pServer)
		g_pServer->PackDataToClient(Player[this->m_nPlayerIndex].m_nNetConnectIdx, &sInfo, sInfo.m_wLength + 1);
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫…Ë∂®◊‘…Ì∞Ôª·–≈œ¢
//-------------------------------------------------------------------------
void	KPlayerTong::SetSelfInfo(TONG_SELF_INFO_SYNC *pInfo)
{
	if (pInfo->m_btJoinFlag == 0)
	{
		if (m_nFlag)
		{
			KUiGameObjectWithName	sUi;
			strcpy(sUi.szName, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
			sUi.nData = TONG_ACTION_LEAVE;
			sUi.nParam = m_nFigure;
			sUi.uParam = 0;
			sUi.szString[0] = 0;
			CoreDataChanged(GDCNI_TONG_ACTION_RESULT, (unsigned int)&sUi, 1);

			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetCamp(camp_free);
			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetCurrentCamp(camp_free);
		}
		Clear();

		CoreDataChanged(GDCNI_PLAYER_BASE_INFO, 0, 0);
		return;
	}


	this->m_nFlag = 1;
	this->m_nFigure = pInfo->m_btFigure;
	this->m_nCamp = pInfo->m_btCamp;
	this->m_btLevel = pInfo->m_btLevel;
	this->m_dwTotalEff = pInfo->m_dwTotalEff;
	this->m_bRecruit = pInfo->m_bRecruit;
	this->m_nTongParam = pInfo->m_nTongParam;
	this->m_dwMoney = pInfo->m_dwMoney;
	this->m_btDirectorNum = pInfo->m_btDirectorNum;
	this->m_btManagerNum = pInfo->m_btManagerNum;
	this->m_dwMemberNum = pInfo->m_dwMemberNum;
	this->m_nTongJiyuParam = pInfo->m_nTongJiyuParam;
	strcpy(this->m_szTongJiyuNotify, pInfo->m_szTongJiyuNotify);
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetCamp(m_nCamp);
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetCurrentCamp(m_nCamp);
	memcpy(this->m_szMasterName, pInfo->m_szMasterName, sizeof(pInfo->m_szMasterName));
	memcpy(this->m_szName, pInfo->m_szTongName, sizeof(pInfo->m_szTongName));
	if (m_nFlag == 1 && m_nFigure != pInfo->m_btFigure)
		memset(this->m_szAgname, 0, sizeof(pInfo->m_szAgname));
	else
		memcpy(this->m_szAgname, pInfo->m_szAgname, sizeof(pInfo->m_szAgname));
	m_dwTongNameID	= g_FileName2Id(m_szName);

	CoreDataChanged(GDCNI_PLAYER_BASE_INFO, 0, 0);
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫ºÏ≤‚ «∑Ò”–»Œ√¸»®¿˚
//-------------------------------------------------------------------------
BOOL	KPlayerTong::CheckInstatePower(TONG_APPLY_INSTATE_COMMAND *pApply)
{
	if (!pApply)
		return FALSE;
	if (pApply->m_wLength <= sizeof(TONG_APPLY_INSTATE_COMMAND) - 1 - sizeof(pApply->m_szName))
		return FALSE;
	if (!m_nFlag || (m_nFigure != enumTONG_FIGURE_MASTER && m_nFigure != enumTONG_FIGURE_DIRECTOR))
		return FALSE;
	if (pApply->m_dwTongNameID != g_FileName2Id(m_szName))
		return FALSE;
	if (pApply->m_btCurFigure == pApply->m_btNewFigure && pApply->m_btCurPos == pApply->m_btNewPos)
		return FALSE;

	char	szName[32];
	memcpy(szName, pApply->m_szName, sizeof(pApply->m_szName));
	szName[31] = 0;
	if (strlen(szName) >= 31)
		return FALSE;

	if (m_nFigure == enumTONG_FIGURE_MASTER)
	{
		if (pApply->m_btCurFigure == enumTONG_FIGURE_DIRECTOR)
		{
			if (pApply->m_btCurPos >= defTONG_MAX_DIRECTOR)
				return FALSE;
		}
		else if (pApply->m_btCurFigure == enumTONG_FIGURE_MANAGER)
		{
			if (pApply->m_btCurPos >= defTONG_MAX_MANAGER)
				return FALSE;
		}
		else if (pApply->m_btCurFigure == enumTONG_FIGURE_MEMBER)
		{
		}
		else
		{
			return FALSE;
		}

		if (pApply->m_btNewFigure == enumTONG_FIGURE_DIRECTOR)
		{
			if (pApply->m_btNewPos >= defTONG_MAX_DIRECTOR)
				return FALSE;
		}
		else if (pApply->m_btNewFigure == enumTONG_FIGURE_MANAGER)
		{
			if (pApply->m_btNewPos >= defTONG_MAX_MANAGER)
				return FALSE;
		}
		else if (pApply->m_btNewFigure == enumTONG_FIGURE_MEMBER)
		{
		}
		else
		{
			return FALSE;
		}
	}
	else // if (m_nFigure == enumTONG_FIGURE_DIRECTOR)
	{
		if (pApply->m_btCurFigure == enumTONG_FIGURE_MANAGER)
		{
			if (pApply->m_btCurPos >= defTONG_MAX_MANAGER)
				return FALSE;
		}
		else if (pApply->m_btCurFigure == enumTONG_FIGURE_MEMBER)
		{
		}
		else
		{
			return FALSE;
		}

		if (pApply->m_btNewFigure == enumTONG_FIGURE_MANAGER)
		{
			if (pApply->m_btNewPos >= defTONG_MAX_MANAGER)
				return FALSE;
		}
		else if (pApply->m_btNewFigure == enumTONG_FIGURE_MEMBER)
		{
		}
		else
		{
			return FALSE;
		}
	}

	return TRUE;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫ºÏ≤‚ «∑Ò”–Ãﬂ»À»®¿˚
//-------------------------------------------------------------------------
BOOL	KPlayerTong::CheckKickPower(TONG_APPLY_KICK_COMMAND *pKick)
{
	if (!pKick)
		return FALSE;
	if (pKick->m_wLength + 1 != sizeof(TONG_APPLY_KICK_COMMAND))
		return FALSE;
	if (!m_nFlag || (m_nFigure != enumTONG_FIGURE_MASTER && m_nFigure != enumTONG_FIGURE_DIRECTOR))
		return FALSE;
	if (pKick->m_dwTongNameID != g_FileName2Id(m_szName))
		return FALSE;

	if (pKick->m_btFigure == enumTONG_FIGURE_MANAGER)
	{
		if (pKick->m_btPos >= defTONG_MAX_MANAGER)
			return FALSE;
	}
	else if (pKick->m_btFigure == enumTONG_FIGURE_MEMBER)
	{
	}
	else
		return FALSE;

	return TRUE;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫±ª»Œ√¸
//-------------------------------------------------------------------------
void	KPlayerTong::BeInstated(STONG_SERVER_TO_CORE_BE_INSTATED *pSync)
{
	this->m_nFlag = 1;
	switch(this->m_nFigure)
	{
	case enumTONG_FIGURE_MEMBER:
		m_dwMemberNum--;
		pSync->m_btFigure==enumTONG_FIGURE_DIRECTOR?m_btDirectorNum++:m_btManagerNum++;
		break;
	case enumTONG_FIGURE_MANAGER:
		m_btManagerNum--;
		pSync->m_btFigure==enumTONG_FIGURE_DIRECTOR?m_btDirectorNum++:m_dwMemberNum++;
		break;
	case enumTONG_FIGURE_DIRECTOR:
		m_btDirectorNum--;
		pSync->m_btFigure==enumTONG_FIGURE_MANAGER?m_btManagerNum++:m_dwMemberNum++;
		break;
	}
	this->m_nFigure = pSync->m_btFigure;
	strcpy(m_szAgname, pSync->m_szAgname);
	SendSelfInfo();
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫±ªÃﬂ≥ˆ∞Ôª·
//-------------------------------------------------------------------------
void	KPlayerTong::BeKicked(STONG_SERVER_TO_CORE_BE_KICKED *pSync)
{
	Clear();

	Npc[Player[m_nPlayerIndex].m_nIndex].SetCamp(camp_free);
	Npc[Player[m_nPlayerIndex].m_nIndex].SetCurrentCamp(camp_free);
	SendSelfInfo();

	SHOW_MSG_SYNC	sMsg;
	// ±ªÃﬂ≥ˆ∞Ôª·
	sMsg.ProtocolType = s2c_msgshow;
	sMsg.m_wMsgID = enumMSG_ID_TONG_BE_KICK;
	sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
	g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫ºÏ≤‚ «∑Ò”–¿Îø™»®¿˚
//-------------------------------------------------------------------------
BOOL	KPlayerTong::CheckLeavePower(TONG_APPLY_LEAVE_COMMAND *pLeave)
{
	if (!pLeave)
		return FALSE;
	if (pLeave->m_wLength + 1 != sizeof(TONG_APPLY_LEAVE_COMMAND))
		return FALSE;
	if (!m_nFlag || m_nFigure == enumTONG_FIGURE_MASTER || m_nFigure == enumTONG_FIGURE_DIRECTOR)
		return FALSE;
	if (pLeave->m_dwTongNameID != g_FileName2Id(m_szName))
		return FALSE;
	return TRUE;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫¿Îø™∞Ôª·
//-------------------------------------------------------------------------
void	KPlayerTong::Leave(STONG_SERVER_TO_CORE_LEAVE *pLeave)
{
	if (strcmp(pLeave->m_szName, Npc[Player[m_nPlayerIndex].m_nIndex].Name) != 0)
		return;

	if (pLeave->m_bSuccessFlag)
	{
		Clear();
		Npc[Player[m_nPlayerIndex].m_nIndex].SetCamp(camp_free);
		Npc[Player[m_nPlayerIndex].m_nIndex].SetCurrentCamp(camp_free);
		Player[m_nPlayerIndex].m_ItemList.DecMoney(TongData.m_sTongParam.m_nLeaveMoney);
		int nRepute=Player[m_nPlayerIndex].m_cTask.GetSaveVal(TASKVALUE_STATTASK_REPUTE);
		Player[m_nPlayerIndex].m_cTask.SetSaveVal(TASKVALUE_STATTASK_REPUTE, nRepute-TongData.m_sTongParam.m_nLeaveRepute);
		Player[m_nPlayerIndex].m_cTong.m_dwLeaveTime = TongData.m_sTongParam.m_nLeaveTime + KSG_GetCurSec();
		SendSelfInfo();

		SHOW_MSG_SYNC	sMsg;
		// ¿Îø™∞Ôª·≥…π¶
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_TONG_LEAVE_SUCCESS;
		sMsg.m_lpBuf = (void *)Player[m_nPlayerIndex].m_cTong.m_dwLeaveTime;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
		g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
	}
	else
	{
		SendSelfInfo();

		SHOW_MSG_SYNC	sMsg;
		// ¿Îø™∞Ôª· ß∞‹
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_TONG_LEAVE_FAIL;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
		g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
	}
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫ºÏ≤‚ «∑Ò”–»®¿˚ªª∞Ô÷˜
//-------------------------------------------------------------------------
BOOL	KPlayerTong::CheckChangeMasterPower(TONG_APPLY_CHANGE_MASTER_COMMAND *pChange)
{
	if (!pChange)
		return FALSE;
	if (pChange->m_wLength + 1 != sizeof(TONG_APPLY_CHANGE_MASTER_COMMAND))
		return FALSE;
	if (!m_nFlag || m_nFigure != enumTONG_FIGURE_MASTER)
		return FALSE;
	if (pChange->m_dwTongNameID != g_FileName2Id(m_szName))
		return FALSE;
	char	szName[32];
	memcpy(szName, pChange->m_szName, sizeof(pChange->m_szName));
	szName[31] = 0;
	if (strlen(szName) >= 31)
		return FALSE;

	return TRUE;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫ºÏ≤‚ «∑Ò”–ƒ‹¡¶Ω” ‹¥´Œª
//-------------------------------------------------------------------------
BOOL	KPlayerTong::CheckGetMasterPower()
{
	if (!m_nFlag || m_nFigure == enumTONG_FIGURE_MASTER)
		return FALSE;
//	if (m_nFigure != pCheck->m_btFigure)
//		return FALSE;
	if (strcmp(m_szMasterName, Npc[Player[m_nPlayerIndex].m_nIndex].Name) == 0)
		return FALSE;
	// µ»º∂Œ Ã‚
	if (Npc[Player[m_nPlayerIndex].m_nIndex].m_Level < TongData.m_sTongParam.m_nLevel ||
		(int)Player[m_nPlayerIndex].m_nLeadLevel < TongData.m_sTongParam.m_nLeadLevel)
		return FALSE;

	return TRUE;
}

BOOL	KPlayerTong::CheckGetMasterPower(STONG_SERVER_TO_CORE_CHECK_GET_MASTER_POWER *pCheck)
{
	if (!pCheck)
		return FALSE;
	if (!m_nFlag || m_nFigure == enumTONG_FIGURE_MASTER)
		return FALSE;
	if (pCheck->m_dwTongNameID != g_FileName2Id(m_szName))
		return FALSE;
//	if (m_nFigure != pCheck->m_btFigure)
//		return FALSE;
	if (strcmp(pCheck->m_szName, Npc[Player[m_nPlayerIndex].m_nIndex].Name) != 0)
		return FALSE;
	// µ»º∂Œ Ã‚
	if (Npc[Player[m_nPlayerIndex].m_nIndex].m_Level < TongData.m_sTongParam.m_nLevel ||
		(int)Player[m_nPlayerIndex].m_nLeadLevel < TongData.m_sTongParam.m_nLeadLevel)
		return FALSE;

	return TRUE;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫¥´Œªµº÷¬…Ì∑›∏ƒ±‰
//-------------------------------------------------------------------------
void	KPlayerTong::ChangeAs(STONG_SERVER_TO_CORE_CHANGE_AS *pAs)
{
	if (!pAs)
		return;
	if (!m_nFlag)
		return;
	if (pAs->m_dwTongNameID != g_FileName2Id(m_szName))
		return;

	if(pAs->m_btFigure==enumTONG_FIGURE_MASTER)
	{
		switch(this->m_nFigure)
		{
		case enumTONG_FIGURE_MEMBER:
			break;
		case enumTONG_FIGURE_MANAGER:
			m_btManagerNum--;
			break;
		case enumTONG_FIGURE_DIRECTOR:
			m_btDirectorNum--;
			break;
		}
	}
	m_nFigure = pAs->m_btFigure;
	strcpy(m_szAgname, pAs->m_szAgname);
	strcpy(this->m_szMasterName, pAs->m_szName);

	this->SendSelfInfo();

	SHOW_MSG_SYNC	sMsg;

	sMsg.ProtocolType = s2c_msgshow;
	if (m_nFigure == enumTONG_FIGURE_MASTER)
		sMsg.m_wMsgID = enumMSG_ID_TONG_CHANGE_AS_MASTER;
	else
		sMsg.m_wMsgID = enumMSG_ID_TONG_CHANGE_AS_MEMBER;
	sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
	g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);

	return;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫ªª∞Ô÷˜
//-------------------------------------------------------------------------
void	KPlayerTong::ChangeMaster(char *lpszMaster)
{
	if (!lpszMaster || !lpszMaster[0])
		return;
	if (strlen(lpszMaster) >= sizeof(this->m_szMasterName))
		return;
	strcpy(m_szMasterName, lpszMaster);

	this->SendSelfInfo();
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫µ«¬Ω ±∫ÚªÒµ√∞Ôª·–≈œ¢
//-------------------------------------------------------------------------
void	KPlayerTong::Login(STONG_SERVER_TO_CORE_LOGIN *pLogin)
{
	if (strcmp(Npc[Player[this->m_nPlayerIndex].m_nIndex].Name, pLogin->m_szName) != 0)
		return;

	if (pLogin->m_nFlag == 0)
	{
		Clear();
		Npc[Player[m_nPlayerIndex].m_nIndex].SetCamp(camp_free);
		Npc[Player[m_nPlayerIndex].m_nIndex].SetCurrentCamp(camp_free);
		this->SendSelfInfo();

		return;
	}
	m_nFlag			= 1;
	m_dwMoney		= pLogin->m_nMoney;
	m_nFigure		= pLogin->m_nFigure;
	m_nCamp			= pLogin->m_nCamp;
	m_dwMemberNum	= pLogin->m_dwMemberNum;
	m_btManagerNum	= pLogin->m_btManagerNum;
	m_btDirectorNum	= pLogin->m_btDirectorNum;
	m_btLevel		= pLogin->m_btLevel;
	m_dwTotalEff	= pLogin->m_dwTotalEff;
	m_nJoinTm		= pLogin->m_nJoinTm;
	m_nSaveEff	= pLogin->m_nSaveEff;
	m_bRecruit		= pLogin->m_bRecruit;
	m_nTongParam		= pLogin->m_nTongParam;
	m_dwTongNameID	= g_FileName2Id(pLogin->m_szTongName);
	m_nApplyTo		= 0;
	strcpy(m_szName, pLogin->m_szTongName);
	strcpy(m_szAgname, pLogin->m_szAgname);
	strcpy(m_szMasterName, pLogin->m_szMaster);
	m_nTongJiyuParam = pLogin->m_nTongJiyuParam;
	strcpy(m_szTongJiyuNotify, pLogin->m_szTongJiyuNotify);
	m_nTongNationalEmblem = KSG_StringGetValue(1,m_nTongJiyuParam,5);
	Npc[Player[m_nPlayerIndex].m_nIndex].SetCamp(m_nCamp);
	Npc[Player[m_nPlayerIndex].m_nIndex].SetCurrentCamp(m_nCamp);
	this->SendSelfInfo();
}
#endif

#ifndef _SERVER
void    KPlayerTong::OpenCreateInterface()
{
	CoreDataChanged(GDCNI_OPEN_TONG_CREATE_SHEET, 0, 0);
}

BOOL KPlayerTong::ApplySaveMoney(DWORD nMoney)
{
	if (!m_nFlag)
		return FALSE;
	if (nMoney <= 0)
		return FALSE;
	if (Player[CLIENT_PLAYER_INDEX].m_ItemList.GetMoney(room_equipment) < nMoney)
		return FALSE;

	TONG_APPLY_SAVE_COMMAND	sSave;
	sSave.ProtocolType		= c2s_extendtong;
	sSave.m_wLength			= sizeof(sSave) - 1;
	sSave.m_btMsgId			= enumTONG_COMMAND_ID_APPLY_SAVE;
	sSave.m_dwTongNameID	= g_FileName2Id(this->m_szName);
	sSave.m_dwMoney			= nMoney;
	strcpy(sSave.m_szName, Npc[Player[m_nPlayerIndex].m_nIndex].Name);
	if (g_pClient)
		g_pClient->SendPackToServer(&sSave, sSave.m_wLength + 1);

	return TRUE;
}

BOOL KPlayerTong::ApplyGetMoney(DWORD nMoney)
{
	if (!m_nFlag)
		return FALSE;
	if (nMoney <= 0)
		return FALSE;
	if (m_nFigure != enumTONG_FIGURE_MASTER && m_nFigure != enumTONG_FIGURE_DIRECTOR)
		return FALSE;
	if (this->m_dwMoney < nMoney)
		return FALSE;

	TONG_APPLY_SAVE_COMMAND	sSave;
	sSave.ProtocolType		= c2s_extendtong;
	sSave.m_wLength			= sizeof(sSave) - 1;
	sSave.m_btMsgId			= enumTONG_COMMAND_ID_APPLY_GET;
	sSave.m_dwTongNameID	= g_FileName2Id(this->m_szName);
	sSave.m_dwMoney			= nMoney;
	strcpy(sSave.m_szName, Npc[Player[m_nPlayerIndex].m_nIndex].Name);
	if (g_pClient)
		g_pClient->SendPackToServer(&sSave, sSave.m_wLength + 1);

	return TRUE;
}

BOOL KPlayerTong::ApplyChangeJiyu(char* szMasterName, int nParam, char* szNotify)
{
	if (!m_nFlag)
		return FALSE;
	if (m_nFigure != enumTONG_FIGURE_MASTER)
		return FALSE;
	if (m_dwMoney < TongData.m_sTongParam.m_nMoneySaveJiyu)
	{
		char	szBuf[80];
		sprintf(szBuf, MSG_TONG_CHANGE_SAVEJIYU_ERROR, TongData.m_sTongParam.m_nMoneySaveJiyu);
		defFuncShowNormalMsg(szBuf);
		return FALSE;
	}

	TONG_APPLY_CHANGE_INFO_COMMAND	sSave;
	sSave.ProtocolType		= c2s_extendtong;
	sSave.m_wLength			= sizeof(sSave) - 1;
	sSave.m_btMsgId			= enumTONG_COMMAND_ID_APPLY_CHANGE_JIYU;
	sSave.m_dwTongNameID	= g_FileName2Id(this->m_szName);
	strcpy(sSave.m_szName, szMasterName);
	sSave.m_nMoney = TongData.m_sTongParam.m_nMoneySaveJiyu;
	sSave.m_nTongJiyuParam = nParam;
	strcpy(sSave.m_szTongJiyuNotify, szNotify);
	if (g_pClient)
		g_pClient->SendPackToServer(&sSave, sSave.m_wLength + 1);

	return TRUE;
}

BOOL KPlayerTong::ApplyRecruit(BOOL bRecruit)
{
	if (!m_nFlag)
		return FALSE;

	if (m_nFigure != enumTONG_FIGURE_MASTER)
		return FALSE;
		
	TONG_CHANGE_RECRUIT_COMMAND	sSave;
	sSave.ProtocolType		= c2s_extendtong;
	sSave.m_wLength			= sizeof(sSave) - 1;
	sSave.m_btMsgId			= enumTONG_COMMAND_ID_APPLY_CHANGE_RECRUIT;
	sSave.m_dwTongNameID	= g_FileName2Id(this->m_szName);
	sSave.m_bRecruit 		= bRecruit;
	if (g_pClient)
		g_pClient->SendPackToServer(&sSave, sSave.m_wLength + 1);

	return TRUE;
}
#endif

#ifdef _SERVER

BOOL KPlayerTong::CheckMoneyPower( TONG_APPLY_SAVE_COMMAND *pSave )
{
	if (!pSave)
		return FALSE;
	if (pSave->m_wLength + 1 != sizeof(TONG_APPLY_SAVE_COMMAND))
		return FALSE;
	if ((!m_nFlag || m_nFigure == enumTONG_FIGURE_MANAGER || m_nFigure == enumTONG_FIGURE_MEMBER) && pSave->m_btMsgId != enumTONG_COMMAND_ID_APPLY_SAVE)
		return FALSE;
	if (!m_nFlag && pSave->m_btMsgId == enumTONG_COMMAND_ID_APPLY_SAVE)
		return FALSE;
	if (pSave->m_dwTongNameID != g_FileName2Id(m_szName))
		return FALSE;

	return TRUE;
}

BOOL KPlayerTong::CheckSaveEffPower( TONG_APPLY_SAVE_EFF_COMMAND *pSave )
{
	if (!pSave)
		return FALSE;
	if (pSave->m_wLength + 1 != sizeof(TONG_APPLY_SAVE_EFF_COMMAND))
		return FALSE;
	if (!m_nFlag)
		return FALSE;
	if (pSave->m_dwTongNameID != g_FileName2Id(m_szName))
		return FALSE;

	return TRUE;
}

void KPlayerTong::ChangeMoney(DWORD dwMoney)
{
	m_dwMoney = dwMoney;

	this->SendSelfInfo();
}

void KPlayerTong::ChangeTongEff(DWORD dwTotalEff)
{
	m_dwTotalEff = dwTotalEff;

	this->SendSelfInfo();
}
#endif

#ifndef _SERVER
BOOL	KPlayerTong::ApplyChangeAgname(int nCurFigure, int nCurPos, char *lpszName, char *lpszTongAgname)
{
	if (!lpszName || !lpszName[0] || strlen(lpszName) >= 32)
		return FALSE;
	if (!lpszTongAgname || !lpszTongAgname[0] || strlen(lpszTongAgname) >= 32)
		return FALSE;
	if (!m_nFlag)
		return FALSE;
	if (nCurFigure == enumTONG_FIGURE_MEMBER)
	{
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, "Bπn kh´ng th” thay ÆÊi danh hi÷u cho ß÷ Tˆ bÊn bang");
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		return FALSE;
	}
	if (m_nFigure != enumTONG_FIGURE_MASTER && m_nFigure != enumTONG_FIGURE_DIRECTOR)
		return FALSE;
	if (nCurFigure == enumTONG_FIGURE_MANAGER && (nCurPos < 0 || nCurPos >= defTONG_MAX_MANAGER))
		return FALSE;
	if (m_nFigure == enumTONG_FIGURE_DIRECTOR)
	{
		if (nCurFigure != enumTONG_FIGURE_MANAGER && nCurFigure != enumTONG_FIGURE_MEMBER)
			return FALSE;
	}
	
	TONG_APPLY_CHANGE_AGNAME_COMMAND	sChange;
	sChange.ProtocolType	= c2s_extendtong;
	sChange.m_wLength		= sizeof(sChange) - 1;
	sChange.m_btMsgId		= enumTONG_COMMAND_ID_APPLY_CHANGE_AGNAME;
	sChange.m_dwTongNameID	= g_FileName2Id(this->m_szName);
	sChange.m_btFigure		= nCurFigure;
	sChange.m_btPos			= nCurPos;
	strcpy(sChange.m_szName, lpszName);
	strcpy(sChange.m_szAgname, lpszTongAgname);
	if (g_pClient)
		g_pClient->SendPackToServer(&sChange, sChange.m_wLength + 1);
	
	return TRUE;
}
#endif

#ifdef _SERVER
BOOL	KPlayerTong::CheckChangeAgnamePower(TONG_APPLY_CHANGE_AGNAME_COMMAND *pChange)
{
	if (!pChange)
		return FALSE;
	if (pChange->m_wLength + 1 != sizeof(TONG_APPLY_CHANGE_AGNAME_COMMAND))
		return FALSE;
	if (!m_nFlag)
		return FALSE;
	if (m_nFigure != enumTONG_FIGURE_MASTER && m_nFigure != enumTONG_FIGURE_DIRECTOR)
		return FALSE;
	if (pChange->m_btFigure == enumTONG_FIGURE_MEMBER)
		return FALSE;
	if (pChange->m_btFigure == enumTONG_FIGURE_MANAGER && (pChange->m_btPos < 0 || pChange->m_btPos >= defTONG_MAX_MANAGER))
		return FALSE;
	if (m_nFigure == enumTONG_FIGURE_DIRECTOR)
	{
		if (pChange->m_btFigure != enumTONG_FIGURE_MANAGER && pChange->m_btFigure != enumTONG_FIGURE_MEMBER)
			return FALSE;
	}
	if (pChange->m_dwTongNameID != g_FileName2Id(m_szName))
		return FALSE;
	char	szName[32];
	memcpy(szName, pChange->m_szName, sizeof(pChange->m_szName));
	szName[31] = 0;

	char	szTongAgname[32];
	memcpy(szTongAgname, pChange->m_szAgname, sizeof(pChange->m_szAgname));
	szTongAgname[31] = 0;

	return TRUE;
}
#endif

#ifdef _SERVER
BOOL	KPlayerTong::CheckGetAgnamePower(STONG_SERVER_TO_CORE_CHECK_GET_AGNAME_POWER *pCheck)
{
	if (!pCheck)
		return FALSE;
	if (!m_nFlag)
		return FALSE;
	if (m_nFigure == enumTONG_FIGURE_MEMBER)
		return FALSE;
	if (pCheck->m_dwTongNameID != g_FileName2Id(m_szName))
		return FALSE;
	if (strcmp(pCheck->m_szName, Npc[Player[m_nPlayerIndex].m_nIndex].Name) != 0)
		return FALSE;
	if (strcmp(pCheck->m_szAgname, m_szAgname) == 0)
		return FALSE;

	return TRUE;
}
#endif

#ifdef _SERVER
void	KPlayerTong::BeChangedAgname(STONG_SERVER_TO_CORE_BE_CHANGED_AGNAME *pSync)
{
	strcpy(m_szAgname, pSync->m_szAgname);
	
	SendSelfInfo();
}
#endif

#ifndef _SERVER
BOOL	KPlayerTong::ApplyChangeSexAgname(char *lpszTongAgname, int nSex)
{
	if (!lpszTongAgname || !lpszTongAgname[0] || strlen(lpszTongAgname) >= 32)
		return FALSE;
	if (!m_nFlag)
		return FALSE;
	if (m_nFigure != enumTONG_FIGURE_MASTER && m_nFigure != enumTONG_FIGURE_DIRECTOR)
		return FALSE;
	
	TONG_APPLY_CHANGE_SEX_AGNAME_COMMAND	sChange;
	sChange.ProtocolType	= c2s_extendtong;
	sChange.m_wLength		= sizeof(sChange) - 1;
	sChange.m_btMsgId		= enumTONG_COMMAND_ID_APPLY_CHANGE_SEX_AGNAME;
	sChange.m_dwTongNameID	= g_FileName2Id(this->m_szName);
	sChange.m_btSex			= nSex;
	strcpy(sChange.m_szAgname, lpszTongAgname);
	if (g_pClient)
		g_pClient->SendPackToServer(&sChange, sChange.m_wLength + 1);
	
	return TRUE;
}
#endif

#ifdef _SERVER
BOOL	KPlayerTong::CheckChangeSexAgnamePower(TONG_APPLY_CHANGE_SEX_AGNAME_COMMAND *pChange)
{
	if (!pChange)
		return FALSE;
	if (pChange->m_wLength + 1 != sizeof(TONG_APPLY_CHANGE_SEX_AGNAME_COMMAND))
		return FALSE;
	if (!m_nFlag)
		return FALSE;
	if (m_nFigure != enumTONG_FIGURE_MASTER && m_nFigure != enumTONG_FIGURE_DIRECTOR)
		return FALSE;
	if (pChange->m_dwTongNameID != g_FileName2Id(m_szName))
		return FALSE;

	char	szTongAgname[32];
	memcpy(szTongAgname, pChange->m_szAgname, sizeof(pChange->m_szAgname));
	szTongAgname[31] = 0;
	if (strlen(szTongAgname) >= 31)
		return FALSE;
	
	return TRUE;
}

BOOL	KPlayerTong::CheckChangeInfo(TONG_APPLY_CHANGE_INFO_COMMAND *pChange)
{
	if (!pChange)
		return FALSE;
	if (pChange->m_wLength + 1 != sizeof(TONG_APPLY_CHANGE_INFO_COMMAND))
		return FALSE;
	if (!m_nFlag)
		return FALSE;
	if (m_nFigure != enumTONG_FIGURE_MASTER)
		return FALSE;
	if (pChange->m_dwTongNameID != g_FileName2Id(m_szName))
		return FALSE;

	return TRUE;
}

BOOL	KPlayerTong::CheckChangeRecruit(TONG_CHANGE_RECRUIT_COMMAND *pChange)
{
	if (!pChange)
		return FALSE;
	if (pChange->m_wLength + 1 != sizeof(TONG_CHANGE_RECRUIT_COMMAND))
		return FALSE;
	if (!m_nFlag)
		return FALSE;
	if (m_nFigure != enumTONG_FIGURE_MASTER)
		return FALSE;
	if (pChange->m_dwTongNameID != g_FileName2Id(m_szName))
		return FALSE;
	
	return TRUE;
}
#endif

#ifndef _SERVER
BOOL	KPlayerTong::ApplyTongChangeCamp(int nCamp)
{
	if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
		return FALSE;

	if (!m_nFlag)
		return FALSE;

	if (m_nFigure != enumTONG_FIGURE_MASTER)
	{
		defFuncShowNormalMsg(MSG_TONG_CHANGE_CAMP_ERROR1);
		return FALSE;
	}

	if (nCamp != camp_justice && nCamp != camp_evil && nCamp != camp_balance)
	{
		defFuncShowNormalMsg(MSG_TONG_CHANGE_CAMP_ERROR2);
		return FALSE;
	}
	
	if (nCamp == m_nCamp)
		return FALSE;

	if (m_dwMoney < TongData.m_sTongParam.m_nMoneyChangeCamp)
	{
		char	szBuf[80];
		sprintf(szBuf, MSG_TONG_CHANGE_CAMP_ERROR3, TongData.m_sTongParam.m_nMoneyChangeCamp);
		defFuncShowNormalMsg(szBuf);
		return FALSE;
	}
	defFuncShowNormalMsg(MSG_TONG_CHANGE_CAMP);

	TONG_APPLY_CHANGE_CAMP_COMMAND	sChange;
	sChange.ProtocolType = c2s_extendtong;
	sChange.m_wLength = sizeof(sChange) - 1;
	sChange.m_btMsgId = enumTONG_COMMAND_ID_APPLY_CHANGE_CAMP;
	sChange.m_dwTongNameID	= g_FileName2Id(this->m_szName);
	sChange.m_btCamp = (BYTE)nCamp;
	sChange.m_nMoney = TongData.m_sTongParam.m_nMoneyChangeCamp;
	
	if (g_pClient)
		g_pClient->SendPackToServer(&sChange, sChange.m_wLength + 1);
	
	return TRUE;
}
#endif

#ifndef _SERVER
BOOL	KPlayerTong::ChangeCamp(int nCamp)
{
	if (nCamp != camp_justice && nCamp != camp_evil && nCamp != camp_balance)
		return FALSE;
	
	if (nCamp == m_nCamp)
		return FALSE;
	
	m_nCamp		= nCamp;
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetCamp(m_nCamp);
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetCurrentCamp(m_nCamp);
	
	return TRUE;
}

BOOL	KPlayerTong::ChangeLevel(BYTE btLevel)
{	
	m_btLevel	= btLevel;	
	return TRUE;
}

BOOL	KPlayerTong::ChangeRecruit(BOOL bRecruit)
{	
	m_bRecruit	= bRecruit;	
	return TRUE;
}

BOOL	KPlayerTong::ChangeTongParam(int nTongParam)
{	
	m_nTongParam	= nTongParam;	
	return TRUE;
}

BOOL	KPlayerTong::ChangeJiyu(int nParam, char* szNotify)
{	
	m_nTongJiyuParam = nParam;
	strcpy(m_szTongJiyuNotify, szNotify);
	return TRUE;
}

BOOL	KPlayerTong::ChangeEff(int nSaveEff, DWORD dwTotalEff)
{	
	m_nSaveEff = nSaveEff;
	m_dwTotalEff = dwTotalEff;
	return TRUE;
}
#endif

#ifdef _SERVER
int		KPlayerTong::CheckChangeCampCondition(TONG_APPLY_CHANGE_CAMP_COMMAND *pChange)
{
	if (!pChange)
		return enumTONG_CHANGE_CAMP_ERROR_ID1;
	
	if (pChange->m_wLength + 1 != sizeof(TONG_APPLY_CHANGE_CAMP_COMMAND))
		return enumTONG_CHANGE_CAMP_ERROR_ID2;

	if (Player[m_nPlayerIndex].CheckTrading())
		return enumTONG_CHANGE_CAMP_ERROR_ID3;

	if (!m_nFlag)
		return enumTONG_CHANGE_CAMP_ERROR_ID4;
	
	if (m_nFigure != enumTONG_FIGURE_MASTER)
		return enumTONG_CHANGE_CAMP_ERROR_ID5;

	if (pChange->m_dwTongNameID != g_FileName2Id(m_szName))
		return FALSE;

	if (pChange->m_btCamp != camp_justice && pChange->m_btCamp != camp_evil && pChange->m_btCamp != camp_balance)
		return enumTONG_CHANGE_CAMP_ERROR_ID6;

	if (m_nCamp == pChange->m_btCamp)
		return enumTONG_CHANGE_CAMP_ERROR_ID7;

	if (m_dwMoney < TongData.m_sTongParam.m_nMoneyChangeCamp)
		return enumTONG_CHANGE_CAMP_ERROR_ID8;

	return 0;
}
#endif

#ifdef _SERVER
void	KPlayerTong::BeChangedCamp(STONG_SERVER_TO_CORE_BE_CHANGED_CAMP *pSync)
{
	int nSubWorldIndex=Npc[Player[m_nPlayerIndex].m_nIndex].m_SubWorldIndex;
	int nSubWorldID = SubWorld[nSubWorldIndex].m_SubWorldID;
	
	if (pSync->m_nCamp != camp_justice && pSync->m_nCamp != camp_evil && pSync->m_nCamp != camp_balance)
		return;

	if (pSync->m_nCamp == m_nCamp)
		return;
	
	m_dwMoney	= pSync->m_nMoney;
	m_nCamp		= pSync->m_nCamp;
	this->SendSelfInfo();

	Npc[Player[m_nPlayerIndex].m_nIndex].SetCamp(m_nCamp);
	Npc[Player[m_nPlayerIndex].m_nIndex].SetCurrentCamp(m_nCamp);
}

void	KPlayerTong::BeChangedLevel(STONG_SERVER_TO_CORE_BE_CHANGED_LEVEL *pSync)
{	
	m_btLevel		= pSync->m_btLevel;

	this->SendSelfInfo();
}

void	KPlayerTong::BeChangedRecruit(STONG_SERVER_TO_CORE_BE_CHANGED_RECRUIT *pSync)
{	
	m_bRecruit		= pSync->m_bRecruit;

	this->SendSelfInfo();
}

void	KPlayerTong::BeChangedTongParam(STONG_SERVER_TO_CORE_BE_CHANGED_TONGPARAM *pSync)
{	
	m_nTongParam		= pSync->m_nTongParam;
}

void	KPlayerTong::BeChangedJiyu(STONG_SERVER_TO_CORE_BE_CHANGED_JIYU *pSync)
{	
	m_dwMoney = pSync->m_nMoney;
	m_nTongJiyuParam = pSync->m_nTongJiyuParam;
	strcpy(m_szTongJiyuNotify, pSync->m_szTongJiyuNotify);
	m_nTongNationalEmblem = KSG_StringGetValue(1,m_nTongJiyuParam,5);

	this->SendSelfInfo();
}

void	KPlayerTong::BeChangedEff(STONG_SERVER_TO_CORE_BE_CHANGED_EFF *pSync)
{	
	m_nSaveEff = pSync->m_nTotalEff;
	m_dwTotalEff = pSync->m_dwTotalEff;

	this->SendSelfInfo();
}
#endif