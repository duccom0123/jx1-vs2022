#ifndef KProtocolProcessH
#define	KProtocolProcessH

#include "../Headers/KProtocol.h"
class KProtocolProcess
{
private:
#ifndef _SERVER
	void (KProtocolProcess::*ProcessFunc[s2c_end])(BYTE* pMsg);
#else
	void (KProtocolProcess::*ProcessFunc[c2s_end])(int nIndex, BYTE* pMsg);
#endif
public:
	KProtocolProcess();
	~KProtocolProcess();
#ifndef _SERVER
	void ProcessNetMsg(BYTE* pMsg);
#else
	void ProcessNetMsg(int nIndex, BYTE* pMsg);
#endif
private:
#ifdef TOOLVERSION
public:
#endif
#ifndef _SERVER
	void	s2cAccoutLoginResult(BYTE * pMsg);
	void	SyncCurNormalData(BYTE* pMsg);
	void	SyncWorld(BYTE* pMsg);
	void	SyncNpc(BYTE* pMsg);
	void	SyncNpcMin(BYTE* pMsg);
	void	SyncNpcMinPlayer(BYTE* pMsg);
	void	SyncPlayer(BYTE* pMsg);
	void	SyncPlayerMin(BYTE* pMsg);
	void	SyncCurPlayer(BYTE* pMsg);
	void	SyncObjectAdd(BYTE* pMsg);
	void	SyncObjectState(BYTE* pMsg);
	void	SyncObjectDir(BYTE* pMsg);
	void	SyncObjectRemove(BYTE* pMsg);
	void	NetCommandWalk(BYTE* pMsg);
	void	NetCommandRun(BYTE* pMsg);
	void	NetCommandJump(BYTE* pMsg);
	void	NetCommandSkill(BYTE* pMsg);
	void	NetCommandHurt(BYTE* pMsg);
	void	NetCommandDeath(BYTE* pMsg);
	void	NetCommandRemoveNpc(BYTE* pMsg);
	void	NetCommandChgCurCamp(BYTE* pMsg);
	void	NetCommandChgCamp(BYTE* pMsg);
	void	NetCommandSit(BYTE* pMsg);
	void	NetCommandStand(BYTE* pMsg);
	void	NetCommandChat(BYTE* pMsg);
	void	NetCommandSetPos(BYTE* pMsg);
	void	OpenSaleBox(BYTE* pMsg);
	void	OpenSomeBox(BYTE* pMsg);
	void	s2cShowTeamInfo(BYTE* pMsg);
	void	s2cUpdataSelfTeamInfo(BYTE* pMsg);
	void	s2cApplyTeamInfoFalse(BYTE* pMsg);
	void	s2cCreateTeam(BYTE* pMsg);
	void	s2cApplyCreateTeamFalse(BYTE* pMsg);
	void	s2cSetTeamState(BYTE* pMsg);
	void	s2cApplyAddTeam(BYTE* pMsg);
	void	s2cTeamAddMember(BYTE* pMsg);
	void	s2cLeaveTeam(BYTE* pMsg);
	void	s2cTeamChangeCaptain(BYTE* pMsg);
	void	s2cSetFactionData(BYTE* pMsg);
	void	s2cLeaveFaction(BYTE* pMsg);
	void	s2cSetMissionData(BYTE* pMsg);
	void	s2cGetLeadExp(BYTE* pMsg);
	void	s2cGetTeammateLevel(BYTE* pMsg);
	void	s2cLevelUp(BYTE* pMsg);
	void	s2cGetCurAttribute(BYTE* pMsg);
	void	s2cGetSkillLevel(BYTE* pMsg);
	void	s2cSyncAllSkill(BYTE * pMsg);
	void	s2cSyncMoney(BYTE* pMsg);
	void	s2cMoveItem(BYTE* pMsg);
	void	s2cRemoveItem(BYTE* pMsg);
	void	s2cSyncItem(BYTE* pMsg);
	void	s2cSyncMagic(BYTE* pMsg);
	void	SyncScriptAction(BYTE* pMsg);
	void	s2cChatGetApplyAddFriend(BYTE* pMsg);
	void	s2cSyncRoleList(BYTE* pMsg);
	void	SyncEnd(BYTE* pMsg);
	void	s2cTradeChangeState(BYTE* pMsg);
	void	s2cNpcSetMenuState(BYTE* pMsg);
	void	s2cTradeMoneySync(BYTE* pMsg);
	void	s2cTradeDecision(BYTE* pMsg);
	void	s2cPlayerExp(BYTE* pMsg);
	void	s2cTeamInviteAdd(BYTE * pMsg);
	void	s2cTradePressOkSync(BYTE * pMsg);
	void	s2cPing(BYTE* pMsg);
	void	s2cDirectlyCastSkill(BYTE * pMsg);
	void	s2cShowMsg(BYTE * pMsg);
	void	SyncStateEffect(BYTE* pMsg);
	void	IgnoreState(BYTE* pMsg);
	void	PlayerRevive(BYTE* pMsg);
	void	RequestNpcFail(BYTE* pMsg);
	void	s2cTradeApplyStart(BYTE* pMsg);
	void	s2cCreateTong(BYTE* pMsg);
	void	s2cJoinTong(BYTE* pMsg);
	void	s2cJoinTongReply(BYTE* pMsg);
	void	s2cItemAutoMove(BYTE* pMsg);
	void	s2cChangeWeather(BYTE* pMsg);
	void	s2cPKSyncNormalFlag(BYTE* pMsg);
	void	s2cPKSyncEnmityState(BYTE* pMsg);
	void	s2cPKSyncExerciseState(BYTE* pMsg);
	void	s2cPKValueSync(BYTE* pMsg);
	void	NpcSleepSync(BYTE* pMsg);
	void	s2cViewEquip(BYTE* pMsg);
	void	LadderResult(BYTE* pMsg);
	void	LadderList(BYTE* pMsg);
	void	s2cTongCreate(BYTE* pMsg);
	void	ServerReplyClientPing(BYTE* pMsg);
	void	ItemChangeDurability(BYTE* pMsg);
	void	SendTradeAction(BYTE* pMsg);
	void	SendTradeItem(BYTE* pMsg);
	void	SendTradeCount(BYTE* pMsg);
	void	SendTradeFail(BYTE* pMsg);	
	void	NetCommandSetRank(BYTE* pMsg);
	void	NetCommandSetExpandRank(BYTE* pMsg);
	void	NetCommandSetHorse(BYTE* pMsg);
	void	s2cSyncTaskValue(BYTE* pMsg);
	void	s2cPlayerSync(BYTE* pMsg);
	void	SetExtPoint(BYTE* pMsg);
	void	SyncGive(BYTE* pMsg);
	void	SyncMaskLock(BYTE* pMsg);
	void	SyncDataRes(BYTE* pMsg);
	void	s2cSyncRankData(BYTE* pMsg);
	void	s2cSyncSuperShop(BYTE* pMsg);
	void	s2cSparApplyStart(BYTE* pMsg);
	void	s2cLockMove(BYTE* pMsg);
	void	s2cChatRoomState(BYTE* pMsg);
	void	s2cApplyCreateChatRoomFalse(BYTE* pMsg);
	void	s2cRequestChatRoomList(BYTE* pMsg);
	void	s2cRequestMemberList(BYTE* pMsg);
	void	s2cRequestBlackList(BYTE* pMsg);
	void	s2cOpenMake(BYTE* pMsg);

	void	s2cExtend(BYTE* pMsg);
	void	s2cExtendChat(BYTE* pMsg);
	void	s2cExtendFriend(BYTE* pMsg);
	void	s2cExtendTong(BYTE* pMsg);
	

#else
	void	RemoveRole(int nIndex, BYTE * pProtocol);
	void	NpcRequestCommand(int nIndex, BYTE* pMsg);
	void	ObjRequestCommand(int nIndex, BYTE* pProtocol);
	void	NpcWalkCommand(int nIndex, BYTE* pProtocol);
	void	NpcRunCommand(int nIndex, BYTE* pProtocol);
	void	NpcSkillCommand(int nIndex, BYTE* pProtocol);
	void	PlayerApplyTeamInfo(int nIndex, BYTE* pProtocol);
	void	PlayerApplyCreateTeam(int nIndex, BYTE* pProtocol);
	void	PlayerApplyTeamChangeState(int nIndex, BYTE* pProtocol);
	void	PlayerApplyAddTeam(int nIndex, BYTE* pProtocol);
	void	PlayerAcceptTeamMember(int nIndex, BYTE* pProtocol);
	void	PlayerApplyLeaveTeam(int nIndex, BYTE* pProtocol);
	void	PlayerApplyTeamKickMember(int nIndex, BYTE* pProtocol);
	void	PlayerApplyTeamChangeCaptain(int nIndex, BYTE* pProtocol);
	void	PlayerApplyTeamDismiss(int nIndex, BYTE* pProtocol);
	void	PlayerApplySetPK(int nIndex, BYTE* pProtocol);
	void	PlayerSendChat(int nIndex, BYTE* pProtocol);
	void	PlayerAddBaseAttribute(int nIndex, BYTE* pProtocol);
	void	PlayerApplyAddSkillPoint(int nIndex, BYTE* pProtocol);
	void	PlayerEatItem(int nIndex, BYTE* pProtocol);
	void	PlayerPickUpItem(int nIndex, BYTE* pProtocol);
	void	PlayerMoveItem(int nIndex, BYTE* pProtocol);
	void	PlayerSellItem(int nIndex, BYTE* pProtocol);
	void	PlayerBuyItem(int nIndex, BYTE* pProtocol);
	void	PlayerDropItem(int nIndex, BYTE* pProtocol);
	void	PlayerSelUI(int nIndex, BYTE* pProtocol);
	void	TradeApplyOpen(int nIndex, BYTE* pProtocol);
	void	TradeApplyClose(int nIndex, BYTE* pProtocol);
	void	TradeApplyStart(int nIndex, BYTE* pProtocol);
	void	TradeMoveMoney(int nIndex, BYTE* pProtocol);
	void	TradeDecision(int nIndex, BYTE* pProtocol);
	void	DialogNpc(int nIndex, BYTE * pProtocol);
	void	TeamInviteAdd(int nIndex, BYTE * pProtocol);
	void	ChangeAuraSkill(int nIndex, BYTE * pProtocol);
	void	TeamReplyInvite(int nIndex, BYTE * pProtocol);
	//void	ReplyPing(int nIndex, BYTE* pProtocol);
	void	NpcSitCommand(int nIndex, BYTE* pProtocol);
	void	ObjMouseClick(int nIndex, BYTE* pProtocol);
	void	StoreMoneyCommand(int nIndex, BYTE* pProtocol);
	void	NpcReviveCommand(int nIndex, BYTE* pProtocol);
	void	c2sTradeReplyStart(int nIndex, BYTE* pProtocol);
	void	c2sPKApplyChangeNormalFlag(int nIndex, BYTE* pProtocol);
	void	c2sPKApplyEnmity(int nIndex, BYTE* pProtocol);
	void	c2sViewEquip(int nIndex, BYTE* pProtocol);
	void	LadderQuery(int nIndex, BYTE* pProtocol);
	void	ItemRepair(int nIndex, BYTE* pProtocol);
	void	TradeSet(int nIndex, BYTE* pProtocol);
	void	TradeStart(int nIndex, BYTE* pProtocol);
	void	TradeView(int nIndex, BYTE* pProtocol);
	void	TradeBuy(int nIndex, BYTE* pProtocol);
	void	TradeEnd(int nIndex, BYTE* pProtocol);	
	void	NpcHorseCommand(int nIndex, BYTE* pProtocol);
	void	PlayerCommand(int nIndex, BYTE* pProtocol);
	void	InputCommand(int nIndex, BYTE* pProtocol);
	void	UnlockCommand(int nIndex, BYTE* pProtocol);
	void	BreakCommand(int nIndex, BYTE* pProtocol);
	void	JoinTongReply(int nIndex, BYTE* pProtocol);
	void	SparApplyStart(int nIndex, BYTE* pProtocol);
	void	LockMove(int nIndex, BYTE* pProtocol);
	void	ChatRoomDecision(int nIndex, BYTE* pProtocol);
	void	c2sSetImage(int nIndex, BYTE* pProtocol);
#endif
};

extern KProtocolProcess g_ProtocolProcess;
#endif
