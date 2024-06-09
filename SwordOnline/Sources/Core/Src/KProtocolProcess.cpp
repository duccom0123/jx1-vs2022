#include "KEngine.h"
#include "KCore.h"
#ifndef _SERVER
#include "../../Headers/IClient.h"
#include "CoreShell.h"
#include "KViewItem.h"
#include "KSellItem.h"
#endif
#include "KObjSet.h"
#include "KNpcSet.h"
#include "KPlayer.h"
#include "KPlayerSet.h"
#include "KPlayerTeam.h"
#include "KPlayerChatRoom.h"
#include "KPlayerFaction.h"
#include "KTongData.h"
#include "KNpc.h"
#include "KNpcRes.h"
#include "KSubWorld.h"
#include "KTaskFuns.h"
#include "LuaFuns.h"
#include "KProtocolProcess.h"
#include "KSkills.h"
#include "KThiefSkill.h"
#include "KItemSet.h"
#include "KItemChangeRes.h"
#include "KBuySell.h"
#include "KSubWorldSet.h"
#include "KMissle.h"
#include "KOption.h"
//#include "MyAssert.h"
#include "Scene/ObstacleDef.h"
#include "KMath.h"
#ifndef _SERVER
#include "Scene\KScenePlaceC.h"
#include <time.h>
#endif
#include "KTongProtocol.h"
#include "KLadder.h"

KProtocolProcess g_ProtocolProcess;
KProtocolProcess::KProtocolProcess()
{
	ZeroMemory(ProcessFunc, sizeof(ProcessFunc));
#ifndef _SERVER
	ProcessFunc[s2c_login] = NULL;
	ProcessFunc[s2c_logout] = NULL;
	ProcessFunc[s2c_syncend] = &KProtocolProcess::SyncEnd;
	ProcessFunc[s2c_synccurplayer] = &KProtocolProcess::SyncCurPlayer;
	ProcessFunc[s2c_synccurplayerskill] = &KProtocolProcess::s2cSyncAllSkill;
	ProcessFunc[s2c_synccurplayernormal] = &KProtocolProcess::SyncCurNormalData;
	ProcessFunc[s2c_newplayer] = NULL;
	ProcessFunc[s2c_removeplayer] = NULL;
	ProcessFunc[s2c_syncworld] = &KProtocolProcess::SyncWorld;
	ProcessFunc[s2c_syncplayer] = &KProtocolProcess::SyncPlayer;
	ProcessFunc[s2c_syncplayermin] = &KProtocolProcess::SyncPlayerMin;
	ProcessFunc[s2c_syncnpc] = &KProtocolProcess::SyncNpc;
	ProcessFunc[s2c_syncnpcmin] = &KProtocolProcess::SyncNpcMin;
	ProcessFunc[s2c_syncnpcminplayer] = &KProtocolProcess::SyncNpcMinPlayer;
	ProcessFunc[s2c_objadd] = &KProtocolProcess::SyncObjectAdd;
	ProcessFunc[s2c_syncobjstate] = &KProtocolProcess::SyncObjectState;
	ProcessFunc[s2c_syncobjdir] = &KProtocolProcess::SyncObjectDir;
	ProcessFunc[s2c_objremove] = &KProtocolProcess::SyncObjectRemove;
	ProcessFunc[s2c_npcremove] = &KProtocolProcess::NetCommandRemoveNpc;
	ProcessFunc[s2c_npcwalk] = &KProtocolProcess::NetCommandWalk;
	ProcessFunc[s2c_npcrun] = &KProtocolProcess::NetCommandRun;
	ProcessFunc[s2c_npcjump] = &KProtocolProcess::NetCommandJump;
	ProcessFunc[s2c_npchurt] = &KProtocolProcess::NetCommandHurt;
	ProcessFunc[s2c_npcdeath] = &KProtocolProcess::NetCommandDeath;
	ProcessFunc[s2c_npcchgcurcamp] = &KProtocolProcess::NetCommandChgCurCamp;
	ProcessFunc[s2c_npcchgcamp] = &KProtocolProcess::NetCommandChgCamp;
	ProcessFunc[s2c_skillcast] = &KProtocolProcess::NetCommandSkill;
	ProcessFunc[s2c_playerexp] = &KProtocolProcess::s2cPlayerExp;
	ProcessFunc[s2c_teaminfo] = &KProtocolProcess::s2cShowTeamInfo;
	ProcessFunc[s2c_teamselfinfo] = &KProtocolProcess::s2cUpdataSelfTeamInfo;
	ProcessFunc[s2c_teamapplyinfofalse] = &KProtocolProcess::s2cApplyTeamInfoFalse;
	ProcessFunc[s2c_teamcreatesuccess] = &KProtocolProcess::s2cCreateTeam;
	ProcessFunc[s2c_teamcreatefalse] = &KProtocolProcess::s2cApplyCreateTeamFalse;
	ProcessFunc[s2c_teamchangestate] = &KProtocolProcess::s2cSetTeamState;
	ProcessFunc[s2c_teamgetapply] = &KProtocolProcess::s2cApplyAddTeam;
	ProcessFunc[s2c_teamaddmember] = &KProtocolProcess::s2cTeamAddMember;
	ProcessFunc[s2c_teamleave] = &KProtocolProcess::s2cLeaveTeam;
	ProcessFunc[s2c_teamchangecaptain] = &KProtocolProcess::s2cTeamChangeCaptain;
	ProcessFunc[s2c_playerfactiondata] = &KProtocolProcess::s2cSetFactionData;
	ProcessFunc[s2c_playerleavefaction] = &KProtocolProcess::s2cLeaveFaction;
	ProcessFunc[s2c_playermissiondata] = &KProtocolProcess::s2cSetMissionData;
	ProcessFunc[s2c_playersyncleadexp] = &KProtocolProcess::s2cGetLeadExp;
	ProcessFunc[s2c_playerlevelup] = &KProtocolProcess::s2cLevelUp;
	ProcessFunc[s2c_teammatelevel] = &KProtocolProcess::s2cGetTeammateLevel;
	ProcessFunc[s2c_playersyncattribute] = &KProtocolProcess::s2cGetCurAttribute;
	ProcessFunc[s2c_playerskilllevel] = &KProtocolProcess::s2cGetSkillLevel;
	ProcessFunc[s2c_syncitem] = &KProtocolProcess::s2cSyncItem;
	ProcessFunc[s2c_syncmagic] = &KProtocolProcess::s2cSyncMagic;
	ProcessFunc[s2c_removeitem] = &KProtocolProcess::s2cRemoveItem;
	ProcessFunc[s2c_syncmoney] = &KProtocolProcess::s2cSyncMoney;
	ProcessFunc[s2c_playermoveitem] = &KProtocolProcess::s2cMoveItem;
	ProcessFunc[s2c_scriptaction] = &KProtocolProcess::SyncScriptAction;
	ProcessFunc[s2c_syncrolelist] = &KProtocolProcess::s2cSyncRoleList;
	ProcessFunc[s2c_tradechangestate] = &KProtocolProcess::s2cTradeChangeState;
	ProcessFunc[s2c_npcsetmenustate] = &KProtocolProcess::s2cNpcSetMenuState;
	ProcessFunc[s2c_trademoneysync] = &KProtocolProcess::s2cTradeMoneySync;
	ProcessFunc[s2c_tradedecision] = &KProtocolProcess::s2cTradeDecision;
	ProcessFunc[s2c_teaminviteadd] = &KProtocolProcess::s2cTeamInviteAdd;
	ProcessFunc[s2c_tradepressoksync] = &KProtocolProcess::s2cTradePressOkSync;
	ProcessFunc[s2c_ping] = &KProtocolProcess::s2cPing;
	ProcessFunc[s2c_npcsit] = &KProtocolProcess::NetCommandSit;
	ProcessFunc[s2c_npcstand] = &KProtocolProcess::NetCommandStand;
	ProcessFunc[s2c_npcchat] = &KProtocolProcess::NetCommandChat;
	ProcessFunc[s2c_npcsetpos] = &KProtocolProcess::NetCommandSetPos;
	ProcessFunc[s2c_opensalebox] = &KProtocolProcess::OpenSaleBox;
	ProcessFunc[s2c_castskilldirectly] = &KProtocolProcess::s2cDirectlyCastSkill;
	ProcessFunc[s2c_msgshow] = &KProtocolProcess::s2cShowMsg;
	ProcessFunc[s2c_syncstateeffect] = &KProtocolProcess::SyncStateEffect;
	ProcessFunc[s2c_ignorestate] = &KProtocolProcess::IgnoreState;
	ProcessFunc[s2c_opensomebox] = &KProtocolProcess::OpenSomeBox;
	ProcessFunc[s2c_playerrevive] = &KProtocolProcess::PlayerRevive;
	ProcessFunc[s2c_requestnpcfail] = &KProtocolProcess::RequestNpcFail;
	ProcessFunc[s2c_tradeapplystart] = &KProtocolProcess::s2cTradeApplyStart;
	ProcessFunc[s2c_createtong] = &KProtocolProcess::s2cCreateTong;
	ProcessFunc[s2c_jointong] = &KProtocolProcess::s2cJoinTong;
	ProcessFunc[s2c_jointongreply] = &KProtocolProcess::s2cJoinTongReply;
	ProcessFunc[s2c_rolenewdelresponse] = NULL;
	ProcessFunc[s2c_ItemAutoMove] = &KProtocolProcess::s2cItemAutoMove;
	ProcessFunc[s2c_changeweather] = &KProtocolProcess::s2cChangeWeather;
	ProcessFunc[s2c_pksyncnormalflag] = &KProtocolProcess::s2cPKSyncNormalFlag;
	ProcessFunc[s2c_pksyncenmitystate] = &KProtocolProcess::s2cPKSyncEnmityState;
	ProcessFunc[s2c_pksyncexercisestate] = &KProtocolProcess::s2cPKSyncExerciseState;
	ProcessFunc[s2c_pksyncpkvalue] = &KProtocolProcess::s2cPKValueSync;
	ProcessFunc[s2c_npcsleepmode] = &KProtocolProcess::NpcSleepSync;
	ProcessFunc[s2c_viewequip] = &KProtocolProcess::s2cViewEquip;
	ProcessFunc[s2c_ladderresult] = &KProtocolProcess::LadderResult;
	ProcessFunc[s2c_ladderlist] = &KProtocolProcess::LadderList;
	ProcessFunc[s2c_tongcreate] = &KProtocolProcess::s2cTongCreate;
	ProcessFunc[s2c_replyclientping] = &KProtocolProcess::ServerReplyClientPing;
	ProcessFunc[s2c_itemdurabilitychange] = &KProtocolProcess::ItemChangeDurability;
	ProcessFunc[s2c_playertradeaction] = &KProtocolProcess::SendTradeAction;
	ProcessFunc[s2c_playertradeitem] = &KProtocolProcess::SendTradeItem;
	ProcessFunc[s2c_playertradecount] = &KProtocolProcess::SendTradeCount;	
	ProcessFunc[s2c_playertradefail] = &KProtocolProcess::SendTradeFail;	
	ProcessFunc[s2c_titlename] = &KProtocolProcess::NetCommandSetRank;
	ProcessFunc[s2c_expandrank] = &KProtocolProcess::NetCommandSetExpandRank;
	ProcessFunc[s2c_npchorsesync] = &KProtocolProcess::NetCommandSetHorse;
	ProcessFunc[s2c_synctaskvalue] = &KProtocolProcess::s2cSyncTaskValue;
	ProcessFunc[s2c_playersync] = &KProtocolProcess::s2cPlayerSync;
	ProcessFunc[s2c_extpoint] = &KProtocolProcess::SetExtPoint;
	ProcessFunc[s2c_opengive] = &KProtocolProcess::SyncGive;
	ProcessFunc[s2c_syncmasklock] = &KProtocolProcess::SyncMaskLock;
	ProcessFunc[s2c_syncrankdata] = &KProtocolProcess::s2cSyncRankData;
	ProcessFunc[s2c_syncsupershop] = &KProtocolProcess::s2cSyncSuperShop;
	ProcessFunc[s2c_sparapplystart] = &KProtocolProcess::s2cSparApplyStart;
	ProcessFunc[s2c_lockmove] = &KProtocolProcess::s2cLockMove;
	ProcessFunc[s2c_chatroomstate] = &KProtocolProcess::s2cChatRoomState;
	ProcessFunc[s2c_chatroomcreatefalse] = &KProtocolProcess::s2cApplyCreateChatRoomFalse;
	ProcessFunc[s2c_requestchatroomlist] = &KProtocolProcess::s2cRequestChatRoomList;
	ProcessFunc[s2c_requestmemberlist] = &KProtocolProcess::s2cRequestMemberList;
	ProcessFunc[s2c_requestblacklist] = &KProtocolProcess::s2cRequestBlackList;

	ProcessFunc[s2c_extend] = &KProtocolProcess::s2cExtend;
	ProcessFunc[s2c_extendchat] = &KProtocolProcess::s2cExtendChat;
	ProcessFunc[s2c_extendfriend] = &KProtocolProcess::s2cExtendFriend;
	ProcessFunc[s2c_extendtong] = &KProtocolProcess::s2cExtendTong;


#else
	ProcessFunc[c2s_login] = NULL;
	ProcessFunc[c2s_logiclogin] = NULL;
	ProcessFunc[c2s_syncend] = NULL;
	ProcessFunc[c2s_loadplayer] = NULL;
	ProcessFunc[c2s_newplayer] = NULL;
	ProcessFunc[c2s_removeplayer] = &KProtocolProcess::RemoveRole;
	ProcessFunc[c2s_requestworld] = NULL;
	ProcessFunc[c2s_requestplayer] = NULL;
	ProcessFunc[c2s_requestnpc] = &KProtocolProcess::NpcRequestCommand;
	ProcessFunc[c2s_requestobj] = &KProtocolProcess::ObjRequestCommand;
	ProcessFunc[c2s_npcwalk] = &KProtocolProcess::NpcWalkCommand;
	ProcessFunc[c2s_npcrun] = &KProtocolProcess::NpcRunCommand;
	ProcessFunc[c2s_npcskill] = &KProtocolProcess::NpcSkillCommand;
	ProcessFunc[c2s_teamapplyinfo] = &KProtocolProcess::PlayerApplyTeamInfo;
	ProcessFunc[c2s_teamapplycreate] = &KProtocolProcess::PlayerApplyCreateTeam;
	ProcessFunc[c2s_teamapplychangestate] = &KProtocolProcess::PlayerApplyTeamChangeState;
	ProcessFunc[c2s_teamapplyadd] = &KProtocolProcess::PlayerApplyAddTeam;
	ProcessFunc[c2s_teamacceptmember] = &KProtocolProcess::PlayerAcceptTeamMember;
	ProcessFunc[c2s_teamapplyleave] = &KProtocolProcess::PlayerApplyLeaveTeam;
	ProcessFunc[c2s_teamapplykickmember] = &KProtocolProcess::PlayerApplyTeamKickMember;
	ProcessFunc[c2s_teamapplychangecaptain] = &KProtocolProcess::PlayerApplyTeamChangeCaptain;
	ProcessFunc[c2s_teamapplydismiss] = &KProtocolProcess::PlayerApplyTeamDismiss;
	ProcessFunc[c2s_playerapplysetpk] = &KProtocolProcess::PlayerApplySetPK;
	ProcessFunc[c2s_playeraddbaseattribute] = &KProtocolProcess::PlayerAddBaseAttribute;
	ProcessFunc[c2s_playerapplyaddskillpoint] = &KProtocolProcess::PlayerApplyAddSkillPoint;
	ProcessFunc[c2s_playereatitem] = &KProtocolProcess::PlayerEatItem;
	ProcessFunc[c2s_playerpickupitem] = &KProtocolProcess::PlayerPickUpItem;
	ProcessFunc[c2s_playermoveitem] = &KProtocolProcess::PlayerMoveItem;
	ProcessFunc[c2s_playersellitem] = &KProtocolProcess::PlayerSellItem;
	ProcessFunc[c2s_playerbuyitem] = &KProtocolProcess::PlayerBuyItem;
	ProcessFunc[c2s_playerthrowawayitem] = &KProtocolProcess::PlayerDropItem;
	ProcessFunc[c2s_playerselui] = &KProtocolProcess::PlayerSelUI;
	ProcessFunc[c2s_dbplayerselect] = NULL;
	ProcessFunc[c2s_tradeapplystateopen] = &KProtocolProcess::TradeApplyOpen;
	ProcessFunc[c2s_tradeapplystateclose] = &KProtocolProcess::TradeApplyClose;
	ProcessFunc[c2s_tradeapplystart] = &KProtocolProcess::TradeApplyStart;
	ProcessFunc[c2s_trademovemoney] = &KProtocolProcess::TradeMoveMoney;
	ProcessFunc[c2s_tradedecision] = &KProtocolProcess::TradeDecision;
	ProcessFunc[c2s_dialognpc]	= &KProtocolProcess::DialogNpc;
	ProcessFunc[c2s_teaminviteadd]	= &KProtocolProcess::TeamInviteAdd;
	ProcessFunc[c2s_changeauraskill] = &KProtocolProcess::ChangeAuraSkill;
	ProcessFunc[c2s_teamreplyinvite] = &KProtocolProcess::TeamReplyInvite;
	ProcessFunc[c2s_ping] = NULL;//ReplyPing;
	ProcessFunc[c2s_npcsit] = &KProtocolProcess::NpcSitCommand;
	ProcessFunc[c2s_objmouseclick] = &KProtocolProcess::ObjMouseClick;
	ProcessFunc[c2s_storemoney] = &KProtocolProcess::StoreMoneyCommand;
	ProcessFunc[c2s_playerrevive] = &KProtocolProcess::NpcReviveCommand;
	ProcessFunc[c2s_tradereplystart] = &KProtocolProcess::c2sTradeReplyStart;
	ProcessFunc[c2s_pkapplychangenormalflag] = &KProtocolProcess::c2sPKApplyChangeNormalFlag;
	ProcessFunc[c2s_pkapplyenmity] = &KProtocolProcess::c2sPKApplyEnmity;
	ProcessFunc[c2s_viewequip] = &KProtocolProcess::c2sViewEquip;
	ProcessFunc[c2s_ladderquery] = &KProtocolProcess::LadderQuery;
	ProcessFunc[c2s_repairitem] = &KProtocolProcess::ItemRepair;
	ProcessFunc[c2s_playertradeset] = &KProtocolProcess::TradeSet;
	ProcessFunc[c2s_playertradestart] = &KProtocolProcess::TradeStart;
	ProcessFunc[c2s_playertradeview] = &KProtocolProcess::TradeView;
	ProcessFunc[c2s_playertradebuy] = &KProtocolProcess::TradeBuy;
	ProcessFunc[c2s_playertradeviewend] = &KProtocolProcess::TradeEnd;
	ProcessFunc[c2s_npchorse] = &KProtocolProcess::NpcHorseCommand;
	ProcessFunc[c2s_playercommand] = &KProtocolProcess::PlayerCommand;
	ProcessFunc[c2s_inputcommand] = &KProtocolProcess::InputCommand;
	ProcessFunc[c2s_unlockcommand] = &KProtocolProcess::UnlockCommand;
	ProcessFunc[c2s_playerbreakcommand] = &KProtocolProcess::BreakCommand ;
	ProcessFunc[c2s_jointongreply] = &KProtocolProcess::JoinTongReply;
	ProcessFunc[c2s_sparapplystart] = &KProtocolProcess::SparApplyStart;
	ProcessFunc[c2s_lockmove] = &KProtocolProcess::LockMove;
	ProcessFunc[c2s_chatroomdecision] = &KProtocolProcess::ChatRoomDecision;
	ProcessFunc[c2s_cpsetimage] = &KProtocolProcess::c2sSetImage;
	
#endif
}

KProtocolProcess::~KProtocolProcess()
{
}

#ifndef _SERVER
void KProtocolProcess::ProcessNetMsg(BYTE* pMsg)
{
	if (!pMsg || pMsg[0] <= s2c_clientbegin || pMsg[0] >= s2c_end || ProcessFunc[pMsg[0]] == NULL)
	{
		if (!pMsg)
			g_DebugLog("[Net Msg Error]!pMsg");
		else if (pMsg[0] <= s2c_clientbegin)
			g_DebugLog("[Net Msg Error]%d <= %d", pMsg[0], s2c_clientbegin);
		else if (pMsg[0] >= s2c_end)
			g_DebugLog("[Net Msg Error]%d >= %d", pMsg[0], s2c_end);
		else if (ProcessFunc[pMsg[0]] == NULL)
			g_DebugLog("[Net Msg Error]ProcessFunc[%d] == NULL", pMsg[0]);
		return;
	}

	if (ProcessFunc[pMsg[0]])
	{
		if(Player[CLIENT_PLAYER_INDEX].m_ItemList.IsLockOperation())
			Player[CLIENT_PLAYER_INDEX].m_ItemList.UnlockOperation();
		(this->*ProcessFunc[pMsg[0]])(pMsg);
	}
}
#else
void KProtocolProcess::ProcessNetMsg(int nIndex, BYTE* pMsg)
{
	_ASSERT(pMsg && pMsg[0] > c2s_gameserverbegin && pMsg[0] < c2s_end);

	BYTE	byProtocol = pMsg[0];
	_ASSERT(nIndex > 0 && nIndex < MAX_PLAYER);
	if (!pMsg || pMsg[0] <= c2s_gameserverbegin || pMsg[0] >= c2s_end || ProcessFunc[pMsg[0]] == NULL)
	{
		if (!pMsg)
			g_DebugLog("[error-Server Protocol]!pMsg");
		else if (pMsg[0] <= s2c_clientbegin)
			g_DebugLog("[error-Server Protocol]%d <= %d", pMsg[0], c2s_gameserverbegin);
		else if (pMsg[0] >= s2c_end)
			g_DebugLog("[error-Server Protocol]%d >= %d", pMsg[0], c2s_end);
		else if (ProcessFunc[pMsg[0]] == NULL)
			g_DebugLog("[error-Server Protocol]ProcessFunc[%d] == NULL", pMsg[0]);
		return;
	}
	if (ProcessFunc[byProtocol])
	{
		if (nIndex > 0 && nIndex < MAX_PLAYER)
		{
			if (Player[nIndex].GetLockState() && 
				(byProtocol == c2s_tradeapplystateopen
				|| byProtocol == c2s_tradeapplystart
				|| byProtocol == c2s_playerbuyitem
				|| byProtocol == c2s_playersellitem
				|| byProtocol == c2s_storemoney
				|| byProtocol == c2s_playertradestart
				|| byProtocol == c2s_playertradebuy
				/*|| byProtocol == c2s_playereatitem*/
				|| byProtocol == c2s_playerthrowawayitem)
				)
			{
				SHOW_MSG_SYNC	sMsg;
				sMsg.ProtocolType = s2c_msgshow;
				sMsg.m_wMsgID = enumMSG_ID_FUNCTION_LOCKED;
				sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
				g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
				return;
			}
			else if (Player[nIndex].m_nPaceBarTime && 
				(byProtocol >= c2s_npcwalk)
				)
			{
				Player[nIndex].m_nPaceBarTime = 0;
				Player[nIndex].m_nPaceBarTimeMax = 0;
			}
			(this->*ProcessFunc[byProtocol])(nIndex, pMsg);
			Player[nIndex].SetLastNetOperationTime(g_SubWorldSet.GetGameTime());
		}
	}
}
#endif



#ifndef _SERVER
void KProtocolProcess::NpcSleepSync(BYTE* pMsg)
{
	NPC_SLEEP_SYNC*	pSync = (NPC_SLEEP_SYNC *)pMsg;
	DWORD	dwNpcId = pSync->NpcID;

	int nIdx = NpcSet.SearchID(dwNpcId);

	if (nIdx > 0)
	{
		Npc[nIdx].SetSleepMode((BOOL)pSync->bSleep);
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}
}

void KProtocolProcess::s2cPing(BYTE* pMsg)
{
//	DWORD	dwTimer = GetTickCount();
//	PING_COMMAND* PingCmd = (PING_COMMAND *)pMsg;
//
//	dwTimer -= PingCmd->m_dwTime;
//	dwTimer >>= 1;
//	g_SubWorldSet.SetPing(dwTimer);
//	g_bPingReply = TRUE;

	DWORD	dwTimer = GetTickCount();
	PING_COMMAND*	PingCmd = (PING_COMMAND *)pMsg;
	PING_CLIENTREPLY_COMMAND	pcc;
	pcc.ProtocolType = c2s_ping;
	pcc.m_dwReplyServerTime = PingCmd->m_dwTime;
	pcc.m_dwClientTime = dwTimer;
	g_pClient->SendPackToServer(&pcc, sizeof(PING_CLIENTREPLY_COMMAND));
	//g_SubWorldSet.SetPing(PingCmd->m_dwTime);
}

void KProtocolProcess::ServerReplyClientPing(BYTE* pMsg)
{
	DWORD	dwTimer = GetTickCount();
	PING_COMMAND* pPc = (PING_COMMAND *)pMsg;

	dwTimer -= pPc->m_dwTime;
	dwTimer >>= 1;
	g_SubWorldSet.SetPing(dwTimer);
}

void KProtocolProcess::s2cPlayerExp(BYTE* pMsg)
{
	PLAYER_EXP_SYNC	*pExp = (PLAYER_EXP_SYNC*)pMsg;
	Player[CLIENT_PLAYER_INDEX].s2cSetExp(pExp->m_nExp);
}

void KProtocolProcess::NetCommandChgCamp(BYTE* pMsg)
{
	DWORD	dwNpcId;

	dwNpcId = *(DWORD *)&pMsg[1];
	int nIdx = NpcSet.SearchID(dwNpcId);
	if (nIdx > 0)
	{
		Npc[nIdx].SetCamp((int)pMsg[5]);
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}
}

void KProtocolProcess::NetCommandSetRank(BYTE* pMsg)
{
	DWORD	dwNpcId;

	dwNpcId = *(DWORD *)&pMsg[1];
	int nIdx = NpcSet.SearchID(dwNpcId);

	if (nIdx > 0)
	{
		Npc[nIdx].m_RankID = (int)pMsg[5];
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}
}

void KProtocolProcess::NetCommandSetExpandRank(BYTE* pMsg)
{
	EXPANDTITLE_SYNC *Cmd = (EXPANDTITLE_SYNC*)pMsg;

	DWORD	dwNpcId;

	dwNpcId = Cmd->ID;
	int nIdx = NpcSet.SearchID(dwNpcId);

	if (nIdx > 0)
	{
		Npc[nIdx].SetExpandRank(&Cmd->ExpandRank);
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}
}

void	KProtocolProcess::s2cSyncTaskValue(BYTE* pMsg)
{
	S2C_SYNCTASKVALUE *Cmd = (S2C_SYNCTASKVALUE*)pMsg;
	Player[CLIENT_PLAYER_INDEX].m_cTask.SetSaveVal(Cmd->nTaskId, Cmd->szTaskValue);
}

void KProtocolProcess::SetExtPoint(BYTE* nPacket)
{
	S2C_EXTPOINT *temp = (S2C_EXTPOINT*)nPacket;
	
	Player[CLIENT_PLAYER_INDEX].SetExtPoint(temp->nExtPoint,temp->nChangeExtPoint);
	if (temp->nChangeExtPoint)
	{
		APPLY_GET_EXTPOINT_COMMAND	nExt;
		nExt.ProtocolType	= c2s_extendtong;
		nExt.m_wLength		= sizeof(nExt) - 1;
		nExt.m_btMsgId		= enumTONG_COMMAND_ID_EXTPOINT;
		nExt.m_nExtPoint 	= temp->nExtPoint;
		if (g_pClient)
			g_pClient->SendPackToServer(&nExt, nExt.m_wLength + 1);
	}
}

void	KProtocolProcess::SyncGive(BYTE* pMsg)
{
	if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
		return;

	PLAYER_GIVE *temp = (PLAYER_GIVE *)pMsg;
	CoreDataChanged(GDCNI_GIVE, (unsigned int)temp->m_szName, (int)temp->m_szInitString);
}

void KProtocolProcess::SyncMaskLock(BYTE* pMsg)
{
	NPC_MASK_SYNC *pInfo = (NPC_MASK_SYNC *)pMsg;
	Player[CLIENT_PLAYER_INDEX].m_ItemList.SetMaskLock(pInfo->ID);
}

void KProtocolProcess::NetCommandChgCurCamp(BYTE* pMsg)
{
	DWORD	dwNpcId;

	dwNpcId = *(DWORD *)&pMsg[1];
	int nIdx = NpcSet.SearchID(dwNpcId);

	if (nIdx > 0)
	{
		Npc[nIdx].ChangeCurrentCamp((int)pMsg[5]);
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}
}

void KProtocolProcess::NetCommandDeath(BYTE* pMsg)
{
	DWORD	dwNpcID;
	
	dwNpcID = *(DWORD *)&pMsg[1];
	int nIdx = NpcSet.SearchID(dwNpcID);

	if (nIdx > 0)
	{
		Npc[nIdx].ProcNetCommand(do_death);
		Npc[nIdx].m_CurrentLife = 0;
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}
}

void KProtocolProcess::NetCommandJump(BYTE* pMsg)
{
	NPC_JUMP_SYNC* pNetCommandJump = (NPC_JUMP_SYNC *)pMsg;
	DWORD dwNpcId = pNetCommandJump->ID;
	int nIdx = NpcSet.SearchID(dwNpcId);
	
	if (Player[CLIENT_PLAYER_INDEX].ConformIdx(nIdx))
	{
		Npc[nIdx].SendCommand(do_jump, pNetCommandJump->nMpsX, pNetCommandJump->nMpsY);
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}
}

void KProtocolProcess::NetCommandHurt(BYTE* pMsg)
{
	NPC_HURT_SYNC*	pSync = (NPC_HURT_SYNC *)pMsg;
	
	int nIdx = NpcSet.SearchID(pSync->ID);
	if (nIdx > 0)
	{
		Npc[nIdx].ProcNetCommand(do_hurt, pSync->nFrames, pSync->nX, pSync->nY);
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}
}

void KProtocolProcess::NetCommandRemoveNpc(BYTE* pMsg)
{
	DWORD	dwNpcID;
	dwNpcID = *(DWORD *)&pMsg[1];
	int nIdx = NpcSet.SearchID(dwNpcID);

	if (Player[CLIENT_PLAYER_INDEX].ConformIdx(nIdx))
	{
		if (Npc[nIdx].m_RegionIndex >= 0)
		{
			if(*(BOOL *)&pMsg[5])
				SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].RemoveNpc(nIdx);
			SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].DecRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, obj_npc);
		}
		NpcSet.Remove(nIdx);
	}
}

void KProtocolProcess::NetCommandRun(BYTE* pMsg)
{
	DWORD	dwNpcID;
	DWORD	MapX, MapY;
	
	dwNpcID = *(DWORD *)&pMsg[1];
	MapX = *(int *)&pMsg[5];
	MapY = *(int *)&pMsg[9];
	int nIdx = NpcSet.SearchID(dwNpcID);
	if (Player[CLIENT_PLAYER_INDEX].ConformIdx(nIdx))
	{
		Npc[nIdx].SendCommand(do_run, MapX, MapY);
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}
}

void KProtocolProcess::NetCommandSit(BYTE* pMsg)
{
	NPC_SIT_SYNC*	pSitSync;

	pSitSync = (NPC_SIT_SYNC *)pMsg;

	int nIdx = NpcSet.SearchID(pSitSync->ID);
	Npc[nIdx].SendCommand(do_sit);
	Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
}

void KProtocolProcess::NetCommandStand(BYTE* pMsg)
{
	NPC_PLAYER_TYPE_NORMAL_SYNC*	pStandSync = (NPC_PLAYER_TYPE_NORMAL_SYNC *)pMsg;

	int nIdx = NpcSet.SearchID(pStandSync->ID);

	if(nIdx)
	{
		if (Player[CLIENT_PLAYER_INDEX].ConformIdx(nIdx))
		{
			Npc[nIdx].SendCommand(do_stand);
			Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
		}
		else if(Npc[nIdx].m_Kind == kind_player)
		{
			int nMpsX, nMpsY;
			Npc[nIdx].GetMpsPos(&nMpsX, &nMpsY);
			
			if(g_GetDistance(nMpsX, nMpsY, pStandSync->MapX, pStandSync->MapY) < 256)
				return;

			int nRegion, nMapX, nMapY, nOffX, nOffY;
			SubWorld[0].Mps2Map(pStandSync->MapX, pStandSync->MapY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
			if(nRegion > 0)
			{
				SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].DecRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, obj_npc);

				_ASSERT(nRegion >= 0);
				Npc[nIdx].m_RegionIndex = nRegion;
				Npc[nIdx].m_dwRegionID = SubWorld[0].m_Region[nRegion].m_RegionID;
				SubWorld[0].NpcChangeRegion(-1, SubWorld[0].m_Region[nRegion].m_RegionID, nIdx);

				SubWorld[0].Mps2Map(pStandSync->MapX, pStandSync->MapY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
				Npc[nIdx].m_MapX = nMapX;
				Npc[nIdx].m_MapY = nMapY;
				Npc[nIdx].m_OffX = pStandSync->OffX;
				Npc[nIdx].m_OffY = pStandSync->OffY;
				
				SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].AddRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, obj_npc);
			}
		}
	}
}

void KProtocolProcess::NetCommandChat(BYTE* pMsg)
{
	NPC_CHAT_SYNC *pSync = (NPC_CHAT_SYNC*)pMsg;

	int nIdx = NpcSet.SearchID(pSync->ID);

	if (nIdx > 0)
	{
		Npc[nIdx].SetChatInfo(Npc[nIdx].Name, pSync->szMsg, pSync->nMsgLen);
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}
}

void KProtocolProcess::NetCommandSetPos(BYTE* pMsg)
{
	NPC_PLAYER_TYPE_NORMAL_SYNC *pSync = (NPC_PLAYER_TYPE_NORMAL_SYNC*)pMsg;
	
	int nIdx = NpcSet.SearchID(pSync->ID);
	if(nIdx > 0)
	{
		int nRegion, nMapX, nMapY, nOffX, nOffY;
		SubWorld[0].Mps2Map(pSync->MapX, pSync->MapY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);

		SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].DecRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, obj_npc);

		int nRegionX = pSync->MapX / (SubWorld[0].m_nCellWidth * SubWorld[0].m_nRegionWidth);
		int nRegionY = pSync->MapY / (SubWorld[0].m_nCellHeight * SubWorld[0].m_nRegionHeight);
		
		DWORD	dwRegionID = MAKELONG(nRegionX, nRegionY);
		SubWorld[0].LoadMap(SubWorld[0].m_SubWorldID, dwRegionID);

		nRegion = SubWorld[0].FindRegion(dwRegionID);
		_ASSERT(nRegion >= 0);
		Npc[nIdx].m_RegionIndex = nRegion;
		Npc[nIdx].m_dwRegionID = dwRegionID;
		SubWorld[0].NpcChangeRegion(-1, SubWorld[0].m_Region[nRegion].m_RegionID, nIdx);

		SubWorld[0].Mps2Map(pSync->MapX, pSync->MapY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
		Npc[nIdx].m_MapX = nMapX;
		Npc[nIdx].m_MapY = nMapY;
		Npc[nIdx].m_OffX = pSync->OffX;
		Npc[nIdx].m_OffY = pSync->OffY;
		
		SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].AddRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, obj_npc);

		Npc[nIdx].SendCommand(do_stand);
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}
}

void KProtocolProcess::OpenSaleBox(BYTE* pMsg)
{
	SALE_BOX_SYNC* pSale = (SALE_BOX_SYNC *)pMsg;

	BuySell.OpenSale((BuySellInfo*)&pSale->m_BuySellInfo);
}

void KProtocolProcess::OpenSomeBox(BYTE* pMsg)
{
	SOME_BOX_SYNC* pBox = (SOME_BOX_SYNC *)pMsg;

	if (pBox->bEquipEx)
		CoreDataChanged(GDCNI_OPEN_EQUIPEX_BOX, NULL, NULL);
	else
		CoreDataChanged(GDCNI_OPEN_STORE_BOX, pBox->nBoxIndex, NULL);
}


void KProtocolProcess::PlayerRevive(BYTE* pMsg)
{
	NPC_REVIVE_SYNC* pSync = (NPC_REVIVE_SYNC*)pMsg;

	int nIdx = NpcSet.SearchID(pSync->ID);
	if (nIdx > 0)
	{
		if (!Npc[nIdx].IsPlayer() && pSync->Type == REMOTE_REVIVE_TYPE)
		{
			SubWorld[0].m_WorldMessage.Send(GWM_NPC_DEL, nIdx);
			return;
		}
		else
		{
			Npc[nIdx].ProcNetCommand(do_revive);
		}
	}
}

void KProtocolProcess::RequestNpcFail(BYTE* pMsg)
{
	NPC_REQUEST_FAIL* pNpcSync = (NPC_REQUEST_FAIL *)pMsg;

	if (NpcSet.IsNpcRequestExist(pNpcSync->ID))
		NpcSet.RemoveNpcRequest(pNpcSync->ID);	
}

void KProtocolProcess::NetCommandSkill(BYTE* pMsg)
{
	DWORD	dwNpcID;
	int		nSkillID, nSkillLevel;
	int		MapX, MapY;
	
	dwNpcID = *(DWORD *)&pMsg[1];
	nSkillID = *(int *)&pMsg[5];
	nSkillLevel = *(int *)&pMsg[9];
	MapX = *(int *)&pMsg[13];
	MapY = *(int *)&pMsg[17];
	if (MapY < 0)
		return ;

	//当指定某个目标时(MapX == -1),MapY为目标的NpcdwID，需要转换成本地的NpcIndex才行
	if (MapX < 0)
	{
		if (MapX != -1)
			return;
	
		MapY = NpcSet.SearchID(MapY);
		if (MapY == 0)
			return;

		if (Npc[MapY].m_RegionIndex < 0)
			return;

	}
	
	int nIdx = NpcSet.SearchID(dwNpcID);
	
	if (nIdx <= 0) 
		return;

	if (Player[CLIENT_PLAYER_INDEX].ConformIdx(nIdx))
	{
		Npc[nIdx].m_SkillList.SetSkillLevel(nSkillID, nSkillLevel);
		Npc[nIdx].SendCommand(do_skill, nSkillID, MapX, MapY);
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}
}

void KProtocolProcess::NetCommandWalk(BYTE* pMsg)
{
	DWORD	dwNpcID;
	DWORD	MapX, MapY;
	
	dwNpcID = *(DWORD *)&pMsg[1];
	MapX = *(int *)&pMsg[5];
	MapY = *(int *)&pMsg[9];
	int nIdx = NpcSet.SearchID(dwNpcID);
	if (Player[CLIENT_PLAYER_INDEX].ConformIdx(nIdx))
	{
		Npc[nIdx].SendCommand(do_walk, MapX, MapY);
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}
}

//-------------------------------------------------------------------------
//	功能：收到服务器通知队伍创建失败
//-------------------------------------------------------------------------
void KProtocolProcess::s2cApplyCreateTeamFalse(BYTE* pMsg)
{
	PLAYER_SEND_CREATE_TEAM_FALSE *pCreateFalse = (PLAYER_SEND_CREATE_TEAM_FALSE*)pMsg;
	KSystemMessage	sMsg;

	switch (pCreateFalse->m_btErrorID)
	{
	// 已经在队伍中，说明客户端队伍数据有错误，申请重新获得队伍数据
	case Team_Create_Error_InTeam:
		Player[CLIENT_PLAYER_INDEX].ApplySelfTeamInfo();
		break;

	// 当前处于不能组队状态
	case Team_Create_Error_CannotCreate:
		sprintf(sMsg.szMessage, MSG_TEAM_CANNOT_CREATE);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		break;
	case Team_Create_Error_NormalPK:
		sprintf(sMsg.szMessage, MSG_TEAM_CREATE_FAIL1);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		break;

	default:
		sprintf(sMsg.szMessage, MSG_TEAM_CREATE_FAIL);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		break;
	}
	Player[CLIENT_PLAYER_INDEX].m_cTeam.UpdateInterface();
}

//-------------------------------------------------------------------------
//	功能：收到服务器通知查询某个队伍组队情况失败
//-------------------------------------------------------------------------
void KProtocolProcess::s2cApplyTeamInfoFalse(BYTE* pMsg)
{
	// 界面显示队伍查询失败(not end)
}
//-------------------------------------------------------------------------
//	功能：收到服务器通知创建一支队伍
//-------------------------------------------------------------------------
void KProtocolProcess::s2cCreateTeam(BYTE* pMsg)
{
	PLAYER_SEND_CREATE_TEAM_SUCCESS	*pCreateSuccess = (PLAYER_SEND_CREATE_TEAM_SUCCESS*)pMsg;

	g_Team[0].CreateTeam(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Level, pCreateSuccess->nTeamServerID);
	Player[CLIENT_PLAYER_INDEX].m_cTeam.Release();
	Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag = 1;
	Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure = TEAM_CAPTAIN;
	Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nApplyCaptainID = 0;
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].RestoreCurrentCamp();

	KSystemMessage	sMsg;
	sprintf(sMsg.szMessage, MSG_TEAM_CREATE);
	sMsg.eType = SMT_NORMAL;
	sMsg.byConfirmType = SMCT_NONE;
	sMsg.byPriority = 0;
	sMsg.byParamSize = 0;
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

	sprintf(sMsg.szMessage, MSG_TEAM_CREATE);
	sMsg.eType = SMT_TEAM;
	sMsg.byConfirmType = SMCT_CLICK;
	sMsg.byPriority = 1;
	sMsg.byParamSize = 0;
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetInstantSpr(enumINSTANT_STATE_CREATE_TEAM);
}

//-------------------------------------------------------------------------
//	功能：收到服务器发过来的当前某属性的点数
//-------------------------------------------------------------------------
void KProtocolProcess::s2cGetCurAttribute(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].s2cGetCurAttribute(pMsg);
}

//-------------------------------------------------------------------------
//	功能：收到服务器发来的统率力经验值
//-------------------------------------------------------------------------
void KProtocolProcess::s2cGetLeadExp(BYTE* pMsg)
{
	PLAYER_LEAD_EXP_SYNC	*pLeadExp = (PLAYER_LEAD_EXP_SYNC*)pMsg;
	if (pLeadExp->m_nLeadLevel)
	{
		Player[CLIENT_PLAYER_INDEX].m_nLeadLevel = pLeadExp->m_nLeadLevel;
		Player[CLIENT_PLAYER_INDEX].m_nLeadExp = PlayerSet.m_cLeadExp.GetLevelExp(Player[CLIENT_PLAYER_INDEX].m_nLeadLevel-1);
		Player[CLIENT_PLAYER_INDEX].m_nNextLevelLeadExp = PlayerSet.m_cLeadExp.GetLevelExp(Player[CLIENT_PLAYER_INDEX].m_nLeadLevel);

		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, MSG_LEADER_LEVEL_UP, Player[CLIENT_PLAYER_INDEX].m_nLeadLevel);
		sMsg.eType = SMT_PLAYER;
		sMsg.byConfirmType = SMCT_CLICK;
		sMsg.byPriority = 1;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
	}
	else
	{
		int nLevel = Player[CLIENT_PLAYER_INDEX].m_nLeadLevel;

		Player[CLIENT_PLAYER_INDEX].m_nLeadExp = pLeadExp->m_nLeadExp;
		Player[CLIENT_PLAYER_INDEX].m_nLeadLevel = PlayerSet.m_cLeadExp.GetLevel(Player[CLIENT_PLAYER_INDEX].m_nLeadExp, Player[CLIENT_PLAYER_INDEX].m_nLeadLevel);
		Player[CLIENT_PLAYER_INDEX].m_nNextLevelLeadExp = PlayerSet.m_cLeadExp.GetLevelExp(Player[CLIENT_PLAYER_INDEX].m_nLeadLevel);

		if (nLevel < Player[CLIENT_PLAYER_INDEX].m_nLeadLevel)
		{
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_LEADER_LEVEL_UP, Player[CLIENT_PLAYER_INDEX].m_nLeadLevel);
			sMsg.eType = SMT_PLAYER;
			sMsg.byConfirmType = SMCT_CLICK;
			sMsg.byPriority = 1;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
	}
}

//-------------------------------------------------------------------------
//	功能：收到服务器发过来的某技能点数
//-------------------------------------------------------------------------
void KProtocolProcess::s2cGetSkillLevel(BYTE* pMsg)
{
	int		nSkillIndex;
	PLAYER_SKILL_LEVEL_SYNC	*pSkill = (PLAYER_SKILL_LEVEL_SYNC*)pMsg;
	if (pSkill->m_nSkillLevel > MAX_SKILLLEVEL)
		return;
	nSkillIndex = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.FindSame(pSkill->m_nSkillID);
	Player[CLIENT_PLAYER_INDEX].m_nSkillPoint = pSkill->m_nLeavePoint;
	if ( !nSkillIndex )
	{
		if ( Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.Add(pSkill->m_nSkillID, pSkill->m_nSkillLevel, pSkill->m_nSkillExp, pSkill->m_bTempSkill) == 0 )
			return;
	}
	else
	{
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.SetLevel(nSkillIndex, pSkill->m_nSkillLevel);
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.SetAddLevel(pSkill->m_nSkillID, pSkill->m_nAddLevel);
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.SetCurLevel(pSkill->m_nSkillID, pSkill->m_nSkillLevel + pSkill->m_nAddLevel);
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.SetExp(nSkillIndex, pSkill->m_nSkillExp);
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.SetTempSkill(nSkillIndex, pSkill->m_bTempSkill);
	}

	// 通知界面更新技能数据
	CoreDataChanged(GDCNI_FIGHT_SKILL_POINT, 0, Player[CLIENT_PLAYER_INDEX].m_nSkillPoint);
	KUiSkillData SkillData;

	KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(pSkill->m_nSkillID, 1);
	if (!pOrdinSkill)
		return;
	eSkillStyle eStyle = ( eSkillStyle ) pOrdinSkill->GetSkillStyle();
			
	switch(eStyle)
	{
	case SKILL_SS_Missles:			//	子弹类		本技能用于发送子弹类
	case SKILL_SS_Melee:
	case SKILL_SS_InitiativeNpcState:	//	主动类		本技能用于改变当前Npc的主动状态
	case SKILL_SS_PassivityNpcState:		//	被动类		本技能用于改变Npc的被动状态
		{
			if(pOrdinSkill->IsBase())
				return;
		}break;
	case SKILL_SS_Thief:					//	偷窃类
		{
			
		}
		break;
	default:
		return;
	}
	int nSkillPosition = -1;
	SkillData.uGenre = CGOG_SKILL_FIGHT;
	nSkillPosition = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetSkillPosition(pSkill->m_nSkillID);
	SkillData.uId = pSkill->m_nSkillID;
	SkillData.nLevel = pSkill->m_nSkillLevel;
	if(nSkillPosition < 0) return;
	CoreDataChanged(GDCNI_SKILL_CHANGE, (unsigned int)&SkillData, nSkillPosition);
}

//-------------------------------------------------------------------------
//	功能：收到服务器发来的队友等级变化通知
//-------------------------------------------------------------------------
void KProtocolProcess::s2cGetTeammateLevel(BYTE* pMsg)
{
	PLAYER_TEAMMATE_LEVEL_SYNC	*pLevel = (PLAYER_TEAMMATE_LEVEL_SYNC*)pMsg;
	if (!Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag)
	{
		Player[CLIENT_PLAYER_INDEX].ApplySelfTeamInfo();
		return;
	}

	if ((DWORD)g_Team[0].m_nCaptain == pLevel->m_dwTeammateID)
	{
		g_Team[0].m_nMemLevel[0] = (DWORD)pLevel->m_btLevel;
		return;
	}

	for (int i = 0; i < MAX_TEAM_MEMBER; i++)
	{
		if ((DWORD)g_Team[0].m_nMember[i] == pLevel->m_dwTeammateID)
		{
			g_Team[0].m_nMemLevel[i + 1] = (DWORD)pLevel->m_btLevel;
			break;
		}
	}
}

//-------------------------------------------------------------------------
//	功能：收到服务器通知离开门派
//-------------------------------------------------------------------------
void KProtocolProcess::s2cLeaveFaction(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].m_cFaction.LeaveFaction();

	// 改变阵营
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetCamp(camp_free);

	CoreDataChanged(GDCNI_PLAYER_BASE_INFO, 0, 0);
}

//-------------------------------------------------------------------------
//	功能：收到服务器通知有成员离开(包括自己离开)
//-------------------------------------------------------------------------
void KProtocolProcess::s2cLeaveTeam(BYTE* pMsg)
{
	if (!Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag)
	{
		Player[CLIENT_PLAYER_INDEX].m_cTeam.UpdateInterface();
		return;
	}

	PLAYER_LEAVE_TEAM	*pLeaveTeam = (PLAYER_LEAVE_TEAM*)pMsg;

	// 自己离开
	if (pLeaveTeam->m_dwNpcID == Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID)
	{
		Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag = 0;
		Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nApplyCaptainID = 0;
		g_Team[0].Release();
		if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetMenuState() == PLAYER_MENU_STATE_TEAMOPEN)
			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetMenuState(PLAYER_MENU_STATE_NORMAL);
	}
	// 别人离开
	else
	{
		g_Team[0].DeleteMember(pLeaveTeam->m_dwNpcID);
	}

	Player[CLIENT_PLAYER_INDEX].m_cTeam.UpdateInterface();
	return;
}

//-------------------------------------------------------------------------
//	功能：收到服务器通知升级
//-------------------------------------------------------------------------
void KProtocolProcess::s2cLevelUp(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].s2cLevelUp(pMsg);
}

void KProtocolProcess::s2cMoveItem(BYTE* pMsg)
{
	PLAYER_MOVE_ITEM_SYNC	*pMove = (PLAYER_MOVE_ITEM_SYNC*)pMsg;

	ItemPos		DownPos, UpPos;
	DownPos.nPlace = pMove->m_btDownPos;
	DownPos.nX = pMove->m_btDownX;
	DownPos.nY = pMove->m_btDownY;
	UpPos.nPlace = pMove->m_btUpPos;
	UpPos.nX = pMove->m_btUpX;
	UpPos.nY = pMove->m_btUpY;

	Player[CLIENT_PLAYER_INDEX].m_ItemList.ExchangeItem(&DownPos, &UpPos);
}

void KProtocolProcess::s2cRemoveItem(BYTE* pMsg)
{
	ITEM_REMOVE_SYNC	*pRemove = (ITEM_REMOVE_SYNC*)pMsg;

	int		nIdx;
	nIdx = Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchID(pRemove->m_ID);
	if (nIdx > 0)
	{
		Player[CLIENT_PLAYER_INDEX].m_ItemList.Remove(nIdx);
	}

	
}

//-------------------------------------------------------------------------
//	功能：收到服务器发来的门派信息
//-------------------------------------------------------------------------
void KProtocolProcess::s2cSetFactionData(BYTE* pMsg)
{
	PLAYER_FACTION_DATA	*pData = (PLAYER_FACTION_DATA*)pMsg;

	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetCamp(pData->m_btCamp);
	Player[CLIENT_PLAYER_INDEX].m_cFaction.m_nCurFaction = (char)pData->m_btCurFaction;
	Player[CLIENT_PLAYER_INDEX].m_cFaction.m_nFirstAddFaction = (char)pData->m_btFirstFaction;
	Player[CLIENT_PLAYER_INDEX].m_cFaction.m_nAddTimes = pData->m_nAddTimes;

	CoreDataChanged(GDCNI_PLAYER_BASE_INFO, 0, 0);
}

void KProtocolProcess::s2cSetMissionData(BYTE* pMsg)
{
	PLAYER_MISSION_DATA	*pData = (PLAYER_MISSION_DATA*)pMsg;

	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nMissionGroup = pData->m_nMissionGroup;

	CoreDataChanged(GDCNI_PLAYER_BASE_INFO, 0, 0);
}

//-------------------------------------------------------------------------
//	功能：收到服务器通知队伍状态(队长申请开放、关闭队伍状态时才会收到此信息)
//-------------------------------------------------------------------------
void KProtocolProcess::s2cSetTeamState(BYTE* pMsg)
{
	// 客户端数据有误，申请更新数据
	PLAYER_TEAM_CHANGE_STATE	*pTeamState = (PLAYER_TEAM_CHANGE_STATE*)pMsg;
	switch (pTeamState->m_btState)
	{
		case Team_S_OpenClose:
		{
			if (!Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag || Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure != TEAM_CAPTAIN)
			{
				Player[CLIENT_PLAYER_INDEX].ApplySelfTeamInfo();
				return;
			}

			Player[CLIENT_PLAYER_INDEX].m_cTeam.ReleaseList();
			if (pTeamState->m_btFlag)
			{
				g_Team[0].SetTeamOpen();
			}
			else
			{
				g_Team[0].SetTeamClose();
			}
		}
		break;
		case Team_S_CreatTeamFlag:
		{
			if (pTeamState->m_btFlag)
			{
				Player[CLIENT_PLAYER_INDEX].m_cTeam.SetCreatTeamFlag(TRUE);
			}
			else
			{
				Player[CLIENT_PLAYER_INDEX].m_cTeam.SetCreatTeamFlag(FALSE);
			}
		}
		break;
		case Team_S_PKFollowCaptain:
		{
			if (!Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag || Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure == TEAM_CAPTAIN)
			{
				Player[CLIENT_PLAYER_INDEX].ApplySelfTeamInfo();
				return;
			}

			if (pTeamState->m_btFlag)
			{
				Player[CLIENT_PLAYER_INDEX].m_cTeam.SetPKFollowCaptain(TRUE);
			}
			else
			{
				Player[CLIENT_PLAYER_INDEX].m_cTeam.SetPKFollowCaptain(FALSE);
			}
		}
		break;
		case Team_S_ModePick:
		{
			if (!Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag)
			{
				Player[CLIENT_PLAYER_INDEX].ApplySelfTeamInfo();
				return;
			}

			g_Team[0].SetModePick(pTeamState->m_btFlag);
		}
		break;
	}
}

//-------------------------------------------------------------------------
//	功能：收到服务器通知队伍状态(队长申请开放、关闭队伍状态时才会收到此信息)
//-------------------------------------------------------------------------
void	KProtocolProcess::s2cApplyAddTeam(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].s2cApplyAddTeam(pMsg);
}

//-------------------------------------------------------------------------
//	功能：收到服务器发来的某个队伍的组队情况，显示之
//-------------------------------------------------------------------------
void KProtocolProcess::s2cShowTeamInfo(BYTE* pMsg)
{
}

void KProtocolProcess::s2cSyncItem(BYTE* pMsg)
{
	ITEM_SYNC	*pItemSync = (ITEM_SYNC*)pMsg;
	int nIndex = 0;
	if (pItemSync->m_bIsNew)
	{
		nIndex = ItemSet.Add(
			pItemSync->m_Nature,
			pItemSync->m_Genre,
			pItemSync->m_Series,
			pItemSync->m_Level,
			pItemSync->m_Luck,
			pItemSync->m_Detail,
			pItemSync->m_Particur,
			pItemSync->m_MagicLevel,
			pItemSync->m_Version,
			pItemSync->m_RandomSeed);
	}
	else
		nIndex = ItemSet.SearchID(pItemSync->m_ID);
	if (nIndex)
	{
		Item[nIndex].SetTemp(pItemSync->m_bTemp);
		Item[nIndex].SetOwner(pItemSync->m_dwOwner);
		Item[nIndex].SetTradePrice(pItemSync->m_TradePrice);
		Item[nIndex].SetID(pItemSync->m_ID);
		if(pItemSync->m_Nature >= NATURE_GOLD)
			Item[nIndex].SetRow(pItemSync->m_Detail);
		else
			Item[nIndex].SetDetailType(pItemSync->m_Detail);
		Item[nIndex].SetNature(pItemSync->m_Nature);
		Item[nIndex].SetGenre(pItemSync->m_Genre);
		Item[nIndex].SetParticular( pItemSync->m_Particur);
		Item[nIndex].SetLevel(pItemSync->m_Level);
		Item[nIndex].SetDurability(pItemSync->m_Durability);
		Item[nIndex].SetExpireTime(pItemSync->m_ExpireTime);
		Item[nIndex].SetLock(&pItemSync->m_LockItem);
		Item[nIndex].SetLockSell(pItemSync->m_bLockSell);
		Item[nIndex].SetLockTrade(pItemSync->m_bLockTrade);
		Item[nIndex].SetLockDrop(pItemSync->m_bLockDrop);
		Item[nIndex].SetParam(pItemSync->m_Param);
		Item[nIndex].SetMantle(pItemSync->m_Mantle);
		Item[nIndex].SetBackLocal(&pItemSync->m_BackLocal);
		Item[nIndex].SetFortune(pItemSync->m_Fortune);
		Item[nIndex].SetStackNum(pItemSync->m_StackNum);
		memcpy(Item[nIndex].m_GeneratorParam.nGeneratorLevel, pItemSync->m_MagicLevel, sizeof(int) * MAX_ITEM_MAGICATTRIB);
		Item[nIndex].m_GeneratorParam.uRandomSeed = pItemSync->m_RandomSeed;
		Item[nIndex].m_GeneratorParam.nVersion = pItemSync->m_Version;
		Item[nIndex].m_GeneratorParam.nLuck = pItemSync->m_Luck;

		if (pItemSync->m_bIsNew)
			Player[CLIENT_PLAYER_INDEX].m_ItemList.Add(nIndex, pItemSync->m_btPlace, pItemSync->m_btX, pItemSync->m_btY);
		else
			Player[CLIENT_PLAYER_INDEX].m_ItemList.Add(nIndex, 
			Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Items[nIndex].nPlace, 
			Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Items[nIndex].nX, 
			Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Items[nIndex].nY);
	}
}

void KProtocolProcess::s2cSyncMagic(BYTE* pMsg)
{
	ITEM_SYNC_MAGIC *pItemSync = (ITEM_SYNC_MAGIC*)pMsg;
	DWORD nIdx = ItemSet.SearchID(pItemSync->m_dwID);
	if ( nIdx > 0 && nIdx < MAX_ITEM )
	{
		memcpy(Item[nIdx].m_GeneratorParam.nGeneratorLevel, pItemSync->m_MagicLevel, sizeof(int)*MAX_ITEM_MAGICLEVEL);
		Item[nIdx].SetMagicAttrib((KItemNormalAttrib*)pItemSync->m_MagicAttrib);
	}
}
//-------------------------------------------------------------------------
//	功能：收到服务器发过来的同步money的消息
//-------------------------------------------------------------------------
void KProtocolProcess::s2cSyncMoney(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].s2cSyncMoney(pMsg);
}

void KProtocolProcess::s2cSyncRoleList(BYTE* pMsg)
{
	ROLE_LIST_SYNC	*pSync = (ROLE_LIST_SYNC *)pMsg;
	// TODO: 通过CORESHELL交给界面？
}

//-------------------------------------------------------------------------
//	功能：收到服务器通知有新人加入队伍
//-------------------------------------------------------------------------
void KProtocolProcess::s2cTeamAddMember(BYTE* pMsg)
{
	if (!Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag )
	{
		Player[CLIENT_PLAYER_INDEX].ApplySelfTeamInfo();
		return;
	}

	PLAYER_TEAM_ADD_MEMBER	*pAddMem = (PLAYER_TEAM_ADD_MEMBER*)pMsg;
	int		nMemNum;

	nMemNum = g_Team[0].FindFree();
	if (nMemNum < 0)
		return;
	g_Team[0].m_nMember[nMemNum] = pAddMem->m_dwNpcID;
	g_Team[0].m_nMemLevel[nMemNum + 1] = (DWORD)pAddMem->m_btLevel;
	strcpy(g_Team[0].m_szMemName[nMemNum + 1], pAddMem->m_szName);
	g_Team[0].m_nMemNum++;
	Player[CLIENT_PLAYER_INDEX].m_cTeam.DeleteOneFromApplyList(pAddMem->m_dwNpcID);
	Player[CLIENT_PLAYER_INDEX].m_cTeam.UpdateInterface();

	KSystemMessage	sMsg;
	sprintf(sMsg.szMessage, MSG_TEAM_ADD_MEMBER, pAddMem->m_szName);
	sMsg.eType = SMT_NORMAL;
	sMsg.byConfirmType = SMCT_NONE;
	sMsg.byPriority = 0;
	sMsg.byParamSize = 0;
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
}

//-------------------------------------------------------------------------
//	功能：收到服务器通知更换队长
//-------------------------------------------------------------------------
void KProtocolProcess::s2cTeamChangeCaptain(BYTE* pMsg)
{
	if (!Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag)
	{
		Player[CLIENT_PLAYER_INDEX].ApplySelfTeamInfo();
		return;
	}

	int		nLevel, nMemNo;
	char	szName[32];
	PLAYER_TEAM_CHANGE_CAPTAIN	*pChange = (PLAYER_TEAM_CHANGE_CAPTAIN*)pMsg;

	nMemNo = g_Team[0].FindMemberID(pChange->m_dwCaptainID);
	// 当前队伍数据有误
	if ((DWORD)g_Team[0].m_nCaptain != pChange->m_dwMemberID || nMemNo < 0)
	{
		Player[CLIENT_PLAYER_INDEX].ApplySelfTeamInfo();
		return;
	}

	// 自己被任命为队长
	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID == pChange->m_dwCaptainID)
	{
		KSystemMessage	sMsg;
		if (pChange->m_bMySelf)
			sprintf(sMsg.szMessage, MSG_TEAM_CHANGE_CAPTAIN_SELF, g_Team[0].m_szMemName[0]);
		else
			sprintf(sMsg.szMessage, MSG_TEAM_AUTO_CAPTAIN_SELF, g_Team[0].m_szMemName[0]);
		sMsg.eType = SMT_TEAM;
		sMsg.byConfirmType = SMCT_UI_TEAM;
		sMsg.byPriority = 3;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
	}
	else	// 别人被任命为队长
	{
		KSystemMessage	sMsg;
		if (pChange->m_bMySelf)
			sprintf(sMsg.szMessage, MSG_TEAM_CHANGE_CAPTAIN, g_Team[0].m_szMemName[nMemNo + 1]);
		else
			sprintf(sMsg.szMessage, MSG_TEAM_AUTO_CAPTAIN, g_Team[0].m_szMemName[nMemNo + 1]);
		sMsg.eType = SMT_TEAM;
		sMsg.byConfirmType = SMCT_UI_TEAM;
		sMsg.byPriority = 3;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
	}

	// 更换人员
	nLevel = g_Team[0].m_nMemLevel[0];
	strcpy(szName, g_Team[0].m_szMemName[0]);
	g_Team[0].m_nCaptain = pChange->m_dwCaptainID;
	g_Team[0].m_nMemLevel[0] = g_Team[0].m_nMemLevel[nMemNo + 1];
	strcpy(g_Team[0].m_szMemName[0], g_Team[0].m_szMemName[nMemNo + 1]);
	g_Team[0].m_nMember[nMemNo] = pChange->m_dwMemberID;
	g_Team[0].m_nMemLevel[nMemNo + 1] = nLevel;
	strcpy(g_Team[0].m_szMemName[nMemNo + 1], szName);

	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID == pChange->m_dwCaptainID)
	{
		Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure = TEAM_CAPTAIN;
		Player[CLIENT_PLAYER_INDEX].m_cTeam.ReleaseList();
	}
	else if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID == pChange->m_dwMemberID)
	{
		Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure = TEAM_MEMBER;
		Player[CLIENT_PLAYER_INDEX].m_cTeam.ReleaseList();
	}

	Player[CLIENT_PLAYER_INDEX].m_cTeam.UpdateInterface();
}

//-------------------------------------------------------------------------
//	功能：收到服务器发来的自己队伍的组队情况，更新相应信息
//-------------------------------------------------------------------------
void KProtocolProcess::s2cUpdataSelfTeamInfo(BYTE* pMsg)
{
	int		i;
	PLAYER_SEND_SELF_TEAM_INFO	*pSelfInfo = (PLAYER_SEND_SELF_TEAM_INFO*)pMsg;

	// 数据包出错
	if (pSelfInfo->m_dwNpcID[0] == 0)
		return;

	Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag = 1;
	Player[CLIENT_PLAYER_INDEX].m_nLeadExp = pSelfInfo->m_nLeadExp;
	Player[CLIENT_PLAYER_INDEX].m_nLeadLevel = PlayerSet.m_cLeadExp.GetLevel(Player[CLIENT_PLAYER_INDEX].m_nLeadExp);
	Player[CLIENT_PLAYER_INDEX].m_nNextLevelLeadExp = PlayerSet.m_cLeadExp.GetLevelExp(Player[CLIENT_PLAYER_INDEX].m_nLeadLevel);
	if (Player[CLIENT_PLAYER_INDEX].m_nNextLevelLeadExp == 0)
		Player[CLIENT_PLAYER_INDEX].m_nNextLevelLeadExp = Player[CLIENT_PLAYER_INDEX].m_nLeadLevel;

	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID == pSelfInfo->m_dwNpcID[0])	// 队长
	{
		Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure = TEAM_CAPTAIN;
	}
	else													// 队员
	{
		Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure = TEAM_MEMBER;
		for (i = 0; i < MAX_TEAM_APPLY_LIST; i++)
			Player[CLIENT_PLAYER_INDEX].m_cTeam.m_sApplyList[i].Release();
	}
	g_Team[0].m_nCaptain = pSelfInfo->m_dwNpcID[0];
	if (pSelfInfo->m_btState == 0)
		g_Team[0].SetTeamClose();
	else
		g_Team[0].SetTeamOpen();
	g_Team[0].m_nTeamServerID = pSelfInfo->nTeamServerID;
	g_Team[0].m_nMemLevel[0] = (DWORD)pSelfInfo->m_btLevel[0];
	strcpy(g_Team[0].m_szMemName[0], pSelfInfo->m_szNpcName[0]);
	g_Team[0].m_nMemNum = 0;
	for (i = 0; i < MAX_TEAM_MEMBER; i++)
	{
		if (pSelfInfo->m_dwNpcID[i + 1] > 0)
		{
			g_Team[0].m_nMember[i] = pSelfInfo->m_dwNpcID[i + 1];
			g_Team[0].m_nMemLevel[i + 1] = (DWORD)pSelfInfo->m_btLevel[i + 1];
			strcpy(g_Team[0].m_szMemName[i + 1], pSelfInfo->m_szNpcName[i + 1]);
			g_Team[0].m_nMemNum++;
		}
		else
		{
			g_Team[0].m_nMember[i] = -1;
			g_Team[0].m_nMemLevel[i + 1] = 0;
			g_Team[0].m_szMemName[i + 1][0] = 0;
		}
	}
	Player[CLIENT_PLAYER_INDEX].m_cTeam.UpdateInterface();
}

void KProtocolProcess::SyncCurNormalData(BYTE* pMsg)
{
	CURPLAYER_NORMAL_SYNC	*pSync = (CURPLAYER_NORMAL_SYNC*)pMsg;
	if (pSync->m_shLife > 0)
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLife = pSync->m_shLife;
	else
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLife = 0;
	if (pSync->m_shStamina > 0)
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentStamina = pSync->m_shStamina;
	else
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentStamina = 0;

	if (pSync->m_shMana > 0)
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentMana = pSync->m_shMana;
	else
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentMana = 0;

	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	switch (pSync->m_btTeamData)
	{
	case 0x00:
		if ( Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag )
		{
			Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag = 0;
			Player[CLIENT_PLAYER_INDEX].ApplySelfTeamInfo();
		}
		break;
	case 0x03:
		if ( !Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag || Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure != TEAM_CAPTAIN)
		{
			Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag = 1;
			Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure = TEAM_CAPTAIN;
			Player[CLIENT_PLAYER_INDEX].ApplySelfTeamInfo();
		}
		break;
	case 0x01:
		if ( !Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag || Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure != TEAM_MEMBER)
		{
			Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag = 1;
			Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure = TEAM_MEMBER;
			Player[CLIENT_PLAYER_INDEX].ApplySelfTeamInfo();
		}
		break;
	}
}

void KProtocolProcess::SyncCurPlayer(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].SyncCurPlayer(pMsg);
}

void KProtocolProcess::SyncNpc(BYTE* pMsg)
{
	NPC_SYNC* NpcSync = (NPC_SYNC *)pMsg;
	
	int nRegion, nMapX, nMapY, nOffX, nOffY;
	SubWorld[0].Mps2Map(NpcSync->MapX, NpcSync->MapY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);

	if (nRegion == -1)
		return;

	int nIdx = NpcSet.SearchID(NpcSync->ID);
	if (!nIdx)
	{
		nIdx = NpcSet.Add(NpcSync->NpcSettingIdx, 0, NpcSync->MapX, NpcSync->MapY);
		Npc[nIdx].m_dwID = NpcSync->ID;
		Npc[nIdx].m_Kind = NpcSync->m_btKind;
		Npc[nIdx].m_Height = 0;
		
		if (NpcSet.IsNpcRequestExist(NpcSync->ID))
			NpcSet.RemoveNpcRequest(NpcSync->ID);
	}
	else
	{
		if (Npc[nIdx].m_RegionIndex >= 0)
			SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].DecRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, obj_npc);

		Npc[nIdx].m_Dir = NpcSync->Dir;
		Npc[nIdx].m_MapX = nMapX;
		Npc[nIdx].m_MapY = nMapY;
		Npc[nIdx].m_OffX = nOffX;
		Npc[nIdx].m_OffY = nOffY;

		Npc[nIdx].m_NpcSettingIdx = (short)HIWORD(NpcSync->NpcSettingIdx);
		Npc[nIdx].m_Level = LOWORD(NpcSync->NpcSettingIdx);

		if (Npc[nIdx].m_RegionIndex >= 0)
		{
			if (Npc[nIdx].m_RegionIndex != nRegion)
			{
				SubWorld[0].NpcChangeRegion(SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].m_RegionID, SubWorld[0].m_Region[nRegion].m_RegionID, nIdx);
				Npc[nIdx].m_RegionIndex = nRegion;
			}
			SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].AddRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, obj_npc);
		}
		else
		{
			SubWorld[0].m_Region[nRegion].AddNpc(nIdx);
			Npc[nIdx].m_RegionIndex = nRegion;
			SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].AddRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, obj_npc);
		}
	}
	Npc[nIdx].m_Camp			= (NPCCAMP)NpcSync->Camp;
	Npc[nIdx].m_CurrentCamp		= (NPCCAMP)NpcSync->CurrentCamp;
	Npc[nIdx].m_Series			= NpcSync->m_bySeries;
	Npc[nIdx].m_btSpecial		= NpcSync->Special;
	Npc[nIdx].m_nMissionGroup	= NpcSync->MissionGroup;

	Npc[nIdx].m_CurrentLifeMax	= NpcSync->CurrentLifeMax;
	Npc[nIdx].m_CurrentLife		= NpcSync->CurrentLife;
	
	if (Npc[nIdx].m_Doing != do_death || Npc[nIdx].m_Doing != do_revive)
		Npc[nIdx].SendCommand((NPCCMD)NpcSync->m_Doing, NpcSync->MapX, NpcSync->MapY);

	Npc[nIdx].SetMenuState(NpcSync->m_btMenuState);
	memset(Npc[nIdx].Name, 0, sizeof(Npc[nIdx].Name));
	memcpy(Npc[nIdx].Name, NpcSync->m_szName, NpcSync->m_wLength - (sizeof(NPC_SYNC) - 1 - sizeof(NpcSync->m_szName)));

	Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
}

void KProtocolProcess::SyncNpcMin(BYTE* pMsg)
{
	NPC_NORMAL_SYNC* NpcSync = (NPC_NORMAL_SYNC *)pMsg;

	if (NpcSync->Doing == do_revive || NpcSync->Doing == do_death)
		return;
	
	int nIdx = NpcSet.SearchID(NpcSync->ID);
	if (!nIdx)
	{
		if (!NpcSet.IsNpcRequestExist(NpcSync->ID))
		{
			SendClientCmdRequestNpc(NpcSync->ID);
			NpcSet.InsertNpcRequest(NpcSync->ID);
		}
	}
	else
	{
		int nRegion, nMapX, nMapY, nOffX, nOffY;
		SubWorld[0].Mps2Map(NpcSync->MapX, NpcSync->MapY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);

		if (Npc[nIdx].m_RegionIndex == -1 && nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex)	
		{
			if (nRegion == -1)
			{		
				return;
			}
			else
			{
				Npc[nIdx].m_MapX = nMapX;
				Npc[nIdx].m_MapY = nMapY;
				Npc[nIdx].m_OffX = nOffX;
				Npc[nIdx].m_OffY = nOffY;
				Npc[nIdx].m_RegionIndex = nRegion;
				Npc[nIdx].m_dwRegionID = SubWorld[0].m_Region[nRegion].m_RegionID;
				SubWorld[0].m_Region[nRegion].AddNpc(nIdx);
				SubWorld[0].m_Region[nRegion].AddRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, obj_npc);
			}
			
			if (NpcSync->Doing == do_stand)
				Npc[nIdx].ProcNetCommand(do_stand);
		}
		else
		{
			if (Npc[nIdx].m_RegionIndex != nRegion && nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex)
			{
				SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].RemoveNpc(nIdx);
				SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].DecRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, obj_npc);
				Npc[nIdx].m_MapX = nMapX;
				Npc[nIdx].m_MapY = nMapY;
				Npc[nIdx].m_OffX = nOffX;
				Npc[nIdx].m_OffY = nOffY;
				Npc[nIdx].m_RegionIndex = nRegion;
				if (nRegion >= 0)
				{
					SubWorld[0].m_Region[nRegion].AddNpc(nIdx);
					Npc[nIdx].m_dwRegionID = SubWorld[0].m_Region[nRegion].m_RegionID;
					SubWorld[0].m_Region[nRegion].AddRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, obj_npc);
				}
			}
		}

		if (nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex)
		{
			int	nOldLife = Npc[nIdx].m_CurrentLife;

			Npc[nIdx].m_CurrentLifeMax	= NpcSync->CurrentLifeMax;
			Npc[nIdx].m_CurrentLife		= NpcSync->CurrentLife;
			
			if (Npc[nIdx].m_Kind == kind_normal)
				Npc[nIdx].AddBlood(nOldLife - Npc[nIdx].m_CurrentLife);
			Npc[nIdx].SetCurrentCamp(NpcSync->Camp);
		}

		Npc[nIdx].m_CurrentRunSpeed		= NpcSync->RunSpeed;
		Npc[nIdx].m_CurrentWalkSpeed	= NpcSync->WalkSpeed;
		Npc[nIdx].m_CurrentAttackSpeed	= NpcSync->AttackSpeed;
		Npc[nIdx].m_CurrentCastSpeed	= NpcSync->CastSpeed;
		Npc[nIdx].SetNpcState(NpcSync->StateInfo);

		Npc[nIdx].m_FreezeState.nTime	= NpcSync->State & STATE_FREEZE;
		Npc[nIdx].m_PoisonState.nTime	= NpcSync->State & STATE_POISON;
		Npc[nIdx].m_StunState.nTime		= NpcSync->State & STATE_STUN;
		Npc[nIdx].m_HideState.nTime		= NpcSync->State & STATE_HIDE;
		Npc[nIdx].m_FrozenAction.nTime	= NpcSync->State & STATE_FROZEN;
		Npc[nIdx].m_WalkRun.nTime		= NpcSync->State & STATE_WALKRUN;
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}
}

//-------------------------------------------------------------------------
//	功能：收到服务器消息同步本玩家npc数据
//-------------------------------------------------------------------------
void KProtocolProcess::SyncNpcMinPlayer(BYTE* pMsg)
{	
	NPC_PLAYER_TYPE_NORMAL_SYNC	*pSync = (NPC_PLAYER_TYPE_NORMAL_SYNC*)pMsg;

	_ASSERT(pSync->ID == Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID);
	if (pSync->ID != Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID)
		return;

	int nRegion, nMapX, nMapY, nOffX, nOffY, nNpcIdx;
	SubWorld[0].Mps2Map(pSync->MapX, pSync->MapY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);

	nNpcIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;

	if (Npc[nNpcIdx].m_RegionIndex == -1)
	{
		if (nRegion < 0)
		{
			return;
		}
		Npc[nNpcIdx].m_RegionIndex = nRegion;
		Npc[nNpcIdx].m_dwRegionID = SubWorld[0].m_Region[nRegion].m_RegionID;
		SubWorld[0].NpcChangeRegion(-1, SubWorld[0].m_Region[nRegion].m_RegionID, nNpcIdx);
		SubWorld[0].m_Region[nRegion].AddRef(nMapX, nMapY, obj_npc);

		Npc[nNpcIdx].m_MapX = nMapX;
		Npc[nNpcIdx].m_MapY = nMapY;
		Npc[nNpcIdx].m_OffX = pSync->OffX;
		Npc[nNpcIdx].m_OffY = pSync->OffY;
		Npc[nNpcIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
		return;
	}

	if (nRegion == -1)
	{
		SubWorld[0].m_Region[Npc[nNpcIdx].m_RegionIndex].DecRef(Npc[nNpcIdx].m_MapX, Npc[nNpcIdx].m_MapY, obj_npc);

		int nRegionX = pSync->MapX / (SubWorld[0].m_nCellWidth * SubWorld[0].m_nRegionWidth);
		int nRegionY = pSync->MapY / (SubWorld[0].m_nCellHeight * SubWorld[0].m_nRegionHeight);
		
		DWORD	dwRegionID = MAKELONG(nRegionX, nRegionY);
		SubWorld[0].LoadMap(SubWorld[0].m_SubWorldID, dwRegionID);

		nRegion = SubWorld[0].FindRegion(dwRegionID);
		_ASSERT(nRegion >= 0);
		Npc[nNpcIdx].m_RegionIndex = nRegion;
		Npc[nNpcIdx].m_dwRegionID = dwRegionID;
		SubWorld[0].NpcChangeRegion(-1, SubWorld[0].m_Region[nRegion].m_RegionID, nNpcIdx);

		SubWorld[0].Mps2Map(pSync->MapX, pSync->MapY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
		Npc[nNpcIdx].m_MapX = nMapX;
		Npc[nNpcIdx].m_MapY = nMapY;
		Npc[nNpcIdx].m_OffX = pSync->OffX;
		Npc[nNpcIdx].m_OffY = pSync->OffY;
		
		SubWorld[0].m_Region[Npc[nNpcIdx].m_RegionIndex].AddRef(Npc[nNpcIdx].m_MapX, Npc[nNpcIdx].m_MapY, obj_npc);
		Npc[nNpcIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;

		return;
	}

	/*if (Npc[nNpcIdx].m_RegionIndex != nRegion)
	{
		SubWorld[0].m_Region[Npc[nNpcIdx].m_RegionIndex].RemoveNpc(nNpcIdx);
		SubWorld[0].m_Region[Npc[nNpcIdx].m_RegionIndex].DecRef(Npc[nNpcIdx].m_MapX, Npc[nNpcIdx].m_MapY, obj_npc);
		Npc[nNpcIdx].m_MapX = nMapX;
		Npc[nNpcIdx].m_MapY = nMapY;
		Npc[nNpcIdx].m_OffX = nOffX;
		Npc[nNpcIdx].m_OffY = nOffY;
		Npc[nNpcIdx].m_RegionIndex = nRegion;
		if (nRegion >= 0)
		{
			SubWorld[0].m_Region[nRegion].AddNpc(nNpcIdx);
			Npc[nNpcIdx].m_dwRegionID = SubWorld[0].m_Region[nRegion].m_RegionID;
			SubWorld[0].m_Region[nRegion].AddRef(Npc[nNpcIdx].m_MapX, Npc[nNpcIdx].m_MapY, obj_npc);
		}
	}*/

	BYTE	byBarrier = SubWorld[0].m_Region[Npc[nNpcIdx].m_RegionIndex].GetBarrier(Npc[nNpcIdx].m_MapX, Npc[nNpcIdx].m_MapY, Npc[nNpcIdx].m_OffX, Npc[nNpcIdx].m_OffY);
	if (0 != byBarrier && Obstacle_JumpFly != byBarrier)
	{
		g_DebugLog("[Barrier]Player in Barrier");
	}
}

//-------------------------------------------------------------------------
//	功能：收到服务器消息添加一个obj
//-------------------------------------------------------------------------
void KProtocolProcess::SyncObjectAdd(BYTE* pMsg)
{
	OBJ_ADD_SYNC	*pObjSyncAdd = (OBJ_ADD_SYNC*)pMsg;
	int				nObjIndex;
	KObjItemInfo	sInfo;

	nObjIndex = ObjSet.FindID(pObjSyncAdd->m_nID);
	if (nObjIndex > 0)
		return;

	sInfo.m_nItemID = pObjSyncAdd->m_nItemID;
	sInfo.m_nItemWidth = pObjSyncAdd->m_btItemWidth;
	sInfo.m_nItemHeight = pObjSyncAdd->m_btItemHeight;
	sInfo.m_nMoneyNum = pObjSyncAdd->m_nMoneyNum;
	sInfo.m_nColorID = pObjSyncAdd->m_btColorID;
	sInfo.m_nGenre = pObjSyncAdd->m_nGenre;
	sInfo.m_nDetailType = pObjSyncAdd->m_nDetailType;
	sInfo.m_nMovieFlag = ((pObjSyncAdd->m_btFlag & 0x02) > 0 ? 1 : 0);
	sInfo.m_nSoundFlag = ((pObjSyncAdd->m_btFlag & 0x01) > 0 ? 1 : 0);
	sInfo.m_bOverLook = ((pObjSyncAdd->m_btFlag & 0x04) > 0 ? 1 : 0);
	memset(sInfo.m_szName, 0, sizeof(sInfo.m_szName));
	memcpy(sInfo.m_szName, pObjSyncAdd->m_szName, pObjSyncAdd->m_wLength + 1 + sizeof(pObjSyncAdd->m_szName) - sizeof(OBJ_ADD_SYNC));

	nObjIndex = ObjSet.ClientAdd(
		pObjSyncAdd->m_nID,
		pObjSyncAdd->m_nDataID,
		pObjSyncAdd->m_btState,
		pObjSyncAdd->m_btDir,
		pObjSyncAdd->m_wCurFrame,
		pObjSyncAdd->m_nXpos,
		pObjSyncAdd->m_nYpos,
		sInfo);
}

//-------------------------------------------------------------------------
//	功能：收到服务器消息更新某个obj方向
//-------------------------------------------------------------------------
void KProtocolProcess::SyncObjectDir(BYTE* pMsg)
{
	OBJ_SYNC_DIR	*pObjSyncDir = (OBJ_SYNC_DIR*)pMsg;
	int				nObjIndex;
	nObjIndex = ObjSet.FindID(pObjSyncDir->m_nID);
	if (nObjIndex <= 0)
	{
		// 向服务器发添加请求
		OBJ_CLIENT_SYNC_ADD	sObjClientSyncAdd;
		sObjClientSyncAdd.ProtocolType = c2s_requestobj;
		sObjClientSyncAdd.m_nID = pObjSyncDir->m_nID;
		if (g_pClient)
			g_pClient->SendPackToServer(&sObjClientSyncAdd, sizeof(sObjClientSyncAdd));
	}
	else
	{	// 同步方向
		Object[nObjIndex].SetDir(pObjSyncDir->m_btDir);
	}
}

//-------------------------------------------------------------------------
//	功能：收到服务器消息删除某个obj
//-------------------------------------------------------------------------
void KProtocolProcess::SyncObjectRemove(BYTE* pMsg)
{
	OBJ_SYNC_REMOVE	*pObjSyncRemove = (OBJ_SYNC_REMOVE*)pMsg;
	int				nObjIndex;
	nObjIndex = ObjSet.FindID(pObjSyncRemove->m_nID);
	if (nObjIndex > 0)
	{	// 删除
		Object[nObjIndex].Remove(pObjSyncRemove->m_btSoundFlag);
	}
}

//-------------------------------------------------------------------------
//	功能：收到服务器消息更新某个obj状态
//-------------------------------------------------------------------------
void KProtocolProcess::SyncObjectState(BYTE* pMsg)
{
	OBJ_SYNC_STATE	*pObjSyncState = (OBJ_SYNC_STATE*)pMsg;
	int				nObjIndex;
	nObjIndex = ObjSet.FindID(pObjSyncState->m_nID);
	if (nObjIndex <= 0)
	{
		// 向服务器发添加请求
		OBJ_CLIENT_SYNC_ADD	sObjClientSyncAdd;
		sObjClientSyncAdd.ProtocolType = c2s_requestobj;
		sObjClientSyncAdd.m_nID = pObjSyncState->m_nID;
		if (g_pClient)
			g_pClient->SendPackToServer(&sObjClientSyncAdd, sizeof(sObjClientSyncAdd));
	}
	else
	{	// 同步状态
		if (Object[nObjIndex].m_nRegionIdx == -1)
		{
			int	nRegion;
			nRegion = SubWorld[0].FindRegion(Object[nObjIndex].m_nBelongRegion);
			if (nRegion >= 0)
			{
				Object[nObjIndex].m_nRegionIdx = nRegion;
				SubWorld[0].m_Region[nRegion].AddObj(nObjIndex);
			}
		}
		Object[nObjIndex].SetState(pObjSyncState->m_btState);
	}
}

void KProtocolProcess::SyncPlayer(BYTE* pMsg)
{
	PLAYER_SYNC*	pPlaySync = (PLAYER_SYNC *)pMsg;

	int nIdx = NpcSet.SearchID(pPlaySync->ID);
	Npc[nIdx].m_nTeamServerID		= pPlaySync->TeamServerID;
	Npc[nIdx].m_HelmType			= pPlaySync->HelmType;
	Npc[nIdx].m_ArmorType			= pPlaySync->ArmorType;
	Npc[nIdx].m_HorseType			= (char)pPlaySync->HorseType;
	Npc[nIdx].m_bRideHorse			= Npc[nIdx].m_HorseType >= 0;
	Npc[nIdx].m_WeaponType			= pPlaySync->WeaponType;
	Npc[nIdx].m_Kind				= kind_player;
	Npc[nIdx].m_RankID				= pPlaySync->RankID;
	memcpy(&Npc[nIdx].m_CurExpandRank, &pPlaySync->ExpandRank, sizeof(pPlaySync->ExpandRank));
	Npc[nIdx].m_PTrade				= pPlaySync->Trade;
	Npc[nIdx].m_byTranslife			= pPlaySync->Translife;
	Npc[nIdx].m_byViprank			= pPlaySync->Viprank;
	Npc[nIdx].m_nRepute				= pPlaySync->Repute;
	Npc[nIdx].m_nFuYuan				= pPlaySync->FuYuan;
	Npc[nIdx].m_nPKFlag				= pPlaySync->PKFlag;
	Npc[nIdx].m_nPKValue			= pPlaySync->PKValue;
	Npc[nIdx].m_byMantleLevel		= pPlaySync->MantleLevel;
	Npc[nIdx].m_ImagePlayer			= pPlaySync->ImagePlayer;
	if(Option.GetLow(LowPlayer) && nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex)
	{
		if(Npc[nIdx].m_NpcSettingIdx == PLAYER_MALE_NPCTEMPLATEID)
			Npc[nIdx].m_MaskType = ManTypeNameIdx;
		else
			Npc[nIdx].m_MaskType = WomanTypeNameIdx;
	}
	else
		Npc[nIdx].m_MaskType   		= pPlaySync->MaskType;
	if (nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex)
	{
		Player[CLIENT_PLAYER_INDEX].m_ImagePlayer		= pPlaySync->ImagePlayer;
	}
	Npc[nIdx].m_MantleType			= pPlaySync->MantleType;
	Npc[nIdx].m_nRankInWorld		= pPlaySync->RankInWorld;
	Npc[nIdx].m_nFactionNumber		= pPlaySync->FactionNumber;
	Npc[nIdx].m_nPacePercent		= pPlaySync->PacePer;
	Npc[nIdx].m_nFigure				= pPlaySync->m_Figure;
	if (pPlaySync->TongNameID)
	{
		Npc[nIdx].m_bTongFlag		= TRUE;
		Npc[nIdx].m_dwTongNameID	= pPlaySync->TongNameID;
		Npc[nIdx].m_nTongNationalEmblem = pPlaySync->TongNationalEmblem;
		strcpy(Npc[nIdx].m_szTongName, pPlaySync->TongName);
		strcpy(Npc[nIdx].m_szTongAgname, pPlaySync->TongAgname);
	}
	else
	{
		Npc[nIdx].m_bTongFlag		= FALSE;
		Npc[nIdx].m_dwTongNameID	= 0;
		Npc[nIdx].m_nTongNationalEmblem = 0;
		Npc[nIdx].m_szTongName[0]	= 0;
		Npc[nIdx].m_szTongAgname[0] = 0;
	}
	strcpy(Npc[nIdx].MateName, pPlaySync->MateName);
	if (pPlaySync->m_btSomeFlag & 0x01)
		Npc[nIdx].m_FightMode		= enumFightActive;
	else
		Npc[nIdx].m_FightMode		= enumFightNone;
	if (pPlaySync->m_btSomeFlag & 0x02)
		Npc[nIdx].SetSleepMode(1);
	else
		Npc[nIdx].SetSleepMode(0);
	if (pPlaySync->m_btSomeFlag & 0x04)
		Npc[nIdx].m_bTongFlag		= TRUE;
	else
		Npc[nIdx].m_bTongFlag		= FALSE;
}

void KProtocolProcess::SyncPlayerMin(BYTE* pMsg)
{
	PLAYER_NORMAL_SYNC* pPlaySync = (PLAYER_NORMAL_SYNC *)pMsg;

	int nIdx = NpcSet.SearchID(pPlaySync->ID);
	Npc[nIdx].m_nTeamServerID		= pPlaySync->TeamServerID;
	Npc[nIdx].m_HelmType			= pPlaySync->HelmType;
	Npc[nIdx].m_ArmorType			= pPlaySync->ArmorType;
	Npc[nIdx].m_HorseType			= (char)pPlaySync->HorseType;
	Npc[nIdx].m_bRideHorse			= Npc[nIdx].m_HorseType >= 0;
	Npc[nIdx].m_WeaponType			= pPlaySync->WeaponType;
	Npc[nIdx].m_Kind				= kind_player;
	Npc[nIdx].m_RankID				= pPlaySync->RankID;
	memcpy(&Npc[nIdx].m_CurExpandRank, &pPlaySync->ExpandRank, sizeof(pPlaySync->ExpandRank));
	Npc[nIdx].m_PTrade				= pPlaySync->Trade;
	Npc[nIdx].m_byTranslife			= pPlaySync->Translife;
	Npc[nIdx].m_byViprank			= pPlaySync->Viprank;
	Npc[nIdx].m_nRepute				= pPlaySync->Repute;
	Npc[nIdx].m_nFuYuan				= pPlaySync->FuYuan;
	Npc[nIdx].m_nPKFlag				= pPlaySync->PKFlag;
	Npc[nIdx].m_nPKValue			= pPlaySync->PKValue;
	Npc[nIdx].m_byMantleLevel		= pPlaySync->MantleLevel;
	if(Option.GetLow(LowPlayer) && nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex)
	{
		if(Npc[nIdx].m_NpcSettingIdx == PLAYER_MALE_NPCTEMPLATEID)
			Npc[nIdx].m_MaskType = ManTypeNameIdx;
		else
			Npc[nIdx].m_MaskType = WomanTypeNameIdx;
	}
	else
		Npc[nIdx].m_MaskType   			= pPlaySync->MaskType;
	Npc[nIdx].m_MantleType			= pPlaySync->MantleType;
	Npc[nIdx].m_nRankInWorld		= pPlaySync->RankInWorld;
	Npc[nIdx].m_nFactionNumber		= pPlaySync->FactionNumber;
	Npc[nIdx].m_nPacePercent		= pPlaySync->PacePer;
	Npc[nIdx].m_nFigure				= pPlaySync->m_Figure;
	if (pPlaySync->TongNameID)
	{
		Npc[nIdx].m_bTongFlag		= TRUE;
		Npc[nIdx].m_dwTongNameID	= pPlaySync->TongNameID;
		Npc[nIdx].m_nTongNationalEmblem = pPlaySync->TongNationalEmblem;
		strcpy(Npc[nIdx].m_szTongName, pPlaySync->TongName);
		strcpy(Npc[nIdx].m_szTongAgname, pPlaySync->TongAgname);
	}
	else
	{
		Npc[nIdx].m_bTongFlag		= FALSE;
		Npc[nIdx].m_dwTongNameID	= 0;
		Npc[nIdx].m_nTongNationalEmblem = 0;
		Npc[nIdx].m_szTongName[0]	= 0;
		Npc[nIdx].m_szTongAgname[0] = 0;
	}
	strcpy(Npc[nIdx].MateName, pPlaySync->MateName);
	if (pPlaySync->m_btSomeFlag & 0x01)
		Npc[nIdx].m_FightMode		= enumFightActive;
	else
		Npc[nIdx].m_FightMode		= enumFightNone;
	if (pPlaySync->m_btSomeFlag & 0x02)
		Npc[nIdx].SetSleepMode(1);
	else
		Npc[nIdx].SetSleepMode(0);
	if (nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex)
	{
		Player[CLIENT_PLAYER_INDEX].m_ImagePlayer		= pPlaySync->ImagePlayer;
	}		
}

void KProtocolProcess::SyncScriptAction(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].OnScriptAction((PLAYER_SCRIPTACTION_SYNC *)pMsg);
}

void KProtocolProcess::SyncWorld(BYTE* pMsg)
{
	WORLD_SYNC *WorldSync = (WORLD_SYNC *)pMsg;

	if (Player[CLIENT_PLAYER_INDEX].m_nIndex > 0)
	{
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendCommand(do_stand);
	}
	if (SubWorld[0].m_SubWorldID != WorldSync->SubWorld)
	{
		SubWorld[0].LoadMap(WorldSync->SubWorld, WorldSync->Region);
/*		SubWorld[0].m_Region[0].Load(WorldSync->SubWorld, WorldSync->Region);
		SubWorld[0].m_Region[0].Init(SubWorld[0].m_nRegionWidth, SubWorld[0].m_nRegionHeight);
		for (int i = 0; i < 8; i++)
		{
			if (SubWorld[0].m_Region[0].m_nConRegionID[i] == -1)
				continue;
			SubWorld[0].m_Region[i+1].Load(WorldSync->SubWorld, SubWorld[0].m_Region[0].m_nConRegionID[i]);
			SubWorld[0].m_Region[i+1].Init(SubWorld[0].m_nRegionWidth, SubWorld[0].m_nRegionHeight);
		}*/
	}
	
	if (SubWorld[0].m_dwCurrentTime > WorldSync->Frame)
	{
		// 客户端快了，减速处理，或者是Lag引起的旧消息，需要处理
	}
	else if (SubWorld[0].m_dwCurrentTime < WorldSync->Frame)
	{
		// 客户端慢了，加速处理
	}

	SubWorld[0].m_dwCurrentTime = WorldSync->Frame;


	// 同步天气
	SubWorld[0].m_nWeather = WorldSync->Weather;
	g_ScenePlace.ChangeWeather(WorldSync->Weather);
}

void	KProtocolProcess::s2cSyncAllSkill(BYTE * pMsg)
{
	SKILL_SEND_ALL_SYNC	* pSync = (SKILL_SEND_ALL_SYNC*) pMsg;
	int nSkillCount = (pSync->m_wProtocolLong - 2) / sizeof(SKILL_SEND_ALL_SYNC_DATA);
	int nNpcIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	KSkill * pOrdinSkill = NULL;
	Npc[nNpcIndex].m_SkillList.Clear();
	for (int i = 0; i < nSkillCount; i ++)
	{
		if (pSync->m_sAllSkill[i].SkillId)
		{
			
//			pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(pSync->m_sAllSkill[i].SkillId, pSync->m_sAllSkill[i].SkillLevel);
//			if (!pOrdinSkill) 
//              continue;

			Npc[nNpcIndex].m_SkillList.Add(
				pSync->m_sAllSkill[i].SkillId,
				pSync->m_sAllSkill[i].SkillLevel,
				pSync->m_sAllSkill[i].SkillExp
            );
		}
	}
}

void KProtocolProcess::SyncEnd(BYTE* pMsg)
{
	BYTE	SyncEnd = (BYTE)c2s_syncend;
	if (g_pClient)
		g_pClient->SendPackToServer(&SyncEnd, sizeof(BYTE));
	Player[CLIENT_PLAYER_INDEX].SetDefaultImmedSkill();
	g_DebugLog("[TRACE]SyncEnd to Server");
	CoreDataChanged(GDCNI_GAME_START, 0, 0);	

	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetInstantSpr(enumINSTANT_STATE_LOGIN);

	g_SubWorldSet.m_cMusic.Start(SubWorld[0].m_SubWorldID, SubWorld[0].m_dwCurrentTime, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_FightMode);

	Player[CLIENT_PLAYER_INDEX].m_cPK.Init();
}

void	KProtocolProcess::s2cTradeChangeState(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].s2cTradeChangeState(pMsg);
}

void	KProtocolProcess::s2cNpcSetMenuState(BYTE* pMsg)
{
	NPC_SET_MENU_STATE_SYNC	*pState = (NPC_SET_MENU_STATE_SYNC*)pMsg;
	int		nNpc;
	if (pState->m_dwID == Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID)
	{
		nNpc = Player[CLIENT_PLAYER_INDEX].m_nIndex;
		if (pState->m_btState == PLAYER_MENU_STATE_TRADEOPEN)
		{
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_TRADE_STATE_OPEN);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		else if (pState->m_btState == PLAYER_MENU_STATE_NORMAL)
		{
			if (Npc[nNpc].GetMenuState() == PLAYER_MENU_STATE_TRADEOPEN)
			{
				KSystemMessage	sMsg;
				sprintf(sMsg.szMessage, MSG_TRADE_STATE_CLOSE);
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
				sMsg.byParamSize = 0;
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			}
		}
	}
	else
	{
		nNpc = NpcSet.SearchID(pState->m_dwID);
		if (nNpc == 0)
			return;
	}
	if (pState->m_wLength > sizeof(NPC_SET_MENU_STATE_SYNC) - 1 - sizeof(pState->m_szSentence))
		Npc[nNpc].SetMenuState(pState->m_btState, pState->m_szSentence, pState->m_wLength - (sizeof(NPC_SET_MENU_STATE_SYNC) - 1 - sizeof(pState->m_szSentence)));
	else
		Npc[nNpc].SetMenuState(pState->m_btState);
}

void	KProtocolProcess::s2cTradeMoneySync(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].s2cTradeMoneySync(pMsg);
}

void	KProtocolProcess::s2cTradeDecision(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].s2cTradeDecision(pMsg);
}

void	KProtocolProcess::s2cTeamInviteAdd(BYTE * pMsg)
{
	Player[CLIENT_PLAYER_INDEX].m_cTeam.ReceiveInvite((TEAM_INVITE_ADD_SYNC*)pMsg);
}

void	KProtocolProcess::s2cTradePressOkSync(BYTE * pMsg)
{
	Player[CLIENT_PLAYER_INDEX].m_cTrade.SetTradeState((TRADE_STATE_SYNC*)pMsg);
}

void	KProtocolProcess::s2cDirectlyCastSkill(BYTE * pMsg)
{
	DWORD	dwNpcID;
	int		nSkillID, nSkillLevel;
	DWORD	MapX, MapY;
	
	dwNpcID = *(DWORD *)&pMsg[1];
	nSkillID = *(int *)&pMsg[5];
	nSkillLevel = *(int *)&pMsg[9];
	MapX = *(int *)&pMsg[13];
	MapY = *(int *)&pMsg[17];
	
	//当指定某个目标时(MapX == -1),MapY为目标的NpcdwID，需要转换成本地的NpcIndex才行
	if (MapX == -1)
	{
		if (MapY < 0 ) return;
		MapY = NpcSet.SearchID(MapY);
		if (MapY == 0)	return;
		if (Npc[MapY].m_RegionIndex < 0)
			return;
		int nX, nY;
		nX = Npc[MapY].m_MapX;
		nY = Npc[MapY].m_MapY;
		SubWorld[0].Map2Mps(Npc[MapY].m_RegionIndex, nX, nY, 0, 0, (int *)&MapX, (int *)&MapY);
	}
	
	int nIdx = NpcSet.SearchID(dwNpcID);
	
	_ASSERT (nSkillID > 0 && nSkillLevel > 0);
	KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(nSkillID, nSkillLevel);
	if (!pOrdinSkill) 
        return ;
	
    pOrdinSkill->Cast(nIdx, MapX, MapY);

	if(!pOrdinSkill->IsAura())
	{
		DWORD dwCastTime = 0;
		eSkillStyle eStyle = (eSkillStyle)pOrdinSkill->GetSkillStyle();
		if (eStyle == SKILL_SS_Missles 
			|| eStyle == SKILL_SS_Melee 
			|| eStyle == SKILL_SS_InitiativeNpcState 
			|| eStyle == SKILL_SS_PassivityNpcState)
		{
			dwCastTime = pOrdinSkill->GetDelayPerCast(Npc[nIdx].m_bRideHorse);
		}
		else
		{
			switch(eStyle)
			{
			case SKILL_SS_Thief:
				{
					dwCastTime = ((KThiefSkill*)pOrdinSkill)->GetDelayPerCast();
				}break;
			}
		}
		Npc[nIdx].m_SkillList.SetNextCastTime(nSkillID, SubWorld[0].m_dwCurrentTime, SubWorld[0].m_dwCurrentTime + dwCastTime);
	}
}

void	KProtocolProcess::s2cShowMsg(BYTE *pMsg)
{
	if (!pMsg)
		return;
	SHOW_MSG_SYNC	*pShowMsg = (SHOW_MSG_SYNC*)pMsg;
	switch (pShowMsg->m_wMsgID)
	{
	case enumMSG_ID_TEAM_KICK_ONE:
		{
			char	szName[32];
			KSystemMessage	sMsg;

			memset(szName, 0, sizeof(szName));
			memcpy(szName, pMsg + sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID), pShowMsg->m_wLength + 1 + sizeof(LPVOID) - sizeof(SHOW_MSG_SYNC));
			if (strcmp(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name, szName) == 0)
			{
				sprintf(sMsg.szMessage, MSG_TEAM_BE_KICKEN);
				sMsg.eType = SMT_TEAM;
				sMsg.byConfirmType = SMCT_CLICK;
				sMsg.byPriority = 1;
			}
			else
			{
				sprintf(sMsg.szMessage, MSG_TEAM_KICK_ONE, szName);
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
			}
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TEAM_DISMISS:
		{
			if (!Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag)
				break;
			KSystemMessage	sMsg;
			if (Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure == TEAM_CAPTAIN)
			{
				sprintf(sMsg.szMessage, MSG_TEAM_DISMISS_CAPTAIN);
			}
			else
			{
				sprintf(sMsg.szMessage, MSG_TEAM_DISMISS_MEMBER, g_Team[0].m_szMemName[0]);
			}
			sMsg.eType = SMT_TEAM;
			sMsg.byConfirmType = SMCT_CLICK;
			sMsg.byPriority = 1;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TEAM_LEAVE:
		{
			if (!Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag)
				break;
			KSystemMessage	sMsg;
			DWORD	dwID = *(DWORD*)(&pShowMsg->m_lpBuf);
			if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID == dwID)
			{
				sprintf(sMsg.szMessage, MSG_TEAM_LEAVE_SELF_MSG, g_Team[0].m_szMemName[0]);
			}
			else
			{
				sprintf(sMsg.szMessage, MSG_TEAM_LEAVE, NORMAL_UNCLEAR_WORD);
				for (int i = 0; i < MAX_TEAM_MEMBER; i++)
				{
					if ((DWORD)g_Team[0].m_nMember[i] == dwID)
					{
						sprintf(sMsg.szMessage, MSG_TEAM_LEAVE, g_Team[0].m_szMemName[i + 1]);
						break;
					}
				}
			}
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TEAM_REFUSE_INVITE:
		{
			char	szName[32];
			memset(szName, 0, sizeof(szName));
			memcpy(szName, &pShowMsg->m_lpBuf, pShowMsg->m_wLength + 1 + sizeof(LPVOID) - sizeof(SHOW_MSG_SYNC));

			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_TEAM_REFUSE_INVITE, szName);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TEAM_APPLY_INVITE_FAIL:
		{
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_TEAM_APPLY_INVITE_FAIL);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TEAM_APPLY_INVITE_FAIL2:
		{
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_TEAM_APPLY_INVITE_FAIL2);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TEAM_SELF_ADD:
		{
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_TEAM_SELF_ADD, g_Team[0].m_szMemName[0]);
			sMsg.eType = SMT_TEAM;
			sMsg.byConfirmType = SMCT_UI_TEAM;
			sMsg.byPriority = 3;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TEAM_ADDMEMBER_NOTVALID:
		{
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_TEAM_ADDMEMBER_NOTVALID);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 3;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TEAM_CHANGE_CAPTAIN_FAIL:
		{
			int		nMember;
			DWORD	dwID = *(DWORD*)(&pShowMsg->m_lpBuf);
			nMember = g_Team[0].FindMemberID(dwID);
			if (nMember < 0)
				break;
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;

			sprintf(sMsg.szMessage, MSG_TEAM_CHANGE_CAPTAIN_FAIL1);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			sprintf(sMsg.szMessage, MSG_TEAM_CHANGE_CAPTAIN_FAIL2, g_Team[0].m_szMemName[nMember + 1]);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TEAM_CHANGE_CAPTAIN_FAIL2:
		{
			int		nMember;
			DWORD	dwID = *(DWORD*)(&pShowMsg->m_lpBuf);
			nMember = g_Team[0].FindMemberID(dwID);
			if (nMember < 0)
				break;
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;

			sprintf(sMsg.szMessage, MSG_TEAM_CHANGE_CAPTAIN_FAIL1);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			sprintf(sMsg.szMessage, MSG_TEAM_CHANGE_CAPTAIN_FAIL3);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_OBJ_CANNOT_PICKUP:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;

			strcpy(sMsg.szMessage, MSG_OBJ_CANNOT_PICKUP);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_OBJ_TOO_FAR:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			strcpy(sMsg.szMessage, MSG_OBJ_TOO_FAR);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_DEC_MONEY:
		{	
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_DEC_MONEY, (int)pShowMsg->m_lpBuf);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_SPAR_VICTORY:
		{	
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_SPAR_VICTORY);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_SPAR_DEFEAT:
		{	
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_SPAR_DEFEAT);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TRADE_SELF_ROOM_FULL:
		{
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_TRADE_SELF_ROOM_FULL);
			sMsg.eType = SMT_SYSTEM;
			sMsg.byConfirmType = SMCT_CLICK;
			sMsg.byPriority = 1;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TRADE_DEST_ROOM_FULL:
		{
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_TRADE_DEST_ROOM_FULL, Player[CLIENT_PLAYER_INDEX].m_cTrade.m_szDestName);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TRADEFOLKGAME_SELF_ROOM_FULL:
		{
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_TRADEFOLKGAME_SELF_ROOM_FULL, TRADEFOLKGAME_NEED_WIDTH, TRADEFOLKGAME_NEED_HEIGHT);
			sMsg.eType = SMT_SYSTEM;
			sMsg.byConfirmType = SMCT_CLICK;
			sMsg.byPriority = 1;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TRADEFOLKGAME_DEST_ROOM_FULL:
		{
			int	nIdx = NpcSet.SearchID(*((DWORD*)&pShowMsg->m_lpBuf));
			if (nIdx <= 0)
				return;
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_TRADEFOLKGAME_DEST_ROOM_FULL, Npc[nIdx].Name, TRADEFOLKGAME_NEED_WIDTH, TRADEFOLKGAME_NEED_HEIGHT);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TRADE_REFUSE_APPLY:
		{
			int	nIdx = NpcSet.SearchID(*((DWORD*)&pShowMsg->m_lpBuf));
			if (nIdx <= 0)
				return;
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_TRADE_REFUSE_APPLY, Npc[nIdx].Name);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_SPAR_REFUSE_APPLY:
		{
			int	nIdx = NpcSet.SearchID(*((DWORD*)&pShowMsg->m_lpBuf));
			if (nIdx <= 0)
				return;
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_SPAR_REFUSE_APPLY, Npc[nIdx].Name);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_GET_ITEM:
		{
			DWORD	dwID = LOWORD(*(DWORD*)(&pShowMsg->m_lpBuf));
			int	nStackNum = (int)HIWORD(*(DWORD*)(&pShowMsg->m_lpBuf));
			
			int nItemIdx = ItemSet.SearchID(dwID);
			if (nItemIdx <= 0 || nItemIdx >= MAX_ITEM)
				break;

			BOOL bIsStack = Item[nItemIdx].IsStack();
			char	szName[128];
			KSystemMessage	sMsg;

			strcpy(szName, Item[nItemIdx].GetName());

			if (strlen(szName) >= sizeof(sMsg.szMessage) - strlen(bIsStack ? MSG_ADD_ITEM_STACK : MSG_ADD_ITEM))
				break;

			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			if(bIsStack)
				sprintf(sMsg.szMessage, MSG_ADD_ITEM_STACK, nStackNum, szName);
			else
				sprintf(sMsg.szMessage, MSG_ADD_ITEM, szName);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;	
	case enumMSG_ID_MONEY_CANNOT_PICKUP:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;

			strcpy(sMsg.szMessage, MSG_MONEY_CANNOT_PICKUP);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_CANNOT_ADD_TEAM:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TEAM_CANNOT_CREATE);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TEAM_IS_FULL:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TEAM_IS_FULL);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_CANNOT_INVITE_TEAM:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TEAM_CLOSE);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TARGET_CANNOT_ADD_TEAM:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TEAM_TARGET_CANNOT_ADD_TEAM);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_GET_INVITE_TEAM_REPLY:
		{
			char	szName[32];
			memset(szName, 0, sizeof(szName));
			memcpy(szName, &pShowMsg->m_lpBuf, pShowMsg->m_wLength + 1 + sizeof(LPVOID) - sizeof(SHOW_MSG_SYNC));

			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TEAM_SEND_INVITE, szName);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_PK_HIGHT_LOCK_NOT_SWITCH:
		{
			int	nPKValue = (int)*((DWORD*)&pShowMsg->m_lpBuf);

			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_PK_HIGHT_LOCK_NOT_SWITCH, nPKValue);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_PKSTATE_NOT_OPEN:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_PKSTATE_NOT_OPEN);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_PK_ERROR_1:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_PK_ERROR_1);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_PK_ERROR_2:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_PK_ERROR_2);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_PK_ERROR_3:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_PK_ERROR_3);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_PK_ERROR_4:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_PK_ERROR_4);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_PK_ERROR_5:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_PK_ERROR_5);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_PK_ERROR_6:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_PK_ERROR_6);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_PK_ERROR_7:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_PK_ERROR_7);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_PK_ERROR_8:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_PK_ERROR_8);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_PK_ERROR_9:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_PK_ERROR_9);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_PK_IS_LOCK:
		{
			int	nNormalPKTimeLong = (int)*((DWORD*)&pShowMsg->m_lpBuf);

			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MGS_PK_IS_LOCK, nNormalPKTimeLong/1000/60);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_PK_IS_LOCK1:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MGS_PK_IS_LOCK1);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_PK_IS_LOCK2:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MGS_PK_IS_LOCK2);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_PK_IS_LOCK3:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MGS_PK_IS_LOCK3);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_PK_IS_LOCK4:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MGS_PK_IS_LOCK4);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_PK_IS_LOCK5:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MGS_PK_IS_LOCK5);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_PK_IS_LOCK6:
		{
			int	nFightPKTimeLong = (int)*((DWORD*)&pShowMsg->m_lpBuf);

			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MGS_PK_IS_LOCK6);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_PK_IS_LOCK7:
		{
			int	nFightPKTimeLong = (int)*((DWORD*)&pShowMsg->m_lpBuf);

			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MGS_PK_IS_LOCK7, nFightPKTimeLong/1000);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TRADE_BUZY:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_CANT_TRADE_BUZY);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TRADE_CLOSED:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TRADE_CLOSED);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TRADE_SELLED:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TRADE_SELLED);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_DEATH_LOSE_ITEM:
		{
			char	szName[32];
			memset(szName, 0, sizeof(szName));
			memcpy(szName, &pShowMsg->m_lpBuf, pShowMsg->m_wLength + 1 + sizeof(LPVOID) - sizeof(SHOW_MSG_SYNC));

			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_DEATH_LOSE_ITEM, szName);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TONG_REFUSE_ADD:
		{
			char	szName[32];

			memset(szName, 0, sizeof(szName));
			memcpy(szName, pMsg + sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID), pShowMsg->m_wLength + 1 + sizeof(LPVOID) - sizeof(SHOW_MSG_SYNC));

			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TONG_REFUSE_ADD, szName);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TONG_BE_KICK:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TONG_BE_KICKED);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

			KUiGameObjectWithName	sUi;
			strcpy(sUi.szName, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
			sUi.nData = TONG_ACTION_DISMISS;
			sUi.nParam = 0;
			sUi.uParam = 0;
			sUi.szString[0] = 0;
			CoreDataChanged(GDCNI_TONG_ACTION_RESULT, (unsigned int)&sUi, 1);

			CoreDataChanged(GDCNI_PLAYER_BASE_INFO, 0, 0);
		}
		break;
	case enumMSG_ID_TONG_LEAVE_SUCCESS:
		{
			Player[CLIENT_PLAYER_INDEX].m_cTong.m_dwLeaveTime = (DWORD)pShowMsg->m_lpBuf;
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TONG_LEAVE_SUCCESS, TongData.m_sTongParam.m_nLeaveTime);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

			CoreDataChanged(GDCNI_PLAYER_BASE_INFO, 0, 0);
		}
		break;
	case enumMSG_ID_TONG_LEAVE_FAIL:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TONG_LEAVE_FAIL);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TONG_NO_MONEY:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TONG_NO_MONEY);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TONG_CHANGE_AS_MASTER:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TONG_CHANGE_AS_MASTER);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

			KUiGameObjectWithName	sUi;
			strcpy(sUi.szName, Player[CLIENT_PLAYER_INDEX].m_cTong.m_szMasterName);
			sUi.nData = TONG_ACTION_DEMISE;
			sUi.nParam = 0;
			sUi.uParam = 0;
			sUi.szString[0] = 0;
			CoreDataChanged(GDCNI_TONG_ACTION_RESULT, (unsigned int)&sUi, 1);
		}
		break;
	case enumMSG_ID_TONG_CHANGE_AS_MEMBER:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TONG_CHANGE_AS_MEMBER);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

			KUiGameObjectWithName	sUi;
			strcpy(sUi.szName, Player[CLIENT_PLAYER_INDEX].m_cTong.m_szMasterName);
			sUi.nData = TONG_ACTION_DEMISE;
			sUi.nParam = 0;
			sUi.uParam = 0;
			sUi.szString[0] = 0;
			CoreDataChanged(GDCNI_TONG_ACTION_RESULT, (unsigned int)&sUi, 1);
		}
		break;
	case enumMSG_ID_TONG_APPLY_ADD_ERROR:
		{
			char	szName[32];

			memset(szName, 0, sizeof(szName));
			memcpy(szName, pMsg + sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID), pShowMsg->m_wLength + 1 + sizeof(LPVOID) - sizeof(SHOW_MSG_SYNC));

			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TONG_APPLY_ADD_ERROR, szName);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TONG_APPLY_ADD_ERROR1:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TONG_APPLY_ADD_ERROR1);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TONG_APPLY_ADD_ERROR2:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TONG_APPLY_ADD_ERROR2);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TONG_APPLY_ADD_ERROR3:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TONG_APPLY_ADD_ERROR3);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TONG_APPLY_ADD_ERROR4:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TONG_APPLY_ADD_ERROR4);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TONG_APPLY_ADD_ERROR5:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TONG_APPLY_ADD_ERROR5);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TONG_APPLY_ADD_ERROR6:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TONG_APPLY_ADD_ERROR6);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TONG_APPLY_ADD_ERROR7:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TONG_APPLY_ADD_ERROR7);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_FIGHT_MODE_ERROR1:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_FIGHT_MODE_ERROR1);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_FIGHT_MODE_ERROR2:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_FIGHT_MODE_ERROR2);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_FIGHT_MODE_ERROR3:
		{
			int	nIdx = NpcSet.SearchID(*((DWORD*)&pShowMsg->m_lpBuf));
			if (nIdx <= 0)
				return;

			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_FIGHT_MODE_ERROR3, Npc[nIdx].Name);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_SHOP_NO_ROOM:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_SYSTEM;
			sMsg.byConfirmType = SMCT_CLICK;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_SHOP_NO_ROOM);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_SHOP_NO_MONEY:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_SYSTEM;
			sMsg.byConfirmType = SMCT_CLICK;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_SHOP_NO_MONEY);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_SHOP_NO_EXTPOINT:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_SYSTEM;
			sMsg.byConfirmType = SMCT_CLICK;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_SHOP_NO_EXTPOINT);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_SHOP_NO_FUYUAN:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_SYSTEM;
			sMsg.byConfirmType = SMCT_CLICK;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_SHOP_NO_FUYUAN);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_SHOP_NO_REPUTE:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_SYSTEM;
			sMsg.byConfirmType = SMCT_CLICK;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_SHOP_NO_REPUTE);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_SHOP_NO_ACCUM:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_SYSTEM;
			sMsg.byConfirmType = SMCT_CLICK;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_SHOP_NO_ACCUM);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_SHOP_NO_HONOR:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_SYSTEM;
			sMsg.byConfirmType = SMCT_CLICK;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_SHOP_NO_HONOR);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_SHOP_NO_RESPECT:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_SYSTEM;
			sMsg.byConfirmType = SMCT_CLICK;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_SHOP_NO_RESPECT);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_FUNCTION_CHAT_FORBIDDENED:
		{
			time_t rawtime=(int)*((DWORD*)&pShowMsg->m_lpBuf)+1451581200;
			struct tm * timeinfo = localtime(&rawtime);

			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			strftime(sMsg.szMessage, sizeof(sMsg.szMessage), MSG_NPC_NOT_PAY_FOR_SPEECH, timeinfo);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_FUNCTION_FOBIDDENED:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_FUNCTION_FOBIDDENED);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_FUNCTION_LOCKED:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_FUNCTION_LOCKED);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_LOCK_NOT_TRADE:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_LOCK_NOT_TRADE);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_LOCK_NOT_SETPRICE:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_LOCK_NOT_SETPRICE);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_LOCK_NOT_THROW:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_LOCK_NOT_THROW);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_ITEM_TOWNPORTAL:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_SYSTEM;
			sMsg.byConfirmType = SMCT_CLICK;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_NPC_NOT_USE_TOWNPORTAL);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_ITEM_CANTREPAIR:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_BROKEN_CANTREPAIR);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_ITEM_SAME_IMMEDIATE:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_ITEM_SAME_IN_IMMEDIATE);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_ITEM_CANTPUT_IMMEDIATE:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_ITEM_PUT_IN_IMMEDIATE);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_HORSE_CANT_SWITCH1:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_CANT_SWITCH_HORSE);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_HORSE_CANT_SWITCH2:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_CANT_SWITCH_HORSE_SIT);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_CHATROOM_JOIN:
		{
			char	szName[32];
			KSystemMessage	sMsg;

			memset(szName, 0, sizeof(szName));
			memcpy(szName, pMsg + sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID), pShowMsg->m_wLength + 1 + sizeof(LPVOID) - sizeof(SHOW_MSG_SYNC));
			sprintf(sMsg.szMessage, MSG_CHATROOM_JOIN, szName);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

			CoreDataChanged(GDCNI_CHATROOM_UPDATE_INTERFACE, 0, 0);
		}
		break;
	case enumMSG_ID_CHATROOM_LEAVE:
		{
			char	szName[32];
			KSystemMessage	sMsg;

			memset(szName, 0, sizeof(szName));
			memcpy(szName, pMsg + sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID), pShowMsg->m_wLength + 1 + sizeof(LPVOID) - sizeof(SHOW_MSG_SYNC));
			sprintf(sMsg.szMessage, MSG_CHATROOM_LEAVE, szName);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

			CoreDataChanged(GDCNI_CHATROOM_UPDATE_INTERFACE, 0, 0);
		}
		break;
	case enumMSG_ID_CHATROOM_BEKICK:
		{
			char	szName[32];
			KSystemMessage	sMsg;

			memset(szName, 0, sizeof(szName));
			memcpy(szName, pMsg + sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID), pShowMsg->m_wLength + 1 + sizeof(LPVOID) - sizeof(SHOW_MSG_SYNC));
			sprintf(sMsg.szMessage, MSG_CHATROOM_BEKICK, szName);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

			CoreDataChanged(GDCNI_CHATROOM_UPDATE_INTERFACE, 0, 0);
		}
		break;
	case enumMSG_ID_CHATROOM_OPENGAME:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_CHATROOM_OPENGAME, (int)*((DWORD*)&pShowMsg->m_lpBuf));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

			CoreDataChanged(GDCNI_CHATROOM_UPDATE_INTERFACE, 0, 0);
		}
		break;
	case  enumMSG_ID_CHATROOM_REVERSE:
		{
			char	szName[32];
			KSystemMessage	sMsg;

			memset(szName, 0, sizeof(szName));
			memcpy(szName, pMsg + sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID), pShowMsg->m_wLength + 1 + sizeof(LPVOID) - sizeof(SHOW_MSG_SYNC));
			sprintf(sMsg.szMessage, MSG_CHATROOM_REVERSE, szName);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

			CoreDataChanged(GDCNI_CHATROOM_UPDATE_INTERFACE, 0, 0);
		}
		break;
	case enumMSG_ID_NPC_RENASCENCE_SOMEONE:
		{
			KSystemMessage sMsg;
			sMsg.byConfirmType = SMCT_UI_RENASCENCE;
			sMsg.byParamSize = 0;
			sMsg.byPriority = 0;
			sMsg.eType = SMT_PLAYER;
			int	nIdx = NpcSet.SearchID(*((DWORD*)&pShowMsg->m_lpBuf));
			if (nIdx)
				sprintf(sMsg.szMessage, MSG_NPC_RENASCENCE_SOMEONE, Npc[nIdx].Name);
			else
				sprintf(sMsg.szMessage, MSG_NPC_RENASCENCE);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_NPC_RENASCENCE:
		{
			KSystemMessage sMsg;
			sMsg.byConfirmType = SMCT_UI_RENASCENCE;
			sMsg.byParamSize = 0;
			sMsg.byPriority = 0;
			sMsg.eType = SMT_PLAYER;
			sprintf(sMsg.szMessage, MSG_NPC_RENASCENCE);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	default:
		break;
	}
}

void	KProtocolProcess::SyncStateEffect(BYTE* pMsg)
{
	STATE_EFFECT_SYNC*	pSync = (STATE_EFFECT_SYNC *)pMsg;

	int nDataNum = MAX_SKILL_STATE - (sizeof(STATE_EFFECT_SYNC) - pSync->m_wLength) / sizeof(KMagicAttrib);
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetStateSkillEffect(Player[CLIENT_PLAYER_INDEX].m_nIndex, pSync->m_dwSkillID, pSync->m_nLevel, pSync->m_MagicAttrib, nDataNum, pSync->m_nTime, pSync->m_bOverLook);
}

void	KProtocolProcess::IgnoreState(BYTE* pMsg)
{
	IGNORE_STATE_SYNC	*pSync = (IGNORE_STATE_SYNC*)pMsg;
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].IgnoreState((BOOL)pSync->bNegative);
}

void	KProtocolProcess::s2cTradeApplyStart(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].s2cTradeApplyStart(pMsg);
}

void	KProtocolProcess::s2cCreateTong(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].m_cTong.ForceCreateTong(pMsg);
}

void	KProtocolProcess::s2cJoinTong(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].m_cTong.s2cJoinTong(pMsg);
}

void	KProtocolProcess::s2cJoinTongReply(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].m_cTong.s2cJoinTongReply(pMsg);
}

void	KProtocolProcess::s2cItemAutoMove(BYTE* pMsg)
{
	ITEM_AUTO_MOVE_SYNC	*pSync = (ITEM_AUTO_MOVE_SYNC*)pMsg;
	ItemPos	sSrc, sDest;
	sSrc.nPlace = pSync->m_btSrcPos;
	sSrc.nX = pSync->m_btSrcX;
	sSrc.nY = pSync->m_btSrcY;
	sDest.nPlace = pSync->m_btDestPos;
	sDest.nX = pSync->m_btDestX;
	sDest.nY = pSync->m_btDestY;
	
	Player[CLIENT_PLAYER_INDEX].m_ItemList.AutoMoveItem(sSrc, sDest);
}

extern IClientCallback* l_pDataChangedNotifyFunc;

void KProtocolProcess::s2cExtend(BYTE* pMsg)
{
	EXTEND_HEADER* pHeader = (EXTEND_HEADER*)(pMsg + sizeof(tagExtendProtoHeader));

	if (pHeader->ProtocolFamily == pf_playercommunity)
	{
		if (pHeader->ProtocolID == playercomm_s2c_notifychannelid)
		{
			PLAYERCOMM_NOTIFYCHANNELID* pNChann = (PLAYERCOMM_NOTIFYCHANNELID*)pHeader;

			l_pDataChangedNotifyFunc->NotifyChannelID(pNChann->channel, pNChann->channelid, pNChann->cost);
		}
	}
}

void KProtocolProcess::s2cExtendChat(BYTE* pMsg)
{
	tagExtendProtoHeader* pExHdr = (tagExtendProtoHeader*)pMsg;
	void* pExPckg = pExHdr + 1;
	BYTE protocol = *(BYTE*)(pExPckg);

	if (protocol == chat_someonechat)
	{
		CHAT_SOMEONECHAT_SYNC* pCscSync = (CHAT_SOMEONECHAT_SYNC*)pExPckg;

		l_pDataChangedNotifyFunc->MSNMessageArrival(
			pCscSync->someone, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name,
			(const char*)(pCscSync + 1), pCscSync->sentlen, (const char*)&pCscSync->item, true);
	}
	else if (protocol == chat_channelchat)
	{
		CHAT_CHANNELCHAT_SYNC* pCccSync = (CHAT_CHANNELCHAT_SYNC*)pExPckg;
		l_pDataChangedNotifyFunc->ChannelMessageArrival(
			pCccSync->channelid, pCccSync->someone,
			(const char*)(pCccSync + 1), pCccSync->sentlen, (const char*)&pCccSync->item, pCccSync->someflag, true);
	}
	else if (protocol == chat_feedback)
	{
		////X
		CHAT_FEEDBACK* pCfb = (CHAT_FEEDBACK*)pExPckg;
		DWORD* pChannelid = (DWORD*)(pCfb + 1);

		if (*pChannelid == -1)
		{//someone
			char* pDstName = (char*)(pChannelid + 1);
			BYTE* pSentlen = (BYTE*)(pDstName + _NAME_LEN);
			void* pSent = pSentlen + 1;

			l_pDataChangedNotifyFunc->MSNMessageArrival(
				Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name, pDstName,
				(const char*)pSent, *pSentlen, (const char*)&pCfb->item, pCfb->code != codeFail);
		}
		else
		{//channel
			BYTE* pSentlen = (BYTE*)(pChannelid + 1);
			void* pSent = pSentlen + 1;

			l_pDataChangedNotifyFunc->ChannelMessageArrival(
				*pChannelid, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name,
				(const char*)pSent, *pSentlen, (const char*)&pCfb->item, pCfb->someflag, pCfb->code != codeFail);
		}
		////X
	}
}


static BOOL sParseUGName(const std::string& name, std::string* pUnit, std::string* pGroup)
{
	static const char char_split = '\n';

	size_t pos = name.find(char_split);
	if (pos == name.npos)
	{
		if (pUnit)
			pUnit->resize(0);
		if (pGroup)
			pGroup->assign(name);
	}
	else
	{
		std::string::const_iterator itSplit = name.begin() + pos;

		if (pUnit)
			pUnit->assign(name.begin(), itSplit);
		if (pGroup)
			pGroup->assign(itSplit + 1, name.end());
	}

	return TRUE;
}

void KProtocolProcess::s2cExtendFriend(BYTE* pMsg)
{
	tagExtendProtoHeader* pExHdr = (tagExtendProtoHeader*)pMsg;
	void* pExPckg = pExHdr + 1;

	EXTEND_HEADER* pHeader = (EXTEND_HEADER*)(pExPckg);

	if (pHeader->ProtocolFamily == pf_tong)
	{
		_ASSERT(FALSE);
		//tong message
		//...
	}
	else if (pHeader->ProtocolFamily == pf_friend)
	{
		if (pHeader->ProtocolID == friend_c2c_askaddfriend)
		{
			ASK_ADDFRIEND_SYNC* pAafSync = (ASK_ADDFRIEND_SYNC*)pHeader;
			l_pDataChangedNotifyFunc->FriendInvite(pAafSync->srcrole);
		}
		else if (pHeader->ProtocolID == friend_c2s_erasefriend)
		{
			REP_ERASEFRIEND_SYNC* pEfSync = (REP_ERASEFRIEND_SYNC*)pHeader;
			l_pDataChangedNotifyFunc->DeleteFriend(pEfSync->srcrole);
		}
		else if (pHeader->ProtocolID == friend_c2c_repaddfriend)
		{
			REP_ADDFRIEND_SYNC* pRafSync = (REP_ADDFRIEND_SYNC*)pHeader;
			l_pDataChangedNotifyFunc->AddFriend(pRafSync->srcrole, pRafSync->answer);
		}
		else if (pHeader->ProtocolID == friend_s2c_repsyncfriendlist)
		{
			REP_SYNCFRIENDLIST* pRsfl = (REP_SYNCFRIENDLIST*)pHeader;
			char* pGroupTag = (char*)(pRsfl + 1);
			if (*pGroupTag != specGroup)
				goto on_error;
			{{
			char* pGroup = (char*)(pGroupTag + 1);
on_newgroup:
			std::string theUnit, theGroup;
			sParseUGName(std::string(pGroup), &theUnit, &theGroup);

			for (char* pRoleTag = pGroup + strlen(pGroup) + 1; ; )
			{
				if (*pRoleTag == specOver)
					goto on_over;
				else if (*pRoleTag == specGroup)
				{
					pGroup = pRoleTag + 1;
					goto on_newgroup;
				}
				else if (*pRoleTag == specRole)
				{
					char* pState = pRoleTag + 1;
					char* pRole = pState + 1;
					l_pDataChangedNotifyFunc->FriendInfo(pRole, (char*)theUnit.c_str(), (char*)theGroup.c_str(), (BYTE)*pState);
					pRoleTag = pRole + strlen(pRole) + 1;
					continue;
				}
				else
					goto on_error;
			}
			}}
on_error:
			_ASSERT(FALSE);
on_over:
			0;
		}
		else if (pHeader->ProtocolID == friend_s2c_friendstate)
		{
			FRIEND_STATE* pFs = (FRIEND_STATE*)pHeader;
			for (char* pRole = (char*)(pFs + 1); *pRole; pRole += strlen(pRole) + 1)
				l_pDataChangedNotifyFunc->FriendStatus(pRole, pFs->state);
		}
		else if (pHeader->ProtocolID == friend_s2c_syncassociate)
		{
			FRIEND_SYNCASSOCIATE* pFsa = (FRIEND_SYNCASSOCIATE*)pHeader;

			char* szGroup = (char*)(pFsa + 1);
			std::string group(szGroup);

			std::string theUnit, theGroup;
			sParseUGName(group, &theUnit, &theGroup);
			_ASSERT(theGroup.empty());

			for (char* szRole = szGroup + group.size() + 1; *szRole; szRole += strlen(szRole) + 1)
				l_pDataChangedNotifyFunc->AddPeople((char*)theUnit.c_str(), szRole);
		}
	}
}


void KProtocolProcess::s2cExtendTong(BYTE* pMsg)
{
	S2C_TONG_HEAD	*pHead = (S2C_TONG_HEAD*)pMsg;
	switch (pHead->m_btMsgId)
	{
	case enumTONG_SYNC_ID_CREATE_FAIL:
		{
			TONG_CREATE_FAIL_SYNC *pFail = (TONG_CREATE_FAIL_SYNC*)pMsg;

			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;

			sprintf(sMsg.szMessage, "Th祅h l藀 bang h閕 th蕋 b筰");
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

			switch (pFail->m_btFailId)
			{
			case enumTONG_CREATE_ERROR_ID1:		// Player[m_nPlayerIndex].m_nIndex <= 0
				break;
			case enumTONG_CREATE_ERROR_ID2:		// 交易过程中
				break;
			case enumTONG_CREATE_ERROR_ID3:		// 帮会名问题
				sprintf(sMsg.szMessage, MSG_TONG_CREATE_ERROR09);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			case enumTONG_CREATE_ERROR_ID4:		// 帮会阵营问题
				sprintf(sMsg.szMessage, MSG_TONG_CREATE_ERROR02);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			case enumTONG_CREATE_ERROR_ID5:		// 已经是帮会成员
				sprintf(sMsg.szMessage, MSG_TONG_CREATE_ERROR03);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			case enumTONG_CREATE_ERROR_ID6:		// 自己的阵营问题
				sprintf(sMsg.szMessage, MSG_TONG_CREATE_ERROR04);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			case enumTONG_CREATE_ERROR_ID7:		// 等级问题
				sprintf(sMsg.szMessage, MSG_TONG_CREATE_ERROR05);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			case enumTONG_CREATE_ERROR_ID8:		// 钱问题
				sprintf(sMsg.szMessage, MSG_TONG_CREATE_ERROR10);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			case enumTONG_CREATE_ERROR_ID9:		// 组队不能建帮会
				sprintf(sMsg.szMessage, MSG_TONG_CREATE_ERROR08);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			case enumTONG_CREATE_ERROR_ID10:	// 帮会模块出错
				break;
			case enumTONG_CREATE_ERROR_ID11:	// 名字字符串出错
				sprintf(sMsg.szMessage, MSG_TONG_CREATE_ERROR11, TongData.m_sTongParam.m_nRepute);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			case enumTONG_CREATE_ERROR_ID12:	// 名字字符串过长
				sprintf(sMsg.szMessage, MSG_TONG_CREATE_ERROR12, TongData.m_sTongParam.m_nFuYuan);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			case enumTONG_CREATE_ERROR_ID13:	// 帮会同名错误
				sprintf(sMsg.szMessage, MSG_TONG_CREATE_ERROR13);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			case enumTONG_CREATE_ERROR_ID14:	// 帮会产生失败
				sprintf(sMsg.szMessage, MSG_TONG_CREATE_ERROR14);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			case enumTONG_CREATE_ERROR_ID15:	// 帮会产生失败
				sprintf(sMsg.szMessage, MSG_TONG_CREATE_ERROR15);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			}
		}
		break;
	case enumTONG_SYNC_ID_TRANSFER_ADD_APPLY:
		{
			TONG_APPLY_ADD_SYNC	*pApply = (TONG_APPLY_ADD_SYNC*)pMsg;
			char	szName[32];
			DWORD	dwNameID;
			int		nPlayerIdx;

			memset(szName, 0, sizeof(szName));
			memcpy(szName, pApply->m_szName, pApply->m_wLength + 1 + sizeof(pApply->m_szName) - sizeof(TONG_APPLY_ADD_SYNC));
			dwNameID = g_FileName2Id(szName);
			nPlayerIdx = pApply->m_nPlayerIdx;

			// 给界面发消息，收到申请，是否同意
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_TONG_APPLY_ADD_SELF_MSG, szName);
			sMsg.eType = SMT_CLIQUE;
			sMsg.byConfirmType = SMCT_UI_TONG_JOIN;
			sMsg.byPriority = 3;
			sMsg.byParamSize = sizeof(KUiPlayerItem);
			
			KUiPlayerItem	player;
			strcpy(player.Name, szName);
			player.nIndex = pApply->m_nPlayerIdx;
			player.uId = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, (int)&player);
		}
		break;
	case enumTONG_SYNC_ID_ADD:
		{
			TONG_ADD_SYNC	*pAdd = (TONG_ADD_SYNC*)pMsg;
			char	szName[32], szAgname[32], szMaster[32];

			memcpy(szName, pAdd->m_szTongName,sizeof(szName));
			memcpy(szAgname, pAdd->m_szAgname, sizeof(szAgname));
			memcpy(szMaster, pAdd->m_szMaster, sizeof(szMaster));

			Player[CLIENT_PLAYER_INDEX].m_cTong.AddTong(pAdd->m_btCamp, szName, szAgname, szMaster);
			// 通知界面更新数据
			KUiGameObjectWithName	sUi;
			memset(&sUi, 0, sizeof(sUi));
			strcpy(sUi.szName, szName);
			sUi.nData = TONG_ACTION_APPLY;
			strcpy(sUi.szString, szAgname);
			CoreDataChanged(GDCNI_TONG_ACTION_RESULT, (unsigned int)&sUi, 1);
		/*------------------------------*/
			CoreDataChanged(GDCNI_PLAYER_BASE_INFO, 0, 0);
		}
		break;

	case enumTONG_SYNC_ID_HEAD_INFO:
		{
			TONG_HEAD_INFO_SYNC	*pInfo = (TONG_HEAD_INFO_SYNC*)pMsg;

			// 通知界面得到某帮会信息
			int nIdx = NpcSet.SearchID(pInfo->m_dwNpcID);
			if (nIdx <= 0)
				break;

			KUiPlayerRelationWithOther	sUi;
			sUi.nIndex = nIdx;
			sUi.uId = Npc[nIdx].m_dwID;
			if (nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex)
				sUi.nRelation = Player[CLIENT_PLAYER_INDEX].m_cTong.m_nFigure;
			else
				sUi.nRelation = -1;
			sUi.nData = 0;
			sUi.nParam = 0;
			strcpy(sUi.Name, Npc[nIdx].Name);

			KTongInfo	sInfo;
			memset(&sInfo, 0, sizeof(sInfo));
			sInfo.nFaction = pInfo->m_btCamp;
			sInfo.btLevel = pInfo->m_btLevel;
			sInfo.nMemberCount = (int)pInfo->m_dwMemberNum;
			sInfo.nManagerCount = (int)pInfo->m_btManagerNum;
			sInfo.nDirectorCount = (int)pInfo->m_btDirectorNum;
			sInfo.nMoney = pInfo->m_dwMoney;
			strcpy(sInfo.szName, pInfo->m_szTongName);
			strcpy(sInfo.szMasterName, pInfo->m_sMember[0].m_szName);
			sInfo.dwTotalEff = pInfo->m_dwTotalEff;
			sInfo.nMasterSaveEff = pInfo->m_sMember[0].m_nMasterEff;
			sInfo.nMasterJoinTm = pInfo->m_sMember[0].m_nMasterJoinTm;
			sInfo.bRecruit = pInfo->m_bRecruit;
			sInfo.nTongJiyuParam = pInfo->m_nTongJiyuParam;
			strcpy(sInfo.szTongJiyuNotify, pInfo->m_szTongJiyuNotify);
			CoreDataChanged(GDCNI_TONG_INFO, (unsigned int)&sUi, (unsigned int)&sInfo);

			KUiGameObjectWithName	sObj;
			strcpy(sObj.szName, pInfo->m_szTongName);
			sObj.nData = enumTONG_FIGURE_DIRECTOR;
			sObj.nParam = 0;
			sObj.uParam = defTONG_MAX_DIRECTOR;
			sObj.szString[0] = 0;

			KTongMemberItem	sItem[defTONG_MAX_DIRECTOR];
			memset(sItem, 0, sizeof(sItem));
			for (int i = 0; i < pInfo->m_btDirectorNum; i++)
			{
				sItem[i].nData = enumTONG_FIGURE_DIRECTOR;
				strcpy(sItem[i].Name, pInfo->m_sMember[i + 1].m_szName);
				strcpy(sItem[i].szAgname, pInfo->m_sMember[i + 1].m_szAgname);
				sItem[i].bOnline = pInfo->m_sMember[i + 1].m_bOnline;
				sItem[i].nSaveEff = pInfo->m_sMember[i + 1].m_nDirectorEff[i];
				sItem[i].nJoinTm = pInfo->m_sMember[i + 1].m_nDirectorJoinTm[i];
			}
			CoreDataChanged(GDCNI_TONG_MEMBER_LIST, (unsigned int)&sObj, (unsigned int)sItem);
		}
		break;
	case enumTONG_SYNC_ID_SELF_INFO:
		{
			TONG_SELF_INFO_SYNC	*pInfo = (TONG_SELF_INFO_SYNC*)pMsg;
			Player[CLIENT_PLAYER_INDEX].m_cTong.SetSelfInfo(pInfo);
			// 通知界面更新数据
			KTongInfo	sInfo;
			memset(&sInfo, 0, sizeof(sInfo));
			sInfo.nFaction = pInfo->m_btCamp;
			sInfo.btLevel = pInfo->m_btLevel;
			sInfo.nMemberCount = (int)pInfo->m_dwMemberNum;
			sInfo.nManagerCount = (int)pInfo->m_btManagerNum;
			sInfo.nDirectorCount = (int)pInfo->m_btDirectorNum;
			sInfo.nMoney = pInfo->m_dwMoney;
			strcpy(sInfo.szName, pInfo->m_szTongName);
			strcpy(sInfo.szMasterName, pInfo->m_szMasterName);
			sInfo.dwTotalEff = pInfo->m_dwTotalEff;
			sInfo.nMasterJoinTm=0;
			sInfo.nMasterSaveEff=0;
			sInfo.bRecruit = pInfo->m_bRecruit;
			sInfo.nTongJiyuParam = pInfo->m_nTongJiyuParam;
			strcpy(sInfo.szTongJiyuNotify, pInfo->m_szTongJiyuNotify);
			CoreDataChanged(GDCNI_TONG_INFO, NULL, (unsigned int)&sInfo);

			CoreDataChanged(GDCNI_PLAYER_BASE_INFO, 0, 0);
		}
		break;
	case enumTONG_SYNC_ID_MANAGER_INFO:
		{
			TONG_MANAGER_INFO_SYNC	*pInfo = (TONG_MANAGER_INFO_SYNC*)pMsg;

			// 通知界面得到某帮会队长信息
			KUiGameObjectWithName	sObj;
			strcpy(sObj.szName, pInfo->m_szTongName);
			sObj.nData = enumTONG_FIGURE_MANAGER;
			sObj.nParam = pInfo->m_btStateNo;
			sObj.uParam = pInfo->m_btCurNum;
			sObj.szString[0] = 0;

			KTongMemberItem	sItem[defTONG_ONE_PAGE_MAX_NUM];
			memset(sItem, 0, sizeof(sItem));
			for (int i = 0; i < pInfo->m_btCurNum; i++)
			{
				sItem[i].nData = enumTONG_FIGURE_MANAGER;
				strcpy(sItem[i].Name, pInfo->m_sMember[i].m_szName);
				strcpy(sItem[i].szAgname, pInfo->m_sMember[i].m_szAgname);
				sItem[i].bOnline = pInfo->m_sMember[i].m_bOnline;
				sItem[i].nSaveEff = pInfo->m_sMember[i].m_nManagerEff[i];
				sItem[i].nJoinTm = pInfo->m_sMember[i].m_nManagerJoinTm[i];
			}

			CoreDataChanged(GDCNI_TONG_MEMBER_LIST, (unsigned int)&sObj, (unsigned int)sItem);
		}
		break;
	case enumTONG_SYNC_ID_MEMBER_INFO:
		{
			TONG_MEMBER_INFO_SYNC	*pInfo = (TONG_MEMBER_INFO_SYNC*)pMsg;

			// 通知界面得到某帮会帮众信息
			KUiGameObjectWithName	sObj;
			strcpy(sObj.szName, pInfo->m_szTongName);
			sObj.nData = enumTONG_FIGURE_MEMBER;
			sObj.nParam = pInfo->m_btStateNo;
			sObj.uParam = pInfo->m_btCurNum;
			sObj.szString[0] = 0;

			KTongMemberItem	sItem[defTONG_ONE_PAGE_MAX_NUM];
			memset(sItem, 0, sizeof(sItem));
			for (int i = 0; i < pInfo->m_btCurNum; i++)
			{
				sItem[i].nData = enumTONG_FIGURE_MEMBER;
				strcpy(sItem[i].Name, pInfo->m_sMember[i].m_szName);
				if (pInfo->m_sMember[i].m_btSex)
					strcpy(sItem[i].szAgname, pInfo->m_szFemaleAgname);
				else
					strcpy(sItem[i].szAgname, pInfo->m_szMaleAgname);
				if (!sItem[i].szAgname[0])
					strcpy(sItem[i].szAgname, pInfo->m_szAgname);
				sItem[i].bOnline = pInfo->m_sMember[i].m_bOnline;
				sItem[i].nSaveEff = pInfo->m_sMember[i].m_nMemberEff;
				sItem[i].nJoinTm = pInfo->m_sMember[i].m_nMemberJoinTm;
			}

			CoreDataChanged(GDCNI_TONG_MEMBER_LIST, (unsigned int)&sObj, (unsigned int)sItem);
		}
		break;
	case enumTONG_SYNC_ID_INSTATE:
		{
			TONG_INSTATE_SYNC	*pInstate = (TONG_INSTATE_SYNC*)pMsg;
			// 通知界面任命是否成功
			KUiGameObjectWithName	sUi;
			strcpy(sUi.szName, pInstate->m_szName);
			sUi.nData = TONG_ACTION_ASSIGN;
			sUi.nParam = pInstate->m_btNewFigure;
			sUi.uParam = pInstate->m_btOldFigure;
			strcpy(sUi.szString, pInstate->m_szAgname);
			CoreDataChanged(GDCNI_TONG_ACTION_RESULT, (unsigned int)&sUi, pInstate->m_btSuccessFlag);
		}
		break;
	case enumTONG_SYNC_ID_KICK:
		{
			TONG_KICK_SYNC	*pKick = (TONG_KICK_SYNC*)pMsg;
			// 通知界面踢人是否成功
			KUiGameObjectWithName	sUi;
			strcpy(sUi.szName, pKick->m_szName);
			sUi.nData = TONG_ACTION_DISMISS;
			sUi.nParam = pKick->m_btFigure;
			sUi.uParam = pKick->m_btPos;
			sUi.szString[0] = 0;
			CoreDataChanged(GDCNI_TONG_ACTION_RESULT, (unsigned int)&sUi, pKick->m_btSuccessFlag);
		}
		break;
	case enumTONG_SYNC_ID_CHANGE_MASTER_FAIL:
		{
			TONG_CHANGE_MASTER_FAIL_SYNC	*pFail = (TONG_CHANGE_MASTER_FAIL_SYNC*)pMsg;

			switch (pFail->m_btFailID)
			{
			case 0:		// 对方不在线
				{
					KSystemMessage	sMsg;
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					sprintf(sMsg.szMessage, MSG_TONG_CHANGE_MASTER_ERROR1);
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				}
				break;
			case 1:		// 对方能力不够！
				{
					KSystemMessage	sMsg;
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					sprintf(sMsg.szMessage, MSG_TONG_CHANGE_MASTER_ERROR2);
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				}
				break;
			case 2:		// 名字不对！
				{
					KSystemMessage	sMsg;
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					sprintf(sMsg.szMessage, MSG_TONG_CHANGE_MASTER_ERROR3);
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				}
				break;

			default:
				break;
			}
		}
		break;
	case enumTONG_SYNC_ID_CHANGE_AGNAME_FAIL:
		{
			TONG_CHANGE_AGNAME_FAIL_SYNC	*pFail = (TONG_CHANGE_AGNAME_FAIL_SYNC*)pMsg;
			
			switch (pFail->m_btFailID)
			{
			case 0:		// 对方不在线
				{
					KSystemMessage	sMsg;
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					sprintf(sMsg.szMessage, MSG_TONG_CHANGE_AGNAME_ERROR1);
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				}
				break;
			case 1:		// 对方能力不够！
				{
					KSystemMessage	sMsg;
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					sprintf(sMsg.szMessage, MSG_TONG_CHANGE_AGNAME_ERROR2);
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				}
				break;
			case 2:		// 名字不对！
				{
					KSystemMessage	sMsg;
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					sprintf(sMsg.szMessage, MSG_TONG_CHANGE_AGNAME_ERROR3);
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				}
				break;
			default:
				break;
			}
		}
		break;
	case enumTONG_SYNC_ID_CHANGE_CAMP_FAIL:
		{
			TONG_CHANGE_CAMP_FAIL_SYNC *pFail = (TONG_CHANGE_CAMP_FAIL_SYNC*)pMsg;

			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;						
			switch (pFail->m_btFailID)
			{
			case enumTONG_CHANGE_CAMP_ERROR_ID1:
				break;
			case enumTONG_CHANGE_CAMP_ERROR_ID2:
				break;
			case enumTONG_CHANGE_CAMP_ERROR_ID3:
				break;
			case enumTONG_CHANGE_CAMP_ERROR_ID4:
				break;
			case enumTONG_CHANGE_CAMP_ERROR_ID5:
				break;
			case enumTONG_CHANGE_CAMP_ERROR_ID6:
				sprintf(sMsg.szMessage, MSG_TONG_CHANGE_CAMP_ERROR1);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			case enumTONG_CHANGE_CAMP_ERROR_ID7:
				sprintf(sMsg.szMessage, MSG_TONG_CHANGE_CAMP_ERROR2);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			case enumTONG_CHANGE_CAMP_ERROR_ID8:
				sprintf(sMsg.szMessage, MSG_TONG_CHANGE_CAMP_ERROR3);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			}
		}
		break;
	default:
		break;
	}
}


void KProtocolProcess::s2cChangeWeather(BYTE* pMsg)
{
	g_ScenePlace.ChangeWeather(((SYNC_WEATHER*)pMsg)->WeatherID);
}

void	KProtocolProcess::s2cPKSyncNormalFlag(BYTE* pMsg)
{
	PK_NORMAL_FLAG_SYNC	*pFlag = (PK_NORMAL_FLAG_SYNC*)pMsg;
	Player[CLIENT_PLAYER_INDEX].m_cPK.SetNormalPKState(pFlag->m_btFlag);
}

void	KProtocolProcess::s2cPKSyncEnmityState(BYTE* pMsg)
{
	PK_ENMITY_STATE_SYNC	*pState = (PK_ENMITY_STATE_SYNC*)pMsg;
	char	szName[32];

	memset(szName, 0, sizeof(szName));
	memcpy(szName, pState->m_szName, pState->m_wLength + 1 + sizeof(pState->m_szName) - sizeof(PK_ENMITY_STATE_SYNC));
	Player[CLIENT_PLAYER_INDEX].m_cPK.SetEnmityPKState(pState->m_btState, pState->m_dwNpcID, szName, pState->m_bAim, pState->m_bSpar);
}

void	KProtocolProcess::s2cPKSyncExerciseState(BYTE* pMsg)
{
	PK_EXERCISE_STATE_SYNC	*pState = (PK_EXERCISE_STATE_SYNC*)pMsg;
	char	szName[32];
	memset(szName, 0, sizeof(szName));
	memcpy(szName, pState->m_szName, pState->m_wLength + 1 + sizeof(pState->m_szName) - sizeof(PK_EXERCISE_STATE_SYNC));
	Player[CLIENT_PLAYER_INDEX].m_cPK.SetExercisePKState(pState->m_btState, pState->m_dwNpcID, szName);
}

void	KProtocolProcess::s2cPKValueSync(BYTE* pMsg)
{
	PK_VALUE_SYNC	*pValue = (PK_VALUE_SYNC*)pMsg;
	Player[CLIENT_PLAYER_INDEX].m_cPK.SetPKValue(pValue->m_nPKValue);
}

void KProtocolProcess::NetCommandSetHorse(BYTE* pMsg)
{
	NPC_HORSE_SYNC *pInfo = (NPC_HORSE_SYNC *)pMsg;

	int nIdx = NpcSet.SearchID(pInfo->m_dwID);

	if (nIdx > 0)
	{
		Npc[nIdx].SwitchRideHorse(pInfo->m_bRideHorse);
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}
}


void	KProtocolProcess::s2cViewEquip(BYTE* pMsg)
{
	g_cViewItem.GetData(pMsg);
}

void	KProtocolProcess::s2cTongCreate(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].m_cTong.Create((TONG_CREATE_SYNC*)pMsg);
}

void	KProtocolProcess::ItemChangeDurability(BYTE* pMsg)
{
	ITEM_DURABILITY_CHANGE	*pIDC = (ITEM_DURABILITY_CHANGE *)pMsg;

	int nIdx = ItemSet.SearchID(pIDC->dwItemID);
	
	if (nIdx)
	{
		Item[nIdx].SetDurability(Item[nIdx].GetDurability() + pIDC->nChange);
		//_ASSERT(Item[nIdx].GetDurability() > 0);

		if(Item[nIdx].GetDurability() == 0)
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 1;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_ITEM_DAMAGED, Item[nIdx].GetName());
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		else if (Item[nIdx].GetDurability() <= Item[nIdx].GetMaxDurability() / ItemSet.m_sRepairParam.nWarningBaseline)
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 1;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_ITEM_NEARLY_DAMAGED, Item[nIdx].GetName());
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
	}
}

void	KProtocolProcess::SendTradeAction(BYTE* pMsg)
{
	PLAYER_TRADE_ACTION_SYNC *pInfo = (PLAYER_TRADE_ACTION_SYNC *)pMsg;
	if (pInfo->bOpen)
		g_cSellItem.ViewTrade(pInfo->dwID);
	else
		g_cSellItem.CancelTrade();
}

void	KProtocolProcess::SendTradeItem(BYTE* pMsg)
{
	g_cSellItem.GetData(pMsg);
}

void	KProtocolProcess::SendTradeCount(BYTE* pMsg)
{
	PLAYER_TRADE *pInfo = (PLAYER_TRADE *)pMsg;
	g_cSellItem.Update(FALSE, pInfo->dwID, pInfo->nParam);
}

void	KProtocolProcess::SendTradeFail(BYTE* pMsg)
{
	PLAYER_TRADE_START_FAIL_SYNC *pTrade = (PLAYER_TRADE_START_FAIL_SYNC *)pMsg;

	KSystemMessage	sMsg;
	
	sprintf(sMsg.szMessage, MSG_TRADE_COUNT);
	sMsg.eType = SMT_NORMAL;
	sMsg.byConfirmType = SMCT_NONE;
	sMsg.byPriority = 1;
	sMsg.byParamSize = 0;
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);	
}

void KProtocolProcess::LadderList(BYTE* pMsg)
{
	LADDER_LIST*	pList = (LADDER_LIST *)pMsg;
	KRankIndex		*pLadderListIndex = NULL;
	
	if (pList->nCount > 0 && pList->nCount < enumLadderEnd)
	{
		pLadderListIndex = new KRankIndex[pList->nCount];
	}
	if (pLadderListIndex)
	{
		for (int i = 0; i < pList->nCount; i++)
		{
			pLadderListIndex[i].usIndexId = pList->dwLadderID[i];
			pLadderListIndex[i].bValueAppened = true;
			pLadderListIndex[i].bSortFlag = true;
		}
		CoreDataChanged(GDCNII_RANK_INDEX_LIST_ARRIVE, pList->nCount, (int)pLadderListIndex);
		delete [] pLadderListIndex;
		pLadderListIndex = NULL;
	}
}

void KProtocolProcess::LadderResult(BYTE* pMsg)
{
	LADDER_DATA*	pLadderData = (LADDER_DATA *)pMsg;
	KRankMessage*	pLadderMessage = NULL;

	pLadderMessage = new KRankMessage[10];
	if (pLadderMessage)
	{
		for (int i = 0; i < 10; i++)
		{
			pLadderMessage[i].usMsgLen = strlen(pLadderData->StatData[i].Name);
			strcpy(pLadderMessage[i].szMsg, pLadderData->StatData[i].Name);
			pLadderMessage[i].nValueAppend = pLadderData->StatData[i].nValue;
			pLadderMessage[i].cSortFlag = (char)pLadderData->StatData[i].bySort;
		}
		unsigned int uParam = 10 | (((WORD)pLadderData->dwLadderID) << 16);
		CoreDataChanged(GDCNII_RANK_INFORMATION_ARRIVE, uParam, (int)pLadderMessage);
		delete [] pLadderMessage;
		pLadderMessage = NULL;
	}
}


void KProtocolProcess::s2cPlayerSync(BYTE* pMsg)
{
	S2C_PLAYER_SYNC *pSync = (S2C_PLAYER_SYNC*)pMsg;
	switch (pSync->m_wMsgID)
	{
		case enumS2C_PLAYERSYNC_ID_EXIT:
			CoreDataChanged(GDCNI_EXIT_GAME, NULL, NULL);
			break;
		case enumS2C_PLAYERSYNC_ID_GIVE:
			CoreDataChanged(GDCNI_GIVE, NULL, NULL);
			break;
		case enumS2C_PLAYERSYNC_ID_EQUIPEXPAND:
			Player[CLIENT_PLAYER_INDEX].SetEquipExpandTime(*(int*)(&pSync->m_lpBuf));
			break;
		case enumS2C_PLAYERSYNC_ID_EXPANDBOX:
			Player[CLIENT_PLAYER_INDEX].SetExpandBoxNum(*(int*)(&pSync->m_lpBuf));
			break;
		case enumS2C_PLAYERSYNC_ID_LOCKSTATE:
			Player[CLIENT_PLAYER_INDEX].SetLockState(*(BOOL*)(&pSync->m_lpBuf));
			break;
		case enumS2C_PLAYERSYNC_ID_PROPPOINT:
			Player[CLIENT_PLAYER_INDEX].m_nAttributePoint = *(int*)(&pSync->m_lpBuf);
			break;
		case enumS2C_PLAYERSYNC_ID_MAGICPOINT:
			Player[CLIENT_PLAYER_INDEX].m_nSkillPoint = *(int*)(&pSync->m_lpBuf);
			CoreDataChanged(GDCNI_FIGHT_SKILL_POINT, 0, Player[CLIENT_PLAYER_INDEX].m_nSkillPoint);
			break;
		case enumS2C_PLAYERSYNC_ID_RANKDATA:
			CoreDataChanged(GDCNI_RANKDATA, NULL, NULL);
			break;
		case enumS2C_PLAYERSYNC_ID_ENCHASE:
			CoreDataChanged(GDCNI_ENCHASE, NULL, NULL);
			break;
		case enumS2C_PLAYERSYNC_ID_INPUT:
			CoreDataChanged(GDCNI_INPUT, *(int*)(&pSync->m_lpBuf), 0);
			break;
		case enumS2C_PLAYERSYNC_ID_MASKFEATURE:
			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SwitchMaskFeature();
			break;
	}
}	

void KProtocolProcess::s2cSyncRankData(BYTE* pMsg)
{
	PLAYER_MISSION_RANKDATA* pInfo = (PLAYER_MISSION_RANKDATA *)pMsg;
	memcpy(&Player[CLIENT_PLAYER_INDEX].m_MissionData, &pInfo->SelfData, sizeof(pInfo->SelfData));
	for(int i = 0; i < MISSION_STATNUM;i++)
		memcpy(&Player[CLIENT_PLAYER_INDEX].m_MissionRank[i], 
					&pInfo->MissionRank[i], sizeof(pInfo->MissionRank[i])); 
}

void 	KProtocolProcess::s2cSyncSuperShop(BYTE* pMsg)
{
	S2C_SUPERSHOP* pInfo = (S2C_SUPERSHOP*)pMsg;
	BuySell.OpenSale(pInfo->m_nSaleType, (BuySellInfo*)&pInfo->m_BuySellInfo);
}

void	KProtocolProcess::s2cSparApplyStart(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].s2cSparApplyStart(pMsg);
}

void KProtocolProcess::s2cLockMove(BYTE* pMsg)
{
	PLAYER_LOCKMOVE	*pLock = (PLAYER_LOCKMOVE*)pMsg;
	
	Player[CLIENT_PLAYER_INDEX].SetLockMove(&pLock->m_LockMove);
}

void KProtocolProcess::s2cChatRoomState(BYTE* pMsg)
{
	PLAYER_SEND_CHATROOM_STATE	*pRoom = (PLAYER_SEND_CHATROOM_STATE*)pMsg;

	switch (pRoom->m_btMsgId)
	{
	case ChatRoom_Create_Success:
		{
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_CHATROOM_CREATE, pRoom->lpszName);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case ChatRoom_Join_Host:
		Player[CLIENT_PLAYER_INDEX].m_cRoom.JoinRoom(pRoom->lpszName, pRoom->nId, CHATROOM_HOST);
		break;
	case ChatRoom_Join_Member:
		Player[CLIENT_PLAYER_INDEX].m_cRoom.JoinRoom(pRoom->lpszName, pRoom->nId, CHATROOM_MEMBER);
		break;
	case ChatRoom_Join_Fail1:
		{
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_CHATROOM_JOIN_FAIL1, pRoom->lpszName);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case ChatRoom_Join_Fail2:
		{
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_CHATROOM_JOIN_FAIL2, pRoom->lpszName);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case ChatRoom_Join_Fail3:
		{
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_CHATROOM_JOIN_FAIL3, pRoom->lpszName);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case ChatRoom_Leave:
		{
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_CHATROOM_SELF_LEAVE, pRoom->lpszName);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			Player[CLIENT_PLAYER_INDEX].m_cRoom.Release();
			CoreDataChanged(GDCNI_CHATROOM_LEAVE, 0, 0);
		}
		break;
	case ChatRoom_Delete:
		{
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_CHATROOM_SELF_DELETE, pRoom->lpszName);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			Player[CLIENT_PLAYER_INDEX].m_cRoom.Release();
			CoreDataChanged(GDCNI_CHATROOM_LEAVE, 0, 0);
		}
		break;
	case ChatRoom_ForceLeave:
		{
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_CHATROOM_SELF_FORCELEAVE, pRoom->lpszName);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			Player[CLIENT_PLAYER_INDEX].m_cRoom.Release();
			CoreDataChanged(GDCNI_CHATROOM_LEAVE, 0, 0);
		}
		break;
	case ChatRoom_BeKick:
		{
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_CHATROOM_SELF_BEKICK, pRoom->lpszName);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			Player[CLIENT_PLAYER_INDEX].m_cRoom.Release();
			CoreDataChanged(GDCNI_CHATROOM_LEAVE, 0, 0);
		}
		break;
	case ChatRoom_AddBlackList:
		{
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_CHATROOM_ADDBLACKLIST, pRoom->lpszName);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case ChatRoom_AddBlackList_Fail:
		{
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_CHATROOM_ADDBLACKLIST_FAIL, pRoom->lpszName);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case ChatRoom_RemoveBlackList:
		{
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_CHATROOM_REMOVEBLACKLIST, pRoom->lpszName);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case ChatRoom_ChangePw:
		{
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_CHATROOM_SELD_CHANGEPW, pRoom->lpszName);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	default:
		break;
	}
	CoreDataChanged(GDCNI_CHATROOM_UPDATE_INTERFACE, 0, 0);
	CoreDataChanged(GDCNI_PLAYER_BASE_INFO, 0, 0);
}

void KProtocolProcess::s2cApplyCreateChatRoomFalse(BYTE* pMsg)
{
	PLAYER_SEND_CREATE_CHATROOM_FALSE *pCreateFalse = (PLAYER_SEND_CREATE_CHATROOM_FALSE*)pMsg;
	KSystemMessage	sMsg;
	
	sMsg.eType = SMT_NORMAL;
	sMsg.byConfirmType = SMCT_NONE;
	sMsg.byPriority = 0;
	sMsg.byParamSize = 0;
	switch (pCreateFalse->m_btErrorID)
	{
	// 已经在队伍中，说明客户端队伍数据有错误，申请重新获得队伍数据
	case ChatRoom_Create_Error_Name:
		sprintf(sMsg.szMessage, MSG_CHATROOM_CREATE_ERROR_NAME);
		break;
	case ChatRoom_Create_Error_Pass:
		sprintf(sMsg.szMessage, MSG_CHATROOM_CREATE_ERROR_PASS);
		break;
	case ChatRoom_Create_Error_SameName:
		sprintf(sMsg.szMessage, MSG_CHATROOM_CREATE_ERROR_SAMENAME);
		break;
	case ChatRoom_Create_Error_SameHost:
		sprintf(sMsg.szMessage, MSG_CHATROOM_CREATE_ERROR_SAMEHOST);
		break;
	case ChatRoom_Create_Error_RoomFull:
		sprintf(sMsg.szMessage, MSG_CHATROOM_CREATE_ERROR_ROOMFULL);
		break;
	default:
		return;
	}
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
}

void KProtocolProcess::s2cRequestChatRoomList(BYTE* pMsg)
{
	PLAYER_REQUEST_CHATROOM_LIST	*pList = (PLAYER_REQUEST_CHATROOM_LIST*)pMsg;

	KOneRoom pRoom;
	strcpy(pRoom.lpszRoomHost, pList->lpszRoomHost);
	strcpy(pRoom.lpszRoomName, pList->lpszRoomName);
	pRoom.byRoomMemNum = pList->byRoomMemNum;
	pRoom.nRoomLeftTime = pList->nRoomLeftTime;
	pRoom.nRoomParam = pList->nRoomParam;
	CoreDataChanged(GDCNI_REQUEST_CHATROOM_LIST, (unsigned int)&pRoom, NULL);
}

void KProtocolProcess::s2cRequestMemberList(BYTE* pMsg)
{
	PLAYER_REQUEST_LIST	*pList = (PLAYER_REQUEST_LIST*)pMsg;
	KOneMem pMem;
	strcpy(pMem.lpszName, pList->lpszName);
	pMem.nMemParam = pList->nMemParam;
	pMem.nMemIndex = pList->nMemIndex;
	pMem.bIsRoomHost = pList->bIsRoomHost;
	CoreDataChanged(GDCNI_REQUEST_MEMBER_LIST, (unsigned int)&pMem, NULL);
}

void KProtocolProcess::s2cRequestBlackList(BYTE* pMsg)
{
	PLAYER_REQUEST_LIST	*pList = (PLAYER_REQUEST_LIST*)pMsg;
	KOneMem pMem;
	strcpy(pMem.lpszName, pList->lpszName);
	pMem.nMemParam = pList->nMemParam;
	pMem.nMemIndex = pList->nMemIndex;
	pMem.bIsRoomHost = pList->bIsRoomHost;
	CoreDataChanged(GDCNI_REQUEST_BLACK_LIST, (unsigned int)&pMem, NULL);
}
#else
void KProtocolProcess::RemoveRole(int nIndex, BYTE * pProtocol)
{

}

void KProtocolProcess::NpcRequestCommand(int nIndex, BYTE* pProtocol)
{
	NPC_REQUEST_COMMAND *pNpcRequestSync = (NPC_REQUEST_COMMAND *)pProtocol;
	NpcSet.SyncNpc(pNpcRequestSync->ID, Player[nIndex].m_nNetConnectIdx);
}

//-------------------------------------------------------------------------
//	功能：客户端向服务器请求更新某个obj数据
//-------------------------------------------------------------------------
void KProtocolProcess::ObjRequestCommand(int nIndex, BYTE* pProtocol)
{
	OBJ_CLIENT_SYNC_ADD	*pObjClientSyncAdd = (OBJ_CLIENT_SYNC_ADD*)pProtocol;
	ObjSet.SyncAdd(pObjClientSyncAdd->m_nID, Player[nIndex].m_nNetConnectIdx);
}

void KProtocolProcess::NpcWalkCommand(int nIndex, BYTE* pProtocol)
{
	NPC_WALK_COMMAND* pNetCommand = (NPC_WALK_COMMAND *)pProtocol;
	int ParamX = pNetCommand->nMpsX;
	int ParamY = pNetCommand->nMpsY;
	if (ParamX < 0)
	{
		ParamX = 0;
	}
	if (ParamY < 0)
	{
		ParamY = 0;
	}
	Npc[Player[nIndex].m_nIndex].SendCommand(do_walk, ParamX, ParamY);
}

void KProtocolProcess::NpcRunCommand(int nIndex, BYTE* pProtocol)
{
	NPC_RUN_COMMAND* pNetCommand = (NPC_RUN_COMMAND *)pProtocol;

	int ParamX = pNetCommand->nMpsX;
	int ParamY = pNetCommand->nMpsY;
	if (ParamX < 0)
	{
		ParamX = 0;
	}
	if (ParamY < 0)
	{
		ParamY = 0;
	}
	Npc[Player[nIndex].m_nIndex].SendCommand(do_run, ParamX, ParamY);
}


void KProtocolProcess::NpcSkillCommand(int nIndex, BYTE* pProtocol)
{
	NPC_SKILL_COMMAND* pNetCommand = (NPC_SKILL_COMMAND *)pProtocol;

	int ParamX = pNetCommand->nSkillID;
	int ParamY = pNetCommand->nMpsX;
	int ParamZ = pNetCommand->nMpsY;
	// 参数合法性检查
	if (ParamX <= 0 || ParamX > MAX_SKILL )
	{
		return ;
	}

	if (ParamZ < 0) 
		return;

	if (ParamY < 0)
	{
		if (ParamY != -1) 
			return;

		int nNpcIndex = Player[nIndex].FindAroundNpc((DWORD)ParamZ);
		if (nNpcIndex > 0)
			Npc[Player[nIndex].m_nIndex].SendCommand(do_skill, ParamX, ParamY, nNpcIndex);
	}
	else
		Npc[Player[nIndex].m_nIndex].SendCommand(do_skill, ParamX, ParamY, ParamZ);
}

void KProtocolProcess::PlayerApplyTeamInfo(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].S2CSendTeamInfo(pProtocol);
}

void KProtocolProcess::PlayerApplyCreateTeam(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].CreateTeam(pProtocol);
}

void KProtocolProcess::PlayerApplyTeamChangeState(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].SetTeamState(pProtocol);
}

void KProtocolProcess::PlayerApplyAddTeam(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].S2CSendAddTeamInfo(pProtocol);
}

void KProtocolProcess::PlayerAcceptTeamMember(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].AddTeamMember(pProtocol);
}

void KProtocolProcess::PlayerApplyLeaveTeam(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].LeaveTeam(pProtocol);
}

void KProtocolProcess::PlayerApplyTeamKickMember(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].TeamKickOne(pProtocol);
}

void KProtocolProcess::PlayerApplyTeamChangeCaptain(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].TeamChangeCaptain(pProtocol);
}

void KProtocolProcess::PlayerApplyTeamDismiss(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].TeamDismiss(pProtocol);
}

void KProtocolProcess::JoinTongReply(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].m_cTong.JoinTongReply(pProtocol);
}

void KProtocolProcess::PlayerApplySetPK(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].SetPK(pProtocol);
}

void KProtocolProcess::PlayerAddBaseAttribute(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].AddBaseAttribute(pProtocol);
}

void KProtocolProcess::PlayerApplyAddSkillPoint(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].AddSkillPoint(pProtocol);
}

void KProtocolProcess::PlayerEatItem(int nIndex, BYTE* pProtocol)
{
	if (Player[nIndex].CheckTrading())
		return;
	Player[nIndex].EatItem(pProtocol);
}

void KProtocolProcess::PlayerPickUpItem(int nIndex, BYTE* pProtocol)
{
	if (Player[nIndex].CheckTrading())
		return;
	Player[nIndex].ServerPickUpItem(pProtocol);
}

void KProtocolProcess::PlayerMoveItem(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].ServerMoveItem(pProtocol);
}

void KProtocolProcess::PlayerSellItem(int nIndex, BYTE* pProtocol)
{
	if (Player[nIndex].CheckTrading())
		return;
	Player[nIndex].SellItem(pProtocol);
}

void KProtocolProcess::PlayerBuyItem(int nIndex, BYTE* pProtocol)
{
	if (Player[nIndex].CheckTrading())
		return;
	Player[nIndex].BuyItem(pProtocol);
}

void KProtocolProcess::PlayerDropItem(int nIndex, BYTE* pProtocol)
{
	if (Player[nIndex].CheckTrading())
		return;
	Player[nIndex].ServerThrowAwayItem(pProtocol);
}

void KProtocolProcess::PlayerSelUI(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].ProcessPlayerSelectFromUI(pProtocol);
}

void	KProtocolProcess::TradeApplyOpen(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].TradeApplyOpen(pProtocol);
}

void	KProtocolProcess::TradeApplyClose(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].TradeApplyClose(pProtocol);
}

void	KProtocolProcess::TradeApplyStart(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].TradeApplyStart(pProtocol);
}

void	KProtocolProcess::TradeMoveMoney(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].TradeMoveMoney(pProtocol);
}

void	KProtocolProcess::TradeDecision(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].TradeDecision(pProtocol);
}

void	KProtocolProcess::DialogNpc(int nIndex, BYTE * pProtocol)
{
	Player[nIndex].DialogNpc(pProtocol)	;
}

void	KProtocolProcess::TeamInviteAdd(int nIndex, BYTE * pProtocol)
{
	Player[nIndex].m_cTeam.InviteAdd(nIndex, (TEAM_INVITE_ADD_COMMAND*)pProtocol);
}

void	KProtocolProcess::ChangeAuraSkill(int nIndex, BYTE * pProtocol)
{
	SKILL_CHANGEAURASKILL_COMMAND * pCommand = (SKILL_CHANGEAURASKILL_COMMAND*) pProtocol;
	Npc[Player[nIndex].m_nIndex].SetAuraSkill(pCommand->m_nAuraSkill);
}

void	KProtocolProcess::TeamReplyInvite(int nIndex, BYTE * pProtocol)
{
	TEAM_REPLY_INVITE_COMMAND	*pReply = (TEAM_REPLY_INVITE_COMMAND*)pProtocol;
	if (!pProtocol ||
		pReply->m_nIndex <= 0 ||
		pReply->m_nIndex >= MAX_PLAYER ||
		Player[pReply->m_nIndex].m_nIndex <= 0)
		return;
	if (pReply->m_btResult && Player[nIndex].m_cTeam.GetCreatTeamFlag() == FALSE)
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_CANNOT_ADD_TEAM;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
		g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		return;
	}
	Player[pReply->m_nIndex].m_cTeam.GetInviteReply(pReply->m_nIndex, nIndex, pReply->m_btResult);
}

//void KProtocolProcess::ReplyPing(int nIndex, BYTE* pProtocol)
//{
//	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
//		return;
//
//	PING_COMMAND PingCmd, *pPingCmd;
//
//	pPingCmd = (PING_COMMAND *)pProtocol;
//
//	PingCmd.ProtocolType = s2c_ping;
//	PingCmd.m_dwTime = pPingCmd->m_dwTime;
//	g_pServer->SendData(Player[nIndex].m_nNetConnectIdx, &PingCmd, sizeof(PING_COMMAND));
//	Player[nIndex].m_uLastPingTime = g_SubWorldSet.GetGameTime();
//}

void KProtocolProcess::NpcSitCommand(int nIndex, BYTE* pProtocol)
{
	NPC_SIT_COMMAND *pSitCmd;

	pSitCmd = (NPC_SIT_COMMAND *)pProtocol;

	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;

	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	if (pSitCmd->m_btSitFlag)
		Npc[Player[nIndex].m_nIndex].SendCommand(do_sit);
	else
		Npc[Player[nIndex].m_nIndex].SendCommand(do_stand);
}


void KProtocolProcess::ObjMouseClick(int nIndex, BYTE* pProtocol)
{
	if (Player[nIndex].CheckTrading())
		return;

	int		nSubWorldIdx, nRegionIdx, nObjIdx;
	int		nPlayerX, nPlayerY, nObjX, nObjY;
	OBJ_MOUSE_CLICK_SYNC 	*pObj = (OBJ_MOUSE_CLICK_SYNC*)pProtocol;

	nSubWorldIdx = Npc[Player[nIndex].m_nIndex].m_SubWorldIndex;
	nRegionIdx = SubWorld[nSubWorldIdx].FindRegion(pObj->m_dwRegionID);
	if (nRegionIdx < 0)
		return;
	nObjIdx = SubWorld[nSubWorldIdx].m_Region[nRegionIdx].FindObject(pObj->m_nObjID);
	if (nObjIdx <= 0)
		return;

	SubWorld[nSubWorldIdx].Map2Mps(
		Npc[Player[nIndex].m_nIndex].m_RegionIndex,
		Npc[Player[nIndex].m_nIndex].m_MapX,
		Npc[Player[nIndex].m_nIndex].m_MapY,
		Npc[Player[nIndex].m_nIndex].m_OffX,
		Npc[Player[nIndex].m_nIndex].m_OffY,
		&nPlayerX,
		&nPlayerY);
	SubWorld[nSubWorldIdx].Map2Mps(
		nRegionIdx,
		Object[nObjIdx].m_nMapX,
		Object[nObjIdx].m_nMapY,
		Object[nObjIdx].m_nOffX,
		Object[nObjIdx].m_nOffY,
		&nObjX,
		&nObjY);
/*
	Obj_Kind_MapObj = 0,		// 地图物件，主要用于地图动画
	Obj_Kind_Body,				// npc 的尸体
	Obj_Kind_Box,				// 宝箱
	Obj_Kind_Item,				// 掉在地上的装备
	Obj_Kind_Money,				// 掉在地上的钱
	Obj_Kind_LoopSound,			// 循环音效
	Obj_Kind_RandSound,			// 随机音效
	Obj_Kind_Light,				// 光源（3D模式中发光的东西）
	Obj_Kind_Door,				// 门类
	Obj_Kind_Trap,				// 陷阱
	Obj_Kind_Prop,				// 小道具，可重生
	Obj_Kind_Num,				// 物件的种类数
*/
	switch (Object[nObjIdx].m_nKind)
	{
	case Obj_Kind_Box:
		if (g_GetDistance(nPlayerX, nPlayerY, nObjX, nObjY) > defMAX_EXEC_OBJ_SCRIPT_DISTANCE)
			break;
		if (Object[nObjIdx].m_nState == OBJ_BOX_STATE_CLOSE)
			Object[nObjIdx].ExecScript(nIndex);
		break;
	case Obj_Kind_Door:
		break;
	case Obj_Kind_Prop:
		if (g_GetDistance(nPlayerX, nPlayerY, nObjX, nObjY) > defMAX_EXEC_OBJ_SCRIPT_DISTANCE)
			break;
		if (Object[nObjIdx].m_nState == OBJ_PROP_STATE_DISPLAY)
			Object[nObjIdx].ExecScript(nIndex);
		break;
	}
}

void KProtocolProcess::StoreMoneyCommand(int nIndex, BYTE* pProtocol)
{
	STORE_MONEY_COMMAND*	pCommand = (STORE_MONEY_COMMAND *)pProtocol;

	if (pCommand->m_byDir)	// 取钱
		Player[nIndex].m_ItemList.ExchangeMoney(room_repository, room_equipment, pCommand->m_dwMoney);
	else					// 存钱
		Player[nIndex].m_ItemList.ExchangeMoney(room_equipment, room_repository, pCommand->m_dwMoney);
}

void KProtocolProcess::NpcReviveCommand(int nIndex, BYTE* pProtocol)
{
//	NPC_REVIVE_COMMAND*		pCommand = (NPC_REVIVE_COMMAND *)pProtocol;
	if(Npc[Player[nIndex].m_nIndex].m_FightMode == enumFightNone)
		return;

	Player[nIndex].Revive(REMOTE_REVIVE_TYPE);
}

void KProtocolProcess::c2sTradeReplyStart(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].c2sTradeReplyStart(pProtocol);
}

void KProtocolProcess::c2sPKApplyChangeNormalFlag(int nIndex, BYTE* pProtocol)
{
	PK_APPLY_NORMAL_FLAG_COMMAND	*pApply = (PK_APPLY_NORMAL_FLAG_COMMAND*)pProtocol;
	Player[nIndex].m_cPK.SetNormalPKState(pApply->m_btFlag);
}

void KProtocolProcess::c2sPKApplyEnmity(int nIndex, BYTE* pProtocol)
{
	PK_APPLY_ENMITY_COMMAND	*pApply = (PK_APPLY_ENMITY_COMMAND*)pProtocol;

	if (pApply->m_bRefuse && pApply->m_bSpar)
	{
		int		nIdx = NpcSet.SearchID(pApply->m_dwNpcID);
		if (nIdx)
		{
			SHOW_MSG_SYNC	sMsg;
			sMsg.ProtocolType = s2c_msgshow;
			sMsg.m_wMsgID = enumMSG_ID_SPAR_REFUSE_APPLY;
			sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
			sMsg.m_lpBuf = (LPVOID)Npc[Player[nIndex].m_nIndex].m_dwID;
			g_pServer->PackDataToClient(Player[Npc[nIdx].GetPlayerIdx()].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
			sMsg.m_lpBuf = 0;
		}
		return;
	}

	if (Player[nIndex].m_nIndex && !Npc[Player[nIndex].m_nIndex].m_FightMode)
	{
		if (pApply->m_bSpar && NpcSet.m_nSparPacific)
		{
		}
		else
		{
			SHOW_MSG_SYNC	sMsg;
			sMsg.ProtocolType = s2c_msgshow;
			sMsg.m_wMsgID = enumMSG_ID_PK_ERROR_1;
			sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
			g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
			return;
		}
	}
	if (pApply->m_dwNpcID <= 0 || pApply->m_dwNpcID >= MAX_NPC)
		return;

	int nNpcIdx = Player[nIndex].FindAroundNpc(pApply->m_dwNpcID);
	if (nNpcIdx <= 0)
		return;

	if (Npc[nNpcIdx].m_Kind != kind_player || Npc[nNpcIdx].GetPlayerIdx() <= 0)
		return;

	Player[nIndex].m_cPK.EnmityPKOpen(Npc[nNpcIdx].GetPlayerIdx(), pApply->m_bSpar);
}

#define		defMAX_VIEW_EQUIP_TIME			30
void	KProtocolProcess::c2sViewEquip(int nIndex, BYTE* pProtocol)
{
	if (g_SubWorldSet.GetGameTime() - Player[nIndex].m_nViewEquipTime < defMAX_VIEW_EQUIP_TIME)
		return;
	Player[nIndex].m_nViewEquipTime = g_SubWorldSet.GetGameTime();

	VIEW_EQUIP_COMMAND	*pView = (VIEW_EQUIP_COMMAND*)pProtocol;
	if (pView->m_dwNpcID == Npc[Player[nIndex].m_nIndex].m_dwID)
		return;
	int nPlayerIdx = Player[nIndex].FindAroundPlayer(pView->m_dwNpcID);
	if (nPlayerIdx <= 0)
		return;
	Player[nPlayerIdx].SendEquipItemInfo(nIndex);
}

void KProtocolProcess::LadderQuery(int nIndex, BYTE* pProtocol)
{
	LADDER_QUERY*	pLQ = (LADDER_QUERY *)pProtocol;
	if (nIndex > 0 && nIndex < MAX_PLAYER)
	{
		int lnID = Player[nIndex].m_nNetConnectIdx;
		if (lnID >= 0)
		{
			LADDER_DATA	LadderData;
			LadderData.ProtocolType = s2c_ladderresult;
			LadderData.dwLadderID = pLQ->dwLadderID;
			void* pData = (void *)Ladder.GetTopTen(LadderData.dwLadderID);
			if (pData)
			{
				memcpy(LadderData.StatData, pData, sizeof(LadderData.StatData));
				g_pServer->PackDataToClient(lnID, &LadderData, sizeof(LadderData));
			}
		}
	}
}

void KProtocolProcess::ItemRepair(int nIndex, BYTE* pProtocol)
{
	ITEM_REPAIR	*pIR = (ITEM_REPAIR *)pProtocol;
	if (nIndex > 0 && nIndex < MAX_PLAYER)
	{
		Player[nIndex].RepairItem(pIR->dwItemID);
	}
}

void KProtocolProcess::TradeSet(int nIndex, BYTE* pProtocol)
{
	PLAYER_TRADE_SET_COMMAND *pInfo = (PLAYER_TRADE_SET_COMMAND *)pProtocol;
	if (nIndex > 0 && nIndex < MAX_PLAYER)
	{ 
		int nIdx = Player[nIndex].m_ItemList.SearchID(pInfo->m_dwID);
		if (nIdx > 0)
		{
			if ((Item[nIdx].GetLock()->IsLock() || 
				Item[nIdx].GetLockTrade()) && 
				pInfo->m_nPrice)
			{
				SHOW_MSG_SYNC	sMsg;
				sMsg.ProtocolType = s2c_msgshow;
				sMsg.m_wMsgID = enumMSG_ID_LOCK_NOT_SETPRICE;
				sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
				if(g_pServer && Player[nIndex].m_nNetConnectIdx != -1)
					g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
				return;
			}
			Item[nIdx].SetTradePrice(pInfo->m_nPrice);
			Player[nIndex].m_ItemList.SyncItem(nIdx);
		}
	}
}

void KProtocolProcess::TradeStart(int nIndex, BYTE* pProtocol)
{
	if (nIndex > 0 && nIndex < MAX_PLAYER)
	{
		Player[nIndex].TradeStart(pProtocol);
	}
}

void KProtocolProcess::TradeView(int nIndex, BYTE* pProtocol)
{
	PLAYER_TRADE_VIEW_COMMAND	*pView = (PLAYER_TRADE_VIEW_COMMAND*)pProtocol;
	if (pView->m_dwNpcID == Npc[Player[nIndex].m_nIndex].m_dwID)
		return;
	int nPlayerIdx = Player[nIndex].FindAroundPlayer(pView->m_dwNpcID);
	if (nPlayerIdx <= 0)
		return;
	Player[nPlayerIdx].SendTradeItem(nIndex);
}

void KProtocolProcess::TradeBuy(int nIndex, BYTE* pProtocol)
{
	if (Player[nIndex].CheckTrading())
		return;
	
	if (Npc[Player[nIndex].m_nIndex].m_FightMode)
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_FIGHT_MODE_ERROR1;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
		g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		return;
	}

	PLAYER_TRADE_BUY_COMMAND* pPlayer = (PLAYER_TRADE_BUY_COMMAND *)pProtocol;
	int nPlayerIdx = Player[nIndex].FindAroundPlayer(pPlayer->m_dwID);
	
	if (nPlayerIdx <= 0 || !Player[nPlayerIdx].m_PTrade.nTrade)
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_TRADE_CLOSED;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
		g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		return;
	}

	if (Player[nPlayerIdx].m_ItemList.SearchID(Item[pPlayer->m_nIdx].GetID()) <= 0)
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_TRADE_SELLED;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
		g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		return;
	}

	int nPrice = Item[pPlayer->m_nIdx].GetTradePrice();

	if (Player[nIndex].m_ItemList.GetEquipmentMoney() < nPrice)
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_SHOP_NO_MONEY;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
		g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		return;
	}

	int		p,x, y;
	if (Player[nIndex].m_ItemList.CheckCanPlaceInEquipment(Item[pPlayer->m_nIdx].GetWidth(), Item[pPlayer->m_nIdx].GetHeight(), &p, &x, &y))
	{
		Player[nPlayerIdx].m_ItemList.Remove(pPlayer->m_nIdx);
		int nIdx = ItemSet.Add(&Item[pPlayer->m_nIdx]);
		Player[nIndex].m_ItemList.Add(nIdx, p, x, y, false);
		Player[nIndex].Pay(nPrice);

		Player[nPlayerIdx].Earn(nPrice);
		Player[nPlayerIdx].SendTradeCount(nIndex);
	}
	else
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_SHOP_NO_ROOM;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
		g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		return;
	}
}

void KProtocolProcess::TradeEnd(int nIndex, BYTE* pProtocol)
{
	if (nIndex > 0 && nIndex < MAX_PLAYER)
	{		
		Player[nIndex].m_PTrade.Release();
	}
}

void KProtocolProcess::NpcHorseCommand(int nIndex, BYTE* pProtocol)
{
	NPC_HORSE_COMMAND *pHorseCmd;

	pHorseCmd = (NPC_HORSE_COMMAND *)pProtocol;
	
	int nIdx = Player[nIndex].m_ItemList.m_EquipItem[itempart_horse];

	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;

	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	if (Npc[Player[nIndex].m_nIndex].CanSwitchRideHorse())
	{
		Npc[Player[nIndex].m_nIndex].SwitchRideHorse(!Npc[Player[nIndex].m_nIndex].m_bRideHorse);
	}
}


void KProtocolProcess::PlayerCommand(int nIndex, BYTE* pProtocol)
{
	PLAYER_COMMAND	*pCommand = (PLAYER_COMMAND*)pProtocol;
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;

	switch (pCommand->m_wMsgID)
	{
	case enumC2S_PLAYERCOMMAND_ID_LOCKSTATE:
		Player[nIndex].SetLockState(TRUE);
		break;
	case enumC2S_PLAYERCOMMAND_ID_GIVE:
		{
			if (*(BOOL*)(&pCommand->m_lpBuf))
			{
				if (Player[nIndex].m_szTaskExcuteFun[0])
					PlayerSet.ExecuteScript(nIndex, Player[nIndex].m_dwTaskExcuteScriptId,Player[nIndex].m_szTaskExcuteFun,0);
			}
			Player[nIndex].m_ItemList.BackLocal();//da ra ne

			S2C_PLAYER_SYNC	sMsg;
			sMsg.ProtocolType = s2c_playersync;
			sMsg.m_wLength = sizeof(S2C_PLAYER_SYNC) - 1;
			sMsg.m_lpBuf = 0;
			sMsg.m_wMsgID = enumS2C_PLAYERSYNC_ID_GIVE;
			g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, &sMsg, sizeof(S2C_PLAYER_SYNC));
		}
		break;
	case enumC2S_PLAYERCOMMAND_ID_LOCKITEM:
		{
			int nIdx = Player[nIndex].m_ItemList.SearchID(LOWORD(*(DWORD*)(&pCommand->m_lpBuf)));
			Player[nIndex].m_ItemList.Lock(nIdx, HIWORD(*(int*)(&pCommand->m_lpBuf))>0);
		}
		break;
	case enumC2S_PLAYERCOMMAND_ID_SUPERSHOP:
		Player[nIndex].ExecuteScript(SCRIPT_PROTOCOL_FILE, "PermitSuperShop", 0);
		break;
	case enumC2S_PLAYERCOMMAND_ID_OFFLINE:
		Player[nIndex].ExecuteScript(LOGOUT_SCRIPT, NORMAL_FUNCTION_NAME, 0);
		break;
	case enumC2S_PLAYERCOMMAND_ID_MASKFEATURE:
		Npc[Player[nIndex].m_nIndex].SwitchMaskFeature();
		break;
	case enumC2S_PLAYERCOMMAND_ID_RETURN:
		Player[nIndex].BackToTownPortal();
		break;
	default:
		break;
	}
}

void KProtocolProcess::InputCommand(int nIndex, BYTE* pProtocol)
{
	C2S_BUF_COMMAND *pInfo = (C2S_BUF_COMMAND *)pProtocol;
	if (nIndex > 0 && nIndex < MAX_PLAYER)
	{
		strcpy(Player[nIndex].m_szLastInput, pInfo->szBuf);
		Player[nIndex].ExecuteScript(Player[nIndex].m_dwTaskExcuteScriptId, Player[nIndex].m_szTaskExcuteFun, pInfo->szBuf);
	}
}

void KProtocolProcess::UnlockCommand(int nIndex, BYTE* pProtocol)
{
	C2S_BUF_COMMAND *pInfo = (C2S_BUF_COMMAND *)pProtocol;
	if (nIndex > 0 && nIndex < MAX_PLAYER)
	{
		Player[nIndex].CheckSavePw(pInfo->szBuf);
	}
}

void KProtocolProcess::BreakCommand(int nIndex, BYTE* pProtocol)
{
	PLAYER_BREAK_COMMAND *pInfo = (PLAYER_BREAK_COMMAND *)pProtocol;
	if (nIndex > 0 && nIndex < MAX_PLAYER)
	{
		if (pInfo->nNum)
			Player[nIndex].PlayerBreakItem(pInfo->dwItemID,pInfo->nNum,pInfo->bIsBreakAll);
		else
			Player[nIndex].AutoLoseItem(pInfo->dwItemID);
	}
}

void	KProtocolProcess::SparApplyStart(int nIndex, BYTE* pProtocol)
{
	if (nIndex > 0 && nIndex < MAX_PLAYER)
		Player[nIndex].SparApplyStart(pProtocol);
}

void	KProtocolProcess::LockMove(int nIndex, BYTE* pProtocol)
{
	PLAYER_LOCKMOVE *pInfo = (PLAYER_LOCKMOVE *)pProtocol;
	if (nIndex > 0 && nIndex < MAX_PLAYER)
	{
		Player[nIndex].SetLockMove(&pInfo->m_LockMove);
	}
}

void	KProtocolProcess::ChatRoomDecision(int nIndex, BYTE* pProtocol)
{
	PLAYER_CHATROOM_DECISION_COMMAND *pInfo = (PLAYER_CHATROOM_DECISION_COMMAND *)pProtocol;
	if (nIndex > 0 && nIndex < MAX_PLAYER)
	{
		if(pInfo->m_bRoomList)
		{
			if(pInfo->m_nDecision == -1)
				Player[nIndex].m_cRoom.RequestList();
			else if(pInfo->m_nDecision == -2)
				Player[nIndex].m_cRoom.LeaveRoom();
			else if(pInfo->m_nDecision == -3)
				Player[nIndex].m_cRoom.DeleteRoom();
			else
			{
				Player[nIndex].m_cRoom.JoinRoom(KSG_StringGetValue(4,pInfo->m_nDecision,1), KSG_StringGetValue(4,pInfo->m_nDecision,2));
			}
		}
		else
		{
			Player[nIndex].m_cRoom.Roomcommand(pInfo->m_nDecision);
		}
	}
}

void KProtocolProcess::c2sSetImage(int nIndex, BYTE* pProtocol)
{
	CP_DATAU *pSetImgCmd;

	pSetImgCmd = (CP_DATAU *)pProtocol;
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;

	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;
	Player[nIndex].SetNumImg(pSetImgCmd->int_ID);		
}
#endif