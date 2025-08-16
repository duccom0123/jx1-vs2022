#include "KCore.h"

#ifdef _SERVER
#include "KEngine.h"
#include "KSubWorldSet.h"
#include "KSubWorld.h"
#include "KPlayer.h"
#ifndef _STANDALONE
#include "../../../lib/S3DBInterface.h"
#else
#include "S3DBInterface.h"
#endif
#include "KNpc.h"
#include "KItem.h"
#include "KItemList.h"
#include "KItemGenerator.h"
#include "KItemSet.h"
#include "KNpcSet.h"
#include "KPlayerSet.h"
#include "KItemChangeRes.h"
#include <time.h>
//#include "MyAssert.H"
#include "KTaskFuns.h"


// 是否将数据库存取档的数据保存下来以供调试
//#define DEBUGOPT_DB_ROLEDATA_OUT 




KList g_DBMsgList;

int KPlayer::AddDBPlayer(char * szPlayerName, int sex, DWORD * pdwID)
{
	return FALSE;
}

int KPlayer::LoadDBPlayerInfo(BYTE *pPlayerInfo, int &nStep, unsigned int &nParam)
{
	_ASSERT(pPlayerInfo);
	int nRet = 0;
	int nRetValue = 0;
	
	switch(nStep)
	{
	case STEP_BASE_INFO:
		//数据库中玩家基本信息表
		m_pCurStatusOffset = pPlayerInfo;

		if ((nRet = LoadPlayerBaseInfo(pPlayerInfo, m_pCurStatusOffset, nParam)) == 1)
		{
			nRetValue = SendSyncData(nStep, nParam);
			nStep++; 
			nParam = 0;
		}
		else 
		{
			if (nRet == -1)
			{
				nStep++;
				nParam = 0;
				return 0;
			}
			else
			{
				nRetValue = SendSyncData(nStep, nParam);
			}
		}
		break;
	case STEP_FIGHT_SKILL_LIST:
		//************************************************
		// 玩家的战斗技能列表
		if ((nRet = LoadPlayerFightSkillList(pPlayerInfo, m_pCurStatusOffset, nParam)) == 1)
		{
			nRetValue = SendSyncData(nStep, nParam);
			nStep++;
			nParam = 0;
		}
		else
		{
			if (nRet == -1)
			{
				nStep++;
				nParam = 0;
				return 0;
			}
//			else
//			{
//				nRetValue = SendSyncData(nStep, nParam);
//			}
		}
		break;
	case STEP_STATE_SKILL_LIST:
		//*************************************************
		// 玩家的生活技能列表
		if ((nRet = LoadPlayerStateSkillList(pPlayerInfo, m_pCurStatusOffset, nParam)) == 1)
		{
			nRetValue = SendSyncData(nStep, nParam);
			nStep++;
			nParam = 0;
		}
		else
		{
			if (nRet == -1)
			{
				nStep ++;
				nParam = 0;
				return 0;
			}
			else
			{
				nRetValue = SendSyncData(nStep, nParam);
			}
		}
		
		break;
	case STEP_TASK_LIST:
		//*************************************************		
		// 玩家的任务列表
		if ((nRet = LoadPlayerTaskList(pPlayerInfo, m_pCurStatusOffset, nParam)) == 1)
		{
			
			int i = 0;
			for (i  = 0; i < TASKVALUE_MAXWAYPOINT_COUNT; i ++)
			{
				int nWayPoint = m_cTask.GetSaveVal(TASKVALUE_SAVEWAYPOINT_BEGIN + i);
				if (nWayPoint)
				{
					KIndexNode * pNewNode = new KIndexNode;
					pNewNode->m_nIndex = nWayPoint;
					m_PlayerWayPointList.AddTail(pNewNode);
				}
			}
			
			for (i  = 0; i < TASKVALUE_MAXSTATION_COUNT / 2; i ++)
			{
				DWORD Stations = 0;
				if (Stations = (DWORD)m_cTask.GetSaveVal(TASKVALUE_SAVESTATION_BEGIN + i))
				{
					int nStation1 = (int) HIWORD(Stations);
					int nStation2 = (int) LOWORD(Stations);
					
					if (nStation1)
					{
						KIndexNode * pNewNode = new KIndexNode;
						pNewNode->m_nIndex = nStation1;
						m_PlayerStationList.AddTail(pNewNode);
					}
					
					if (nStation2)
					{
						KIndexNode * pNewNode = new KIndexNode;
						pNewNode->m_nIndex = nStation2;
						m_PlayerStationList.AddTail(pNewNode);
					}
				}
			}
			g_TimerTask.LoadTask(this);
			nRetValue = SendSyncData(nStep, nParam);
			nStep++;
			nParam = 0;
		}
		else
		{
			if (nRet == -1)
			{
				nStep ++;
				nParam = 0;
				return 0;
			}
			else
			{
				nRetValue = SendSyncData(nStep, nParam);
			}
		}
		break;
	case STEP_ITEM_LIST:
		//*************************************************
		// 玩家的装备列表
		if ((nRet = LoadPlayerItemList(pPlayerInfo, m_pCurStatusOffset, nParam)) == 1)
		{
			nRetValue = SendSyncData(nStep, nParam);
			nStep++;
			nParam = 0;
		}
		else
		{
			if (nRet == -1)
			{
				nStep ++;
				nParam = 0;
				return 0;
			}
			else
			{
				nRetValue = SendSyncData(nStep, nParam);
			}
		}
		break;
	default:
		nStep = STEP_SYNC_END;
		break;
	}

	if (nStep == STEP_SYNC_END)
		m_PlayerDBLoad = TRUE;

	return nRetValue;
}

int KPlayer::UpdateDBPlayerInfo(BYTE* pPlayerInfo)
{
	//注意，存档取档都必须按照P顺序
	if (!pPlayerInfo)
		return -1;

	//数据库中玩家基本信息表
	SavePlayerBaseInfo(pPlayerInfo);

	// 玩家的战斗技能列表
	SavePlayerFightSkillList(pPlayerInfo);

	SavePlayerStateSkillList(pPlayerInfo);
	// 玩家的任务列表
	SavePlayerTaskList(pPlayerInfo);

	// 玩家的物品列表
	SavePlayerItemList(pPlayerInfo);
	return 1;	
}

int	KPlayer::LoadPlayerBaseInfo(BYTE  * pRoleBuffer, BYTE * &pCurData, unsigned int &nParam)
{
	if (!pRoleBuffer)
	{
		KASSERT(pRoleBuffer);
		return -1;
	}
	
	TRoleData * pRoleData = (TRoleData*)pRoleBuffer;
	
	if (nParam != 0) return -1;
	
	int		nSex;
	int		nLevel;	
#define PLAYER_MALE_NPCTEMPLATEID		-1
#define PLAYER_FEMALE_NPCTEMPLATEID		-2

	nLevel = pRoleData->BaseInfo.ifightlevel;
	nSex = pRoleData->BaseInfo.bSex;
	if (nSex)
		nSex = MAKELONG(nLevel, PLAYER_FEMALE_NPCTEMPLATEID);
	else
		nSex = MAKELONG(nLevel, PLAYER_MALE_NPCTEMPLATEID);

	//登入点
	m_sLoginRevivalPos.m_nSubWorldID = pRoleData->BaseInfo.irevivalid;
	m_sLoginRevivalPos.m_ReviveID = pRoleData->BaseInfo.irevivalx;
	//---------------------------------------------------------------------------------------------
label_retry:
	POINT Pos;
	g_SubWorldSet.GetRevivalPosFromId(m_sLoginRevivalPos.m_nSubWorldID,
		m_sLoginRevivalPos.m_ReviveID,
		&Pos);
	
	m_sLoginRevivalPos.m_nMpsX = Pos.x;
	m_sLoginRevivalPos.m_nMpsY = Pos.y;
		//---------------------------------------------------------------------------------------------
	PLAYER_REVIVAL_POS	tempPos;

	if (pRoleData->BaseInfo.cUseRevive)
	{
		tempPos.m_nSubWorldID = m_sLoginRevivalPos.m_nSubWorldID;
		tempPos.m_ReviveID = m_sLoginRevivalPos.m_ReviveID;
		tempPos.m_nMpsX = m_sLoginRevivalPos.m_nMpsX;
		tempPos.m_nMpsY = m_sLoginRevivalPos.m_nMpsY;
	}
	else
	{
		tempPos.m_nSubWorldID = pRoleData->BaseInfo.ientergameid;
		tempPos.m_nMpsX = pRoleData->BaseInfo.ientergamex;
		tempPos.m_nMpsY = pRoleData->BaseInfo.ientergamey;
	}

	if(g_SubWorldSet.SearchWorld(tempPos.m_nSubWorldID) == -1)
	{
		g_SubWorldSet.GetRevivalPosFromId(SubWorld[0].m_SubWorldID,
			1,
			&Pos);
		tempPos.m_nSubWorldID = SubWorld[0].m_SubWorldID;
		tempPos.m_nMpsX = Pos.x;
		tempPos.m_nMpsY = Pos.y;
	}

	m_nIndex = NpcSet.Add(nSex, 
		g_SubWorldSet.SearchWorld(tempPos.m_nSubWorldID),
		tempPos.m_nMpsX,
		tempPos.m_nMpsY);

	if(m_nIndex <= 0) 
	{
		g_DebugLog("[Error!]AddNpc Error DBFuns.cpp");
		if (pRoleData->BaseInfo.cUseRevive)
		{
			return -1;
		}
		else
		{
			pRoleData->BaseInfo.cUseRevive = 1;
			goto label_retry;
		}
	}

	m_sDeathRevivalPos = m_sLoginRevivalPos;

	KNpc* pNpc = &Npc[m_nIndex];
	pNpc->m_Kind = kind_player;
	pNpc->SetPlayerIdx(m_nPlayerIndex);
	pNpc->m_Level = nLevel;

	//玩家信息
	strcpy(pNpc->Name, pRoleData->BaseInfo.szName);
	m_nForbiddenTm = pRoleData->BaseInfo.nForbiddenTm;
	
	m_nAttributePoint = pRoleData->BaseInfo.ileftprop;
	m_nSkillPoint = pRoleData->BaseInfo.ileftfight;

	m_nStrength		= pRoleData->BaseInfo.ipower;
	m_nDexterity	= pRoleData->BaseInfo.iagility;
	m_nVitality		= pRoleData->BaseInfo.iouter;
	m_nEngergy		= pRoleData->BaseInfo.iinside;
	m_nLucky		= pRoleData->BaseInfo.iluck;
	m_cTong.Clear();
	m_cTong.DBSetTongNameID(pRoleData->BaseInfo.dwTongID);
	m_nCurStrength = m_nStrength;
	m_nCurDexterity = m_nDexterity;
	m_nCurVitality = m_nVitality;
	m_nCurEngergy = m_nEngergy;
	pNpc->m_CurrentLucky = m_nLucky;
	this->SetFirstDamage();
	this->SetBaseAttackRating();
	this->SetBaseDefence();

	//玩家等级信息
	m_nExp			= pRoleData->BaseInfo.ifightexp;
	pNpc->m_byTranslife = pRoleData->BaseInfo.ifighttranslife;
	m_nNextLevelExp = PlayerSet.m_cLevelAdd.GetLevelExp(pNpc->m_Level, pNpc->m_byTranslife);
	m_nLeadLevel	= pRoleData->BaseInfo.ileadlevel;
	m_nLeadExp		= pRoleData->BaseInfo.ileadexp;

	//门派信息
	m_cFaction.m_nCurFaction		= (char)pRoleData->BaseInfo.nSect;
	m_cFaction.m_nFirstAddFaction	= (char)pRoleData->BaseInfo.nFirstSect;
	m_cFaction.m_nAddTimes			= pRoleData->BaseInfo.ijoincount;
	pNpc->m_RankID					= pRoleData->BaseInfo.isectrole;
	memcpy(&pNpc->m_ExpandRank, &pRoleData->BaseInfo.RankEx, sizeof(pRoleData->BaseInfo.RankEx));
	if(pNpc->m_ExpandRank.dwLeftTime && (pNpc->m_ExpandRank.dwLeftTime < KSG_GetCurSec()))
		pNpc->m_ExpandRank.Release();
	m_nWorldStat					= pRoleData->BaseInfo.nWorldStat;
	m_nSectStat						= pRoleData->BaseInfo.nSectStat;
	m_nKillPeopleNumber				= pRoleData->BaseInfo.nKillPeopleNumber;
	m_dwEquipExpandTime				= pRoleData->BaseInfo.iexitemrole;
	m_btRepositoryNum					= pRoleData->BaseInfo.iexboxrole;
	m_cTong.m_dwLeaveTime			= pRoleData->BaseInfo.ileavetongtime; 
	m_ImagePlayer   = pRoleData->BaseInfo.ihelmres;
	//m_nExtPoint			= pRoleData->BaseInfo.iextpoint;
	SetExtPoint(m_nExtPoint, 0);
	//现金和贮物箱中的钱
	int nCashMoney = 0;
	int nSaveMoney = 0;
//	this->m_ItemList.Init(GetPlayerIndex());
	nCashMoney		= pRoleData->BaseInfo.imoney;
	nSaveMoney		= pRoleData->BaseInfo.isavemoney;
	m_ItemList.SetMoney(nCashMoney, nSaveMoney,0);

	pNpc->m_Series	= pRoleData->BaseInfo.ifiveprop;
	pNpc->m_Camp	= pRoleData->BaseInfo.iteam;
	
	pNpc->m_nSex	= pRoleData->BaseInfo.bSex;

	pNpc->m_LifeMax	= pRoleData->BaseInfo.imaxlife;
	pNpc->m_StaminaMax = pRoleData->BaseInfo.imaxstamina;
	pNpc->m_ManaMax = pRoleData->BaseInfo.imaxinner;

	pNpc->m_LifeReplenish = PLAYER_LIFE_REPLENISH;
	pNpc->m_ManaReplenish = PLAYER_MANA_REPLENISH;
	pNpc->m_StaminaGain = PLAYER_STAMINA_GAIN;
	pNpc->m_StaminaLoss = PLAYER_STAMINA_LOSS;

	this->SetBaseResistData();
	SetBaseSpeedAndRadius();
	pNpc->RestoreNpcBaseInfo();
	
	pNpc->m_CurrentLife = pRoleData->BaseInfo.icurlife;
	pNpc->m_CurrentMana = pRoleData->BaseInfo.icurinner;
	pNpc->m_CurrentStamina = pRoleData->BaseInfo.icurstamina;

	// PK信息
	m_cPK.SetLockPKState(pRoleData->BaseInfo.cPkStatus, pRoleData->BaseInfo.ilockpkstate);
	m_cPK.SetPKValue(pRoleData->BaseInfo.ipkvalue);
	for (int i = 0; i < MAX_STATTASK; i ++)
		m_cTask.SetSaveVal(i, pRoleData->BaseInfo.istattask[i]);
	

	// 初始化部分数据（这些数据数据库不存储）
	m_BuyInfo.Clear();
	m_cMenuState.Release();
	m_cChat.Release();
	m_cTeam.Release();
	m_cTeam.SetCreatTeamFlag(m_nPlayerIndex, TRUE);
	m_cTeam.SetFreezeTeamFlag(m_nPlayerIndex, TRUE);
	m_nPeapleIdx = 0;
	m_nObjectIdx = 0;
	memset(m_szTaskAnswerFun, 0, sizeof(m_szTaskAnswerFun));
	m_nAvailableAnswerNum = 0;
	Npc[m_nIndex].m_ActionScriptID = 0;
	Npc[m_nIndex].m_TrapScriptID = 0;
	m_nViewEquipTime = 0;

	pNpc->m_Experience = 0;

	pNpc->m_WeaponType = g_ItemChangeRes.GetWeaponRes(0, 0, 0, 0);
	pNpc->m_ArmorType = g_ItemChangeRes.GetArmorRes(0, 0);
	pNpc->m_HelmType = g_ItemChangeRes.GetHelmRes(0, 0);
	pNpc->m_HorseType = g_ItemChangeRes.GetHorseRes(0, 0);

	nParam = 1;
	pCurData = (BYTE *)&pRoleData->pBuffer;
	// 登入游戏时战斗模式
	pNpc->m_FightMode = pRoleData->BaseInfo.cFightMode;
	// 是否是处于跨服务器状态
	if (pRoleData->BaseInfo.cIsExchange)
	{
	}
	return 1;
}

int	KPlayer::LoadPlayerItemList(BYTE * pRoleBuffer , BYTE* &pItemBuffer, unsigned int &nParam)
{
	KASSERT(pRoleBuffer);
	
	int nItemCount = ((TRoleData *)pRoleBuffer)->nItemCount;
	TDBItemData * pItemData = (TDBItemData *)pItemBuffer;
	if (nItemCount == 0) return 1;
	if (nParam != 0)
//		m_ItemList.Init(m_nPlayerIndex);
//	else 
	{
		//如果要求获得的物品号大于实际的物品数量，则退出
		if (nParam >= nItemCount )
			return -1;
	}

	char szSection[100];
	char szSubSection[100];
	char szKey[100];
	int nItemClass;
	int nItemBaseInfoCount ;
	int nItemEfficInfoCount;
	int nItemRequireInfoCount;
	int dwItemID = 0; 
	int nDetailType = 0;
	int nParticularType = 0;
	int nSeries = 0;
	int nLevel = 0;
	int nLocal = 0;
	int nItemX = 0;
	int nItemY = 0;
	int	nLucky = 0;
	int nBegin = nParam;
	int nEnd = nParam + DBLOADPERTIME_ITEM;
	
	if ( nEnd > nItemCount)
		nEnd = nItemCount;

	nParam = nEnd;		

	time_t rawtime;
	struct tm * timeinfo;
	
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	for (int i = nBegin ; i < nEnd; i ++)
	{
		
		KItem NewItem;	
		ZeroMemory(&NewItem, sizeof(KItem));
		
		//物件基本信息**************************************************************
		time_t rawtime;
		struct tm * timeinfo;
		
		time ( &rawtime );
		timeinfo = localtime ( &rawtime );
		
		if (pItemData->iexpire)
		{
			if (pItemData->iexpire < KSG_GetCurSec())
				continue;
		}

		nItemClass	= pItemData->iequipclasscode;
		NewItem.m_CommonAttrib.nItemNature		= pItemData->iequipnaturecode;
		NewItem.m_CommonAttrib.nDetailType		= pItemData->idetailtype;
		NewItem.m_CommonAttrib.nParticularType	= pItemData->iparticulartype;
		NewItem.m_CommonAttrib.nLevel			= pItemData->ilevel;
		NewItem.m_CommonAttrib.nSeries			= pItemData->iseries;
		nItemX									= pItemData->ix;
		nItemY									= pItemData->iy;
		nLocal									= pItemData->ilocal;
		NewItem.m_CommonAttrib.nRow				= pItemData->irow;
		memset(NewItem.m_GeneratorParam.nGeneratorLevel, 0, sizeof(NewItem.m_GeneratorParam.nGeneratorLevel));
		memcpy(NewItem.m_GeneratorParam.nGeneratorLevel, pItemData->iparam, sizeof(NewItem.m_GeneratorParam.nGeneratorLevel));
		
		NewItem.m_GeneratorParam.nVersion			= pItemData->iequipversion;
		NewItem.m_GeneratorParam.uRandomSeed		= pItemData->irandseed;
		NewItem.m_GeneratorParam.nLuck				= pItemData->ilucky;
		BOOL bGetEquiptResult = 0;
		switch(nItemClass)
		{
		case item_equip :			// 装备
			bGetEquiptResult = ItemGen.Gen_ExistEquipment(
					NewItem.m_CommonAttrib.nItemNature,
					NewItem.m_CommonAttrib.nItemNature>=NATURE_GOLD?NewItem.m_CommonAttrib.nRow:NewItem.m_CommonAttrib.nDetailType,
					NewItem.m_CommonAttrib.nParticularType,
					NewItem.m_CommonAttrib.nSeries,
					NewItem.m_CommonAttrib.nLevel, 
					NewItem.m_GeneratorParam.nGeneratorLevel, 
					NewItem.m_GeneratorParam.nLuck,
					NewItem.m_GeneratorParam.nVersion,
					&NewItem);				
			break;
		case item_medicine:			// 药品
				bGetEquiptResult = ItemGen.Gen_Medicine(
					NewItem.m_CommonAttrib.nDetailType,
					NewItem.m_CommonAttrib.nLevel,
					NewItem.m_GeneratorParam.nVersion, 
					&NewItem);
			break;

		case item_event:				// 矿石
			{
				bGetEquiptResult = ItemGen.Gen_Event(NewItem.m_CommonAttrib.nDetailType, 
					&NewItem);				
			}
			break;
		case item_materials:			// 药材
			break;
		case item_task:				// 任务
			{
				bGetEquiptResult = ItemGen.Gen_Quest(NewItem.m_CommonAttrib.nDetailType, &NewItem);
			}
			break;
		case item_townportal:
			{
				bGetEquiptResult = ItemGen.Gen_TownPortal(NewItem.m_CommonAttrib.nDetailType, &NewItem);
			}
			break;
		case item_magicscript:
			{
				bGetEquiptResult = ItemGen.Gen_MagicScript(NewItem.m_CommonAttrib.nDetailType, 
					&NewItem,
					NewItem.m_CommonAttrib.nLevel,
					NewItem.m_CommonAttrib.nSeries,
					NewItem.m_GeneratorParam.nLuck);
			}
			break;
		}
		if(pItemData->idurability != -2)
			NewItem.SetDurability(pItemData->idurability);
		NewItem.GetBackLocal()->Release();
		NewItem.SetExpireTime(pItemData->iexpire);
		NewItem.SetOwner(pItemData->iowner);
		NewItem.SetTradePrice(pItemData->itradeprice);
		KLockItem pLock;
		pLock.nState = pItemData->ilockstate;
		pLock.dwLockTime = pItemData->ilocktime;
		if (pLock.nState ==  LOCK_STATE_UNLOCK)
		{
			if (pLock.dwLockTime <= KSG_GetCurSec())
			{
				pLock.nState = LOCK_STATE_NORMAL;
				pLock.dwLockTime = 0;
			}
		}
		NewItem.SetLock(&pLock);
		NewItem.SetFortune(pItemData->ifortune);
		NewItem.SetLockSell((BOOL)pItemData->ilocksell);
		NewItem.SetLockTrade((BOOL)pItemData->ilocktrade);
		NewItem.SetLockDrop((BOOL)pItemData->ilockdrop);
		NewItem.SetParam(pItemData->iparamv);
		NewItem.SetStackNum(pItemData->istacknum);
		NewItem.SetMantle(pItemData->imantle);
		NewItem.SetFlash(pItemData->iflash);
		NewItem.SetRow(pItemData->irow);
		pItemData ++;
		
		int nIndex = ItemSet.Add(&NewItem);
		
		if (nIndex <= 0) 
		{
			KASSERT(0);
			continue ;
		}

		m_ItemList.Add(nIndex, nLocal, nItemX, nItemY, false);
		
	}

	pItemBuffer	= (BYTE *)pItemData;

	if (nParam >= nItemCount)
		return 1;
	else 
		return 0;
}


int	KPlayer::LoadPlayerFightSkillList(BYTE * pRoleBuffer, BYTE * &pFightBuffer, unsigned int& nParam)
{
	KASSERT(pRoleBuffer);
	int nSkillCount = 0;
	nSkillCount = ((TRoleData*)(pRoleBuffer))->nFightSkillCount;
	
	if (nSkillCount == 0)	
		return 1;
	
	if (nParam >= nSkillCount )	
		return -1;
	
	int nBegin	= nParam;
	int nEnd	= nSkillCount;
	if (nEnd > nSkillCount) 	nEnd = nSkillCount;
	nParam = nEnd;
	TDBSkillData * pSkillData = NULL;
	if (nBegin == 0)
		pSkillData = (TDBSkillData *)((BYTE*)pRoleBuffer + ((TRoleData*)pRoleBuffer)->dwFSkillOffset);
	else
		pSkillData = (TDBSkillData*)pFightBuffer;
	for (int i = nBegin ; i < nEnd; i ++, pSkillData ++ )
	{
		int nSkillId = 0;
		int nSkillLevel = 0;
		int nSkillExp = 0;
		nSkillId = pSkillData->m_nSkillId;
		nSkillLevel = pSkillData->m_nSkillLevel;
		nSkillExp = pSkillData->m_nSkillVal;
		Npc[m_nIndex].m_SkillList.Add(nSkillId, nSkillLevel, nSkillExp);
	}
	pFightBuffer = (BYTE*) pSkillData;

	if (nParam >= nSkillCount) return 1;
	else 
		return 0;
}

int	KPlayer::LoadPlayerStateSkillList(BYTE * pRoleBuffer, BYTE * &pStateBuffer, unsigned int& nParam)
{
	KASSERT(pRoleBuffer);
	int nStateCount = ((TRoleData*)(pRoleBuffer))->nStateSkillCount;
	
	if (nStateCount == 0)
		return 1;

	if (nParam >= nStateCount )		return -1;
	int nBegin	= nParam;
	int nEnd	= nStateCount;
	if (nEnd > nStateCount) 	nEnd = nStateCount;
	nParam = nEnd;

	TDBSkillData * pStateData = (TDBSkillData*)pStateBuffer;
	for (int i = nBegin ; i < nEnd; i ++, pStateData ++ )
	{
		KSkill *pSkill = (KSkill*)g_SkillManager.GetSkill(pStateData->m_nSkillId, pStateData->m_nSkillLevel);
		pSkill->CastStateSkill(m_nIndex, 0, 0, pStateData->m_nSkillVal, TRUE);
	}
	pStateBuffer = (BYTE*) pStateData;

	if (nParam >= nStateCount) return 1;
	else 
		return 0;
}

int	KPlayer::LoadPlayerTaskList(BYTE * pRoleBuffer, BYTE * &pTaskBuffer, unsigned int& nParam)
{
	KASSERT(pRoleBuffer);
	while(m_PlayerStationList.GetHead())
	{
		KIndexNode * pNode = (KIndexNode * ) m_PlayerStationList.GetHead();
		m_PlayerStationList.RemoveHead();
		delete pNode;
	}
	while(m_PlayerWayPointList.GetHead())
	{
		KIndexNode * pNode = (KIndexNode * ) m_PlayerWayPointList.GetHead();
		m_PlayerWayPointList.RemoveHead();
		delete pNode;
	}
	if(nParam == 0)	m_cTask.Release();

	int nTaskCount = 0;
	int nTaskId = 0;
	char szTaskIDKey[100];
	char szTaskValueKey[100];
	nTaskCount = ((TRoleData* )pRoleBuffer)->nTaskCount;

	if (nTaskCount == 0) return 1;
	if (nParam >= nTaskCount ) return -1;
	int nBegin	= nParam;
	int nEnd	= nBegin + DBLOADPERTIME_TASK;
	if (nEnd > nTaskCount) nEnd = nTaskCount;
	nParam = nEnd;
	TDBTaskData * pTaskData = (TDBTaskData*) pTaskBuffer;
	for (int i = nBegin; i < nEnd; i ++ , pTaskData ++)
	{
		nTaskId = pTaskData->m_nTaskId;
		if (nTaskId >= MAX_TASK) 
		{
			KASSERT(0);
			continue;//任务ID超过上限了！
		}
		if (pTaskData->m_szTaskValue[0])
		{
			if(nTaskId == TASKVALUE_BASEDATA_PASSWORD)
				this->SetSavePw((char*)pTaskData->m_szTaskValue, FALSE);
			else
				m_cTask.SetSaveVal(nTaskId, (char*)pTaskData->m_szTaskValue);
		}
	}
	pTaskBuffer = (BYTE*) pTaskData;
	if (nParam >= nTaskCount) return 1;
	else 
	return 0;
}

int	KPlayer::SavePlayerBaseInfo(BYTE * pRoleBuffer)
{
	_ASSERT(pRoleBuffer);
	if (m_nIndex <= 0) return -1;

	KNpc * pNpc = &Npc[m_nIndex];
	TRoleData * pRoleData = (TRoleData*)pRoleBuffer;
	//玩家信息
	DWORD dwRoleTm = pRoleData->BaseInfo.iroletm;
	if (dwRoleTm <=0)
		dwRoleTm = KSG_GetCurSec();
	memset(pRoleData, 0, sizeof(TRoleData));
	pRoleData->bBaseNeedUpdate = 1;
	pRoleData->BaseInfo.iroletm = dwRoleTm;
	strcpy(pRoleData->BaseInfo.szName, Name);
	if (AccountName[0])
		strcpy(pRoleData->BaseInfo.caccname, AccountName);
	pRoleData->BaseInfo.nForbiddenTm = m_nForbiddenTm;
	pRoleData->BaseInfo.ileftprop = m_nAttributePoint;
	pRoleData->BaseInfo.ileftfight = m_nSkillPoint;
	pRoleData->BaseInfo.ipower = m_nStrength;
	pRoleData->BaseInfo.iagility = m_nDexterity;
	pRoleData->BaseInfo.iouter = m_nVitality;
	pRoleData->BaseInfo.iinside	=  m_nEngergy;
	pRoleData->BaseInfo.iluck = m_nLucky;
	
	pRoleData->BaseInfo.ileavetongtime =  m_cTong.m_dwLeaveTime;
	pRoleData->BaseInfo.dwTongID = m_cTong.GetTongNameID();
	strcpy(pRoleData->BaseInfo.itongname, m_cTong.m_szName);
	pRoleData->BaseInfo.itonglevel = m_cTong.m_btLevel;
	pRoleData->BaseInfo.itongmemnum = m_cTong.m_dwMemberNum+m_cTong.m_btManagerNum+m_cTong.m_btDirectorNum;
	pRoleData->BaseInfo.itongeff = m_cTong.m_dwTotalEff;

	//玩家等级信息
	pRoleData->BaseInfo.ifightexp = m_nExp;
	pRoleData->BaseInfo.ifightlevel = pNpc->m_Level;
	pRoleData->BaseInfo.ifighttranslife = pNpc->m_byTranslife;
	
	pRoleData->BaseInfo.ileadlevel = m_nLeadLevel;
	pRoleData->BaseInfo.ileadexp =	m_nLeadExp;

	//门派信息
	pRoleData->BaseInfo.nSect =		m_cFaction.m_nCurFaction;
	pRoleData->BaseInfo.nFirstSect = 	m_cFaction.m_nFirstAddFaction;
	pRoleData->BaseInfo.ijoincount = m_cFaction.m_nAddTimes;
	pRoleData->BaseInfo.isectrole = pNpc->m_RankID;
	memcpy(&pRoleData->BaseInfo.RankEx,	&pNpc->m_ExpandRank, sizeof(pNpc->m_ExpandRank));
	pRoleData->BaseInfo.nWorldStat	= m_nWorldStat;
	pRoleData->BaseInfo.nSectStat	= m_nSectStat;
	pRoleData->BaseInfo.nKillPeopleNumber	= m_nKillPeopleNumber;
	pRoleData->BaseInfo.iexitemrole = m_dwEquipExpandTime;
	pRoleData->BaseInfo.iexboxrole = m_btRepositoryNum;
	pRoleData->BaseInfo.ilockpkstate = m_nLockPKState;
	
	pRoleData->BaseInfo.ihelmres =  m_ImagePlayer;
	
	pRoleData->BaseInfo.iextpoint = m_nExtPoint;
	
	//现金和贮物箱中的钱
	int nCashMoney = 0;
	int nSaveMoney = 0;
	
	nCashMoney  = m_ItemList.GetMoney(room_equipment);
	nSaveMoney	= m_ItemList.GetMoney(room_repository);
	pRoleData->BaseInfo.imoney			= nCashMoney;
	pRoleData->BaseInfo.isavemoney		= nSaveMoney;
	pRoleData->BaseInfo.ifiveprop		= pNpc->m_Series;
	pRoleData->BaseInfo.iteam			= pNpc->m_Camp;
	pRoleData->BaseInfo.bSex			= pNpc->m_nSex;
	pRoleData->BaseInfo.imaxlife		= pNpc->m_LifeMax;
	pRoleData->BaseInfo.imaxstamina		= pNpc->m_StaminaMax;
	pRoleData->BaseInfo.imaxinner		= pNpc->m_ManaMax;
	pRoleData->BaseInfo.icurlife		= pNpc->m_CurrentLife;
	pRoleData->BaseInfo.icurinner		= pNpc->m_CurrentMana;
	pRoleData->BaseInfo.icurstamina		= pNpc->m_CurrentStamina;

	//登入点
	pRoleData->BaseInfo.irevivalid = 	m_sLoginRevivalPos.m_nSubWorldID;
	pRoleData->BaseInfo.irevivalx = 	m_sLoginRevivalPos.m_ReviveID;
	pRoleData->BaseInfo.irevivaly = 	0;

	if (m_bExchangeServer)	// 准备跨服务器：把存盘点设为目标点
	{
		pRoleData->BaseInfo.cUseRevive = 0;
		pRoleData->BaseInfo.ientergameid = m_sExchangePos.m_dwMapID;
		pRoleData->BaseInfo.ientergamex = m_sExchangePos.m_nX;
		pRoleData->BaseInfo.ientergamey = m_sExchangePos.m_nY;
		pRoleData->BaseInfo.cFightMode = (BYTE)pNpc->m_FightMode;
	}
	else if (pNpc->m_SubWorldIndex >= 0 && pNpc->m_RegionIndex >= 0 && pNpc->m_Doing != do_death && pNpc->m_Doing != do_revive)
	{
		pRoleData->BaseInfo.cUseRevive = m_bUseReviveIdWhenLogin;
		pRoleData->BaseInfo.ientergameid = SubWorld[pNpc->m_SubWorldIndex].m_SubWorldID;
		pNpc->GetMpsPos(&pRoleData->BaseInfo.ientergamex, &pRoleData->BaseInfo.ientergamey);
		pRoleData->BaseInfo.cFightMode = (BYTE)pNpc->m_FightMode;
	}
	else
	{
		pRoleData->BaseInfo.cUseRevive = 1;
		pRoleData->BaseInfo.cFightMode = enumFightNone;
		if (pNpc->m_Doing == do_death || pNpc->m_Doing == do_revive)
		{
			pRoleData->BaseInfo.icurlife = pNpc->m_LifeMax;
			pRoleData->BaseInfo.icurinner = pNpc->m_ManaMax;
			pRoleData->BaseInfo.icurstamina = pNpc->m_StaminaMax;
		}
	}

	//PK相关
	pRoleData->BaseInfo.cPkStatus = (BYTE)m_cPK.GetNormalPKState();
	pRoleData->BaseInfo.ipkvalue = m_cPK.GetPKValue();
	pRoleData->dwFSkillOffset = (BYTE * )pRoleData->pBuffer - (BYTE *)pRoleBuffer;
	for (int i = 0; i < MAX_STATTASK; i ++)
		pRoleData->BaseInfo.istattask[i] = m_cTask.GetSaveVal(TASKVALUE_STATTASK_REPUTE+i);
	return 1;
	
}

int	KPlayer::SavePlayerItemList(BYTE * pRoleBuffer)
{
	_ASSERT(pRoleBuffer);

	TRoleData * pRoleData = (TRoleData*) pRoleBuffer;

	TDBItemData * pItemData = (TDBItemData*) ((BYTE*)pRoleData +  pRoleData->dwItemOffset);

	int	nItemCount = 0;
	char szSection[100];
	char szKey[100];

	int nIdx = 0;
	while(1)
	{
		nIdx = m_ItemList.m_UseIdx.GetNext(nIdx);
		if (nIdx == 0 )
			break;
		int nItemIndex = m_ItemList.m_Items[nIdx].nIdx;

		if (Item[nItemIndex].IsTemp())
			continue;

		sprintf(szSection, "%s%d", SECTION_ITEM, nItemCount + 1);
		//玩家基本信息*****************************************************************	
		pItemData->iequipnaturecode =  Item[nItemIndex].m_CommonAttrib.nItemNature;
		pItemData->iequipclasscode =  Item[nItemIndex].m_CommonAttrib.nItemGenre;
		pItemData->iequipcode =  Item[nItemIndex].GetID();
		pItemData->idetailtype =  Item[nItemIndex].m_CommonAttrib.nDetailType;
		pItemData->iparticulartype =  Item[nItemIndex].m_CommonAttrib.nParticularType;
		pItemData->ilevel =  Item[nItemIndex].m_CommonAttrib.nLevel;
		pItemData->iseries =  Item[nItemIndex].m_CommonAttrib.nSeries;
		if (Item[nItemIndex].GetBackLocal()->nPlace >= pos_hand && Item[nItemIndex].GetBackLocal()->nPlace < pos_num)
		{
			pItemData->ilocal =  Item[nItemIndex].GetBackLocal()->nPlace;
			pItemData->ix =  Item[nItemIndex].GetBackLocal()->nX;
			pItemData->iy =  Item[nItemIndex].GetBackLocal()->nY;
		}
		else
		{
			pItemData->ilocal =  m_ItemList.m_Items[nIdx].nPlace;
			pItemData->ix =  m_ItemList.m_Items[nIdx].nX;
			pItemData->iy =  m_ItemList.m_Items[nIdx].nY;
		}
		pItemData->iequipversion = Item[nItemIndex].GetItemParam()->nVersion;
		pItemData->irandseed = Item[nItemIndex].GetItemParam()->uRandomSeed;
		memset(pItemData->iparam, 0, sizeof(pItemData->iparam));
		memcpy(pItemData->iparam, Item[nItemIndex].GetItemParam()->nGeneratorLevel, sizeof(pItemData->iparam));
		pItemData->ilucky = 	 Item[nItemIndex].GetItemParam()->nLuck;
		pItemData->idurability = Item[nItemIndex].GetDurability();
		pItemData->iidentify = 0;
		pItemData->istacknum	= Item[nItemIndex].GetStackNum();
		pItemData->iexpire = Item[nItemIndex].GetExpireTime();
		pItemData->ilockstate = Item[nItemIndex].GetLock()->nState;
		pItemData->ilocktime = Item[nItemIndex].GetLock()->dwLockTime;
		pItemData->iparamv = Item[nItemIndex].GetParam();
		pItemData->ifortune = Item[nItemIndex].GetFortune();
		pItemData->iowner = Item[nItemIndex].GetOwner();
		pItemData->itradeprice = Item[nItemIndex].GetTradePrice();
		pItemData->ilocksell = Item[nItemIndex].m_CommonAttrib.bLockSell;
		pItemData->ilocktrade = Item[nItemIndex].m_CommonAttrib.bLockTrade;
		pItemData->ilockdrop = Item[nItemIndex].m_CommonAttrib.bLockDrop;
		pItemData->imantle = Item[nItemIndex].GetMantle();
		pItemData->iflash = Item[nItemIndex].GetFlash();
		pItemData->irow = Item[nItemIndex].GetRow();
		pItemData++;
		nItemCount ++;
	}
	pRoleData->nItemCount = nItemCount;
	
	pRoleData->dwDataOffset = (BYTE*)pItemData - (BYTE*)pRoleBuffer;
	pRoleData->dwDataLen = (BYTE*)pItemData - (BYTE*)pRoleBuffer;
	return 1;
}

int	KPlayer::SavePlayerFightSkillList(BYTE * pRoleBuffer)
{
	_ASSERT(pRoleBuffer);
	if (m_nIndex <= 0) 
		return FALSE;
	TRoleData * pRoleData = (TRoleData *)pRoleBuffer;
	TDBSkillData * pSkillData = (TDBSkillData *) (pRoleBuffer + pRoleData->dwFSkillOffset);
	int nCount = Npc[m_nIndex].m_SkillList.UpdateDBSkillList((BYTE *)pSkillData);
	if (nCount > 0)
	{
		pRoleData->nFightSkillCount = nCount;
		pRoleData->dwSSkillOffset = (BYTE*)pSkillData - pRoleBuffer + sizeof(TDBSkillData) * nCount;
	}
	else
	{
		pRoleData->nFightSkillCount  = 0;
		pRoleData->dwSSkillOffset = (BYTE*)pSkillData - pRoleBuffer;
	}
	return 1;
}

int	KPlayer::SavePlayerStateSkillList(BYTE * pRoleBuffer)
{
	_ASSERT(pRoleBuffer);
	if (m_nIndex <= 0) 
		return FALSE;
	TRoleData * pRoleData = (TRoleData *)pRoleBuffer;
	TDBSkillData * pStateData = (TDBSkillData *) (pRoleBuffer + pRoleData->dwSSkillOffset);
	int nCount = Npc[m_nIndex].UpdateDBStateList((BYTE *)pStateData);
	if (nCount > 0)
	{
		pRoleData->nStateSkillCount = nCount;
		pRoleData->dwTaskOffset = (BYTE*)pStateData - pRoleBuffer + sizeof(TDBSkillData) * nCount;
	}
	else
	{
		pRoleData->nStateSkillCount  = 0;
		pRoleData->dwTaskOffset = (BYTE*)pStateData - pRoleBuffer;
	}
	return 1;
}

int	KPlayer::SavePlayerTaskList(BYTE * pRoleBuffer)
{
	_ASSERT(pRoleBuffer);
	TRoleData * pRoleData = (TRoleData *) pRoleBuffer;
	int nTaskCount = 0;
	int nTaskId = 0;
	int nTaskDegee = 0;
	char szTaskIDKey[100];
	char szTaskValueKey[100];

	KIndexNode * pNode = (KIndexNode*)m_PlayerStationList.GetHead();
	int n = 0;
	memset(&m_cTask.szSave[TASKVALUE_SAVESTATION_BEGIN], 0, (TASKVALUE_MAXSTATION_COUNT / 2) * sizeof(m_cTask.szSave[TASKVALUE_SAVESTATION_BEGIN]));
	memset(&m_cTask.szSave[TASKVALUE_SAVEWAYPOINT_BEGIN], 0, TASKVALUE_MAXWAYPOINT_COUNT * sizeof(m_cTask.szSave[TASKVALUE_SAVEWAYPOINT_BEGIN]));
	while(pNode)
	{
		if (n >= TASKVALUE_MAXSTATION_COUNT) break;
		DWORD dwValue = m_cTask.GetSaveVal(TASKVALUE_SAVESTATION_BEGIN + n / 2);
		DWORD OrValue = pNode->m_nIndex ;
		OrValue = OrValue << ((n % 2) * 16);
		dwValue = dwValue | OrValue;
		itoa(dwValue, m_cTask.szSave[TASKVALUE_SAVESTATION_BEGIN + n / 2], 10);
		n++;
		pNode = (KIndexNode*)pNode->GetNext();
	}
	n = 0;
	pNode = (KIndexNode*) m_PlayerWayPointList.GetHead();
	while(pNode)
	{
		if (n >= TASKVALUE_MAXWAYPOINT_COUNT) break;
		if (pNode->m_nIndex == 0) continue;
		itoa(pNode->m_nIndex,m_cTask.szSave[TASKVALUE_SAVEWAYPOINT_BEGIN + n],10);
		n++;
		pNode = (KIndexNode*)pNode->GetNext();
	}
	
	g_TimerTask.SaveTask(this);

	TDBTaskData * pTaskData = (TDBTaskData *)(pRoleBuffer + pRoleData->dwTaskOffset);
	for (int i = 0; i < MAX_TASK; i ++)
	{
		if (!m_cTask.szSave[i][0]) continue;
		pTaskData->m_nTaskId = i;
		strcpy(pTaskData->m_szTaskValue, m_cTask.GetSaveStr(i));
		nTaskCount ++;
		pTaskData ++;
	}
		
	pRoleData->nTaskCount = nTaskCount;
	pRoleData->dwItemOffset = (BYTE *)pTaskData - pRoleBuffer;
	return 1;
}
#endif

