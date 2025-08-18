            // KSkills.cpp: implementation of the KSkills class.
//
//////////////////////////////////////////////////////////////////////
#include "KCore.h"
#ifdef _STANDALONE
#include "KSG_StringProcess.h"
#else
#include "../../Engine/Src/KSG_StringProcess.h"
#endif
#include "KSkills.h"
#include "KMissle.h"
#include "KMissleSet.h"
#include "KNpc.h"
#include "math.h"
#include "KNpcSet.h"
#include "KSubWorld.h"
#include "KMath.h"
#include "KEngine.h"
#include "KTabFile.h"
#include "KTabFileCtrl.h"
#include "KMissleMagicAttribsData.h"
#include "KPlayer.h"
#include "CoreShell.h"
#ifndef _SERVER
#include "../../Represent/iRepresent/iRepresentshell.h"
#include "scene/KScenePlaceC.h"
#include "../../Represent/iRepresent/KRepresentUnit.h"
#include "imgref.h"
#include "KMagicDesc.h"
#include "KOption.h"
#endif

#define	 NPCINDEXOFOBJECT 0 //Îï¼þ·¢Ä§·¨Ê±Ëù¶ÔÓ¦µÄNpc±àºÅ
const char * g_MagicID2String(int nAttrib);
extern  const KScript * g_GetScript(DWORD dwScriptId);
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/*!*****************************************************************************
// Function		: KSkill::KSkill
// Purpose		: 
// Return		: 
// Comments		:
// Author		: RomanDou
*****************************************************************************/
KSkill::KSkill()
{
	m_nFlySkillId =  m_nCollideSkillId = m_nVanishedSkillId = 0;
	
    // add by FreewayChen in 2003.6.6
    m_nImmediateAttribsNum = m_nStateAttribsNum = m_nMissleAttribsNum = m_nDamageAttribsNum = m_nAppendSkillNum =0;
	m_nSkillCostType = attrib_mana_v;
    m_nWaitTime = 0;
	m_nEquiptLimited = 0;
	m_nDoHurtP = 1;
	m_szMagicSkillDesc[0] = 0;
	m_nIsExpSkill = FALSE;
	m_bSkillReduceResist = FALSE;
	m_bSkillLifeReplenish = FALSE;
	memset(m_nAppendSkillId, 0, sizeof(m_nAppendSkillId));
#ifndef _SERVER
	m_szSkillDesc[0] = 0;
	m_szManPreCastSoundFile[0] = 0;
	m_szFMPreCastSoundFile[0] = 0;
#else
	m_dwSkillLevelUpScriptID = 0;
	m_dwSkillLevelDataScriptId = 0;
#endif
	
}
/*!*****************************************************************************
// Function		: KSkill::~KSkill
// Purpose		: 
// Return		: 
// Comments		:
// Author		: RomanDou
*****************************************************************************/
KSkill::~KSkill()
{
}

/*!*****************************************************************************
// Function		: KSkill::Param2PCoordinate
// Purpose		: 
// Return		: 
// Argumant		: int nLauncher
// Argumant		: int nParam1
// Argumant		: int nParam2
// Argumant		: int nParam3
// Argumant		: int *npPX
// Argumant		: int *npPY
// Comments		:
// Author		: RomanDou
*****************************************************************************/
inline int	KSkill::Param2PCoordinate(int nLauncher, int nParam1, int nParam2 , int *npPX, int *npPY, eSkillLauncherType eLauncherType)  const 
{
	
	int nRegionId, nDesMapX, nDesMapY ;
	int nTargetId = -1;
	if (eLauncherType == SKILL_SLT_Obj) return 0;
	
	switch(nParam1)
	{
	case -1://nParam2 ²ÎÊýÖ¸ÏòÄ³¸öNpc£¬»òObjµÄIndex
		nTargetId		= nParam2;
		nRegionId		= Npc[nParam2].m_RegionIndex;
		nDesMapX		= Npc[nParam2].m_MapX;
		nDesMapY		= Npc[nParam2].m_MapY;
		
		if (eLauncherType == SKILL_SLT_Npc)
			SubWorld[Npc[nLauncher].m_SubWorldIndex].Map2Mps(nRegionId, nDesMapX , nDesMapY, Npc[nParam2].m_OffX , Npc[nParam2].m_OffY, npPX, npPY);
		else if(eLauncherType == SKILL_SLT_Obj)
			SubWorld[Object[nLauncher].m_nSubWorldID].Map2Mps(nRegionId, nDesMapX, nDesMapY, Object[nParam2].m_nOffX , Object[nParam2].m_nOffY, npPX, npPY);
		else;
		break;
		
	case -2://nParam ²ÎÊýÖ¸ÏòÄ³¸ö·½Ïò
		
		break;
	default://Ä¬ÈÏÊ±, nParam1 ÓënParam2 ÎªÊµ¼Êµã×ø±ê
		*npPX = nParam1;
		*npPY = nParam2;
		break;
	}
	
	if (*npPX < 0 || *npPY < 0)	
		g_DebugLog("Param2PCoordinate error nParam1 ,nParam2 [%d,%d], nPX,nPY", nParam1, nParam2, *npPX, * npPY);
	
	return nTargetId;
}

int KSkill::CanCastSkill(int nLauncher, int &nParam1, int &nParam2)  const 
{
	//¶Ô×ÔÒÑµÄÖ÷¶¯¸¨Öú¼¼ÄÜ
	if (m_bTargetSelf && (nParam1 != -1)) 
	{
		nParam1 = -1;
		nParam2 = nLauncher;
		goto relationisvalid;
	}
	else
	{
		if (m_bTargetOnly && (nParam1 != -1)) return 0;
		
		if (nParam1 == -1)
		{
			if ( nParam2 <= 0 || nParam2 >= MAX_NPC) return 0;
			NPC_RELATION  Relation = NpcSet.GetRelation(nLauncher, nParam2);
			
			if (m_bTargetEnemy)
			{
				if (Relation & relation_enemy) goto relationisvalid;
			}
			
			if (m_bTargetAlly)
			{
				if (Relation & relation_ally) goto relationisvalid;
			}
			
			if (m_bTargetSelf)
			{
				if (Relation & relation_self) goto relationisvalid;
			}
			return 0;
		}
		
	}
	
relationisvalid:

	if (Npc[nLauncher].IsPlayer())
	{
		//-2±íÊ¾¼¼ÄÜ²»ÊÜµ±Ç°×°±¸µÄÏÞÖÆ,
		//-1±íÊ¾¿ÕÊÖÏÞÖÆ
		//0-99ÊÜÄ³ÖÖ½üÉí¹¥»÷ÀàµÄ×°±¸ÏÞÖÆ È¡ÖµÎª¸Ã×°±¸µÄ¾ßÌåÀà±ðºÅ
		//100-199ÊÜÄ³ÖÖÔ¶³Ì¹¥»÷ÀàµÄ×°±¸ÏÞÖÆ È¡ÖµÎª¸Ã×°±¸µÄ¾ßÌåÀà±ðºÅ ¼Ó100
		if (Npc[nLauncher].m_SilentState.nTime > 0)
		{
#ifndef _SERVER
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_NPC_NOT_USE_SKILL_SILENT);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
#endif
			return 0;
		}
		if (-2 != m_nEquiptLimited)
		{

#ifdef _SERVER
			int nPlayerIdx		= Npc[nLauncher].GetPlayerIdx();
#else
			int nPlayerIdx		= CLIENT_PLAYER_INDEX;
#endif
			int nDetailType		= Player[nPlayerIdx].m_ItemList.GetWeaponType();
			int nParticularType = Player[nPlayerIdx].m_ItemList.GetWeaponParticular();
			
			//½üÉíÎäÆ÷
			if (nDetailType == 0)
			{
				
			}//Ô¶³ÌÎäÆ÷
			else if (nDetailType == 1)
			{
				nParticularType += MAX_MELEEWEAPON_PARTICULARTYPE_NUM;
			}//¿ÕÊÖ
			else if (nDetailType == -1)
			{
				nParticularType = -1;
			}
			if (nParticularType == HAND_PARTICULAR)
				nParticularType = -1;

			if (nParticularType != m_nEquiptLimited)
			{
#ifndef _SERVER
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_NPC_NOT_USE_SKILL_WEAPON);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
#endif
			return 0;
			}
		}

		if (nParam1 == -1)
		{
			if ( nParam2 <= 0 || nParam2 >= MAX_NPC) return FALSE;
			if (Npc[nParam2].IsPlayer())
			{
				if (Npc[nLauncher].m_FightMode != Npc[nParam2].m_FightMode)
					return 0;
			}
		}		
		//0±íÊ¾²»ÏÞÖÆ
		//1±íÊ¾²»¿ÉÒÔÆïÂí·¢¸Ã¼¼ÄÜ
		//2±íÊ¾±ØÐëÆïÂí·¢¸Ã¼¼ÄÜ
		if (m_nHorseLimited)
		{
			switch(m_nHorseLimited)
			{
			case 1:
				{
					if (Npc[nLauncher].m_bRideHorse)
					{
#ifndef _SERVER
						KSystemMessage	sMsg;
						sprintf(sMsg.szMessage, MSG_NPC_NOT_USE_SKILL_HORSE1);
						sMsg.eType = SMT_NORMAL;
						sMsg.byConfirmType = SMCT_NONE;
						sMsg.byPriority = 0;
						sMsg.byParamSize = 0;
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
#endif
						return 0;
					}
				}
				break;
			case 2:
				{
					if (!Npc[nLauncher].m_bRideHorse)
					{
#ifndef _SERVER
						KSystemMessage	sMsg;
						sprintf(sMsg.szMessage, MSG_NPC_NOT_USE_SKILL_HORSE2);
						sMsg.eType = SMT_NORMAL;
						sMsg.byConfirmType = SMCT_NONE;
						sMsg.byPriority = 0;
						sMsg.byParamSize = 0;
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
#endif
						return 0;
					}
				}
				break;
			}
		}
		if(m_bTargetEnemy || m_bTargetAlly)
		{
			int distance = 0;
			if(nParam1 == -1)
			{
				distance = NpcSet.GetDistance(nLauncher, nParam2);
				if (distance > GetAttackRadius())
					return -1;
			}
			else
			{
				if(m_eMisslesForm == SKILL_MF_AtTarget/* || 
					m_eMisslesForm == SKILL_MF_AtFirer*/)
				{
					int nLauncherX, nLauncherY;
					Npc[nLauncher].GetMpsPos(&nLauncherX, &nLauncherY);
					distance = g_GetDistance(nLauncherX, nLauncherY, nParam1, nParam2);
					if (distance > GetAttackRadius())
					{
#ifndef _SERVER
						KSystemMessage	sMsg;
						sprintf(sMsg.szMessage, MSG_NPC_NOT_USE_SKILL_DISTANCE);
						sMsg.eType = SMT_NORMAL;
						sMsg.byConfirmType = SMCT_NONE;
						sMsg.byPriority = 0;
						sMsg.byParamSize = 0;
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
#endif
						return 0;
					}
				}
			}
		}
	}
	return 1;
}

//		µ±Íæ¼Òµ÷ÓÃÄ³¸ö¼¼ÄÜÊ±·¢Éú [5/28/2002]
//		¿Í»§¶ËºÍ·þÎñÆ÷¶ËÔÚ¼¼ÄÜµÄµ÷ÓÃ·½ÃæÓÐÒ»Ð©²»Í¬
//		·þÎñÆ÷¶ËÒ»°ãÊÕµ½µÄ¾­¹ý¿Í»§¶Ë´¦ÀíÍêµÄ²ÎÊý
//		ÓÎÏ·ÊÀ½çÒÔÏûÏ¢ÃüÁî»úÖÆÖ´ÐÐÃ¿¸ö±ä»¯£¬Òò´Ë¶ÔÓÚ·¢¼¼ÄÜÓ¦¸ÃÒ²ÊÇÍ³Ò»Êý¾Ý½Ó¿Ú
//		¿Í»§¶ËÊ±£¬Èç¹û¸ÃÏûÏ¢ÊÇÀ´×Ô±¾»úÍæ¼ÒµÄÊäÈë£¬Ôò±ØÐë½«Æä×ª»»ÎªÊµ¼ÊµÄÏûÏ¢
//		À´Ö´ÐÐ¡£Í¬Ê±»¹Ó¦½«×ª»»ºÃµÄÏûÏ¢´«¸ø·þÎñÆ÷¶Ë
/*
ÓÐ¹Ø´«µÄ²ÎÊýÊÇMapX£¬»¹ÊÇPointX¸ù¾Ý¾ßÌåµÄÄ§·¨¼¼ÄÜ¶ø¶¨
±ÈÈçÒ»°ã·ÉÐÐÄ§·¨ÎªMap×ø±ê£¬¶ø¶¨µãÄ§·¨ÎªPoint×ø±ê
*/
/*
×¢Òâµ±µ÷ÓÃCastÊ±£¬±ØÐëÒÑÈ·±£µ±Ç°µÄnLauncherIndexÓëSocketÏà¶ÔÓ¦µÄdwIdÒ»ÖÂ£¬¼´IsMatch()Í¨¹ý¡£
*/

/*!*****************************************************************************
// Function		: KSkill::Cast
// Purpose		: ·¢¼¼ÄÜµÄÍ³Ò»½Ó¿Ú
// Return		: 
// Argumant		: int nLauncher ·¢ËÍÕßId
// Argumant		: int nParam1   
// Argumant		: int nParam2
// Argumant		: int nWaitTime ·¢ËÍµÄÑÓ³ÙÊ±¼ä
// Argumant		: eSkillLauncherType eLauncherType ·¢ËÍÕßÀàÐÍ
// Comments		:
// Author		: RomanDou
*****************************************************************************/
BOOL	KSkill::Cast(int nLauncher, int nParam1, int nParam2, int nWaitTime, eSkillLauncherType eLauncherType)  const 
{
	//-----------------½Ó¿Úº¯ÊýÈë¿Úµã£¬¼ì²â²ÎÊýºÏ·¨ÐÔ-------------------------------
	if (nLauncher < 0 )
	{
		g_DebugLog("Skill::Cast(), nLauncher < 0 , Return False;"); 
		return FALSE; 
	}
	//¼ì²é·¢ËÍÕßÊÇ·ñ·ûºÏÒªÇó

	switch(eLauncherType)
	{
	case SKILL_SLT_Npc:
		{
			if (MAX_NPC <= nLauncher) return FALSE;
			if (Npc[nLauncher].m_dwID < 0) return FALSE;
			if (nParam1 == -1)
			{
				if (nParam2 >= MAX_NPC) 
					return FALSE;
				
				if (
					(Npc[nParam2].m_Index <= 0)
					|| Npc[nLauncher].m_SubWorldIndex != Npc[nParam2].m_SubWorldIndex
					)
					return FALSE;
			}
		}
		break;
		
	case SKILL_SLT_Obj:
		{
			return FALSE;
			if (MAX_OBJECT <= nLauncher) return FALSE;
			if (Object[nLauncher].m_nDataID < 0) return FALSE;
		}
		break;
	case SKILL_SLT_Missle:
		{
			if (MAX_MISSLE <= nLauncher) 
				return FALSE;
			
			if (Missle[nLauncher].m_nMissleId < 0) 
				return FALSE;
			
			if (nParam1 == -1)
			{
				if (nParam2 >= MAX_NPC) 
					return FALSE;
				
				if ((Npc[nParam2].m_Index <= 0) ||  Missle[nLauncher].m_nSubWorldId != Npc[nParam2].m_SubWorldIndex)
					return FALSE;
			}

		}
		break;
	default:
		{
			return FALSE;
		}
	}

	
	
	if (nParam1 < 0 && nParam2 < 0 ) 
		return FALSE;
	
	if (nWaitTime < 0 ) 
	{
		g_DebugLog("Call Skill::Cast(), nWaitTime < 0 "); 
		nWaitTime = 0;
	}
	
	//------------------------------------------------------------------------------
	
	switch(m_eSkillStyle)
	{
		
	case	SKILL_SS_Missles:				//·¢×Óµ¯
		{
			CastMissles(nLauncher, nParam1, nParam2, nWaitTime, eLauncherType);
		}
		break;
		
	case	SKILL_SS_Melee:
		{
			
			
		}break;
		
	case	SKILL_SS_InitiativeNpcState:	//¸Ä±ä½ÇÉ«µÄÖ÷¶¯×´Ì¬
		{
			CastInitiativeSkill(nLauncher, nParam1, nParam2, nWaitTime);
		}
		break;
		
	case	SKILL_SS_PassivityNpcState:	//¸Ä±ä½ÇÉ«µÄ±»¶¯×´Ì¬	
		{
			CastPassivitySkill(nLauncher, nParam1, nParam2, nWaitTime);
		}
		break;
		
	case	SKILL_SS_CreateNpc:			//²úÉúÐÂµÄNpc¡¢¹ÖÎï 
		{
			
		}
		break;
		
	case	SKILL_SS_BuildPoison:			//Á¶¶¾Êõ
		{
			
		}
		break;
		
	case	SKILL_SS_AddPoison:			//¼Ó¶¾Êõ
		{
			
		}
		break;
		
	case	SKILL_SS_GetObjDirectly:		//¸ô¿ÕÈ¡Îï	
		{
			
		}
		break;
		
	case	SKILL_SS_StrideObstacle:		//¿çÔ½ÕÏ°­
		{
			
		}
		break;
		
	case	SKILL_SS_BodyToObject:		//Ê¬±ä
		{
			
		}
		break;
		
	case	SKILL_SS_Mining:				//²É¿ó
		{
			
		}
		break;
		
	case	SKILL_SS_RepairWeapon:		//ÐÞ¸´Êõ
		{
			
		}
		break;
		
	case	SKILL_SS_Capture:				//²¶×½Êõ 
		{
			
		}
		break;
	}
//		printf("m_bStartEvent %d %d %d\n", m_bStartEvent, m_nStartSkillId, m_nEventSkillLevel);
	if (m_bStartEvent && m_nStartSkillId > 0 && m_nEventSkillLevel > 0)
	{
		KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(m_nStartSkillId, m_nEventSkillLevel);
		if (!pOrdinSkill) 
            return FALSE;
        pOrdinSkill->Cast(nLauncher, nParam1, nParam2, nWaitTime, eLauncherType);
	}
	return TRUE;	  
}

/*!*****************************************************************************
// Function		: KSkill::Vanish
// Purpose		: ×Óµ¯ÉúÃü½áÊøÊ±»Øµ÷
// Return		: 
// Argumant		: KMissle* Missle
// Comments		:
// Author		: RomanDou
*****************************************************************************/
void	KSkill::Vanish(KMissle * pMissle)  const 
{
	OnMissleEvent(Missle_VanishEvent, pMissle);
}

BOOL KSkill::OnMissleEvent(unsigned short usEvent, KMissle * pMissle)  const 
{
	if (!pMissle) 
        return FALSE;

	int nLauncherIdx = pMissle->m_nLauncher;
	
    if (
		pMissle->m_nMissleId <= 0 
		|| pMissle->m_nMissleId >= MAX_MISSLE 
		|| nLauncherIdx <= 0
		|| nLauncherIdx >= MAX_NPC
		|| Npc[nLauncherIdx].m_Index <= 0
		)
        return FALSE;

	
	if (
		(!Npc[nLauncherIdx].IsMatch(pMissle->m_dwLauncherId)) 
		|| Npc[nLauncherIdx].m_SubWorldIndex != pMissle->m_nSubWorldId
		|| Npc[nLauncherIdx].m_RegionIndex < 0
		)
	{
		return FALSE;
	}
	
	int nEventSkillId = 0;
	int nEventSkillLevel = 0;
	switch(usEvent)
	{
	case Missle_FlyEvent:
		if (!m_bFlyingEvent || m_nFlySkillId <= 0 || m_nEventSkillLevel <= 0)
			return FALSE;
		nEventSkillId = m_nFlySkillId ;
		nEventSkillLevel = m_nEventSkillLevel;
		break;
		
	case Missle_StartEvent:
		if (!m_bStartEvent || m_nStartSkillId <= 0 || m_nEventSkillLevel <= 0)
			return FALSE;
		nEventSkillId = m_nStartSkillId ;
		nEventSkillLevel = m_nEventSkillLevel;
		break;
		
	case Missle_VanishEvent:
		if (!m_bVanishedEvent || m_nVanishedSkillId <= 0 || m_nEventSkillLevel <= 0)
			return FALSE;
		nEventSkillId = m_nVanishedSkillId ;
		nEventSkillLevel = m_nEventSkillLevel;
		break;
		
	case Missle_CollideEvent:
		if (!m_bCollideEvent || m_nCollideSkillId <= 0 || m_nEventSkillLevel <= 0)
			return FALSE;
		nEventSkillId = m_nCollideSkillId;
		nEventSkillLevel = m_nEventSkillLevel;
		break;
	default:
		return FALSE;
	}
		
	int nDesPX = 0, nDesPY = 0;
	
	if (m_bByMissle)
	{
		pMissle->GetMpsPos(&nDesPX, &nDesPY);
	}
	else
	{
		Npc[nLauncherIdx].GetMpsPos(&nDesPX, &nDesPY);
	}
	
	KSkill * pOrdinSkill = (KSkill *)g_SkillManager.GetSkill(nEventSkillId, nEventSkillLevel);
	if (!pOrdinSkill) 
        return FALSE;
	
	BOOL bRetCode = FALSE;
	
    if (m_bByMissle)    //When Event
	{
		if (pOrdinSkill->GetSkillStyle() == SKILL_SS_Missles)
		{
			bRetCode = pOrdinSkill->CastMissles(pMissle->m_nMissleId, nDesPX, nDesPY, 0, SKILL_SLT_Missle);
		}
	}
	else
	{
		if (pOrdinSkill->GetSkillStyle() == SKILL_SS_Missles)
		{   
            bRetCode = pOrdinSkill->CastMissles(nLauncherIdx, nDesPX, nDesPY, 0, SKILL_SLT_Npc);
		}
	}
	
	return bRetCode;
}

/*!*****************************************************************************
// Function		: KSkill::FlyEvent
// Purpose		: 
// Return		: void 
// Argumant		: int nMissleId
// Comments		:
// Author		: RomanDou
*****************************************************************************/
void KSkill::FlyEvent(KMissle * pMissle)  const 
{
	OnMissleEvent(Missle_FlyEvent, pMissle);
}

/*!*****************************************************************************
// Function		: KSkill::Collidsion
// Purpose		: ×Óµ¯±»×²Ê±»Øµ÷
// Return		: 
// Argumant		: KMissle* Missle
// Comments		:
// Author		: RomanDou
*****************************************************************************/
void	KSkill::Collidsion(KMissle * pMissle)  const 
{
	OnMissleEvent(Missle_CollideEvent, pMissle);
}

/*!*****************************************************************************
// Function		: KSkill::CastMissles
// Purpose		: ·¢ËÍ×Óµ¯¼¼ÄÜ
// Return		: 
// Argumant		: int nLauncher  ·¢ËÍÕßid
// Argumant		: int nParam1
// Argumant		: int nParam2
// Argumant		: int nWaitTime  ÑÓ³¤Ê±¼ä
// Argumant		: eSkillLauncherType eLauncherType ·¢ËÍÕßÀàÐÍ
// Comments		:
// Author		: RomanDou
*****************************************************************************/
BOOL	KSkill::CastMissles(int nLauncher, int nParam1, int nParam2, int nWaitTime  , eSkillLauncherType eLauncherType )  const 
{
	int nRegionId		=	0;
	int	nDesMapX		=	0;//µØÍ¼×ø±ê
	int nDesMapY		=	0;
	int nDesOffX		=	0;
	int nDesOffY		=	0;
	int nSrcOffX		=	0;
	int nSrcOffY		=	0;
	int nSrcPX			=	0;//µã×ø±ê
	int nSrcPY			=	0;
	int nDesPX			=	0;
	int nDesPY			=	0;
	int nDistance		=	0;
	int nDir			=	0;
	int nDirIndex		=	0;
	int nTargetId		=	-1;
	int nRefPX			=	0;
	int nRefPY			=	0;
	TOrdinSkillParam	SkillParam ;
	SkillParam.eLauncherType = SKILL_SLT_Npc;
	SkillParam.nParent = 0;
	SkillParam.eParentType = (eSkillLauncherType)0;
	SkillParam.nWaitTime = nWaitTime;
	SkillParam.nTargetId = 0;
	if (nLauncher <= 0) return FALSE;

	switch(m_eMisslesForm)
	{
	/*
	»ðÇ½Ê±£¬µÚÒ»Êý×Ö²ÎÊý±íÊ¾×Óµ¯Ö®¼äµÄ³¤¶È¼ä¸ô
	X2  = X1 + N * SinA
	Y2  = Y2 - N * CosA
	*/
		
	case	SKILL_MF_Wall:			//Ç½ÐÎ	¶à¸ö×Óµ¯³Ê´¹Ö±·½ÏòÅÅÁÐ£¬ÀàÊ½»ðÇ½×´
		{
			//Ç½ÐÎÄ§·¨²»¿ÉÒÔÖ»´«·½Ïò
			if (nParam1 == SKILL_SPT_Direction) return FALSE;
			
			switch(eLauncherType)
			{
			case SKILL_SLT_Npc:
				{	
					nTargetId		= Param2PCoordinate(nLauncher,nParam1, nParam2, &nDesPX, &nDesPY,  SKILL_SLT_Npc);
					
					if (Npc[nLauncher].m_SubWorldIndex < 0) 
					{
						return FALSE;
					}
					
					SubWorld[Npc[nLauncher].m_SubWorldIndex].Map2Mps(Npc[nLauncher].m_RegionIndex, Npc[nLauncher].m_MapX, Npc[nLauncher].m_MapY, Npc[nLauncher].m_OffX, Npc[nLauncher].m_OffY, &nSrcPX, &nSrcPY);
					
					nDirIndex		= g_GetDirIndex(nSrcPX, nSrcPY, nDesPX, nDesPY);
					nDir			= g_DirIndex2Dir(nDirIndex, MaxMissleDir);
					nDir = nDir + MaxMissleDir / 4;
					if (nDir >= MaxMissleDir) nDir -= MaxMissleDir;
					SkillParam.nLauncher = nLauncher;
					SkillParam.eLauncherType = eLauncherType;
					SkillParam.nTargetId = nTargetId;
					if (m_nValue1 == 0 || m_nValue2 == 0)
						CastWall(&SkillParam, nDir, nDesPX, nDesPY);
					else
						CastWall(&SkillParam , nDir, nSrcPX, nSrcPY);
					
				}	break;
			case SKILL_SLT_Obj:
				{
				}break;
			case SKILL_SLT_Missle:
				{
					KMissle * pMissle = &Missle[nLauncher];
					if (!Npc[pMissle->m_nLauncher].IsMatch(pMissle->m_dwLauncherId)) return FALSE;
					
					SubWorld[Missle[nLauncher].m_nSubWorldId].Map2Mps(pMissle->m_nRegionId, pMissle->m_nCurrentMapX, pMissle->m_nCurrentMapY , pMissle->m_nXOffset, pMissle->m_nYOffset, &nRefPX, &nRefPY);
					int nDir = pMissle->m_nDir + MaxMissleDir / 4;
					if (nDir >= MaxMissleDir) nDir -= MaxMissleDir;
					SkillParam.nLauncher = pMissle->m_nLauncher;
					SkillParam.nParent = nLauncher;
					SkillParam.nParent = SKILL_SLT_Missle;
					SkillParam.nTargetId = pMissle->m_nFollowNpcIdx;
					CastWall(&SkillParam,  nDir, nRefPX, nRefPY);
				}break;
			}
		}break;
		
		
	case	SKILL_MF_Line:				//ÏßÐÎ	¶à¸ö×Óµ¯³ÊÆ½ÐÐÓÚÍæ¼Ò·½ÏòÅÅÁÐ
		{
			
			if (nParam1 == SKILL_SPT_Direction)
			{
				switch(eLauncherType)
				{
				case SKILL_SLT_Npc:
					{
						SubWorld[Npc[nLauncher].m_SubWorldIndex].Map2Mps(Npc[nLauncher].m_RegionIndex, Npc[nLauncher].m_MapX, Npc[nLauncher].m_MapY, Npc[nLauncher].m_OffX, Npc[nLauncher].m_OffY, &nSrcPX, &nSrcPY);
						if (nParam2 > MaxMissleDir || nParam2 < 0) return FALSE;
						nDir = nParam2;
						SkillParam.nLauncher = nLauncher;
						SkillParam.eLauncherType = eLauncherType;
						SkillParam.nTargetId = nTargetId;
						CastLine(&SkillParam, nDir, nSrcPX,nSrcPY);
						
					}break;
				case SKILL_SLT_Obj:
					{
						
					}break;
				case SKILL_SLT_Missle:
					{
						KMissle * pMissle = &Missle[nLauncher];
						if (nParam2 > MaxMissleDir || nParam2 < 0) return FALSE;
						if (!Npc[pMissle->m_nLauncher].IsMatch(pMissle->m_dwLauncherId)) return FALSE;
						nDir = nParam2;
						SubWorld[pMissle->m_nSubWorldId].Map2Mps(pMissle->m_nRegionId, pMissle->m_nCurrentMapX, pMissle->m_nCurrentMapY , pMissle->m_nXOffset, pMissle->m_nYOffset, &nRefPX, &nRefPY);
						SkillParam.nLauncher = pMissle->m_nLauncher;
						SkillParam.nParent = nLauncher;
						SkillParam.nTargetId = pMissle->m_nFollowNpcIdx;
						CastLine(&SkillParam, nDir,  nRefPX, nRefPY);
					}break;
				}
				
			}
			else
			{
				switch(eLauncherType)
				{
				case SKILL_SLT_Npc:
					{
						nTargetId		= Param2PCoordinate(nLauncher,nParam1, nParam2, &nDesPX, &nDesPY,  SKILL_SLT_Npc);
						SubWorld[Npc[nLauncher].m_SubWorldIndex].Map2Mps(Npc[nLauncher].m_RegionIndex, Npc[nLauncher].m_MapX, Npc[nLauncher].m_MapY, Npc[nLauncher].m_OffX, Npc[nLauncher].m_OffY, &nSrcPX, &nSrcPY);
						nDirIndex		= g_GetDirIndex(nSrcPX, nSrcPY, nDesPX, nDesPY);
						nDir			= g_DirIndex2Dir(nDirIndex, MaxMissleDir);
						SkillParam.nLauncher = nLauncher;
						SkillParam.eLauncherType = eLauncherType;
						SkillParam.nTargetId = nTargetId;
						if (m_nChildSkillNum == 1 && (g_MisslesLib[m_nChildSkillId].m_eMoveKind == MISSLE_MMK_Line || g_MisslesLib[m_nChildSkillId].m_eMoveKind == MISSLE_MMK_Parabola) ) 
						{
							if (nSrcPX == nDesPX && nSrcPY == nDesPY)		return FALSE ;
							nDistance = g_GetDistance(nSrcPX, nSrcPY, nDesPX, nDesPY);
							
							if (nDistance == 0 ) return FALSE;
							int		nYLength = nDesPY - nSrcPY;
							int		nXLength = nDesPX - nSrcPX;
							int		nSin = (nYLength << 10) / nDistance;	// ·Å´ó1024±¶
							int		nCos = (nXLength << 10) / nDistance;
							
							if (abs(nSin) > 1024) 
								return FALSE;

							if (abs(nCos) > 1024) 
								return FALSE;
							
							
							CastExtractiveLineMissle(&SkillParam, nDir, nSrcPX, nSrcPY, nCos, nSin, nDesPX, nDesPY);
						}
						else
							CastLine(&SkillParam, nDir, nSrcPX,nSrcPY);
					}break;
				case SKILL_SLT_Obj:
					{
					}break;
				case SKILL_SLT_Missle:
					{
						KMissle * pMissle = &Missle[nLauncher];
						if (!Npc[pMissle->m_nLauncher].IsMatch(pMissle->m_dwLauncherId)) return FALSE;
						SubWorld[pMissle->m_nSubWorldId].Map2Mps(pMissle->m_nRegionId, pMissle->m_nCurrentMapX, pMissle->m_nCurrentMapY , pMissle->m_nXOffset, pMissle->m_nYOffset, &nRefPX, &nRefPY);
						SkillParam.nLauncher = pMissle->m_nLauncher;
						SkillParam.nParent = nLauncher;
						SkillParam.eParentType = eLauncherType;
						SkillParam.nTargetId = pMissle->m_nFollowNpcIdx;
						CastLine(&SkillParam,  pMissle->m_nDir,  nRefPX, nRefPY);
					}break;
				}
			}
		}
		break;
		
		//  Êý×Ö²ÎÊýÒ»±íÊ¾×Óµ¯Ö®¼äµÄ½Ç¶È²î£¬ÒÔ64·½ÏòÎª×¼
		//  ´«À´µÄX/Y²ÎÊýÎª¸ñ×Ó×ø±ê
		
	case	SKILL_MF_Spread:				//É¢ÐÎ	¶à¸ö×Óµ¯³ÊÒ»¶¨µÄ½Ç¶ÈµÄ·¢É¢×´	
		{
			if (nParam1 == SKILL_SPT_Direction)
			{
				switch(eLauncherType)
				{
				case SKILL_SLT_Npc:
					{
						SubWorld[Npc[nLauncher].m_SubWorldIndex].Map2Mps(Npc[nLauncher].m_RegionIndex, Npc[nLauncher].m_MapX, Npc[nLauncher].m_MapY, Npc[nLauncher].m_OffX, Npc[nLauncher].m_OffY, &nSrcPX, &nSrcPY);
						if (nParam2 > MaxMissleDir || nParam2 < 0) return FALSE;
						nDir = nParam2;
						SkillParam.nLauncher = nLauncher;
						SkillParam.eLauncherType = eLauncherType;
						CastSpread(&SkillParam, nDir, nSrcPX,nSrcPY);
					}break;
				case SKILL_SLT_Obj:
					{
						
					}break;
				case SKILL_SLT_Missle:
					{
						KMissle * pMissle = &Missle[nLauncher];
						if (nParam2 > MaxMissleDir || nParam2 < 0) return FALSE;
						if (!Npc[pMissle->m_nLauncher].IsMatch(pMissle->m_dwLauncherId)) return FALSE;
						nDir = nParam2;
						SubWorld[pMissle->m_nSubWorldId].Map2Mps(pMissle->m_nRegionId, pMissle->m_nCurrentMapX, pMissle->m_nCurrentMapY , pMissle->m_nXOffset, pMissle->m_nYOffset, &nRefPX, &nRefPY);
						SkillParam.nLauncher = pMissle->m_nLauncher;
						SkillParam.nParent = nLauncher;
						SkillParam.eParentType = eLauncherType;
						SkillParam.nTargetId = pMissle->m_nFollowNpcIdx;
						CastSpread(&SkillParam, nDir,  nRefPX, nRefPY);
					}break;
				}
			}
			else
			{
				switch(eLauncherType)
				{
				case SKILL_SLT_Npc:
					{
						nTargetId		= Param2PCoordinate(nLauncher,nParam1, nParam2, &nDesPX, &nDesPY, SKILL_SLT_Npc);		
						SubWorld[Npc[nLauncher].m_SubWorldIndex].Map2Mps(Npc[nLauncher].m_RegionIndex, Npc[nLauncher].m_MapX, Npc[nLauncher].m_MapY, Npc[nLauncher].m_OffX, Npc[nLauncher].m_OffY, &nSrcPX, &nSrcPY);
						nDirIndex		= g_GetDirIndex(nSrcPX, nSrcPY, nDesPX, nDesPY);
						nDir			= g_DirIndex2Dir(nDirIndex, MaxMissleDir);
						SkillParam.nLauncher = nLauncher;
						SkillParam.eLauncherType = eLauncherType;
						SkillParam.nTargetId = nTargetId;
						
						if (m_nChildSkillNum == 1 && (g_MisslesLib[m_nChildSkillId].m_eMoveKind == MISSLE_MMK_Line) ) 
						{
							if (nSrcPX == nDesPX && nSrcPY == nDesPY)		return FALSE ;
							nDistance = g_GetDistance(nSrcPX, nSrcPY, nDesPX, nDesPY);
							
							if (nDistance == 0 ) return FALSE;
							int		nYLength = nDesPY - nSrcPY;
							int		nXLength = nDesPX - nSrcPX;
							int		nSin = (nYLength << 10) / nDistance;	// ·Å´ó1024±¶
							int		nCos = (nXLength << 10) / nDistance;

							if (abs(nSin) > 1024) 
								return FALSE;
							
							if (abs(nCos) > 1024) 
								return FALSE;

							CastExtractiveLineMissle(&SkillParam, nDir, nSrcPX, nSrcPY, nCos, nSin, nDesPX, nDesPY);
						}
						else
							CastSpread(&SkillParam, nDir, nSrcPX, nSrcPY);
					}break;
				case SKILL_SLT_Obj:
					{
						
					}break;
				case SKILL_SLT_Missle:
					{
						KMissle * pMissle = &Missle[nLauncher];
						if (!Npc[pMissle->m_nLauncher].IsMatch(pMissle->m_dwLauncherId)) return FALSE;
						SubWorld[pMissle->m_nSubWorldId].Map2Mps(pMissle->m_nRegionId, pMissle->m_nCurrentMapX, pMissle->m_nCurrentMapY , pMissle->m_nXOffset, pMissle->m_nYOffset, &nRefPX, &nRefPY);
						SkillParam.nLauncher = pMissle->m_nLauncher;
						SkillParam.nParent = nLauncher;
						SkillParam.eParentType = eLauncherType;
						SkillParam.nTargetId = pMissle->m_nFollowNpcIdx;
						CastSpread(&SkillParam ,pMissle->m_nDir,  nRefPX, nRefPY);
					}break;
				}
			}
			
		}break;
		
		
		//ÒÔµ±Ç°µãÎªÔ²µã²úÉú¶à¸öÎ§ÈÅµÄ×Óµ¯
		//·Ö³ÉÁ½ÖÖÇé¿ö£¬Ò»ÖÖÎªÒÔÔ­µØÎªÔ­ÐÄ·¢³ö£¬ÁíÒ»ÖÖÎªÒÔÄ¿±êµãÎªÔ­ÐÄ·¢³ö
		// Êý×Ö²ÎÊýÒ»±íÊ¾ ÊÇ·ñÎªÔ­µØ·¢³ö
		
	case	SKILL_MF_Circle:				//Ô²ÐÎ	¶à¸ö×Óµ¯Î§³ÉÒ»¸öÈ¦
		{
			if (nParam1 == SKILL_SPT_Direction) return FALSE;
			
			switch(eLauncherType)
			{
			case SKILL_SLT_Npc:
				{
					nTargetId		= Param2PCoordinate(nLauncher,nParam1, nParam2,  &nDesPX, &nDesPY, eLauncherType);
					SubWorld[Npc[nLauncher].m_SubWorldIndex].Map2Mps(Npc[nLauncher].m_RegionIndex, Npc[nLauncher].m_MapX, Npc[nLauncher].m_MapY, Npc[nLauncher].m_OffX, Npc[nLauncher].m_OffY, &nSrcPX, &nSrcPY);
					nDirIndex		= g_GetDirIndex(nSrcPX, nSrcPY, nDesPX, nDesPY);
					nDir			= g_DirIndex2Dir(nDirIndex, MaxMissleDir);
					SkillParam.nLauncher = nLauncher;
					SkillParam.eLauncherType = eLauncherType;
					SkillParam.nTargetId = nTargetId;
					
					if (m_nValue1 == 0)
						CastCircle(&SkillParam, nDir, nSrcPX, nSrcPY);
					else
						CastCircle(&SkillParam, nDir, nDesPX, nDesPY);
				}break;
			case SKILL_SLT_Obj:
				{
					
				}break;
			case SKILL_SLT_Missle:
				{
					KMissle * pMissle = &Missle[nLauncher];
					if (!Npc[pMissle->m_nLauncher].IsMatch(pMissle->m_dwLauncherId)) return FALSE;
					SubWorld[pMissle->m_nSubWorldId].Map2Mps(pMissle->m_nRegionId, pMissle->m_nCurrentMapX, pMissle->m_nCurrentMapY , pMissle->m_nXOffset, pMissle->m_nYOffset, &nRefPX, &nRefPY);
					SkillParam.nLauncher = pMissle->m_nLauncher;
					SkillParam.nParent = nLauncher;
					SkillParam.eParentType = eLauncherType;
					SkillParam.nTargetId = pMissle->m_nFollowNpcIdx;
					CastCircle(&SkillParam, pMissle->m_nDir,  nRefPX, nRefPY);
				}break;
			}
			
		}break;
		
	case	SKILL_MF_Random:				//Ëæ»ú	¶à¸ö×Óµ¯Ëæ»úÅÅ·Å
		{
			switch(eLauncherType)
			{
			case SKILL_SLT_Npc:
				{
					
				}break;
			case SKILL_SLT_Obj:
				{
					
				}break;
			case SKILL_SLT_Missle:
				{
					
				}break;
			}
		}
		break;
		
	case	SKILL_MF_AtTarget:				//¶¨µã	¶à¸ö×Óµ¯¸ù¾Ý
		{
			if (nParam1 == SKILL_SPT_Direction) return FALSE;	
			
			switch(eLauncherType)
			{
			case SKILL_SLT_Npc:
				{
					nTargetId		= Param2PCoordinate(nLauncher,nParam1, nParam2, &nDesPX, &nDesPY);
					nDirIndex		= g_GetDirIndex(nSrcPX, nSrcPY, nDesPX, nDesPY);
					nDir			= g_DirIndex2Dir(nDirIndex, MaxMissleDir);
					SkillParam.nLauncher = nLauncher;
					SkillParam.eLauncherType = eLauncherType;
					SkillParam.nTargetId = nTargetId;
					CastZone(&SkillParam, nDir, nDesPX, nDesPY);
				}break;
			case SKILL_SLT_Obj:
				{
					
				}break;
			case SKILL_SLT_Missle:
				{
					KMissle * pMissle = &Missle[nLauncher];
					if (!Npc[pMissle->m_nLauncher].IsMatch(pMissle->m_dwLauncherId)) return FALSE;
					SubWorld[pMissle->m_nSubWorldId].Map2Mps(pMissle->m_nRegionId, pMissle->m_nCurrentMapX, pMissle->m_nCurrentMapY , pMissle->m_nXOffset, pMissle->m_nYOffset, &nRefPX, &nRefPY);
					SkillParam.nLauncher = pMissle->m_nLauncher;
					SkillParam.nParent = nLauncher;
					SkillParam.eParentType = eLauncherType;
					SkillParam.nTargetId = pMissle->m_nFollowNpcIdx;
					CastZone(&SkillParam, pMissle->m_nDir, nRefPX, nRefPY);
				}break;
			}
		}break;
		
	case	SKILL_MF_AtFirer:				//±¾Éí	¶à¸ö×Óµ¯Í£ÔÚÍæ¼Òµ±Ç°Î»ÖÃ
		{
			if (nParam1 == SKILL_SPT_Direction) return FALSE;
			switch(eLauncherType)
			{
			case SKILL_SLT_Npc:
				{
					SubWorld[Npc[nLauncher].m_SubWorldIndex].Map2Mps(Npc[nLauncher].m_RegionIndex, Npc[nLauncher].m_MapX, Npc[nLauncher].m_MapY, Npc[nLauncher].m_OffX, Npc[nLauncher].m_OffY, &nSrcPX, &nSrcPY);
					nDirIndex		= g_GetDirIndex(nSrcPX, nSrcPY, nDesPX, nDesPY);
					nDir			= g_DirIndex2Dir(nDirIndex, MaxMissleDir);
					SkillParam.nLauncher = nLauncher;
					SkillParam.eLauncherType = eLauncherType;
					SkillParam.nTargetId = nTargetId;
					CastZone(&SkillParam,  nDir, nSrcPX, nSrcPY);
				}break;
			case SKILL_SLT_Obj:
				{
					
				}break;
			case SKILL_SLT_Missle:
				{
					KMissle * pMissle = &Missle[nLauncher];
					if (!Npc[pMissle->m_nLauncher].IsMatch(pMissle->m_dwLauncherId)) return FALSE;
					SubWorld[pMissle->m_nSubWorldId].Map2Mps(pMissle->m_nRegionId, pMissle->m_nCurrentMapX, pMissle->m_nCurrentMapY , pMissle->m_nXOffset, pMissle->m_nYOffset, &nRefPX, &nRefPY);
					SkillParam.nLauncher = pMissle->m_nLauncher;
					SkillParam.nParent = nLauncher;
					SkillParam.eParentType = eLauncherType;
					SkillParam.nTargetId = pMissle->m_nFollowNpcIdx;
					CastZone(&SkillParam , pMissle->m_nDir, nRefPX, nRefPY);
				}break;
			}
			
		}break;
		
	case	SKILL_MF_Zone:
		{
			if (nParam1 == SKILL_SPT_Direction) return FALSE;
			
			switch(eLauncherType)
			{
			case SKILL_SLT_Npc:
				{
					nTargetId		= Param2PCoordinate(nLauncher,nParam1, nParam2,  &nDesPX, &nDesPY);
					SubWorld[Npc[nLauncher].m_SubWorldIndex].Map2Mps(Npc[nLauncher].m_RegionIndex, Npc[nLauncher].m_MapX, Npc[nLauncher].m_MapY, Npc[nLauncher].m_OffX, Npc[nLauncher].m_OffY, &nSrcPX, &nSrcPY);
					nDirIndex		= g_GetDirIndex(nSrcPX, nSrcPY, nDesPX, nDesPY);
					nDir			= g_DirIndex2Dir(nDirIndex, MaxMissleDir);
					SkillParam.nLauncher = nLauncher;
					SkillParam.eLauncherType = eLauncherType;
					SkillParam.nTargetId = nTargetId;
					CastZone(&SkillParam, nDir, nSrcPX, nSrcPY);
				}break;
			case SKILL_SLT_Obj:
				{
					
				}break;
			case SKILL_SLT_Missle:
				{
					KMissle * pMissle = &Missle[nLauncher];
					if (!Npc[pMissle->m_nLauncher].IsMatch(pMissle->m_dwLauncherId)) return FALSE;
					SubWorld[pMissle->m_nSubWorldId].Map2Mps(pMissle->m_nRegionId, pMissle->m_nCurrentMapX, pMissle->m_nCurrentMapY , pMissle->m_nXOffset, pMissle->m_nYOffset, &nRefPX, &nRefPY);
					SkillParam.nLauncher = pMissle->m_nLauncher;
					SkillParam.nParent = nLauncher;
					SkillParam.eParentType = eLauncherType;
					SkillParam.nTargetId = pMissle->m_nFollowNpcIdx;
					CastZone(&SkillParam, pMissle->m_nDir, nRefPX, nRefPY);
				}break;
			}
		}break;
	}
	return TRUE;
}

/*!*****************************************************************************
// Function		: KSkill::CastZone
// Purpose		: 
// Return		: int 
// Argumant		: int nLauncher
// Argumant		: eSkillLauncherType eLauncherType
// Argumant		: int nDir
// Argumant		: int nRefPX
// Argumant		: int nRefPY
// Argumant		: int nWaitTime
// Argumant		: int nTargetId
// Comments		:
// Author		: RomanDou
*****************************************************************************/
//nValue1 = 0 ±íÊ¾¾ØÐÎÇøÓò  nValue1 = 1 ±íÊ¾Ô²ÐÎÇøÓò
//nValue2 = 0 
int KSkill::CastZone(TOrdinSkillParam * pSkillParam , int nDir, int nRefPX, int nRefPY)  const 
{
	int nLauncher = pSkillParam->nLauncher;
	eSkillLauncherType eLauncherType = pSkillParam->eLauncherType;
	
	if (eLauncherType != SKILL_SLT_Npc) return 0;
	int nCastMissleNum	= 0;
	int nBeginPX ;
	int nBeginPY ;
	if (m_nChildSkillNum == 1)
	{
		nBeginPX = nRefPX;
		nBeginPY = nRefPY;
	}
	else 
	{
		nBeginPX		= nRefPX - m_nChildSkillNum * SubWorld[Npc[nLauncher].m_SubWorldIndex].m_nCellWidth / 2;
		nBeginPY		= nRefPY - m_nChildSkillNum * SubWorld[Npc[nLauncher].m_SubWorldIndex].m_nCellHeight / 2;
	}
	
#ifdef _SERVER
	KMissleMagicAttribsData * pNewMagicAttribsData = CreateMissleMagicAttribsData(nLauncher);
#endif //_SERVER
	
	for (int i = 0; i < m_nChildSkillNum; i ++)
		for (int j = 0; j < m_nChildSkillNum; j ++)
		{
			if (m_bBaseSkill)
			{
				int nMissleIndex ;
				int nSubWorldId ; 
				
				nSubWorldId = Npc[nLauncher].m_SubWorldIndex;
				
				if (m_nValue1 == 1)
					if ( ((i - m_nChildSkillNum / 2) * (i - m_nChildSkillNum / 2) + (j - m_nChildSkillNum / 2) * (j - m_nChildSkillNum / 2)) > (m_nChildSkillNum * m_nChildSkillNum / 4))			continue;
					
					
					if (nSubWorldId < 0)	goto exit;
					int nDesSubX = nBeginPX + j * SubWorld[nSubWorldId].m_nCellWidth;
					int nDesSubY = nBeginPY +  i * SubWorld[nSubWorldId].m_nCellHeight;
					nMissleIndex = MissleSet.Add(nSubWorldId, nDesSubX , nDesSubY);
					
					if (nMissleIndex < 0)	continue;
					
					Missle[nMissleIndex].m_nDir				= nDir;
					Missle[nMissleIndex].m_nDirIndex		= g_Dir2DirIndex(nDir, MaxMissleDir);
					CreateMissle(nLauncher, m_nChildSkillId, nMissleIndex);
					Missle[nMissleIndex].m_nFollowNpcIdx	= pSkillParam->nTargetId;
					if (pSkillParam->nTargetId)
						Missle[nMissleIndex].m_dwFollowNpcID	= Npc[pSkillParam->nTargetId].m_dwID;
					Missle[nMissleIndex].m_dwBornTime		= SubWorld[nSubWorldId].m_dwCurrentTime;
					Missle[nMissleIndex].m_nSubWorldId		= nSubWorldId;
					Missle[nMissleIndex].m_nLauncher		= nLauncher;
					Missle[nMissleIndex].m_dwLauncherId		= Npc[nLauncher].m_dwID;
					Missle[nMissleIndex].m_nPKFlag			= Npc[nLauncher].m_nPKFlag;
					Missle[nMissleIndex].m_nMissleSeries	= -1;
					KSkill * pSkill =(KSkill*) Npc[nLauncher].GetActiveSkill();
					if(pSkill)
						Missle[nMissleIndex].m_nMissleSeries	= pSkill->GetSkillSeries();
					
					if (pSkillParam->nParent)
						Missle[nMissleIndex].m_nParentMissleIndex = pSkillParam->nParent;
					else 
						Missle[nMissleIndex].m_nParentMissleIndex = 0;
					
					Missle[nMissleIndex].m_nSkillId			= m_nId;
					Missle[nMissleIndex].m_nStartLifeTime	= pSkillParam->nWaitTime + GetMissleGenerateTime(i * m_nChildSkillNum + j);
					Missle[nMissleIndex].m_nLifeTime		+= Missle[nMissleIndex].m_nStartLifeTime;
					Missle[nMissleIndex].m_nRefPX			= nDesSubX;
					Missle[nMissleIndex].m_nRefPY			= nDesSubY;
					
					if (Missle[nMissleIndex].m_eMoveKind == MISSLE_MMK_Line|| Missle[nMissleIndex].m_eMoveKind == MISSLE_MMK_RollBack || Missle[nMissleIndex].m_eMoveKind == MISSLE_MMK_Follow)
					{
						Missle[nMissleIndex].m_nXFactor = g_DirCos(nDir, MaxMissleDir);
						Missle[nMissleIndex].m_nYFactor = g_DirSin(nDir, MaxMissleDir);
					}
#ifdef _SERVER
					Missle[nMissleIndex].SetMagicAttribsData(pNewMagicAttribsData);
#endif //_SERVER
					nCastMissleNum ++;
			}
			else
			{
				_ASSERT(m_nChildSkillId > 0 && m_nChildSkillLevel > 0)	;
				KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(m_nChildSkillId, m_nChildSkillLevel);
				if (pOrdinSkill) 
				{
					if (!pSkillParam->nParent)
						nCastMissleNum += pOrdinSkill->Cast(nLauncher, nBeginPX + j * SubWorld[Npc[nLauncher].m_SubWorldIndex].m_nCellWidth , nBeginPY +  i * SubWorld[Npc[nLauncher].m_SubWorldIndex].m_nCellHeight, pSkillParam->nWaitTime + GetMissleGenerateTime(i * m_nChildSkillNum + j ), eLauncherType);
					else 
						nCastMissleNum += pOrdinSkill->Cast(pSkillParam->nLauncher, nBeginPX + j * SubWorld[Npc[nLauncher].m_SubWorldIndex].m_nCellWidth , nBeginPY +  i * SubWorld[Npc[nLauncher].m_SubWorldIndex].m_nCellHeight, pSkillParam->nWaitTime + GetMissleGenerateTime(i * m_nChildSkillNum + j ), pSkillParam->eLauncherType);
				}
			}
			
		}
exit:	
#ifdef _SERVER
		if (pNewMagicAttribsData)
			if (pNewMagicAttribsData->GetRef() == 0)
				delete pNewMagicAttribsData;
#endif
			return nCastMissleNum;
}

/*!*****************************************************************************
// Function		: KSkill::CastLine
// Purpose		: 
// Return		: 
// Argumant		: int nLauncher
// Argumant		: eSkillLauncherType eLauncherType
// Argumant		: int nDir
// Argumant		: int nRefPX
// Argumant		: int nRefPY
// Argumant		: int nWaitTime
// Argumant		: int nTargetId
// Comments		:
// Author		: RomanDou
*****************************************************************************/
// Value1 ×Óµ¯Ö®¼äµÄ¼ä¾à
// Value2 
int		KSkill::CastLine(TOrdinSkillParam *pSkillParam, int nDir, int nRefPX, int nRefPY)  const 
{
	int nLauncher = pSkillParam->nLauncher;
	eSkillLauncherType eLauncherType = pSkillParam->eLauncherType;
	if (eLauncherType != SKILL_SLT_Npc) return 0;
	int	nDirIndex		= g_Dir2DirIndex(nDir, MaxMissleDir);
	int nDesSubX		= 0;
	int nDesSubY		= 0;
	int nCastMissleNum	= 0;
	
	//×Óµ¯Ö®¼äµÄ¼ä¾à
	int nMSDistanceEach = m_nValue1;
	
#ifdef _SERVER
	KMissleMagicAttribsData * pNewMagicAttribsData = CreateMissleMagicAttribsData(nLauncher);
#endif //_SERVER
	
	int nNum = 0;
	//·Ö±ðÉú³É¶àÉÙ×Óµ¯
	for(int i = 0; i < m_nChildSkillNum; i++)
	{
		if (m_nValue2)
		{
			int nCurMSDistance	= -1 * nMSDistanceEach * m_nChildSkillNum / 2;
			int nDir1 = nDirIndex + MaxMissleDir / 4;
			if (nDir1 > MaxMissleDir)
				nDir1 -= MaxMissleDir;
			int nDIndex = g_Dir2DirIndex(nDir1,MaxMissleDir);
			if (i % 2)
			{
				nCurMSDistance = -nCurMSDistance;
				nNum++;
			}
			
			nDesSubX	= nRefPX + ((nCurMSDistance * nNum * g_DirCos(nDIndex, MaxMissleDir) )>>10);
			nDesSubY	= nRefPY + ((nCurMSDistance * nNum * g_DirSin(nDIndex, MaxMissleDir) )>>10);
		}
		else
		{
			nDesSubX	= nRefPX + ((nMSDistanceEach * (i + 1) * g_DirCos(nDirIndex, MaxMissleDir) )>>10);
			nDesSubY	= nRefPY + ((nMSDistanceEach * (i + 1) * g_DirSin(nDirIndex, MaxMissleDir) )>>10);
		}
		if (nDesSubX < 0 || nDesSubY < 0) 	continue;
		
		if (m_bBaseSkill)
		{
			int nMissleIndex ;
			int nSubWorldId ; 
			nSubWorldId = Npc[nLauncher].m_SubWorldIndex;
			
			if (nSubWorldId < 0)	goto exit;
			nMissleIndex = MissleSet.Add(nSubWorldId, nDesSubX, nDesSubY);
			
			if (nMissleIndex < 0)	continue;
			
			Missle[nMissleIndex].m_nDir				= nDir;
			Missle[nMissleIndex].m_nDirIndex		= nDirIndex;
			CreateMissle(nLauncher, m_nChildSkillId, nMissleIndex);
			Missle[nMissleIndex].m_nFollowNpcIdx	= pSkillParam->nTargetId;
			if (pSkillParam->nTargetId)
				Missle[nMissleIndex].m_dwFollowNpcID	= Npc[pSkillParam->nTargetId].m_dwID;
			Missle[nMissleIndex].m_dwBornTime		= SubWorld[nSubWorldId].m_dwCurrentTime;
			Missle[nMissleIndex].m_nSubWorldId		= nSubWorldId;
			Missle[nMissleIndex].m_nLauncher		= nLauncher;
			Missle[nMissleIndex].m_dwLauncherId		= Npc[nLauncher].m_dwID;
			Missle[nMissleIndex].m_nPKFlag			= Npc[nLauncher].m_nPKFlag;
			Missle[nMissleIndex].m_nMissleSeries	= -1;
			KSkill * pSkill =(KSkill*) Npc[nLauncher].GetActiveSkill();
			if(pSkill)
				Missle[nMissleIndex].m_nMissleSeries	= pSkill->GetSkillSeries();
			
			if (pSkillParam->nParent)
				Missle[nMissleIndex].m_nParentMissleIndex = pSkillParam->nParent;
			else 
				Missle[nMissleIndex].m_nParentMissleIndex = 0;
			
			Missle[nMissleIndex].m_nSkillId			= m_nId;
			Missle[nMissleIndex].m_nStartLifeTime	= pSkillParam->nWaitTime + GetMissleGenerateTime(i);
			Missle[nMissleIndex].m_nLifeTime		+= Missle[nMissleIndex].m_nStartLifeTime;	
			Missle[nMissleIndex].m_nRefPX			= nDesSubX;
			Missle[nMissleIndex].m_nRefPY			= nDesSubY;
			if (Missle[nMissleIndex].m_eMoveKind == MISSLE_MMK_Line || Missle[nMissleIndex].m_eMoveKind == MISSLE_MMK_RollBack || Missle[nMissleIndex].m_eMoveKind == MISSLE_MMK_Follow)
			{
				Missle[nMissleIndex].m_nXFactor = g_DirCos(nDir, MaxMissleDir);
				Missle[nMissleIndex].m_nYFactor = g_DirSin(nDir, MaxMissleDir);
			}
			
			
#ifdef _SERVER
			Missle[nMissleIndex].SetMagicAttribsData(pNewMagicAttribsData);
#endif //_SERVER
			nCastMissleNum ++;
		}
		else
		{
			_ASSERT(m_nChildSkillId > 0 && m_nChildSkillLevel > 0)	;
			KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(m_nChildSkillId, m_nChildSkillLevel);
			if (pOrdinSkill) 
			{
				if (!pSkillParam->nParent)
					nCastMissleNum += pOrdinSkill->Cast(nLauncher, nDesSubX, nDesSubY, pSkillParam->nWaitTime + GetMissleGenerateTime(i), eLauncherType);
				else
					nCastMissleNum += pOrdinSkill->Cast(pSkillParam->nParent, nDesSubX, nDesSubY, pSkillParam->nWaitTime + GetMissleGenerateTime(i), pSkillParam->eParentType);
				
			}
		}
		
	}
	
exit:	
#ifdef _SERVER
	if (pNewMagicAttribsData)
		if (pNewMagicAttribsData->GetRef() == 0)
			delete pNewMagicAttribsData;
#endif
		return nCastMissleNum;
}

int		KSkill::CastExtractiveLineMissle(TOrdinSkillParam* pSkillParam,  int nDir,int nSrcX, int nSrcY, int nXOffset, int nYOffset, int nDesX, int nDesY)  const 
{
	
	_ASSERT(pSkillParam);
	
	int nLauncher = pSkillParam->nLauncher;
	if (pSkillParam->eLauncherType != SKILL_SLT_Npc) return 0;	
	int	nDirIndex		= g_Dir2DirIndex(nDir, MaxMissleDir);
	int nDesSubX		= 0;
	int nDesSubY		= 0;
	int nCastMissleNum	= 0;
	
	//×Óµ¯Ö®¼äµÄ¼ä¾à
	
	
#ifdef _SERVER
	KMissleMagicAttribsData * pNewMagicAttribsData = CreateMissleMagicAttribsData(nLauncher);
#endif //_SERVER
	
	//·Ö±ðÉú³É¶àÉÙ×Óµ¯
	{
		
		if (m_bBaseSkill)
		{
			int nMissleIndex ;
			int nSubWorldId ; 
			
			nSubWorldId = Npc[nLauncher].m_SubWorldIndex;
			
			if (nSubWorldId < 0)	goto exit;
			nMissleIndex = MissleSet.Add(nSubWorldId, nSrcX, nSrcY);
			
			if (nMissleIndex < 0)	goto exit;
			
			Missle[nMissleIndex].m_nDir				= nDir;
			Missle[nMissleIndex].m_nDirIndex		= nDirIndex;
			CreateMissle(nLauncher, m_nChildSkillId, nMissleIndex);
			
			if (Missle[nMissleIndex].m_eMoveKind == MISSLE_MMK_Parabola)
			{
				int nLength = g_GetDistance(nSrcX, nSrcY, nDesX, nDesY);
				int nTime = nLength / Missle[nMissleIndex].m_nSpeed;
				Missle[nMissleIndex].m_nHeightSpeed	= Missle[nMissleIndex].m_nZAcceleration * (nTime - 1) / 2;
				
			}
			
			Missle[nMissleIndex].m_nFollowNpcIdx	= pSkillParam->nTargetId;
			if (pSkillParam->nTargetId)
				Missle[nMissleIndex].m_dwFollowNpcID	= Npc[pSkillParam->nTargetId].m_dwID;
			Missle[nMissleIndex].m_dwBornTime		= SubWorld[nSubWorldId].m_dwCurrentTime;
			Missle[nMissleIndex].m_nSubWorldId		= nSubWorldId;
			Missle[nMissleIndex].m_nLauncher		= nLauncher;
			Missle[nMissleIndex].m_dwLauncherId		= Npc[nLauncher].m_dwID;
			Missle[nMissleIndex].m_nPKFlag			= Npc[nLauncher].m_nPKFlag;
			Missle[nMissleIndex].m_nMissleSeries	= -1;
			KSkill * pSkill =(KSkill*) Npc[nLauncher].GetActiveSkill();
			if(pSkill)
				Missle[nMissleIndex].m_nMissleSeries	= pSkill->GetSkillSeries();
		
			if (pSkillParam->nParent)
				Missle[nMissleIndex].m_nParentMissleIndex = pSkillParam->nParent;
			else 
				Missle[nMissleIndex].m_nParentMissleIndex = 0;
			
			Missle[nMissleIndex].m_nSkillId			= m_nId;
			Missle[nMissleIndex].m_nStartLifeTime	= pSkillParam->nWaitTime + GetMissleGenerateTime(0);
			Missle[nMissleIndex].m_nLifeTime		+= Missle[nMissleIndex].m_nStartLifeTime;	
			Missle[nMissleIndex].m_nRefPX			= nSrcX;
			Missle[nMissleIndex].m_nRefPY			= nSrcY;

			int nTempR = 0;
			int nTempMapX = 0;
			int nTempMapY = 0;
			int nTempOffsetX = 0;
			int nTempOffsetY = 0;

			Missle[nMissleIndex].m_bNeedReclaim = TRUE;
			int nLength = g_GetDistance(nSrcX, nSrcY, nDesX, nDesY);
			Missle[nMissleIndex].m_nFirstReclaimTime = nLength / Missle[nMissleIndex].m_nSpeed + Missle[nMissleIndex].m_nStartLifeTime;
			Missle[nMissleIndex].m_nEndReclaimTime = Missle[nMissleIndex].m_nFirstReclaimTime + SubWorld[nSubWorldId].m_nCellWidth / Missle[nMissleIndex].m_nSpeed + 2;

			if (Missle[nMissleIndex].m_eMoveKind == MISSLE_MMK_Line || Missle[nMissleIndex].m_eMoveKind == MISSLE_MMK_Parabola || Missle[nMissleIndex].m_eMoveKind == MISSLE_MMK_Follow)
			{	
				Missle[nMissleIndex].m_nXFactor = nXOffset;
				Missle[nMissleIndex].m_nYFactor = nYOffset;
			}
			
			
#ifdef _SERVER
			Missle[nMissleIndex].SetMagicAttribsData(pNewMagicAttribsData);
#endif //_SERVER
			
			nCastMissleNum ++;
		}
		else
		{
			KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(m_nChildSkillId, m_nChildSkillLevel);
			if (pOrdinSkill) 
			{
				if (!pSkillParam->nParent)
					nCastMissleNum += pOrdinSkill->Cast(nLauncher, nDesSubX, nDesSubY, pSkillParam->nWaitTime + GetMissleGenerateTime(0), pSkillParam->eLauncherType);
				else
					nCastMissleNum += pOrdinSkill->Cast(pSkillParam->nParent, nDesSubX, nDesSubY, pSkillParam->nWaitTime + GetMissleGenerateTime(0), pSkillParam->eParentType);
				
			}
		}
		
	}
	
exit:	
#ifdef _SERVER
	if (pNewMagicAttribsData)
		if (pNewMagicAttribsData->GetRef() == 0)
			delete pNewMagicAttribsData;
#endif
		
		return nCastMissleNum;
		
}

/*!*****************************************************************************
// Function		: KSkill::CastWall
// Purpose		: Wall Magic 
// Return		: int 
// Argumant		: int nLauncher
// Argumant		: eSkillLauncherType eLauncherType
// Argumant		: int nDir
// Argumant		: int nRefPX
// Argumant		: int nRefPY
// Argumant		: int nWaitTime
// Argumant		: int nTargetId
// Comments		:
// Author		: RomanDou
*****************************************************************************/
/*
m_nValue1 ±íÊ¾×Óµ¯Ö®¼äµÄ¾àÀë£¬µ¥Î»ÏñËØµã
*/
int KSkill::CastWall(TOrdinSkillParam * pSkillParam,  int nDir , int nRefPX , int nRefPY)  const 
{
	int nLauncher = pSkillParam->nLauncher;
	eSkillLauncherType eLauncherType = pSkillParam->eLauncherType;
	
	if (eLauncherType != SKILL_SLT_Npc) return 0;
	int	nDirIndex		= g_Dir2DirIndex(nDir, MaxMissleDir);
	int nDesSubX		= 0;
	int nDesSubY		= 0;
	int nCastMissleNum	= 0;
	
	
	//×Óµ¯Ö®¼äµÄ¼ä¾à
	int nMSDistanceEach = m_nValue1;
	int nCurMSDistance	= -1 * nMSDistanceEach * m_nChildSkillNum / 2;
	
#ifdef _SERVER
	KMissleMagicAttribsData * pNewMagicAttribsData = CreateMissleMagicAttribsData(nLauncher);
#endif //_SERVER
	
	//·Ö±ðÉú³É¶àÉÙ×Óµ¯
	for(int i = 0; i < m_nChildSkillNum; i++)
	{
		nDesSubX	= nRefPX + ((nCurMSDistance * g_DirCos(nDirIndex, MaxMissleDir)) >>10);
		nDesSubY	= nRefPY + ((nCurMSDistance * g_DirSin(nDirIndex, MaxMissleDir)) >>10);
		
		if (nDesSubX < 0 || nDesSubY < 0) 	continue;
		
		if (m_bBaseSkill)
		{
			int nMissleIndex ;
			int nSubWorldId ; 
			nSubWorldId = Npc[nLauncher].m_SubWorldIndex;
			
			if (nSubWorldId < 0)	
			{
				goto exit;
			}
			
			nMissleIndex = MissleSet.Add(nSubWorldId, nDesSubX, nDesSubY);
			if (nMissleIndex < 0)	
			{
				continue;
			}

			if (m_nValue2)
			{
				int nDirTemp = nDir - MaxMissleDir / 4;
				if (nDirTemp < 0) nDirTemp += MaxMissleDir;
				Missle[nMissleIndex].m_nDir				= nDirTemp;
				Missle[nMissleIndex].m_nDirIndex = g_Dir2DirIndex(nDirTemp, 64);

			}
			else
			{
				Missle[nMissleIndex].m_nDir				= nDir;
				Missle[nMissleIndex].m_nDirIndex		= nDirIndex;
			}
			
			Missle[nMissleIndex].m_nSubWorldId		= nSubWorldId;
			CreateMissle(nLauncher, m_nChildSkillId, nMissleIndex);
			Missle[nMissleIndex].m_nFollowNpcIdx	= pSkillParam->nTargetId;
			if (pSkillParam->nTargetId)
				Missle[nMissleIndex].m_dwFollowNpcID	= Npc[pSkillParam->nTargetId].m_dwID;
			Missle[nMissleIndex].m_dwBornTime		= SubWorld[nSubWorldId].m_dwCurrentTime;
			Missle[nMissleIndex].m_nLauncher		= nLauncher;
			Missle[nMissleIndex].m_dwLauncherId		= Npc[nLauncher].m_dwID;
			Missle[nMissleIndex].m_nPKFlag			= Npc[nLauncher].m_nPKFlag;
			Missle[nMissleIndex].m_nMissleSeries	= -1;
			KSkill * pSkill =(KSkill*) Npc[nLauncher].GetActiveSkill();
			if(pSkill)
				Missle[nMissleIndex].m_nMissleSeries	= pSkill->GetSkillSeries();
			
			if (pSkillParam->nParent)
				Missle[nMissleIndex].m_nParentMissleIndex = pSkillParam->nParent;
			else 
				Missle[nMissleIndex].m_nParentMissleIndex = 0;
			
			
			Missle[nMissleIndex].m_nSkillId			= m_nId;
			Missle[nMissleIndex].m_nStartLifeTime	= pSkillParam->nWaitTime + GetMissleGenerateTime(i);
			Missle[nMissleIndex].m_nLifeTime		+= Missle[nMissleIndex].m_nStartLifeTime;
			Missle[nMissleIndex].m_nRefPX			= nDesSubX;
			Missle[nMissleIndex].m_nRefPY			= nDesSubY;
			
			if (Missle[nMissleIndex].m_eMoveKind == MISSLE_MMK_Line || Missle[nMissleIndex].m_eMoveKind == MISSLE_MMK_RollBack || Missle[nMissleIndex].m_eMoveKind == MISSLE_MMK_Follow)
			{
				Missle[nMissleIndex].m_nXFactor = g_DirCos(Missle[nMissleIndex].m_nDir, MaxMissleDir);
				Missle[nMissleIndex].m_nYFactor = g_DirSin(Missle[nMissleIndex].m_nDir, MaxMissleDir);
			}
			
#ifdef _SERVER
			Missle[nMissleIndex].SetMagicAttribsData(pNewMagicAttribsData);
#endif //_SERVER
			
			nCastMissleNum ++;
		}
		else
		{
			_ASSERT(m_nChildSkillId > 0 && m_nChildSkillLevel > 0)	;
			KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(m_nChildSkillId, m_nChildSkillLevel);
			if (pOrdinSkill) 
			{
				if (!pSkillParam->nParent)
					nCastMissleNum += pOrdinSkill->Cast(nLauncher, nDesSubX, nDesSubY, pSkillParam->nWaitTime + GetMissleGenerateTime(i), eLauncherType);
				else
					nCastMissleNum += pOrdinSkill->Cast(pSkillParam->nParent, nDesSubX, nDesSubY, pSkillParam->nWaitTime +  GetMissleGenerateTime(i), pSkillParam->eParentType);
			}
		}
		
		nCurMSDistance += nMSDistanceEach;
	}
	
exit:	
#ifdef _SERVER
	if (pNewMagicAttribsData)
		if (pNewMagicAttribsData->GetRef() == 0)
			delete pNewMagicAttribsData;
#endif
		return nCastMissleNum;
}

/*!*****************************************************************************
// Function		: KSkill::CastCircle
// Purpose		: 
// Return		: 
// Argumant		: int nLauncher
// Argumant		: eSkillLauncherType  eLauncherType
// Argumant		: int nDir
// Argumant		: int nRefPX
// Argumant		: int nRefPY
// Argumant		: int nWaitTime
// Argumant		: int nTargetId
// Comments		:
// Author		: RomanDou
*****************************************************************************/
// Value1  == 0 ±íÊ¾·¢ËÍÕßÎªÔ²ÐÄ²úÉúÔ²£¬·ñÔòÒÔÄ¿±êµãÎªÔ²ÐÄ²úÉúÔ²
int		KSkill::CastCircle(TOrdinSkillParam * pSkillParam, int nDir, int nRefPX, int nRefPY)  const 
{
	int nLauncher = pSkillParam->nLauncher;
	eSkillLauncherType  eLauncherType = pSkillParam->eLauncherType;
	if (eLauncherType != SKILL_SLT_Npc) return 0;	
	int nDesSubPX	= 0;
	int nDesSubPY	= 0;
	int nFirstStep	= m_nValue2;			//µÚÒ»²½µÄ³¤¶È£¬×Óµ¯ÔÚ¸Õ·¢³öÈ¥Ê±ÀëÍæ¼ÒµÄ¾àÀë
	int nCurSubDir	= 0;
	int nDirPerNum  = 	MaxMissleDir / m_nChildSkillNum  ;
	int nCastMissleNum = 0;
#ifdef _SERVER
	KMissleMagicAttribsData * pNewMagicAttribsData = CreateMissleMagicAttribsData(nLauncher);
#endif //_SERVER
	
	//·Ö±ðÉú³É¶à¸ö×Óµ¯
	for(int i = 0; i < m_nChildSkillNum; i++)
	{
		int nCurSubDir	= nDir + nDirPerNum * i ;
		
		if (nCurSubDir < 0)
			nCurSubDir = MaxMissleDir + nCurSubDir;
		
		if (nCurSubDir >= MaxMissleDir)
			nCurSubDir -= MaxMissleDir;
		
		int nSinAB	= g_DirSin(nCurSubDir, MaxMissleDir);
		int nCosAB	= g_DirCos(nCurSubDir, MaxMissleDir);
		
		nDesSubPX	= nRefPX + ((nCosAB * nFirstStep) >> 10);
		nDesSubPY	= nRefPY + ((nSinAB * nFirstStep) >> 10);
		
		
		
		if (nDesSubPX < 0 || nDesSubPY < 0) 	continue;
		
		if (m_bBaseSkill)
		{
			int nMissleIndex ;
			int nSubWorldId ; 
			
			nSubWorldId = Npc[nLauncher].m_SubWorldIndex;
			
			if (nSubWorldId < 0)	goto exit;
			nMissleIndex = MissleSet.Add(nSubWorldId, nDesSubPX, nDesSubPY);
			
			if (nMissleIndex < 0)	
			{
				continue;
			}
			
			Missle[nMissleIndex].m_nDir			= nCurSubDir;
			Missle[nMissleIndex].m_nDirIndex	= g_Dir2DirIndex(nCurSubDir, MaxMissleDir);
			CreateMissle(nLauncher, m_nChildSkillId, nMissleIndex);
			
			Missle[nMissleIndex].m_nFollowNpcIdx	= pSkillParam->nTargetId;
			if (pSkillParam->nTargetId)
				Missle[nMissleIndex].m_dwFollowNpcID	= Npc[pSkillParam->nTargetId].m_dwID;
			Missle[nMissleIndex].m_dwBornTime		= SubWorld[nSubWorldId].m_dwCurrentTime;
			Missle[nMissleIndex].m_nSubWorldId		= nSubWorldId;
			Missle[nMissleIndex].m_nLauncher		= nLauncher;
			Missle[nMissleIndex].m_dwLauncherId		= Npc[nLauncher].m_dwID;
			Missle[nMissleIndex].m_nPKFlag			= Npc[nLauncher].m_nPKFlag;
			Missle[nMissleIndex].m_nMissleSeries	= -1;
			KSkill * pSkill =(KSkill*) Npc[nLauncher].GetActiveSkill();
			if(pSkill)
				Missle[nMissleIndex].m_nMissleSeries	= pSkill->GetSkillSeries();
			
			if (pSkillParam->nParent)
				Missle[nMissleIndex].m_nParentMissleIndex = pSkillParam->nParent;
			else 
				Missle[nMissleIndex].m_nParentMissleIndex = 0;
			
			
			Missle[nMissleIndex].m_nSkillId			= m_nId;
			Missle[nMissleIndex].m_nStartLifeTime	= pSkillParam->nWaitTime + GetMissleGenerateTime(i);
			Missle[nMissleIndex].m_nLifeTime		+= Missle[nMissleIndex].m_nStartLifeTime;
			Missle[nMissleIndex].m_nRefPX			= nDesSubPX;
			Missle[nMissleIndex].m_nRefPY			= nDesSubPY;
			
			if (Missle[nMissleIndex].m_eMoveKind == MISSLE_MMK_Line || Missle[nMissleIndex].m_eMoveKind == MISSLE_MMK_Follow)
			{
				Missle[nMissleIndex].m_nXFactor = g_DirCos(nCurSubDir, MaxMissleDir);
				Missle[nMissleIndex].m_nYFactor = g_DirSin(nCurSubDir, MaxMissleDir);
			}
			
			
#ifdef _SERVER
			Missle[nMissleIndex].SetMagicAttribsData(pNewMagicAttribsData);
#endif //_SERVER
			
			nCastMissleNum ++;
		}
		else
		{
			_ASSERT(m_nChildSkillId > 0 && m_nChildSkillLevel > 0)	;
			KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(m_nChildSkillId, m_nChildSkillLevel);
			if (pOrdinSkill) 
			{
				if (!pSkillParam->nParent)
					nCastMissleNum += pOrdinSkill->Cast(nLauncher, nDesSubPX, nDesSubPY, pSkillParam->nWaitTime + GetMissleGenerateTime(i), eLauncherType);
				else
					nCastMissleNum += pOrdinSkill->Cast(pSkillParam->nParent, nDesSubPX, nDesSubPY, pSkillParam->nWaitTime + GetMissleGenerateTime(i), pSkillParam->eParentType);
			}
		}
		
	}

exit:	
#ifdef _SERVER
	if (pNewMagicAttribsData)
		if (pNewMagicAttribsData->GetRef() == 0)
			delete pNewMagicAttribsData;
#endif
		
		return nCastMissleNum;
}

/*!*****************************************************************************
// Function		: KSkill::CastSpread
// Purpose		: 
// Return		: 
// Argumant		: int nLauncher
// Argumant		: eSkillLauncherType eLauncherType
// Argumant		: int nDir
// Argumant		: int nRefPX
// Argumant		: int nRefPY
// Argumant		: int nWaitTime
// Argumant		: int nTargetId
// Comments		:
// Author		: RomanDou
*****************************************************************************/
/*
Value1 Ã¿¸ö×Óµ¯Ïà²îµÄ½Ç¶Èµ¥Î»
Value2 Ã¿Ò»²½µÄ³¤¶È£¬µÚÒ»²½µÄ³¤¶È£¬×Óµ¯ÔÚ¸Õ·¢³öÈ¥Ê±ÀëÍæ¼ÒµÄ¾àÀë
*/
int		KSkill::CastSpread(TOrdinSkillParam * pSkillParam, int nDir, int nRefPX, int nRefPY)  const 
{
	int nLauncher = pSkillParam->nLauncher;
	eSkillLauncherType eLauncherType = pSkillParam->eLauncherType;
	if (eLauncherType != SKILL_SLT_Npc) return 0;
	int nDesSubMapX		= 0;
	int nDesSubMapY		= 0;
	int nFirstStep		= m_nValue2;			//µÚÒ»²½µÄ³¤¶È£¬×Óµ¯ÔÚ¸Õ·¢³öÈ¥Ê±ÀëÍæ¼ÒµÄ¾àÀë
	int nCurMSRadius	= m_nChildSkillNum / 2 ; 
	int nCurSubDir		= 0;
	int	nCastMissleNum  = 0;			//Êµ¼Ê·¢ËÍµÄMissleµÄÊýÁ¿
	
	// Sin A+B = SinA*CosB + CosA*SinB
	// Cos A+B = CosA*CosB - SinA*SinB
	// Sin A = nYFactor
	// Cos A = nXFactor
	
#ifdef _SERVER
	KMissleMagicAttribsData * pNewMagicAttribsData = CreateMissleMagicAttribsData(nLauncher);
#endif
	
	int nDesSubX = 0;
	int nDesSubY = 0;
	int nXFactor = 0;
	int nYFactor = 0;
	
	if (pSkillParam->nTargetId > 0)
	{
		int nTargetId = pSkillParam->nTargetId;
		int nDistance = 0;
		int nDesX, nDesY;
		if (Npc[nTargetId].m_Index > 0 && Npc[nTargetId].m_SubWorldIndex >= 0) 
			SubWorld[Npc[nTargetId].m_SubWorldIndex].Map2Mps(Npc[nTargetId].m_RegionIndex, Npc[nTargetId].m_MapX, Npc[nTargetId].m_MapY, Npc[nTargetId].m_OffX, Npc[nTargetId].m_OffY, &nDesX, &nDesY);
		
		nDistance = (int)sqrt((nDesX - nRefPX)*(nDesX - nRefPX) +	(nDesY - nRefPY)*(nDesY - nRefPY));
		nXFactor = ((nDesX - nRefPX)<<10) / nDistance;
		nYFactor = ((nDesY - nRefPY)<<10) / nDistance;
		
		nDesSubX = nRefPX + ((nXFactor * nFirstStep)>>10);
		nDesSubY = nRefPY + ((nYFactor * nFirstStep)>>10);
		
		if (nDesSubX < 0  || nDesSubY < 0 ) return 0;
	}
	
	int nTargetId = pSkillParam->nTargetId;
	
	//·Ö±ðÉú³É¶à¸ö×Óµ¯
	for(int i = 0; i < m_nChildSkillNum; i++)
	{
		int nDSubDir	= m_nValue1 * nCurMSRadius; 
		nCurSubDir		= nDir - m_nValue1 * nCurMSRadius;
		
		
		if (nCurSubDir < 0)
			nCurSubDir = MaxMissleDir + nCurSubDir;
		
		if (nCurSubDir >= MaxMissleDir)
			nCurSubDir -= MaxMissleDir;
		
		int nSinAB	;
		int nCosAB	;
		
		if (nTargetId > 0)
		{
			nDSubDir	+= 48;
			if (nDSubDir >= MaxMissleDir)
				nDSubDir -= MaxMissleDir;
			//sin(a - b) = sinacosb - cosa*sinb
			//cos(a - b) = cosacoab + sinasinb
			nSinAB = (nYFactor * g_DirCos(nDSubDir, MaxMissleDir) - nXFactor * g_DirSin(nDSubDir, MaxMissleDir)) >> 10;
			nCosAB = (nXFactor * g_DirCos(nDSubDir, MaxMissleDir) + nYFactor * g_DirSin(nDSubDir , MaxMissleDir)) >> 10;
		}
		else
		{
			nSinAB = g_DirSin(nCurSubDir, MaxMissleDir);
			nCosAB = g_DirCos(nCurSubDir, MaxMissleDir);
		}
		
		nDesSubX	= nRefPX + ((nCosAB * nFirstStep) >> 10);
		nDesSubY	= nRefPY + ((nSinAB * nFirstStep) >> 10);
		
		if (nDesSubX < 0 || nDesSubY < 0) 	continue;
		
		if (m_bBaseSkill)
		{
			
			int nMissleIndex ;
			int nSubWorldId ; 
			nSubWorldId = Npc[nLauncher].m_SubWorldIndex;
			
			if (nSubWorldId < 0)	goto exit;
			
			nMissleIndex = MissleSet.Add(nSubWorldId, nDesSubX, nDesSubY);
			
			if (nMissleIndex < 0)	continue;
			
			Missle[nMissleIndex].m_nDir				= nCurSubDir;
			Missle[nMissleIndex].m_nDirIndex		= g_Dir2DirIndex(nCurSubDir, MaxMissleDir);
			CreateMissle(nLauncher, m_nChildSkillId, nMissleIndex);
			Missle[nMissleIndex].m_nFollowNpcIdx	= nTargetId;
			if (pSkillParam->nTargetId)
				Missle[nMissleIndex].m_dwFollowNpcID	= Npc[pSkillParam->nTargetId].m_dwID;
			Missle[nMissleIndex].m_dwBornTime		= SubWorld[nSubWorldId].m_dwCurrentTime;
			Missle[nMissleIndex].m_nSubWorldId		= nSubWorldId;
			Missle[nMissleIndex].m_nLauncher		= nLauncher;
			Missle[nMissleIndex].m_dwLauncherId		= Npc[nLauncher].m_dwID;
			Missle[nMissleIndex].m_nPKFlag			= Npc[nLauncher].m_nPKFlag;
			Missle[nMissleIndex].m_nMissleSeries	= -1;
			KSkill * pSkill =(KSkill*) Npc[nLauncher].GetActiveSkill();
			if(pSkill)
				Missle[nMissleIndex].m_nMissleSeries	= pSkill->GetSkillSeries();
			
			if (pSkillParam->nParent)
				Missle[nMissleIndex].m_nParentMissleIndex = pSkillParam->nParent;
			else 
				Missle[nMissleIndex].m_nParentMissleIndex = 0;
			
			Missle[nMissleIndex].m_nSkillId			= m_nId;
			Missle[nMissleIndex].m_nStartLifeTime	= pSkillParam->nWaitTime + GetMissleGenerateTime(i);
			Missle[nMissleIndex].m_nLifeTime		+= Missle[nMissleIndex].m_nStartLifeTime;
			Missle[nMissleIndex].m_nXFactor			= nCosAB;
			Missle[nMissleIndex].m_nYFactor			= nSinAB;
			Missle[nMissleIndex].m_nRefPX			= nDesSubX;
			Missle[nMissleIndex].m_nRefPY			= nDesSubY;
			
#ifdef _SERVER
			Missle[nMissleIndex].SetMagicAttribsData(pNewMagicAttribsData);
#endif //_SERVER
			nCastMissleNum ++;
		}
		else
		{
			_ASSERT(m_nChildSkillId > 0 && m_nChildSkillLevel > 0)	;
			KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(m_nChildSkillId, m_nChildSkillLevel);
			if (pOrdinSkill) 
			{
				if (!pSkillParam->nParent)
					nCastMissleNum +=  pOrdinSkill->Cast(nLauncher,  nRefPX, nRefPY , pSkillParam->nWaitTime + GetMissleGenerateTime(i), eLauncherType);
				else
					nCastMissleNum +=  pOrdinSkill->Cast(pSkillParam->nParent,  nRefPX, nRefPY , pSkillParam->nWaitTime + GetMissleGenerateTime(i), pSkillParam->eParentType); 
			}
		}
		
		nCurMSRadius -- ;
	}
exit:	
#ifdef _SERVER
	if (pNewMagicAttribsData)
		if (pNewMagicAttribsData->GetRef() == 0)
			delete pNewMagicAttribsData;
#endif
		
		return nCastMissleNum;
}

/*!*****************************************************************************
// Function		: KSkill::GetChildSkillNum
// Purpose		: ÓÉÓÚ¿ÉÄÜÄ³Ð©¼¼ÄÜÖÐ£¬Ëæ×Å¼¼ÄÜµÈ¼¶µÄÉý¼¶£¬×Óµ¯µÄÊýÄ¿Ò²»áÒò´ËÔö¼Ó£¬ËùÒÔÍ¨¹ý¸Ãº¯Êý»ñµÃÊµ¼ÊµÄ×Ó¼¼ÄÜÊýÄ¿
// Return		: 
// Argumant		: int nLevel
// Comments		:
// Author		: RomanDou
*****************************************************************************/
int 	KSkill::GetChildSkillNum(int nLevel)  const 
{
	return m_nChildSkillNum;
};
/*!*****************************************************************************
// Function		: KSkill::CreateMissle
// Purpose		: ÉèÖÃ×Óµ¯µÄ»ù±¾Êý¾Ý£¬ÒÔ¼°¸Ã¼¼ÄÜ¸ÃµÈ¼¶ÏÂµÄ¶Ô×Óµ¯ÐÅÏ¢µÄ±ä¶¯Êý¾Ý
//					ÉèÖÃÓÃÓÚÊýÖµ¼ÆËãµÄÖ¸Õë
// Return		: 
// Argumant		: int nChildSkillId
// Argumant		: int nMissleIndex
// Comments		:
// Author		: RomanDou
*****************************************************************************/
void	KSkill::CreateMissle(int nLauncher, int nChildSkillId, int nMissleIndex)  const 
{
	_ASSERT(nChildSkillId > 0 && nChildSkillId < MAX_MISSLESTYLE && nMissleIndex > 0);
	
	if (nLauncher <= 0) 
	{
		return ;
	}
	
	KMissle * pMissle = &Missle[nMissleIndex];
	
	g_MisslesLib[nChildSkillId] = *pMissle;//¸´ÖÆ¿½±´¶ÔÏó
	
	pMissle->m_nLevel			= m_ulLevel;
	pMissle->m_bCollideEvent	= m_bCollideEvent;
	pMissle->m_bVanishedEvent   = m_bVanishedEvent;
	pMissle->m_bStartEvent		= m_bStartEvent;
	pMissle->m_bFlyEvent		= m_bFlyingEvent;
	pMissle->m_nFlyEventTime	= m_nFlyEventTime;
	pMissle->m_nMissleId		= nMissleIndex;
	pMissle->m_bClientSend      = m_bClientSend;
	pMissle->m_bMustBeHit		= m_bMustBeHit;
	pMissle->m_bIsMelee			= m_bIsMelee;
	pMissle->m_bByMissle		= m_bByMissle;
	pMissle->m_bTargetSelf		= (m_bTargetSelf == 1);
	pMissle->m_nInteruptTypeWhenMove = m_nInteruptTypeWhenMove;
	pMissle->m_bHeelAtParent	= m_bHeelAtParent;
	pMissle->m_bUseAttackRating	= m_bUseAttackRate;
	pMissle->m_nDoHurtP			= m_nDoHurtP;
	
	if (pMissle->m_nInteruptTypeWhenMove)
	{
		Npc[nLauncher].GetMpsPos(&pMissle->m_nLauncherSrcPX, &pMissle->m_nLauncherSrcPY);
	}
	pMissle->m_eRelation = m_eRelation;

	
#ifndef _SERVER
	pMissle->m_MissleRes.m_bNeedShadow   = m_bNeedShadow;
	pMissle->m_MissleRes.m_nMaxShadowNum = m_nMaxShadowNum;
	pMissle->m_MissleRes.m_nMissleId	 = nMissleIndex;
	if (!pMissle->m_MissleRes.Init()) g_DebugLog("Create bullet sticker failed! %s", __FILE__) ;
#endif
	
	pMissle->DoWait();
	
	for (int i = 0  ; i < m_nMissleAttribsNum; i ++)
	{
		switch (m_MissleAttribs[i].nAttribType)
		{
		case magic_missle_movekind_v:
			{
				pMissle->m_eMoveKind	= (eMissleMoveKind) m_MissleAttribs[i].nValue[0];
			}break;
			
		case magic_missle_speed_v:	
			{
				pMissle->m_nSpeed		= m_MissleAttribs[i].nValue[0];
			}break;
			
		case magic_missle_lifetime_v:
			{
				pMissle->m_nLifeTime	= m_MissleAttribs[i].nValue[0];
			}break;
			
		case magic_missle_height_v:	
			{
				pMissle->m_nHeight		= m_MissleAttribs[i].nValue[0];
			}break;
			
		case magic_missle_damagerange_v:
			{
				pMissle->m_nDamageRange = m_MissleAttribs[i].nValue[0];
			}break;
			
		case magic_missle_radius_v:	
			{
			}break;
		case magic_missle_missrate:
			{
				pMissle->m_nMissRate = m_MissleAttribs[i].nValue[0];
			}break;
		case magic_missle_hitcount:
			{
				pMissle->m_nHitCount = m_MissleAttribs[i].nValue[0];
			}break;
		case magic_missle_range:
			{
			}break;
		case magic_missle_dmginterval:
			{
				pMissle->m_ulDamageInterval = m_MissleAttribs[i].nValue[0];
			}break;
		case magic_missle_zspeed:
			{
				pMissle->m_nHeightSpeed = m_MissleAttribs[i].nValue[0];
			}break;
		case magic_missle_ablility:
			{
			}break;
		case magic_missle_param:
			{
			}break;
		case magic_missle_wait:
			{
			}break;
		case magic_missle_fly:
			{
			}break;
		case magic_missle_collide:
			{
			}break;
		case magic_missle_vanish:
			{
			}break;
		}
	}
	
	if (m_bIsMelee)
		pMissle->m_nLifeTime = Npc[nLauncher].ModifyMissleLifeTime(pMissle->m_nLifeTime);
	else
	{
		pMissle->m_nSpeed = Npc[nLauncher].ModifyMissleSpeed(pMissle->m_nSpeed);
		pMissle->m_bCollideVanish = Npc[nLauncher].ModifyMissleCollsion(pMissle->m_bCollideVanish);
	}
	
}


/*!*****************************************************************************
// Function		: KSkill::GetInfoFromTabFile
// Purpose		: ´ÓTabFileÖÐ»ñµÃ¸Ã¼¼ÄÜµÄ³£¹æÊôÐÔ
// Return		: 
// Argumant		: int nCol
// Comments		:
// Author		: RomanDou
*****************************************************************************/
BOOL	KSkill::GetInfoFromTabFile(int nRow)
{
	KITabFile * pITabFile = &g_OrdinSkillsSetting;
	return GetInfoFromTabFile(&g_OrdinSkillsSetting, nRow);
}


BOOL	KSkill::GetInfoFromTabFile(KITabFile *pSkillsSettingFile, int nRow)
{
	if (!pSkillsSettingFile || nRow < 0) return FALSE;
	//	
	pSkillsSettingFile->GetString(nRow, "SkillName",		"", m_szName, sizeof(m_szName) ,TRUE);
	pSkillsSettingFile->GetInteger(nRow, "SkillId",			0, (int *)&m_nId,TRUE);
	pSkillsSettingFile->GetInteger(nRow, "Attrib",			0, (int *)&m_nAttrib,TRUE);
	
	int nReqLevel = 0;
	pSkillsSettingFile->GetInteger(nRow, "ReqLevel",		0, (int *)&nReqLevel, TRUE);
	m_usReqLevel = (unsigned short)nReqLevel;


	pSkillsSettingFile->GetInteger(nRow, "EqtLimit",		-2, (int *)&m_nEquiptLimited, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "HorseLimit",		0, (int *)&m_nHorseLimited, TRUE);

	pSkillsSettingFile->GetInteger(nRow, "DoHurt",			1, (int *)&m_nDoHurtP);
	pSkillsSettingFile->GetInteger(nRow, "ChildSkillNum",	0, &m_nChildSkillNum,TRUE);
	pSkillsSettingFile->GetInteger(nRow, "MisslesForm",		0, (int *)&m_eMisslesForm, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "CharClass",		0, &m_nCharClass, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "SkillStyle",		0, (int *)&m_eSkillStyle, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "CharAnimId",		0, (int *)&m_nCharActionId, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "IsPhysical",		0, &m_bIsPhysical, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "IsAura",			0, &m_bIsAura, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "IsUseAR",			0, &m_bUseAttackRate, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "TargetOnly",		0, &m_bTargetOnly, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "TargetEnemy",		0, &m_bTargetEnemy, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "TargetAlly",		0, &m_bTargetAlly, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "TargetObj",		0, &m_bTargetObj, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "TargetNoNpc",		0, &m_bTargetNoNpc, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "BaseSkill",		0, &m_bBaseSkill, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "ByMissle",		0, &m_bByMissle, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "ChildSkillId",	0, &m_nChildSkillId, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "StartEvent",		0, &m_bStartEvent, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "StartSkillId",	0, &m_nStartSkillId, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "FlyEvent",		0, &m_bFlyingEvent, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "FlySkillId",		0, &m_nFlySkillId, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "FlyEventTime",	0, &m_nFlyEventTime, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "CollideEvent",	0, &m_bCollideEvent, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "CollidSkillId",	0, &m_nCollideSkillId, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "VanishedEvent",	0, &m_bVanishedEvent, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "VanishedSkillId",	0, &m_nVanishedSkillId, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "SkillCostType",	0, (int *)&m_nSkillCostType, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "CostValue",		0, &m_nCost, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "TimePerCast",		0, &m_nMinTimePerCast, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "TimePerCastOnHorse",		0, &m_nMinTimePerCastOnHorse, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "Param1",			0, &m_nValue1, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "Param2",			0, &m_nValue2, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "ChildSkillLevel", 0, &m_nChildSkillLevel, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "EventSkillLevel", 0, &m_nEventSkillLevel, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "IsMelee",			0, &m_bIsMelee, TRUE);
	
	pSkillsSettingFile->GetInteger(nRow, "MslsGenerate",	0, (int *)&m_eMisslesGenerateStyle, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "MslsGenerateData",0, &m_nMisslesGenerateData, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "MaxShadowNum",	0, &m_nMaxShadowNum, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "AttackRadius",	50, &m_nAttackRadius, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "WaitTime",		0, &m_nWaitTime, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "ClientSend",		0, &m_bClientSend, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "TargetSelf",		0, &m_bTargetSelf, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "StopWhenMove",	0, &m_nInteruptTypeWhenMove, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "HeelAtParent",  0, (int *)&m_bHeelAtParent, TRUE );
	pSkillsSettingFile->GetInteger(nRow, "ShowEvent",  0, (int *)&m_nShowEvent, TRUE );
	//·þÎñÆ÷¶ËÐèÒª»ñµÃ²¢Í¨Öª¿Í»§¶Ë
	pSkillsSettingFile->GetInteger(nRow, "StateSpecialId",  0, &m_nStateSpecialId, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "IsExpSkill",		0, (int *)&m_nIsExpSkill, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "Series",		-1, (int *)&m_nSeries, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "ShowAddition",		0, (int *)&m_nShowAddition, TRUE);

	m_eRelation = 0;
	if (m_bTargetEnemy)
		m_eRelation |= relation_enemy;
	
	if (m_bTargetAlly)
		m_eRelation |= relation_ally;
	
	if (m_bTargetSelf)
		m_eRelation |= relation_self;
	
#ifndef _SERVER
	pSkillsSettingFile->GetString(nRow, "SkillDesc", "", m_szSkillDesc, sizeof(m_szSkillDesc));
	pSkillsSettingFile->GetInteger(nRow, "NeedShadow",		0, &m_bNeedShadow, TRUE);
	pSkillsSettingFile->GetString(nRow, "SkillIcon","\\spr\\skill\\Í¼±ê\\Í¨ÓÃ.spr",	m_szSkillIcon, 80);
	if (!m_szSkillIcon[0])	strcpy(m_szSkillIcon, "\\spr\\skill\\Í¼±ê\\Í¨ÓÃ.spr");
		KImageParam sImage;
	if (g_pRepresent->GetImageParam(m_szSkillIcon, &sImage, ISI_T_SPR) == false)
	{
		strcpy(m_szSkillIcon, UNKNOWNITEM_SPR36);
	}
	pSkillsSettingFile->GetInteger(nRow, "LRSkill",		0, (int*)&m_eLRSkillInfo);
	pSkillsSettingFile->GetString(nRow, "PreCastSpr", "", m_szPreCastEffectFile, 100);
	pSkillsSettingFile->GetString(nRow, "ManCastSnd","", m_szManPreCastSoundFile, 100);
	pSkillsSettingFile->GetString(nRow, "FMCastSnd", "", m_szFMPreCastSoundFile, 100);
#else
	char szLevelScript[MAX_PATH];
	
	//¶ÁÈ¡Éè¶¨½Å±¾1£¬¼ÇÂ¼Ã¿Ò»¼¶¼¼ÄÜµÄÊôÐÔ±ä»¯£¬ÊýÖµÉè¶¨µÄ
	pSkillsSettingFile->GetString(nRow, "LvlSetScript", "", szLevelScript, MAX_PATH);
	if (szLevelScript[0])
	{
		strlwr(szLevelScript);
		m_dwSkillLevelDataScriptId = g_FileName2Id(szLevelScript);
	}
	
	//¶ÁÈ¡Éè¶¨½Å±¾2£¬¼ÇÂ¼¼¼ÄÜÏÞÖÆÐÅÏ¢
	pSkillsSettingFile->GetString(nRow, "LevelUpScript","",szLevelScript, MAX_PATH);
	if (szLevelScript[0])
	{
		strlwr(szLevelScript);
		m_dwSkillLevelUpScriptID = g_FileName2Id(szLevelScript);
	}
#endif
	return TRUE;
}

/*!*****************************************************************************
// Function		: KSkill::LoadSkillLevelData
// Purpose		: ¶Á±í»ñµÃµ±Ç°µÈ¼¶ÏÂµ±Ç°¼¼ÄÜµÄ¼¼ÄÜ¡¢×Óµ¯¡¢Åö×²ÊýÖµÓ°Ïì
// Return		: 
// Argumant		: int nLevel
// Comments		:
// Author		: Romandou
****************************************************************************/
void		KSkill::LoadSkillLevelData(unsigned long  nLevel /* =0*/, int nParam)
{
	m_nMissleAttribsNum = 0;
	m_nDamageAttribsNum = 0;
	m_nAddSkillDamageNum = 0;
	m_nImmediateAttribsNum = 0;
	m_nStateAttribsNum	= 0;		//¸öÊý£¬×î´ó10
		
	char szSettingScriptName[MAX_PATH];
	char szSettingNameValue[100];
	char szSettingDataValue[100];
	char szResult[300];
	int nRowId = nParam;
	if (nRowId < 2) return ;
	//Question ±ØÐëË³ÐòÒ»ÖÁ²ÅÐÐ
	KLuaScript * pScript = NULL;
#ifndef _SERVER
	g_OrdinSkillsSetting.GetString(nRowId,  "LvlSetScript", "", szSettingScriptName, MAX_PATH );
	if (!szSettingScriptName[0]) return;
	g_SetFilePath("\\");
	//¼ÓÔØLua½Å±¾
	KLuaScript Script;
	Script.Init();
	if (!Script.Load(szSettingScriptName)) 
	{
		g_DebugLog("Can't load %s", szSettingScriptName);
		return;
	}
	pScript  = &Script;
#else
	if (!m_dwSkillLevelDataScriptId) 
	{
		g_DebugLog("Not exits script [%s]\n", GetSkillName());
		return ;
	}
	
	pScript = (KLuaScript*)g_GetScript(m_dwSkillLevelDataScriptId);
	
	if (!pScript)
	{
		g_DebugLog("Can't find script [%s]\n", GetSkillName());
		return ;
	}
#endif
	
	
	int nSafeIndex = 1;
	pScript->SafeCallBegin(&nSafeIndex);
	
	for(int i = 0 ;  i  < MAXSKILLLEVELSETTINGNUM ; i ++)
	{
		char szSettingName[40];
		char szSettingData[40];
		sprintf(szSettingName, "LvlSetting%d", i + 1);
		sprintf(szSettingData, "LvlData%d", i + 1);
		
		g_OrdinSkillsSetting.GetString(nRowId, szSettingName, "", szSettingNameValue, 100);
		g_OrdinSkillsSetting.GetString(nRowId, szSettingData, "", szSettingDataValue, 100);
		if (szSettingNameValue[0] == 0 	|| szSettingDataValue[0] == '0'	)
		{
			continue;
		}
		
		pScript->CallFunction("GetSkillLevelData", 1, "ssd", szSettingNameValue, szSettingDataValue, nLevel);
		const char * szType = lua_typename(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState));
		if (Lua_IsNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)
		{
			int nResult = (int)Lua_ValueToNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState));
			sprintf(szResult, "%d", nResult);
		}
		else if (Lua_IsString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)
		{
			if(strcmp(szSettingNameValue, "skill_desc") == 0)
			{
#ifndef _SERVER
				strcpy(m_szMagicSkillDesc, (char *)Lua_ValueToString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)));
#endif
				continue;
			}
			else
			strcpy(szResult , (char *)Lua_ValueToString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)));
		}
		else
		{
			char szMsg[300];
			sprintf(szMsg, "Cap ky nang %d(%s,%s) da xay ra loi, xin kiem ra lai!",nLevel, szSettingNameValue, szSettingDataValue);
			g_DebugLog(szMsg);
			break;
		}
		ParseString2MagicAttrib(nLevel, szSettingNameValue, szResult);
		
	}
	pScript->SafeCallEnd(nSafeIndex);
}

#ifdef _SERVER
//When nLauncher == 0 , means neednt  AppendSkillEffect;
KMissleMagicAttribsData* KSkill::CreateMissleMagicAttribsData(int nLauncher)  const 
{
	if (nLauncher < 0 || m_bClientSend) return NULL; 
	
	KMissleMagicAttribsData* pMissleMagicAttribsData = new KMissleMagicAttribsData;
	
	pMissleMagicAttribsData->m_pStateMagicAttribs = (KMagicAttrib *)m_StateAttribs;
	pMissleMagicAttribsData->m_nStateMagicAttribsNum = m_nStateAttribsNum;
	
	pMissleMagicAttribsData->m_pImmediateAttribs = (KMagicAttrib *)m_ImmediateAttribs;
	pMissleMagicAttribsData->m_nImmediateMagicAttribsNum = m_nImmediateAttribsNum;
	
	KMagicAttrib * pDamageAttribs =  new KMagicAttrib[MAX_MISSLE_DAMAGEATTRIB];
	pMissleMagicAttribsData->m_nDamageMagicAttribsNum = m_nDamageAttribsNum;
	
	//¸ù¾ÝÍæ¼ÒµÄ»ù±¾ÊôÐÔ£¬È·¶¨×Óµ¯µÄÉËº¦
	if (nLauncher)
	{
		Npc[nLauncher].AppendSkillEffect(m_nId, m_bIsPhysical, m_bIsMelee, (KMagicAttrib *)m_DamageAttribs, pDamageAttribs);
	}
	else
	{
		memcpy(pDamageAttribs, (KMagicAttrib *)m_DamageAttribs, sizeof(m_DamageAttribs));
	}
	
	pMissleMagicAttribsData->m_pDamageMagicAttribs = pDamageAttribs;
	return pMissleMagicAttribsData;
}
#endif
/*!*****************************************************************************
// Function		: KSkill::SetMissleGenerateTime
// Purpose		: »ñµÃµ±Ç°µÄ×Óµ¯µÄÊµ¼Ê²úÉúÊ±¼ä
// Return		: void 
// Argumant		: Missle * pMissle
// Argumant		: int nNo
// Comments		:
// Author		: RomanDou
*****************************************************************************/
unsigned int KSkill::GetMissleGenerateTime(int nNo) const 
{
	
	switch(m_eMisslesGenerateStyle)
	{
	case SKILL_MGS_NULL:
		{
			return m_nWaitTime;
		}break;
		
	case SKILL_MGS_SAMETIME:
		{
			return  m_nWaitTime + m_nMisslesGenerateData;
		}break;
		
	case SKILL_MGS_ORDER:		
		{
			return  m_nWaitTime + nNo * m_nMisslesGenerateData;
		}break;
		
	case SKILL_MGS_RANDONORDER:	
		{
			if (g_Random(2) == 1) 
				return m_nWaitTime + nNo * m_nMisslesGenerateData + g_Random(m_nMisslesGenerateData);
			else 
				return m_nWaitTime + nNo * m_nMisslesGenerateData  - g_Random(m_nMisslesGenerateData / 2);
		}break;
		
	case SKILL_MGS_RANDONSAME:	
		{
			return  m_nWaitTime + g_Random(m_nMisslesGenerateData);
		}break;
		
	case SKILL_MGS_CENTEREXTENDLINE:
		{
			if (m_nChildSkillNum <= 1) return m_nWaitTime;
			int nCenter = m_nChildSkillNum / 2	;
			return m_nWaitTime + abs(nNo - nCenter) * m_nMisslesGenerateData ;
		}
	}
	return m_nWaitTime;
}

int KSkill::GetSkillIdFromName(char * szSkillName)  
{
	//	
	if (!szSkillName || !szSkillName[0]) 
        return -1;
	
	for (int i = 0; i < MAX_SKILL; i ++)
	{
		KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(i, 1);
		if (pOrdinSkill) 
		{
			if (!strcmp(pOrdinSkill->m_szName, szSkillName))
            {
                return i;
            }
		}
	}
	return -1;
	
}


/*!*****************************************************************************
// Function		: KSkill::CastInitiativeSkill
// Purpose		: Ö÷¶¯¸¨Öú¼¼ÄÜ
// Return		: BOOL 
// Argumant		: int nLauncher
// Argumant		: int nParam1
// Argumant		: int nParam2
// Argumant		: int nWaitTime
// Comments		:
// Author		: RomanDou
*****************************************************************************/
BOOL KSkill::CastInitiativeSkill(int nLauncher, int nParam1, int nParam2, int nWaitTime)  const 
{
#ifdef _SERVER
	//¶Ô×ÔÒÑµÄÖ÷¶¯¸¨Öú¼¼ÄÜ
	if (nParam1 != -1 && m_bTargetSelf) 
	{
		nParam1 = -1;
		nParam2 = nLauncher;
	}
	else
	{
		if (nParam1 != -1 || nParam2 <= 0 || nParam2 >= MAX_NPC) return FALSE;
		
		NPC_RELATION  Relation = NpcSet.GetRelation(nLauncher, nParam2);
		
		if (m_bTargetEnemy)
		{
			if (Relation & relation_enemy) 
				goto lab_processdamage;
		}
		
		if (m_bTargetAlly)
		{
			if (Relation & relation_ally) 
				goto lab_processdamage;
		}
		
		if (m_bTargetSelf)
		{
			if (Relation & relation_self) 
				goto lab_processdamage;
		}
		return FALSE;
		
	}
	
lab_processdamage:			
	
	KMissleMagicAttribsData * pAttribsData = CreateMissleMagicAttribsData(nLauncher);
	if (pAttribsData) 
	{
		if (Npc[nParam2].ReceiveDamage(nLauncher, -1, m_bIsMelee, pAttribsData->m_pDamageMagicAttribs, m_bUseAttackRate, m_nDoHurtP, 0))
		{		
			if (pAttribsData->m_nStateMagicAttribsNum > 0)
				Npc[nParam2].SetStateSkillEffect(nLauncher, m_nId, m_ulLevel, pAttribsData->m_pStateMagicAttribs, pAttribsData->m_nStateMagicAttribsNum, pAttribsData->m_pStateMagicAttribs[0].nValue[1]);
			
			if (pAttribsData->m_nImmediateMagicAttribsNum > 0)
				Npc[nParam2].SetImmediatelySkillEffect(nLauncher, pAttribsData->m_pImmediateAttribs, pAttribsData->m_nImmediateMagicAttribsNum);
		}
		return TRUE;
	}
	if (pAttribsData->DelRef() == 0)
		delete pAttribsData;
#endif //_SERVER
	return TRUE;
}

/*!*****************************************************************************
// Function		: KSkill::CastPassivitySkill
// Purpose		: ±»¶¯
// Return		: BOOL 
// Argumant		: int nLauncher
// Argumant		: int nParam1
// Argumant		: int nParam2
// Argumant		: int nWaitTime
// Comments		:
// Author		: RomanDou
*****************************************************************************/
BOOL KSkill::CastPassivitySkill(int nLauncher, int nParam1, int nParam2, int nWaitTime)  const 
{
#ifdef _SERVER
	//ÊÇ±»¶¯¼¼ÄÜÊ±£¬ÊÇ·ñ»¹ÐèÒªÉú³ÉMissleMagicAttribs?
	KMissleMagicAttribsData * pAttribsData = (KMissleMagicAttribsData*)m_StateAttribs;//CreateMissleMagicAttribsData(nLauncher);
	if (m_nStateAttribsNum > 0)
	{
		Npc[nLauncher].SetStateSkillEffect(nLauncher, m_nId, m_ulLevel, (KMagicAttrib *)m_StateAttribs, m_nStateAttribsNum, -1);
	}
#endif
	return TRUE;
}

/*!*****************************************************************************
// Function		: KSkill::ParseString2MagicAttrib
// Purpose		: ½âÎöÍ¨¹ý½Å±¾ÔËËã»ñµÃµÄ¼¼ÄÜÊý¾Ý
// Return		: 
// Argumant		: char * szMagicAttribName
// Argumant		: char * szValue
// Comments		:
// Author		: RomanDou
*****************************************************************************/
BOOL	KSkill::ParseString2MagicAttrib(unsigned long ulLevel, char * szMagicAttribName, char * szValue)  
{
	int nValue1 = 0;
	int nValue2 = 0;
	int nValue3 = 0;
    const char *pcszTemp = NULL;
	if ((!szMagicAttribName) || (!szMagicAttribName[0])) return FALSE;
	//nValue2 µ±ÖµÎª-1Ê±ÎªÓÀ¾ÃÐÔ×´Ì¬£¬0Îª·Ç×´Ì¬£¬ÆäËüÖµÎªÓÐÊ±Ð§ÐÔ×´Ì¬Ä§·¨Ð§¹û
	//ÐèÒª½«×´Ì¬Êý¾ÝÓë·Ç×´Ì¬Êý¾Ý·ÖÀë³öÀ´£¬·ÅÈëÏàÓ¦µÄÊý×éÄÚ£¬²¢¼ÇÂ¼×ÜÊýÁ¿
	for (int i  = 0 ; i <= magic_normal_end; i ++)
	{
	
		if (!strcmp(szMagicAttribName, MagicAttrib2String(i)))
		{
            pcszTemp = szValue;
            nValue1 = KSG_StringGetInt(&pcszTemp, 0);
            KSG_StringSkipSymbol(&pcszTemp, ',');
            nValue2 = KSG_StringGetInt(&pcszTemp, 0);
            KSG_StringSkipSymbol(&pcszTemp, ',');
            nValue3 = KSG_StringGetInt(&pcszTemp, 0);
			if ((i > magic_missle_begin && i < magic_missle_end) || 
				(i > magic_missle_exp_begin && i < magic_missle_exp_end))
			{
				m_MissleAttribs[m_nMissleAttribsNum].nAttribType = i;
				m_MissleAttribs[m_nMissleAttribsNum].nValue[0] = nValue1;
				m_MissleAttribs[m_nMissleAttribsNum].nValue[1] = nValue2;
				m_MissleAttribs[m_nMissleAttribsNum].nValue[2] = nValue3;
				m_nMissleAttribsNum ++;
				return TRUE;
			}
			if (i >= magic_addskilldamage1 && i <= magic_addskilldamage9)
			{
                m_AddSkillDamage[m_nAddSkillDamageNum].nAttribType = i; 
                m_AddSkillDamage[m_nAddSkillDamageNum].nValue[0] = nValue1; 
                m_AddSkillDamage[m_nAddSkillDamageNum].nValue[1] = nValue2; 
                m_AddSkillDamage[m_nAddSkillDamageNum].nValue[2] = nValue3; 
				m_nAddSkillDamageNum++;
				return TRUE;
			}
			if (i > magic_skill_begin && i < magic_skill_end)
			{
				switch(i)
				{
				case magic_skill_cost_v:				// ÏûºÄMANA
					{
						m_nCost = nValue1;
					}
					break;
					
				case magic_skill_costtype_v:
					{
						m_nSkillCostType = (NPCATTRIB)nValue1;
					}
					break;
					
				case magic_skill_mintimepercast_v: 		// Ã¿´Î·¢Ä§·¨µÄ¼ä¸ôÊ±¼ä
					{
						m_nMinTimePerCast = nValue1;
					}
					break;
					
				case magic_skill_misslenum_v:
					{
						m_nChildSkillNum = nValue1;
					}
					break;
					
				case magic_skill_misslesform_v:
					{
						m_eMisslesForm = (eMisslesForm) nValue1;
					}
					break;
				case magic_skill_param1_v:
					{
						m_nValue1 = nValue1;
					}
					break;
				case magic_skill_param2_v:	
					{
						m_nValue2 = nValue2;
					}
					break;
				case magic_skill_eventskilllevel:
					{
						m_nEventSkillLevel = nValue1;
					}
					break;
				case magic_skill_attackradius:
					{
						m_nAttackRadius = nValue1;
					}
					break;
				case magic_skill_showevent:	
					{
						m_nShowEvent = nValue1;
					}
					break;
				case magic_skill_appendskill:
					{
						if(m_nAppendSkillNum >= MAX_APPENDSKILL)
							break;
						m_nAppendSkillId[m_nAppendSkillNum] = nValue1;
						m_nAppendSkillNum++;
					}break;
				}
				return TRUE;
			}
			
			if (i > magic_damage_begin && i < magic_damage_end)
			{
				switch(i)
				{
				case magic_attackrating_v:
				case magic_attackrating_p:
					m_DamageAttribs[0].nAttribType = i;
					m_DamageAttribs[0].nValue[0] = nValue1;
					m_DamageAttribs[0].nValue[1] = nValue2;
					m_DamageAttribs[0].nValue[2] = nValue3;
					m_nDamageAttribsNum ++;
					break;
				case magic_ignoredefense_p:
					m_DamageAttribs[1].nAttribType = i;
					m_DamageAttribs[1].nValue[0] = nValue1;
					m_DamageAttribs[1].nValue[1] = nValue2;
					m_DamageAttribs[1].nValue[2] = nValue3;
					m_nDamageAttribsNum ++;
					break;
				case magic_magicdamage_v:
					m_DamageAttribs[2].nAttribType = i;
					m_DamageAttribs[2].nValue[0] = nValue1;
					m_DamageAttribs[2].nValue[1] = nValue2;
					m_DamageAttribs[2].nValue[2] = nValue3;
					m_nDamageAttribsNum ++;
					break;
				case magic_seriesdamage_p:
					m_DamageAttribs[3].nAttribType = i;
					m_DamageAttribs[3].nValue[0] = nValue1;
					m_DamageAttribs[3].nValue[1] = nValue2;
					m_DamageAttribs[3].nValue[2] = nValue3;
					m_nDamageAttribsNum ++;
					break;
				case magic_deadlystrike_p:
					m_DamageAttribs[4].nAttribType = i;
					m_DamageAttribs[4].nValue[0] = nValue1;
					m_DamageAttribs[4].nValue[1] = nValue2;
					m_DamageAttribs[4].nValue[2] = nValue3;
					m_nDamageAttribsNum ++;
					break;
				case magic_fatallystrike_p:
					m_DamageAttribs[5].nAttribType = i;
					m_DamageAttribs[5].nValue[0] = nValue1;
					m_DamageAttribs[5].nValue[1] = nValue2;
					m_DamageAttribs[5].nValue[2] = nValue3;
					m_nDamageAttribsNum ++;
					break;
				case magic_steallife_p:
					m_DamageAttribs[6].nAttribType = i;
					m_DamageAttribs[6].nValue[0] = nValue1;
					m_DamageAttribs[6].nValue[1] = nValue2;
					m_DamageAttribs[6].nValue[2] = nValue3;
					m_nDamageAttribsNum ++;
					break;
				case magic_stealmana_p:
					m_DamageAttribs[7].nAttribType = i;
					m_DamageAttribs[7].nValue[0] = nValue1;
					m_DamageAttribs[7].nValue[1] = nValue2;
					m_DamageAttribs[7].nValue[2] = nValue3;
					m_nDamageAttribsNum ++;
					break;
				case magic_stealstamina_p:					
					m_DamageAttribs[8].nAttribType = i;
					m_DamageAttribs[8].nValue[0] = nValue1;
					m_DamageAttribs[8].nValue[1] = nValue2;
					m_DamageAttribs[8].nValue[2] = nValue3;
					m_nDamageAttribsNum ++;
					break;
				case magic_physicsdamage_v:
				case magic_physicsenhance_p:
					m_DamageAttribs[9].nAttribType = i;
					m_DamageAttribs[9].nValue[0] = nValue1;
					m_DamageAttribs[9].nValue[1] = nValue2;
					m_DamageAttribs[9].nValue[2] = nValue3;
					m_nDamageAttribsNum ++;
					break;
				case magic_colddamage_v:
					m_DamageAttribs[10].nAttribType = i;
					m_DamageAttribs[10].nValue[0] = nValue1;
					m_DamageAttribs[10].nValue[1] = nValue2;
					m_DamageAttribs[10].nValue[2] = nValue3;
					m_nDamageAttribsNum ++;
					break;
				case magic_firedamage_v:
					m_DamageAttribs[11].nAttribType = i;
					m_DamageAttribs[11].nValue[0] = nValue1;
					m_DamageAttribs[11].nValue[1] = nValue2;
					m_DamageAttribs[11].nValue[2] = nValue3;
					m_nDamageAttribsNum ++;
					break;
				case magic_lightingdamage_v:
					m_DamageAttribs[12].nAttribType = i;
					m_DamageAttribs[12].nValue[0] = nValue1;
					m_DamageAttribs[12].nValue[1] = nValue2;
					m_DamageAttribs[12].nValue[2] = nValue3;
					m_nDamageAttribsNum ++;
					break;
				case magic_poisondamage_v:
					m_DamageAttribs[13].nAttribType = i;
					m_DamageAttribs[13].nValue[0] = nValue1;
					m_DamageAttribs[13].nValue[1] = nValue2;
					m_DamageAttribs[13].nValue[2] = nValue3;
					m_nDamageAttribsNum ++;
					break;
				case magic_stun_p:
					m_DamageAttribs[14].nAttribType = i;
					m_DamageAttribs[14].nValue[0] = nValue1;
					m_DamageAttribs[14].nValue[1] = nValue2;
					m_DamageAttribs[14].nValue[2] = nValue3;
					m_nDamageAttribsNum ++;
					break;
				case magic_randmove:
					m_DamageAttribs[15].nAttribType = i;
					m_DamageAttribs[15].nValue[0] = nValue1;
					m_DamageAttribs[15].nValue[1] = nValue2;
					m_DamageAttribs[15].nValue[2] = nValue3;
					m_nDamageAttribsNum ++;
					break;
				}
				//return TRUE;
			}
			switch (i)
			{
				case magic_physicsres_p:
				case magic_poisonres_p:
				case magic_coldres_p:
				case magic_fireres_p:
				case magic_lightingres_p:
					m_bSkillReduceResist = TRUE;
					break;
				case magic_lifereplenish_v:
					m_bSkillLifeReplenish = TRUE;
					break;
				case magic_skill_mintimepercastonhorse_v:
					m_nMinTimePerCastOnHorse = nValue1;
					break;
				case magic_skill_collideevent:
					m_bCollideEvent = (BOOL)nValue1;
					m_nCollideSkillId = nValue3;
					break;
				case magic_skill_vanishedevent:
					m_bVanishedEvent = (BOOL)nValue1;
					m_nVanishedSkillId = nValue3;
					break;
				case magic_skill_startevent:
					m_bStartEvent = (BOOL)nValue1;
					m_nStartSkillId = nValue3;
					break;
				case magic_skill_flyevent:
					m_bFlyingEvent = (BOOL)nValue1;
					m_nFlyEventTime = nValue2;
					m_nFlySkillId = nValue3;
					break;
				case magic_skill_dohurt:
					m_nDoHurtP = nValue1;
					break;
				case magic_skill_bymissle:
					m_bByMissle = nValue1;
					break;
			}
			if (nValue2 == 0 || i == magic_skill_flyevent) 
			{
				m_ImmediateAttribs[m_nImmediateAttribsNum].nAttribType = i;
				m_ImmediateAttribs[m_nImmediateAttribsNum].nValue[0] = nValue1;
				m_ImmediateAttribs[m_nImmediateAttribsNum].nValue[1] = nValue2;
				m_ImmediateAttribs[m_nImmediateAttribsNum].nValue[2] = nValue3;
				m_nImmediateAttribsNum ++;
				return TRUE;
			}
			else
			{
				m_StateAttribs[m_nStateAttribsNum].nAttribType = i;
				m_StateAttribs[m_nStateAttribsNum].nValue[0] = nValue1;
				m_StateAttribs[m_nStateAttribsNum].nValue[1] = nValue2;
				m_StateAttribs[m_nStateAttribsNum].nValue[2] = nValue3;
				m_nStateAttribsNum ++;
				return TRUE;
			}
			
		}
	}
	return FALSE;
}

const char * KSkill::MagicAttrib2String(int MagicAttrib)  const 
{
	return 	g_MagicID2String(MagicAttrib);
}

#ifndef _SERVER
void	KSkill::DrawSkillIcon(int x, int y, int Width, int Height)  
{
	
	if (!m_szSkillIcon[0]) return ;
	
	m_RUIconImage.nType = ISI_T_SPR;
	m_RUIconImage.Color.Color_b.a = 255;
	m_RUIconImage.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
	m_RUIconImage.uImage = 0;
	m_RUIconImage.nISPosition = IMAGE_IS_POSITION_INIT;
	m_RUIconImage.bRenderFlag = 0;
	strcpy(m_RUIconImage.szImage, m_szSkillIcon);
	m_RUIconImage.oPosition.nX = x;
	m_RUIconImage.oPosition.nY = y;
	m_RUIconImage.oPosition.nZ = 0;
	m_RUIconImage.nFrame = 0;
	g_pRepresent->DrawPrimitives(1, &m_RUIconImage, RU_T_IMAGE, 1);

	/*KRUShadow	Shadow;			
	Shadow.Color.Color_dw = 0xFF0000;
	Shadow.oPosition.nX = x;
	Shadow.oPosition.nY = y;// + (Height - Height * nPercent / 100);
	Shadow.oEndPos.nX = x + Width;
	Shadow.oEndPos.nY = y + Height;
	g_pRepresent->DrawPrimitives(1, &Shadow, RU_T_SHADOW , FALSE);*/
}

void	KSkill::GetDesc(unsigned long ulSkillId, unsigned long ulCurLevel, char * pszMsg, int nOwnerIndex,  bool bNextLevelDesc)
{
	
	if (!pszMsg) return;
	if (nOwnerIndex <= 0 )	return ;

	char szTemp[256];
	
	KSkill * pTempSkill = NULL;
	KSkill * pCurSkill = NULL;
	KSkill * pNextSkill = NULL;
	if(ulCurLevel == 0)
	{
		pNextSkill = (KSkill *)g_SkillManager.GetSkill(ulSkillId, 1);
		pTempSkill = pNextSkill;
	}
	else
	{
		pCurSkill = (KSkill *) g_SkillManager.GetSkill(ulSkillId, ulCurLevel);
		pNextSkill = (KSkill *) g_SkillManager.GetSkill(ulSkillId, ulCurLevel + 1);
		pTempSkill = pCurSkill;
	}
	
	int nLevel = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetLevel(ulSkillId);
	int nAddLevel = nAddLevel = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetAddLevel(ulSkillId);
	if (pTempSkill == NULL)
	{
		return;
	}
	
	strcat(pszMsg, "<color=255,255,0>");
	strcat(pszMsg, pTempSkill->m_szName);
	switch(pTempSkill->m_nSeries)
	{
	case series_metal:
		strcat(pszMsg, "(hÖ Kim)");
		break;
	case series_wood:
		strcat(pszMsg, "(hÖ Méc)");
		break;
	case series_water:
		strcat(pszMsg, "(hÖ Thñy)");
		break;
	case series_fire:
		strcat(pszMsg, "(hÖ Háa)");
		break;
	case series_earth:
		strcat(pszMsg, "(hÖ Thæ)");
		break;
	}
	strcat(pszMsg, "\n");
	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Level < pTempSkill->GetSkillReqLevel())
	{
		sprintf(szTemp, "<color=255,255,255>§¼ng cÊp yªu cÇu: %d", pTempSkill->GetSkillReqLevel());
		strcat(pszMsg, szTemp);
	}
	strcat (pszMsg, "\n<color=255,255,255>");	
	
	int nStrL = sprintf(szTemp, "%s", pTempSkill->m_szSkillDesc);
	int offset = 0, nL = 0;

	while(szTemp[offset] != 0)
	{
		if(szTemp[offset] == '<')
		{
			if(szTemp[offset+1] == 'e' &&
				szTemp[offset+2] == 'n' &&
				szTemp[offset+3] == 't' &&
				szTemp[offset+4] == 'e' &&
				szTemp[offset+5] == 'r' &&
				szTemp[offset+6] == '>')
				nL = 0;
			if(szTemp[offset+1] == 'c' &&
				szTemp[offset+2] == 'o' &&
				szTemp[offset+3] == 'l' &&
				szTemp[offset+4] == 'o' &&
				szTemp[offset+5] == 'r')
			{
				if (szTemp[offset+6] == '>')
					nL -= 6;
				else
				{
					int k;
					for(k = 0; k<12; k++)
					{
						if(szTemp[offset+6+k] == '>') 
							break;
					}
					nL -= 6+k;
				}
			}
			if(szTemp[offset+1] == 'b' &&
				szTemp[offset+2] == 'c' &&
				szTemp[offset+3] == 'l' &&
				szTemp[offset+4] == 'r')
			{
				if (szTemp[offset+5] == '>')
					nL -= 5;
				else
				{
					if (szTemp[offset+5] == '=')
					{
						int k;
						for(k = 0; k<12; k++)
						{
							if(szTemp[offset+5+k] == '>') 
								break;
						}
						nL -= 5+k;
					}
				}
			}
		}

		if(nL == 32 && (offset+7) <nStrL)
		{
			memmove(&szTemp[offset+7], &szTemp[offset], nStrL-offset+1);
			memcpy(&szTemp[offset],"<enter>",7);
			offset += 7;
			nStrL += 7;
			nL = 0;
		}
		offset++;
		nL++;
	}
    if(strlen(szTemp) > 255)
    szTemp[255] = 0;
    ::memcpy(szTemp, szTemp, 256);
	if (szTemp[0])
	{
		strcat(pszMsg, szTemp);
		strcat(pszMsg, "\n\n");
	}
	if (!pTempSkill->IsBase())
	{
		KIniFile Ini;
		Ini.Load(GAME_SETTING_FILE_INI);
	
		char cbBuffer[16];
		itoa(pTempSkill->GetAttribType(), cbBuffer, 10);
		Ini.GetString("SkillAttrib", cbBuffer, "", szTemp, sizeof(szTemp));
		if (szTemp[0])
		{
			strcat(pszMsg, szTemp);
			strcat (pszMsg, "\n\n");
		}
	}
	if (!Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.IsBaseSkill(ulSkillId))
	{
		if (nAddLevel > 0)
		{
			if (ulCurLevel)
				sprintf(szTemp, "<color=100,100,255>§¼ng cÊp hiÖn thêi: %d (%d+%d)<color>\n" ,ulCurLevel, nLevel, nAddLevel);
			else
				sprintf(szTemp, "§¼ng cÊp hiÖn thêi: %d\n", ulCurLevel);
		}
		else
			sprintf(szTemp, "§¼ng cÊp hiÖn thêi: %d\n", ulCurLevel);
		strcat(pszMsg, szTemp);
	}

    int nAddSkillDamage = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetAddSkillDamage(ulSkillId); 
    if (nAddSkillDamage) 
    { 
        sprintf(szTemp, "Hç trî tõ c¸c kü n¨ng kh¸c: %d%%", nAddSkillDamage); 
        strcat(pszMsg, szTemp); 
        strcat(pszMsg, "\n"); 
    }

    int nSkillEnhance = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentSkillEnhancePercent; 
    if (nSkillEnhance && 
		(pTempSkill->GetSkillStyle() == SKILL_SS_Missles || 
		pTempSkill->GetSkillStyle() == SKILL_SS_Melee) && 
		pTempSkill->IsTargetEnemy() && 
		!pTempSkill->IsBase()) 
    { 
        sprintf(szTemp, "Trang bÞ gåm cã: %d%%", nSkillEnhance); 
        strcat(pszMsg, szTemp); 
        strcat(pszMsg, "\n"); 
    }
	
	int i = 0;
	
	if (pCurSkill)
	{
		pCurSkill->GetDescAboutLevel(ulSkillId, pszMsg);
	}
	
	if (-2 !=pTempSkill->m_nEquiptLimited)
	{
		strcat(pszMsg, "\n");
		switch(pTempSkill->m_nEquiptLimited)
		{
		case -2:
			g_GameSetting.GetString("WeaponLimit", "F2", "", szTemp, sizeof(szTemp));
			strcat(pszMsg, szTemp);
			break;
		case -1:
			g_GameSetting.GetString("WeaponLimit", "F1", "", szTemp, sizeof(szTemp));
			strcat(pszMsg, szTemp);
			break;
		case 0:
			g_GameSetting.GetString("WeaponLimit", "0", "", szTemp, sizeof(szTemp));
			strcat(pszMsg, szTemp);
			break;
		case 1:
			g_GameSetting.GetString("WeaponLimit", "1", "", szTemp, sizeof(szTemp));
			strcat(pszMsg, szTemp);
			break;
		case 2:
			g_GameSetting.GetString("WeaponLimit", "2", "", szTemp, sizeof(szTemp));
			strcat(pszMsg, szTemp);
			break;
		case 3:
			g_GameSetting.GetString("WeaponLimit", "3", "", szTemp, sizeof(szTemp));
			strcat(pszMsg, szTemp);
			break;
		case 4:
			g_GameSetting.GetString("WeaponLimit", "4", "", szTemp, sizeof(szTemp));
			strcat(pszMsg, szTemp);
			break;
		case 5:
			g_GameSetting.GetString("WeaponLimit", "5", "", szTemp, sizeof(szTemp));
			strcat(pszMsg, szTemp);
			break;
		case 101:
			g_GameSetting.GetString("WeaponLimit", "101", "", szTemp, sizeof(szTemp));
			strcat(pszMsg, szTemp);
			break;	
		case 102:
			g_GameSetting.GetString("WeaponLimit", "102", "", szTemp, sizeof(szTemp));
			strcat(pszMsg, szTemp);
			break;
		case 100:
			g_GameSetting.GetString("WeaponLimit", "100", "", szTemp, sizeof(szTemp));
			strcat(pszMsg, szTemp);
			break;
		}
		strcat(pszMsg, "\n");
	}

	if (pTempSkill->m_nHorseLimited)
	{
		if (-2 == pTempSkill->m_nEquiptLimited)
			strcat(pszMsg, "\n");

		switch(pTempSkill->m_nHorseLimited)
		{
		case 1:
			{
				g_GameSetting.GetString("HorseLimit", "1", "", szTemp, sizeof(szTemp));
				strcat(pszMsg, szTemp);
				strcat(pszMsg, "\n");
			}
			break;
		case 2:
			{
				g_GameSetting.GetString("HorseLimit", "2", "", szTemp, sizeof(szTemp));
				strcat(pszMsg, szTemp);
				strcat(pszMsg, "\n");
			}
			break;
		default:
			break;
		}
	}

	if (bNextLevelDesc)
	{
		if (pNextSkill)
		{
			strcat(pszMsg, "\n<color=255,0,0>§¼ng cÊp kÕ tiÕp\n\n");
			pNextSkill->GetDescAboutLevel(ulSkillId, pszMsg, bNextLevelDesc);
		}
		else
		{
			
		}
	}

}

void KSkill::GetDescAboutLevel(unsigned long ulSkillId, char * pszMsg, BOOL bNextLevel/* = FALSE*/, BOOL bAddSkillDamage/* = FALSE*/, BOOL bEventSkill/* = FALSE*/)
{
	char pszInfo[SZBUFLEN_0];
	if (!Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.IsTempSkill(ulSkillId))
	{
		int nLevel = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetLevel(ulSkillId);
		if (nLevel > 0 && nLevel < MAX_SKILLLEVEL)
		{
			if (m_nIsExpSkill && !bNextLevel && !bAddSkillDamage)
			{ 
				int nExp = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetExp(ulSkillId);
				int nNextExp = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetNextExp(ulSkillId);
				
				if (nNextExp)
				{
					float fPer = (float)(nExp * MAX_PERCENT / nNextExp);
					sprintf(pszInfo, "Møc ®é luyÖn: %0.2f%%", fPer); 
					strcat(pszMsg, pszInfo); 
					strcat(pszMsg, "\n");
				}
			} 
		}
	}
	//×Óµ¯ËæÍæ¼ÒÊôÐÔ¼ÆËã¶ø³ÉµÄÉËº¦
	//KMagicAttrib *DamageAttribs[MAX_MISSLE_DAMAGEATTRIB];
	KMagicAttrib *DamageAttribs = m_DamageAttribs;
	//¸ù¾ÝÍæ¼ÒµÄ»ù±¾ÊôÐÔ£¬È·¶¨×Óµ¯µÄÉËº¦

	if (!bNextLevel && !bAddSkillDamage && !IsBase())
		strcat(pszMsg, "\n");

	for (int i  = 0; i < MAX_MISSLE_DAMAGEATTRIB; i ++) 
	{
		if (!(DamageAttribs + i)->nAttribType) continue; 
		if ((DamageAttribs + i)->nAttribType == magic_seriesdamage_p && !bEventSkill) 
		{ 
			sprintf(pszInfo, "Ngò hµnh t­¬ng kh¾c: %d%%", (DamageAttribs + i)->nValue[0]); 
			strcat(pszMsg, pszInfo); 
			strcat(pszMsg, "\n");
		} 
	}
	int nGetCost = GetSkillCost(NULL);

	if (nGetCost && !bAddSkillDamage)
	{
		switch(m_nSkillCostType)
		{
		case attrib_mana_v:
			sprintf(pszInfo, "Tiªu hao néi lùc: %d\n", nGetCost);
			break;
		case attrib_mana_p:
			nGetCost = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentManaMax * GetSkillCost(NULL) / MAX_PERCENT;
			sprintf(pszInfo, "Tiªu hao néi lùc: %d\n", nGetCost);
			break;
		case attrib_stamina_v:
			sprintf(pszInfo, "Tiªu hao thÓ lùc: %d\n", nGetCost);
			break;
		case attrib_stamina_p:
			nGetCost = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentStaminaMax * GetSkillCost(NULL) / MAX_PERCENT;
			sprintf(pszInfo, "Tiªu hao thÓ lùc: %d\n", nGetCost);
			break;
		case attrib_life_v:
			sprintf(pszInfo, "Tiªu hao sinh lùc: %d\n", nGetCost);
			break;
		case attrib_life_p:
			nGetCost = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLifeMax * GetSkillCost(NULL) / MAX_PERCENT;
			sprintf(pszInfo, "Tiªu hao sinh lùc: %d\n", nGetCost);
			break;
		}
		strcat(pszMsg,pszInfo);
	}
	
	int nAttackRadius = GetAttackRadius();
	if (nAttackRadius && !bAddSkillDamage)
	{
		sprintf(pszInfo,"Ph¹m vi hiÖu qu¶: %d\n", nAttackRadius);
		strcat(pszMsg,pszInfo);
	}
	if (m_szMagicSkillDesc[0])
		strcat(pszMsg, m_szMagicSkillDesc);	

	for (int i  = 0; i < MAX_MISSLE_DAMAGEATTRIB; i ++) 
	{ 
		if (!(DamageAttribs + i)->nAttribType || 
			(DamageAttribs + i)->nAttribType >= magic_ignoredefense_p && 
			(DamageAttribs + i)->nAttribType <= magic_seriesdamage_p) continue;

		char * pszInfo = (char *)g_MagicDesc.GetDesc((DamageAttribs + i));
		if (pszInfo[0])
		{
			strcat(pszMsg, pszInfo); 
			strcat(pszMsg, "\n"); 
		}
	}
	//×´Ì¬¼¼ÄÜÐ§¹û

	if(m_nAppendSkillNum)
	{
		for (int i  = 0; i < m_nAppendSkillNum; i ++)
		{
			if (m_nAppendSkillId[i] <= 0 || m_nAppendSkillId[i] >= MAX_SKILL)
				continue;
			int nSkillLevel = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetCurrentLevel(m_nAppendSkillId[i]);
			if (nSkillLevel > GetSkillLevel())
				nSkillLevel = GetSkillLevel();
			if (nSkillLevel > 0)
			{
				KSkill * pTempSkill;
				pTempSkill = (KSkill *) g_SkillManager.GetSkill(m_nAppendSkillId[i], nSkillLevel);
				if(pTempSkill)
				{
					sprintf(pszInfo, "<color=100,100,255>TÇng %d:<color><color=255,255,0> ", i+2);
					strcat(pszMsg, pszInfo);
					strcat(pszMsg, pTempSkill->GetSkillName());
					strcat(pszMsg, "<color>\n");
					pTempSkill->GetDescAboutLevel(ulSkillId, pszMsg, FALSE, TRUE, TRUE);
				}
			}
		}
	}

	for (int i  = 0; i < m_nStateAttribsNum; i ++)
	{
		if (!m_StateAttribs[i].nAttribType || 
			(m_StateAttribs[i].nAttribType > magic_damage_begin && 
			m_StateAttribs[i].nAttribType < magic_damage_end)) continue;
		char* pszInfo = (char *)g_MagicDesc.GetDesc(&m_StateAttribs[i]);
		if (pszInfo[0])
		{
			strcat(pszMsg, pszInfo); 
			strcat(pszMsg, "\n"); 
		}
	}

	int nNum = 0;
	if(m_nShowEvent)
	{
		for (int i  = 0; i < m_nImmediateAttribsNum; i ++)
		{
			if (m_ImmediateAttribs[i].nAttribType >= magic_skill_collideevent && m_ImmediateAttribs[i].nAttribType <= magic_skill_flyevent && m_ImmediateAttribs[i].nValue[0])
			{
				KSkill * pTempSkill = (KSkill *) g_SkillManager.GetSkill(m_ImmediateAttribs[i].nValue[2], m_nEventSkillLevel);
				if(bEventSkill)
					sprintf(pszInfo, "\n<color=100,100,255>TÇng phô %d:<color><color=255,255,0> ", nNum+2);
				else
					sprintf(pszInfo, "\n<color=100,100,255>TÇng %d:<color><color=255,255,0> ", nNum+2);
				strcat(pszMsg,pszInfo);
				strcat(pszMsg, pTempSkill->GetSkillName());
				strcat(pszMsg, "<color>\n");
				pTempSkill->GetDescAboutLevel(ulSkillId, pszMsg, FALSE, TRUE, TRUE);
				nNum++;

				if(nNum >= m_nShowEvent)
					break;
			}
		}

		if(nNum <= 0)
		{
			if(m_bStartEvent && m_nStartSkillId && m_nEventSkillLevel)
			{
				KSkill * pTempSkill = (KSkill *) g_SkillManager.GetSkill(m_nStartSkillId, m_nEventSkillLevel);
				if(bEventSkill)
					sprintf(pszInfo, "\n<color=100,100,255>TÇng phô %d:<color><color=255,255,0> ", nNum+2);
				else
					sprintf(pszInfo, "\n<color=100,100,255>TÇng %d:<color><color=255,255,0> ", nNum+2);
				strcat(pszMsg,pszInfo);
				strcat(pszMsg, pTempSkill->GetSkillName());
				strcat(pszMsg, "<color>\n");
				pTempSkill->GetDescAboutLevel(ulSkillId, pszMsg, FALSE, TRUE, TRUE);
			}
		}
	}

	nNum = 0;
	if(m_nAddSkillDamageNum > 0)
	{
		for (int i  = 0; i < m_nAddSkillDamageNum; i ++) 
		{ 
			if (m_AddSkillDamage[i].nValue[0] <= 0) continue; 
			KSkill * pTempSkill = (KSkill *) g_SkillManager.GetSkill(m_AddSkillDamage[i].nValue[0], 1);

			if (pTempSkill)
			{
				if (pTempSkill->GetShowAddition())
				{
					if(nNum == 0)
					{
						strcat(pszMsg, "\n"); 
						nNum++;
					}
					sprintf(pszInfo, "Kü n¨ng %s: +%d%%", pTempSkill->GetSkillName(), m_AddSkillDamage[i].nValue[2]);
					strcat(pszMsg, pszInfo); 
					strcat(pszMsg, "\n"); 
				}
			}
		}
	}
}

void KSkill::PlayPreCastSound(BOOL bIsFeMale, int nX, int nY)  const 
{
	char * pSoundFile = NULL;
	
	if (!bIsFeMale)
		pSoundFile = (char *)m_szManPreCastSoundFile;
	else 
		pSoundFile = (char *)m_szFMPreCastSoundFile;
	
	int		nCenterX = 0, nCenterY = 0, nCenterZ = 0;
	
	// »ñµÃÆÁÄ»ÖÐÐÄµãµÄµØÍ¼×ø±ê not end
	g_ScenePlace.GetFocusPosition(nCenterX, nCenterY, nCenterZ);
	KCacheNode * pSoundNode = NULL;
	pSoundNode = (KCacheNode*) g_SoundCache.GetNode(pSoundFile, (KCacheNode*)pSoundNode);
	KWavSound * pWave = (KWavSound*)pSoundNode->m_lpData;
	if (pWave)
	{
		pWave->Play((nX - nCenterX) * 5, (10000 - (abs(nX - nCenterX) + abs(nY - nCenterY))) * Option.GetSndVolume() / 100 - 10000, 0);
	}
}
#endif

//---new
BOOL KSkill::CastStateSkill( int nLauncher, int nParam1, int nParam2, int nWaitTime, BOOL bOverLook) const
{
#ifdef _SERVER
	//ÊÇ±»¶¯¼¼ÄÜÊ±£¬ÊÇ·ñ»¹ÐèÒªÉú³ÉMissleMagicAttribs?
	KMissleMagicAttribsData * pAttribsData = (KMissleMagicAttribsData*)m_StateAttribs;//CreateMissleMagicAttribsData(nLauncher);
	if (m_nStateAttribsNum > 0)
	{
		Npc[nLauncher].SetStateSkillEffect(nLauncher, m_nId, m_ulLevel, (KMagicAttrib *)m_StateAttribs, m_nStateAttribsNum, nWaitTime, bOverLook);
	}
#endif
	return TRUE;
}