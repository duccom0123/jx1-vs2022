#include	"KCore.h"
#include	"KNpc.h"
#include	"KPlayer.h"
#include	"KBuySell.h"
#include 	"ImgRef.h"
#include	"KPlayerDef.h"
#include	"KPlayerAI.h"
#include	"KSubWorldSet.h"
#include 	"KMath.h"

#define		INTERVAL_AUTO			1000

static char* szPotionName	= "Tói D­îc PhÈm";
static const char szMagicScriptName[][32] = 
{
	"Tiªn Th¶o Lé",
	"Tiªn Th¶o Lé §Æc BiÖt",
	"N�n Vui v�",
	"N�n B�nh an",
	"N�n May m�n",
	"N�n H�nh ph�c",
	"Song Th�t Th�y",
	"o vn th nh�n to",
	"Lng n bm bm",
	"L�ng ��n ng�i sao",
	"L�ng ��n �ng",
	"L�ng ��n tr�n",
	"L�ng ��n c� ch�p",
	"L�ng ��n k�o qu�n",
};


#ifndef _SERVER
#include	"CoreShell.h"

int KEquipmentArray::FindSame(int i)
{
	int nIdx = 0;
	while(1)
	{
		nIdx = m_Link.GetNext(nIdx);
		if (!nIdx)
			break;

		if (nIdx == i)
			return nIdx;
	}
	return 0;
}

void KPlayerAI::Init()
{
	Release();
}

void KPlayerAI::Release()
{
	m_uTotalTime		 	= 0;
	m_bIsActive				= FALSE;
	m_nLastFightMode		= enumFightNone;
	m_bPriorityUseMouse 	= FALSE;
	m_bFightUseSB 			= FALSE;
	m_bHoldSpaceBar			= FALSE;
	m_nLifeAutoV 			= 0;
	m_nLifeAutoP 			= 0;
	m_nManaAutoV 			= 0;
	m_nManaAutoP 			= 0;
	m_bTPLife 				= FALSE;
	m_nTPLifeV 				= 0;
	m_bTPMana 				= FALSE;
	m_nTPManaV 				= 0;
	m_bTPNotMedicineBlood 	= FALSE;
	m_bTPNotMedicineMana 	= FALSE;
	m_bTPHightMoney 		= FALSE;
	m_nTPHightMoneyV 		= 0;
	m_bTPDamageEquip 		= FALSE;
	m_nTPDamageEquipV 		= 0;	
	m_bTPNotEquipment 		= FALSE;
	m_nTPNotEquipmentV 		= 0;	
	m_bAntiPoison			= FALSE;
	m_bEnchaseExp			= FALSE;
	m_bLifeReplenish		= FALSE;
	m_nLifeReplenishP		= 0;
	m_bInventoryMoney		= FALSE;
	m_bInventoryItem		= FALSE;
	m_bReturnPortal			= FALSE;
	m_nReturnPortalSec		= 0;
	m_nCurReturnPortalSec	= 0;
	m_bFightNear 			= FALSE;
	m_nRadiusAuto 			= 0;
	m_nDistanceAuto 		= 0;
	m_nFightSkill 			= 0;
	m_nFightBack			= 0;
	m_nFightBoss 			= OverLookNothing;
	m_nActiveAuraID1 		= 0;
	m_nActiveAuraID2		= 0;
	m_nTargetIdx 			= 0;
	m_uLifeCountDown		= 0;
	m_uManaCountDown 		= 0;
	m_uPortalCountDown 		= 0;
	m_uAntiPoisonCountDown	= 0;
	m_uEnchaseExpCountDown	= 0;
	m_uOpenMedicineCountDown= 0;
	m_bEatLife 				= FALSE;
	m_bEatMana 				= FALSE;
	m_uLifeTimeUse 			= 0;
	m_uManaTimeUse 			= 0;
	m_nCount_Attack_OverLook = 0;
	m_nLifeOverLook 		= 0;
	m_nCount_Run_OverLook 	= 0;
	m_nObject 				= 0;
	m_bPickFightNone		= FALSE;
	m_bPickMoney 			= FALSE;
	m_bPickNotEquip 		= FALSE;
	m_bPickEquip 			= FALSE;
	m_bFollowPick			= FALSE;
	m_nPickEquipKind 		= enumPickEquipAll;
	m_bFilterEquipment		= FALSE;
	m_bSaveTrash 			= FALSE;
	m_nSaveTrashPrice 		= 0;
	m_bSaveJewelry 			= FALSE;
	m_bFollowPeople 		= FALSE;	
	m_szFollowName[0] 		= 0;
	m_nFollowRadius			= 0;
	m_bAttackNpc			= FALSE;
	m_bAutoParty			= FALSE;
	m_bAutoInvite 			= FALSE;
	m_bAutoInviteList 		= FALSE;
	m_bOverTarget			= FALSE;
	m_bFollowTarget 		= FALSE;
	m_bOpenMedicine			= FALSE;
	m_bRepairEquip			= FALSE;
	m_bMoveMps 				= FALSE;
	m_nMoveStep 			= 0;
	m_nMoveCount			= 0;
	m_bSortEquipment 		= FALSE;
	m_nReturnPortalStep		= Step_Training;
	memset(m_nFilterMagic, 0, sizeof(m_nFilterMagic));
	memset(m_szPartyList, 0, sizeof(m_szPartyList));
	memset(m_nMoveMps, 0, sizeof(m_nMoveMps));
	memset(m_nSupportSkills, 0, sizeof(m_nSupportSkills));
	ClearArrayNpcNeast();
	ClearArrayObjectNeast();
	ClearArrayNpcOverLook();
	ClearArrayObjectOverLook();
	ClearArrayTimeNpcOverLook();
	ClearArrayTimeObjectOverLook();
	ClearArrayInvitePlayer();
	ClearArrayTimeInvitePlayer();	
	m_sListEquipment.m_Link.Init(MAX_EQUIPMENT_ITEM);
}

void KPlayerAI::Active()
{
	if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
		return;

	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Doing == do_death || 
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Doing == do_revive)
		return;
		
	if (m_bIsActive)
	{	
		EatItem();
		SwitchAura();
		if (m_bPriorityUseMouse)
		{
			ClearArrayNpcOverLook();
			ClearArrayTimeNpcOverLook();
			//ClearArrayObjectOverLook();
			//ClearArrayTimeObjectOverLook();
			m_nTargetIdx = 0;
			//m_nObject = 0;
			m_nLifeOverLook = 0;
			m_nCount_Run_OverLook = 0;
			m_nCount_Attack_OverLook = 0;

			if (!m_nObject)
			{
				m_nObject = GetNearObjectArray();
				if (m_nObject)
				{
					BOOL _flagOverLook = FALSE;
					for (int i=0; i<MAX_AUTO_SIZE; i++)
					{
						if (m_nArrayObjectOverLook[i] == m_nObject)
						{
							_flagOverLook = TRUE;
							break;
						}
					}
					if (_flagOverLook)
					{
						m_nObject = 0;
						m_nLifeOverLook = 0;
						m_nCount_Run_OverLook = 0;
						m_nCount_Attack_OverLook = 0;
					}
					else
					{
						ItemPos	sItemPos;
						if (Object[m_nObject].m_nKind == Obj_Kind_Item)
						{
							if (FALSE == Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchPosition(
								Object[m_nObject].m_nItemWidth, 
								Object[m_nObject].m_nItemHeight, &sItemPos, true))
							{
								m_nObject = 0;
								m_nLifeOverLook = 0;
								m_nCount_Run_OverLook = 0;
								m_nCount_Attack_OverLook = 0;
							}
						}
					}
				}
			}
			if (m_nObject)
			{
				if (FollowObject(m_nObject))
					return;
			}
		}
		else
		{
			if (IR_GetCurrentTime() - m_uTotalTime > INTERVAL_AUTO)
			{
				if(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_FightMode == m_nLastFightMode)
				{
				}
				else
				{
					m_nLastFightMode = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_FightMode;
					ClearArrayNpcOverLook();
					ClearArrayTimeNpcOverLook();
					ClearArrayObjectOverLook();
					ClearArrayTimeObjectOverLook();
					m_nTargetIdx = 0;
					m_nObject = 0;
					m_nLifeOverLook = 0;
					m_nCount_Run_OverLook = 0;
					m_nCount_Attack_OverLook = 0;
				}
					
				FilterEquip();
				if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_FightMode == enumFightActive)
				{
					if (m_nReturnPortalStep == Step_Training)
					{
					}
					else
						m_nReturnPortalStep = Step_Training;

					if (CastStateSkill())
						return;
				}
				else
				{
					if(ReturnFromPortal() && m_nReturnPortalStep < Step_Return)
						return;
				}
				m_uTotalTime = IR_GetCurrentTime();
			}
			SortEquipment();

			int i = 0;
			int nCountNpcOverLook = 0;
			int nCountObjectOverLook = 0;
			for (i=0; i<MAX_AUTO_SIZE; i++)
			{
				if (m_nArrayNpcOverLook[i] > 0)
				{
					nCountNpcOverLook++;
					if (GetTickCount() - m_nArrayTimeNpcOverLook[i] > MAX_TIME_RESET_OVERLOOK)
					{
						m_nArrayNpcOverLook[i] = 0;
						m_nArrayTimeNpcOverLook[i] = 0;
					}
				}
				if (m_nArrayObjectOverLook[i] > 0)
				{
					nCountObjectOverLook++;
					if (GetTickCount() - m_nArrayTimeObjectOverLook[i] > MAX_TIME_RESET_OVERLOOK)
					{
						m_nArrayObjectOverLook[i] = 0;
						m_nArrayTimeObjectOverLook[i] = 0;
					}
				}
			}

			if (nCountNpcOverLook >= MAX_AUTO_SIZE)
			{
				ClearArrayNpcOverLook();
				ClearArrayTimeNpcOverLook();
			}

			if (nCountObjectOverLook >= MAX_AUTO_SIZE)
			{
				ClearArrayObjectOverLook();
				ClearArrayTimeObjectOverLook();
			}
			
			if (m_bAutoParty && m_bAutoInvite && Player[CLIENT_PLAYER_INDEX].m_cTeam.m_bCreatTeamFlag)
			{
				if (!Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag)
				{
					Player[CLIENT_PLAYER_INDEX].ApplyCreateTeam();
					ClearArrayInvitePlayer();
					ClearArrayTimeInvitePlayer();
				}
				else
					InviteParty();
			}

			if (m_bFollowPeople && m_szFollowName[0])
			{
				int		nNpc, nRegionNo, i;
				nNpc = SubWorld[0].m_Region[Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_RegionIndex].SearchNpcName(m_szFollowName);
				if(nNpc <= 0)
				{
					for (i = 0; i < 8; i++)
					{
						nRegionNo = SubWorld[0].m_Region[Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_RegionIndex].m_nConnectRegion[i];
						if ( nRegionNo < 0)
							continue;
						nNpc = SubWorld[0].m_Region[nRegionNo].SearchNpcName(m_szFollowName);
						if (nNpc > 0)
							break;
					}
				}
				if (nNpc)
				{
					int distance = NpcSet.GetDistance(Player[CLIENT_PLAYER_INDEX].m_nIndex, nNpc);
					if (distance > m_nFollowRadius)
					{
						int nX, nY;
						Npc[nNpc].GetMpsPos(&nX,&nY);
						MoveTo(nX, nY);

						m_nTargetIdx = 0;
						m_nObject = 0;
						m_nLifeOverLook = 0;
						m_nCount_Run_OverLook = 0;
						m_nCount_Attack_OverLook = 0;
						return;
					}
				}
			}

			if (!m_nObject)
			{
				m_nObject = GetNearObjectArray();
				if (m_nObject)
				{
					BOOL _flagOverLook = FALSE;
					for (i=0; i<MAX_AUTO_SIZE; i++)
					{
						if (m_nArrayObjectOverLook[i] == m_nObject)
						{
							_flagOverLook = TRUE;
							break;
						}
					}
					if (_flagOverLook)
					{
						m_nObject = 0;
						m_nLifeOverLook = 0;
						m_nCount_Run_OverLook = 0;
						m_nCount_Attack_OverLook = 0;
					}
					else
					{
						ItemPos	sItemPos;
						if (Object[m_nObject].m_nKind == Obj_Kind_Item)
						{
							if (FALSE == Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchPosition(
								Object[m_nObject].m_nItemWidth, 
								Object[m_nObject].m_nItemHeight, &sItemPos, true))
							{
								m_nObject = 0;
								m_nLifeOverLook = 0;
								m_nCount_Run_OverLook = 0;
								m_nCount_Attack_OverLook = 0;
							}
						}
					}
				}
			}
			if (m_nObject)
			{
				if (FollowObject(m_nObject))
					return;
			}

			if (!m_nTargetIdx || (m_nTargetIdx && Npc[m_nTargetIdx].m_Kind == kind_normal))
			{
				i = GetNearNpcArray(relation_enemy);
				if (i > 0)
				{
					if(!m_nTargetIdx || Npc[i].m_Kind == kind_player)
					{
						m_nTargetIdx = i;

						BOOL _flagOverLook = FALSE;
						for (i=0; i<MAX_AUTO_SIZE; i++)
						{
							if (m_nArrayNpcOverLook[i] == m_nTargetIdx)
							{
								_flagOverLook = TRUE;
								break;
							}
						}
						if (_flagOverLook)
							m_nTargetIdx = 0;

						m_nLifeOverLook = 0;
						m_nCount_Run_OverLook = 0;
						m_nCount_Attack_OverLook = 0;
					}
				}
			}
			
			if (m_nTargetIdx)
			{
				if (FollowAttack(m_nTargetIdx))
					return;
			}
			
			if (m_bMoveMps)
				MoveMps();
		}
	}
}

BOOL KPlayerAI::CheckNpc(int nIndex)
{
	if (nIndex <= 0 || Npc[nIndex].m_SubWorldIndex != Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SubWorldIndex || 
		Npc[nIndex].m_RegionIndex < 0 || 
	   (Npc[nIndex].m_Kind == kind_player && (Npc[nIndex].m_FightMode == enumFightNone || m_bFollowPeople)) || 
	   (Npc[nIndex].m_Kind == kind_normal && 
	   (Npc[nIndex].m_btSpecial && m_nFightBoss == OverLookAllBoss) || 
	   (Npc[nIndex].m_btSpecial == npc_blue && m_nFightBoss == OverLookBlueBoss) || 
	   (Npc[nIndex].m_btSpecial == npc_gold && m_nFightBoss == OverLookGoldBoss)) || 
	   (Npc[nIndex].m_Kind == kind_normal && !m_bAttackNpc) || 
		Npc[nIndex].m_CurrentLife <= 0 || 
		Npc[nIndex].m_CurrentLifeMax <= 0 || 
		Npc[nIndex].m_HideState.nTime > 0 || 
		Npc[nIndex].m_Doing == do_death || 
		Npc[nIndex].m_Doing == do_revive)
	{
		m_nTargetIdx = 0;
		m_nLifeOverLook = 0;
		m_nCount_Run_OverLook = 0;
		m_nCount_Attack_OverLook = 0;
		return TRUE;
	}
	return FALSE;
}

BOOL KPlayerAI::AddNpc2Array(int nRelation)
{
	int nRangeX = 0;
	int	nRangeY = 0;
	int	nSubWorldIdx = 0;
	int	nRegion = 0;
	int	nMapX = 0;
	int	nMapY = 0;
	if (m_nRadiusAuto <= 0)
		return FALSE;
	else
	{
		nRangeX = m_nRadiusAuto;
		nRangeY = m_nRadiusAuto;
	}

	nSubWorldIdx = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SubWorldIndex;
	nRegion = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_RegionIndex;
	nMapX = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_MapX;
	nMapY = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_MapY;

	int	nRet;
	int	nRMx, nRMy, nSearchRegion;

	nRangeX = nRangeX / SubWorld[nSubWorldIdx].m_nCellWidth;
	nRangeY = nRangeY / SubWorld[nSubWorldIdx].m_nCellHeight;	

	ClearArrayNpcNeast();

	for (int i = 0; i < nRangeX; i++)
	{
		for (int j = 0; j < nRangeY; j++)
		{
			if ((i * i + j * j) > nRangeX * nRangeX)
				continue;

			nRMx = nMapX + i;
			nRMy = nMapY + j;
			nSearchRegion = nRegion;
			if (nRMx < 0)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[2];
				nRMx += SubWorld[nSubWorldIdx].m_nRegionWidth;
			}
			else if (nRMx >= SubWorld[nSubWorldIdx].m_nRegionWidth)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[6];
				nRMx -= SubWorld[nSubWorldIdx].m_nRegionWidth;
			}
			if (nSearchRegion == -1)
				continue;
			if (nRMy < 0)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[4];
				nRMy += SubWorld[nSubWorldIdx].m_nRegionHeight;
			}
			else if (nRMy >= SubWorld[nSubWorldIdx].m_nRegionHeight)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[0];
				nRMy -= SubWorld[nSubWorldIdx].m_nRegionHeight;
			}
			if (nSearchRegion == -1)
				continue;
			nRet = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, Player[CLIENT_PLAYER_INDEX].m_nIndex, nRelation);
			if (Npc[nRet].m_HideState.nTime > 0)
				nRet = 0;
			if (nRet > 0)
			{
				if (m_nFightBack)
				{
					if (Npc[nRet].m_Kind == kind_player && UseItem(TownPortal))
					{
						m_uPortalCountDown = IR_GetCurrentTime();
						return FALSE;
					}
				}
				if(Npc[nRet].m_Kind == kind_player)
				{
					int k = 0;
					for (k=0; k < (MAX_AUTO_SIZE/2); k++)
					{
						if (m_nArrayNpcNeast[k] == 0)
						{
							m_nArrayNpcNeast[k] = nRet;
							break;
						}
					}
					if(k == (MAX_AUTO_SIZE/2))
						return TRUE;
				}
				else if(Npc[nRet].m_Kind == kind_normal)
				{
					int k = 0;
					for (k=(MAX_AUTO_SIZE/2); k < MAX_AUTO_SIZE; k++)
					{
						if (m_nArrayNpcNeast[k] == 0)
						{
							m_nArrayNpcNeast[k] = nRet;
							break;
						}
					}
					if(k == MAX_AUTO_SIZE)
						return TRUE;
				}
			}
			nRMx = nMapX - i;
			nRMy = nMapY + j;
			nSearchRegion = nRegion;
			if (nRMx < 0)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[2];
				nRMx += SubWorld[nSubWorldIdx].m_nRegionWidth;
			}
			else if (nRMx >= SubWorld[nSubWorldIdx].m_nRegionWidth)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[6];
				nRMx -= SubWorld[nSubWorldIdx].m_nRegionWidth;
			}
			if (nSearchRegion == -1)
				continue;
			if (nRMy < 0)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[4];
				nRMy += SubWorld[nSubWorldIdx].m_nRegionHeight;
			}
			else if (nRMy >= SubWorld[nSubWorldIdx].m_nRegionHeight)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[0];
				nRMy -= SubWorld[nSubWorldIdx].m_nRegionHeight;
			}
			if (nSearchRegion == -1)
				continue;		
			nRet = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, Player[CLIENT_PLAYER_INDEX].m_nIndex, nRelation);
			if (Npc[nRet].m_HideState.nTime > 0)
				nRet = 0;
			if (nRet > 0)
			{
				if (m_nFightBack)
				{
					if (Npc[nRet].m_Kind == kind_player && UseItem(TownPortal))
					{
						m_uPortalCountDown = IR_GetCurrentTime();
						return FALSE;
					}
				}
				if(Npc[nRet].m_Kind == kind_player)
				{
					int k = 0;
					for (k=0; k < (MAX_AUTO_SIZE/2); k++)
					{
						if (m_nArrayNpcNeast[k] == 0)
						{
							m_nArrayNpcNeast[k] = nRet;
							break;
						}
					}
					if(k == (MAX_AUTO_SIZE/2))
						return TRUE;
				}
				else if(Npc[nRet].m_Kind == kind_normal)
				{
					int k = 0;
					for (k=(MAX_AUTO_SIZE/2); k < MAX_AUTO_SIZE; k++)
					{
						if (m_nArrayNpcNeast[k] == 0)
						{
							m_nArrayNpcNeast[k] = nRet;
							break;
						}
					}
					if(k == MAX_AUTO_SIZE)
						return TRUE;
				}
			}
			nRMx = nMapX - i;
			nRMy = nMapY - j;
			nSearchRegion = nRegion;
			if (nRMx < 0)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[2];
				nRMx += SubWorld[nSubWorldIdx].m_nRegionWidth;
			}
			else if (nRMx >= SubWorld[nSubWorldIdx].m_nRegionWidth)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[6];
				nRMx -= SubWorld[nSubWorldIdx].m_nRegionWidth;
			}
			if (nSearchRegion == -1)
				continue;
			if (nRMy < 0)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[4];
				nRMy += SubWorld[nSubWorldIdx].m_nRegionHeight;
			}
			else if (nRMy >= SubWorld[nSubWorldIdx].m_nRegionHeight)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[0];
				nRMy -= SubWorld[nSubWorldIdx].m_nRegionHeight;
			}
			if (nSearchRegion == -1)
				continue;		
			nRet = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, Player[CLIENT_PLAYER_INDEX].m_nIndex, nRelation);
			if (Npc[nRet].m_HideState.nTime > 0)
				nRet = 0;
			if (nRet > 0)
			{
				if (m_nFightBack)
				{
					if (Npc[nRet].m_Kind == kind_player && UseItem(TownPortal))
					{
						m_uPortalCountDown = IR_GetCurrentTime();
						return FALSE;
					}
				}
				if(Npc[nRet].m_Kind == kind_player)
				{
					int k = 0;
					for (k=0; k < (MAX_AUTO_SIZE/2); k++)
					{
						if (m_nArrayNpcNeast[k] == 0)
						{
							m_nArrayNpcNeast[k] = nRet;
							break;
						}
					}
					if(k == (MAX_AUTO_SIZE/2))
						return TRUE;
				}
				else if(Npc[nRet].m_Kind == kind_normal)
				{
					int k = 0;
					for (k=(MAX_AUTO_SIZE/2); k < MAX_AUTO_SIZE; k++)
					{
						if (m_nArrayNpcNeast[k] == 0)
						{
							m_nArrayNpcNeast[k] = nRet;
							break;
						}
					}
					if(k == MAX_AUTO_SIZE)
						return TRUE;
				}
			}
			nRMx = nMapX + i;
			nRMy = nMapY - j;
			nSearchRegion = nRegion;			
			if (nRMx < 0)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[2];
				nRMx += SubWorld[nSubWorldIdx].m_nRegionWidth;
			}
			else if (nRMx >= SubWorld[nSubWorldIdx].m_nRegionWidth)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[6];
				nRMx -= SubWorld[nSubWorldIdx].m_nRegionWidth;
			}
			if (nSearchRegion == -1)
				continue;
			if (nRMy < 0)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[4];
				nRMy += SubWorld[nSubWorldIdx].m_nRegionHeight;
			}
			else if (nRMy >= SubWorld[nSubWorldIdx].m_nRegionHeight)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[0];
				nRMy -= SubWorld[nSubWorldIdx].m_nRegionHeight;
			}
			if (nSearchRegion == -1)
				continue;
			
			nRet = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, Player[CLIENT_PLAYER_INDEX].m_nIndex, nRelation);
			if (Npc[nRet].m_HideState.nTime > 0)
				nRet = 0;
			if (nRet > 0)
			{
				if (m_nFightBack)
				{
					if (Npc[nRet].m_Kind == kind_player && UseItem(TownPortal))
					{
						m_uPortalCountDown = IR_GetCurrentTime();
						return FALSE;
					}
				}
				if(Npc[nRet].m_Kind == kind_player)
				{
					int k = 0;
					for (k=0; k < (MAX_AUTO_SIZE/2); k++)
					{
						if (m_nArrayNpcNeast[k] == 0)
						{
							m_nArrayNpcNeast[k] = nRet;
							break;
						}
					}
					if(k == (MAX_AUTO_SIZE/2))
						return TRUE;
				}
				else if(Npc[nRet].m_Kind == kind_normal)
				{
					int k = 0;
					for (k=(MAX_AUTO_SIZE/2); k < MAX_AUTO_SIZE; k++)
					{
						if (m_nArrayNpcNeast[k] == 0)
						{
							m_nArrayNpcNeast[k] = nRet;
							break;
						}
					}
					if(k == MAX_AUTO_SIZE)
						return TRUE;
				}
			}
		}
	}
	return TRUE;
}

BOOL KPlayerAI::AddObject2Array()
{
	int nRangeX = 0;
	int	nRangeY = 0;
	int	nSubWorldIdx = 0;
	int	nRegion = 0;
	int	nMapX = 0;
	int	nMapY = 0;

	if (m_nRadiusAuto <= 0)
		return FALSE;
	else
	{
		nRangeX = m_nRadiusAuto;
		nRangeY = m_nRadiusAuto;
	}

	nSubWorldIdx = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SubWorldIndex;
	nRegion = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_RegionIndex;
	nMapX = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_MapX;
	nMapY = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_MapY;

	int	nRet;
	int	nRMx, nRMy, nSearchRegion;

	nRangeX = nRangeX / SubWorld[nSubWorldIdx].m_nCellWidth;
	nRangeY = nRangeY / SubWorld[nSubWorldIdx].m_nCellHeight;	

	ClearArrayObjectNeast();

	for (int i = 0; i < nRangeX; i++)
	{
		for (int j = 0; j < nRangeY; j++)
		{
			if ((i * i + j * j) > nRangeX * nRangeX)
				continue;

			nRMx = nMapX + i;
			nRMy = nMapY + j;
			nSearchRegion = nRegion;
			if (nRMx < 0)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[2];
				nRMx += SubWorld[nSubWorldIdx].m_nRegionWidth;
			}
			else if (nRMx >= SubWorld[nSubWorldIdx].m_nRegionWidth)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[6];
				nRMx -= SubWorld[nSubWorldIdx].m_nRegionWidth;
			}
			if (nSearchRegion == -1)
				continue;
			if (nRMy < 0)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[4];
				nRMy += SubWorld[nSubWorldIdx].m_nRegionHeight;
			}
			else if (nRMy >= SubWorld[nSubWorldIdx].m_nRegionHeight)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[0];
				nRMy -= SubWorld[nSubWorldIdx].m_nRegionHeight;
			}
			if (nSearchRegion == -1)
				continue;
		
			nRet = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].FindObject(nRMx, nRMy, true);
			if (nRet > 0)
			{	
				int k = 0;
				for (k=0; k < MAX_AUTO_SIZE; k++)
				{
					if (m_nArrayObjectNeast[k] == 0)
					{
						m_nArrayObjectNeast[k] = nRet;
						break;
					}
				}
				if(k == MAX_AUTO_SIZE)
					return TRUE;
			}
			nRMx = nMapX - i;
			nRMy = nMapY + j;
			nSearchRegion = nRegion;
			if (nRMx < 0)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[2];
				nRMx += SubWorld[nSubWorldIdx].m_nRegionWidth;
			}
			else if (nRMx >= SubWorld[nSubWorldIdx].m_nRegionWidth)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[6];
				nRMx -= SubWorld[nSubWorldIdx].m_nRegionWidth;
			}
			if (nSearchRegion == -1)
				continue;
			if (nRMy < 0)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[4];
				nRMy += SubWorld[nSubWorldIdx].m_nRegionHeight;
			}
			else if (nRMy >= SubWorld[nSubWorldIdx].m_nRegionHeight)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[0];
				nRMy -= SubWorld[nSubWorldIdx].m_nRegionHeight;
			}
			if (nSearchRegion == -1)
				continue;		

			nRet = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].FindObject(nRMx, nRMy, true);
			if (nRet > 0)
			{	
				int k = 0;
				for (k=0; k < MAX_AUTO_SIZE; k++)
				{
					if (m_nArrayObjectNeast[k] == 0)
					{
						m_nArrayObjectNeast[k] = nRet;
						break;
					}
				}
				if(k == MAX_AUTO_SIZE)
					return TRUE;
			}
			nRMx = nMapX - i;
			nRMy = nMapY - j;
			nSearchRegion = nRegion;
			if (nRMx < 0)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[2];
				nRMx += SubWorld[nSubWorldIdx].m_nRegionWidth;
			}
			else if (nRMx >= SubWorld[nSubWorldIdx].m_nRegionWidth)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[6];
				nRMx -= SubWorld[nSubWorldIdx].m_nRegionWidth;
			}
			if (nSearchRegion == -1)
				continue;
			if (nRMy < 0)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[4];
				nRMy += SubWorld[nSubWorldIdx].m_nRegionHeight;
			}
			else if (nRMy >= SubWorld[nSubWorldIdx].m_nRegionHeight)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[0];
				nRMy -= SubWorld[nSubWorldIdx].m_nRegionHeight;
			}
			if (nSearchRegion == -1)
				continue;
		
			nRet = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].FindObject(nRMx, nRMy, true);
			if (nRet > 0)
			{	
				int k = 0;
				for (k=0; k < MAX_AUTO_SIZE; k++)
				{
					if (m_nArrayObjectNeast[k] == 0)
					{
						m_nArrayObjectNeast[k] = nRet;
						break;
					}
				}
				if(k == MAX_AUTO_SIZE)
					return TRUE;
			}
			nRMx = nMapX + i;
			nRMy = nMapY - j;
			nSearchRegion = nRegion;			
			if (nRMx < 0)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[2];
				nRMx += SubWorld[nSubWorldIdx].m_nRegionWidth;
			}
			else if (nRMx >= SubWorld[nSubWorldIdx].m_nRegionWidth)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[6];
				nRMx -= SubWorld[nSubWorldIdx].m_nRegionWidth;
			}
			if (nSearchRegion == -1)
				continue;
			if (nRMy < 0)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[4];
				nRMy += SubWorld[nSubWorldIdx].m_nRegionHeight;
			}
			else if (nRMy >= SubWorld[nSubWorldIdx].m_nRegionHeight)
			{
				nSearchRegion = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].m_nConnectRegion[0];
				nRMy -= SubWorld[nSubWorldIdx].m_nRegionHeight;
			}
			if (nSearchRegion == -1)
				continue;
					
			nRet = SubWorld[nSubWorldIdx].m_Region[nSearchRegion].FindObject(nRMx, nRMy, true);
			if (nRet > 0)
			{	
				int k = 0;
				for (k=0; k < MAX_AUTO_SIZE; k++)
				{
					if (m_nArrayObjectNeast[k] == 0)
					{
						m_nArrayObjectNeast[k] = nRet;
						break;
					}
				}
				if(k == MAX_AUTO_SIZE)
					return TRUE;
			}
		}
	}
	return TRUE;
}

int KPlayerAI::GetNearNpcArray(int nRelation)
{
	int nRet = 0;
	if(AddNpc2Array(nRelation))
	{
		int distance = 0 ;
		int distanceMin = 0 ;
		BOOL _fg = FALSE;
		int i = 0;
		int j = 0;
		for (i=0; i < MAX_AUTO_SIZE; i++)
		{
			if (m_nArrayNpcNeast[i] > 0)
			{
				BOOL _findOverLook = FALSE;
				for (j=0; j < MAX_AUTO_SIZE; j++)
				{
					if (m_nArrayNpcNeast[i] == m_nArrayNpcOverLook[j])
					{
						_findOverLook = TRUE;
						break;
					}
				}
				if (_findOverLook == TRUE)
					continue;
				if (CheckNpc(m_nArrayNpcNeast[i]))		
					continue;
				int nX, nY;
				Npc[m_nArrayNpcNeast[i]].GetMpsPos(&nX, &nY);

				distance = NpcSet.GetDistance(Player[CLIENT_PLAYER_INDEX].m_nIndex, m_nArrayNpcNeast[i]);
				if (FALSE == _fg)
				{
					distanceMin = distance;
					_fg = TRUE;
					nRet = m_nArrayNpcNeast[i];
				}
				if (distance <= distanceMin)
				{
					distanceMin = distance;
					nRet = m_nArrayNpcNeast[i];
				}
			}
		}
	}
	return nRet;
}

int KPlayerAI::GetNearObjectArray()
{
	int nRet = 0 ;
	if (AddObject2Array())
	{
		int distance = 0 ;
		int distanceMin = 0 ;
		BOOL _fg = FALSE;
		for (int i=0; i < MAX_AUTO_SIZE; i++)
		{
			if (m_nArrayObjectNeast[i] != 0)
			{
				BOOL _findOverLook = FALSE;
				for (int j=0; j < MAX_AUTO_SIZE; j++)
				{
					if (Object[m_nArrayObjectNeast[i]].m_nID == m_nArrayObjectOverLook[j])
					{
						_findOverLook = TRUE;
						break;
					}
				}
				if (_findOverLook == TRUE)
					continue;
				if (CheckObject(m_nArrayObjectNeast[i]))
					continue;
				int nX, nY;
				Object[m_nArrayObjectNeast[i]].GetMpsPos(&nX, &nY);

				distance = Object[m_nArrayObjectNeast[i]].GetDistanceSquare(Player[CLIENT_PLAYER_INDEX].m_nIndex);
				if (FALSE == _fg)
				{
					distanceMin = distance ;
					_fg = TRUE;
					nRet = m_nArrayObjectNeast[i];
				}
				if (distance <= distanceMin)
				{
					distanceMin = distance ;
					nRet = m_nArrayObjectNeast[i];
				}
			}
		}
	}
	return nRet;
}

#define PLAYERAUTO_PICKUP_CLIENT_DISTANCE PLAYER_PICKUP_CLIENT_DISTANCE
BOOL KPlayerAI::CheckObject(int nObject)
{
	if (!m_bPickFightNone && Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_FightMode == enumFightNone)
		return TRUE;

	if (nObject > 0 && nObject < MAX_OBJECT)
	{
		if(Object[nObject].m_bOverLook && !m_bPickFightNone && Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_FightMode == enumFightActive)
			return TRUE;

		if(!m_bFollowPick)
		{
			int nX1, nY1, nX2, nY2;
			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetMpsPos(&nX1, &nY1);
			Object[nObject].GetMpsPos(&nX2, &nY2);

			if ((nX1 - nX2) * (nX1 - nX2) + (nY1 - nY2) * (nY1 - nY2) > PLAYERAUTO_PICKUP_CLIENT_DISTANCE * PLAYERAUTO_PICKUP_CLIENT_DISTANCE)
				return FALSE;
		}
		if (Object[nObject].m_nKind == Obj_Kind_Money)
		{
			if (m_bPickMoney)
				return FALSE;
		}
		else if (Object[nObject].m_nKind == Obj_Kind_Item)
		{
			if (Object[nObject].m_nGenre == item_equip)
			{
				if (m_bPickEquip)
				{
					if (Object[nObject].m_nDetailType >= equip_horse && 
						Object[nObject].m_nDetailType < equip_detailnum)
						return FALSE;

					if (m_nPickEquipKind == enumPickEquipAll)
						return FALSE;
					else if (m_nPickEquipKind == enumPickEquipJewelry)
					{
						if (CheckEquip(Object[nObject].m_nDetailType))
							return FALSE;
					}
					else if (m_nPickEquipKind == enumPickEquipHaveMagic)
					{
						if (Object[nObject].m_nColorID > equip_normal && 
							Object[nObject].m_nColorID < equip_number)
							return FALSE;
					}
				}
			}
			else
			{
				if (m_bPickNotEquip)
					return FALSE;
			}
		}
	}
	return TRUE;
}

BOOL KPlayerAI::CheckEquip(BYTE btDetail)
{
	if (btDetail == equip_ring || 
		btDetail == equip_amulet || 
		btDetail == equip_pendant)
		return TRUE;
	return FALSE;
}

BOOL KPlayerAI::CheckEquipMagic(int nIdx)
{
	if (nIdx <= 0 || nIdx >= MAX_ITEM)
		return FALSE;

	int i, j;
	for (i = 0; i < MAX_AUTO_FILTERL; i++)
	{
		if (m_nFilterMagic[i][0])
		{
			for (j = 0; j < MAX_ITEM_MAGICATTRIB; j++)
			{
				if (Item[nIdx].m_aryMagicAttrib[j].nAttribType == m_nFilterMagic[i][0] && 
					Item[nIdx].m_aryMagicAttrib[j].nValue[0] >= m_nFilterMagic[i][1])
					return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL KPlayerAI::FollowAttack(int i)
{
	if ( CheckNpc(i) )
	{
		return FALSE;
	}
	
	if (m_bFightUseSB && !m_bHoldSpaceBar)
	{
		return FALSE;
	}

	if (m_nCount_Attack_OverLook >= MAX_TIME_OVERLOOK || m_nCount_Run_OverLook >= MAX_TIME_OVERLOOK)
	{
		for (int j=0; j < MAX_AUTO_SIZE; j++)
		{
			if (m_nArrayNpcOverLook[j] == 0)
			{
				m_nArrayNpcOverLook[j] = i;
				m_nArrayTimeNpcOverLook[j] = GetTickCount();
				m_nTargetIdx = 0;
				m_nLifeOverLook = 0;
				m_nCount_Run_OverLook = 0;
				m_nCount_Attack_OverLook = 0;
				return FALSE;
			}
		}
	}

	int nIdx = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.FindSame(Player[CLIENT_PLAYER_INDEX].GetLeftSkill());
	if (m_nFightSkill)
		nIdx = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.FindSame(m_nFightSkill);
	if (nIdx)
	{
		if(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ActiveSkillID == nIdx)
		{
		}
		else
			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetActiveSkill(nIdx);
	}

	int distance = NpcSet.GetDistance(Player[CLIENT_PLAYER_INDEX].m_nIndex, i);	
	int distanceskill = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentAttackRadius;

	if (m_bFightNear)
	{
		distanceskill = m_nDistanceAuto;
		if (distanceskill > Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentAttackRadius)
			distanceskill = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentAttackRadius;
	}

	if (distance <= distanceskill && distance <= m_nRadiusAuto)
	{
		KSkill * pSkill = (KSkill *) g_SkillManager.GetSkill(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ActiveSkillID, 1);
		if (pSkill)
		{
			if (pSkill->GetHorseLimit())
			{
				switch(pSkill->GetHorseLimit())
				{
				case 1:
					if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_bRideHorse)
						SendClientCmdRide();
					break;
				case 2:
					if (!Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_bRideHorse)
						SendClientCmdRide();
					break;
				}
			}
			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendCommand(do_skill, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ActiveSkillID, -1, i);
			SendClientCmdSkill(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ActiveSkillID, -1, Npc[i].m_dwID);
			if (m_nLifeOverLook == Npc[i].m_CurrentLife)
			{
				if (!m_bOverTarget && Npc[i].m_Kind== kind_normal)
					m_nCount_Attack_OverLook++;
			}
			else
				m_nCount_Attack_OverLook = 0;

			m_nLifeOverLook = Npc[i].m_CurrentLife;
		}
		m_nCount_Run_OverLook = 0;
		return TRUE;
	}
	else
	{
		if (m_bFollowTarget)
		{
			int nDesX, nDesY;
			Npc[i].GetMpsPos(&nDesX, &nDesY);
			MoveTo(nDesX, nDesY);
			if (!m_bOverTarget/* && Npc[i].m_Kind== kind_normal*/)
				m_nCount_Run_OverLook++;
			return TRUE;
		}
	}
	return FALSE;
}

void KPlayerAI::SetSortEquipment(BOOL bFlag)
{
	if (m_bIsActive)
	{
		for (int i = MAX_EQUIPMENT_ITEM - 1; i > 0 ; i--)
		{
			m_sListEquipment.m_Link.Remove(i);
		}
		m_bSortEquipment = bFlag;
	}
}

void KPlayerAI::SortEquipment()
{
	if (m_bSortEquipment)
	{
		int nHand = Player[CLIENT_PLAYER_INDEX].m_ItemList.Hand();
		if (nHand)
		{
			ItemPos	P;
			if ( FALSE == Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchPosition(Item[nHand].GetWidth(), Item[nHand].GetHeight(), &P) )		
			{
				m_bSortEquipment = FALSE;
				return;
			}		
			Player[CLIENT_PLAYER_INDEX].MoveItem(P, P);
			if (!m_sListEquipment.FindSame(nHand))
			{
				m_sListEquipment.m_Link.Insert(nHand);
			}				
		}
		else
		{
			ItemPos	P;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			
			if (pItem)
			{
				if (pItem->nPlace == pos_equiproom)
				{
					if (!m_sListEquipment.FindSame(pItem->nIdx))
					{
						P.nPlace = pItem->nPlace;
						P.nX = pItem->nX;
						P.nY = pItem->nY;
						Player[CLIENT_PLAYER_INDEX].MoveItem(P, P);
						return;
					}
				}
			}

			while(pItem) 
			{ 
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem)
				{
					if (pItem->nPlace == pos_equiproom)
					{
						if (!m_sListEquipment.FindSame(pItem->nIdx))
						{
							P.nPlace = pItem->nPlace;
							P.nX = pItem->nX;
							P.nY = pItem->nY;
							Player[CLIENT_PLAYER_INDEX].MoveItem(P, P);
							return;
						}
					}
				}
			}
			m_bSortEquipment = FALSE;
		}
	}
}

void KPlayerAI::FilterEquip()
{
	if(m_bSortEquipment)
		return;
	if (Player[CLIENT_PLAYER_INDEX].m_bLockState)
		return;
		
	if (m_bFilterEquipment)
	{
		int nHand = Player[CLIENT_PLAYER_INDEX].m_ItemList.Hand();
		if (nHand)
		{
			PLAYER_THROW_AWAY_ITEM_COMMAND	sThrow;
			sThrow.ProtocolType = c2s_playerthrowawayitem;
			if (g_pClient)
				g_pClient->SendPackToServer(&sThrow, sizeof(PLAYER_THROW_AWAY_ITEM_COMMAND));			
		}
		else
		{
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			
			if (pItem)
			{
				if ((pItem->nPlace == pos_equiproom || pItem->nPlace == pos_equiproomex) && 
					Item[pItem->nIdx].GetGenre() == item_equip)
				{
					BOOL bMatch = TRUE;
					if(Item[pItem->nIdx].GetDetailType() < equip_horse)
					{
						if (Item[pItem->nIdx].GetLock()->IsLock() || 
							Item[pItem->nIdx].GetLockDrop() || 
							Item[pItem->nIdx].m_CommonAttrib.nItemNature)
						{
							bMatch = FALSE;
						}
						else if (m_bSaveTrash && Item[pItem->nIdx].GetSalePrice() >= m_nSaveTrashPrice)
						{
							bMatch = FALSE;
						}
						else if (m_bSaveJewelry && CheckEquip(Item[pItem->nIdx].GetDetailType()))
						{
							bMatch = FALSE;
						}
						else
						{
							if (CheckEquipMagic(pItem->nIdx))
								bMatch = FALSE;
						}
					}
					if (bMatch)
					{
						ItemPos	P;
						P.nPlace = pItem->nPlace;
						P.nX = pItem->nX;
						P.nY = pItem->nY;
						Player[CLIENT_PLAYER_INDEX].MoveItem(P, P);
						return;
					}
				}
			}

			while(pItem) 
			{ 
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem)
				{
					if ((pItem->nPlace == pos_equiproom || pItem->nPlace == pos_equiproomex) && 
						Item[pItem->nIdx].GetGenre() == item_equip)
					{
						BOOL bMatch = TRUE;
						if(Item[pItem->nIdx].GetDetailType() < equip_horse)
						{
							if (Item[pItem->nIdx].GetLock()->IsLock() || 
								Item[pItem->nIdx].GetLockDrop() || 
								Item[pItem->nIdx].m_CommonAttrib.nItemNature)
							{
								bMatch = FALSE;
							}
							else if (m_bSaveTrash && Item[pItem->nIdx].GetSalePrice() >= m_nSaveTrashPrice)
							{
								bMatch = FALSE;
							}
							else if (m_bSaveJewelry && CheckEquip(Item[pItem->nIdx].GetDetailType()))
							{
								bMatch = FALSE;
							}
							else
							{
								if (CheckEquipMagic(pItem->nIdx))
									bMatch = FALSE;
							}
						}
						if (bMatch)
						{
							ItemPos	P;
							P.nPlace = pItem->nPlace;
							P.nX = pItem->nX;
							P.nY = pItem->nY;
							Player[CLIENT_PLAYER_INDEX].MoveItem(P, P);
							return;
						}
					}
				}
			}
		}
	}
}

BOOL KPlayerAI::InventoryItem()
{
	int nHand = Player[CLIENT_PLAYER_INDEX].m_ItemList.Hand();
	if (nHand)
	{
		POINT pPos;
		for(int nRoom = room_repository; nRoom <= room_repository+Player[CLIENT_PLAYER_INDEX].m_btRepositoryNum && nRoom <= room_repository5; nRoom++)
		{
			if(Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Room[nRoom].FindRoom(Item[nHand].GetWidth(), Item[nHand].GetHeight(), &pPos))
			{
				ItemPos	P;
				P.nPlace = pos_repositoryroom+nRoom-room_repository;
				P.nX = pPos.x;
				P.nY = pPos.y;
				Player[CLIENT_PLAYER_INDEX].MoveItem(P, P);
				return FALSE;
			}
		}
		return TRUE;
	}
	else
	{
		ItemPos	P;
		PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
		
		if (pItem)
		{
			if (pItem->nPlace == pos_equiproom || pItem->nPlace == pos_equiproomex)
			{
				BOOL bMatch = FALSE;
				if (Item[pItem->nIdx].m_CommonAttrib.nItemNature)
					bMatch = TRUE;
				else if(Item[pItem->nIdx].m_CommonAttrib.nItemGenre == item_task)
					bMatch = TRUE;
				else if (m_bSaveTrash && Item[pItem->nIdx].GetSalePrice() >= m_nSaveTrashPrice)
					bMatch = TRUE;
				else if (m_bSaveJewelry && CheckEquip(Item[pItem->nIdx].GetDetailType()))
					bMatch = TRUE;
				else
				{
					if (CheckEquipMagic(pItem->nIdx))
						bMatch = TRUE;
				}
				if (bMatch)
				{
					POINT pPos;
					for(int nRoom = room_repository; nRoom <= room_repository+Player[CLIENT_PLAYER_INDEX].m_btRepositoryNum && nRoom <= room_repository5; nRoom++)
					{
						if(Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Room[nRoom].FindRoom(Item[pItem->nIdx].GetWidth(), Item[pItem->nIdx].GetHeight(), &pPos))
						{
							ItemPos	P;
							P.nPlace = pItem->nPlace;
							P.nX = pItem->nX;
							P.nY = pItem->nY;
							Player[CLIENT_PLAYER_INDEX].MoveItem(P, P);
							return FALSE;
						}
					}
					return TRUE;
				}
			}
		}

		while(pItem) 
		{ 
			pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
			if (pItem)
			{
				if (pItem->nPlace == pos_equiproom)
				{
					BOOL bMatch = FALSE;
					if (Item[pItem->nIdx].m_CommonAttrib.nItemNature)
						bMatch = TRUE;
					else if(Item[pItem->nIdx].m_CommonAttrib.nItemGenre == item_task)
						bMatch = TRUE;
					else if (m_bSaveTrash && Item[pItem->nIdx].GetSalePrice() >= m_nSaveTrashPrice)
						bMatch = TRUE;
					else if (m_bSaveJewelry && CheckEquip(Item[pItem->nIdx].GetDetailType()))
						bMatch = TRUE;
					else
					{
						if (CheckEquipMagic(pItem->nIdx))
							bMatch = TRUE;
					}
					if (bMatch)
					{
						POINT pPos;
						for(int nRoom = room_repository; nRoom <= room_repository+Player[CLIENT_PLAYER_INDEX].m_btRepositoryNum && nRoom <= room_repository5; nRoom++)
						{
							if(Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Room[nRoom].FindRoom(Item[pItem->nIdx].GetWidth(), Item[pItem->nIdx].GetHeight(), &pPos))
							{
								ItemPos	P;
								P.nPlace = pItem->nPlace;
								P.nX = pItem->nX;
								P.nY = pItem->nY;
								Player[CLIENT_PLAYER_INDEX].MoveItem(P, P);
								return FALSE;
							}
						}
						return TRUE;
					}
				}
			}
		}
	}
	return TRUE;
}

void KPlayerAI::SwitchAura()
{
	if (m_bIsActive)
	{
		if (m_nActiveAuraID1 && Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].IsNpcSkillExist(m_nActiveAuraID1) &&  
			m_nActiveAuraID2 && Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].IsNpcSkillExist(m_nActiveAuraID2))
		{
			if(KSG_GetCurSec() % 2 == 0)
				Player[CLIENT_PLAYER_INDEX].SetRightSkill((int)m_nActiveAuraID1);
			else
				Player[CLIENT_PLAYER_INDEX].SetRightSkill((int)m_nActiveAuraID2);
		}
	}
}

void KPlayerAI::EatItem()
{	
	if(m_bEatLife)
	{
		if ((m_nLifeAutoP && (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLife * MAX_PERCENT / Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLifeMax) < m_nLifeAutoP) || 
			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLife < m_nLifeAutoV)
		{
			if (IR_GetCurrentTime() - m_uLifeCountDown > m_uLifeTimeUse)
			{
				if(UseItem(Blood))
					m_uLifeCountDown = IR_GetCurrentTime();

			}
		}
	}
	if(m_bEatMana)
	{
		if ((m_nManaAutoP && (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentMana * MAX_PERCENT / Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentManaMax) < m_nManaAutoP) || 
			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentMana < m_nManaAutoV)
		{
			if (IR_GetCurrentTime() - m_uManaCountDown > m_uManaTimeUse)
			{
				if(UseItem(Mana))
					m_uManaCountDown = IR_GetCurrentTime();
			}
		}
	}
	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_FightMode)
	{
		bool bTPFlag = false;
		if ((m_bTPNotMedicineBlood && Player[CLIENT_PLAYER_INDEX].m_ItemList.CountCommonItem(NATURE_NORMAL, item_medicine, medicine_blood) <= 0 && 
			 Player[CLIENT_PLAYER_INDEX].m_ItemList.CountCommonItem(NATURE_NORMAL, item_medicine, medicine_both) <= 0) || 
			(m_bTPNotMedicineMana && Player[CLIENT_PLAYER_INDEX].m_ItemList.CountCommonItem(NATURE_NORMAL, item_medicine, medicine_mana) <= 0 && 
			 Player[CLIENT_PLAYER_INDEX].m_ItemList.CountCommonItem(NATURE_NORMAL, item_medicine, medicine_both) <= 0)
			 )
		{
			if (m_bOpenMedicine)
			{
				if (IR_GetCurrentTime() - m_uOpenMedicineCountDown > MAX_TIME_SPACE)
				{
					if(UseItem(MecidineBag))
						m_uOpenMedicineCountDown = IR_GetCurrentTime();
				}
				else
					bTPFlag = true;
			}
			else
				bTPFlag = true;
		}

		if ((m_bTPLife && Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLife < m_nTPLifeV) || 
			(m_bTPMana && Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentMana < m_nTPManaV) || 
			(m_bTPHightMoney && Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipmentMoney() > MONEY_FLOOR && Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipmentMoney() / MONEY_FLOOR > m_nTPHightMoneyV) || 
			(m_bTPDamageEquip && Player[CLIENT_PLAYER_INDEX].m_ItemList.HaveDamageItem(m_nTPDamageEquipV))
			)
		{
			bTPFlag = true;
		}
		if (m_bTPNotEquipment)
		{
			switch (m_nTPNotEquipmentV)
			{
				case enumRoomNotEnough1:
					if (Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchEquipment(1,1) == FALSE)
						bTPFlag = true;
					break;
				case enumRoomNotEnough2:
					if (Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchEquipment(1,2) == FALSE && 
						Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchEquipment(2,1) == FALSE)
						bTPFlag = true;
					break;		
				case enumRoomNotEnough4:
					if (Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchEquipment(2,2) == FALSE)
						bTPFlag = true;
					break;
				case enumRoomNotEnough6:
					if (Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchEquipment(2,3) == FALSE)
						bTPFlag = true;
					break;						
			}
		}
		if (bTPFlag && IR_GetCurrentTime() - m_uPortalCountDown > MAX_TIME_SPACE)
		{
			if(UseItem(TownPortal))
				m_uPortalCountDown = IR_GetCurrentTime();
		}
	}
	if (m_bAntiPoison && Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_PoisonState.nTime)
	{
		if (IR_GetCurrentTime() - m_uAntiPoisonCountDown > MAX_TIME_SPACE)
		{
			if(UseItem(AntiPoison))
				m_uAntiPoisonCountDown = IR_GetCurrentTime();
		}
	}
	if (m_bEnchaseExp && Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentExpEnhance <= 0)
	{
		if (IR_GetCurrentTime() - m_uEnchaseExpCountDown > MAX_TIME_SPACE)
		{
			if(UseItem(EnchaseExp))
				m_uEnchaseExpCountDown = IR_GetCurrentTime();
		}
	}
}


void KPlayerAI::MoveTo(int nX, int nY)
{
	int nMapX, nMapY;
	int nDestX, nDestY, dX, dY, dZ;
	if (Player[CLIENT_PLAYER_INDEX].m_nSendMoveFrames >= defMAX_PLAYER_SEND_MOVE_FRAME)
	{
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetMpsPos(&nMapX, &nMapY);
		dX = nMapX & 0x1F;
		dY = nMapY & 0x1F;
		nDestX = nX + 0x10 - dX;
		nDestY = nY + 0x10 - dY;
		if (!Player[CLIENT_PLAYER_INDEX].m_RunStatus)
		{
			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendCommand(do_walk, nDestX, nDestY);
			SendClientCmdWalk(nDestX, nDestY);
		}
		else
		{
			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendCommand(do_run, nDestX, nDestY);
			SendClientCmdRun(nDestX, nDestY);
		}
		Player[CLIENT_PLAYER_INDEX].m_nSendMoveFrames = 0;
	}
}

BOOL KPlayerAI::CastStateSkill()
{
	if(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Doing == do_skill || 
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Doing == do_magic || 
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Doing == do_attack)
		return TRUE;

	for (int i = 0; i < MAX_AUTO_STATESKILL; i ++)
	{
		if (m_nSupportSkills[i])
		{
			ISkill * pSkill = g_SkillManager.GetSkill(m_nSupportSkills[i], 1);

			if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.CanCast(m_nSupportSkills[i], 
				SubWorld[0].m_dwCurrentTime) && 
				Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Cost(pSkill->GetSkillCostType(), pSkill->GetSkillCost(this), TRUE, TRUE) && 
				Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].IsNpcStateExist(m_nSupportSkills[i]) == FALSE && 
				Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].IsNpcSkillExist(m_nSupportSkills[i]))
			{
				Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendCommand(do_skill, m_nSupportSkills[i], -1, Player[CLIENT_PLAYER_INDEX].m_nIndex);
				SendClientCmdSkill(m_nSupportSkills[i], -1, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID);
				return TRUE;
			}
		}
	}

	if (m_bLifeReplenish)
	{
		int nSkillLifeReplenishID = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.FindSkillLifeReplenish();
		ISkill * pSkill = g_SkillManager.GetSkill(nSkillLifeReplenishID, 1);
		if (nSkillLifeReplenishID && 
			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Cost(pSkill->GetSkillCostType(), pSkill->GetSkillCost(this), TRUE, TRUE))
		{
			if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLife * MAX_PERCENT / Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLifeMax < m_nLifeReplenishP)
			{
				Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendCommand(do_skill, nSkillLifeReplenishID, -1, Player[CLIENT_PLAYER_INDEX].m_nIndex);
				SendClientCmdSkill(nSkillLifeReplenishID, -1, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID);
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL KPlayerAI::FollowObject(int nObject)
{
	if ( CheckObject(nObject) )
	{
		m_nObject = 0;
		return FALSE;
	}

	int nX1, nY1, nX2, nY2;
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetMpsPos(&nX1, &nY1);
	Object[nObject].GetMpsPos(&nX2, &nY2);

	if ((nX1 - nX2) * (nX1 - nX2) + (nY1 - nY2) * (nY1 - nY2) < PLAYERAUTO_PICKUP_CLIENT_DISTANCE * PLAYERAUTO_PICKUP_CLIENT_DISTANCE)
	{
		Player[CLIENT_PLAYER_INDEX].CheckObject(nObject);
		for (int i=0; i<MAX_AUTO_SIZE; i++)
		{
			if (m_nArrayObjectOverLook[i] == 0)
			{
				m_nArrayObjectOverLook[i] = Object[m_nObject].m_nID;
				m_nArrayTimeObjectOverLook[i] = GetTickCount();
				break;
			}
		}
		m_nObject = 0;
		m_nCount_Run_OverLook = 0;
		return FALSE;
	}
	else
	{
		if(m_bFollowPick && !m_bPriorityUseMouse)
		{
			MoveTo(nX2, nY2);
			m_nCount_Run_OverLook++;
			if (m_nCount_Run_OverLook >= MAX_TIME_OVERLOOK * 2)
			{
				for (int i=0; i<MAX_AUTO_SIZE; i++)
				{
					if (m_nArrayObjectOverLook[i] == 0)
					{
						m_nArrayObjectOverLook[i] = Object[nObject].m_nID;
						m_nArrayTimeObjectOverLook[i] = GetTickCount();
						m_nObject = 0;
						nObject = 0;
						break;
					}
				}
				return FALSE;
			}
			return TRUE;
		}
		else
		{
			m_nObject = 0;
			m_nCount_Run_OverLook = 0;
		}
	}
	return FALSE;
}

BOOL KPlayerAI::UseItem(int type)
{
	if(type == Blood ||
		type == Mana)
	{
		if (m_bOpenMedicine && Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_FightMode && 
			(Player[CLIENT_PLAYER_INDEX].m_ItemList.CountCommonItem(NATURE_NORMAL, item_medicine, medicine_blood) <= 0 || 
			Player[CLIENT_PLAYER_INDEX].m_ItemList.CountCommonItem(NATURE_NORMAL, item_medicine, medicine_mana) <= 0) && 
			Player[CLIENT_PLAYER_INDEX].m_ItemList.CountCommonItem(NATURE_NORMAL, item_medicine, medicine_both) <= 0)
		{
			if (IR_GetCurrentTime() - m_uOpenMedicineCountDown > MAX_TIME_SPACE)
			{
				if(UseItem(MecidineBag))
				{
					m_uOpenMedicineCountDown = IR_GetCurrentTime();
					return TRUE;
				}
			}
		}
	}

	ItemPos    Pos;
	PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
	
	if (pItem)
	{
		if (pItem->nPlace == pos_equiproom || 
			pItem->nPlace == pos_immediacy)
		{
			switch(type)
			{
			case Blood:
				if (Item[pItem->nIdx].GetGenre() == item_medicine)
				{
					if (Item[pItem->nIdx].GetDetailType() == medicine_blood || 
						Item[pItem->nIdx].GetDetailType() == medicine_both)
					{
						Pos.nPlace = pItem->nPlace;
						Pos.nX = pItem->nX;
						Pos.nY = pItem->nY;
						Player[CLIENT_PLAYER_INDEX].ApplyUseItem(pItem->nIdx, Pos);
						return TRUE;
					}
				}
				break;
			case Mana:
				if (Item[pItem->nIdx].GetGenre() == item_medicine)
				{
					if (Item[pItem->nIdx].GetDetailType() == medicine_mana || 
						Item[pItem->nIdx].GetDetailType() == medicine_both)
					{
						Pos.nPlace = pItem->nPlace;
						Pos.nX = pItem->nX;
						Pos.nY = pItem->nY;
						Player[CLIENT_PLAYER_INDEX].ApplyUseItem(pItem->nIdx, Pos);
						return TRUE;
					}
				}
				break;
			case TownPortal:
				if (Item[pItem->nIdx].GetGenre() == item_townportal)
				{
					if (Item[pItem->nIdx].GetDetailType() != townportal_special)
					{
						Pos.nPlace = pItem->nPlace;
						Pos.nX = pItem->nX;
						Pos.nY = pItem->nY;
						Player[CLIENT_PLAYER_INDEX].ApplyUseItem(pItem->nIdx, Pos);
						return TRUE;
					}
				}
				break;
			case TownPortal_l:
				if (Item[pItem->nIdx].GetGenre() == item_townportal)
				{
					if (Item[pItem->nIdx].GetDetailType() == townportal_special)
					{
						Pos.nPlace = pItem->nPlace;
						Pos.nX = pItem->nX;
						Pos.nY = pItem->nY;
						Player[CLIENT_PLAYER_INDEX].ApplyUseItem(pItem->nIdx, Pos);
						return TRUE;
					}
				}
				break;
			case AntiPoison:
				if (Item[pItem->nIdx].GetGenre() == item_medicine)
				{
					if (Item[pItem->nIdx].GetDetailType() == medicine_antipoison)
					{
						Pos.nPlace = pItem->nPlace;
						Pos.nX = pItem->nX;
						Pos.nY = pItem->nY;
						Player[CLIENT_PLAYER_INDEX].ApplyUseItem(pItem->nIdx, Pos);
						return TRUE;
					}
				}
				break;
			case EnchaseExp:
				{
				for(int i=0; i < sizeof(szMagicScriptName)/sizeof(szMagicScriptName[0]);i++)
				{
					if (strcmp(Item[pItem->nIdx].GetName(), szMagicScriptName[i]) == 0)
					{
						Pos.nPlace = pItem->nPlace;
						Pos.nX = pItem->nX;
						Pos.nY = pItem->nY;
						Player[CLIENT_PLAYER_INDEX].ApplyUseItem(pItem->nIdx, Pos);
						return TRUE;
					}
				}
				}
				break;
			case MecidineBag:
				if (strcmp(Item[pItem->nIdx].GetName(), szPotionName) == 0)
				{
					Pos.nPlace = pItem->nPlace;
					Pos.nX = pItem->nX;
					Pos.nY = pItem->nY;
					Player[CLIENT_PLAYER_INDEX].ApplyUseItem(pItem->nIdx, Pos);
					return TRUE;
				}
				break;
			}
		}
	}
	
	while(pItem) 
	{ 
		pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
		if (pItem)
		{
			if (pItem->nPlace == pos_equiproom || 
				pItem->nPlace == pos_immediacy)
			{
				switch(type)
				{
				case Blood:
					if (Item[pItem->nIdx].GetGenre() == item_medicine)
					{
						if (Item[pItem->nIdx].GetDetailType() == medicine_blood || 
							Item[pItem->nIdx].GetDetailType() == medicine_both)
						{
							Pos.nPlace = pItem->nPlace;
							Pos.nX = pItem->nX;
							Pos.nY = pItem->nY;
							Player[CLIENT_PLAYER_INDEX].ApplyUseItem(pItem->nIdx, Pos);
							return TRUE;
						}
					}
					break;
				case Mana:
					if (Item[pItem->nIdx].GetGenre() == item_medicine)
					{
						if (Item[pItem->nIdx].GetDetailType() == medicine_mana || 
							Item[pItem->nIdx].GetDetailType() == medicine_both)
						{
							Pos.nPlace = pItem->nPlace;
							Pos.nX = pItem->nX;
							Pos.nY = pItem->nY;
							Player[CLIENT_PLAYER_INDEX].ApplyUseItem(pItem->nIdx, Pos);
							return TRUE;
						}
					}
					break;
				case TownPortal:
					if (Item[pItem->nIdx].GetGenre() == item_townportal)
					{
						if (Item[pItem->nIdx].GetDetailType() != townportal_special)
						{
							Pos.nPlace = pItem->nPlace;
							Pos.nX = pItem->nX;
							Pos.nY = pItem->nY;
							Player[CLIENT_PLAYER_INDEX].ApplyUseItem(pItem->nIdx, Pos);
							return TRUE;
						}
					}
					break;
				case TownPortal_l:
					if (Item[pItem->nIdx].GetGenre() == item_townportal)
					{
						if (Item[pItem->nIdx].GetDetailType() == townportal_special)
						{
							Pos.nPlace = pItem->nPlace;
							Pos.nX = pItem->nX;
							Pos.nY = pItem->nY;
							Player[CLIENT_PLAYER_INDEX].ApplyUseItem(pItem->nIdx, Pos);
							return TRUE;
						}
					}
					break;
				case AntiPoison:
					if (Item[pItem->nIdx].GetGenre() == item_medicine)
					{
						if (Item[pItem->nIdx].GetDetailType() == medicine_antipoison)
						{
							Pos.nPlace = pItem->nPlace;
							Pos.nX = pItem->nX;
							Pos.nY = pItem->nY;
							Player[CLIENT_PLAYER_INDEX].ApplyUseItem(pItem->nIdx, Pos);
							return TRUE;
						}
					}
					break;
				case EnchaseExp:
					{
					for(int i=0; i < sizeof(szMagicScriptName)/sizeof(szMagicScriptName[0]);i++)
					{
						if (strcmp(Item[pItem->nIdx].GetName(), szMagicScriptName[i]) == 0)
						{
							Pos.nPlace = pItem->nPlace;
							Pos.nX = pItem->nX;
							Pos.nY = pItem->nY;
							Player[CLIENT_PLAYER_INDEX].ApplyUseItem(pItem->nIdx, Pos);
							return TRUE;
						}
					}
					}
					break;
				case MecidineBag:
					if (strcmp(Item[pItem->nIdx].GetName(), szPotionName) == 0)
					{
						Pos.nPlace = pItem->nPlace;
						Pos.nX = pItem->nX;
						Pos.nY = pItem->nY;
						Player[CLIENT_PLAYER_INDEX].ApplyUseItem(pItem->nIdx, Pos);
						return TRUE;
					}
					break;
				}
			}
		}
	}
	return FALSE;
}

void KPlayerAI::InviteParty()
{
	for (int i=0; i < MAX_AUTO_SIZE; i++)
	{
		if (m_nArrayTimeInvitePlayer[i] > 0)
		{
			m_nArrayTimeInvitePlayer[i]--;
			if (m_nArrayTimeInvitePlayer[i] == 0)
				m_nArrayInvitePlayer[i] = 0;
		}
	}		

	if (g_Team[0].m_nMemNum < g_Team[0].CalcCaptainPower())
	{
		int nCount = NpcSet.GetAroundPlayerForTeamInvite(NULL, 0);
		if(nCount)
		{
			KUiPlayerItem*	m_pNearbyPlayersList = (KUiPlayerItem*)malloc(sizeof(KUiPlayerItem) * nCount);
			NpcSet.GetAroundPlayerForTeamInvite(m_pNearbyPlayersList, nCount);
			for (int i = 0; i < nCount; i++)
			{
				if (m_bAutoInviteList && !CheckPTList(m_pNearbyPlayersList[i].Name))
					continue;
				if (FindSameInvitePlayer(m_pNearbyPlayersList[i].uId))
					continue;
				int j = FindFreeInvitePlayer();
				if (j>=0)
				{
					Player[CLIENT_PLAYER_INDEX].TeamInviteAdd(m_pNearbyPlayersList[i].uId);
					m_nArrayInvitePlayer[j] = m_pNearbyPlayersList[i].uId;
					m_nArrayTimeInvitePlayer[j] = INTERVAL_AUTO;
				}
			}
			free (m_pNearbyPlayersList);
		}
	}
}

BOOL KPlayerAI::CheckPTList(const char *Name)
{
	if (!Name || !Name[0])
		return TRUE;
		
	for (int i = 0; i < MAX_AUTO_LIST; i ++)
	{
		if (m_szPartyList[i][0] && strcmp(m_szPartyList[i], Name) == 0)
			return TRUE;
	}
	return FALSE;
}

BOOL KPlayerAI::MoveMps()
{
	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_FightMode == enumFightActive)
	{
		if (m_nMoveStep >= m_nMoveCount)
			m_nMoveStep = 0;

		if (!m_nMoveMps[m_nMoveStep][0] || 
			(m_nMoveMps[m_nMoveStep][0] != SubWorld[0].m_SubWorldID))
		{
			m_nMoveStep++;
			return FALSE;
		}
		int nMapX, nMapY, dX, dY, dZ;
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetMpsPos(&nMapX, &nMapY);
		dX = nMapX-m_nMoveMps[m_nMoveStep][1];
		dY = nMapY-m_nMoveMps[m_nMoveStep][2];
		dZ = (int)sqrt((float)dX*dX+(float)dY*dY);
		if (dZ>=64)
			MoveTo(m_nMoveMps[m_nMoveStep][1], m_nMoveMps[m_nMoveStep][2]);
		else
			m_nMoveStep++;
	}
	return FALSE;
}

BOOL KPlayerAI::ReturnFromPortal()
{
	BOOL nRet = TRUE;
	switch(m_nReturnPortalStep)
	{
	case Step_Training:
		m_nReturnPortalStep = Step_Repair;
		break;
	case Step_Repair:
		if (m_bRepairEquip)
		{
			for(int i = itempart_head; i < itempart_horse; i++)
			{
				int nIdx = Player[CLIENT_PLAYER_INDEX].m_ItemList.m_EquipItem[i];
				if(nIdx && Item[nIdx].GetRepairPrice())
				{
					if (Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipmentMoney() < Item[nIdx].GetRepairPrice())
						continue;
					SendClientCmdRepair(Item[nIdx].GetID());
				}
			}
		}
		m_nReturnPortalStep = Step_InventoryMoney;
		break;
	case Step_InventoryMoney:
		if(m_bInventoryMoney)
			Player[CLIENT_PLAYER_INDEX].m_ItemList.ExchangeMoney(room_equipment, room_repository, Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipmentMoney());
		m_nReturnPortalStep = Step_InventoryItem;
		break;
	case Step_InventoryItem:
		if(m_bInventoryItem)
		{
			if(InventoryItem())
			{
				m_nCurReturnPortalSec = KSG_GetCurSec() + m_nReturnPortalSec;
				m_nReturnPortalStep = Step_Return;
			}
		}
		else
		{
			m_nCurReturnPortalSec = KSG_GetCurSec() + m_nReturnPortalSec;
			m_nReturnPortalStep = Step_Return;
		}
		break;
	case Step_Return:
		if(m_bReturnPortal)
		{
			if(KSG_GetCurSec() >= m_nCurReturnPortalSec)
			{
				PLAYER_COMMAND	sRP;
				sRP.ProtocolType = c2s_playercommand;
				sRP.m_wMsgID = enumC2S_PLAYERCOMMAND_ID_RETURN;
				sRP.m_lpBuf = 0;
				sRP.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
				g_pClient->SendPackToServer((BYTE*)&sRP, sRP.m_wLength + 1);
				nRet = FALSE;
			}
		}
		break;
	}
	return nRet;
}
#endif
