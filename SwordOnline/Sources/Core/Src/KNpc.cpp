//-----------------------------------------------------------------------
//	Sword3 KNpc.cpp
//-----------------------------------------------------------------------
#include "KCore.h"
//#include <crtdbg.h>
#include "KNpcAI.h"
#include "KSkills.h"
#include "KObj.h"
#include "KObjSet.h"
#include "KMath.h"
#include "KPlayer.h"
#include "KPlayerSet.h"
#include "KNpc.h"
#include "GameDataDef.h"
#include "KSubWorldSet.h"
#include "KRegion.h"
#include "KTaskFuns.h"
#include "KNpcTemplate.h"
#include "KNpcRes.h"
#include "KItemSet.h"
#include "KItemChangeRes.h"
#include "KTongData.h"
#ifdef _SERVER
//#include "KNetServer.h"
//#include "../MultiServer/Heaven/Interface/iServer.h"
#include "KPlayerSet.h"
#include "KSkillManager.h"
#include "../../../lib/S3DBInterface.h"
#else
#include "../../Headers/IClient.h"
#include "CoreShell.h"
#include "Scene/KScenePlaceC.h"
#include "KIme.h"
#include "../../Represent/iRepresent/iRepresentshell.h"
#include "ImgRef.h"
#include "Text.h"
#include "KOption.h"
#endif
#include "KNpcAttribModify.h"
#include "CoreUseNameDef.h"
#include "KSubWorld.h"
#include "Scene/ObstacleDef.h"
#include "KThiefSkill.h"
#ifdef _STANDALONE
#include "KThiefSkill.cpp"
#endif

#ifndef max
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#endif

#define	ATTACKACTION_EFFECT_PERCENT		60	// �����ܶ�����ɰٷ�֮���ٲ�����������
#define	MIN_DOMELEE_RANGE				20
#define	MIN_BLURMOVE_SPEED				1
#define	ACCELERATION_OF_GRAVITY			10

#define		SHOW_CHAT_WIDTH				24
#define		SHOW_CHAT_COLOR				0xffffffff

#define		BLOOD_EVENTTIME				400
#define		BLOOD_MOVESPEED				1
#define		BLOOD_FONTSIZE				16

#define		SHOW_LIFE_WIDTH				38
#define		SHOW_LIFE_HEIGHT			3

#define		SHOW_SPACE_HEIGHT			5
//-----------------------------------------------------------------------
#define	STAMINA_RECOVER_SCALE	4
// ����Ŀ��ߣ����ӵ�λ��
#define	REGIONWIDTH			SubWorld[m_SubWorldIndex].m_nRegionWidth
#define	REGIONHEIGHT		SubWorld[m_SubWorldIndex].m_nRegionHeight
// ���ӵĿ��ߣ����ص�λ���Ŵ���1024����
#define	CELLWIDTH			(SubWorld[m_SubWorldIndex].m_nCellWidth << 10)
#define	CELLHEIGHT			(SubWorld[m_SubWorldIndex].m_nCellHeight << 10)
// ��ǰ����
#define	CURREGION			SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex]
// �������������
#define	LEFTREGIONIDX		CURREGION.m_nConnectRegion[2]
#define	RIGHTREGIONIDX		CURREGION.m_nConnectRegion[6]
#define	UPREGIONIDX			CURREGION.m_nConnectRegion[4]
#define	DOWNREGIONIDX		CURREGION.m_nConnectRegion[0]
#define	LEFTUPREGIONIDX		CURREGION.m_nConnectRegion[3]
#define	LEFTDOWNREGIONIDX	CURREGION.m_nConnectRegion[1]
#define	RIGHTUPREGIONIDX	CURREGION.m_nConnectRegion[5]
#define	RIGHTDOWNREGIONIDX	CURREGION.m_nConnectRegion[7]

#define	LEFTREGION			SubWorld[m_SubWorldIndex].m_Region[LEFTREGIONIDX]
#define	RIGHTREGION			SubWorld[m_SubWorldIndex].m_Region[RIGHTREGIONIDX]
#define	UPREGION			SubWorld[m_SubWorldIndex].m_Region[UPREGIONIDX]
#define	DOWNREGION			SubWorld[m_SubWorldIndex].m_Region[DOWNREGIONIDX]
#define	LEFTUPREGION		SubWorld[m_SubWorldIndex].m_Region[LEFTUPREGIONIDX]
#define	LEFTDOWNREGION		SubWorld[m_SubWorldIndex].m_Region[LEFTDOWNREGIONIDX]
#define	RIGHTUPREGION		SubWorld[m_SubWorldIndex].m_Region[RIGHTUPREGIONIDX]
#define	RIGHTDOWNREGION		SubWorld[m_SubWorldIndex].m_Region[RIGHTDOWNREGIONIDX]

#define	CONREGION(x)		SubWorld[m_SubWorldIndex].m_Region[CURREGION.m_nConnectRegion[x]]
#define	CONREGIONIDX(x)		CURREGION.m_nConnectRegion[x]
// ��ǰ����
#define BROADCAST_REGION(pBuff,uSize,uMaxCount)	 if(m_SubWorldIndex >= 0 && m_SubWorldIndex < MAX_SUBWORLD && SubWorld[m_SubWorldIndex].m_SubWorldID != -1) SubWorld[m_SubWorldIndex].BroadCastRegion((pBuff), (uSize), (uMaxCount), m_RegionIndex, m_MapX, m_MapY);
//-----------------------------------------------------------------------
// Npc[0]������Ϸ������ʹ�ã���Ϊһ��NpcSet���������µ�NPC��
KNpc	Npc[MAX_NPC];


KNpcTemplate	* g_pNpcTemplate[MAX_NPCSTYLE]; //0,0Ϊ���

//-----------------------------------------------------------------------

KNpc::KNpc()
{
#ifdef _SERVER
	m_AiSkillRadiusLoadFlag = 0;	// ֻ��Ҫ�ڹ����ʱ���ʼ��һ��
#endif
	Init();
}

void KNpc::Init()
{
	memset(m_btStateInfo, 0, sizeof(m_btStateInfo));
	m_dwID = 0;
	m_Index = 0;
	m_nPlayerIdx = 0;
	m_ProcessAI = 1;
	m_Kind = kind_normal;
	m_Series = series_metal;
	m_btSpecial = npc_normal;
	m_Camp = camp_free;
	m_CurrentCamp = camp_free;
	m_Doing = do_stand;
	m_Height = 0;
	m_Frames.nCurrentFrame = 0;
	m_Frames.nTotalFrame = 0;
	m_SubWorldIndex = 0;
	m_RegionIndex = -1;
	m_Experience = 0;
	m_ActiveSkillID = 0;
	m_SkillParam1 = 0;
	m_SkillParam2 = 0;

	m_bNpcRemoveDeath = FALSE;
	m_nNpcTimeout = 0;
	ZeroMemory(m_nNpcParam, sizeof(m_nNpcParam));
	m_bNpcFollowFindPath = FALSE;
	m_uFindPathTime = 0;
	m_uFindPathMaxTime = 0;
	m_uLastFindPathTime = 0;

#ifndef _SERVER
	m_ClientDoing = cdo_stand;
	m_nChatContentLen = 0;
	m_nCurChatTime = 0;
	m_nChatNumLine = 0;
	m_nChatFontWidth = 0;
	m_nStature = 0;
	m_dwTongNameID = 0;
	ZeroMemory(m_szTongName,sizeof(m_szTongName));
	ZeroMemory(m_szTongAgname,sizeof(m_szTongAgname));
	m_nTongNationalEmblem = 0;
	m_nFigure = -1;
	m_nTeamServerID = -1;
#endif

	m_CurrentLife = 100;			// Npc�ĵ�ǰ����
	m_CurrentLifeMax = 100;		// Npc�ĵ�ǰ�������ֵ
	m_CurrentLifeReplenish = 0;	// Npc�ĵ�ǰ�����ظ��ٶ�
	m_CurrentLifeReplenishPercent = 0;
	m_CurrentMana = 100;			// Npc�ĵ�ǰ����
	m_CurrentManaMax = 100;		// Npc�ĵ�ǰ�������
	m_CurrentManaReplenish = 0;	// Npc�ĵ�ǰ�����ظ��ٶ�
	m_CurrentStamina = 100;		// Npc�ĵ�ǰ����
	m_CurrentStaminaMax = 100;	// Npc�ĵ�ǰ�������
	m_CurrentStaminaGain = 0;	// Npc�ĵ�ǰ�����ظ��ٶ�
	m_CurrentStaminaLoss = 0;	// Npc�ĵ�ǰ�����½��ٶ�
	m_CurrentAttackRating = 100;	// Npc�ĵ�ǰ������
	m_CurrentDefend = 10;		// Npc�ĵ�ǰ����
	m_CurrentWalkSpeed = 5;		// Npc�ĵ�ǰ�߶��ٶ�
	m_CurrentRunSpeed = 10;		// Npc�ĵ�ǰ�ܶ��ٶ�
	m_CurrentJumpSpeed = 12;	// Npc�ĵ�ǰ��Ծ�ٶ�
	m_CurrentJumpFrame = 40;	// Npc�ĵ�ǰ��Ծʱ��
	m_CurrentAttackSpeed = 0;	// Npc�ĵ�ǰ�����ٶ�
	m_CurrentCastSpeed = 0;		// Npc�ĵ�ǰʩ���ٶ�
	m_CurrentVisionRadius = 40;	// Npc�ĵ�ǰ��Ұ��Χ
	m_CurrentAttackRadius = 30;	// Npc�ĵ�ǰ������Χ
	m_CurrentHitRecover = 0;	// Npc�ĵ�ǰ�ܻ��ظ��ٶ�
	m_CurrentAddPhysicsDamage = 0;	// Npc�ĵ�ǰ�����˺�ֱ�Ӽӵĵ���
	m_CurrentAddPhysicsMagic = 0;
	
	m_Dir = 0;					// Npc�ķ���
	m_JumpStep = 0;
	m_JumpDir = 0;			
	m_MapZ = 0;					// Npc�ĸ߶�
	m_HelmType = 0;				// Npc��ͷ������
	m_ArmorType = 0;			// Npc�Ŀ�������
	m_WeaponType = 0;			// Npc����������
	m_HorseType = -1;			// Npc����������
	m_bRideHorse = FALSE;		// Npc�Ƿ�����
	m_dwNextSwitchHorseTime = 0;
	m_MaskType = 0;					// Npc ��߹���
	m_bMaskFeature = FALSE;
	m_MantleType = 0;
	m_nPKFlag = enumPKNormal;	
	m_nMissionGroup = -1;

	ZeroMemory(Name, sizeof(Name));		// Npc������
	ZeroMemory(Owner, sizeof(Owner));		// Npc������
	ZeroMemory(MateName, sizeof(MateName));		// Npc������

	m_NpcSettingIdx = 0;		// Npc���趨�ļ�����
	m_CorpseSettingIdx = 0;		// Body���趨�ļ�����
	ZeroMemory(ActionScript, sizeof(ActionScript));
	m_ActionScriptID = 0;
	m_DropScriptID = 0;
	m_TrapScriptID = 0;

	m_RankID					= 0;
	m_ExpandRank.Release();
	m_CurExpandRank.Release();
	m_byTranslife				= 0;
	m_byViprank					= 0;
	m_nRepute					= 0;
	m_nFuYuan					= 0;
	m_nPKValue					= 0;
	m_ImagePlayer				= 0;
	m_byMantleLevel				= 0;
	m_nFactionNumber			= -1;

	m_LifeMax					= 100;		// Npc���������
	m_LifeReplenish				= 0;		// Npc�������ظ��ٶ�
	m_ManaMax					= 100;		// Npc���������
	m_ManaReplenish				= 0;		// Npc�������ظ��ٶ�
	m_StaminaMax				= 100;		// Npc���������
	m_StaminaGain				= 0;		// Npc�������ظ��ٶ�
	m_StaminaLoss				= 0;		// Npc�������½��ٶ�
	m_AttackRating				= 100;		// Npc��������
	m_Defend					= 10;		// Npc�ķ���
	m_WalkSpeed					= 6;		// Npc�������ٶ�
	m_RunSpeed					= 10;		// Npc���ܶ��ٶ�
	m_JumpSpeed					= 12;		// Npc����Ծ�ٶ�
	m_AttackSpeed				= 0;		// Npc�Ĺ����ٶ�
	m_CastSpeed					= 0;		// Npc��ʩ���ٶ�
	m_VisionRadius				= 40;		// Npc����Ұ��Χ
	m_DialogRadius				= 124;		// Npc�ĶԻ���Χ
	m_HitRecover				= 12;		// Npc���ܻ��ظ��ٶ�
	m_nPeopleIdx				= 0;

	m_LoopFrames				= 0;
	m_WalkFrame					= 12;
	m_RunFrame					= 15;
	m_StandFrame				= 15;
	m_DeathFrame				= 15;
	m_HurtFrame					= 10;
	m_AttackFrame				= 20;
	m_CastFrame					= 20;
	m_SitFrame					= 15;
	m_JumpFrame					= 40;
	m_AIMAXTime					= 25;
	m_NextAITime				= 0;
	m_ProcessState				= 1;
	m_ReviveFrame				= 100;
	m_bExchangeServer			= FALSE;
	m_bActivateFlag				= FALSE;
	m_FightMode					= enumFightNone;
	m_OldFightMode				= enumFightNone;

#ifdef _SERVER
	m_nCurPKPunishState			= 0;
	m_bReviveNow			= FALSE;
#else
	m_SyncSignal				= 0;
	m_sClientNpcID.m_dwRegionID	= 0;
	m_sClientNpcID.m_nNo		= -1;
	m_ResDir					= 0;
	m_nSleepFlag				= 0;

	m_btCurBlood = 0;
	memset(m_nBlood, 0, sizeof(m_nBlood));
	memset(m_szBlood, 0, sizeof(m_szBlood));

	m_nHurtHeight = 0;
	m_nHurtDesX = 0;
	m_nHurtDesY = 0;

	m_nPacePercent = 0;

	m_bTongFlag					= 0;
	m_MarkMask = 0;
	m_PTrade.Release();
#endif

	m_nLastPoisonDamageIdx = 0;
	m_nLastDamageIdx = 0;
	m_bHaveLoadedFromTemplate = FALSE;
	m_bClientOnly = FALSE;
}

ISkill* KNpc::GetActiveSkill()
{
	_ASSERT(m_ActiveSkillID < MAX_SKILL);
	if (m_SkillList.GetLevel(m_ActiveSkillID) > 0)
		return g_SkillManager.GetSkill(m_ActiveSkillID, m_SkillList.GetCurrentLevel(m_ActiveSkillID));
	else 
		return NULL;
};

#ifdef _SERVER
void KNpc::SetTempCurrentCamp(int nCamp)
{
	if (Player[m_nPlayerIdx].m_cTeam.m_nFlag)
	{
		if (Player[m_nPlayerIdx].m_cTeam.m_nFigure == TEAM_CAPTAIN)
		{
			Npc[Player[m_nPlayerIdx].m_nIndex].ChangeCurrentCamp(nCamp);
			for (int i = 0; i < g_Team[Player[m_nPlayerIdx].m_cTeam.m_nID].m_nMemNum; i++)
			{
				if (g_Team[Player[m_nPlayerIdx].m_cTeam.m_nID].m_nMember[i] > 0)
					Npc[Player[g_Team[Player[m_nPlayerIdx].m_cTeam.m_nID].m_nMember[i]].m_nIndex].ChangeCurrentCamp(nCamp);
			}
		}
		else
			Npc[Player[m_nPlayerIdx].m_nIndex].ChangeCurrentCamp(
			Npc[Player[g_Team[Player[m_nPlayerIdx].m_cTeam.m_nID].m_nCaptain].m_nIndex].m_CurrentCamp);
	}
}
#endif

void KNpc::SetCurrentCamp(int nCamp)
{
	if (IsPlayer())
	{
#ifdef _SERVER
		if (Player[m_nPlayerIdx].m_bForbidCamp)
			return;
#endif
		if (Player[m_nPlayerIdx].m_cTeam.m_nFlag)
		{
#ifdef _SERVER
			SetTempCurrentCamp(nCamp);
#endif
			return;
		}
		else
			m_CurrentCamp = nCamp;
	}
	else
		m_CurrentCamp = nCamp;

#ifdef _SERVER
	POINT	POff[8] = 
	{
		{0, 32},
		{-16, 32},
		{-16, 0},
		{-16, -32},
		{0, -32},
		{16, -32},
		{16, 0},
		{16, 32},
	};

	if (m_RegionIndex < 0)
		return;

	NPC_CHGCURCAMP_SYNC	NetCommand;
	NetCommand.ProtocolType = (BYTE)s2c_npcchgcurcamp;
	NetCommand.ID = m_dwID;
	NetCommand.Camp = (BYTE)m_CurrentCamp;

	int	nMaxCount = MAX_BROADCAST_COUNT;
	CURREGION.BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX, m_MapY);
	int i = 0;
	for (i = 0; i < 8; i++)
	{
		if (CONREGIONIDX(i) == -1)
			continue;
		CONREGION(i).BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
	}
#endif
}

void KNpc::ChangeCurrentCamp(int nCamp)
{
	m_CurrentCamp = nCamp;

#ifdef _SERVER
	POINT	POff[8] = 
	{
		{0, 32},
		{-16, 32},
		{-16, 0},
		{-16, -32},
		{0, -32},
		{16, -32},
		{16, 0},
		{16, 32},
	};

	if (m_RegionIndex < 0)
		return;

	NPC_CHGCURCAMP_SYNC	NetCommand;
	NetCommand.ProtocolType = (BYTE)s2c_npcchgcurcamp;
	NetCommand.ID = m_dwID;
	NetCommand.Camp = (BYTE)m_CurrentCamp;

	int	nMaxCount = MAX_BROADCAST_COUNT;
	CURREGION.BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX, m_MapY);
	int i = 0;
	for (i = 0; i < 8; i++)
	{
		if (CONREGIONIDX(i) == -1)
			continue;
		CONREGION(i).BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
	}
#endif
}

void KNpc::SetCamp(int nCamp)
{
	m_Camp = nCamp;
#ifdef _SERVER
	POINT	POff[8] = 
	{
		{0, 32},
		{-16, 32},
		{-16, 0},
		{-16, -32},
		{0, -32},
		{16, -32},
		{16, 0},
		{16, 32},
	};
	if (m_RegionIndex < 0)
		return;

	NPC_CHGCAMP_SYNC	NetCommand;

	NetCommand.ProtocolType = (BYTE)s2c_npcchgcamp;
	NetCommand.ID = m_dwID;
	NetCommand.Camp = (BYTE)m_Camp;

	int nMaxCount = MAX_BROADCAST_COUNT;
	CURREGION.BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX, m_MapY);
	int i = 0;
	for (i = 0; i < 8; i++)
	{
		if (CONREGIONIDX(i) == -1)
			continue;
		CONREGION(i).BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
	}
#endif
}

void KNpc::RestoreCurrentCamp()
{
#ifdef _SERVER
	if (IsPlayer() && Player[m_nPlayerIdx].m_bForbidCamp)
		return;
#endif
	m_CurrentCamp = m_Camp;
#ifdef _SERVER
	POINT	POff[8] = 
	{
		{0, 32},
		{-16, 32},
		{-16, 0},
		{-16, -32},
		{0, -32},
		{16, -32},
		{16, 0},
		{16, 32},
	};
	if (m_RegionIndex < 0)
		return;

	NPC_CHGCURCAMP_SYNC	NetCommand;
	
	NetCommand.ProtocolType = (BYTE)s2c_npcchgcurcamp;
	NetCommand.ID = m_dwID;
	NetCommand.Camp = (BYTE)m_CurrentCamp;
	int nMaxCount = MAX_BROADCAST_COUNT;
	CURREGION.BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX, m_MapY);
	int i = 0;
	for (i = 0; i < 8; i++)
	{
		if (CONREGIONIDX(i) == -1)
			continue;
		CONREGION(i).BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
	}
#endif
}

#define		NPC_SHOW_CHAT_TIME		15000
int		IR_IsTimePassed(unsigned int uInterval, unsigned int& uLastTimer);

void KNpc::Activate()
{
	// ���������NPC
	if (!m_Index)
	{
		//g_DebugLog("[DEATH] No Index: %d", m_Index);
		return;
	}

	// �л���ͼ�У�������
	if (m_bExchangeServer)
	{
		//g_DebugLog("[DEATH] Change Server: %d", m_bExchangeServer);
		return;
	}
	// Check here
	if (m_bActivateFlag)
	{
		m_bActivateFlag = FALSE;	// restore flag
		return;
	}
//	m_bActivateFlag = TRUE;

	m_LoopFrames++;
	// Process npc special state, such as curse, etc.
	//g_DebugLog("[DEATH] m_ProcessState: %d", m_ProcessState);
#ifdef _SERVER
	this->CheckTrap(m_MapX, m_MapY);

	if (m_nNpcTimeout && g_SubWorldSet.GetGameTime() >= m_nNpcTimeout)
	{
//	    g_DebugLog("den gio xoa");
		m_nNpcTimeout = 0;
		if (m_ActionScriptID)
		{
			NpcSet.ExecuteScript(m_Index, m_ActionScriptID, "Timeout", m_Index);
		}
	}
    
	this->m_cDeathCalcExp.Active();
#endif

	if (m_ProcessState)
	{
		if (ProcessState())
			return;
	}
	if (m_ProcessAI)
	{
		NpcAI.Activate(m_Index);
	}
	ProcCommand(m_ProcessAI);
	ProcStatus();

#ifndef _SERVER

	if (m_RegionIndex == -1)
		return;

	int		nMpsX, nMpsY;

	if (m_MarkMask)
		GetNpcCopyFromTemplate(m_MaskType);

	m_DataRes.SetAction(m_ClientDoing);
	if(m_bRideHorse && m_ClientDoing == cdo_jump)
		m_DataRes.SetRideHorse(FALSE);
	else
		m_DataRes.SetRideHorse(m_bRideHorse);
	m_DataRes.SetArmor(m_ArmorType, m_MantleType);
	m_DataRes.SetHelm(m_HelmType);
	m_DataRes.SetHorse(m_HorseType);
	m_DataRes.SetWeapon(m_WeaponType);	
	// �������ܲ�����״̬����Ч
	m_DataRes.SetState(m_btStateInfo, &g_NpcResList);

	if (Player[CLIENT_PLAYER_INDEX].m_nIndex == m_Index)
	{
		SubWorld[0].Map2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, &nMpsX, &nMpsY);
		m_DataRes.SetPos(m_Index, nMpsX, nMpsY, m_Height, TRUE);
	}
	else
	{
		SubWorld[0].Map2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, &nMpsX, &nMpsY);
		m_DataRes.SetPos(m_Index, nMpsX, nMpsY, m_Height, FALSE);
	}

	// client npc ʱ�������������������
	if (m_Kind == kind_bird || m_Kind == kind_mouse)
		m_SyncSignal = SubWorld[0].m_dwCurrentTime;

	if (m_nChatContentLen > 0)
	{
		if (IR_GetCurrentTime() - m_nCurChatTime > NPC_SHOW_CHAT_TIME)
		{
			m_nChatContentLen = 0;
			m_nChatNumLine = 0;
			m_nChatFontWidth = 0;
			m_nCurChatTime = 0;
		}
	}

	if(IsPlayer())
	{
		if(m_PTrade.nTrade)
		{
			if(m_Doing != do_sit)
				SendClientCmdSit(true);
		}
	}
#endif
}

void KNpc::ProcStatus()
{
	//g_DebugLog("[DEATH] m_bExchangeServer: %d", m_bExchangeServer);
	if (m_bExchangeServer)
		return;

	switch(m_Doing)
	{
	case do_stand:
		OnStand();
		break;
	case do_run:
		OnRun();
		break;
	case do_walk:
		OnWalk();
		break;
	case do_attack:
	case do_magic:
		OnSkill();
		break;
	case do_sit:
		OnSit();
		break;
	case do_jump:
		OnJump();
		break;
	case do_hurt:
		OnHurt();
		break;
	case do_revive:
		OnRevive();
		break;
	case do_death:
		OnDeath();
		break;
	case do_defense:
		OnDefense();
		break;
	case do_special1:
		OnSpecial1();
		break;
	case do_special2:
		OnSpecial2();
		break;
	case do_special3:
		OnSpecial3();
		break;
	case do_blurmove:
		OnBlurMove();
		break;
	case do_manyattack:
		OnManyAttack();
		break;
	case do_runattack:
		OnRunAttack();
		break;
	case do_jumpattack:
		OnJumpAttack();
		break;
	case do_idle:
		OnIdle();
	default:
		break;
	}
#ifndef _SERVER
	if (m_MaskType)
	{
		if(m_MarkMask)
		{
			if(m_MarkMask != m_MaskType)
			{
				ResetNpcTypeName(1);
				m_MarkMask = 0;
			}
		}
		else
		{
			ResetNpcTypeName(0);
			m_MarkMask = m_MaskType;
		}
	}
	else
	{
		if(m_MarkMask)
		{
			ResetNpcTypeName(1);
			m_MarkMask = 0;
		}
	}
#endif
}

void KNpc::ProcCommand(int nAI)
{
	// CmdKind < 0 ��ʾû��ָ��	������ͼҲ������
	if (m_Command.CmdKind == do_none || m_bExchangeServer)
		return;

	if (nAI)
	{
		if (m_RegionIndex < 0)
			return;
		switch (m_Command.CmdKind)
		{
		case do_stand:
			DoStand();
			break;
		case do_walk:
			Goto(m_Command.Param_X, m_Command.Param_Y);
			break;
		case do_run:
			RunTo(m_Command.Param_X, m_Command.Param_Y);
			break;
		case do_jump:
			JumpTo(m_Command.Param_X, m_Command.Param_Y);
			break;
		case do_skill:
			if (int nSkillIdx = m_SkillList.FindSame(m_Command.Param_X))
			{
				SetActiveSkill(nSkillIdx);
				DoSkill(m_Command.Param_Y, m_Command.Param_Z);
			}
			else
			{
				DoStand();
			}
			break;
		case do_sit:
			DoSit();
			break;
		case do_defense:
			DoDefense();
			break;
		case do_idle:
			DoIdle();
			break;
		case do_hurt:
			DoHurt(m_Command.Param_X, m_Command.Param_Y, m_Command.Param_Z);
			break;	
			// ��Ϊ���ͼ�ܰ�ai��Ϊ1
		case do_revive:
			DoStand();
			m_ProcessAI = 1;
			m_ProcessState = 1;
#ifndef _SERVER
			this->SetInstantSpr(enumINSTANT_STATE_REVIVE);
#endif
			break;
		}
	}
	else
	{
		switch(m_Command.CmdKind)
		{
		case do_hurt:
			if (m_RegionIndex >= 0)
				DoHurt(m_Command.Param_X, m_Command.Param_Y, m_Command.Param_Z);
			break;
		case do_revive:
			DoStand();
			m_ProcessAI = 1;
			m_ProcessState = 1;
#ifndef _SERVER
			this->SetInstantSpr(enumINSTANT_STATE_REVIVE);
#endif
			break;
		case do_walk:
			if(m_RandMove.nTime > 0)
			Goto(m_Command.Param_X, m_Command.Param_Y);
			break;
		default:
			break;
		}
	}
	m_Command.CmdKind = do_none;
}

BOOL KNpc::ProcessState()
{
	int nRet = FALSE;
	if (m_RegionIndex < 0)
		return FALSE;
		
	if (!(m_LoopFrames % GAME_UPDATE_TIME))
	{
// �����������������仯ֻ�ɷ���������
#ifdef _SERVER
		// ������
		if (m_Doing == do_sit)
		{
			int nLifeAdd = m_CurrentLifeMax * 3 / 1000;
			if (nLifeAdd <= 0)
				nLifeAdd = 1;
			m_CurrentLife += nLifeAdd;
			if (m_CurrentLife > m_CurrentLifeMax)
				m_CurrentLife = m_CurrentLifeMax;

			int nManaAdd = m_CurrentManaMax * 3 / 1000;
			if (nManaAdd <= 0)
				nManaAdd = 1;
			m_CurrentMana += nManaAdd;
			if (m_CurrentMana > m_CurrentManaMax)
				m_CurrentMana = m_CurrentManaMax;

			m_CurrentStamina += PlayerSet.m_cPlayerStamina.m_nSitAdd;
			if (m_CurrentStamina > m_CurrentStaminaMax)
				m_CurrentStamina = m_CurrentStaminaMax;
		}
		// ������Ȼ�ظ�
		if (m_StunState.nTime <= 0)
		{
			m_CurrentLife += m_CurrentLifeReplenish + (m_CurrentLifeReplenish * m_CurrentLifeReplenishPercent / MAX_PERCENT);
			if (m_CurrentLife > m_CurrentLifeMax)
				m_CurrentLife = m_CurrentLifeMax;
			// ������Ȼ�ظ�
			m_CurrentMana += m_CurrentManaReplenish;
			if (m_CurrentMana > m_CurrentManaMax)
				m_CurrentMana = m_CurrentManaMax;

			// ������Ȼ�ظ�
			if (m_Doing == do_stand)
			{
				m_CurrentStamina += m_CurrentStaminaGain;
			}
			else
			{
				if (m_nPKFlag < enumPKMurder)
					m_CurrentStamina += m_CurrentStaminaGain / STAMINA_RECOVER_SCALE;				
			}
			if (m_CurrentStamina > m_CurrentStaminaMax)
				m_CurrentStamina = m_CurrentStaminaMax;
		}
#endif
		// �⻷����
	
		if (m_ActiveAuraID)
		{
			if (m_SkillList.GetLevel(m_ActiveAuraID) > 0)
			{
				int nCurLevel = m_SkillList.GetCurrentLevel(m_ActiveAuraID);

				int nMpsX, nMpsY;
				SubWorld[m_SubWorldIndex].Map2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, &nMpsX, &nMpsY);
				if (m_ActiveAuraID < MAX_SKILL && nCurLevel < MAX_SKILLLEVEL);
				KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(m_ActiveAuraID, nCurLevel);
#ifdef _SERVER
				NPC_SKILL_SYNC SkillCmd;
				SkillCmd.ID = this->m_dwID;
				if (pOrdinSkill)
				{
			//		if (pOrdinSkill->GetSkillStyle() == SKILL_SS_Missles)
					SkillCmd.nSkillID = pOrdinSkill->GetChildSkillId();
				}
				else
				{
					SkillCmd.nSkillID = 0;
				}

				SkillCmd.nSkillLevel = nCurLevel;
				SkillCmd.nMpsX = -1;
				SkillCmd.nMpsY = m_dwID;
				SkillCmd.ProtocolType = s2c_castskilldirectly;
				POINT	POff[8] = 
				{
					{0, 32},
					{-16, 32},
					{-16, 0},
					{-16, -32},
					{0, -32},
					{16, -32},
					{16, 0},
					{16, 32},
				};
				
				int nMaxCount = MAX_BROADCAST_COUNT;
				CURREGION.BroadCast(&SkillCmd, sizeof(SkillCmd), nMaxCount, m_MapX, m_MapY);
				int i = 0;
				for (i = 0; i < 8; i++)
				{
					if (CONREGIONIDX(i) == -1)
						continue;
					CONREGION(i).BroadCast(&SkillCmd, sizeof(SkillCmd), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
				}
#endif				
				KSkill * pOrdinSkill1 = (KSkill *) g_SkillManager.GetSkill(m_ActiveAuraID, nCurLevel);
				int nChildSkillId = 0;
				if (pOrdinSkill1)
				{
					nChildSkillId = pOrdinSkill1->GetChildSkillId();
					
					KSkill * pOrdinSkill2 = (KSkill *) g_SkillManager.GetSkill(nChildSkillId, nCurLevel);
					if (pOrdinSkill2)
					{
						pOrdinSkill2->Cast(m_Index, nMpsX, nMpsY);
					}
				}
				if(pOrdinSkill->GetAppendSkillNum())
				{
					for(int j = 0; j < pOrdinSkill->GetAppendSkillNum(); j++)
					{
						int nAppendId = pOrdinSkill->GetAppendSkillId(j);
						int nAppendLv = m_SkillList.GetCurrentLevel(nAppendId);
						if(nAppendLv > nCurLevel)
							nAppendLv = nCurLevel;
						if (nAppendId < MAX_SKILL && nAppendLv > 0 && nAppendLv < MAX_SKILLLEVEL);
						{
#ifdef _SERVER
							KSkill * pOrdinSkill1 = (KSkill *) g_SkillManager.GetSkill(nAppendId, nAppendLv);
							if (pOrdinSkill1)
							{
								SkillCmd.nSkillID = pOrdinSkill1->GetChildSkillId();
							}
							else
							{
								SkillCmd.nSkillID = 0;
							}
							SkillCmd.nSkillLevel = nAppendLv;
							SkillCmd.nMpsX = -1;
							SkillCmd.nMpsY = m_dwID;
							SkillCmd.ProtocolType = s2c_castskilldirectly;

							POINT	POff[8] = 
							{
								{0, 32},
								{-16, 32},
								{-16, 0},
								{-16, -32},
								{0, -32},
								{16, -32},
								{16, 0},
								{16, 32},
							};
							
							int nMaxCount = MAX_BROADCAST_COUNT;
							CURREGION.BroadCast(&SkillCmd, sizeof(SkillCmd), nMaxCount, m_MapX, m_MapY);
							int i = 0;
							for (i = 0; i < 8; i++)
							{
								if (CONREGIONIDX(i) == -1)
									continue;
								CONREGION(i).BroadCast(&SkillCmd, sizeof(SkillCmd), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
							}
#endif				
							KSkill * pOrdinSkill2 = (KSkill *) g_SkillManager.GetSkill(nAppendId, nAppendLv);
							int nChildSkillId = 0;
							if (pOrdinSkill2)
							{
								nChildSkillId = pOrdinSkill2->GetChildSkillId();
				
								KSkill * pOrdinSkill3 = (KSkill *) g_SkillManager.GetSkill(nChildSkillId, nAppendLv);
								if (pOrdinSkill3)
								{
									pOrdinSkill3->Cast(m_Index, nMpsX, nMpsY);
								}
							}
						}
					}
				}
			}
		}
	}	
#ifdef _SERVER
	if (m_PoisonState.nTime > 0)
	{ 
		m_PoisonState.nTime--;
		if (m_PoisonState.nValue[1] == 0)
		{
			m_PoisonState.nValue[1] = 1;
		}
		if (!(m_PoisonState.nTime % m_PoisonState.nValue[1]))
		{
			CalcDamage(m_nLastPoisonDamageIdx, -1, m_PoisonState.nValue[0], m_PoisonState.nValue[0], damage_poison, FALSE, 0, TRUE);
		}
		if(m_PoisonState.nTime == 0)
		{
			m_PoisonState.nValue[0] = 0;
			m_PoisonState.nValue[1] = 0;
			m_PoisonState.nValue[2] = 0;
		}
	}
	// ����״̬
	if (m_FreezeState.nTime > 0)
	{
		m_FreezeState.nTime--;
		if (m_FreezeState.nTime & 1)
		{
			nRet = TRUE;
		}
	}
	// ȼ��״̬
	if (m_BurnState.nTime > 0)
	{
		m_BurnState.nTime--;
		if (m_BurnState.nValue[1] == 0)
		{
			m_BurnState.nValue[1] = 1;
		}
		if (!(m_BurnState.nTime % m_BurnState.nValue[1]))
		{
			CalcDamage(m_Index, -1, m_BurnState.nValue[0], m_BurnState.nValue[0], damage_fire, TRUE, 0);
		}
	}

	if (m_FrozenAction.nTime > 0)
	{
		m_FrozenAction.nTime--;
	}
	// ����״̬
	if (m_RandMove.nTime > 0)
	{
		m_ProcessAI	= 0;
		if (!(g_SubWorldSet.GetGameTime() % GAME_UPDATE_TIME))
		{
			int nDesX, nDesY;
			GetMpsPos(&nDesX, &nDesY);
			int nRan = ::GetRandomNumber(0,1);
			if (nRan)
				nDesX -= g_Random(100);
			else
				nDesX += g_Random(100);
				
			nRan = ::GetRandomNumber(0,1);
			if (nRan)
				nDesY -= g_Random(100);
			else
				nDesY += g_Random(100);	
			SendCommand(do_walk, nDesX, nDesY);
		}
		m_RandMove.nTime--;
		if(m_RandMove.nTime == 0)
		m_ProcessAI	= 1;
	}
	
	// ѣ��״̬
	if (m_StunState.nTime > 0)
	{
		m_StunState.nTime--;
		nRet = TRUE;
	}

	// ��Ѫ״̬
	if (m_LifeState.nTime > 0)
	{
		m_LifeState.nTime--;
		if (!(m_LifeState.nTime % GAME_UPDATE_TIME))
		{
			m_CurrentLife += m_LifeState.nValue[0];
			if (m_CurrentLife > m_CurrentLifeMax)
			{
				m_CurrentLife = m_CurrentLifeMax;
			}
		}
	}
	// ��MANA״̬
	if (m_ManaState.nTime > 0)
	{
		m_ManaState.nTime--;
		if (!(m_ManaState.nTime % GAME_UPDATE_TIME))
		{
			m_CurrentMana += m_ManaState.nValue[0];
			if (m_CurrentMana > m_CurrentManaMax)
			{
				m_CurrentMana = m_CurrentManaMax;
			}
		}
	}
	// ����״̬
	if (m_LoseMana.nTime > 0)
	{
		m_LoseMana.nTime--;
		if (!(m_LoseMana.nTime % GAME_FPS))
		{
			m_CurrentMana -= m_LoseMana.nValue[0];
			if (m_CurrentMana < 0)
				m_CurrentMana = 0;
		}
	}
	if (m_HideState.nTime > 0)
	{
		m_HideState.nTime --;
	}
	if (m_SilentState.nTime > 0)
	{
		m_SilentState.nTime --;
	}
	if (m_WalkRun.nTime > 0)
	{
		m_WalkRun.nTime --;
	}
#endif

#ifndef _SERVER
	bool bAdjustColorId = false;

	if (m_FreezeState.nTime > 0)
	{
		if (SubWorld[0].m_dwCurrentTime & 1)
			nRet = TRUE;
		m_DataRes.SetAdjustColorId(KNpcRes::adjustcolor_freeze);
		bAdjustColorId = true;
	}

	if(m_Index == Player[CLIENT_PLAYER_INDEX].m_nIndex)
	{
	if (m_RandMove.nTime > 0)
	{
		m_ProcessAI	= 0;
		if (!(g_SubWorldSet.GetGameTime() % GAME_UPDATE_TIME))
		{
			int nDesX, nDesY;
			GetMpsPos(&nDesX, &nDesY);
			int nRan = ::GetRandomNumber(0,1);
			if (nRan)
				nDesX -= g_Random(100);
			else
				nDesX += g_Random(100);
			
			nRan = ::GetRandomNumber(0,1);
			if (nRan)
				nDesY -= g_Random(100);
			else
				nDesY += g_Random(100);
			SendCommand(do_walk, nDesX, nDesY);
		}
		m_RandMove.nTime--;
		if(m_RandMove.nTime == 0)
		m_ProcessAI	= 1;
	}
	}
	if (m_StunState.nTime > 0)
	{
		m_DataRes.SetSpecialSpr("\\spr\\skill\\����\\mag_spe_ѣ��.spr");
		nRet = TRUE;
	}

	if (m_PoisonState.nTime > 0)
	{
		m_DataRes.SetAdjustColorId(KNpcRes::adjustcolor_poison);
		bAdjustColorId = true;
	}

	if (m_BurnState.nTime > 0)
	{
		m_DataRes.SetAdjustColorId(KNpcRes::adjustcolor_burn);
		bAdjustColorId = true;
	}

	if (!bAdjustColorId)
		m_DataRes.SetAdjustColorId(KNpcRes::adjustcolor_physics);
#endif

	KStateNode* pNode;
	pNode = (KStateNode *)m_StateSkillList.GetHead();
	while(pNode)
	{
		KStateNode* pTempNode = pNode;
		pNode = (KStateNode *)pNode->GetNext();

		if (pTempNode->m_LeftTime == -1)	// ��������
			continue;

		if (pTempNode->m_LeftTime == 0)
		{
			int i = 0;
			for (i = 0; i < MAX_SKILL_STATE; i++)
			{
				if (pTempNode->m_State[i].nAttribType)
				{
					ModifyAttrib(m_Index, &pTempNode->m_State[i]);
				}
			}
			_ASSERT(pTempNode != NULL);
			pTempNode->Remove();
			delete pTempNode;

			pTempNode = NULL;

#ifdef _SERVER
			UpdateNpcStateInfo();
#endif
			continue;
		}
		else
			pTempNode->m_LeftTime --;
	}
	return nRet;

}

#ifdef _SERVER
int	KNpc::UpdateDBStateList(BYTE * pStateBuffer)
{
	if (!pStateBuffer)
		return -1;
	
	int nCount = 0;

	TDBSkillData * pStateData = (TDBSkillData *) pStateBuffer;
	KStateNode* pNode;
	pNode = (KStateNode *)m_StateSkillList.GetHead();
	while(pNode)
	{
		KStateNode* pTempNode = pNode;
		pNode = (KStateNode *)pNode->GetNext();

		if (pTempNode->m_SkillID > 0 && pTempNode->m_SkillID < MAX_SKILL && 
			pTempNode->m_Level > 0 && pTempNode->m_Level < MAX_SKILLLEVEL && 
			pTempNode->m_bOverLook)
		{
			pStateData->m_nSkillId = pTempNode->m_SkillID;
			pStateData->m_nSkillLevel = pTempNode->m_Level;
			pStateData->m_nSkillVal = pTempNode->m_LeftTime;
			pStateData++;
			nCount++;
		}
	}
	return nCount;
}
#endif

void KNpc::DoDeath(int nMode/* = 0*/)
{
	_ASSERT(m_RegionIndex >= 0);
	if (m_RegionIndex < 0)
		return;

	if (m_Doing == do_death)
		return;

	if (IsPlayer() && !m_FightMode)	// �����ڲ�������
	{
		m_CurrentLife = 1;
		return;
	}

#ifndef _SERVER
	if (this->m_Kind == kind_normal)
		this->AddBlood(this->m_CurrentLife);
#endif

#ifdef _SERVER
	if (IsPlayer() && nMode == enumDEATH_MODE_PLAYER_SPAR_NO_PUNISH)
	{
		Player[m_nPlayerIdx].Revive(LOCAL_REVIVE_TYPE);
		return;
	}
#endif

	m_Doing = do_death;
	m_ProcessAI	= 0;
	m_ProcessState = 0;

	m_Frames.nTotalFrame = m_DeathFrame;
	m_Frames.nCurrentFrame = 0;
	
	m_Height = 0;

#ifdef _SERVER
	int nPlayer = 0;
	// ɱ����Ҳ��þ���
	if (this->m_Kind != kind_player)
	{
		nPlayer = m_cDeathCalcExp.CalcExp();
	}

	//����Ʒ
	DeathPunish(nMode, nPlayer);

	if (this->m_Kind == kind_normal)
	{
		if (m_DropScriptID && nPlayer)
			Player[nPlayer].ExecuteScript(m_DropScriptID, "DropRate", m_Index);

		if (m_ActionScriptID)
		{
			if (nPlayer)
				Player[nPlayer].ExecuteScript(m_ActionScriptID, "LastDamage", m_Index);
		}
	}
	else if (this->m_Kind == kind_player)
	{
		if (Player[m_nPlayerIdx].m_dwDeathScriptId)
			Player[m_nPlayerIdx].ExecuteScript(Player[m_nPlayerIdx].m_dwDeathScriptId, "OnDeath", m_nLastDamageIdx);
	}

	NPC_DEATH_SYNC	NetCommand;
	NetCommand.ProtocolType = (BYTE)s2c_npcdeath;
	NetCommand.ID = m_dwID;

	POINT	POff[8] = 
	{
		{0, 32},
		{-16, 32},
		{-16, 0},
		{-16, -32},
		{0, -32},
		{16, -32},
		{16, 0},
		{16, 32},
	};

	int nMaxCount = MAX_BROADCAST_COUNT;
	CURREGION.BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX, m_MapY);
	int i = 0;
	for (i = 0; i < 8; i++)
	{
		if (CONREGIONIDX(i) == -1)
			continue;
		CONREGION(i).BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
	}
#endif
#ifndef _SERVER
	m_ClientDoing = cdo_death;
	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPeopleIdx == m_Index)
	{
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPeopleIdx = 0;
	}
#else
	if(this->m_Kind == kind_player)
	{
		if(Npc[Player[m_nPlayerIdx].m_nIndex].m_bReviveNow)
			return;

		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		if (m_nLastDamageIdx && Npc[m_nLastDamageIdx].m_Kind == kind_player)
		{
			sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
			sMsg.m_lpBuf = (LPVOID)Npc[m_nLastDamageIdx].m_dwID;
			sMsg.m_wMsgID = enumMSG_ID_NPC_RENASCENCE_SOMEONE;
		}
		else
		{
			sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
			sMsg.m_wMsgID = enumMSG_ID_NPC_RENASCENCE;
		}
		g_pServer->PackDataToClient(Player[m_nPlayerIdx].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
	}
#endif
}

void KNpc::OnDeath()
{	
	if (WaitForFrame())
	{
		m_Frames.nCurrentFrame = m_Frames.nTotalFrame - 1;		// ��֤�������ػص�һ֡�����
#ifndef _SERVER
		if (!IsPlayer())
		{
			int		nTempX, nTempY;
			KObjItemInfo	sInfo;

			SubWorld[m_SubWorldIndex].Map2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, &nTempX, &nTempY);
			sInfo.m_nItemID = 0;
			sInfo.m_nItemWidth = 0;
			sInfo.m_nItemHeight = 0;
			sInfo.m_nMoneyNum = 0;
			sInfo.m_nColorID = 0;
			sInfo.m_nGenre = 0;
			sInfo.m_nDetailType = 0;
			sInfo.m_nMovieFlag = 0;
			sInfo.m_nSoundFlag = 0;
			sInfo.m_bOverLook = FALSE;
			sInfo.m_szName[0] = 0;
			ObjSet.ClientAdd(0, m_CorpseSettingIdx, 0, m_Dir, 0, nTempX, nTempY, sInfo);
			m_ProcessAI = 1;
		}
#endif

		// ������
		if (m_Kind != kind_partner)//ս��Npcʱ
		{		
			DoRevive();
#ifdef _SERVER
			if (this->m_Kind == kind_normal)
				NpcSet.ExecuteScript(m_Index, m_ActionScriptID, "DeathSelf", m_Index);
			else if(this->m_Kind == kind_player && Npc[Player[m_nPlayerIdx].m_nIndex].m_bReviveNow)
				Player[m_nPlayerIdx].Revive(REMOTE_REVIVE_TYPE);
#else
			// �ͻ��˰�NPCɾ��
			if (m_Kind != kind_player)
			{
				SubWorld[0].m_WorldMessage.Send(GWM_NPC_DEL, m_Index);
				return;
			}
#endif		
		}
		else	// ͬ���ࣿ�Ժ���˵��
		{
			// �Ժ���˵Not Finish
		}
	}
	else
	{
	}
}

void KNpc::DoDefense()
{
	m_ProcessAI = 0;
}

void KNpc::OnDefense()
{
}

void KNpc::DoIdle()
{
	if (m_Doing == do_idle)
		return;
	m_Doing = do_idle;
}

void KNpc::OnIdle()
{
}

void KNpc::DoHurt(int nHurtFrames, int nX, int nY)
{
	_ASSERT(m_RegionIndex >= 0);
#ifndef _SERVER
	m_DataRes.SetBlur(FALSE);
#endif
	if (m_RegionIndex < 0)
		return;
	if (m_Doing == do_hurt || m_Doing == do_death)
		return;

	// �ܻ��ظ��ٶ��Ѿ��ﵽ100%�ˣ��������˶���
#ifdef _SERVER
	if (m_CurrentHitRecover >= MAX_PERCENT)
		return;
#endif

	m_Doing = do_hurt;
	m_ProcessAI	= 0;

#ifdef _SERVER
	m_Frames.nTotalFrame = m_HurtFrame * (MAX_PERCENT - m_CurrentHitRecover) / MAX_PERCENT;
#else
	m_ClientDoing = cdo_hurt;
	m_Frames.nTotalFrame = nHurtFrames;
	m_nHurtDesX = nX;
	m_nHurtDesY = nY;
	if (m_Height > 0)
	{
		// ��ʱ��¼������Ϊ�߶ȱ仯����OnHurt��ʹ��
		m_nHurtHeight = m_Height;
	}
	else
	{
		m_nHurtHeight = 0;
	}
#endif
	if (m_Frames.nTotalFrame <= 0)
		m_Frames.nTotalFrame = 1;
	m_Frames.nCurrentFrame = 0;

#ifdef _SERVER	// ����Χ9��Region�㲥������
	NPC_HURT_SYNC	NetCommand;
	NetCommand.ProtocolType = (BYTE)s2c_npchurt;
	NetCommand.ID = m_dwID;
	NetCommand.nFrames = m_Frames.nTotalFrame;
	GetMpsPos(&NetCommand.nX, &NetCommand.nY);

	POINT	POff[8] = 
	{
		{0, 32},
		{-16, 32},
		{-16, 0},
		{-16, -32},
		{0, -32},
		{16, -32},
		{16, 0},
		{16, 32},
	};

	int nMaxCount = MAX_BROADCAST_COUNT;
	CURREGION.BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX, m_MapY);
	int i = 0;
	for (i = 0; i < 8; i++)
	{
		if (CONREGIONIDX(i) == -1)
			continue;
		CONREGION(i).BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
	}
#endif
}

void KNpc::OnHurt()
{	
	if (m_RegionIndex < 0)
	{
		g_DebugLog("[error]%s Region Index < 0 when hurt", Name);
		return;
	}
	int nX, nY;
	GetMpsPos(&nX, &nY);
#ifdef _SERVER
	m_Height = 0;
#endif
#ifndef _SERVER
	if(m_Frames.nTotalFrame > 0)
	{
		m_Height = m_nHurtHeight * (m_Frames.nTotalFrame - m_Frames.nCurrentFrame - 1) / m_Frames.nTotalFrame;
		nX = nX + (m_nHurtDesX - nX) * m_Frames.nCurrentFrame / m_Frames.nTotalFrame;
		nY = nY + (m_nHurtDesY - nY) * m_Frames.nCurrentFrame / m_Frames.nTotalFrame;

		int nOldRegion = m_RegionIndex;
		SubWorld[0].m_Region[m_RegionIndex].DecRef(m_MapX, m_MapY, obj_npc);
		int nRegion, nMapX, nMapY, nOffX, nOffY;
		nRegion = -1;
		nMapX = nMapY = nOffX = nOffY = 0;
		SubWorld[m_SubWorldIndex].Mps2Map(nX, nY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
		if (nRegion == -1)
		{
			SubWorld[0].m_Region[nOldRegion].RemoveNpc(m_Index);
			m_dwRegionID = 0;
		}
		else if (nOldRegion != nRegion)
		{
			m_RegionIndex = nRegion;
			m_MapX = nMapX;
			m_MapY = nMapY;
			m_OffX = nOffX;
			m_OffY = nOffY;
			SubWorld[0].NpcChangeRegion(SubWorld[0].m_Region[nOldRegion].m_RegionID, SubWorld[0].m_Region[m_RegionIndex].m_RegionID, m_Index);
			m_dwRegionID = SubWorld[0].m_Region[m_RegionIndex].m_RegionID;
		}
		if (nRegion >= 0)
			CURREGION.AddRef(m_MapX, m_MapY, obj_npc);

		if ( !m_bClientOnly && m_RegionIndex >= 0)
		{
			SubWorld[0].m_Region[m_RegionIndex].DecRef(m_MapX, m_MapY, obj_npc);
			m_RegionIndex = nRegion;
			m_MapX = nMapX;
			m_MapY = nMapY;
			m_OffX = nOffX;
			m_OffY = nOffY;
			SubWorld[0].m_Region[m_RegionIndex].AddRef(m_MapX, m_MapY, obj_npc);
		}
	}
#endif
	if (WaitForFrame())
	{
		DoStand();
		m_ProcessAI = 1;
	}
}

void KNpc::DoSpecial1()
{
	DoBlurAttack();
}

void KNpc::OnSpecial1()
{	
	if (WaitForFrame() &&m_Frames.nTotalFrame != 0)
	{
#ifndef _SERVER
		m_DataRes.SetBlur(FALSE);
#endif
		DoStand();
		m_ProcessAI = 1;	
	}
	else if (IsReachFrame(ATTACKACTION_EFFECT_PERCENT))
	{
		KSkill * pSkill = (KSkill*)GetActiveSkill();
		if (pSkill)
		{
			int nChildSkill = pSkill->GetChildSkillId();
			int nChildSkillLevel = pSkill->m_ulLevel;
			
			if (nChildSkill > 0)
			{
				KSkill * pChildSkill = (KSkill*)g_SkillManager.GetSkill(nChildSkill, nChildSkillLevel);
				if (pChildSkill)
				{
					pChildSkill->Cast(m_Index, m_SkillParam1, m_SkillParam2);
				}
			}
		}

		if (m_Frames.nTotalFrame <= 0)
		{
			m_ProcessAI = 1;
		}
	}
}

void KNpc::DoSpecial2()
{
}

void KNpc::OnSpecial2()
{
	if (WaitForFrame() &&m_Frames.nTotalFrame != 0)
	{
#ifndef _SERVER
		m_DataRes.SetBlur(FALSE);
#endif
		DoStand();
		m_ProcessAI = 1;	
	}
	else if (IsReachFrame(ATTACKACTION_EFFECT_PERCENT))
	{
		ISkill * pSkill = GetActiveSkill();
		eSkillStyle eStyle = (eSkillStyle)pSkill->GetSkillStyle();
		switch(eStyle)
		{
		case SKILL_SS_Thief:
			{
				( (KThiefSkill*)pSkill )->OnSkill(this);
			}
			break;
		} 
		
		if (m_Frames.nTotalFrame <= 0)
		{
			m_ProcessAI = 1;
		}
	}
	
}

void KNpc::DoSpecial3()
{
}

void KNpc::OnSpecial3()
{
}

void KNpc::DoStand()
{
#ifndef _SERVER
	m_DataRes.SetBlur(FALSE);
#endif
	m_Frames.nTotalFrame = m_StandFrame;
	if (m_Doing == do_stand)
	{	
		return;
	}
	else
	{
		m_Doing = do_stand;
		m_Frames.nCurrentFrame = 0;
#ifdef _SERVER
		NPC_PLAYER_TYPE_NORMAL_SYNC	NetCommand;
		NetCommand.ProtocolType = (BYTE)s2c_npcstand;
		NetCommand.ID = m_dwID;
		GetMpsPos(&NetCommand.MapX, &NetCommand.MapY);
		NetCommand.OffX = m_OffX;
		NetCommand.OffY = m_OffY;

		POINT	POff[8] = 
		{
			{0, 32},
			{-16, 32},
			{-16, 0},
			{-16, -32},
			{0, -32},
			{16, -32},
			{16, 0},
			{16, 32},
		};

		int nMaxCount = MAX_BROADCAST_COUNT;
		CURREGION.BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX, m_MapY);
		int i = 0;
		for (i = 0; i < 8; i++)
		{
			if (CONREGIONIDX(i) == -1)
				continue;
			CONREGION(i).BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
		}
#else
		if (m_FightMode)
			m_ClientDoing = cdo_fightstand;
		else if (g_Random(6) != 1)
			m_ClientDoing = cdo_stand;
		else
			m_ClientDoing = cdo_stand1;

		m_DataRes.StopSound();
#endif
	}
}


void KNpc::OnStand()
{	
	if (WaitForFrame())
	{
#ifndef _SERVER
		if (m_FightMode)
		{
			m_ClientDoing = cdo_fightstand;
		}
		else if (g_Random(6) != 1)
		{
			m_ClientDoing = cdo_stand;
		}
		else
		{
			m_ClientDoing = cdo_stand1;
		}
#endif
	}
}

void KNpc::DoRevive()
{
	if (m_RegionIndex < 0)
	{
		g_DebugLog("[error]%s Region Index < 0 when dorevive", Name);
		return;
	}
#ifndef _SERVER
	m_DataRes.SetBlur(FALSE);
#endif
	if (m_Doing == do_revive)
	{
		return;
	}
	else
	{	
		m_Doing = do_revive;
		m_ProcessAI = 0;
		m_ProcessState = 0;

		ClearStateSkillEffect();
		ClearNormalState();

#ifdef _SERVER
		if (IsPlayer())
		{
			return;
		}
		m_Frames.nTotalFrame = m_ReviveFrame;
		SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].DecRef(m_MapX, m_MapY, obj_npc);
		SubWorld[m_SubWorldIndex].NpcChangeRegion(m_RegionIndex, VOID_REGION, m_Index);	// spe 03/06/28
		m_Frames.nCurrentFrame = 0;
#else
		m_Frames.nTotalFrame = m_DeathFrame;
		m_ClientDoing = cdo_death;
#endif
	}
}

void KNpc::OnRevive()
{	
#ifdef _SERVER
	if (!IsPlayer() && WaitForFrame())
	{
		Revive();
	}
#else	// �ͻ���
	m_Frames.nCurrentFrame = m_Frames.nTotalFrame - 1;
#endif
}

void KNpc::DoRun()
{
	_ASSERT(m_RegionIndex >= 0);

	if (m_CurrentRunSpeed)
		m_Frames.nTotalFrame = (m_RunFrame * m_RunSpeed) / m_CurrentRunSpeed;
	else
		m_Frames.nTotalFrame = m_RunFrame;

#ifndef _SERVER
	if (m_FightMode)
	{
		m_ClientDoing = cdo_fightrun;
	}
	else
	{
		m_ClientDoing = cdo_run;
	}
#endif

#ifdef _SERVER

	NPC_RUN_SYNC	NetCommand;
	NetCommand.ProtocolType = (BYTE)s2c_npcrun;
	NetCommand.ID = m_dwID;
	NetCommand.nMpsX = m_DesX;
	NetCommand.nMpsY = m_DesY;

	POINT	POff[8] = 
	{
		{0, 32},
		{-16, 32},
		{-16, 0},
		{-16, -32},
		{0, -32},
		{16, -32},
		{16, 0},
		{16, 32},
	};

	int nMaxCount = MAX_BROADCAST_COUNT;
	CURREGION.BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX, m_MapY);
	int i = 0;
	for (i = 0; i < 8; i++)
	{
		if (CONREGIONIDX(i) == -1)
			continue;
		CONREGION(i).BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
	}
#endif

	if (m_Doing == do_run)
	{
		return;
	}

	m_Doing = do_run;

	m_Frames.nCurrentFrame = 0;
}

void KNpc::OnRun()
{	
	if (m_Doing == do_hurt || m_Doing == do_death || m_Doing == do_revive)
		DoStand();
	WaitForFrame();

#ifndef _SERVER
	if (m_WalkRun.nTime)
		m_DataRes.SetBlur(TRUE);
#endif

#ifdef _SERVER
	if (!(m_LoopFrames % GAME_UPDATE_TIME))
	{
		if(Npc[Player[m_nPlayerIdx].m_nIndex].m_nCurPKPunishState == enumDEATH_MODE_TOURNAMENTS_PUNISH)
		{
		}
		else
		{
			switch (m_nPKFlag)
			{
				case enumPKMurder:
					m_CurrentStamina -= PlayerSet.m_cPlayerStamina.m_nKillRunSub;
				break;
				case enumPKTongWar:
					m_CurrentStamina -= PlayerSet.m_cPlayerStamina.m_nTongWarRunSub;
				break;
			}
		}
		if (m_CurrentStamina <= 0)
			m_CurrentStamina = 0;
	}
#endif
	if(m_Doing == do_runattack)
	{	
		m_CurrentRunSpeed += 50;
		ServerMove(m_CurrentRunSpeed);
		m_CurrentRunSpeed -= 50;
	}
	else
	{
		ServerMove(m_CurrentRunSpeed);
	}
}

void KNpc::DoSit()
{
	_ASSERT(m_RegionIndex >= 0);

	if (m_Doing == do_sit)
		return;

	m_Doing = do_sit;
	
#ifdef _SERVER	// ����Χ9��Region�㲥������
	NPC_SIT_SYNC	NetCommand;
	NetCommand.ProtocolType = (BYTE)s2c_npcsit;
	NetCommand.ID = m_dwID;
		
	POINT	POff[8] = 
	{
		{0, 32},
		{-16, 32},
		{-16, 0},
		{-16, -32},
		{0, -32},
		{16, -32},
		{16, 0},
		{16, 32},
	};

	int nMaxCount = MAX_BROADCAST_COUNT;
	CURREGION.BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX, m_MapY);
	int i = 0;
	for (i = 0; i < 8; i++)
	{
		if (CONREGIONIDX(i) == -1)
			continue;
		CONREGION(i).BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
	}
#endif

#ifndef _SERVER
			m_ClientDoing = cdo_sit;
#endif
	

	m_Frames.nTotalFrame = m_SitFrame;
	m_Frames.nCurrentFrame = 0;
	
}

void KNpc::OnSit()
{
	// ������������û���趨��
	if (WaitForFrame())
	{	
		m_Frames.nCurrentFrame = m_Frames.nTotalFrame - 1;
	}	
}

void KNpc::DoSkill(int nX, int nY)
{
	_ASSERT(m_RegionIndex >= 0);

	if (Player[m_nPlayerIdx].CheckTrading())
		return;
	if (m_Doing == do_skill)
		return;

	// ��ս��״̬���ܷ�����
	ISkill * pSkill = GetActiveSkill();
	if(pSkill)
	{
		eSkillStyle eStyle = (eSkillStyle)pSkill->GetSkillStyle();
if (IsPlayer())
	{
		if (!m_FightMode)
			return;
#ifdef _SERVER
		if (m_nPlayerIdx > 0)
			Player[m_nPlayerIdx].m_ItemList.Abrade(enumAbradeAttack);
#endif
	}
		if (m_SkillList.CanCast(m_ActiveSkillID, SubWorld[m_SubWorldIndex].m_dwCurrentTime))
		{
			switch (pSkill->CanCastSkill(m_Index, nX, nY))
			{
			case -1:
#ifdef _SERVER
				if(nX == -1)
				{
					int nDesX, nDesY;
					Npc[nY].GetMpsPos(&nDesX, &nDesY);
					SendCommand(do_run, nDesX, nDesY);
					return;
				}
#endif
				break;
			case 0:
				goto Exit;
				break;
			case 1:
				if( m_Kind != kind_player || Cost(pSkill->GetSkillCostType(), pSkill->GetSkillCost(this)))
				{
		/*------------------------------------------------------------------------------------
		������ʱ������ָ��Ŀ�����ʱ������Skill.Cast������������һ������Ϊ-1,�ڶ���ΪNpc index
		��S2Cʱ���ڶ�������������Server��NpcIndexתΪNpcdwID�γ�ȥ��
		��C�յ���ָ��ʱ����NpcdwIDתΪ������NpcIndex
			-------------------------------------------------------------------------------------*/
#ifdef _SERVER	// ����Χ9��Region�㲥������
					NPC_SKILL_SYNC	NetCommand;
					
					NetCommand.ProtocolType = (BYTE)s2c_skillcast;
					NetCommand.ID = m_dwID;
					NetCommand.nSkillID = m_ActiveSkillID;
					NetCommand.nSkillLevel = m_SkillList.GetCurrentLevel(m_ActiveSkillID);
					
					if (nY <= 0 ) 
					{
						DoStand();
						return;
					}
					
					NetCommand.nMpsX = nX;
					if (nX == -1) //m_nDesX == -1 means attack someone whose id is DesY , and if m_nDesX == -2 means attack at somedir
					{
						NetCommand.nMpsY = Npc[nY].m_dwID;
						if (0 == NetCommand.nMpsY || Npc[nY].m_SubWorldIndex != m_SubWorldIndex)
							return;
					}
					else
					{
						NetCommand.nMpsY = nY;
					}

					m_SkillParam1 = nX;
					m_SkillParam2 = nY;
					m_DesX = nX;
					m_DesY = nY;
					
					POINT	POff[8] = 
					{
						{0, 32},
						{-16, 32},
						{-16, 0},
						{-16, -32},
						{0, -32},
						{16, -32},
						{16, 0},
						{16, 32},
					};
					int nMaxCount = MAX_BROADCAST_COUNT;
					CURREGION.BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX, m_MapY);
					int i = 0;
					for (i = 0; i < 8; i++)
					{
						if (CONREGIONIDX(i) == -1)
							continue;
						CONREGION(i).BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
					}
#endif		
					if (eStyle == SKILL_SS_Missles 
						|| eStyle == SKILL_SS_Melee 
						|| eStyle == SKILL_SS_InitiativeNpcState 
						|| eStyle == SKILL_SS_PassivityNpcState)
					{
						DoOrdinSkill((KSkill *) pSkill, nX, nY);
					}
					else
					{
						switch(eStyle)
						{
						case SKILL_SS_Thief:
							{
								((KThiefSkill*)pSkill)->DoSkill(this, nX, nY);

							}break;
						default:
							return;
						}
					}
					if (m_HideState.nTime)
						m_HideState.nTime = 0;
					return;
				}
			}
		}
	}
	else
	{
		_ASSERT(pSkill);
		return;
	}
Exit:
	m_nPeopleIdx = 0;
	m_nObjectIdx = 0;
	DoStand();
} 

int KNpc::DoOrdinSkill(KSkill * pSkill, int nX, int nY)
{
	_ASSERT(pSkill);

#ifndef _SERVER		
	m_DataRes.StopSound();
	int x, y, tx, ty;
	SubWorld[m_SubWorldIndex].Map2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, &x, &y);
	
	if (nY < 0)
		return 0;
	
	if (nX < 0)
	{
		if (nX != -1) 
			return 0;
		
		if (nY >= MAX_NPC || Npc[nY].m_dwID == 0 || Npc[nY].m_SubWorldIndex != m_SubWorldIndex)
			return 0;
		Npc[nY].GetMpsPos(&tx, &ty);
	}
	else
	{
		tx = nX;
		ty = nY;
	}
	
	m_SkillParam1 = nX;
	m_SkillParam2 = nY;
	m_DesX = nX;
	m_DesY = nY;
	
	m_Dir = g_GetDirIndex(x, y, tx, ty);
	if (pSkill->GetPreCastEffectFile()[0])
		m_DataRes.SetSpecialSpr((char *)pSkill->GetPreCastEffectFile());
	
	if (IsPlayer())
		pSkill->PlayPreCastSound(m_nSex,x, y);
	
	if (pSkill->IsNeedShadow())		
		m_DataRes.SetBlur(TRUE);
	else
		m_DataRes.SetBlur(FALSE);
#endif
	
	CLIENTACTION ClientDoing = pSkill->GetActionType();
	
#ifndef _SERVER
	if (ClientDoing >= cdo_count) 
		m_ClientDoing = cdo_magic;
	else if (ClientDoing != cdo_none)
		m_ClientDoing = ClientDoing;
#endif
	if (pSkill->GetSkillStyle() == SKILL_SS_Melee)
	{
		if (CastMeleeSkill(pSkill) == FALSE)
		{
			m_nPeopleIdx = 0;
			m_nObjectIdx = 0;
			m_ProcessAI = 1;
			DoStand();
			
			return 1 ;
		}
		if(!pSkill->IsAura())
		{
			DWORD dwCastTime = 0;
			eSkillStyle eStyle = (eSkillStyle)pSkill->GetSkillStyle();
			if (eStyle == SKILL_SS_Missles 
				|| eStyle == SKILL_SS_Melee 
				|| eStyle == SKILL_SS_InitiativeNpcState 
				|| eStyle == SKILL_SS_PassivityNpcState)
			{
				dwCastTime = pSkill->GetDelayPerCast(m_bRideHorse);
			}
			else
			{
				switch(eStyle)
				{
				case SKILL_SS_Thief:
					{
						dwCastTime = ((KThiefSkill*)pSkill)->GetDelayPerCast();
					}break;
				}
			}
			m_SkillList.SetNextCastTime(m_ActiveSkillID, SubWorld[m_SubWorldIndex].m_dwCurrentTime, SubWorld[m_SubWorldIndex].m_dwCurrentTime + dwCastTime);
		}
	}
	//�������ܵļ����ͷ�ʱ������ͨ���ܲ�ͬ��һ����AttackFrame,һ����CastFrame
	else if (pSkill->IsPhysical())
	{
		if (ClientDoing == cdo_none) 
			m_Frames.nTotalFrame = 0;
		else
		{
			int nTotalFrame = m_AttackFrame * MAX_PERCENT / (m_CurrentAttackSpeed + MAX_PERCENT);
			m_Frames.nTotalFrame = nTotalFrame - nTotalFrame % 2;
			if (m_Frames.nTotalFrame <= 0)
				m_Frames.nTotalFrame = 1;
		}
		
#ifndef _SERVER
		if (g_Random(3))
			m_ClientDoing = cdo_attack;
		else 
			m_ClientDoing = cdo_attack1;
#endif
		m_Doing = do_attack;
	}
	else
	{
		if (ClientDoing == cdo_none) 
			m_Frames.nTotalFrame = 0;
		else
		{
			int nTotalFrame = m_CastFrame * MAX_PERCENT / (m_CurrentCastSpeed + MAX_PERCENT);
			m_Frames.nTotalFrame = nTotalFrame - nTotalFrame % 2;
			if (m_Frames.nTotalFrame <= 0)
				m_Frames.nTotalFrame = 1;
		}
		m_Doing  = do_magic;
	}
	m_ProcessAI = 0;
	m_Frames.nCurrentFrame = 0;	
	return 1;
}

BOOL	KNpc::CastMeleeSkill(KSkill * pSkill)
{
	BOOL bSuceess = FALSE;
	_ASSERT(pSkill);
	
	switch(pSkill->GetMeleeType())
	{
	case Melee_AttackWithBlur:
		{
			bSuceess = DoBlurAttack();
		}break;
	case Melee_Jump:
		{
			if (NewJump(m_DesX, m_DesY))
			{
				DoJump();
				bSuceess = TRUE;
			}
			
		}break;
	case Melee_JumpAndAttack:
		{
			if (m_DesX < 0 && m_DesY > 0) 
			{
				int x, y;
				SubWorld[m_SubWorldIndex].Map2Mps
					(
					Npc[m_DesY].m_RegionIndex,
					Npc[m_DesY].m_MapX, 
					Npc[m_DesY].m_MapY, 
					Npc[m_DesY].m_OffX, 
					Npc[m_DesY].m_OffY, 
					&x,
					&y
					);
				
				m_DesX = x + 1;
				m_DesY = y;
			}

			if (NewJump(m_DesX, m_DesY))
			{
				DoJumpAttack();
				bSuceess = TRUE;
			}

		}break;
	case Melee_RunAndAttack:
		{
			bSuceess = DoRunAttack();

		}break;
	case Melee_ManyAttack:
		{
			bSuceess = DoManyAttack();
		}break;
	case Melee_MoveWithBlur:
		{
			m_SkillParam1 = pSkill->GetParam1();
			bSuceess = DoBlurMove();
		}break;
	default:
		m_ProcessAI = 1;
		break;
	}
	return bSuceess;

}

BOOL KNpc::DoBlurAttack()// DoSpecail1
{
	if (m_Doing == do_special1)
		return FALSE;
	
	KSkill * pSkill = (KSkill*) GetActiveSkill();
	if (!pSkill) 
        return FALSE;
	
	_ASSERT(pSkill->GetSkillStyle() == SKILL_SS_Melee);

#ifndef _SERVER
		m_ClientDoing = pSkill->GetActionType();
		m_DataRes.SetBlur(TRUE);
#endif

	m_Frames.nTotalFrame = m_AttackFrame * MAX_PERCENT / (m_CurrentAttackSpeed + MAX_PERCENT);
	m_Frames.nCurrentFrame = 0;
	m_Doing = do_special1;
	return TRUE;
}

void KNpc::OnSkill()
{
	if (WaitForFrame() && m_Frames.nTotalFrame != 0)
	{
		DoStand();
		m_ProcessAI = 1;	
	}
	else if (IsReachFrame(ATTACKACTION_EFFECT_PERCENT))
	{
		KSkill * pSkill = NULL;
#ifndef _SERVER
		m_DataRes.SetBlur(FALSE);
#endif

		if (m_DesX == -1) 
		{
			if (m_DesY <= 0) 
				goto Label_ProcessAI;
			
			//��ʱ�ý�ɫ�Ѿ���Чʱ
			if (Npc[m_DesY].m_RegionIndex < 0) 
				goto Label_ProcessAI;
		}
			
		pSkill =(KSkill*) GetActiveSkill();

		if (pSkill)
		{
			pSkill->Cast(m_Index, m_DesX, m_DesY);

			DWORD dwCastTime = 0;
			eSkillStyle eStyle = (eSkillStyle)pSkill->GetSkillStyle();
			if (eStyle == SKILL_SS_Missles 
				|| eStyle == SKILL_SS_Melee 
				|| eStyle == SKILL_SS_InitiativeNpcState 
				|| eStyle == SKILL_SS_PassivityNpcState)
			{
				dwCastTime = pSkill->GetDelayPerCast(m_bRideHorse);
			}
			else if (eStyle == SKILL_SS_Thief)
				dwCastTime = ((KThiefSkill*)pSkill)->GetDelayPerCast();

			m_SkillList.SetNextCastTime(m_ActiveSkillID, SubWorld[m_SubWorldIndex].m_dwCurrentTime, SubWorld[m_SubWorldIndex].m_dwCurrentTime + dwCastTime);
		}

Label_ProcessAI:
		if (m_Frames.nTotalFrame <= 0)
		{

			m_ProcessAI = 1;
		}
	}
}

void KNpc::JumpTo(int nMpsX, int nMpsY)
{
	if (NewJump(nMpsX, nMpsY))
		DoJump();
	else
	{
		RunTo(nMpsX, nMpsY);
	}
}

void KNpc::RunTo(int nMpsX, int nMpsY)
{
	if (NewPath(nMpsX, nMpsY))
		DoRun();
}

void KNpc::Goto(int nMpsX, int nMpsY)
{
	if (NewPath(nMpsX, nMpsY))
		DoWalk();
}

void KNpc::DoWalk()
{
	_ASSERT(m_RegionIndex >= 0);

	if (m_CurrentWalkSpeed)
		m_Frames.nTotalFrame = (m_WalkFrame * m_WalkSpeed) / m_CurrentWalkSpeed + 1;
	else
		m_Frames.nTotalFrame = m_WalkFrame;
	
#ifdef _SERVER		// Server�˵Ĵ���
	NPC_WALK_SYNC	NetCommand;
	NetCommand.ProtocolType = (BYTE)s2c_npcwalk;
	NetCommand.ID = m_dwID;
	NetCommand.nMpsX = m_DesX;
	NetCommand.nMpsY = m_DesY;

	POINT	POff[8] = 
	{
		{0, 32},
		{-16, 32},
		{-16, 0},
		{-16, -32},
		{0, -32},
		{16, -32},
		{16, 0},
		{16, 32},
	};
	int nMaxCount = MAX_BROADCAST_COUNT;
	CURREGION.BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX, m_MapY);
	int i = 0;
	for (i = 0; i < 8; i++)
	{
		if (CONREGIONIDX(i) == -1)
			continue;
		CONREGION(i).BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
	}	
#endif

	if (m_Doing == do_walk)
	{
		return;
	}
	m_Doing = do_walk;
	m_Frames.nCurrentFrame = 0;

#ifndef _SERVER
	if (m_FightMode)
	{
		m_ClientDoing = cdo_fightwalk;
	}
	else
	{
		m_ClientDoing = cdo_walk;
	}
#endif
	
}

void KNpc::OnWalk()
{
#ifndef	_SERVER
	if (m_WalkRun.nTime)
		m_DataRes.SetBlur(TRUE);
#endif
	WaitForFrame();
	ServerMove(m_CurrentWalkSpeed);
}

void KNpc::ModifyAttrib(int nAttacker, void* pData)
{
	if (pData != NULL)
	{
		g_NpcAttribModify.ModifyAttrib(this, pData);
	}
}

#ifdef _SERVER
BOOL KNpc::CalcDamage(int nAttacker, int nMissleSeries, int nMin, int nMax, DAMAGE_TYPE nType, BOOL bIsMelee, BOOL bReturn /* = FALSE */, int nFiveElements_DamageP /*0*/, int nStolen_Life/*0*/, int nStolen_Mana/*0*/, int nStolen_Stamina/*0*/, BOOL bIsDS /*= FALSE*/, BOOL bIsFS /*= FALSE*/)
{
	if (m_Doing == do_death || m_Doing == do_revive || m_RegionIndex < 0)
		return FALSE;

	if (nMin + nMax <= 0)
		return FALSE;

	int nDamage = 0, nCurDamage = 0;
	if (m_Series == series_minus)
	{
		nDamage = 1;
	}
	else
	{
		int	nRes = 0;
		int nDamageRange = nMax - nMin;

		if (nDamageRange < 0) 
		{
			nDamage = nMax + g_Random(-nDamageRange);
		}
		else
		{
			nDamage = nMin + g_Random(nMax - nMin);
		}
		if(bReturn)
		{
			if (bIsDS)
				nDamage = nDamage * MAX_DEATLY_STRIKE_ENHANCEP / MAX_PERCENT;

			if (bIsFS)
				nDamage = m_CurrentLife / MAX_PERCENT * GetRandomNumber(MIN_FATALLY_STRIKE_ENHANCEP, MAX_FATALLY_STRIKE_ENHANCEP);

			if(this->m_Kind == kind_normal)
			{
				if(this->m_btSpecial)
					nDamage = nDamage * NpcSet.m_nNpcSpecialDamageRate / MAX_PERCENT;
			}
		}
		else
		{
			if(m_Level >= LEVEL_EXPLOSIVE)
			{
				ReplySkill();

				if (m_CurrentLife < (m_CurrentLifeMax * LIFE_EXPLOSIVE / MAX_PERCENT))
					RescueSkill();
			}

			if(Npc[nAttacker].m_Level >= LEVEL_EXPLOSIVE)
				Npc[nAttacker].AttackSkill(m_Index);
		}
		switch(nType)
		{
		case damage_physics:
			nRes = m_CurrentPhysicsResist;
			if (nRes > m_CurrentPhysicsResistMax)
			{
				nRes = m_CurrentPhysicsResistMax;
			}
			nDamage -= m_PhysicsArmor.nValue[0];;
			if (bIsMelee)
			{
				nMax = m_CurrentMeleeDmgRetPercent;
			}
			else
			{
				nMax = m_CurrentRangeDmgRetPercent;
			}
			break;
		case damage_cold:
			nRes = m_CurrentColdResist;
			if (nRes > m_CurrentColdResistMax)
			{
				nRes = m_CurrentColdResistMax;
			}
			nDamage -= m_ColdArmor.nValue[0];
			if (bIsMelee)
			{
				nMax = m_CurrentMeleeDmgRetPercent;
			}
			else
			{
				nMax = m_CurrentRangeDmgRetPercent;
			}
			break;
		case damage_fire:
			nRes = m_CurrentFireResist;
			if (nRes > m_CurrentFireResistMax)
			{
				nRes = m_CurrentFireResistMax;
			}
			nDamage -= m_FireArmor.nValue[0];
			if (bIsMelee)
			{
				nMax = m_CurrentMeleeDmgRetPercent;
			}
			else
			{
				nMax = m_CurrentRangeDmgRetPercent;
			}
			break;
		case damage_light:
			nRes = m_CurrentLightResist;
			if (nRes > m_CurrentLightResistMax)
			{
				nRes = m_CurrentLightResistMax;
			}
			nDamage -= m_LightArmor.nValue[0];
			if (bIsMelee)
			{
				nMax = m_CurrentMeleeDmgRetPercent;
			}
			else
			{
				nMax = m_CurrentRangeDmgRetPercent;
			}
			break;
		case damage_poison:
			nRes = m_CurrentPoisonResist;
			if (nRes > m_CurrentPoisonResistMax)
			{
				nRes = m_CurrentPoisonResistMax;
			}
			nDamage -= m_PoisonArmor.nValue[0];
			if (bIsMelee)
			{
				nMax = m_CurrentMeleeDmgRetPercent;
			}
			else
			{
				nMax = m_CurrentRangeDmgRetPercent;
			}
			m_nLastPoisonDamageIdx = nAttacker;
			break;
		case damage_magic:
			nRes = 0;
			break;
		default:
			nRes = 0;
			break;
		}
			if (this->m_Kind == kind_player)
			{
				if(Npc[nAttacker].m_Kind == kind_player)
				{
					nDamage = nDamage * NpcSet.m_nPKDamageRate / MAX_PERCENT;
//					if(NpcSet.m_nLevelPKDamageRate > 0 && (Npc[nAttacker].m_Level < this->m_Level))
//						nDamage += nDamage * ((this->m_Level - Npc[nAttacker].m_Level) / NpcSet.m_nLevelPKDamageRate) / MAX_PERCENT;
				}
			}		
		if (nDamage <= 0)
			return FALSE;

		if ((nMissleSeries == series_metal && m_Series == series_wood) || 
			(nMissleSeries == series_water && m_Series == series_fire) || 
			(nMissleSeries == series_wood && m_Series == series_earth) || 
			(nMissleSeries == series_fire && m_Series == series_metal) || 
			(nMissleSeries == series_earth && m_Series == series_water))
		{
			nRes -= nFiveElements_DamageP;
			nDamage -= (Npc[nAttacker].m_CurrentFiveElementsEnhance - m_CurrentFiveElementsResist);
		}
		else if ((nMissleSeries == series_metal && m_Series == series_fire) || 
			(nMissleSeries == series_water && m_Series == series_earth) || 
			(nMissleSeries == series_wood && m_Series == series_metal) || 
			(nMissleSeries == series_fire && m_Series == series_water) || 
			(nMissleSeries == series_earth && m_Series == series_wood))
		{
			nRes += nFiveElements_DamageP;
			nDamage -= (m_CurrentFiveElementsResist - Npc[nAttacker].m_CurrentFiveElementsEnhance);
		}

		if (nRes > MAX_RESIST)
		{
			nRes = MAX_RESIST;
		}

		if (m_ManaShield.nValue[0] > 0)
		{
			int nManaDamage = nDamage * m_ManaShield.nValue[0] / MAX_PERCENT;
			m_CurrentMana -= nManaDamage;
			nCurDamage += nManaDamage;
			if (m_CurrentMana < 0)
			{
				nDamage -= m_CurrentMana;
				nCurDamage += m_CurrentMana;
				m_CurrentMana = 0;
			}
			else
			{
				nDamage -= nManaDamage;
			}
		}
		if (m_CurrentManaShield > 0)
			nDamage -= m_CurrentManaShield;
		nDamage -= nDamage * nRes / MAX_PERCENT;
		if (nDamage <= 0)
			return FALSE;
		if (nAttacker > 0)
		{
			if (bReturn)
			{
				if(nDamage > 0 && nType == damage_poison)
				{
					if(m_CurrentPoisonDamageReturnPercent)
					{
						if (m_PoisonState.nTime)
						{
							nMin = nDamage * m_CurrentPoisonDamageReturnPercent / MAX_PERCENT;
							Npc[nAttacker].CalcDamage(m_Index, -1, nMin, nMin, damage_magic, FALSE, 0, TRUE);
						}
					}
				}
			}
			else
			{
				if(nDamage > 0)
				{
					if (bIsMelee)
					{
						nMin = m_CurrentMeleeDmgRet;
						nMin += nDamage * nMax / MAX_PERCENT;
						if (Npc[nAttacker].m_CurrentReturnResPercent > 0)
							nMin -= nMin * Npc[nAttacker].m_CurrentReturnResPercent / MAX_PERCENT;
						Npc[nAttacker].CalcDamage(m_Index, -1, nMin, nMin, damage_magic, FALSE, 0, TRUE);
					}
					else
					{
						nMin = m_CurrentRangeDmgRet;
						nMin += nDamage * nMax / MAX_PERCENT;
						if (Npc[nAttacker].m_CurrentReturnResPercent)
							nMin -= nMin * Npc[nAttacker].m_CurrentReturnResPercent / MAX_PERCENT;
						Npc[nAttacker].CalcDamage(m_Index, -1, nMin, nMin, damage_magic, FALSE, 0, TRUE);
					}
				}
			}
		}
		if (m_Kind != kind_player && Npc[nAttacker].m_Kind == kind_player && Npc[nAttacker].m_nPlayerIdx > 0) {
			m_cDeathCalcExp.AddDamage(Npc[nAttacker].m_nPlayerIdx, (m_CurrentLife - nDamage > 0 ? nDamage : m_CurrentLife));
		}
		m_nLastDamageIdx = nAttacker;
	}

	if (nDamage > 0)
	{
		Npc[nAttacker].m_CurrentLife += nDamage * nStolen_Life / MAX_PERCENT;
		if (Npc[nAttacker].m_CurrentLife > Npc[nAttacker].m_CurrentLifeMax)
			Npc[nAttacker].m_CurrentLife = Npc[nAttacker].m_CurrentLifeMax;

		Npc[nAttacker].m_CurrentMana += nDamage * nStolen_Mana / MAX_PERCENT;
		if (Npc[nAttacker].m_CurrentMana > Npc[nAttacker].m_CurrentManaMax)
			Npc[nAttacker].m_CurrentMana = Npc[nAttacker].m_CurrentManaMax;

		Npc[nAttacker].m_CurrentStamina += nDamage * nStolen_Stamina / MAX_PERCENT;
		if (Npc[nAttacker].m_CurrentStamina > Npc[nAttacker].m_CurrentStaminaMax)
			Npc[nAttacker].m_CurrentStamina = Npc[nAttacker].m_CurrentStaminaMax;

		m_CurrentMana += nDamage * m_CurrentDamage2Mana / MAX_PERCENT;
		if (m_CurrentMana > m_CurrentManaMax)
			m_CurrentMana = m_CurrentManaMax;
	}
	m_CurrentLife -= nDamage;
	nCurDamage += nDamage;
	if (m_CurrentLife <= 0)
	{
		nCurDamage += m_CurrentLife;
		if (m_Doing != do_death && 
			m_Doing != do_revive)
		{
			if((m_DeathSkill[0].nSkillId > 0 && m_DeathSkill[0].nSkillId < MAX_SKILL) && m_Level >= LEVEL_EXPLOSIVE)
				DeathSkill();

			int nMode = DeathCalcPKValue(m_nLastDamageIdx);
			DoDeath(nMode);

			if (m_Kind == kind_player)
				Player[m_nPlayerIdx].m_cPK.CloseAll();
		}
	}
	if (nCurDamage > 0 && (this->m_Kind == kind_player) && 
		(Npc[nAttacker].m_Kind == kind_player))
	{
		if (Player[Npc[nAttacker].m_nPlayerIdx].m_dwDamageScriptId)
			Player[m_nPlayerIdx].ExecuteScript(Player[m_nPlayerIdx].m_dwDamageScriptId, "OnDamage", nCurDamage);
	}
	return TRUE;
}

void KNpc::ReplySkill()
{
	if (!m_Index)
		return;

	if (m_Doing == do_death || m_Doing == do_revive)
		return;

	for (int i = 0; i < MAX_AUTOSKILL; i ++)
	{
		if (m_ReplySkill[i].nSkillId > 0 && m_ReplySkill[i].nSkillId < MAX_SKILL && 
			m_ReplySkill[i].nSkillLevel > 0 && m_ReplySkill[i].nSkillLevel < MAX_SKILLLEVEL)
		{
			if (m_ReplySkill[i].dwNextCastTime < SubWorld[m_SubWorldIndex].m_dwCurrentTime)
			{
				if (g_RandPercent(m_ReplySkill[i].nRate))
				{
					this->Cast(m_ReplySkill[i].nSkillId, m_ReplySkill[i].nSkillLevel);
					m_ReplySkill[i].dwNextCastTime = SubWorld[m_SubWorldIndex].m_dwCurrentTime + m_ReplySkill[i].nWaitCastTime;
				}
			}
		}
	}
}

void KNpc::RescueSkill()
{
	if (!m_Index)
		return;

	if (m_Doing == do_death || m_Doing == do_revive)
		return;

	for (int i = 0; i < MAX_AUTOSKILL; i ++)
	{
		if (m_RescueSkill[i].nSkillId > 0 && m_RescueSkill[i].nSkillId < MAX_SKILL && 
			m_RescueSkill[i].nSkillLevel > 0 && m_RescueSkill[i].nSkillLevel < MAX_SKILLLEVEL)
		{
			if (m_RescueSkill[i].dwNextCastTime < SubWorld[m_SubWorldIndex].m_dwCurrentTime)
			{
				if (g_RandPercent(m_RescueSkill[i].nRate))
				{
					this->Cast(m_RescueSkill[i].nSkillId, m_RescueSkill[i].nSkillLevel);
					m_RescueSkill[i].dwNextCastTime = SubWorld[m_SubWorldIndex].m_dwCurrentTime + m_RescueSkill[i].nWaitCastTime;
				}
			}
		}
	}
}

void KNpc::AttackSkill(int nLauncher)
{
	if (!m_Index || !Npc[nLauncher].m_Index)
		return;

	if (m_Doing == do_death || m_Doing == do_revive)
		return;

	if (Npc[nLauncher].m_Doing == do_death || Npc[nLauncher].m_Doing == do_revive)
		return;

	for (int i = 0; i < MAX_AUTOSKILL; i ++)
	{
		if (m_AttackSkill[i].nSkillId > 0 && m_AttackSkill[i].nSkillId < MAX_SKILL && 
			m_AttackSkill[i].nSkillLevel > 0 && m_AttackSkill[i].nSkillLevel < MAX_SKILLLEVEL)
		{
			if (m_AttackSkill[i].dwNextCastTime < SubWorld[m_SubWorldIndex].m_dwCurrentTime)
			{
				if (g_RandPercent(m_AttackSkill[i].nRate))
				{
					KSkill * pSkill = (KSkill *) g_SkillManager.GetSkill(m_AttackSkill[i].nSkillId, m_AttackSkill[i].nSkillLevel);
					if(pSkill)
						pSkill->Cast(m_Index, -1, nLauncher);
					m_AttackSkill[i].dwNextCastTime = SubWorld[m_SubWorldIndex].m_dwCurrentTime + m_AttackSkill[i].nWaitCastTime;
				}
			}
		}
	}
}

void KNpc::DeathSkill()
{
	if (!m_Index)
		return;

	if (m_Doing == do_death || m_Doing == do_revive)
		return;

	for (int i = 0; i < MAX_AUTOSKILL; i ++)
	{
		if (m_DeathSkill[i].nSkillId > 0 && m_ReplySkill[i].nSkillId < MAX_SKILL && 
			m_DeathSkill[i].nSkillLevel > 0 && m_DeathSkill[i].nSkillLevel < MAX_SKILLLEVEL)
		{
			if (m_DeathSkill[i].dwNextCastTime < SubWorld[m_SubWorldIndex].m_dwCurrentTime)
			{
				if (g_RandPercent(m_DeathSkill[i].nRate))
				{
					this->Cast(m_DeathSkill[i].nSkillId, m_DeathSkill[i].nSkillLevel);
					m_DeathSkill[i].dwNextCastTime = SubWorld[m_SubWorldIndex].m_dwCurrentTime + m_DeathSkill[i].nWaitCastTime;
				}
			}
		}
	}
}
#endif

#ifdef _SERVER
BOOL KNpc::ReceiveDamage(int nLauncher, int nMissleSeries, BOOL bIsMelee, void *pData, BOOL bUseAR, int nDoHurtP, int nMissRate)
{
	if (!m_Index || !Npc[nLauncher].m_Index)
		return FALSE;

	if (!pData)
		return FALSE;
		
	if (m_Doing == do_death || m_Doing == do_revive)
		return TRUE;

	if (Npc[nLauncher].m_Doing == do_death || Npc[nLauncher].m_Doing == do_revive)
		return TRUE;

	int nRdc;
	KMagicAttrib *pTemp = NULL;

	pTemp = (KMagicAttrib *)pData;

	int nAr = pTemp->nValue[0];
	pTemp++;
	int nIgnoreAr = pTemp->nValue[0];
	
	if (bUseAR)
	{
		if (!CheckHitTarget(nAr, m_CurrentDefend, nIgnoreAr))
			return FALSE;
	}

	if (Npc[nLauncher].IsPlayer() && Npc[nLauncher].m_FightMode)
	{
		if (Npc[nLauncher].m_ActiveSkillID > 0 && Npc[nLauncher].m_ActiveSkillID < MAX_SKILL)
		{
			if (Npc[nLauncher].m_SkillList.GetLevel(Npc[nLauncher].m_ActiveSkillID) < MAX_TRAIN_SKILLEXPLEVEL)
			{
				KSkill * pSkill = (KSkill *) g_SkillManager.GetSkill(Npc[nLauncher].m_ActiveSkillID, 1);
				if (pSkill->IsExp() && pSkill->IsTargetEnemy())
				{
					if (Npc[nLauncher].m_SkillList.IncreaseExp(Npc[nLauncher].m_SkillList.GetSkillIdx(Npc[nLauncher].m_ActiveSkillID), 1 * g_SkillExpRate))
						Player[Npc[nLauncher].m_nPlayerIdx].UpdataCurData();

					PLAYER_SKILL_LEVEL_SYNC NewSkill;
					NewSkill.ProtocolType = s2c_playerskilllevel;
					NewSkill.m_nSkillID = Npc[nLauncher].m_ActiveSkillID;
					NewSkill.m_nSkillLevel = Npc[nLauncher].m_SkillList.GetLevel(Npc[nLauncher].m_ActiveSkillID);
					NewSkill.m_nAddLevel = Npc[nLauncher].m_SkillList.GetAddLevel(Npc[nLauncher].m_ActiveSkillID);
					NewSkill.m_nSkillExp = Npc[nLauncher].m_SkillList.GetExp(Npc[nLauncher].m_ActiveSkillID);
					NewSkill.m_bTempSkill = Npc[nLauncher].m_SkillList.IsTempSkill(Npc[nLauncher].m_ActiveSkillID);
					NewSkill.m_nLeavePoint = Player[Npc[nLauncher].m_nPlayerIdx].m_nSkillPoint;
					g_pServer->PackDataToClient(Player[Npc[nLauncher].m_nPlayerIdx].m_nNetConnectIdx, (BYTE*)&NewSkill, sizeof(PLAYER_SKILL_LEVEL_SYNC));
				}
			}
		}
	}

	pTemp++;
	int nMagicDamage = pTemp->nValue[0];
	
	pTemp++;
	int nFiveElementsDamageP = pTemp->nValue[0];

	pTemp++;
	BOOL bIsDS = FALSE;
	if (g_RandPercent(pTemp->nValue[0]))
		bIsDS = TRUE;

	pTemp++;
	BOOL bIsFS = FALSE;
	if (g_RandPercent(pTemp->nValue[0] - m_CurrentFatallyStrikeResP))
		bIsFS = TRUE;

	pTemp++;
	int nStolenLifeP = pTemp->nValue[0];
	
	pTemp++;
	int nStolenManaP = pTemp->nValue[0];

	pTemp++;
	int nStolenStaminaP = pTemp->nValue[0];

	pTemp++;
	CalcDamage(nLauncher, nMissleSeries, pTemp->nValue[0], pTemp->nValue[2], damage_physics, bIsMelee, FALSE, nFiveElementsDamageP, nStolenLifeP, nStolenManaP, nStolenStaminaP, bIsDS);

	pTemp++;
	if(CalcDamage(nLauncher, nMissleSeries, pTemp->nValue[0], pTemp->nValue[2], damage_cold, bIsMelee, FALSE, nFiveElementsDamageP, 0, 0, 0, FALSE, bIsFS))
	{
		nRdc = m_CurrentFreezeTimeReducePercent;
		if(nRdc >= MAX_PERCENT)
			nRdc = MAX_RESIST;
		if (m_FreezeState.nTime <= 0)
				m_FreezeState.nTime = pTemp->nValue[1] - (pTemp->nValue[1] * nRdc / MAX_PERCENT);
	}
	pTemp++;
	CalcDamage(nLauncher, nMissleSeries, pTemp->nValue[0], pTemp->nValue[2], damage_fire, bIsMelee, FALSE, nFiveElementsDamageP, 0, 0, 0, FALSE, bIsFS);

	pTemp++;
	CalcDamage(nLauncher, nMissleSeries, pTemp->nValue[0], pTemp->nValue[2], damage_light, bIsMelee, FALSE, nFiveElementsDamageP, 0, 0, 0, FALSE, bIsFS);

	pTemp++;
	if(CalcDamage(nLauncher, nMissleSeries, pTemp->nValue[0], pTemp->nValue[2], damage_poison, bIsMelee, FALSE, nFiveElementsDamageP, 0, 0, 0, FALSE, bIsFS))
	{
		nRdc = m_CurrentPoisonTimeReducePercent;
		if(nRdc >= MAX_PERCENT)
			nRdc = MAX_RESIST;
		if (m_PoisonState.nTime <= 0)
		{
			m_PoisonState.nTime = pTemp->nValue[1] - (pTemp->nValue[1] * nRdc / MAX_PERCENT);
			m_PoisonState.nValue[0] = pTemp->nValue[0];
			m_PoisonState.nValue[1] = pTemp->nValue[2];
		}
		else
		{
			int d1, d2, t1, t2, c1, c2;
			d1 = m_PoisonState.nValue[0];
			d2 = pTemp->nValue[0];
			t1 = m_PoisonState.nTime;
			t2 = pTemp->nValue[1] - (pTemp->nValue[1] * nRdc / MAX_PERCENT);
			c1 = m_PoisonState.nValue[1];
			c2 = pTemp->nValue[2];
			if (c1 > 0 && c2 > 0 && d1 > 0 && d2 > 0) 
			{
				m_PoisonState.nValue[0] = ((c1 + c2) * d1 / c1 + (c1 + c2) * d2 / c2) / 2;
				m_PoisonState.nTime = (t1 * d1 * c2 + t2 * d2 * c1) / (d1 * c2 + d2 * c1);
				m_PoisonState.nValue[1] = (c1 + c2) / 2;
			}
		}
	}
	pTemp++;
	if (m_StunState.nTime <= 0)
	{
		if (g_RandPercent(pTemp->nValue[0]))
		{
			nRdc = m_CurrentStunTimeReducePercent;
			if(nRdc >= MAX_PERCENT)
				nRdc = MAX_RESIST;
			m_StunState.nTime = pTemp->nValue[1] - (pTemp->nValue[1] * nRdc / MAX_PERCENT);
		}
	}
	pTemp++;
	if(g_RandPercent(nMissRate))
	{
		this->ClearNormalState();
		this->IgnoreState(TRUE);
	}

	pTemp++;
	if(pTemp->nValue[0] && pTemp->nValue[1] && g_RandPercent(nMissRate))
		return FALSE;

	if (g_RandPercent(nDoHurtP))
		DoHurt();

	m_nPeopleIdx = nLauncher;
	return TRUE;
}
#endif

void KNpc::SetImmediatelySkillEffect(int nLauncher, void *pData, int nDataNum)
{
	if (!pData || !nDataNum)
		return;

	KMagicAttrib*	pTemp = (KMagicAttrib *)pData;
	_ASSERT(nDataNum <= MAX_SKILL_STATE);
	for (int i = 0; i < nDataNum; i++)
	{
		ModifyAttrib(nLauncher, pTemp);
		pTemp++;
	}
}

void KNpc::AppendSkillEffect(int nSkillID, BOOL bIsPhysical, BOOL bIsMelee, void *pSrcData, void *pDesData)
{	
	int nMinDamage = m_PhysicsDamage.nValue[0] + m_CurrentAddPhysicsDamage;
	int	nMaxDamage = m_PhysicsDamage.nValue[2] + m_CurrentAddPhysicsDamage;
	int nAddDamageP = this->m_SkillList.GetAddSkillDamage(nSkillID) + this->m_CurrentSkillEnhancePercent;
	if(m_CurrentMana == m_CurrentManaMax)
		nAddDamageP += m_CurrentManaToSkillEnhanceP;

	KMagicAttrib* pTemp = (KMagicAttrib *)pSrcData;
	KMagicAttrib* pDes = (KMagicAttrib *)pDesData;
	if (pTemp->nAttribType == magic_attackrating_p)
	{
		pDes->nAttribType = magic_attackrating_v;
		pDes->nValue[0] = m_CurrentAttackRating + m_AttackRating * pTemp->nValue[0] / MAX_PERCENT;
	}
	else
	{
		pDes->nAttribType = magic_attackrating_v;
		pDes->nValue[0] = m_CurrentAttackRating;
	}
	pTemp++;
	pDes++;
	if (pTemp->nAttribType == magic_ignoredefense_p)
	{
		pDes->nAttribType = magic_ignoredefense_p;
		pDes->nValue[0] = pTemp->nValue[0];
	}
	pTemp++;
	pDes++;
	if (pTemp->nAttribType == magic_magicdamage_v)
	{
		pDes->nAttribType = magic_magicdamage_v;
		pDes->nValue[0] = pTemp->nValue[0] + (pTemp->nValue[0] * nAddDamageP / MAX_PERCENT);
		pDes->nValue[2] = pTemp->nValue[2] + (pTemp->nValue[2] * nAddDamageP / MAX_PERCENT);
	}
	pTemp++;
	pDes++;
	if (pTemp->nAttribType == magic_seriesdamage_p)
	{
		pDes->nAttribType = magic_seriesdamage_p;
		pDes->nValue[0] = pTemp->nValue[0];
		pDes->nValue[1] = pTemp->nValue[1];
		pDes->nValue[2] = pTemp->nValue[2];
	}
	pTemp++;
	pDes++;
	if (pTemp->nAttribType == magic_deadlystrike_p)
	{
		pDes->nAttribType = magic_deadlystrike_p;
		pDes->nValue[0] = pTemp->nValue[0];
		pDes->nValue[1] = pTemp->nValue[1];
		pDes->nValue[2] = pTemp->nValue[2];
	}
	if (bIsPhysical)
		pDes->nValue[0] += m_CurrentDeadlyStrikeEnhanceP;
	pTemp++;
	pDes++;
	if (pTemp->nAttribType == magic_fatallystrike_p)
	{
		pDes->nAttribType = magic_fatallystrike_p;
		pDes->nValue[0] = pTemp->nValue[0];
		pDes->nValue[1] = pTemp->nValue[1];
		pDes->nValue[2] = pTemp->nValue[2];
	}
	pDes->nValue[0] += m_CurrentFatallyStrikeEnhanceP;
	pTemp++;
	pDes++;
	if (pTemp->nAttribType == magic_steallife_p)
	{
		pDes->nAttribType = magic_steallife_p;
		pDes->nValue[0] = pTemp->nValue[0];
		pDes->nValue[1] = pTemp->nValue[1];
		pDes->nValue[2] = pTemp->nValue[2];
		
	}
	if (bIsPhysical)
		pDes->nValue[0] += m_CurrentLifeStolen;
	pTemp++;
	pDes++;
	if (pTemp->nAttribType == magic_stealmana_p)
	{
		pDes->nAttribType = magic_stealmana_p;
		pDes->nValue[0] = pTemp->nValue[0];
		pDes->nValue[1] = pTemp->nValue[1];
		pDes->nValue[2] = pTemp->nValue[2];
		
	}
	if (bIsPhysical)
		pDes->nValue[0] += m_CurrentManaStolen;
	pTemp++;
	pDes++;
	if (pTemp->nAttribType == magic_stealstamina_p)
	{
		pDes->nAttribType = magic_stealstamina_p;
		pDes->nValue[0] = pTemp->nValue[0];
		pDes->nValue[1] = pTemp->nValue[1];
		pDes->nValue[2] = pTemp->nValue[2];
		
	}
	if (bIsPhysical)
		pDes->nValue[0] += m_CurrentStaminaStolen;
	pTemp++;
	pDes++;
	if (pTemp->nAttribType == magic_physicsenhance_p)
	{
		pDes->nAttribType = magic_physicsdamage_v;
		pDes->nValue[0] = nMinDamage * (MAX_PERCENT + (pTemp->nValue[0] + (pTemp->nValue[0] * nAddDamageP / MAX_PERCENT))) / MAX_PERCENT;
		pDes->nValue[2] = nMaxDamage * (MAX_PERCENT + (pTemp->nValue[0] + (pTemp->nValue[0] * nAddDamageP / MAX_PERCENT))) / MAX_PERCENT;
		if (IsPlayer())
		{
			if (Player[m_nPlayerIdx].m_ItemList.GetWeaponType() == equip_meleeweapon)
			{
				pDes->nValue[0] += nMinDamage * m_CurrentMeleeEnhance[Player[m_nPlayerIdx].m_ItemList.GetWeaponParticular()] / MAX_PERCENT;
				pDes->nValue[2] += nMaxDamage * m_CurrentMeleeEnhance[Player[m_nPlayerIdx].m_ItemList.GetWeaponParticular()] / MAX_PERCENT;
			}
			else if (Player[m_nPlayerIdx].m_ItemList.GetWeaponType() == equip_rangeweapon)
			{
				pDes->nValue[0] += nMinDamage * m_CurrentRangeEnhance / MAX_PERCENT;
				pDes->nValue[2] += nMaxDamage * m_CurrentRangeEnhance / MAX_PERCENT;
			}
			else	// ����
			{
				pDes->nValue[0] += nMinDamage * m_CurrentHandEnhance / MAX_PERCENT;
				pDes->nValue[2] += nMaxDamage * m_CurrentHandEnhance / MAX_PERCENT;
			}
		}
	}
	else if (pTemp->nAttribType == magic_physicsdamage_v)
	{
		pDes->nAttribType = magic_physicsdamage_v;
		pDes->nValue[0] = pTemp->nValue[0] + (pTemp->nValue[0] * nAddDamageP / MAX_PERCENT);
		pDes->nValue[2] = pTemp->nValue[2] + (pTemp->nValue[2] * nAddDamageP / MAX_PERCENT);

		if (!bIsPhysical)
	    {
			pDes->nValue[0] += m_PhysicsMagic.nValue[0] + m_CurrentAddPhysicsMagic;
			pDes->nValue[2] += m_PhysicsMagic.nValue[2] + m_CurrentAddPhysicsMagic;
	    }
	}
	pTemp++;
	pDes++;
	if (pTemp->nAttribType == magic_colddamage_v)
	{
		pDes->nAttribType = magic_colddamage_v;
		pDes->nValue[0] = pTemp->nValue[0] + (pTemp->nValue[0] * nAddDamageP / MAX_PERCENT);
		pDes->nValue[1] = pTemp->nValue[1] + m_CurrentColdEnhance;
		pDes->nValue[2] = pTemp->nValue[2] + (pTemp->nValue[2] * nAddDamageP / MAX_PERCENT);
	
		if (!bIsPhysical)
		{
			pDes->nValue[0] += m_PhysicsMagic.nValue[0] + m_CurrentColdMagic.nValue[0];
			pDes->nValue[1] = max(pDes->nValue[1], m_CurrentColdMagic.nValue[1] + m_CurrentColdEnhance);
			pDes->nValue[2] += m_PhysicsMagic.nValue[0] + m_CurrentColdMagic.nValue[2];
		}	
	}
	if (bIsPhysical)
	{
		pDes->nValue[0] += m_CurrentColdDamage.nValue[0];
		pDes->nValue[1] = max(pDes->nValue[1], m_CurrentColdDamage.nValue[1] + m_CurrentColdEnhance);
		pDes->nValue[2] += m_CurrentColdDamage.nValue[2];		
	}
	pTemp++;
	pDes++;
	if (pTemp->nAttribType == magic_firedamage_v)
	{
		pDes->nAttribType = magic_firedamage_v;
		pDes->nValue[0] = pTemp->nValue[0] + (pTemp->nValue[0] * nAddDamageP / MAX_PERCENT);
		pDes->nValue[2] = pTemp->nValue[2] + (pTemp->nValue[2] * nAddDamageP / MAX_PERCENT);

		if (!bIsPhysical)
		{
			pDes->nValue[0] += m_PhysicsMagic.nValue[0] + m_CurrentFireMagic.nValue[0];
			pDes->nValue[2] += m_PhysicsMagic.nValue[2] + m_CurrentFireMagic.nValue[2];
		} 
	}
	if (bIsPhysical)
	{
		pDes->nValue[0] += m_CurrentFireDamage.nValue[0];
		pDes->nValue[2] += m_CurrentFireDamage.nValue[2];
	}
	pTemp++;
	pDes++;
	if (pTemp->nAttribType == magic_lightingdamage_v)
	{
		pDes->nAttribType = magic_lightingdamage_v;
		pDes->nValue[0] = pTemp->nValue[0] + (pTemp->nValue[2] - pTemp->nValue[0]) * m_CurrentLightEnhance / MAX_PERCENT;
		pDes->nValue[2] = pTemp->nValue[2] + (pTemp->nValue[2] * nAddDamageP / MAX_PERCENT);

		if (!bIsPhysical)
		{
			pDes->nValue[0] += m_PhysicsMagic.nValue[0] + m_CurrentLightMagic.nValue[0];
			pDes->nValue[2] += m_PhysicsMagic.nValue[2] + m_CurrentLightMagic.nValue[2];
		}
	
	}
	if (bIsPhysical)
	{
		pDes->nValue[0] += m_CurrentLightDamage.nValue[0];
		pDes->nValue[2] += m_CurrentLightDamage.nValue[2];
	}
	pTemp++;
	pDes++;
	if (pTemp->nAttribType == magic_poisondamage_v)
	{
		pDes->nAttribType = magic_poisondamage_v;
		pDes->nValue[0] = pTemp->nValue[0] + (pTemp->nValue[0] * nAddDamageP / MAX_PERCENT);
		pDes->nValue[1] = pTemp->nValue[1];
		pDes->nValue[2] = pTemp->nValue[2] * (MAX_PERCENT - m_CurrentPoisonEnhance) / MAX_PERCENT;
		if (pDes->nValue[2] <= 0)
			pDes->nValue[2] = 1;

		if (!bIsPhysical)
		{
			g_NpcAttribModify.MixPoisonDamage(pDes, &m_CurrentPoisonMagic);
			pDes->nValue[0] += m_PhysicsMagic.nValue[0];
			pDes->nValue[2] += m_PhysicsMagic.nValue[2];
		}
	}
	if (bIsPhysical)
		g_NpcAttribModify.MixPoisonDamage(pDes, &m_CurrentPoisonDamage);
	pTemp++;
	pDes++;
	if (pTemp->nAttribType == magic_stun_p)
	{
		pDes->nAttribType = magic_stun_p;
		pDes->nValue[0] = pTemp->nValue[0];
		pDes->nValue[1] = pTemp->nValue[1];
		pDes->nValue[2] = pTemp->nValue[2];
	}
	pTemp++;
	pDes++;
	if (pTemp->nAttribType == magic_ignorenegativestate_p)
	{
		pDes->nAttribType = magic_ignorenegativestate_p;
		pDes->nValue[0] = pTemp->nValue[0];
		pDes->nValue[1] = pTemp->nValue[1];
		pDes->nValue[2] = pTemp->nValue[2];
	}
	pTemp++;
	pDes++;
	if (pTemp->nAttribType == magic_randmove)
	{
		pDes->nAttribType = magic_randmove;
		pDes->nValue[0] = pTemp->nValue[0];
		pDes->nValue[1] = pTemp->nValue[1];
		pDes->nValue[2] = pTemp->nValue[2];
	}
}

void KNpc::ServerMove(int MoveSpeed)
{
	if (m_Doing != do_walk && m_Doing != do_run && m_Doing != do_hurt && m_Doing != do_runattack)
		return;

	if (MoveSpeed <= 0)
		return;

	if (MoveSpeed >= SubWorld[m_SubWorldIndex].m_nCellWidth)
	{
		MoveSpeed = SubWorld[m_SubWorldIndex].m_nCellWidth - 1;
	}

#ifndef _SERVER
	if (m_RegionIndex < 0 || m_RegionIndex >= 9)
	{
		g_DebugLog("Npc (%d) ServerMove RegionIdx = %d", m_Index, m_RegionIndex);
		_ASSERT(0);
		DoStand();
		return;
	}
#else
	_ASSERT(m_RegionIndex >= 0);
	if (m_RegionIndex < 0)
		return;
#endif
	int x, y;

	SubWorld[m_SubWorldIndex].Map2Mps(m_RegionIndex, m_MapX, m_MapY, 0, 0, &x, &y);
	x = (x << 10) + m_OffX;
	y = (y << 10) + m_OffY;

	int nRet = m_PathFinder.GetDir(x, y, m_Dir, m_DesX, m_DesY, MoveSpeed, &m_Dir);

#ifndef _SERVER
	if(nRet == 1)
	{
		x = g_DirCos(m_Dir, 64) * MoveSpeed;
		y = g_DirSin(m_Dir, 64) * MoveSpeed;
	}
	else if (nRet == 0)
	{
		DoStand();
		return;
	}
	else if (nRet == -1)
	{
		SubWorld[0].m_Region[m_RegionIndex].RemoveNpc(m_Index);
		SubWorld[0].m_Region[m_RegionIndex].DecRef(m_MapX, m_MapY, obj_npc);
		m_RegionIndex = -1;
		return;
	}
	else
	{
		return;
	}
#endif
#ifdef _SERVER
	if(nRet == 1)
	{
		x = g_DirCos(m_Dir, 64) * MoveSpeed;
		y = g_DirSin(m_Dir, 64) * MoveSpeed;
	}
	else
	{
		DoStand();
		return;
	}
#endif

	int nOldRegion = m_RegionIndex;
	int nOldMapX = m_MapX;
	int nOldMapY = m_MapY;
	int nOldOffX = m_OffX;
	int nOldOffY = m_OffY;

	m_OffX += x;
	m_OffY += y;

	if (!m_bClientOnly)
		CURREGION.DecRef(m_MapX, m_MapY, obj_npc);
	
	if (m_OffX < 0)
	{
		m_MapX--;
		m_OffX += CELLWIDTH;
	}
	else if (m_OffX > CELLWIDTH)
	{
		m_MapX++;
		m_OffX -= CELLWIDTH;
	}

	if (m_OffY < 0)
	{
		m_MapY--;
		m_OffY += CELLHEIGHT;
	}
	else if (m_OffY > CELLHEIGHT)
	{
		m_MapY++;
		m_OffY -= CELLHEIGHT;
	}
	
	if (m_MapX < 0)
	{
		m_RegionIndex = LEFTREGIONIDX;
		m_MapX += REGIONWIDTH;
	}
	else if (m_MapX >= REGIONWIDTH)
	{
		m_RegionIndex = RIGHTREGIONIDX;
		m_MapX -= REGIONWIDTH;
	}

	if (m_RegionIndex >= 0)
	{
		if (m_MapY < 0)
		{
			m_RegionIndex = UPREGIONIDX;
			m_MapY += REGIONHEIGHT;
		}
		else if (m_MapY >= REGIONHEIGHT)
		{
			m_RegionIndex = DOWNREGIONIDX;
			m_MapY -= REGIONHEIGHT;
		}
		if (!m_bClientOnly && m_RegionIndex >= 0)
			CURREGION.AddRef(m_MapX, m_MapY, obj_npc);
	}

	if (m_RegionIndex == -1)
	{
		m_RegionIndex = nOldRegion;
		m_MapX = nOldMapX;
		m_MapY = nOldMapY;
		m_OffX = nOldOffX;
		m_OffY = nOldOffY;
		CURREGION.AddRef(m_MapX, m_MapY, obj_npc);
		return;
	}

	if (nOldRegion != m_RegionIndex)
	{
#ifdef _SERVER
		SubWorld[m_SubWorldIndex].NpcChangeRegion(nOldRegion, m_RegionIndex, m_Index);
		if (IsPlayer())
		{
			SubWorld[m_SubWorldIndex].PlayerChangeRegion(nOldRegion, m_RegionIndex, m_nPlayerIdx);
			if (m_nPlayerIdx > 0)
			{
				Player[m_nPlayerIdx].m_ItemList.Abrade(enumAbradeMove);
			}
		}
#else
		SubWorld[0].NpcChangeRegion(SubWorld[0].m_Region[nOldRegion].m_RegionID, SubWorld[0].m_Region[m_RegionIndex].m_RegionID, m_Index);
		m_dwRegionID = SubWorld[0].m_Region[m_RegionIndex].m_RegionID;
#endif
	}
}

void KNpc::ServerJump(int nSpeed)
{
	_ASSERT(m_RegionIndex >= 0);
	if (m_RegionIndex < 0)
		return;

	if (!(m_Doing == do_jump || m_Doing == do_jumpattack))
		return;
	
	if (nSpeed <= 0)
		return;

	if (nSpeed >= SubWorld[m_SubWorldIndex].m_nCellWidth)
	{
		nSpeed = SubWorld[m_SubWorldIndex].m_nCellWidth - 1;
	}

	m_OffX += g_DirCos(m_JumpDir, 64) * nSpeed;
	m_OffY += g_DirSin(m_JumpDir, 64) * nSpeed;

	// s = vt - a * t * t / 2
	m_Height = (m_JumpFirstSpeed * m_Frames.nCurrentFrame - ACCELERATION_OF_GRAVITY * m_Frames.nCurrentFrame * m_Frames.nCurrentFrame / 2) / 8;
	if (m_Height < 0)
		m_Height = 0;

	int nOldRegion = m_RegionIndex;
	int nOldMapX = m_MapX;
	int nOldMapY = m_MapY;
	int nOldOffX = m_OffX;
	int nOldOffY = m_OffY;
	CURREGION.DecRef(m_MapX, m_MapY, obj_npc);
	
	if (m_OffX < 0)
	{
		m_MapX--;
		m_OffX += CELLWIDTH;
	}
	else if (m_OffX > CELLWIDTH)
	{
		m_MapX++;
		m_OffX -= CELLWIDTH;
	}
	
	if (m_OffY < 0)
	{
		m_MapY--;
		m_OffY += CELLHEIGHT;
	}
	else if (m_OffY > CELLHEIGHT)
	{
		m_MapY++;
		m_OffY -= CELLHEIGHT;
	}
	
	if (m_MapX < 0)
	{
		m_RegionIndex = LEFTREGIONIDX;
		m_MapX += REGIONWIDTH;
	}
	else if (m_MapX >= REGIONWIDTH)
	{
		m_RegionIndex = RIGHTREGIONIDX;
		m_MapX -= REGIONWIDTH;
	}
	
	if (m_RegionIndex >= 0)
	{
		if (m_MapY < 0)
		{
			m_RegionIndex = UPREGIONIDX;
			m_MapY += REGIONHEIGHT;
		}
		else if (m_MapY >= REGIONHEIGHT)
		{
			m_RegionIndex = DOWNREGIONIDX;
			m_MapY -= REGIONHEIGHT;
		}
		if (m_RegionIndex >= 0)
			CURREGION.AddRef(m_MapX, m_MapY, obj_npc);
	}

	if (m_RegionIndex == -1)	// �������ƶ���-1 Region�������������������ָ�ԭ����
	{
		m_RegionIndex = nOldRegion;
		m_MapX = nOldMapX;
		m_MapY = nOldMapY;
		m_OffX = nOldOffX;
		m_OffY = nOldOffY;
		CURREGION.AddRef(m_MapX, m_MapY, obj_npc);
		return;
	}

	if (nOldRegion != m_RegionIndex)
	{
#ifdef _SERVER
		SubWorld[m_SubWorldIndex].NpcChangeRegion(nOldRegion, m_RegionIndex, m_Index);
		if (IsPlayer())
		{
			SubWorld[m_SubWorldIndex].PlayerChangeRegion(nOldRegion, m_RegionIndex, m_nPlayerIdx);
			if (m_nPlayerIdx > 0)
			{
				Player[m_nPlayerIdx].m_ItemList.Abrade(enumAbradeMove);
			}
		}
#else
		if (m_RegionIndex >= 0)
		{
			SubWorld[0].NpcChangeRegion(SubWorld[0].m_Region[nOldRegion].m_RegionID, SubWorld[0].m_Region[m_RegionIndex].m_RegionID, m_Index);
			m_dwRegionID = SubWorld[0].m_Region[m_RegionIndex].m_RegionID;
		}
#endif
	}		
}

void KNpc::SendCommand(NPCCMD cmd,int x,int y, int z)
{
	if(cmd == do_run)
	{
		if ((m_CurrentStamina < PlayerSet.m_cPlayerStamina.m_nKillRunSub && m_nPKFlag == enumPKMurder) || 
			(m_CurrentStamina < PlayerSet.m_cPlayerStamina.m_nTongWarRunSub && m_nPKFlag == enumPKTongWar))
			cmd = do_walk;
	}

	if (m_FrozenAction.nTime > 0)
	{
		if (cmd == do_walk || 
			cmd == do_run || 
			cmd == do_runattack || 
			cmd == do_jump || 
			cmd == do_jumpattack || 
			cmd == do_skill || 
			cmd == do_magic || 
			cmd == do_attack || 
			cmd == do_blurmove)
		{
			return;
		}
	}

	if (m_RandMove.nTime > 0)
	{
		if (cmd == do_run || 
			cmd == do_runattack || 
			cmd == do_jump || 
			cmd == do_jumpattack || 
			cmd == do_skill || 
			cmd == do_magic || 
			cmd == do_attack || 
			cmd == do_blurmove)
		{
			return;
		}
	}

	m_Command.CmdKind = cmd;
	m_Command.Param_X = x;
	m_Command.Param_Y = y;
	m_Command.Param_Z = z;

}

BOOL KNpc::NewPath(int nMpsX, int nMpsY)
{
	m_DesX = nMpsX;
	m_DesY = nMpsY;
	return TRUE;
}

BOOL KNpc::NewJump(int nMpsX, int nMpsY)
{
	_ASSERT(m_CurrentJumpSpeed > 0);
	if (m_CurrentJumpSpeed <= 0)
		return FALSE;
	
	int nX, nY;
	GetMpsPos(&nX, &nY);

	if (nX == nMpsX && nY == nMpsY)
		return FALSE;

	int nDir = g_GetDirIndex(nX, nY, nMpsX, nMpsY);
	int	nMaxLength = m_CurrentJumpSpeed * m_CurrentJumpFrame;
	int	nWantLength = g_GetDistance(nX, nY, nMpsX, nMpsY);
	int	nSin = g_DirSin(nDir, 64);
	int	nCos = g_DirCos(nDir, 64);

	if (nWantLength > nMaxLength)
	{
		m_DesX = nX + ((nMaxLength * nCos) >> 10);
		m_DesY = nY + ((nMaxLength * nSin) >> 10);
		nWantLength = nMaxLength;
	}
	else if (nWantLength <= MIN_DOMELEE_RANGE)
	{
		m_DesX = nMpsX;
		m_DesY = nMpsY;
		return FALSE;
	}

	m_JumpStep = nWantLength / m_CurrentJumpSpeed;

	int nTestX = 0;
	int nTestY = 0;
	int nSuccessStep = 0;

	for (int i = 1; i < m_JumpStep + 1; i++)
	{
		nTestX = nX + ((m_CurrentJumpSpeed * nCos * i) >> 10);
		nTestY = nY + ((m_CurrentJumpSpeed * nSin * i) >> 10);
		int nBarrier = SubWorld[m_SubWorldIndex].GetBarrier(nTestX, nTestY);
		DWORD	dwTrap = SubWorld[m_SubWorldIndex].GetTrap(nTestX, nTestY);
		if (Obstacle_NULL == nBarrier && dwTrap == 0)
		{
			nSuccessStep = i;
		}
		if (Obstacle_Normal == nBarrier || Obstacle_Fly == nBarrier || dwTrap)
		{
			if (nSuccessStep <= MIN_DOMELEE_RANGE / m_CurrentJumpSpeed)
			{
				return FALSE;
			}
			m_DesX = nX + ((m_CurrentJumpSpeed * nCos * nSuccessStep) >> 10);
			m_DesY = nY + ((m_CurrentJumpSpeed * nSin * nSuccessStep) >> 10);
			m_JumpStep = nSuccessStep;
			break;
		}
	}
	m_JumpDir = nDir;
	return TRUE;
}

BOOL KNpc::Cost(NPCATTRIB nType, int nCost, BOOL bIsAudit, BOOL bNotShowMessage)
{
	if (!IsPlayer())
		return TRUE;

	int *pSource = NULL;
	int nCurCost = nCost;
	switch(nType)
	{
	case attrib_mana_v:
		pSource = &m_CurrentMana;
		break;
	case attrib_mana_p:
		pSource = &m_CurrentMana;
		nCurCost = m_CurrentManaMax * nCost / MAX_PERCENT;
		break;
	case attrib_life_v:
		pSource = &m_CurrentLife;
		break;
	case attrib_life_p:
		pSource = &m_CurrentLife;
		nCurCost = m_CurrentLifeMax * nCost / MAX_PERCENT;
		break;
	case attrib_stamina_v:
		pSource = &m_CurrentStamina;
		break;
	case attrib_stamina_p:
		pSource = &m_CurrentStamina;
		nCurCost = m_CurrentStaminaMax * nCost / MAX_PERCENT;
		break;
	default:
		break;
	}

	if (pSource)
	{
		if ((nType== attrib_life_v || nType== attrib_life_p)? ((*pSource-1) < nCurCost):(*pSource < nCurCost))
		{
			if (!bNotShowMessage)
			{
#ifndef _SERVER
				KSystemMessage Msg;

				Msg.byConfirmType = SMCT_NONE;
				Msg.byParamSize = 0;
				Msg.byPriority = 1;
				Msg.eType = SMT_NORMAL;
				switch(nType)
				{
				case attrib_mana_v:
				case attrib_mana_p:
					g_StrCpyLen(Msg.szMessage, MSG_NPC_NO_MANA, sizeof(Msg.szMessage));
					break;
				case attrib_life_v:
				case attrib_life_p:
					g_StrCpyLen(Msg.szMessage, MSG_NPC_NO_LIFE, sizeof(Msg.szMessage));
					break;
				case attrib_stamina_v:
				case attrib_stamina_p:
					g_StrCpyLen(Msg.szMessage, MSG_NPC_NO_STAMINA, sizeof(Msg.szMessage));
					break;
				default:
					break;
				}
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, NULL);
#endif
			}
			return FALSE;
		}
		else
		{
#ifdef _SERVER
			if (!bIsAudit)
				*pSource -= nCurCost;
#endif
			return TRUE;
		}
	}
	return FALSE;
}

#ifdef _SERVER
void KNpc::Cast(int nSkillId, int nSkillLevel)
{
	if (nSkillId < MAX_SKILL && nSkillLevel < MAX_SKILLLEVEL)
	{
		int nMpsX, nMpsY;
		SubWorld[m_SubWorldIndex].Map2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, &nMpsX, &nMpsY);		
		_ASSERT(nSkillId < MAX_SKILL && nSkillLevel < MAX_SKILLLEVEL);

		NPC_SKILL_SYNC SkillCmd;
		SkillCmd.ID = this->m_dwID;
		SkillCmd.nSkillID = nSkillId;	
		SkillCmd.nSkillLevel = nSkillLevel;
		SkillCmd.nMpsX = -1;
		SkillCmd.nMpsY = m_dwID;
		SkillCmd.ProtocolType = s2c_castskilldirectly;
		
		POINT	POff[8] = 
		{
			{0, 32},
			{-16, 32},
			{-16, 0},
			{-16, -32},
			{0, -32},
			{16, -32},
			{16, 0},
			{16, 32},
		};
		
		int nMaxCount = MAX_BROADCAST_COUNT;
		CURREGION.BroadCast(&SkillCmd, sizeof(SkillCmd), nMaxCount, m_MapX, m_MapY);
		int i = 0;
		for (i = 0; i < 8; i++)
		{
			if (CONREGIONIDX(i) == -1)
				continue;
			CONREGION(i).BroadCast(&SkillCmd, sizeof(SkillCmd), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
		}
			
		KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(nSkillId, nSkillLevel);
		pOrdinSkill->Cast(m_Index, nMpsX, nMpsY);

		if(!pOrdinSkill->IsAura())
		{
			DWORD dwCastTime = 0;
			eSkillStyle eStyle = (eSkillStyle)pOrdinSkill->GetSkillStyle();
			if (eStyle == SKILL_SS_Missles 
				|| eStyle == SKILL_SS_Melee 
				|| eStyle == SKILL_SS_InitiativeNpcState 
				|| eStyle == SKILL_SS_PassivityNpcState)
			{
				dwCastTime = pOrdinSkill->GetDelayPerCast(m_bRideHorse);
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
			m_SkillList.SetNextCastTime(nSkillId, SubWorld[m_SubWorldIndex].m_dwCurrentTime, SubWorld[m_SubWorldIndex].m_dwCurrentTime + dwCastTime);
		}
	}
}
#endif

void KNpc::DoJump()
{
	_ASSERT(m_RegionIndex >= 0);
	if (m_RegionIndex < 0)
		return;

	if (m_Doing == do_jump)
		return;
#ifndef _SERVER
	if(m_WalkRun.nTime)
		m_DataRes.SetBlur(TRUE);
#endif
	m_Doing = do_jump;
	m_Dir = m_JumpDir;
	m_ProcessAI	= 0;
	m_JumpFirstSpeed = ACCELERATION_OF_GRAVITY * (m_JumpStep - 1) / 2 ;
#ifdef _SERVER	// ����Χ9��Region�㲥������
	NPC_JUMP_SYNC	NetCommand;
	NetCommand.ProtocolType = (BYTE)s2c_npcjump;
	NetCommand.ID = m_dwID;
	NetCommand.nMpsX = m_DesX;
	NetCommand.nMpsY = m_DesY;
		
	POINT	POff[8] = 
	{
		{0, 32},
		{-16, 32},
		{-16, 0},
		{-16, -32},
		{0, -32},
		{16, -32},
		{16, 0},
		{16, 32},
	};
	int nMaxCount = MAX_BROADCAST_COUNT;
	CURREGION.BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX, m_MapY);
	for (int i= 0; i < 8; i++)
	{
		if (CONREGIONIDX(i) == -1)
			continue;
		CONREGION(i).BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
	}
#endif

#ifndef _SERVER
		m_ClientDoing = cdo_jump;
#endif

	m_Frames.nTotalFrame = m_JumpStep;
	m_Frames.nCurrentFrame = 0;	
}

BOOL KNpc::OnJump()
{
	ServerJump(m_CurrentJumpSpeed);
	if (WaitForFrame())
	{
		DoStand();
		m_ProcessAI	= 1;
//#ifndef _SERVER	
//		m_DataRes.SetBlur(FALSE);
//#endif
		return FALSE;
	}
	return TRUE;
}

BOOL KNpc::WaitForFrame()
{
	m_Frames.nCurrentFrame++;
	if (m_Frames.nCurrentFrame < m_Frames.nTotalFrame)
	{
		return FALSE;
	}
	m_Frames.nCurrentFrame = 0;
	return TRUE;
}

BOOL KNpc::IsReachFrame(int nPercent)
{
	if (m_Frames.nCurrentFrame == m_Frames.nTotalFrame * nPercent / MAX_PERCENT)
	{
		return TRUE;
	}
	return FALSE;
}

//�ͻ��˴�����õ���NpcSettingIdx�ǰ�����16λNpc��ģ������16λΪ�ȼ�
void KNpc::Load(int nNpcSettingIdx, int nLevel)
{
	m_PathFinder.Init(m_Index);
	if (nLevel <= 0) 
	{
		nLevel = 1;
	}
	m_NpcSettingIdx = nNpcSettingIdx;
	m_Level = nLevel;

#ifndef _SERVER
	char	szNpcTypeName[32];
#endif
	if (nNpcSettingIdx == PLAYER_MALE_NPCTEMPLATEID || nNpcSettingIdx == PLAYER_FEMALE_NPCTEMPLATEID)
	{
#ifndef _SERVER
		if (nNpcSettingIdx == PLAYER_MALE_NPCTEMPLATEID)
		{
			strcpy(szNpcTypeName, "MainMan");
			m_StandFrame = NpcSet.GetPlayerStandFrame(TRUE);
			m_WalkFrame = NpcSet.GetPlayerWalkFrame(TRUE);
			m_RunFrame = NpcSet.GetPlayerRunFrame(TRUE);
		}
		else
		{
			strcpy(szNpcTypeName, "MainLady");
			m_StandFrame = NpcSet.GetPlayerStandFrame(FALSE);
			m_WalkFrame = NpcSet.GetPlayerWalkFrame(FALSE);
			m_RunFrame = NpcSet.GetPlayerRunFrame(FALSE);
		}
#endif
		//		TODO: Load Player Data;
		m_WalkSpeed = NpcSet.GetPlayerWalkSpeed();
		m_RunSpeed = NpcSet.GetPlayerRunSpeed();
		m_AttackFrame = NpcSet.GetPlayerAttackFrame();
		m_HurtFrame	= NpcSet.GetPlayerHurtFrame();
	}
	else
	{
		GetNpcCopyFromTemplate(nNpcSettingIdx);

#ifndef _SERVER	
		g_NpcSetting.GetString(nNpcSettingIdx + 2, "NpcResType", "", szNpcTypeName, sizeof(szNpcTypeName));
		if (!szNpcTypeName[0])
		{
			g_NpcKindFile.GetString(2, "CharacterName", "", szNpcTypeName, sizeof(szNpcTypeName));//���û�ҵ����õ�һ��npc����
		}
		g_NpcSetting.GetInteger(nNpcSettingIdx + 2, "AIMode", 12, &m_AiMode);
		g_NpcSetting.GetInteger(nNpcSettingIdx + 2, "AIParam1", 12, &m_AiParam[0]);
		g_NpcSetting.GetInteger(nNpcSettingIdx + 2, "AIParam2", 12, &m_AiParam[1]);
		g_NpcSetting.GetInteger(nNpcSettingIdx + 2, "AIParam3", 12, &m_AiParam[2]);
		g_NpcSetting.GetInteger(nNpcSettingIdx + 2, "AIParam4", 12, &m_AiParam[3]);
		g_NpcSetting.GetInteger(nNpcSettingIdx + 2, "AIParam5", 12, &m_AiParam[4]);
		g_NpcSetting.GetInteger(nNpcSettingIdx + 2, "AIParam6", 12, &m_AiParam[5]);
		g_NpcSetting.GetInteger(nNpcSettingIdx + 2, "AIParam7", 12, &m_AiParam[6]);
		g_NpcSetting.GetInteger(nNpcSettingIdx + 2, "ActiveRadius", 12, &m_ActiveRadius);
		g_NpcSetting.GetInteger(nNpcSettingIdx + 2, "ClientOnly", 0, &m_bClientOnly);
		// �����࣬11��12��17����AiParam[6]����߻��趨�߶�
		// add by flying
		if (m_AiMode == 11 || m_AiMode == 12 || m_AiMode == 17)
			m_AiParam[6] = m_AiMode;
#endif
	}
#ifndef _SERVER
	this->RemoveRes();
	m_DataRes.Init(szNpcTypeName, &g_NpcResList);
	
	m_DataRes.SetAction(m_ClientDoing);
	if(m_bRideHorse && m_ClientDoing == cdo_jump)
		m_DataRes.SetRideHorse(FALSE);
	else
		m_DataRes.SetRideHorse(m_bRideHorse);
	m_DataRes.SetArmor(m_ArmorType, m_MantleType);
	m_DataRes.SetHelm(m_HelmType);
	m_DataRes.SetHorse(m_HorseType);
	m_DataRes.SetWeapon(m_WeaponType);
#endif
	m_CurrentCamp = m_Camp;
}

void KNpc::GetMpsPos(int *pPosX, int *pPosY)
{
#ifdef _SERVER
	SubWorld[m_SubWorldIndex].Map2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, pPosX, pPosY);
#else
	SubWorld[m_SubWorldIndex].Map2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, pPosX, pPosY);
#endif
}

BOOL	KNpc::SetActiveSkill(int nSkillIdx)
{
	if (nSkillIdx <= 0 || nSkillIdx >= MAX_NPCSKILL)
		return FALSE;

	if (m_SkillList.m_Skills[nSkillIdx].SkillId <= 0 || m_SkillList.m_Skills[nSkillIdx].CurrentSkillLevel <= 0)
		return FALSE;

	m_ActiveSkillID = m_SkillList.m_Skills[nSkillIdx].SkillId;

	int nCurLevel = m_SkillList.m_Skills[nSkillIdx].CurrentSkillLevel;

	_ASSERT(m_ActiveSkillID < MAX_SKILL && nCurLevel < MAX_SKILLLEVEL && nCurLevel > 0);
	
	ISkill * pISkill =  g_SkillManager.GetSkill(m_ActiveSkillID, nCurLevel);
	if (pISkill)
    {
		m_CurrentAttackRadius = pISkill->GetAttackRadius();
    }
	return TRUE;
}

void KNpc::SetAuraSkill(int nSkillID)
{
	if (nSkillID <= 0 || nSkillID >= MAX_SKILL) 
    {
        nSkillID = 0;
    }
	else
	{
		int  nCurLevel = m_SkillList.GetCurrentLevel(nSkillID);
		if (nCurLevel <= 0)
        {
            nSkillID = 0;
        }
		else
		{
			_ASSERT(nSkillID < MAX_SKILL && nCurLevel < MAX_SKILLLEVEL);
			
			KSkill * pOrdinSkill = (KSkill *)g_SkillManager.GetSkill(nSkillID, nCurLevel);
            if (!pOrdinSkill || !pOrdinSkill->IsAura())
			{
				nSkillID  = 0;
			}
		}
	}
	m_ActiveAuraID = nSkillID;

#ifndef _SERVER
	SKILL_CHANGEAURASKILL_COMMAND ChangeAuraMsg;
	ChangeAuraMsg.ProtocolType = c2s_changeauraskill;
	ChangeAuraMsg.m_nAuraSkill = m_ActiveAuraID;
	if (g_pClient)
		g_pClient->SendPackToServer(&ChangeAuraMsg, sizeof(SKILL_CHANGEAURASKILL_COMMAND));
#else
	UpdateNpcStateInfo();
#endif
}

BOOL KNpc::SetPlayerIdx(int nIdx)
{
	if (nIdx <= 0 || nIdx >= MAX_PLAYER)
		return FALSE;

	if (m_Kind != kind_player)
		return FALSE;

	m_nPlayerIdx = nIdx;
	return TRUE;
}

void KNpc::SwitchMaskFeature()
{
	m_bMaskFeature = !m_bMaskFeature;
#ifdef _SERVER
	int nIdx = Player[m_nPlayerIdx].m_ItemList.GetEquipment(itempart_mask);
	if (m_bMaskFeature)
	{
		if (nIdx && !Player[m_nPlayerIdx].m_ItemList.GetMaskLock())
			m_MaskType = 0;
	}
	else
	{
		if (nIdx && !Player[m_nPlayerIdx].m_ItemList.GetMaskLock())
		{
			switch (Item[nIdx].m_CommonAttrib.nItemNature)
			{
			case NATURE_GOLD:
				m_MaskType = g_ItemChangeRes.GetGoldItemRes(Item[nIdx].GetDetailType(), Item[nIdx].GetRow());
				break;
			case NATURE_PLATINA:
				break;
			default:
				g_MaskChangeRes.GetInteger(Item[nIdx].GetParticular() + 2, 2, 0, &m_MaskType);
				break;
			}
		}
	}			
#endif

#ifdef _SERVER
	S2C_PLAYER_SYNC	sMsg;
	sMsg.ProtocolType = s2c_playersync;
	sMsg.m_wLength = sizeof(S2C_PLAYER_SYNC) - 1;
	sMsg.m_lpBuf = 0;
	sMsg.m_wMsgID = enumS2C_PLAYERSYNC_ID_MASKFEATURE;
	g_pServer->PackDataToClient(Player[m_nPlayerIdx].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
#endif
}

#ifdef _SERVER
BOOL KNpc::SendSyncData(int nClient, BOOL bBroadCast/* = FALSE*/)
{
	BOOL	bRet = FALSE;
	NPC_SYNC	NpcSync;

	NpcSync.ProtocolType		= (BYTE)s2c_syncnpc;
	NpcSync.m_btKind			= (BYTE)m_Kind;
	NpcSync.Camp				= (BYTE)m_Camp;
	NpcSync.CurrentCamp			= (BYTE)m_CurrentCamp;
	NpcSync.m_bySeries			= (BYTE)m_Series;
	NpcSync.m_Doing				= (BYTE)m_Doing;

	NpcSync.CurrentLifeMax		= (int)m_CurrentLifeMax;
	if (m_CurrentLifeMax > 0)
		NpcSync.CurrentLife		= (int)m_CurrentLife;
	else
		NpcSync.CurrentLife		= 0;

	if (this->IsPlayer())
		NpcSync.m_btMenuState	= (BYTE)Player[this->m_nPlayerIdx].m_cMenuState.m_nState;
	else
		NpcSync.m_btMenuState	= 0;
	NpcSync.Dir = m_Dir;
	GetMpsPos((int *)&NpcSync.MapX, (int *)&NpcSync.MapY);
	NpcSync.ID					= m_dwID;
	NpcSync.NpcSettingIdx		= MAKELONG(m_Level, m_NpcSettingIdx);

	NpcSync.Special				= (BYTE)m_btSpecial;
	NpcSync.MissionGroup		= (int)m_nMissionGroup;
	strcpy(NpcSync.m_szName, m_Kind == kind_player?(Player[m_nPlayerIdx].m_bForbidName?"":Name):Name);
	NpcSync.m_wLength			= sizeof(NPC_SYNC) - 1 - sizeof(NpcSync.m_szName) + strlen(NpcSync.m_szName);

	if (bBroadCast)
	{
		this->SendDataToNearRegion((BYTE*)&NpcSync, NpcSync.m_wLength + 1);
	}
	else
	{
		if (SUCCEEDED(g_pServer->PackDataToClient(nClient, (BYTE*)&NpcSync, NpcSync.m_wLength + 1)))
		{
			//printf("Packing sync data ok...\n");
			bRet = TRUE;
		}
		else
		{
			printf("Packing sync data failed...\n");
			return FALSE;
		}
	}
//	g_DebugLog("[Sync]%d:%s<%d> request to %d. size:%d", SubWorld[m_SubWorldIndex].m_dwCurrentTime, Name, m_Kind, nClient, NpcSync.m_wLength + 1);

	if (IsPlayer())
	{
		PLAYER_SYNC	PlayerSync;

		PlayerSync.ProtocolType		= (BYTE)s2c_syncplayer;
		PlayerSync.ID				= m_dwID;
		PlayerSync.TeamServerID		= Player[m_nPlayerIdx].m_cTeam.m_nFlag ? (int)Player[m_nPlayerIdx].m_cTeam.m_nID:-1;
		PlayerSync.HelmType			= (BYTE)m_HelmType;
		PlayerSync.ArmorType		= (BYTE)m_ArmorType;
		PlayerSync.WeaponType		= (BYTE)m_WeaponType;
		PlayerSync.MaskType			= m_MaskType;
		PlayerSync.MantleType		= (BYTE)m_MantleType; 
		if (Player[m_nPlayerIdx].m_cTong.m_nFlag)
		{
			PlayerSync.TongNameID	= Player[m_nPlayerIdx].m_cTong.m_dwTongNameID;
			PlayerSync.m_Figure		= (BYTE)Player[m_nPlayerIdx].m_cTong.m_nFigure;
			PlayerSync.TongNationalEmblem = Player[m_nPlayerIdx].m_cTong.m_nTongNationalEmblem;
			strcpy(PlayerSync.TongName, Player[m_nPlayerIdx].m_cTong.m_szName);
			strcpy(PlayerSync.TongAgname, Player[m_nPlayerIdx].m_cTong.m_szAgname);
		}
		else
		{	
			PlayerSync.TongNameID	= 0;
			PlayerSync.m_Figure		= -1;
			PlayerSync.TongNationalEmblem = 0;
			PlayerSync.TongName[0]	= 0;
			PlayerSync.TongAgname[0]= 0;
		}
		strcpy(PlayerSync.MateName, Player[m_nPlayerIdx].m_cTask.GetSaveStr(TASKVALUE_BASEDATA_MATENAME));
		PlayerSync.HorseType        = m_bRideHorse ? (BYTE)m_HorseType : -1;
		PlayerSync.RankID			= (BYTE)m_RankID;
		memcpy(&PlayerSync.ExpandRank, &m_CurExpandRank, sizeof(m_CurExpandRank));
		PlayerSync.Translife		= (BYTE)m_byTranslife;
		PlayerSync.Viprank			= (BYTE)Player[m_nPlayerIdx].m_cTask.GetSaveVal(TASKVALUE_STATTASK_VIPRANK);
		PlayerSync.Repute			= Player[m_nPlayerIdx].m_cTask.GetSaveVal(TASKVALUE_STATTASK_REPUTE);
		PlayerSync.FuYuan			= Player[m_nPlayerIdx].m_cTask.GetSaveVal(TASKVALUE_STATTASK_FUYUAN);
		PlayerSync.PKFlag			= (BYTE)Player[m_nPlayerIdx].m_cPK.GetNormalPKState();
		PlayerSync.PKValue			= (BYTE)Player[m_nPlayerIdx].m_cPK.GetPKValue();
		PlayerSync.ImagePlayer		= (BYTE)Player[m_nPlayerIdx].m_ImagePlayer;
		PlayerSync.MantleLevel		= (BYTE)m_byMantleLevel;
		PlayerSync.RankInWorld		= Player[m_nPlayerIdx].m_nWorldStat;
		PlayerSync.FactionNumber	= Player[m_nPlayerIdx].m_cFaction.GetCurFactionNo();
		PlayerSync.Trade			= (PLAYERTRADE)Player[m_nPlayerIdx].m_PTrade;
		if(Player[m_nPlayerIdx].m_nPaceBarTime)
			PlayerSync.PacePer		= (BYTE)(Player[m_nPlayerIdx].m_nPaceBarTime * MAX_PERCENT / Player[m_nPlayerIdx].m_nPaceBarTimeMax);
		else
			PlayerSync.PacePer		= 0;
		PlayerSync.m_btSomeFlag		= 0;
		if (m_FightMode)
			PlayerSync.m_btSomeFlag |= 0x01;
		if (Player[m_nPlayerIdx].m_bSleepMode)
			PlayerSync.m_btSomeFlag |= 0x02;

		if (bBroadCast)
		{
			this->SendDataToNearRegion((BYTE*)&PlayerSync, sizeof(PLAYER_SYNC));
		}
		else
		{
			if (SUCCEEDED(g_pServer->PackDataToClient(nClient, (BYTE*)&PlayerSync, sizeof(PLAYER_SYNC))))
			{
				//printf("Packing player sync data ok...\n");
				bRet = TRUE;
			}
			else
			{
				printf("Packing player sync data failed...\n");
				return FALSE;
			}
		}
	}
	return bRet;
}

// ƽʱ���ݵ�ͬ��
void KNpc::NormalSync()
{
	if (m_Doing == do_revive || m_Doing == do_death || !m_Index || m_RegionIndex < 0)
		return;

	NPC_NORMAL_SYNC NpcSync;
	int nMpsX, nMpsY;
	GetMpsPos(&nMpsX, &nMpsY);

	NpcSync.ProtocolType = (BYTE)s2c_syncnpcmin;
	NpcSync.ID = m_dwID;

	memcpy(NpcSync.StateInfo, m_btStateInfo ,sizeof(BYTE) * MAX_SKILL_STATE);

	NpcSync.CurrentLifeMax		= (int)m_CurrentLifeMax;
	if (m_CurrentLifeMax > 0)
		NpcSync.CurrentLife			= (int)m_CurrentLife;
	else
		NpcSync.CurrentLife			= 0;

	NpcSync.MapX = nMpsX;
	NpcSync.MapY = nMpsY;

	NpcSync.RunSpeed		= (BYTE)m_CurrentRunSpeed;
	NpcSync.WalkSpeed		= (BYTE)m_CurrentWalkSpeed;
	NpcSync.AttackSpeed		= (BYTE)m_CurrentAttackSpeed;
	NpcSync.CastSpeed		= (BYTE)m_CurrentCastSpeed;

	NpcSync.Camp = (BYTE)m_CurrentCamp;
	NpcSync.State = 0;

	if (m_FreezeState.nTime > 0)
		NpcSync.State |= STATE_FREEZE;
	if (m_PoisonState.nTime > 0)
		NpcSync.State |= STATE_POISON;
	if (m_StunState.nTime > 0)
		NpcSync.State |= STATE_STUN;
	if (m_HideState.nTime > 0)
		NpcSync.State |= STATE_HIDE;
	if (m_FrozenAction.nTime > 0)
		NpcSync.State |= STATE_FROZEN;
	if (m_WalkRun.nTime > 0)
		NpcSync.State |= STATE_WALKRUN;
	NpcSync.Doing = (BYTE)m_Doing;

	POINT	POff[8] = 
	{
		{0, 32},
		{-16, 32},
		{-16, 0},
		{-16, -32},
		{0, -32},
		{16, -32},
		{16, 0},
		{16, 32},
	};
	int nMaxCount = MAX_PLAYER;//MAX_BROADCAST_COUNT;
	CURREGION.BroadCast(&NpcSync, sizeof(NPC_NORMAL_SYNC), nMaxCount, m_MapX, m_MapY);
	int j = 0;
	for (j = 0; j < 8; j++)
	{
		int nConRegion = CURREGION.m_nConnectRegion[j];
		if (nConRegion == -1)
			continue;
		_ASSERT(m_SubWorldIndex >= 0 && nConRegion >= 0);
		SubWorld[m_SubWorldIndex].m_Region[nConRegion].BroadCast((BYTE*)&NpcSync, sizeof(NPC_NORMAL_SYNC), nMaxCount, m_MapX - POff[j].x, m_MapY - POff[j].y);
	}

	if (IsPlayer())
	{
		PLAYER_NORMAL_SYNC PlayerSync;

		PlayerSync.ProtocolType		= (BYTE)s2c_syncplayermin;
		PlayerSync.ID				= m_dwID;
		PlayerSync.TeamServerID		= Player[m_nPlayerIdx].m_cTeam.m_nFlag ? (int)Player[m_nPlayerIdx].m_cTeam.m_nID:-1;
		PlayerSync.HelmType			= (BYTE)m_HelmType;
		PlayerSync.ArmorType		= (BYTE)m_ArmorType;
		PlayerSync.WeaponType		= (BYTE)m_WeaponType;
		PlayerSync.MaskType			= m_MaskType;
		PlayerSync.MantleType		= (BYTE)m_MantleType; 
		if (Player[m_nPlayerIdx].m_cTong.m_nFlag)
		{
			PlayerSync.TongNameID	= Player[m_nPlayerIdx].m_cTong.m_dwTongNameID;
			PlayerSync.m_Figure		= (BYTE)Player[m_nPlayerIdx].m_cTong.m_nFigure;
			PlayerSync.TongNationalEmblem = Player[m_nPlayerIdx].m_cTong.m_nTongNationalEmblem;
			strcpy(PlayerSync.TongName, Player[m_nPlayerIdx].m_cTong.m_szName);
			strcpy(PlayerSync.TongAgname, Player[m_nPlayerIdx].m_cTong.m_szAgname);
		}
		else
		{	
			PlayerSync.TongNameID	= 0;
			PlayerSync.m_Figure		= -1;
			PlayerSync.TongNationalEmblem = 0;
			PlayerSync.TongName[0]	= 0;
			PlayerSync.TongAgname[0]= 0;
		}
		strcpy(PlayerSync.MateName, Player[m_nPlayerIdx].m_cTask.GetSaveStr(TASKVALUE_BASEDATA_MATENAME));
		PlayerSync.HorseType        = m_bRideHorse ? (BYTE)m_HorseType : -1;
		PlayerSync.RankID			= (BYTE)m_RankID;
		memcpy(&PlayerSync.ExpandRank, &m_CurExpandRank, sizeof(m_CurExpandRank));
		PlayerSync.Translife		= (BYTE)m_byTranslife;
		PlayerSync.Viprank			= (BYTE)Player[m_nPlayerIdx].m_cTask.GetSaveVal(TASKVALUE_STATTASK_VIPRANK);
		PlayerSync.Repute			= Player[m_nPlayerIdx].m_cTask.GetSaveVal(TASKVALUE_STATTASK_REPUTE);
		PlayerSync.FuYuan			= Player[m_nPlayerIdx].m_cTask.GetSaveVal(TASKVALUE_STATTASK_FUYUAN);
		PlayerSync.PKFlag			= (BYTE)Player[m_nPlayerIdx].m_cPK.GetNormalPKState();
		PlayerSync.PKValue			= (BYTE)Player[m_nPlayerIdx].m_cPK.GetPKValue();
		PlayerSync.ImagePlayer		= (BYTE)Player[m_nPlayerIdx].m_ImagePlayer;
		PlayerSync.MantleLevel		= (BYTE)m_byMantleLevel;
		PlayerSync.RankInWorld		= Player[m_nPlayerIdx].m_nWorldStat;
		PlayerSync.FactionNumber	= Player[m_nPlayerIdx].m_cFaction.GetCurFactionNo();
		PlayerSync.Trade			= (PLAYERTRADE)Player[m_nPlayerIdx].m_PTrade;
		if(Player[m_nPlayerIdx].m_nPaceBarTime)
			PlayerSync.PacePer		= (BYTE)(Player[m_nPlayerIdx].m_nPaceBarTime * MAX_PERCENT / Player[m_nPlayerIdx].m_nPaceBarTimeMax);
		else
			PlayerSync.PacePer		= 0;
		PlayerSync.m_btSomeFlag		= 0;
		if (m_FightMode)
			PlayerSync.m_btSomeFlag |= 0x01;
		if (Player[m_nPlayerIdx].m_bSleepMode)
			PlayerSync.m_btSomeFlag |= 0x02;

		int nMaxCount = MAX_PLAYER;//MAX_BROADCAST_COUNT;
		CURREGION.BroadCast(&PlayerSync, sizeof(PLAYER_NORMAL_SYNC), nMaxCount, m_MapX, m_MapY);
		for (j = 0; j < 8; j++)
		{
			int nConRegion = CURREGION.m_nConnectRegion[j];
			if (nConRegion == -1)
				continue;
			SubWorld[m_SubWorldIndex].m_Region[nConRegion].BroadCast((BYTE*)&PlayerSync, sizeof(PLAYER_NORMAL_SYNC), nMaxCount, m_MapX - POff[j].x, m_MapY - POff[j].y);
		}

		NPC_PLAYER_TYPE_NORMAL_SYNC	sSync;
		sSync.ProtocolType = s2c_syncnpcminplayer;
		sSync.ID = m_dwID;
		sSync.MapX = nMpsX;
		sSync.MapY = nMpsY;
		sSync.OffX = m_OffX;
		sSync.OffY = m_OffY;
		g_pServer->PackDataToClient(Player[m_nPlayerIdx].m_nNetConnectIdx, (BYTE*)&sSync, sizeof(sSync));
	}
}

void KNpc::BroadCastRevive(int nType)
{
	if (!IsPlayer())
		return;

	if (m_RegionIndex < 0)
		return;

	NPC_REVIVE_SYNC	NpcReviveSync;
	NpcReviveSync.ProtocolType = s2c_playerrevive;
	NpcReviveSync.ID = m_dwID;
	NpcReviveSync.Type = (BYTE)nType;

	POINT	POff[8] = 
	{
		{0, 32},
		{-16, 32},
		{-16, 0},
		{-16, -32},
		{0, -32},
		{16, -32},
		{16, 0},
		{16, 32},
	};
	int nMaxCount = MAX_BROADCAST_COUNT;
	CURREGION.BroadCast((BYTE*)&NpcReviveSync, sizeof(NPC_REVIVE_SYNC), nMaxCount, m_MapX, m_MapY);
	int j = 0;
	for (j = 0; j < 8; j++)
	{
		int nConRegion = CURREGION.m_nConnectRegion[j];
		if (nConRegion == -1)
			continue;
		SubWorld[m_SubWorldIndex].m_Region[nConRegion].BroadCast((BYTE*)&NpcReviveSync, sizeof(NPC_REVIVE_SYNC), nMaxCount, m_MapX - POff[j].x, m_MapY - POff[j].y);
	}
}

int	KNpc::GetPlayerIdx()
{
	if (m_Kind != kind_player)
		return 0;
	return m_nPlayerIdx;
}

#endif

#ifndef _SERVER
#include "scene/KScenePlaceC.h"

int KNpc::PaintInfo(int nHeightOffset, int nFontSize, DWORD dwBorderColor)
{
	if (m_Index != Player[CLIENT_PLAYER_INDEX].m_nIndex && 
		m_HideState.nTime > 0)
	{
		return 0;
	}

	int nMpsX, nMpsY, nXX, nYY;
	GetMpsPos(&nMpsX, &nMpsY);

	DWORD	dwColor = 0xffffffff;
	int nHeightOff = nHeightOffset + nFontSize + 1;

	char pszTemp[128];	

	switch (m_Kind)
	{
	case kind_player:
		switch(m_CurrentCamp)
		{
		case camp_begin:
			dwColor = 0xffffffff;
			break;
		case camp_justice:
			dwColor = 0xff000000 | (255 << 16) | (168 << 8) | 94;
			break;
		case camp_evil:
			dwColor = 0xff000000 | (255 << 16) | (146 << 8) | 255;
			break;
		case camp_balance:
			dwColor = 0xff000000 | (85 << 16) | (255 << 8) | 145;
			break;
		case camp_free:
			dwColor = 0xff000000 | (255 << 16);
			break;
		case camp_animal:
			dwColor = 0xffffffff;
			break;
		case camp_event:
			dwColor = 0xff000000 | (238 << 16) | (18 << 8) | 137;
			break;
		case camp_audience:
			dwColor = 0xffffffff;
			break;
		case camp_tongwar:
			dwColor = 0xff000000 | (104 << 16) | (38 << 8) | 174;
			break;
		}

		strcpy(pszTemp, Name);
		if (m_FreezeState.nTime || m_PoisonState.nTime || m_StunState.nTime)
		{
		    strcat(pszTemp, "(");
			if (m_FreezeState.nTime)
				strcat(pszTemp, "B�ng");
			if (m_PoisonState.nTime)
				strcat(pszTemp, "��c");
			if (m_StunState.nTime)
				strcat(pszTemp, "M�");
			if (m_FrozenAction.nTime)
				strcat(pszTemp, "Cho�ng");
			strcat(pszTemp, ")");	
		}
		nXX = nMpsX - nFontSize * g_StrLen(pszTemp) / 4 + ((m_byMantleLevel > 0 && m_byMantleLevel <= MAX_ITEM_LEVEL) ? 40 : 0);
		nYY = nMpsY;

		g_pRepresent->OutputText(nFontSize, pszTemp, KRF_ZERO_END, nXX, nYY, dwColor, 0, nHeightOff, dwBorderColor);

		if (m_byViprank>0 && (m_byViprank <= MAX_VIPRANK_VALUE))
			nXX = PaintViprank(nHeightOff, nFontSize, nXX, nYY);
		if (m_byMantleLevel>0 && (m_byMantleLevel <= MAX_ITEM_LEVEL))
			nXX = PaintMantle(nHeightOff, nFontSize, nXX, nYY);
		if (m_byTranslife>0 && (m_byTranslife <= MAX_TRANSLIFE_VALUE))
			nXX = PaintTranslife(nHeightOff, nFontSize, nXX, nYY);

		nHeightOffset += nFontSize + 1;
		if (m_dwTongNameID)
		{
			if (m_szTongAgname[0])
				sprintf(pszTemp, "%s %s", m_szTongName, m_szTongAgname);
			else
			{
				switch (m_nFigure)
				{
				case enumTONG_FIGURE_MEMBER:
					sprintf(pszTemp, "%s %s", m_szTongName, defTONG_MEMBER_AGNAME);
					break;
				case enumTONG_FIGURE_MANAGER:
					sprintf(pszTemp, "%s %s", m_szTongName, defTONG_MANAGER_AGNAME);
					break;
				case enumTONG_FIGURE_DIRECTOR:
					sprintf(pszTemp, "%s %s", m_szTongName, defTONG_DIRECTOR_AGNAME);
					break;
				case enumTONG_FIGURE_MASTER:
					sprintf(pszTemp, "%s %s", m_szTongName, defTONG_MASTER_AGNAME);
					break;
				default:
					sprintf(pszTemp, "%s", m_szTongName);
					break;
				}
			}
			nXX = nMpsX - nFontSize * g_StrLen(pszTemp) / 4;
			nYY -= (nFontSize + 4)*2;
			g_pRepresent->OutputText(nFontSize, pszTemp, KRF_ZERO_END, nXX, nYY, dwColor, 0, nHeightOff, dwBorderColor);
			
			if(m_nTongNationalEmblem)
			{
				KRUImage RUIconImage;
				RUIconImage.nType = ISI_T_SPR;
				RUIconImage.Color.Color_b.a = 255;
				RUIconImage.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
				RUIconImage.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
				RUIconImage.uImage = 0;
				RUIconImage.nISPosition = IMAGE_IS_POSITION_INIT;
				strcpy(RUIconImage.szImage, PlayerSet.m_szNationalEmblemPic[m_nTongNationalEmblem]);
				RUIconImage.oPosition.nX = nXX-18;
				RUIconImage.oPosition.nY = nYY-12;
				RUIconImage.oPosition.nZ = nHeightOff;
				RUIconImage.nFrame = 0;
				g_pRepresent->DrawPrimitives(1, &RUIconImage, RU_T_IMAGE, FALSE);
			}
			nHeightOffset += nFontSize + 1;
		}
		
		if (m_CurExpandRank.szName[0])
		{
			nYY -= (nFontSize + 4)*2;

			g_pRepresent->OutputText(nFontSize, m_CurExpandRank.szName, KRF_ZERO_END, 
				nMpsX - nFontSize * g_StrLen(m_CurExpandRank.szName) / 4, nYY, 
				m_CurExpandRank.dwColor, 0, nHeightOff, dwBorderColor);
			nHeightOffset += nFontSize + 1;
		}

		if (m_MaskType)
		{
			pszTemp[0] = 0;

			if (m_bRideHorse)
			{
				g_GameSetting.GetString("Actions", "0", "", pszTemp, sizeof(pszTemp));
			}
			else if (m_Doing == do_sit)
			{
				g_GameSetting.GetString("Actions", "1", "", pszTemp, sizeof(pszTemp));
			}

			if (pszTemp[0])
			{
				KRUImage RUIconImage;
				RUIconImage.nType = ISI_T_SPR;
				RUIconImage.Color.Color_b.a = 255;
				RUIconImage.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
				RUIconImage.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
				RUIconImage.uImage = 0;
				RUIconImage.nISPosition = IMAGE_IS_POSITION_INIT;
				strcpy(RUIconImage.szImage, pszTemp);
				RUIconImage.oPosition.nX = nMpsX + (nFontSize * g_StrLen(Name) / 4) + 10 + (m_byMantleLevel > 0 ? 40 : 0);
				RUIconImage.oPosition.nY = nMpsY + 20;
				RUIconImage.oPosition.nZ = nHeightOff;
				RUIconImage.nFrame = 0;
				g_pRepresent->DrawPrimitives(1, &RUIconImage, RU_T_IMAGE, FALSE);
			}
		}
		break;
	case kind_dialoger:
		g_pRepresent->OutputText(nFontSize, Name, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(Name) / 4, nMpsY, dwColor, 0, nHeightOff, dwBorderColor);
		nHeightOffset += nFontSize + 1;
		break;
	case kind_normal:
		g_GameSetting.GetString("Series", "Image", "", pszTemp, sizeof(pszTemp));

		KRUImage RUIconImageR;
		RUIconImageR.nType = ISI_T_SPR;
		RUIconImageR.Color.Color_b.a = 255;
		RUIconImageR.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
		RUIconImageR.uImage = 0;
		RUIconImageR.nISPosition = IMAGE_IS_POSITION_INIT;
		RUIconImageR.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
		sprintf(RUIconImageR.szImage, pszTemp, m_Series);
		RUIconImageR.oPosition.nX = nMpsX + nFontSize * g_StrLen(Name) / 4 + 5;
		RUIconImageR.oPosition.nY = nMpsY; 
		RUIconImageR.oPosition.nZ = nHeightOff;
		RUIconImageR.nFrame = 0;
		g_pRepresent->DrawPrimitives(1, &RUIconImageR, RU_T_IMAGE, FALSE);
		switch (m_btSpecial)
		{
			case npc_normal:
				dwColor = 0xffffffff;
			break;
			case npc_blue:
				dwColor = 0x006569d7;
			break;
			case npc_gold:
				dwColor = 255 << 16 | 217 << 8 | 78;
			break;
			default:
				dwColor = 0xff000000;	// ��ɫ
			break;
		}
		g_pRepresent->OutputText(nFontSize, Name, KRF_ZERO_END, nMpsX  - nFontSize * g_StrLen(Name) / 4, nMpsY, dwColor, 0, nHeightOff, dwBorderColor);
		break;
	}
	return nHeightOffset;
}

int KNpc::PaintViprank(int nHeightOff, int nFontSize, int nMpsX, int nMpsY)
{
	KRUImage RUIconImage;
	RUIconImage.nType = ISI_T_SPR;
	RUIconImage.Color.Color_b.a = 255;
	RUIconImage.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
	RUIconImage.uImage = 0;
	RUIconImage.nISPosition = IMAGE_IS_POSITION_INIT;
	RUIconImage.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
	strcpy(RUIconImage.szImage, PlayerSet.m_szViprankPic[m_byViprank]);
	KImageParam	Param;
	g_pRepresent->GetImageParam(RUIconImage.szImage, &Param, ISI_T_SPR);
	RUIconImage.oPosition.nX = nMpsX - Param.nWidth/4 - 2;
	RUIconImage.oPosition.nY = nMpsY - Param.nHeight*5;
	RUIconImage.oPosition.nZ = nHeightOff;
	RUIconImage.nFrame = g_SubWorldSet.GetGameTime() % Param.nNumFrames;
	g_pRepresent->DrawPrimitives(1, &RUIconImage, RU_T_IMAGE, 0);

	return RUIconImage.oPosition.nX - Param.nWidth;
}

int KNpc::PaintMantle(int nHeightOff, int nFontSize, int nMpsX, int nMpsY)
{
	KRUImage RUIconImage;
	RUIconImage.nType = ISI_T_SPR;
	RUIconImage.Color.Color_b.a = 255;
	RUIconImage.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
	RUIconImage.uImage = 0;
	RUIconImage.nISPosition = IMAGE_IS_POSITION_INIT;
	RUIconImage.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
	strcpy(RUIconImage.szImage, PlayerSet.m_szFortuneRankPic[m_byMantleLevel]);
	KImageParam	Param;
	g_pRepresent->GetImageParam(RUIconImage.szImage, &Param, ISI_T_SPR);
	RUIconImage.oPosition.nX = nMpsX - Param.nWidth;
	RUIconImage.oPosition.nY = nMpsY - Param.nHeight/2 - 4;
	RUIconImage.oPosition.nZ = nHeightOff;
	RUIconImage.nFrame = g_SubWorldSet.GetGameTime() % Param.nNumFrames;
	g_pRepresent->DrawPrimitives(1, &RUIconImage, RU_T_IMAGE, 0);

	return RUIconImage.oPosition.nX;
}

int KNpc::PaintTranslife(int nHeightOff, int nFontSize, int nMpsX, int nMpsY)
{
	KRUImage RUIconImage;
	RUIconImage.nType = ISI_T_SPR;
	RUIconImage.Color.Color_b.a = 255;
	RUIconImage.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
	RUIconImage.uImage = 0;
	RUIconImage.nISPosition = IMAGE_IS_POSITION_INIT;
	RUIconImage.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
	strcpy(RUIconImage.szImage, PlayerSet.m_szTranlifePic[m_byTranslife]);
	KImageParam	Param;
	g_pRepresent->GetImageParam(RUIconImage.szImage, &Param, ISI_T_SPR);
	RUIconImage.oPosition.nX = nMpsX - (Param.nWidth/4)-2;
	RUIconImage.oPosition.nY = nMpsY + Param.nHeight/2;
	RUIconImage.oPosition.nZ = nHeightOff;
	RUIconImage.nFrame = g_SubWorldSet.GetGameTime() % Param.nNumFrames;
	g_pRepresent->DrawPrimitives(1, &RUIconImage, RU_T_IMAGE, 0);

	return RUIconImage.oPosition.nX;
}

void KNpc::PaintTop(int nHeightOffset, int nnHeightOffset, int nFontSize, DWORD dwBorderColor)
{
	if(m_Kind != kind_player)
		return;

	int	nMpsX, nMpsY;
	GetMpsPos(&nMpsX, &nMpsY);
	if (m_PTrade.nTrade)
	{
		int nWid = nFontSize * g_StrLen(m_PTrade.cName) / 2 + 10;
		int nHei = nFontSize + 12;

		KRUImage RUIconImageR;
		RUIconImageR.nType = ISI_T_SPR;
		RUIconImageR.Color.Color_b.a = 150;
		RUIconImageR.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
		RUIconImageR.uImage = 0;
		RUIconImageR.nISPosition = IMAGE_IS_POSITION_INIT;
		RUIconImageR.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
		strcpy(RUIconImageR.szImage, "\\spr\\Ui3\\��̯\\��̯ͷ��������.spr");
		RUIconImageR.oPosition.nX = nMpsX - nWid / 2;
		RUIconImageR.oPosition.nY = nMpsY - 40;
		RUIconImageR.oPosition.nZ = nHeightOffset + 10;
		RUIconImageR.nFrame = 0;
		for (int i = 0; i < nWid;i++)
		{
			RUIconImageR.oPosition.nX = nMpsX - nWid / 2 + i;
			g_pRepresent->DrawPrimitives(1, &RUIconImageR, RU_T_IMAGE, FALSE);
		}

		DWORD dwColor = 0x00eed66a;
		g_pRepresent->OutputText(nFontSize, m_PTrade.cName, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(m_PTrade.cName) / 4, RUIconImageR.oPosition.nY + nFontSize * 2, dwColor, 0, RUIconImageR.oPosition.nZ + nFontSize - 1, dwBorderColor);

		strcpy(RUIconImageR.szImage, "\\spr\\Ui3\\��̯\\��̯ͷ��������.spr");
		RUIconImageR.nType = ISI_T_SPR;
		RUIconImageR.Color.Color_b.a = 255;
		RUIconImageR.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
		RUIconImageR.uImage = 0;
		RUIconImageR.nISPosition = IMAGE_IS_POSITION_INIT;
		RUIconImageR.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
		RUIconImageR.oPosition.nX = nMpsX + nWid / 2;
		RUIconImageR.oPosition.nY = nMpsY;
		RUIconImageR.oPosition.nZ += 23;
		g_pRepresent->DrawPrimitives(1, &RUIconImageR, RU_T_IMAGE, FALSE);
		
		strcpy(RUIconImageR.szImage, "\\spr\\Ui3\\��̯\\��̯ͷ��������.spr");
		RUIconImageR.nType = ISI_T_SPR;
		RUIconImageR.Color.Color_b.a = 255;
		RUIconImageR.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
		RUIconImageR.uImage = 0;
		RUIconImageR.nISPosition = IMAGE_IS_POSITION_INIT;
		RUIconImageR.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
		RUIconImageR.oPosition.nX = nMpsX - nWid / 2 - 18;
		RUIconImageR.oPosition.nY = nMpsY;
		g_pRepresent->DrawPrimitives(1, &RUIconImageR, RU_T_IMAGE, FALSE);
	}
	if(m_nPacePercent)
	{
		int nPercent = MAX_PERCENT - m_nPacePercent;
		if (nPercent < 0)
			nPercent = 0;
		KRUShadow Shadow;
		Shadow.Color.Color_b.a = 0;
		Shadow.Color.Color_b.r = 238;
		Shadow.Color.Color_b.g = 18;
		Shadow.Color.Color_b.b = 137;
		Shadow.oPosition.nX = nMpsX - SHOW_LIFE_WIDTH / 2;
		Shadow.oPosition.nY = nMpsY;
		Shadow.oPosition.nZ = nnHeightOffset + SHOW_LIFE_HEIGHT - 3;
		Shadow.oEndPos.nX = Shadow.oPosition.nX + SHOW_LIFE_WIDTH * nPercent / MAX_PERCENT;
		Shadow.oEndPos.nY = nMpsY - 2;
		Shadow.oEndPos.nZ = nnHeightOffset - 4;
		g_pRepresent->DrawPrimitives(1, &Shadow, RU_T_SHADOW, FALSE);

		Shadow.Color.Color_dw = 0x16000000;
		Shadow.oPosition.nX = Shadow.oEndPos.nX;
		Shadow.oEndPos.nX = nMpsX + SHOW_LIFE_WIDTH / 2;
		g_pRepresent->DrawPrimitives(1, &Shadow, RU_T_SHADOW, FALSE);
	}
}

int	KNpc::PaintChat(int nHeightOffset)
{
	if (m_Kind != kind_player && m_Kind != kind_dialoger && m_Kind != kind_normal)
		return nHeightOffset;
	if (m_nChatContentLen <= 0)
		return nHeightOffset;
	if (m_nChatNumLine <= 0)
		return nHeightOffset;
	if (m_Index != Player[CLIENT_PLAYER_INDEX].m_nIndex && 
		m_HideState.nTime > 0)
		return 0;

	int nFontSize = 12;
	int					nWidth, nHeight;
	int					nMpsX, nMpsY;
	KOutputTextParam	sParam;
	sParam.BorderColor = 0;

	sParam.nNumLine = m_nChatNumLine;

	nWidth = m_nChatFontWidth * nFontSize / 2;
	nHeight = sParam.nNumLine * (nFontSize + 1);

	nWidth += 6;	//Ϊ�˺ÿ�
	nHeight += 10;	//Ϊ�˺ÿ�

	GetMpsPos(&nMpsX, &nMpsY);
	sParam.nX = nMpsX - nWidth / 2;
	sParam.nY = nMpsY;
	sParam.nZ = nHeightOffset + nHeight;
	sParam.Color = SHOW_CHAT_COLOR;
	sParam.nSkipLine = 0;
	sParam.nVertAlign = 0;
	
	sParam.bPicPackInSingleLine = true;
	g_pRepresent->OutputRichText(nFontSize, &sParam, m_szChatBuffer, m_nChatContentLen, nWidth);

	return sParam.nZ;
}

#include "../../Engine/Src/Text.h"
int	KNpc::SetChatInfo(const char* Name, const char* pMsgBuff, unsigned short nMsgLength)
{
	int nFontSize = 12;

	char szChatBuffer[MAX_SENTENCE_LENGTH];

	memset(szChatBuffer, 0, sizeof(szChatBuffer));

	if (nMsgLength)
	{
		int nOffset = 0;
		if (pMsgBuff[0] != KTC_TAB)
		{
			szChatBuffer[nOffset] = (char)KTC_COLOR;
			nOffset++;
			szChatBuffer[nOffset] = (char)0xFF;
			nOffset++;
			szChatBuffer[nOffset] = (char)0xFF;
			nOffset++;
			szChatBuffer[nOffset] = (char)0x00;
			nOffset++;
			strncpy(szChatBuffer + nOffset, Name, 32);
			nOffset += strlen(Name);
			szChatBuffer[nOffset] = ':';
			nOffset++;
			szChatBuffer[nOffset] = (char)0x20;
			nOffset++;
			szChatBuffer[nOffset] = (char)KTC_COLOR_RESTORE;
			nOffset++;
		}
		else
		{
			pMsgBuff ++;
			nMsgLength --;
		}

		if (nMsgLength)
		{
			memcpy(szChatBuffer + nOffset, pMsgBuff, nMsgLength);
			nOffset += nMsgLength;

			memset(m_szChatBuffer, 0, sizeof(m_szChatBuffer));
			m_nChatContentLen = MAX_SENTENCE_LENGTH;
			TGetLimitLenEncodedString(szChatBuffer, nOffset, nFontSize, SHOW_CHAT_WIDTH,
				m_szChatBuffer, m_nChatContentLen, m_Kind == kind_player ? 2 : 6, true);

			m_nChatNumLine = TGetEncodedTextLineCount(m_szChatBuffer, m_nChatContentLen, SHOW_CHAT_WIDTH, m_nChatFontWidth, nFontSize, 0, 0, true);
			if (m_Kind == kind_player && m_nChatNumLine >= 2)
				m_nChatNumLine = 2;
			m_nCurChatTime = IR_GetCurrentTime();
			return true;
		}
	}
	return false;
}

int	KNpc::PaintLife(int nHeightOffset, bool bSelect)
{
	if (!bSelect &&
		(m_Kind != kind_player &&
		m_Kind != kind_partner)
		)
		return nHeightOffset;

	if (m_CurrentLifeMax <= 0)
		return nHeightOffset;

	if (m_Index != Player[CLIENT_PLAYER_INDEX].m_nIndex && 
		m_HideState.nTime > 0)
		return nHeightOffset;

/*	if (relation_enemy == NpcSet.GetRelation(m_Index, Player[CLIENT_PLAYER_INDEX].m_nIndex) &&
		(m_Kind == kind_player ||
		 m_Kind == kind_partner)
		)
		return nHeightOffset;*/

	int	nMpsX, nMpsY;
	GetMpsPos(&nMpsX, &nMpsY);
	int nWid = SHOW_LIFE_WIDTH;
	//if (Npc[m_Index].m_btSpecial >= npc_gold)
	//	nWid += 12;
	int nHei = SHOW_LIFE_HEIGHT;
	//if (Npc[m_Index].m_btSpecial >= npc_gold)
	//	nHei += 2;
	KRUShadow	Shadow;
	int nPercent = m_CurrentLife * MAX_PERCENT / m_CurrentLifeMax;
	if (nPercent > MAX_PERCENT)
		nPercent = MAX_PERCENT;
	else if (nPercent < 0)
		return nHeightOffset;

	if (nPercent >= 50)
	{
		Shadow.Color.Color_b.r = 0;
		Shadow.Color.Color_b.g = 255;
		Shadow.Color.Color_b.b = 0;
	}
	else if (nPercent >= 25)
	{
		Shadow.Color.Color_b.r = 255;
		Shadow.Color.Color_b.g = 255;
		Shadow.Color.Color_b.b = 0;
	}
	else
	{
		Shadow.Color.Color_b.r = 255;
		Shadow.Color.Color_b.g = 0;
		Shadow.Color.Color_b.b = 0;
	}

	if (relation_enemy == NpcSet.GetRelation(m_Index, Player[CLIENT_PLAYER_INDEX].m_nIndex) &&
		(m_Kind == kind_player ||
		 m_Kind == kind_partner)
		)
	{
		Shadow.Color.Color_b.r = 231;
		Shadow.Color.Color_b.g = 194;
		Shadow.Color.Color_b.b = 0;
	}

	if (Npc[m_Index].m_nPKFlag == enumPKMurder || 
		Player[Npc[m_Index].m_nPlayerIdx].m_cPK.GetEnmityPKState() == enumPK_ENMITY_STATE_PKING)
	{
		Shadow.Color.Color_b.r = 238;
		Shadow.Color.Color_b.g = 18;
		Shadow.Color.Color_b.b = 137;
	}
	if (Npc[m_Index].m_nPKFlag == enumPKTongWar)
	{
		Shadow.Color.Color_b.r = 170;
		Shadow.Color.Color_b.g = 30;
		Shadow.Color.Color_b.b = 255;
	}
	if (Npc[m_Index].m_nPKValue >= 10)
	{
		Shadow.Color.Color_b.r = 255;
		Shadow.Color.Color_b.g = 0;
		Shadow.Color.Color_b.b = 66;
	}

	Shadow.Color.Color_b.a = 0;
	Shadow.oPosition.nX = nMpsX - nWid / 2;
	Shadow.oPosition.nY = nMpsY;
	Shadow.oPosition.nZ = nHeightOffset + nHei;
	Shadow.oEndPos.nX = Shadow.oPosition.nX + nWid * nPercent / MAX_PERCENT;
	Shadow.oEndPos.nY = nMpsY;
	Shadow.oEndPos.nZ = nHeightOffset;
	g_pRepresent->DrawPrimitives(1, &Shadow, RU_T_SHADOW, FALSE);

	//Shadow.Color.Color_dw = 0x16000000;
	Shadow.Color.Color_b.r = 128;
	Shadow.Color.Color_b.g = 128;
	Shadow.Color.Color_b.b = 128;
	Shadow.oPosition.nX = Shadow.oEndPos.nX;
	Shadow.oEndPos.nX = nMpsX + nWid / 2;
	g_pRepresent->DrawPrimitives(1, &Shadow, RU_T_SHADOW, FALSE);

	return nHeightOffset + nHei;
}

void KNpc::Paint()
{
	if (m_Index != Player[CLIENT_PLAYER_INDEX].m_nIndex && 
		(m_CurrentCamp == camp_audience || 
		m_HideState.nTime > 0))
	{
		return;
	}

	BOOL bPaintBody = TRUE;

	if (Option.GetLow(LowNpc))
	{
		if (m_Kind == kind_normal || m_Kind == kind_dialoger)
		{
			if (m_Index != Player[CLIENT_PLAYER_INDEX].m_nIndex)
			{
				bPaintBody = FALSE;
			}
		}
	}

	if (m_ResDir != m_Dir)
	{
		int nDirOff = m_Dir - m_ResDir;
		if (nDirOff > 32)
			nDirOff -= 64;
		else if (nDirOff < - 32)
			nDirOff += 64;
		m_ResDir += nDirOff / 2;
		if (m_ResDir >= 64)
			m_ResDir -= 64;
		if (m_ResDir < 0)
			m_ResDir += 64;
	}
	m_DataRes.Draw(m_Index, m_ResDir, m_Frames.nTotalFrame, m_Frames.nCurrentFrame, FALSE, bPaintBody);

	int nHeight = GetNpcPate() + GetNpcPatePeopleInfo();
	if (m_CurExpandRank.szName[0])
		nHeight += NORMAL_FONTSIZE+1;
	if (m_dwTongNameID)
		nHeight += NORMAL_FONTSIZE+1;
	DrawMenuState(nHeight);
}
#endif

//--------------------------------------------------------------------------
//	���ܣ����ӻ������������
//--------------------------------------------------------------------------
void	KNpc::AddBaseLifeMax(int nLife)
{
	m_LifeMax += nLife;
	m_CurrentLifeMax = m_LifeMax;
}

void	KNpc::SetBaseLifeMax(int nLifeMax) 
{
	m_LifeMax = nLifeMax;
	m_CurrentLifeMax = m_LifeMax;
}
//--------------------------------------------------------------------------
//	���ܣ����ӵ�ǰ���������
//--------------------------------------------------------------------------
void	KNpc::AddCurLifeMax(int nLife)
{
	m_CurrentLifeMax += nLife;
}


//--------------------------------------------------------------------------
//	���ܣ����ӻ������������
//--------------------------------------------------------------------------
void	KNpc::AddBaseStaminaMax(int nStamina)
{
	m_StaminaMax += nStamina;
	m_CurrentStaminaMax = m_StaminaMax;
}

void	KNpc::SetBaseStaminaMax(int nStamina)
{
	m_StaminaMax = nStamina;
	m_CurrentStaminaMax = m_StaminaMax;
}
//--------------------------------------------------------------------------
//	���ܣ����ӵ�ǰ���������
//--------------------------------------------------------------------------
void	KNpc::AddCurStaminaMax(int nStamina)
{
	m_CurrentStaminaMax += nStamina;
}

//--------------------------------------------------------------------------
//	���ܣ����ӻ������������
//--------------------------------------------------------------------------
void	KNpc::AddBaseManaMax(int nMana)
{
	m_ManaMax += nMana;
	m_CurrentManaMax = m_ManaMax;
}

void	KNpc::SetBaseManaMax(int nMana)
{
	m_ManaMax = nMana;
	m_CurrentManaMax = m_ManaMax;
}

//--------------------------------------------------------------------------
//	���ܣ����ӵ�ǰ���������
//--------------------------------------------------------------------------
void	KNpc::AddCurManaMax(int nMana)
{
	m_CurrentManaMax += nMana;
}

/*
//--------------------------------------------------------------------------
//	���ܣ����¼��������ظ��ٶ�
//--------------------------------------------------------------------------
void	KNpc::ResetLifeReplenish()
{
	m_LifeReplenish = (m_Level + 5) / 6;
	m_CurrentLifeReplenish = m_LifeReplenish;
}
*/

/*
//--------------------------------------------------------------------------
//	���ܣ����㵱ǰ���������
//--------------------------------------------------------------------------
void	KNpc::CalcCurLifeMax()
{
}
*/

/*
//--------------------------------------------------------------------------
//	���ܣ����㵱ǰ���������
//--------------------------------------------------------------------------
void	KNpc::CalcCurStaminaMax()
{
	m_CurrentStaminaMax = m_StaminaMax;		// ����Ҫ���� װ�������ܡ�ҩ���ʱ���ȵ�Ӱ��
}
*/

/*
//--------------------------------------------------------------------------
//	���ܣ����㵱ǰ���������
//--------------------------------------------------------------------------
void	KNpc::CalcCurManaMax()
{
	m_CurrentManaMax = m_ManaMax;			// ����Ҫ���� װ�������ܡ�ҩ���ʱ���ȵ�Ӱ��
}
*/

//--------------------------------------------------------------------------
//	���ܣ����㵱ǰ�����ظ��ٶ�
//--------------------------------------------------------------------------
void	KNpc::CalcCurLifeReplenish()
{
	m_CurrentLifeReplenish = m_LifeReplenish;	// ���ɫϵ�𡢽�ɫ�ȼ����Ƿ�ʹ��ҩ�������ܺ�ħ��װ���й�
}

void	KNpc::CalcCurLucky()
{
	m_CurrentLucky = Player[m_nPlayerIdx].m_nLucky;	// ���ɫϵ�𡢽�ɫ�ȼ����Ƿ�ʹ��ҩ�������ܺ�ħ��װ���й�
}

void	KNpc::Remove()
{
/*	m_LoopFrames = 0;
	m_Index = 0;
	m_PlayerIdx = -1;
	m_Kind = 0;
	m_dwID = 0;
	Name[0] = 0;*/
	Init();
#ifndef _SERVER
	m_DataRes.Remove(m_Index);
#endif
}

#ifndef _SERVER
void	KNpc::RemoveRes()
{
	m_DataRes.Remove(m_Index);
}
#endif
//--------------------------------------------------------------------------
//	���ܣ��趨�� npc ���������ԣ����ݻ�û��ɣ�not end
//--------------------------------------------------------------------------
void	KNpc::SetSeries(int nSeries)
{
	m_Series = nSeries;
}
/*!*****************************************************************************
// Function		: KNpc::SetStateSkill
// Purpose		: 
// Return		: void 
// Argumant		: int nSkillID
// Argumant		: int nLevel
// Argumant		: void *pData
// Argumant		: int nDataNum
// Argumant		: int nTime -1��ʾ�������ܣ�ʱ������
// Comments		:
// Author		: Spe
*****************************************************************************/
void KNpc::SetStateSkillEffect(int nLauncher, int nSkillID, int nLevel, void *pData, int nDataNum, int nTime/* = -1*/, BOOL bOverLook/* = FALSE*/)
{
	if (nLevel <= 0|| nSkillID <= 0) return ;

	_ASSERT(nSkillID < MAX_SKILL && nLevel < MAX_SKILLLEVEL);
	KSkill * pOrdinSkill = (KSkill *)g_SkillManager.GetSkill(nSkillID, nLevel);

	_ASSERT(nDataNum < MAX_SKILL_STATE);
	if (nDataNum >= MAX_SKILL_STATE)
		nDataNum = MAX_SKILL_STATE;
#ifdef _SERVER
	if (pData && nDataNum >= 0)
	{
		STATE_EFFECT_SYNC	Sync;
		Sync.ProtocolType = s2c_syncstateeffect;
		Sync.m_dwSkillID = nSkillID;
		Sync.m_nLevel = nLevel;
		Sync.m_nTime = nTime;
		Sync.m_bOverLook = bOverLook;
		memcpy(Sync.m_MagicAttrib, pData, sizeof(KMagicAttrib) * nDataNum);
		Sync.m_wLength = sizeof(Sync) - sizeof(KMagicAttrib) * (MAX_SKILL_STATE - nDataNum) - 1;
		g_pServer->PackDataToClient(Player[m_nPlayerIdx].m_nNetConnectIdx, &Sync, Sync.m_wLength + 1);
	}
#endif

	KStateNode* pNode;
	KMagicAttrib* pTemp = NULL;

	pNode = (KStateNode *)m_StateSkillList.GetHead();
	while(pNode)
	{
		if (pNode->m_SkillID == nSkillID)
		{
			if (pNode->m_Level == nLevel)
			{
				pNode->m_LeftTime = nTime;
				pNode->m_bOverLook = bOverLook;
				pNode->m_bTempStateGraphics = FALSE;
			}
			else if (pNode->m_Level < nLevel)
			{
				pTemp = (KMagicAttrib *)pData;
				for (int i = 0; i < nDataNum; i++)
				{
					// ���ԭ���ܵ�Ӱ��
					ModifyAttrib(nLauncher, &pNode->m_State[i]);
					// ���µȼ��¼��ܵ�Ӱ����㵽NPC����
					ModifyAttrib(nLauncher, pTemp);
					pNode->m_State[i].nAttribType = pTemp->nAttribType;
					pNode->m_State[i].nValue[0] = -pTemp->nValue[0];
					pNode->m_State[i].nValue[1] = -pTemp->nValue[1];
					pNode->m_State[i].nValue[2] = -pTemp->nValue[2];
					pTemp++;
				}
			}
			return;
		}
		pNode = (KStateNode *)pNode->GetNext();
	}
	// û����ѭ���з��أ�˵�����¼���
	pNode = new KStateNode;
	pNode->m_SkillID = nSkillID;
	pNode->m_Level = nLevel;
	pNode->m_LeftTime = nTime;
	pNode->m_bOverLook = bOverLook;
	pNode->m_bTempStateGraphics = FALSE;
	if (pOrdinSkill)
		pNode->m_StateGraphics = pOrdinSkill->GetStateSpecailId();
	else
		pNode->m_StateGraphics = 0;
	pTemp = (KMagicAttrib *)pData;
	for (int i = 0; i < nDataNum; i++)
	{
		// ����NPC����
		ModifyAttrib(nLauncher, pTemp);
		// ���෴ֵ�����������Թ��Ƴ�ʱʹ��
		pNode->m_State[i].nAttribType = pTemp->nAttribType;
		pNode->m_State[i].nValue[0] = -pTemp->nValue[0];
		pNode->m_State[i].nValue[1] = -pTemp->nValue[1];
		pNode->m_State[i].nValue[2] = -pTemp->nValue[2];
		pTemp++;
	}
	m_StateSkillList.AddTail(pNode);
#ifdef _SERVER	
	UpdateNpcStateInfo();
#endif
}

/*!*****************************************************************************
// Function		: KNpc::ModifyMissleCollsion
// Purpose		: 
// Return		: BOOL 
// Argumant		: BOOL bCollsion
// Comments		:
// Author		: Spe
*****************************************************************************/
BOOL KNpc::ModifyMissleCollsion(BOOL bCollsion)
{
	if (bCollsion)
		return TRUE;

	if (g_RandPercent(m_CurrentPiercePercent))
		return TRUE;
	else
		return FALSE;
}

int KNpc::ModifyMissleLifeTime(int nLifeTime)
{
	if (IsPlayer())
	{
		//return Player[m_PlayerIdx].GetWeapon().GetRange();
		return nLifeTime;
	}
	else
	{
		return nLifeTime;
	}
}

int	KNpc::ModifyMissleSpeed(int nSpeed)
{
	if (m_CurrentSlowMissle)
	{
		return nSpeed / 2;
	}
	return nSpeed;
}

BOOL KNpc::DoBlurMove()
{
	if(m_Doing == do_hurt || m_Doing == do_death)
		return FALSE;

	if (m_SkillParam1 <= 0)
		return FALSE;

#ifndef _SERVER
	if (m_RegionIndex < 0 || m_RegionIndex >= 9)
	{
		_ASSERT(0);
		DoStand();
		return FALSE;
	}
#else
	_ASSERT(m_RegionIndex >= 0);
	if (m_RegionIndex < 0)
		return FALSE;
#endif

	KSkill * pSkill =(KSkill*) GetActiveSkill();
	if (!pSkill) 
        return FALSE;

	m_ProcessAI = 0;

	switch(m_SpecialSkillStep)
	{
	case 0:
		{
		int nX, nY;
		GetMpsPos(&nX, &nY);

		if (nX == m_DesX && nY == m_DesY)
		{
			DoStand();
			m_ProcessAI = 1;
			m_SpecialSkillStep = 0;
			return FALSE;
		}

		int nDir = g_GetDirIndex(nX, nY, m_DesX, m_DesY);
		int	nMaxLength = m_SkillParam1;
		int	nWantLength = SubWorld[m_SubWorldIndex].GetDistance(nX, nY, m_DesX, m_DesY);
		int	nSin = g_DirSin(nDir, 64);
		int	nCos = g_DirCos(nDir, 64);

		if (nWantLength > nMaxLength)
		{
			m_DesX = nX + ((nMaxLength * nCos) >> 10);
			m_DesY = nY + ((nMaxLength * nSin) >> 10);
			nWantLength = nMaxLength;
		}
		else if (nWantLength <= MIN_DOMELEE_RANGE)
		{
			return FALSE;
		}

		int nStep = nWantLength / MIN_BLURMOVE_SPEED;

		int nTestX = 0;
		int nTestY = 0;
		int nSuccessStep = 0;

		for (int i = 1; i < nStep + 1; i++)
		{
			nTestX = nX + ((MIN_BLURMOVE_SPEED * nCos * i) >> 10);
			nTestY = nY + ((MIN_BLURMOVE_SPEED * nSin * i) >> 10);
			int nBarrier = SubWorld[m_SubWorldIndex].GetBarrier(nTestX, nTestY);
			DWORD dwTrap = SubWorld[m_SubWorldIndex].GetTrap(nTestX, nTestY);
			if (Obstacle_NULL == nBarrier && dwTrap == 0)
			{
				nSuccessStep = i;
			}
			if (Obstacle_Normal == nBarrier || Obstacle_Fly == nBarrier || dwTrap)
			{
				if (nSuccessStep <= MIN_DOMELEE_RANGE / MIN_BLURMOVE_SPEED)
				{
					DoStand();
					m_ProcessAI = 1;
					m_SpecialSkillStep = 0;
					return FALSE;
				}
				m_DesX = nX + ((MIN_BLURMOVE_SPEED * nCos * nSuccessStep) >> 10);
				m_DesY = nY + ((MIN_BLURMOVE_SPEED * nSin * nSuccessStep) >> 10);
				nStep = nSuccessStep;
				break;
			}
		}
		}
		m_Doing = do_blurmove;
		break;
	case 1:
		m_ProcessAI = 0;
		m_Frames.nTotalFrame = pSkill->GetMissleGenerateTime(0);
		m_Frames.nCurrentFrame = 0;
		m_Doing = do_blurmove;
		break;
	case 2:
		{
#ifndef _SERVER
		int nX, nY;
		GetMpsPos(&nX, &nY);
#endif
		int nOldRegion = m_RegionIndex;
		int nOldMapX = m_MapX;
		int nOldMapY = m_MapY;
		int nOldOffX = m_OffX;
		int nOldOffY = m_OffY;
		
		if (!m_bClientOnly)
			CURREGION.DecRef(m_MapX, m_MapY, obj_npc);

		SubWorld[m_SubWorldIndex].Mps2Map(m_DesX, m_DesY, &m_RegionIndex,&m_MapX, &m_MapY, &m_OffX, &m_OffY);

		if (!m_bClientOnly && m_RegionIndex >= 0)
			CURREGION.AddRef(m_MapX, m_MapY, obj_npc);
		
		
		if (m_RegionIndex == -1)
		{
			m_RegionIndex = nOldRegion;
			m_MapX = nOldMapX;
			m_MapY = nOldMapY;
			m_OffX = nOldOffX;
			m_OffY = nOldOffY;
			CURREGION.AddRef(m_MapX, m_MapY, obj_npc);
			return FALSE;
		}
		
		if (nOldRegion != m_RegionIndex)
		{
#ifdef _SERVER
			SubWorld[m_SubWorldIndex].NpcChangeRegion(nOldRegion, m_RegionIndex, m_Index);
			if (IsPlayer())
			{
				SubWorld[m_SubWorldIndex].PlayerChangeRegion(nOldRegion, m_RegionIndex, m_nPlayerIdx);
			}
#else
			SubWorld[0].NpcChangeRegion(SubWorld[0].m_Region[nOldRegion].m_RegionID, SubWorld[0].m_Region[m_RegionIndex].m_RegionID, m_Index);
			m_dwRegionID = SubWorld[0].m_Region[m_RegionIndex].m_RegionID;
#endif
		}
#ifndef _SERVER
		m_DataRes.CreateBlur(m_Index, g_GetDistance(nX, nY, m_DesX, m_DesY), m_Dir);
#endif
		}
		break;
	}
	return TRUE;
}

void KNpc::OnBlurMove()
{
	if (m_SpecialSkillStep == 0)
	{
		m_SpecialSkillStep ++;
		DoBlurMove();
	}
	else if (m_SpecialSkillStep == 1)
	{
		if (WaitForFrame())
		{
			m_SpecialSkillStep ++;
			DoBlurMove();
		}
	}
	else
	{
		DoStand();
		m_ProcessAI = 1;
		m_SpecialSkillStep = 0;
	}
}

BOOL KNpc::DoManyAttack()
{
	m_ProcessAI = 0;
	
	KSkill * pSkill =(KSkill*) GetActiveSkill();
	if (!pSkill) 
        return FALSE;
	
	if (pSkill->GetChildSkillNum() <= m_SpecialSkillStep) 		
        goto ExitManyAttack;
#ifndef _SERVER
        m_DataRes.SetBlur(TRUE);
#endif
	
	m_Frames.nTotalFrame = pSkill->GetMissleGenerateTime(m_SpecialSkillStep);
	
	int x, y;
	SubWorld[m_SubWorldIndex].Map2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, &x, &y);
//	m_DesX = x;
//	m_DesY = y;

	
#ifndef _SERVER
	if (m_nPlayerIdx > 0)
		pSkill->PlayPreCastSound(m_nSex, x ,y);
	if (g_Random(2))
		m_ClientDoing = cdo_attack;
	else 
		m_ClientDoing = cdo_attack1;
#endif

	
	m_Doing = do_manyattack;
	
	m_Frames.nCurrentFrame = 0;

	return TRUE;

ExitManyAttack:

#ifndef _SERVER
		m_DataRes.SetBlur(FALSE);
#endif
	DoStand();
	m_ProcessAI = 1;
	m_SpecialSkillStep = 0;

	return TRUE;
}

void KNpc::OnManyAttack()
{
	if (WaitForFrame())
	{
#ifndef _SERVER
		m_DataRes.SetBlur(FALSE);
#endif
		KSkill * pSkill = (KSkill*)GetActiveSkill();
		if (!pSkill) 
            return ;

		int nPhySkillId =  pSkill->GetChildSkillId();//GetCurActiveWeaponSkill(); Changed 

		if (nPhySkillId > 0)
		{
			KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(nPhySkillId, pSkill->m_ulLevel, SKILL_SS_Missles);
			if (pOrdinSkill)
            {
				pOrdinSkill->Cast(m_Index, m_SkillParam1, m_SkillParam2);
            }
		}
		m_SpecialSkillStep ++;
		DoManyAttack();

	}	
}

BOOL	KNpc::DoRunAttack()
{
	m_ProcessAI = 0;

	switch(m_SpecialSkillStep)
	{
	case 0:
		m_Frames.nTotalFrame = m_RunSpeed;
		m_ProcessAI = 0;
		
#ifndef _SERVER
		m_DataRes.SetBlur(TRUE);

		if (m_FightMode)
		{
			m_ClientDoing = cdo_fightrun;
		}
		else
		{
			m_ClientDoing = cdo_run;
		}
#endif
		
		if (m_DesX < 0 && m_DesY > 0) 
		{
			int x, y;
			SubWorld[m_SubWorldIndex].Map2Mps
				(
				Npc[m_DesY].m_RegionIndex,
				Npc[m_DesY].m_MapX, 
				Npc[m_DesY].m_MapY, 
				Npc[m_DesY].m_OffX, 
				Npc[m_DesY].m_OffY, 
				&x,
				&y
				);

		m_DesX = x;
		m_DesY = y;
		}

		m_Frames.nCurrentFrame = 0;
		m_Doing = do_runattack;
		break;

	case 1:
#ifndef _SERVER
		if (g_Random(2))	
			m_ClientDoing = cdo_attack;
		else
			m_ClientDoing = cdo_attack1;

		int x, y, tx, ty;
		SubWorld[m_SubWorldIndex].Map2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, &x, &y);
		if (m_SkillParam1 == -1)
		{
			Npc[m_SkillParam2].GetMpsPos(&tx, &ty);
		}
		else
		{
			tx = m_SkillParam1;
			ty = m_SkillParam2;
		}
		m_Dir = g_GetDirIndex(x, y, tx, ty);
#endif
		m_Frames.nTotalFrame = 0;
		m_Frames.nCurrentFrame = 0;
		m_Doing = do_runattack;
		break;

	case 2:
	case 3:
#ifndef _SERVER
		m_DataRes.SetBlur(FALSE);
#endif
		DoStand();
		m_ProcessAI = 1;
		m_SpecialSkillStep = 0;
		return FALSE;
		break;
	}

	m_Frames.nCurrentFrame = 0;
		
	return TRUE;

}

void	KNpc::OnRunAttack()
{
	if (m_SpecialSkillStep == 0)
	{
		OnRun();
		KSkill * pSkill = (KSkill*)GetActiveSkill();
		if (!pSkill) 
            return ;
		
        if (m_Doing == do_stand || (DWORD)m_nCurrentMeleeTime > pSkill->GetMissleGenerateTime(0)) 
		{
			m_SpecialSkillStep ++;
			m_nCurrentMeleeTime = 0;

			DoRunAttack();
		
		}
		else
		{
			KSkill * pSkill = (KSkill*)GetActiveSkill();
			if (!pSkill) 
                return ;
			
            int nCurPhySkillId = pSkill->GetChildSkillId();//GetCurActiveWeaponSkill();
			if (nCurPhySkillId > 0)
			{
				KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(nCurPhySkillId, pSkill->m_ulLevel, SKILL_SS_Missles);
				if (pOrdinSkill)
                {
				    pOrdinSkill->Cast(m_Index, m_SkillParam1, m_SkillParam2);
                }
			}
			m_nCurrentMeleeTime ++;
		}

		m_ProcessAI = 0;
	}
	else if (m_SpecialSkillStep == 1)
	{
		if (WaitForFrame() &&m_Frames.nTotalFrame != 0)
		{
			DoStand();
			m_ProcessAI = 1;	
		}
		else if (IsReachFrame(ATTACKACTION_EFFECT_PERCENT))
		{
			KSkill * pSkill = (KSkill*)GetActiveSkill();
			if (!pSkill) 
                return ;
			
            int nCurPhySkillId = pSkill->GetChildSkillId();//GetCurActiveWeaponSkill();
			if (nCurPhySkillId > 0)
			{
				KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(nCurPhySkillId, pSkill->m_ulLevel, SKILL_SS_Missles);
				if (pOrdinSkill)
                {
				    pOrdinSkill->Cast(m_Index, m_SkillParam1, m_SkillParam2);
                }
			}
			DoStand();
			m_ProcessAI = 1;
			m_SpecialSkillStep = 0;
		}
#ifndef _SERVER
		m_DataRes.SetBlur(FALSE);
#endif
	}
	else
	{
#ifndef _SERVER
		m_DataRes.SetBlur(FALSE);
#endif
		DoStand();
		m_ProcessAI = 1;
		m_SpecialSkillStep = 0;
	}
}

BOOL KNpc::DoJumpAttack()
{
	m_ProcessAI = 0;
	
	switch(m_SpecialSkillStep)
	{
	case 0:
	{
		DoJump();

#ifndef _SERVER
		m_DataRes.SetBlur(TRUE);
		m_ClientDoing = cdo_jump;
#endif
		m_Doing = do_jumpattack;
		break;
		
	case 1:
#ifndef _SERVER
		if (g_Random(2))	
			m_ClientDoing = cdo_attack;
		else
			m_ClientDoing = cdo_attack1;
		int x, y, tx, ty;
		SubWorld[m_SubWorldIndex].Map2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, &x, &y);
		if (m_SkillParam1 == -1)
		{
			Npc[m_SkillParam2].GetMpsPos(&tx, &ty);
		}
		else
		{
			tx = m_SkillParam1;
			ty = m_SkillParam2;
		}
		m_Dir = g_GetDirIndex(x, y, tx, ty);
#endif
		m_Frames.nTotalFrame = m_AttackFrame * MAX_PERCENT / (MAX_PERCENT + m_CurrentAttackSpeed);
		m_Frames.nCurrentFrame = 0;
		m_Doing = do_jumpattack;
		break;
	}
	case 2:
	case 3:
#ifndef _SERVER
		m_DataRes.SetBlur(FALSE);
#endif
		DoStand();
		m_ProcessAI = 1;
		m_SpecialSkillStep = 0;
		return FALSE;
		break;
	}
	
	m_Frames.nCurrentFrame = 0;
	
	return TRUE;
	
}

BOOL KNpc::OnJumpAttack()
{
	if (m_SpecialSkillStep == 0)
	{
		if (!OnJump())
		{
			m_SpecialSkillStep ++;
			m_nCurrentMeleeTime = 0;
			DoJumpAttack();
		}
		m_ProcessAI = 0;
	}
	else if (m_SpecialSkillStep == 1)
	{
#ifndef _SERVER
		m_DataRes.SetBlur(FALSE);
#endif
		if (WaitForFrame() &&m_Frames.nTotalFrame != 0)
		{
			DoStand();
			m_ProcessAI = 1;	
		}
		else if (IsReachFrame(ATTACKACTION_EFFECT_PERCENT))
		{
			KSkill * pSkill =(KSkill*) GetActiveSkill();
			if (!pSkill) 
                return FALSE;
			
            int nCurPhySkillId = pSkill->GetChildSkillId();//GetCurActiveWeaponSkill();
			if (nCurPhySkillId > 0)
			{
				KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(nCurPhySkillId, pSkill->m_ulLevel, SKILL_SS_Missles);
				if (pOrdinSkill)
                {
					pOrdinSkill->Cast(m_Index, m_SkillParam1, m_SkillParam2);
                }
			}
			DoStand();
			m_ProcessAI = 1;
			m_SpecialSkillStep = 0;
		}
	}
	else
	{
#ifndef _SERVER
		m_DataRes.SetBlur(FALSE);
#endif
		DoStand();
		m_ProcessAI = 1;
		m_SpecialSkillStep = 0;
		return FALSE;
	}
	return TRUE;
}

BOOL KNpc::CheckHitTarget(int nAR, int nDf, int nIngore/* = 0*/)
{
	int nDefense = 0;
	if (nIngore < MAX_PERCENT)
		nDefense = nDf * (MAX_PERCENT - nIngore) / MAX_PERCENT;
	int nPercent = 0;
	if (nAR < 0)
		return FALSE;

	if (nDf < 0)
		nPercent = MAX_HIT_PERCENT;
	else if ((nAR + nDefense) == 0) 
		nPercent = 50;
	else
		nPercent = nAR * MAX_PERCENT / (nAR + nDefense);

	if (nPercent > MAX_HIT_PERCENT + 4)
		nPercent = MAX_HIT_PERCENT;

	if (nPercent < 40)
		nPercent = 40;

	BOOL bRet = g_RandPercent(nPercent);
	return bRet;
}

void KNpc::GetNpcCopyFromTemplate(int nNpcTemplateId)
{
	if (nNpcTemplateId < 0) 
		return ;

	if (g_pNpcTemplate[nNpcTemplateId]) //������Ч�򿽱���������������
		LoadDataFromTemplate(nNpcTemplateId);
	else
	{
		g_pNpcTemplate[nNpcTemplateId] = new KNpcTemplate;
		g_pNpcTemplate[nNpcTemplateId]->m_NpcSettingIdx = nNpcTemplateId;
		g_pNpcTemplate[nNpcTemplateId]->Init(nNpcTemplateId);
		g_pNpcTemplate[nNpcTemplateId]->m_bHaveLoadedFromTemplate = TRUE;
		LoadDataFromTemplate(nNpcTemplateId);
	}
}

void	KNpc::LoadDataFromTemplate(int nNpcTemplateId)
{
	if (nNpcTemplateId < 0 )
	{
		g_DebugLog("NpcTemplateId < 0");
		return ;
	}
	
	KNpcTemplate * pNpcTemp = g_pNpcTemplate[nNpcTemplateId];

	m_HeadImage =	pNpcTemp->m_HeadImage;
	m_CorpseSettingIdx =	pNpcTemp->m_CorpseSettingIdx;
	m_DeathFrame =	pNpcTemp->m_DeathFrame;
	m_WalkFrame =	pNpcTemp->m_WalkFrame;
	m_RunFrame =	pNpcTemp->m_RunFrame;
	m_HurtFrame =	pNpcTemp->m_HurtFrame;
	m_WalkSpeed =	pNpcTemp->m_WalkSpeed;
	m_StandFrame =  pNpcTemp->m_StandFrame;
	m_StandFrame1 = pNpcTemp->m_StandFrame1;

#ifndef _SERVER
	m_ArmorType				= pNpcTemp->m_ArmorType;
	m_HelmType				= pNpcTemp->m_HelmType;
	m_WeaponType			= pNpcTemp->m_WeaponType;
#endif

	if(m_Kind != kind_player)
	{
		m_AttackFrame =	pNpcTemp->m_AttackFrame;
		m_CastFrame =	pNpcTemp->m_CastFrame;
		m_RunSpeed =	pNpcTemp->m_RunSpeed;
#ifndef _SERVER
		m_HorseType				= pNpcTemp->m_HorseType;
		m_bRideHorse			= pNpcTemp->m_bRideHorse;
#endif
		strcpy(Name, pNpcTemp->Name);
		m_Kind = pNpcTemp->m_Kind;
		m_Camp = pNpcTemp->m_Camp;
		m_Series = pNpcTemp->m_Series;
		m_bClientOnly = pNpcTemp->m_bClientOnly;
		m_NpcSettingIdx = pNpcTemp->m_NpcSettingIdx;
		m_nStature		= pNpcTemp->m_nStature;

#ifdef _SERVER	
		m_SkillList		= pNpcTemp->m_SkillList;
		m_AiMode		= pNpcTemp->m_AiMode;
		m_AiAddLifeTime	= 0;

		if (!m_AiSkillRadiusLoadFlag)
		{
			m_AiSkillRadiusLoadFlag = 1;
			int i = 0;
			for (i = 0; i < MAX_AI_PARAM - 1; i ++)
				m_AiParam[i] =	pNpcTemp->m_AiParam[i];

			int		nMaxRadius = 0, nTempRadius;
			KSkill	*pSkill;
			for (i = 1; i < MAX_NPC_USE_SKILL + 1; i++)
			{
				pSkill = (KSkill*)g_SkillManager.GetSkill(m_SkillList.m_Skills[i].SkillId, m_SkillList.m_Skills[i].CurrentSkillLevel);
				if (!pSkill)
					continue;
				nTempRadius = pSkill->GetAttackRadius();
				if (nTempRadius > nMaxRadius)
					nMaxRadius = nTempRadius;
			}
			m_AiParam[MAX_AI_PARAM - 1] = nMaxRadius * nMaxRadius;
		}

		m_FireResistMax			= pNpcTemp->m_FireResistMax;
		m_ColdResistMax			= pNpcTemp->m_ColdResistMax;
		m_LightResistMax		= pNpcTemp->m_LightResistMax;
		m_PoisonResistMax		= pNpcTemp->m_PoisonResistMax;
		m_PhysicsResistMax		= pNpcTemp->m_PhysicsResistMax;
		m_ActiveRadius			= pNpcTemp->m_ActiveRadius;
		m_VisionRadius			= pNpcTemp->m_VisionRadius;
		m_AIMAXTime				= pNpcTemp->m_AIMAXTime;
		m_HitRecover			= pNpcTemp->m_HitRecover;
		m_ReviveFrame			= pNpcTemp->m_ReviveFrame;
		m_Experience			= pNpcTemp->m_Experience;
		m_CurrentExperience		= m_Experience;
		m_LifeReplenish			= pNpcTemp->m_LifeReplenish;
		m_AttackRating			= pNpcTemp->m_AttackRating;
		m_Defend				= pNpcTemp->m_Defend;
		m_PhysicsDamage			= pNpcTemp->m_PhysicsDamage;
		m_RedLum				= pNpcTemp->m_RedLum;
		m_GreenLum				= pNpcTemp->m_GreenLum;
		m_BlueLum				= pNpcTemp->m_BlueLum;
		m_FireResist			= pNpcTemp->m_FireResist;
		m_ColdResist			= pNpcTemp->m_ColdResist;
		m_LightResist			= pNpcTemp->m_LightResist;
		m_PoisonResist			= pNpcTemp->m_PoisonResist;
		m_PhysicsResist			= pNpcTemp->m_PhysicsResist;
#endif
		RestoreNpcBaseInfo();
	}
}

//-----------------------------------------------------------------------
//	���ܣ��趨���������������Сֵ not end ��Ҫ���ǵ��õĵط�
//-----------------------------------------------------------------------
void	KNpc::SetPhysicsDamage(int nMinDamage, int nMaxDamage)
{
	m_PhysicsDamage.nValue[0] = nMinDamage;
	m_PhysicsDamage.nValue[2] = nMaxDamage;
}

void	KNpc::SetReviveFrame(int nReviveFrame)
{
	m_ReviveFrame = nReviveFrame;
}


//-----------------------------------------------------------------------
//	���ܣ��趨����������
//-----------------------------------------------------------------------
void	KNpc::SetBaseAttackRating(int nAttackRating)
{
	m_AttackRating = nAttackRating;
	// �˴�����Ҫ����װ�������ܵ�Ӱ�죬�������ǰֵ
	m_CurrentAttackRating = m_AttackRating;
}

//-----------------------------------------------------------------------
//	���ܣ��趨������
//-----------------------------------------------------------------------
void	KNpc::SetBaseDefence(int nDefence)
{
	m_Defend = nDefence;
	// �˴�����Ҫ����װ�������ܵ�Ӱ�죬�������ǰֵ
	m_CurrentDefend = m_Defend;
}

/*
//-----------------------------------------------------------------------
//	���ܣ��趨�����ٶ�
//-----------------------------------------------------------------------
void	KNpc::SetBaseWalkSpeed(int nSpeed)
{
	m_WalkSpeed = nSpeed;
	// �˴�����Ҫ����װ�������ܵ�Ӱ�죬�������ǰֵ (not end)
	m_CurrentWalkSpeed = m_WalkSpeed;
}
*/

/*
//-----------------------------------------------------------------------
//	���ܣ��趨�ܲ��ٶ�
//-----------------------------------------------------------------------
void	KNpc::SetBaseRunSpeed(int nSpeed)
{
	m_RunSpeed = nSpeed;
	// �˴�����Ҫ����װ�������ܵ�Ӱ�죬�������ǰֵ (not end)
	m_CurrentRunSpeed = m_RunSpeed;
}
*/

#ifdef _SERVER
void KNpc::DeathPunish(int nMode, int nBelongPlayer)
{
#define	LOSE_EXP_SCALE		10

	if (IsPlayer())
	{
		// ��npc kill
		if (nMode == enumDEATH_MODE_NPC_KILL)
		{
			Player[m_nPlayerIdx].m_ItemList.Abrade(enumAbradeDefend);
			// �������
			//if (Player[m_nPlayerIdx].m_nExp > 0)
			//{
				int nSubExp;
				if (m_Level <= 10)
					nSubExp = (PlayerSet.m_cLevelAdd.GetLevelExp(m_Level, m_byTranslife)  / MAX_PERCENT) * 2;
				else
					nSubExp = (PlayerSet.m_cLevelAdd.GetLevelExp(m_Level, m_byTranslife)  / MAX_PERCENT) * 4;

				Player[m_nPlayerIdx].DirectAddExp( -nSubExp );
			//}
			// Ǯ����
			int nMoney = Player[m_nPlayerIdx].m_ItemList.GetEquipmentMoney() / 2;
			if (nMoney > 0)
			{
				Player[m_nPlayerIdx].m_ItemList.CostMoney(nMoney);
				// ��ʧ��Ǯ��Ϣ
				SHOW_MSG_SYNC	sMsg;
				sMsg.ProtocolType = s2c_msgshow;
				sMsg.m_wMsgID = enumMSG_ID_DEC_MONEY;
				sMsg.m_lpBuf = (void *)(nMoney);
				sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
				g_pServer->PackDataToClient(Player[m_nPlayerIdx].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
				sMsg.m_lpBuf = 0;

				if (nMoney / 2 > 0)
					PlayerDeadCreateMoneyObj(nMoney / 2);
			}
		}
		// �д裬û�гͷ�
		else if (nMode == enumDEATH_MODE_PLAYER_NO_PUNISH)
		{
			return;
		}
		else if (nMode == enumDEATH_MODE_PLAYER_SPAR_NO_PUNISH)
		{
			return;
		}
		else if (nMode == enumDEATH_MODE_PKBATTLE_PUNISH || nMode == enumDEATH_MODE_TOURNAMENTS_PUNISH)
		{
			return;
		}
		// PK��������PKֵ����ͷ�
		else //if (nMode == enumDEATH_MODE_PLAYER_PUNISH)
		{
			Player[m_nPlayerIdx].m_ItemList.Abrade(enumAbradeDefend);

			int	nPKValue;
			nPKValue = Player[this->m_nPlayerIdx].m_cPK.GetPKValue();
			if (nPKValue < 0)
				nPKValue = 0;
			if (nPKValue > MAX_DEATH_PUNISH_PK_VALUE)
				nPKValue = MAX_DEATH_PUNISH_PK_VALUE;

			// �������
			if(m_Level < NpcSet.m_nLevelBoundaryPKPunish)
			{
				DWORD		dwLevelExp = PlayerSet.m_cLevelAdd.GetLevelExp(m_Level, m_byTranslife);
				Player[m_nPlayerIdx].DirectAddExp( -(dwLevelExp / MAX_PERCENT * PlayerSet.m_sPKPunishParam[nPKValue].m_nExpP) );
			}
			else
				Player[m_nPlayerIdx].DirectAddExp( -PlayerSet.m_sPKPunishParam[nPKValue].m_nExpV );

			// Ǯ����
			int nMoney = Player[m_nPlayerIdx].m_ItemList.GetEquipmentMoney() * PlayerSet.m_sPKPunishParam[nPKValue].m_nMoney / MAX_PERCENT;
			if (nMoney > 0)
			{
				Player[m_nPlayerIdx].m_ItemList.CostMoney(nMoney);
				// ��ʧ��Ǯ��Ϣ
				SHOW_MSG_SYNC	sMsg;
				sMsg.ProtocolType = s2c_msgshow;
				sMsg.m_wMsgID = enumMSG_ID_DEC_MONEY;
				sMsg.m_lpBuf = (void *)(nMoney);
				sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
				g_pServer->PackDataToClient(Player[m_nPlayerIdx].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
				sMsg.m_lpBuf = 0;
				
				if (nMoney / 2 > 0)
					PlayerDeadCreateMoneyObj(nMoney / 2);
			}

			// ��ʧ��Ʒ
			Player[m_nPlayerIdx].m_ItemList.AutoLoseItemFromEquipmentRoom(PlayerSet.m_sPKPunishParam[nPKValue].m_nItem);

			// ��ʧ�������ϵ�װ��
			if (g_Random(MAX_PERCENT) < PlayerSet.m_sPKPunishParam[nPKValue].m_nEquip)
			{
				Player[m_nPlayerIdx].m_ItemList.AutoLoseEquip();
			}
			Player[m_nPlayerIdx].m_cPK.AddPKValue(NpcSet.m_nBeKilledAddPKValue);
			if (m_nLastDamageIdx)
			{
				if (Npc[m_nLastDamageIdx].IsPlayer())
				{
					KPlayerChat::MakeEnemy(Name, Npc[m_nLastDamageIdx].Name);
				}
			}
		}
	}
}

// �������ʱ���������Ǯ����һ��object
void	KNpc::PlayerDeadCreateMoneyObj(int nMoneyNum)
{
	int		nX, nY;
	POINT	ptLocal;
	KMapPos	Pos;

	GetMpsPos(&nX, &nY);
	ptLocal.x = nX;
	ptLocal.y = nY;
	SubWorld[m_SubWorldIndex].GetFreeObjPos(ptLocal);

	Pos.nSubWorld = m_SubWorldIndex;
	SubWorld[m_SubWorldIndex].Mps2Map(ptLocal.x, ptLocal.y, 
		&Pos.nRegion, &Pos.nMapX, &Pos.nMapY, 
		&Pos.nOffX, &Pos.nOffY);
	
	int nObjIdx = ObjSet.AddMoneyObj(Pos, nMoneyNum);
	if (nObjIdx > 0 && nObjIdx < MAX_OBJECT)
	{
		Object[nObjIdx].SetItemBelong(-1);
	}
}

void KNpc::Revive()
{	
	RestoreNpcBaseInfo();
	int nRegion, nMapX, nMapY, nOffX, nOffY;
	SubWorld[m_SubWorldIndex].Mps2Map(m_OriginX, m_OriginY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
	m_RegionIndex = nRegion;
	m_MapX = nMapX;
	m_MapY = nMapY;
	m_MapZ = 0;
	m_OffX = nOffX;
	m_OffY = nOffY;
	if (m_RegionIndex < 0)
		return;
	SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].AddRef(m_MapX, m_MapY, obj_npc);
#ifdef _SERVER
	//SubWorld[m_SubWorldIndex].m_WorldMessage.Send(GWM_NPC_CHANGE_REGION, VOID_REGION, nRegion, m_Index);
	SubWorld[m_SubWorldIndex].NpcChangeRegion(VOID_REGION, nRegion, m_Index);	// spe 03/06/28

	if (m_ActionScriptID)
		NpcSet.ExecuteScript(m_Index, m_ActionScriptID, "Revive", m_Index);
#else
	SubWorld[0].NpcChangeRegion(VOID_REGION, SubWorld[0].m_Region[nRegion].m_RegionID, m_Index);
#endif
	DoStand();
	m_ProcessAI = 1;
	m_ProcessState = 1;
	m_AiAddLifeTime = 0;
}

void KNpc::RestoreLiveData()
{

}
#endif



#ifdef	_SERVER
// ����Χ�����㲥
void	KNpc::SendDataToNearRegion(void* pBuffer, DWORD dwSize)
{
	_ASSERT(m_RegionIndex >= 0);
	if (m_RegionIndex < 0)
		return;

	POINT	POff[8] = 
	{
		{0, 32},
		{-16, 32},
		{-16, 0},
		{-16, -32},
		{0, -32},
		{16, -32},
		{16, 0},
		{16, 32},
	};
	int nMaxCount = MAX_BROADCAST_COUNT;
	SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].BroadCast(pBuffer, dwSize, nMaxCount, m_MapX, m_MapY);
	for (int i= 0; i < 8; i++)
	{
		if (SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].m_nConnectRegion[i] < 0)
			continue;
		SubWorld[m_SubWorldIndex].m_Region[SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].m_nConnectRegion[i]].BroadCast(pBuffer, dwSize, nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
	}
}
#endif



#ifdef	_SERVER
//-----------------------------------------------------------------------------
//	���ܣ�����ʱ�����PKֵ
//-----------------------------------------------------------------------------
int		KNpc::DeathCalcPKValue(int nKiller)
{
	// ����
	if (nKiller <= 0 || nKiller >= MAX_NPC)
		return enumDEATH_MODE_NPC_KILL;
	
	if (m_nCurPKPunishState == enumDEATH_MODE_PKBATTLE_PUNISH || 
		m_nCurPKPunishState == enumDEATH_MODE_TOURNAMENTS_PUNISH)
		return m_nCurPKPunishState;

	// ���֮�䣬������
	if (this->m_Kind != kind_player || Npc[nKiller].m_Kind != kind_player || !m_FightMode)
		return enumDEATH_MODE_NPC_KILL;
	// ������д裬������
	if (Player[m_nPlayerIdx].m_cPK.GetExercisePKAim() == Npc[nKiller].m_nPlayerIdx)
	{
		if (Player[m_nPlayerIdx].m_cPK.IsEnmitySpar())
		{
			SHOW_MSG_SYNC	sMsg;
			sMsg.ProtocolType = s2c_msgshow;
			sMsg.m_wMsgID = enumMSG_ID_SPAR_DEFEAT;
			sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
			g_pServer->PackDataToClient(Player[m_nPlayerIdx].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
			sMsg.m_wMsgID = enumMSG_ID_SPAR_VICTORY;
			sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
			g_pServer->PackDataToClient(Player[Npc[nKiller].m_nPlayerIdx].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
			return enumDEATH_MODE_PLAYER_SPAR_NO_PUNISH;
		}

		return enumDEATH_MODE_PLAYER_NO_PUNISH;
	}
	// ����ǳ�ɱ
	if (Player[m_nPlayerIdx].m_cPK.GetEnmityPKState() == enumPK_ENMITY_STATE_PKING &&
		Player[m_nPlayerIdx].m_cPK.GetEnmityPKAim() == Npc[nKiller].m_nPlayerIdx)
	{
		if (Player[Npc[nKiller].m_nPlayerIdx].m_cPK.IsEnmitySpar())
		{
			SHOW_MSG_SYNC	sMsg;
			sMsg.ProtocolType = s2c_msgshow;
			sMsg.m_wMsgID = enumMSG_ID_SPAR_DEFEAT;
			sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
			g_pServer->PackDataToClient(Player[m_nPlayerIdx].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
			sMsg.m_wMsgID = enumMSG_ID_SPAR_VICTORY;
			sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
			g_pServer->PackDataToClient(Player[Npc[nKiller].m_nPlayerIdx].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
			return enumDEATH_MODE_PLAYER_SPAR_NO_PUNISH;
		}
		else
		{
			if (Player[Npc[nKiller].m_nPlayerIdx].m_cPK.IsEnmityPKLauncher())
				Player[Npc[nKiller].m_nPlayerIdx].m_cPK.AddPKValue(NpcSet.m_nEnmityAddPKValue);

			Player[Npc[nKiller].m_nPlayerIdx].m_nKillPeopleNumber++;

			return enumDEATH_MODE_PLAYER_PUNISH;
		}
	}
	if (Player[m_nPlayerIdx].m_cPK.GetNormalPKState() != enumPKMurder && Player[Npc[nKiller].m_nPlayerIdx].m_cPK.GetNormalPKState() == enumPKMurder)
	{
		Player[Npc[nKiller].m_nPlayerIdx].m_cPK.AddPKValue(NpcSet.m_nMurderAddPKValue);

		Player[Npc[nKiller].m_nPlayerIdx].m_nKillPeopleNumber++;

		return enumDEATH_MODE_PLAYER_PUNISH;
	}
	if (m_Level <= 50 && Npc[nKiller].m_Level * 2 >= m_Level * 3)
	{
		if (!Player[m_nPlayerIdx].m_cPK.GetNormalPKState())
		{
			if (Npc[nKiller].m_CurrentCamp == camp_free)
				Player[Npc[nKiller].m_nPlayerIdx].m_cPK.AddPKValue(NpcSet.m_nKillerPKFactionAddPKValue);
			else
				Player[Npc[nKiller].m_nPlayerIdx].m_cPK.AddPKValue(NpcSet.m_nFactionPKFactionAddPKValue);
		}
		Player[Npc[nKiller].m_nPlayerIdx].m_nKillPeopleNumber++;

		return enumDEATH_MODE_PLAYER_PUNISH;
	}

	return enumDEATH_MODE_PLAYER_PUNISH;
}
#endif

#ifdef	_SERVER
//-----------------------------------------------------------------------------
//	���ܣ�������Χ9��Region���Ƿ���ָ���� player
//-----------------------------------------------------------------------------
int	KNpc::FindAroundPlayer(const char* Name)
{
	int nNpc = 0;
	if (Name[0] <= 0 || m_RegionIndex < 0)
		return nNpc;
	nNpc = SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].SearchNpcName(Name);
	if (nNpc)
		return nNpc;
	int		nRegionNo;
	for (int i = 0; i < 8; i++)
	{
		nRegionNo = SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].m_nConnectRegion[i];
		if ( nRegionNo < 0)
			continue;
		nNpc = SubWorld[m_SubWorldIndex].m_Region[nRegionNo].SearchNpcName(Name);
		if (nNpc)
			return nNpc;
	}
	return nNpc;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	���ܣ��趨ͷ��״̬
//-------------------------------------------------------------------------
void	KNpc::SetMenuState(int nState, char *lpszSentence, int nLength)
{
	this->m_DataRes.SetMenuState(nState, lpszSentence, nLength);
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	���ܣ����ͷ��״̬
//-------------------------------------------------------------------------
int		KNpc::GetMenuState()
{
	return this->m_DataRes.GetMenuState();
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	���ܣ�������Χ9��Region���Ƿ���ָ�� ID �� npc
//-------------------------------------------------------------------------
DWORD	KNpc::SearchAroundID(DWORD dwID)
{
	int		nIdx, nRegionNo;
	nIdx = SubWorld[0].m_Region[m_RegionIndex].SearchNpc(dwID);
	if (nIdx)
		return nIdx;
	for (int i = 0; i < 8; i++)
	{
		nRegionNo = SubWorld[0].m_Region[m_RegionIndex].m_nConnectRegion[i];
		if ( nRegionNo < 0)
			continue;
		nIdx = SubWorld[0].m_Region[nRegionNo].SearchNpc(dwID);
		if (nIdx)
			return nIdx;
	}
	return 0;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	���ܣ��趨�����ֻ����һ�������spr�ļ�
//-------------------------------------------------------------------------
void	KNpc::SetSpecialSpr(char *lpszSprName)
{
	m_DataRes.SetSpecialSpr(lpszSprName);
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	���ܣ��趨˲����Ч
//-------------------------------------------------------------------------
void	KNpc::SetInstantSpr(int nNo)
{
	char	szName[FILE_NAME_LENGTH];
	szName[0] = 0;
	NpcSet.m_cInstantSpecial.GetSprName(nNo, szName, sizeof(szName));
	if (szName[0])
		this->SetSpecialSpr(szName);
}
#endif

#ifndef _SERVER
int		KNpc::GetNormalNpcStandDir(int nFrame)
{
	return m_DataRes.GetNormalNpcStandDir(nFrame);
}

void KNpc::GetNpcResFile(int nNpcSettingIdx, char* pszResPath)
{
	if (nNpcSettingIdx < 0)
		return;

	char szNpcTypeName[32];
	g_NpcSetting.GetString(nNpcSettingIdx + 2, "NpcResType", "", szNpcTypeName, sizeof(szNpcTypeName));
	m_DataRes.m_pcResTemp = g_NpcResList.AddNpcRes(szNpcTypeName);
	if ( m_DataRes.m_pcResTemp == NULL )
	{
		strcpy(pszResPath, UNKNOWNITEM_SPR);
		return;
	}
	m_DataRes.GetResFile(::GetRandomNumber(1, 12), pszResPath);
	if (!pszResPath[0])
		m_DataRes.GetResFile(0, pszResPath);
}

void KNpc::KeyToImage(char* szKey, int nAction, KUiImage* pImage)
{
	if (szKey[0] == 0)
	{
		memset(pImage->Name, 0, sizeof(pImage->Name));
		pImage->Name[0] = 0x20;
	}
	else
	{
		KImageParam sImage;
		if (g_pRepresent->GetImageParam(szKey, &sImage, ISI_T_SPR) == true)
		{
			strcpy(pImage->Name, szKey);
			pImage->Frame = sImage.nNumFrames;
		}
		else
		{
			if (strcmp(szKey, NPCNAME_KEY) != 0)
			{
				m_DataRes.m_pcResTemp = g_NpcResList.AddNpcRes(szKey);
				if ( m_DataRes.m_pcResTemp == NULL )
				{
					strcpy(pImage->Name, UNKNOWNITEM_SPR);
					pImage->Frame = 0;
				}
				else
				{
					for (int i = 0; i < MAX_PART; i++) 
					{
						m_DataRes.m_pcResTemp->GetFileName(i, nAction, 0, "", pImage->Name, sizeof(pImage->Name)); 
						if (pImage->Name[0]) 
						{ 
							pImage->Frame = (m_DataRes.m_pcResTemp->GetTotalFrames(i, nAction, 0, MAX_PART))/ 
							(m_DataRes.m_pcResTemp->GetTotalDirs(i, nAction, 0, MAX_PART)); return; 
						} 
					} 
					for (int j = 0; j < MAX_PART; j++) 
					{  
						m_DataRes.m_pcResTemp->GetFileName(j, 0, 0, "", pImage->Name, sizeof(pImage->Name)); 
						if (pImage->Name[0]) 
						{ 
							pImage->Frame = (m_DataRes.m_pcResTemp->GetTotalFrames(j, 0, 0, MAX_PART))/ 
							(m_DataRes.m_pcResTemp->GetTotalDirs(j, 0, 0, MAX_PART)); return; 
						} 
					}
				}
			}
			else
			{ 
				if (Player[m_nPlayerIdx].m_nLastNpcIndex) 
				{
					for (int i = 0; i < MAX_PART; i++) 
					{
						Npc[Player[m_nPlayerIdx].m_nLastNpcIndex].GetNpcRes()->m_pcResNode->GetFileName(i, nAction, 0, "", pImage->Name, sizeof(pImage->Name)); 
						if (pImage->Name[0]) 
						{ 
							pImage->Frame = (Npc[Player[m_nPlayerIdx].m_nLastNpcIndex].GetNpcRes()->m_pcResNode->GetTotalFrames(i, nAction, 0, MAX_PART))/ 
							(Npc[Player[m_nPlayerIdx].m_nLastNpcIndex].GetNpcRes()->m_pcResNode->GetTotalDirs(i, nAction, 0, MAX_PART)); return; 
						} 
					} 
					for (int j = 0; j < MAX_PART; j++) 
					{  
						Npc[Player[m_nPlayerIdx].m_nLastNpcIndex].GetNpcRes()->m_pcResNode->GetFileName(j, 0, 0, "", pImage->Name, sizeof(pImage->Name)); 
						if (pImage->Name[0]) 
						{ 
							pImage->Frame = (Npc[Player[m_nPlayerIdx].m_nLastNpcIndex].GetNpcRes()->m_pcResNode->GetTotalFrames(j, 0, 0, MAX_PART))/ 
							(Npc[Player[m_nPlayerIdx].m_nLastNpcIndex].GetNpcRes()->m_pcResNode->GetTotalDirs(j, 0, 0, MAX_PART)); return; 
						} 
					}
				}
			}
		}
	}
}
#endif

#ifdef _SERVER
//���¸��½�ɫ״̬��Ϣ����
void	KNpc::UpdateNpcStateInfo()
{
	int i = 0, j = 0;
	memset(m_btStateInfo, 0 ,sizeof(BYTE) * MAX_SKILL_STATE);
	KStateNode *pNode = (KStateNode*)m_StateSkillList.GetTail();

	if (m_ActiveAuraID)
	{
		if (m_SkillList.GetLevel(m_ActiveAuraID) > 0)
		{
			KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(m_ActiveAuraID, 1);
			if (pOrdinSkill)
			{
				if (pOrdinSkill->GetStateSpecailId())
					m_btStateInfo[i++] = pOrdinSkill->GetStateSpecailId();

				if(pOrdinSkill->GetAppendSkillNum())
				{
					for( j = 0; j < pOrdinSkill->GetAppendSkillNum(); j++)
					{
						if(m_SkillList.GetLevel(pOrdinSkill->GetAppendSkillId(j)) <= 0)
							continue;
						pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(pOrdinSkill->GetAppendSkillId(j), 1);
						if (pOrdinSkill)
						{
							if (pOrdinSkill->GetStateSpecailId())
								m_btStateInfo[i++] = pOrdinSkill->GetStateSpecailId();
						}
					}
				}
			}
		}
	}

	while ( pNode && i < MAX_SKILL_STATE)
	{
		if (pNode->m_StateGraphics > 0)
			m_btStateInfo[i++] = pNode->m_StateGraphics;
		pNode = (KStateNode*)pNode->GetPrev();
	}

	for (i; i < MAX_SKILL_STATE;i++)
		m_btStateInfo[i] = 0;
}
#endif

#ifndef _SERVER
void	KNpc::SetNpcState(BYTE* pNpcState)
{
	if (!pNpcState)
		return ;

	memcpy(m_btStateInfo, pNpcState, sizeof(BYTE) * MAX_SKILL_STATE);
}
#endif

void	KNpc::ClearNpcState()
{
	KStateNode * pNode = (KStateNode*)m_StateSkillList.GetHead();
	KStateNode * pTempNode = NULL;
	
	while(pNode)
	{
		pTempNode = pNode;
		pNode = (KStateNode*) pNode->GetNext();
		pTempNode->Remove();
		delete pTempNode;
	}
	return;
}


void	KNpc::RestoreNpcBaseInfo()
{
	m_CurrentCamp = m_Camp;
	m_ActiveSkillID = 0;
	m_ActiveAuraID = 0;

	m_nPeopleIdx = 0;
	m_nLastDamageIdx = 0;
	m_nLastPoisonDamageIdx = 0;
	m_nObjectIdx = 0;

	m_CurrentLife			= m_LifeMax;
	m_CurrentLifeMax		= m_LifeMax;
	m_CurrentLifeReplenish	= m_LifeReplenish;
	m_CurrentLifeReplenishPercent = 0;
	m_CurrentMana			= m_ManaMax;
	m_CurrentManaMax		= m_ManaMax;
	m_CurrentManaReplenish	= m_ManaReplenish;
	m_CurrentStamina		= m_StaminaMax;
	m_CurrentStaminaMax		= m_StaminaMax;
	m_CurrentStaminaGain	= m_StaminaGain;
	m_CurrentStaminaLoss	= m_StaminaLoss;

	memset(&m_CurrentFireDamage, 0, sizeof(m_CurrentFireDamage));
	memset(&m_CurrentColdDamage, 0, sizeof(m_CurrentColdDamage));
	memset(&m_CurrentLightDamage, 0, sizeof(m_CurrentLightDamage));
	memset(&m_CurrentPoisonDamage, 0, sizeof(m_CurrentPoisonDamage));

	memset(&m_CurrentFireMagic, 0, sizeof(m_CurrentFireMagic));
	memset(&m_CurrentColdMagic, 0, sizeof(m_CurrentColdMagic));
	memset(&m_CurrentLightMagic, 0, sizeof(m_CurrentLightMagic));
	memset(&m_CurrentPoisonMagic, 0, sizeof(m_CurrentPoisonMagic));

	m_CurrentAttackRating	= m_AttackRating;
	m_CurrentDefend			= m_Defend;

	m_CurrentFireResist		= m_FireResist;
	m_CurrentColdResist		= m_ColdResist;
	m_CurrentPoisonResist	= m_PoisonResist;
	m_CurrentLightResist	= m_LightResist;
	m_CurrentPhysicsResist	= m_PhysicsResist;
	m_CurrentFireResistMax	= m_FireResistMax;
	m_CurrentColdResistMax	= m_ColdResistMax;
	m_CurrentPoisonResistMax = m_PoisonResistMax;
	m_CurrentLightResistMax	= m_LightResistMax;
	m_CurrentPhysicsResistMax  = m_PhysicsResistMax;

	m_CurrentWalkSpeed		= m_WalkSpeed;
	m_CurrentRunSpeed		= m_RunSpeed;
	m_CurrentAttackSpeed	= m_AttackSpeed;
	m_CurrentCastSpeed		= m_CastSpeed;
	m_CurrentVisionRadius	= m_VisionRadius;
	m_CurrentActiveRadius	= m_ActiveRadius;
	m_CurrentHitRecover		= m_HitRecover;

	m_CurrentDamage2Mana	= 0;
	m_CurrentLifeStolen		= 0;
	m_CurrentManaStolen		= 0;
	m_CurrentStaminaStolen	= 0;
	m_CurrentPiercePercent	= 0;
	m_CurrentFreezeTimeReducePercent	= 0;
	m_CurrentPoisonTimeReducePercent	= 0;
	m_CurrentStunTimeReducePercent		= 0;
	m_CurrentFireEnhance	= 0;
	m_CurrentColdEnhance	= 0;
	m_CurrentPoisonEnhance	= 0;
	m_CurrentLightEnhance	= 0;
	m_CurrentRangeEnhance	= 0;
	m_CurrentHandEnhance	= 0;
	m_CurrentDeadlyStrikeEnhanceP	= 0;
	m_CurrentFatallyStrikeEnhanceP	= 0;
	m_CurrentFatallyStrikeResP	= 0;
	m_CurrentManaShield = 0;
	m_CurrentStaticMagicShieldP = 0;
	m_CurrentLucky = 0;
	m_CurrentExpEnhance = 0;
	m_CurrentPoisonDamageReturnPercent = 0;
	m_CurrentReturnSkillPercent = 0;
	m_CurrentIgnoreSkillPercent = 0;
	ZeroMemory(m_CurrentMeleeEnhance, sizeof(m_CurrentMeleeEnhance));
	memset(&m_ReplySkill, 0, sizeof(m_ReplySkill));
	memset(&m_RescueSkill, 0, sizeof(m_RescueSkill));
	memset(&m_AttackSkill, 0, sizeof(m_AttackSkill));
	memset(&m_DeathSkill, 0, sizeof(m_DeathSkill));
	m_CurrentIgnoreNegativeStateP = 0;

	m_CurrentSkillEnhancePercent = 0;
	m_CurrentFiveElementsEnhance = 0;
	m_CurrentFiveElementsResist = 0;
	m_CurrentManaToSkillEnhanceP = 0;

	ClearStateSkillEffect();
	ClearNormalState();
}

#ifndef _SERVER
void KNpc::DrawBorder()
{
	if (m_Index <= 0)
		return;

	m_DataRes.DrawBorder();
}

int KNpc::DrawMenuState(int n)
{
	if (m_Index <= 0)
		return n;

	return m_DataRes.DrawMenuState(n);
}

void KNpc::DrawBlood()
{
	if (m_Kind != kind_normal)
		return;

	int nFontSize = 12;

	
	int nHeightOff = GetNpcPate();
	{
		nHeightOff = PaintLife(nHeightOff, true);
		nHeightOff += SHOW_SPACE_HEIGHT;
	}
	{
		nHeightOff = PaintInfo(nHeightOff, true);
	}
}
#endif

#ifdef _SERVER
int KNpc::SetPos(int nX, int nY)
{
	if (m_SubWorldIndex < 0)
	{
		_ASSERT(0);
		return 0;
	}
	int nRegion, nMapX, nMapY, nOffX, nOffY;
	SubWorld[m_SubWorldIndex].Mps2Map(nX, nY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);

	if (nRegion < 0)
	{
		g_DebugLog("[Script]SetPos error:SubWorld:%d, Pos(%d, %d)", SubWorld[m_SubWorldIndex].m_SubWorldID, nX, nY);
		return 0;
	}

	int nOldRegion = m_RegionIndex;
	if (m_RegionIndex >= 0)
	{
		SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].DecRef(m_MapX, m_MapY, obj_npc);
		NPC_REMOVE_SYNC	RemoveSync;
		RemoveSync.ProtocolType = s2c_npcremove;
		RemoveSync.ID = m_dwID;
		RemoveSync.Rv = FALSE;
		SendDataToNearRegion(&RemoveSync, sizeof(NPC_REMOVE_SYNC));
	}
	m_RegionIndex = nRegion;
	m_MapX = nMapX;
	m_MapY = nMapY;
	m_MapZ = 0;
	m_OffX = nOffX;
	m_OffY = nOffY;

	if (nOldRegion != nRegion)
	{
		SubWorld[m_SubWorldIndex].NpcChangeRegion(nOldRegion, nRegion, m_Index);
		if (IsPlayer())
			SubWorld[m_SubWorldIndex].PlayerChangeRegion(nOldRegion, nRegion, m_nPlayerIdx);
	}
	SubWorld[m_SubWorldIndex].m_Region[nRegion].AddRef(m_MapX, m_MapY, obj_npc);

	if (IsPlayer())
	{
		NPC_PLAYER_TYPE_NORMAL_SYNC	sSync;
		sSync.ProtocolType = s2c_npcsetpos;
		sSync.ID = m_dwID;
		sSync.MapX = nX;
		sSync.MapY = nY;
		sSync.OffX = m_OffX;
		sSync.OffY = m_OffY;
		g_pServer->PackDataToClient(Player[m_nPlayerIdx].m_nNetConnectIdx, (BYTE*)&sSync, sizeof(sSync));
	}
	DoStand();
	m_ProcessAI = 1;
	return 1;
}
#endif


#ifdef _SERVER
int KNpc::ChangeWorld(DWORD dwSubWorldID, int nX, int nY)
{
	int nTargetSubWorld = g_SubWorldSet.SearchWorld(dwSubWorldID);
	
	if (IsPlayer())
	{
		if (-1 == nTargetSubWorld)
		{
			TobeExchangeServer(dwSubWorldID, nX, nY);
			g_DebugLog("MapID %d haven't been loaded!", dwSubWorldID);
			return 0;
		}
	}

	if (IsPlayer())
		Player[m_nPlayerIdx].m_nPrePayMoney = 0;// ���ǿ�����������û�Ǯ
	// �л���������Ǳ���
	if (nTargetSubWorld == m_SubWorldIndex)
	{
		// ֻ���л�����
		return SetPos(nX, nY);
	}
	
	int nRegion, nMapX, nMapY, nOffX, nOffY;
	SubWorld[nTargetSubWorld].Mps2Map(nX, nY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
	// �л���������Ƿ�
	if (nRegion < 0)
	{
		g_DebugLog("[Map]Change Pos(%d,%d) Invalid!", nX, nY);
		return 0;
	}
	
	if (m_SubWorldIndex >= 0 && m_RegionIndex >= 0)
	{
		SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].RemoveNpc(m_Index);
		SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].DecRef(m_MapX, m_MapY, obj_npc);

		NPC_REMOVE_SYNC	RemoveSync;
		RemoveSync.ProtocolType = s2c_npcremove;
		RemoveSync.ID = m_dwID;
		RemoveSync.Rv = TRUE;
		SendDataToNearRegion(&RemoveSync, sizeof(NPC_REMOVE_SYNC));
	}

	int nSourceSubWorld = m_SubWorldIndex;
	int nSourceRegion = m_RegionIndex;

	m_SubWorldIndex = nTargetSubWorld;
	m_RegionIndex = nRegion;
	m_MapX = nMapX;
	m_MapY = nMapY;
	m_MapZ = 0;
	m_OffX = nOffX;
	m_OffY = nOffY;
	SubWorld[nTargetSubWorld].Map2Mps(nRegion, nMapX, nMapY, nOffX, nOffY, &m_OriginX, &m_OriginY);
	SubWorld[nTargetSubWorld].m_Region[nRegion].AddNpc(m_Index);
	SubWorld[nTargetSubWorld].m_Region[nRegion].AddRef(m_MapX, m_MapY, obj_npc);
	DoStand();
	m_ProcessAI = 1;

	if (IsPlayer())
	{
		SubWorld[nTargetSubWorld].SendSyncData(m_Index, Player[m_nPlayerIdx].m_nNetConnectIdx);
		SubWorld[nSourceSubWorld].RemovePlayer(nSourceRegion, m_nPlayerIdx);
		SubWorld[nTargetSubWorld].AddPlayer(nRegion, m_nPlayerIdx);

		TRADE_DECISION_COMMAND	sTrade;
		sTrade.ProtocolType = c2s_tradedecision;
		sTrade.m_btDecision = 0;
		sTrade.m_btFolkGame = 0;
		Player[m_nPlayerIdx].TradeDecision((BYTE*)&sTrade);
	}
	return 1;
}
#endif

#ifdef _SERVER
void KNpc::TobeExchangeServer(DWORD dwMapID, int nX, int nY)
{
	if (!IsPlayer())
	{
		return;
	}

	m_OldFightMode = m_FightMode;
	m_bExchangeServer = TRUE;
	if (m_nPlayerIdx > 0 && m_nPlayerIdx <= MAX_PLAYER)
	{
		Player[m_nPlayerIdx].TobeExchangeServer(dwMapID, nX, nY);
	}
}
#endif

BOOL KNpc::IsPlayer()
{
#ifdef _SERVER
	return m_Kind == kind_player;
#else
	return m_Index == Player[CLIENT_PLAYER_INDEX].m_nIndex;
#endif
}

// ���NPC���ϵķǱ�����ļ���״̬
void KNpc::ClearStateSkillEffect()
{
	KStateNode* pNode;
	pNode = (KStateNode *)m_StateSkillList.GetHead();
	while(pNode)
	{
		KStateNode* pTempNode = pNode;
		pNode = (KStateNode *)pNode->GetNext();

		if (pTempNode->m_bOverLook)	// ��������
			continue;

		if (pTempNode->m_LeftTime == -1)	// ��������
			continue;

		if (pTempNode->m_LeftTime > 0)
		{
			for (int i = 0; i < MAX_SKILL_STATE; i++)
			{
				if (pTempNode->m_State[i].nAttribType)
					ModifyAttrib(m_Index, &pTempNode->m_State[i]);
			}
			_ASSERT(pTempNode != NULL);
			pTempNode->Remove();
			delete pTempNode;
			pTempNode = NULL;
			continue;
		}
	}
#ifdef _SERVER
	UpdateNpcStateInfo();
#endif
}

void KNpc::IgnoreState(BOOL bNegative)
{
	KStateNode* pNode;
	pNode = (KStateNode *)m_StateSkillList.GetHead();
	while(pNode)
	{
		KStateNode* pTempNode = pNode;
		pNode = (KStateNode *)pNode->GetNext();

		if (pTempNode->m_bOverLook)	// ��������
			continue;

		if (pTempNode->m_LeftTime >= 0)
		{
			if(bNegative)
			{
				KSkill * pSkill = (KSkill *) g_SkillManager.GetSkill(pTempNode->m_SkillID, pTempNode->m_Level);
				if (!pSkill->IsTargetOnly() && !pSkill->IsTargetEnemy())
					continue;
			}

			int i = 0;
			for (i = 0; i < MAX_SKILL_STATE; i++)
			{
				if (pTempNode->m_State[i].nAttribType)
				{
					ModifyAttrib(m_Index, &pTempNode->m_State[i]);
				}
			}
			_ASSERT(pTempNode != NULL);
			pTempNode->Remove();
			delete pTempNode;

			pTempNode = NULL;

#ifdef _SERVER
			UpdateNpcStateInfo();
#endif
			continue;
		}
	}
#ifdef _SERVER
	IGNORE_STATE_SYNC	Sync;
	Sync.ProtocolType = s2c_ignorestate;
	Sync.bNegative = bNegative;
	g_pServer->PackDataToClient(Player[m_nPlayerIdx].m_nNetConnectIdx, &Sync, sizeof(IGNORE_STATE_SYNC));
#endif
}

void KNpc::ClearNormalState()
{
	ZeroMemory(&m_PhysicsArmor, sizeof(m_PhysicsArmor));
	ZeroMemory(&m_ColdArmor, sizeof(m_ColdArmor));
	ZeroMemory(&m_FireArmor, sizeof(m_FireArmor));
	ZeroMemory(&m_PoisonArmor, sizeof(m_PoisonArmor));
	ZeroMemory(&m_LightArmor, sizeof(m_LightArmor));
	ZeroMemory(&m_ManaShield, sizeof(m_ManaShield));
	ZeroMemory(&m_PoisonState, sizeof(m_PoisonState));
	ZeroMemory(&m_FreezeState, sizeof(m_FreezeState));
	ZeroMemory(&m_BurnState, sizeof(m_BurnState));
	ZeroMemory(&m_FrozenAction, sizeof(m_FrozenAction));
	ZeroMemory(&m_RandMove, sizeof(m_RandMove));
	ZeroMemory(&m_StunState, sizeof(m_StunState));
	ZeroMemory(&m_LifeState, sizeof(m_LifeState));
	ZeroMemory(&m_ManaState, sizeof(m_ManaState));
	ZeroMemory(&m_LoseMana, sizeof(m_LoseMana));
	ZeroMemory(&m_HideState, sizeof(m_HideState));
	ZeroMemory(&m_SilentState, sizeof(m_SilentState));
	ZeroMemory(&m_WalkRun, sizeof(m_WalkRun));
}

BOOL KNpc::IsNpcStateExist(int nId)
{
	if (nId <= 0)
		return FALSE;
	KStateNode* pNode;
	pNode = (KStateNode *)m_StateSkillList.GetHead();
	while(pNode)
	{
		if (pNode->m_SkillID == nId)
			return TRUE;

		pNode = (KStateNode *)pNode->GetNext();
	}
	return FALSE;
}

BOOL KNpc::IsNpcSkillExist(int nId)
{
	if (nId <= 0)
		return FALSE;
	for (int i = 1; i < MAX_NPCSKILL; i++)
	{
		if (m_SkillList.m_Skills[i].SkillId)
		{	
			if (m_SkillList.m_Skills[i].SkillId == nId)
				return TRUE;
		}
	}
	return FALSE;
}

BOOL KNpc::CheckTrap(int nMapX, int nMapY)
{
	if (m_Kind != kind_player)
		return FALSE;
	
	if (m_Index <= 0)
		return FALSE;

	if (m_SubWorldIndex < 0 || m_RegionIndex < 0)
		return FALSE;

	DWORD dwTrap = SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].GetTrap(nMapX, nMapY);

	if (m_TrapScriptID == dwTrap)
		return FALSE;
	else
		m_TrapScriptID = dwTrap;

	if (!m_TrapScriptID)
		return FALSE;
	Player[m_nPlayerIdx].ExecuteScript(m_TrapScriptID, NORMAL_FUNCTION_NAME, 0);
	return TRUE;
}

void KNpc::SetFightMode(BOOL bFightMode)
{
#ifdef _SERVER
	if (this->m_Kind == kind_player)
		Player[this->m_nPlayerIdx].m_cPK.CloseAll();
#endif

	m_FightMode = bFightMode;
}

void KNpc::TurnTo(int nIdx)
{
	if (!Npc[nIdx].m_Index || !m_Index)
		return;

	int nX1, nY1, nX2, nY2;

	GetMpsPos(&nX1, &nY1);
	Npc[nIdx].GetMpsPos(&nX2, &nY2);

	m_Dir = g_GetDirIndex(nX1, nY1, nX2, nY2);
}

void KNpc::ReCalcStateEffect()
{
	KStateNode* pNode;
	pNode = (KStateNode *)m_StateSkillList.GetHead();
	while(pNode)
	{
		if (pNode->m_LeftTime != 0)	// ��������(-1)������(>0)
		{
			int i = 0;
			for (i = 0; i < MAX_SKILL_STATE; i++)
			{
				if (pNode->m_State[i].nAttribType)
				{
					KMagicAttrib	MagicAttrib;
					MagicAttrib.nAttribType = pNode->m_State[i].nAttribType;
					MagicAttrib.nValue[0] = -pNode->m_State[i].nValue[0];
					MagicAttrib.nValue[1] = -pNode->m_State[i].nValue[1];
					MagicAttrib.nValue[2] = -pNode->m_State[i].nValue[2];
					ModifyAttrib(m_Index, &MagicAttrib);
				}
			}
		}
		pNode = (KStateNode *)pNode->GetNext();
	}
}

#ifndef _SERVER
extern KTabFile g_ClientWeaponSkillTabFile;
#endif

int		KNpc::GetCurActiveWeaponSkill()
{
	int nSkillId = 0;
	if (IsPlayer())
	{
		
		int nDetailType = Player[m_nPlayerIdx].m_ItemList.GetWeaponType();
		int nParticularType = Player[m_nPlayerIdx].m_ItemList.GetWeaponParticular();
		
		//��������
		if (nDetailType == 0)
		{
			nSkillId = g_nMeleeWeaponSkill[nParticularType];
		}//Զ������
		else if (nDetailType == 1)
		{
			nSkillId = g_nRangeWeaponSkill[nParticularType];
		}//����
		else if (nDetailType == -1)
		{
			nSkillId = g_nHandSkill;
		}
	}
	else
	{
#ifdef _SERVER
		//Real Npc
		return 0;
#else
		if (m_Kind == kind_player) // No Local Player
		{
			g_ClientWeaponSkillTabFile.GetInteger(m_WeaponType + 1, "SkillId", 0, &nSkillId);
		}
		else						//Real Npc
		{
			return 0;//
		}
#endif
	}
	return nSkillId;
}

#ifndef _SERVER
void KNpc::ProcNetCommand(NPCCMD cmd, int x /* = 0 */, int y /* = 0 */, int z /* = 0 */)
{
	switch (cmd)
	{
	case do_death:
		DoDeath();
		break;
	case do_hurt:
		DoHurt(x, y, z);
		break;
	case do_revive:
		DoStand();
		m_ProcessAI = 1;
		m_ProcessState = 1;
		SetInstantSpr(enumINSTANT_STATE_REVIVE);
		break;
	case do_stand:
		DoStand();
		m_ProcessAI = 1;
		m_ProcessState = 1;
	default:
		break;
	}
}
#endif

#ifndef _SERVER
void	KNpc::AddBlood(int nNo)
{
	if (nNo > 0)
	{
		m_nBlood[m_btCurBlood] = BLOOD_EVENTTIME;
		itoa(nNo, m_szBlood[m_btCurBlood], 10);

		m_btCurBlood++;
		if (m_btCurBlood >= BLOOD_COUNT)
			m_btCurBlood = 0;
	}
}

int	KNpc::PaintBlood(int nHeightOffset)
{
	int i = 0;
	while(i < BLOOD_COUNT)
	{
		if (m_szBlood[i][0]==0)
		{
			i++;
			continue;
		}

		int	nMpsX, nMpsY;
		GetMpsPos(&nMpsX, &nMpsY);

		g_pRepresent->OutputText(BLOOD_FONTSIZE, m_szBlood[i], KRF_ZERO_END, nMpsX - BLOOD_FONTSIZE * g_StrLen(m_szBlood[i]) / 4, nMpsY, 
			0x00ff0000, 0, nHeightOffset + (BLOOD_EVENTTIME - m_nBlood[i]) * BLOOD_MOVESPEED / 5, 0xff000000);
		
		m_nBlood[i]--;
		if (m_nBlood[i] <= 0)
		{
			m_szBlood[i][0]=0;
			break;
		}
		i++;
	}
	return nHeightOffset;
}

#endif

#ifndef _SERVER
int	KNpc::GetNpcPate()
{
	int nHeight = m_Height + m_nStature;
	if (m_Kind == kind_player)
	{
		if (m_nSex)
			nHeight += 84;	//Ů
		else
			nHeight += 84;	//��

		if (m_MaskType <= 0)
		{
			if (m_DataRes.IgnoreShowRes()==FALSE && m_Doing == do_sit && MulDiv(10, m_Frames.nCurrentFrame, m_Frames.nTotalFrame) >= 8)
				nHeight -= MulDiv(30, m_Frames.nCurrentFrame, m_Frames.nTotalFrame);
			
			if (m_bRideHorse)
				nHeight += 38;	//����
		}
		else
		{
			nHeight += 16;
		}
	}
	
	return nHeight;
}
#endif

#ifndef _SERVER
int	KNpc::GetNpcPatePeopleInfo()
{
	int nFontSize = 12;
	if (m_nChatContentLen > 0 && m_nChatNumLine > 0)
		return m_nChatNumLine * (nFontSize + 1);

	int nHeight = 0;
	if (NpcSet.CheckShowLife())
	{
		if (m_Kind == kind_player || m_Kind == kind_partner)
		{
			if (m_CurrentLifeMax > 0 && (relation_enemy == NpcSet.GetRelation(m_Index, Player[CLIENT_PLAYER_INDEX].m_nIndex)) &&
				Npc[m_Index].m_nPKFlag != enumPKMurder
				)
				nHeight += SHOW_LIFE_HEIGHT;
		}
	}
	if (NpcSet.CheckShowName())
	{
		if (nHeight != 0)
			nHeight += SHOW_SPACE_HEIGHT;//�ÿ�

		if (m_Kind == kind_player || m_Kind == kind_dialoger)
			nHeight += nFontSize + 1;
	}
	return nHeight;
}
#endif


void KNpc::SwitchRideHorse(BOOL bRideHorse)
{
	int nIdx = Player[m_nPlayerIdx].m_ItemList.GetEquipment(itempart_horse);
	if(nIdx <= 0)
		return;

	m_bRideHorse = bRideHorse;

	if(m_bRideHorse)
		Item[nIdx].ApplyMagicAttribToNPC(&Npc[m_Index], MAX_ITEM_MAGICATTRIB / 2);
	else
		Item[nIdx].RemoveMagicAttribFromNPC(&Npc[m_Index], MAX_ITEM_MAGICATTRIB / 2);

#ifdef _SERVER
	
	NPC_HORSE_SYNC	NetCommand;
	NetCommand.ProtocolType = BYTE(s2c_npchorsesync);
	NetCommand.m_dwID = m_dwID;
	NetCommand.m_bRideHorse = bRideHorse;
	if (m_RegionIndex < 0)
		return;
	int nMaxCount = MAX_BROADCAST_COUNT;
	POINT	POff[8] = 
	{
		{0, 32},
		{-16, 32},
		{-16, 0},
		{-16, -32},
		{0, -32},
		{16, -32},
		{16, 0},
		{16, 32},
	};
	CURREGION.BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX, m_MapY);
	int i = 0;
	for (i = 0; i < 8; i++)
	{
		if (CONREGIONIDX(i) == -1)
			continue;
		CONREGION(i).BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
	}
#endif
}

#ifdef _SERVER
#define		MAX_SWITCH_HORSE_FIGHT_ACTIVE		90
#define		MAX_SWITCH_HORSE_FIGHT_NONE			36
BOOL KNpc::CanSwitchRideHorse()
{
	if (Player[m_nPlayerIdx].CheckTrading())
		return FALSE;

	if (Player[m_nPlayerIdx].m_ItemList.GetEquipment(itempart_horse) <= 0)
		return FALSE;

	if (Npc[Player[m_nPlayerIdx].m_nIndex].m_Doing == do_sit)
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_HORSE_CANT_SWITCH2;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
		g_pServer->PackDataToClient(Player[m_nPlayerIdx].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		return FALSE;
	}
	if (m_dwNextSwitchHorseTime <= 0 || 
		(g_SubWorldSet.GetGameTime() >= m_dwNextSwitchHorseTime))
	{
		m_dwNextSwitchHorseTime = g_SubWorldSet.GetGameTime() + (m_FightMode ? MAX_SWITCH_HORSE_FIGHT_ACTIVE : MAX_SWITCH_HORSE_FIGHT_NONE);
		return TRUE;
	}
	else
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_HORSE_CANT_SWITCH1;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
		g_pServer->PackDataToClient(Player[m_nPlayerIdx].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		return FALSE;
	}
	return FALSE;
}
#endif

void	KNpc::ResetNpcTypeName(int nMark)
{
	
#ifndef _SERVER
	char	szNpcTypeName[32];
#endif
	if (nMark == 1)
	{
#ifndef _SERVER
		if (m_NpcSettingIdx == PLAYER_MALE_NPCTEMPLATEID)
		{
			strcpy(szNpcTypeName, "MainMan");
			m_StandFrame = NpcSet.GetPlayerStandFrame(TRUE);
			m_WalkFrame = NpcSet.GetPlayerWalkFrame(TRUE);
			m_RunFrame = NpcSet.GetPlayerRunFrame(TRUE);
		}
		else
		{
			strcpy(szNpcTypeName, "MainLady");
			m_StandFrame = NpcSet.GetPlayerStandFrame(FALSE);
			m_WalkFrame = NpcSet.GetPlayerWalkFrame(FALSE);
			m_RunFrame = NpcSet.GetPlayerRunFrame(FALSE);
		}
#endif
		m_WalkSpeed = NpcSet.GetPlayerWalkSpeed();
		m_RunSpeed = NpcSet.GetPlayerRunSpeed();
		m_AttackFrame = NpcSet.GetPlayerAttackFrame();
		m_HurtFrame	= NpcSet.GetPlayerHurtFrame();
	}
	else
	{
		GetNpcCopyFromTemplate(m_MaskType);
#ifndef _SERVER	
		g_NpcSetting.GetString(m_MaskType + 2, "NpcResType", "", szNpcTypeName, sizeof(szNpcTypeName));
		if (!szNpcTypeName[0])
		{
			g_NpcKindFile.GetString(2, "CharacterName", "", szNpcTypeName, sizeof(szNpcTypeName));//���û�ҵ����õ�һ��npc����
		}
#endif
	}
#ifndef _SERVER
	this->RemoveRes();
	m_DataRes._Init(szNpcTypeName, &g_NpcResList);

	m_DataRes.SetAction(m_ClientDoing);
	if(m_bRideHorse && m_ClientDoing == cdo_jump)
		m_DataRes.SetRideHorse(FALSE);
	else
		m_DataRes.SetRideHorse(m_bRideHorse);
	m_DataRes.SetArmor(m_ArmorType, m_MantleType);
	m_DataRes.SetHelm(m_HelmType);
	m_DataRes.SetHorse(m_HorseType);
	m_DataRes.SetWeapon(m_WeaponType);
#endif
}

void KNpc::SetRank(int nRank)
{	
	m_RankID = nRank;
#ifdef _SERVER
	POINT	POff[8] = 
	{
		{0, 32},
		{-16, 32},
		{-16, 0},
		{-16, -32},
		{0, -32},
		{16, -32},
		{16, 0},
		{16, 32},
	};
	TITLE_SYNC	NetCommand;
	NetCommand.ProtocolType = BYTE(s2c_titlename);
	NetCommand.ID = m_dwID;
	NetCommand.Rank = m_RankID;
	if (m_RegionIndex < 0)
		return;
	int nMaxCount = MAX_BROADCAST_COUNT;
	CURREGION.BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX, m_MapY);
	int i = 0;
	for (i = 0; i < 8; i++)
	{
		if (CONREGIONIDX(i) == -1)
			continue;
		CONREGION(i).BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
	}

#endif
}

void KNpc::SetExpandRank(KExpandRank* ExpandRank)
{
	int i = 0;
	KStateNode* pNode;
	// û����ѭ���з��أ�˵�����¼���
	BOOL bAddNewStateGraphics = TRUE;

	int nCurStateGraphics = m_CurExpandRank.nStateGraphics;

	m_CurExpandRank = *ExpandRank;
	if (m_CurExpandRank.dwLeftTime > KSG_GetCurSec())
		m_ExpandRank = *ExpandRank;

	pNode = (KStateNode *)m_StateSkillList.GetHead();
	while(pNode)
	{
		if (pNode->m_bTempStateGraphics && 
			(pNode->m_StateGraphics == nCurStateGraphics))
		{
			bAddNewStateGraphics = FALSE;
			pNode->m_StateGraphics = m_CurExpandRank.nStateGraphics;
		}
		pNode = (KStateNode *)pNode->GetNext();
	}

	if (bAddNewStateGraphics)
	{
		pNode = new KStateNode;
		pNode->m_SkillID = 0;
		pNode->m_Level = 0;
		pNode->m_LeftTime = -1;
		pNode->m_bOverLook = TRUE;
		pNode->m_bTempStateGraphics = TRUE;
		pNode->m_StateGraphics = m_CurExpandRank.nStateGraphics;
		m_StateSkillList.AddTail(pNode);
	}

#ifdef _SERVER
	POINT	POff[8] = 
	{
		{0, 32},
		{-16, 32},
		{-16, 0},
		{-16, -32},
		{0, -32},
		{16, -32},
		{16, 0},
		{16, 32},
	};
	EXPANDTITLE_SYNC	NetCommand;
	NetCommand.ProtocolType = BYTE(s2c_expandrank);
	NetCommand.ID = m_dwID;
	memcpy(&NetCommand.ExpandRank, &m_CurExpandRank, sizeof(m_CurExpandRank));
	if (m_RegionIndex < 0)
		return;

	int nMaxCount = MAX_BROADCAST_COUNT;
	CURREGION.BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX, m_MapY);

	for (i = 0; i < 8; i++)
	{
		if (CONREGIONIDX(i) == -1)
			continue;
		CONREGION(i).BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
	}
#endif

#ifdef _SERVER	
	UpdateNpcStateInfo();
#endif
}
