#include "KCore.h"
#include "KThiefSkill.h"
#ifndef _SERVER
#include "../../Represent/iRepresent/iRepresentshell.h"
#include "scene/KScenePlaceC.h"
#include "../../Represent/iRepresent/KRepresentUnit.h"
#include "imgref.h"
#include "KMagicDesc.h"
#include "KOption.h"
#include "KMissle.h"
#endif

KThiefSkill ::KThiefSkill()
{
	m_ulAttackRadius = 50;
	m_ulDelayPerCast = 40;
	m_nSkillCostType = attrib_mana_v;
#ifndef _SERVER
	m_szTargetMovie[0] = 0;
	m_szSkillSound[0] = 0;
#endif
	m_nCost = 50;
}

KThiefSkill::~KThiefSkill()
{

}

int		KThiefSkill::GetSkillId()
{
	return 400;
	
}

const char *KThiefSkill::GetSkillName()
{
	return m_szSkillName;
}

int		KThiefSkill::GetSkillStyle()
{
	return SKILL_SS_Thief;
}



void	KThiefSkill::LoadSkillLevelData(unsigned long  ulLevel, int nParam)
{
	
	return;
}

int		KThiefSkill::DoSkill(KNpc * pNpc, int nX, int nY)
{
	_ASSERT(pNpc);
	if (nX != -1 || nY <= 0)
		return 0;
	pNpc->m_Frames.nTotalFrame = pNpc->m_AttackFrame;

#ifndef _SERVER
	if (m_szSkillSpecialFile[0])
		pNpc->m_DataRes.SetSpecialSpr(m_szSkillSpecialFile);
		pNpc->m_ClientDoing = cdo_stand;

		TMissleForShow Show;
		Show.nLauncherIndex = pNpc->m_Index;
		Show.nNpcIndex = nY;
		//KMissle::CreateMissleForShow("\\spr\\skill\\NPC\\mag_npc_¶¾Ë®ÏûÊ§.spr", "30,1,1", "\\sound\\skill\\¿ñÀ×ÕðµØ.wav", &Show);
		KMissle::CreateMissleForShow(m_szTargetMovie, m_szTargetMovieParam, m_szSkillSound, &Show);
#endif
	pNpc->m_Doing = do_special2;
	pNpc->m_ProcessAI = 0;
	pNpc->m_Frames.nCurrentFrame = 0;	
	return 1;
}

#ifdef _SERVER
int KThiefSkill::Cast_Redress(KPlayer * pLaucherPlayer, KPlayer * pTargetPlayer)
{
	_ASSERT(pLaucherPlayer && pTargetPlayer);
	int nTotalMoney = pTargetPlayer->m_ItemList.GetMoney(room_equipment);
	int nLevel1 = Npc[pLaucherPlayer->m_nIndex].m_Level;
	int nLevel2 = Npc[pTargetPlayer->m_nIndex].m_Level;
	int nOdds = (int) (10 - ((int)abs(nLevel1 - nLevel2)) * 0.1);
	int nRand = GetRandomNumber(1, 100);
	char szMsg[100];
	if (nTotalMoney <= 0 ||	nRand < nOdds)
	{
		int nThiefMoney = nTotalMoney * m_nThiefPercent / 100 ;
		pTargetPlayer->Pay(nThiefMoney);
		
		sprintf(szMsg, "LÊy c¾p ng©n l­îng trong ng­êi cña %s vµ ñng hé %d l­îng cho tiÒn ph­¬ng", Npc[pLaucherPlayer->m_nIndex].Name, nThiefMoney );
		KPlayerChat::SendSystemInfo(1, pTargetPlayer->GetPlayerIndex(), "LÊy c¾p thµnh c«ng", (char *) szMsg, strlen(szMsg) );
		
		sprintf(szMsg, "LÊy c¾p thµnh c«ng ng©n l­îng cña %s, ñng hé %d l­îng cho tiÒn ph­¬ng! ", Npc[pTargetPlayer->m_nIndex].Name, nThiefMoney);
		KPlayerChat::SendSystemInfo(1, pLaucherPlayer->GetPlayerIndex(), "LÊy c¾p thµnh c«ng", (char *) szMsg, strlen(szMsg) );
		return TRUE;
	}
	
	sprintf(szMsg, "%s cè lÊy c¾p tiÒn cña b¹n, nh­ng kh«ng thµnh c«ng!", Npc[pLaucherPlayer->m_nIndex].Name);
	KPlayerChat::SendSystemInfo(1, pTargetPlayer->GetPlayerIndex(), "Ch­a lÊy c¾p ®­îc", (char *) szMsg, strlen(szMsg) );
	
	sprintf(szMsg, "B¹n kh«ng lÊy ®­îc ng©n l­îng cña %s!", Npc[pTargetPlayer->m_nIndex].Name);
	KPlayerChat::SendSystemInfo(1, pLaucherPlayer->GetPlayerIndex(), "Ch­a lÊy c¾p ®­îc", (char *) szMsg, strlen(szMsg) );
	return TRUE;
}

int KThiefSkill::Cast_ShowLife(KPlayer * pLauncherPlayer, KPlayer * pTargetPlayer)
{
	_ASSERT(pLauncherPlayer && pTargetPlayer);
	char szMsg[200];
	sprintf(szMsg, "ChØ sè sinh m¹ng hiÖn t¹i cña %s lµ: %d / %d ", Npc[pTargetPlayer->m_nIndex].Name, Npc[pTargetPlayer->m_nIndex].m_CurrentLife, Npc[pTargetPlayer->m_nIndex].m_CurrentLifeMax);
	KPlayerChat::SendSystemInfo(1, pLauncherPlayer->GetPlayerIndex(), "T×nh b¸o ®èi ph­¬ng", (char *) szMsg, strlen(szMsg) );
	return TRUE;
}

int KThiefSkill::Cast_ShowMana(KPlayer * pLauncherPlayer, KPlayer * pTargetPlayer)
{
	_ASSERT(pLauncherPlayer && pTargetPlayer);
	char szMsg[200];
	sprintf(szMsg, "ChØ sè néi lùc hiÖn t¹i cña %s lµ: %d / %d ", Npc[pTargetPlayer->m_nIndex].Name, Npc[pTargetPlayer->m_nIndex].m_CurrentMana, Npc[pTargetPlayer->m_nIndex].m_CurrentManaMax);
	KPlayerChat::SendSystemInfo(1, pLauncherPlayer->GetPlayerIndex(), "T×nh b¸o ®èi ph­¬ng", (char *) szMsg, strlen(szMsg) );
	return TRUE;
}

int KThiefSkill::Cast_ShowMoney(KPlayer * pLauncherPlayer, KPlayer * pTargetPlayer)
{
	_ASSERT(pLauncherPlayer && pTargetPlayer);
	char szMsg[200];
	sprintf(szMsg, "Sè tiÒn %s hiÖn cã: %d l­îng ", Npc[pTargetPlayer->m_nIndex].Name, pTargetPlayer->m_ItemList.GetMoney(room_equipment));
	KPlayerChat::SendSystemInfo(1, pLauncherPlayer->GetPlayerIndex(), "T×nh b¸o ®èi ph­¬ng", (char *) szMsg, strlen(szMsg) );
	return TRUE;
}

int KThiefSkill::Cast_RunScript(KPlayer * pLauncherPlayer, KPlayer * pTargetPlayer)
{
	_ASSERT(pLauncherPlayer && pTargetPlayer);
	if (m_dwParam1)
		pTargetPlayer->ExecuteScript(m_dwParam1, NORMAL_FUNCTION_NAME, pLauncherPlayer->GetPlayerIndex());
	return TRUE;
}
#endif

int KThiefSkill::Cast(KPlayer * pLaucherPlayer, KPlayer * pTargetPlayer)
{

#ifdef _SERVER
	_ASSERT(pLaucherPlayer && pTargetPlayer);
	if (pTargetPlayer && pLaucherPlayer)
	{
		switch(m_nThiefStyle)
		{
		case thief_redress:
			{
				return Cast_Redress(pLaucherPlayer, pTargetPlayer);
			}break;
		case thief_showlife:
			{
				return Cast_ShowLife(pLaucherPlayer, pTargetPlayer);
			}break;
		
		case thief_showmana:
			{
				return Cast_ShowMana(pLaucherPlayer, pTargetPlayer);
			}break;

		case thief_showmoney:
			{
				return Cast_ShowMoney(pLaucherPlayer, pTargetPlayer);
			}break;
		case thief_runscript:
			{
				return Cast_RunScript(pLaucherPlayer, pTargetPlayer);

			}
		default:
			return FALSE;
		}
	}
#endif
	return TRUE;
}

int KThiefSkill::OnSkill(KNpc * pNpc)
{
	_ASSERT(pNpc);
	
	if (!pNpc)
		return 0;
	KPlayer * pLauncherPlayer	= NULL;
	KPlayer * pTargetPlayer		= NULL;
	
#ifndef _SERVER
	pNpc->m_DataRes.SetBlur(FALSE);
#endif
	if (pNpc->m_DesX == -1) 
	{
		if (pNpc->m_DesY <= 0) 
			goto Label_ProcessAI1;
		
		//´ËÊ±¸Ã½ÇÉ«ÒÑ¾­ÎÞÐ§Ê±
		if (Npc[pNpc->m_DesY].m_RegionIndex < 0) 
			goto Label_ProcessAI1;
	}
	
	if (pNpc->m_nPlayerIdx < 0 
		|| pNpc->m_DesY < 0 
		|| pNpc->m_DesY >= MAX_NPC 
#ifdef _SERVER
		|| Npc[pNpc->m_DesY].m_nPlayerIdx < 0
#endif
		)
		goto Label_ProcessAI1;
	
	pLauncherPlayer = &Player[pNpc->m_nPlayerIdx];
	pTargetPlayer = &Player[Npc[pNpc->m_DesY].m_nPlayerIdx];
	
	Cast(pLauncherPlayer, pTargetPlayer);
	
Label_ProcessAI1:
	if (pNpc->m_Frames.nTotalFrame == 0)
	{
		pNpc->m_ProcessAI = 1;
	}
	
	return 1;
	
}


BOOL	KThiefSkill::CanCastSkill  (int nLauncher, int &nParam1, int &nParam2)  const 
{
	if (nParam1 != -1)
		return FALSE;
	
	if ( nParam2 <= 0 || nParam2 >= MAX_NPC) return FALSE;
	NPC_RELATION  Relation = NpcSet.GetRelation(nLauncher, nParam2);

	if (Npc[nParam2].m_Index < 0 || Npc[nLauncher].m_Index < 0) 
		return FALSE;
	
	if (Npc[nParam2].m_Camp == camp_begin || Npc[nParam2].m_Camp == camp_event || Npc[nParam2].m_Camp == camp_audience)
		return FALSE;

#ifdef _SERVER
	if (!Npc[nParam2].IsPlayer() || !Npc[nLauncher].IsPlayer())
		return FALSE;
#endif

#ifndef _SERVER
		int distance = NpcSet.GetDistance(nLauncher, nParam2);
		if (distance > (int)m_ulAttackRadius)
			return FALSE;
#endif
	
	return TRUE;
}

#ifndef _SERVER
void	KThiefSkill::GetDesc(unsigned long ulSkillId, unsigned long ulCurLevel, char * pszMsg, int nOwnerIndex,  bool bNextLevelDesc)
{
	strcpy(pszMsg, m_szSkillDesc);
}
#endif

BOOL	KThiefSkill::LoadSetting(char * szSettingFile)
{
	_ASSERT(szSettingFile);
	KTabFile TabFile ;
	if (!TabFile.Load(szSettingFile))
	{
		g_DebugLog("Can't load %s", szSettingFile);
		return FALSE;
	}

	TabFile.GetInteger(2, "AttackRadius", 100, (int *)&m_ulAttackRadius);
	TabFile.GetInteger(2, "TimePerCast", 100, (int*)&m_ulDelayPerCast);
	TabFile.GetInteger(2, "SkillCostType", 0, (int*)&m_nSkillCostType );
	TabFile.GetInteger(2, "Cost", 50, (int *)&m_nCost);
	TabFile.GetInteger(2, "ThiefPercent", 30, (int *)&m_nThiefPercent);

	TabFile.GetInteger(2, "TargetEnemy", TRUE, &m_bTargetEnemy);
	TabFile.GetInteger(2, "TargetAlly",	 TRUE, &m_bTargetAlly);
	TabFile.GetInteger(2, "TargetObj",	 FALSE, &m_bTargetObj);
	TabFile.GetInteger(2, "ThiefStyle",		0, &m_nThiefStyle);
	TabFile.GetString(2, "SkillName", "", m_szSkillName, sizeof(m_szSkillName));
	TabFile.GetInteger(2, "CostUsePrecent", 1, &m_bUseCostPercent);
	
	if (m_nThiefStyle == thief_runscript)
	{
		char szScript[200];
		TabFile.GetString(2, "Param1", "", szScript, sizeof(szScript));
		strlwr(szScript);
		m_dwParam1 =g_FileName2Id(szScript);
	}
	else
	{
		TabFile.GetInteger(2, "Param1", 0, (int*)&m_dwParam1);
	}

#ifndef _SERVER
	TabFile.GetString(2, "Desc", "", m_szSkillDesc, sizeof(m_szSkillDesc));
	TabFile.GetString(2, "SkillIcon", "\\spr\\skill\\Í¼±ê\\Í¨ÓÃ.spr", m_szSkillIcon, sizeof(m_szSkillIcon));
	TabFile.GetString(2, "Movie", "", m_szSkillSpecialFile, sizeof(m_szSkillSpecialFile));
	TabFile.GetString(2, "TargetMovie", "", m_szTargetMovie, sizeof(m_szTargetMovie));
	TabFile.GetString(2, "TargetMovieInfo", "1,1,1", m_szTargetMovieParam, sizeof(m_szTargetMovieParam));
	TabFile.GetString(2, "SkillSound", "", m_szSkillSound, sizeof(m_szSkillSound));
#endif
	return TRUE;

}

int		KThiefSkill::GetSkillCost(void * pParam) const
{
	if (m_bUseCostPercent)
	{
		_ASSERT(pParam);
		if (!pParam)
			return 10000;

		KNpc * pNpc = (KNpc*)pParam;
		int nValue = 10000;
		if (pNpc->m_Index > 0)
		{
			switch(m_nSkillCostType)
			{
			case	attrib_mana_v:
			case	attrib_mana_p:
				{
					nValue = pNpc->m_CurrentManaMax;
				}
				break;
			case attrib_stamina_v:
			case attrib_stamina_p:
				{
					nValue = pNpc->m_CurrentStaminaMax;
				}
				break;
			case attrib_life_v:
			case attrib_life_p:
				{
					nValue = pNpc->m_CurrentLifeMax;
				}break;
			default:
				nValue = 10000;
				break;
			}
			
			nValue = m_nCost * nValue / 100;
			return nValue;
		}
		else
		{
			return nValue;
		}
	}
	else
		return m_nCost;
	
	return 10000;
}


#ifndef _SERVER
void	KThiefSkill::DrawSkillIcon(int x, int y, int Width, int Height)  
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
}
#endif
