//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KCore.cpp
// Date:	2000.08.08
// Code:	Daphnis Wang
// Desc:	Core class
//---------------------------------------------------------------------------
#include "KCore.h"
#include "KEngine.h"
#include "KFilePath.h"
#ifndef _SERVER
#include "KNpcResList.h"
#include "KBmpFile.h"
#include "ImgRef.h"
#include "../../Represent/iRepresent/iRepresentshell.h"
#endif
#include "KItemChangeRes.h"
#include "KNpcSet.h"
#include "KTabFile.h"
#include "KSkills.h"
#include "KPlayerSet.h"
#include "KPlayerTeam.h"
#include "KPlayerChatRoom.h"
#include "KMissleSet.h"
#include "KTongData.h"
#include "KFaction.h"
#include "KGameData.h"
#include "KMath.h"
#include "time.h"
#include "KPlayerTask.h"
#include "KSubWorldSet.h"
#include "KItemGenerator.h"
#include "KObjSet.h"
#include "KItemSet.h"
#include "KNpc.h"
#include "KNpcTemplate.h"
#include "CoreUseNameDef.h"
#include "GameDataDef.h"
#include "KBuySell.h"
#include "KSortScript.h"
#include "LuaFuns.h"

#pragma warning (disable: 4512)
#pragma warning (disable: 4786)

#ifdef _SERVER
#ifndef _STANDALONE
#include "../../../lib/S3DBInterface.h"
#else
#include "S3DBInterface.h"
#endif
#endif

#ifdef _SERVER
extern int g_WayPointPriceUnit;	//WayPoint表格中价格的单位量，WayPoint价格 = 单位量 * 表格数值
extern int g_StationPriceUnit;	//Station表格中价格的单位量，Station价格 = 单位量 * 表格数值
extern int g_DockPriceUnit;
#endif


#ifndef _SERVER
#define CLIENTWEAPONSKILL_TABFILE		"\\settings\\ClientWeaponSkill.txt"	
KTabFile g_ClientWeaponSkillTabFile;

#define ADJUSTCOLOR_TABFILE				"\\settings\\AdjustColor.txt"	
unsigned int		InitAdjustColorTab();

unsigned int	* g_pAdjustColorTab = NULL;
unsigned int g_ulAdjustColorCount = 0;

#endif

BOOL	InitTaskSetting();

#ifndef _SERVER
#include "KMagicDesc.h"
#include "Scene/KScenePlaceC.h"
BOOL g_bUISelIntelActiveWithServer = FALSE;//当前选择框是否与服务器端交互
BOOL g_bUISpeakActiveWithServer = FALSE;
int	g_bUISelLastSelCount = 0;
extern KTabFile g_StringResourseTabFile;
KTabFile g_RankTabSetting;
#endif

//是否将脚本出错信息输出到文件供调试
#ifdef _SERVER
IServer* g_pServer;
IClient* g_pTongClient;
extern KTabFile g_EventItemTab;
extern KTabFile g_QuestItemTab;
extern KTabFile g_TownPortalTab;
extern KTabFile g_MagicScriptTab;
#else
IClient* g_pClient;
BOOL	 g_bPingReply;
#endif
//---------------------------------------------------------------------------
#ifdef TOOLVERSION
KSpriteCache	g_SpriteCache;
#endif
KTabFile		g_OrdinSkillsSetting, g_MisslesSetting;
KTabFile		g_SkillLevelSetting;
KTabFile		g_NpcSetting;
#ifndef _SERVER
KTabFile		g_NpcKindFile; //记录Npc人物类型文件
#endif
int				g_nMeleeWeaponSkill[MAX_MELEEWEAPON_PARTICULARTYPE_NUM];
int				g_nRangeWeaponSkill[MAX_RANGEWEAPON_PARTICULARTYPE_NUM];	
int				g_nHandSkill;

KTabFile 		g_MaskChangeRes;
KIniFile 		g_GameSetting;
KIniFile 		g_MapTraffic;

#ifdef _SERVER
int				g_ExpRate = 1;
int				g_MoneyRate = 1;
int				g_SkillExpRate = 1;
int				g_Skill120ExpRate = 1;
KLuaScript * 	g_pStartScript = NULL;
#endif

#ifndef	_SERVER
KSoundCache		g_SoundCache;
KMusic			*g_pMusic = NULL;
#endif

KLuaScript g_WorldScript;

void g_InitProtocol();

//---------------------------------------------------------------------------
CORE_API void g_InitCore()
{
	time_t ltime;
	time(&ltime);
	printf("Stasting Core.... %s", ctime( &ltime ));

#ifdef _SERVER
	printf("MaxPlayer : %d\n", MAX_PLAYER);
	printf("MaxItem   : %d\n", MAX_ITEM);
	printf("MaxNpc    : %d\n", MAX_NPC);
	printf("MaxLever  : %d\n", MAX_LEVEL);
#endif
	
	g_InitProtocol();
	g_FindDebugWindow("#32770", "DebugWin");	
#ifdef _DEBUG
	{
		srand((unsigned int) time(NULL));
#ifdef DEBUGOPT_SCRIPT_MSG_FILEOUT
		struct tm *newtime;
		time_t long_time;
		time( &long_time );   	 /* Get time as long integer. */
		newtime = localtime( &long_time ); /* Convert to local time. */
		char szFileName[MAX_PATH];
		char szFileName1[MAX_PATH];
		char szPathName[MAX_PATH];
		g_CreatePath("\\DebugData");
		g_CreatePath("\\DebugData\\Script");
			
		GetCurrentDirectory(MAX_PATH, szPathName);

		sprintf(szFileName, "%s\\DebugData\\Script\\ScriptOut_%d%d%d%d%d.txt", szPathName, newtime->tm_mon, newtime->tm_mday, newtime->tm_hour, newtime->tm_min, newtime->tm_sec);
		sprintf(szFileName1, "%s\\DebugData\\Script\\ScriptErr_%d%d%d%d%d.txt", szPathName, newtime->tm_mon, newtime->tm_mday, newtime->tm_hour, newtime->tm_min, newtime->tm_sec);
		lua_setdebugout(szFileName, szFileName1);
#endif
	}

#endif
	
#ifndef __linux
	g_RandomSeed(GetTickCount());
#else
	g_RandomSeed(clock());
#endif
	srand( (unsigned)time( NULL ) );
	time( &ltime );
	
#ifdef TOOLVERSION	
	g_SpriteCache.Init(256);
#endif

#ifndef _SERVER
	g_bPingReply = TRUE;
	g_SoundCache.Init(256);
	g_SubWorldSet.m_cMusic.Init();
#endif

#ifndef TOOLVERSION
	g_InitSeries();
    if (!g_InitMath())
    	printf("[Math]Init Math Error!");
 
	ItemSet.Init();
	ItemGen.Init();
#ifndef _SERVER
	InitAdjustColorTab();//加载偏色表......
	g_MagicDesc.Init();
#endif
	g_ItemChangeRes.Init();
#endif
	NpcSet.Init();
	ObjSet.Init();
	MissleSet.Init();
	
	g_IniScriptEngine();
//	g_SetFilePath(SETTING_PATH);
	g_OrdinSkillsSetting.Load(SKILL_SETTING_FILE);
	g_MisslesSetting.Load(MISSLES_SETTING_FILE);
	g_NpcSetting.Load(NPC_SETTING_FILE);
	InitGameSetting();
	InitSkillSetting();
	InitMissleSetting();
	InitNpcSetting();

	InitTaskSetting();

	// 这个涉及到与技能相关的东西，所以必须放在技能初始化之后

	if (!PlayerSet.Init())
		printf("Init PlayerSet Error!!!!\n");

	if (!TongData.Init())
		printf("Init TongData Error!!!!\n");
	
#ifdef _SERVER
	if (!GameData.Init())
		printf("Init GameData Error!!!!\n");

	memset(g_TaskGlobalValue, 0, sizeof(g_TaskGlobalValue));
	g_TeamSet.Init();
	g_ChatRoomSet.Init();
	
//	g_SetFilePath("\\");
	g_SubWorldSet.Load("\\maps\\WorldSet.ini");
	g_WorldScript.Init();
	g_WorldScript.RegisterFunctions(WorldScriptFuns, g_GetWorldScriptFunNum());
		
#endif
	g_SubWorldSet.LoadFile();
#ifndef _SERVER
	g_ScenePlace.Initialize();
#endif
	time(&ltime);

	if(!g_Faction.Init())
		printf("Init Faction Fail\n");

	time(&ltime);
		
	memset(g_nMeleeWeaponSkill, 0, sizeof(g_nMeleeWeaponSkill));
	memset(g_nRangeWeaponSkill, 0, sizeof(g_nRangeWeaponSkill));
	KTabFile Weapon_PhysicsSkillIdFile;
	if (Weapon_PhysicsSkillIdFile.Load(WEAPON_PHYSICSSKILLFILE))
	{
		int nHeight = Weapon_PhysicsSkillIdFile.GetHeight() - 1;
		int nDetailCol		= Weapon_PhysicsSkillIdFile.FindColumn(WEAPON_DETAILTYPE);
		int	nParticularCol	= Weapon_PhysicsSkillIdFile.FindColumn(WEAPON_PARTICULARTYPE);
		int nPhysicsSkillCol = Weapon_PhysicsSkillIdFile.FindColumn(WEAPON_SKILLID);
		
		for (int i = 0; i < nHeight; i ++)
		{
			int nDetail = 0;
			int nParticular  = 0;
			int nPhysicsSkill = 0;
			Weapon_PhysicsSkillIdFile.GetInteger(i + 2, nDetailCol, -1, &nDetail);
			Weapon_PhysicsSkillIdFile.GetInteger(i + 2, nParticularCol, -1, &nParticular);
			Weapon_PhysicsSkillIdFile.GetInteger(i + 2, nPhysicsSkillCol, -1, &nPhysicsSkill);
			
			//近程武器
			if (nDetail == 0 )
			{
				if (nParticular >= 0 && nParticular < MAX_MELEEWEAPON_PARTICULARTYPE_NUM && nPhysicsSkill > 0 && nPhysicsSkill < MAX_SKILL)
					g_nMeleeWeaponSkill[nParticular] = nPhysicsSkill;
			}
			else if (nDetail == 1)
			{
				if (nParticular >= 0 && nParticular < MAX_RANGEWEAPON_PARTICULARTYPE_NUM && nPhysicsSkill > 0 && nPhysicsSkill < MAX_SKILL)
					g_nRangeWeaponSkill[nParticular] = nPhysicsSkill;
			}
			else if (nDetail == -1) //空手
			{
				if (nPhysicsSkill > 0 && nPhysicsSkill < MAX_SKILL)
					g_nHandSkill = nPhysicsSkill;
			}
		}
	}
#ifndef TOOLVERSION
	if (!BuySell.Init())
		printf("BuySell Init Failed!\n");
#endif
}


BOOL	InitTaskSetting()
{
#ifdef _SERVER
	if (!g_EventItemTab.Load(EVENTITEM_TABFILE))
	{
		g_DebugLog("[TASK]CAN NOT LOAD EventItem Setting File %s", EVENTITEM_TABFILE);
	}
	if (!g_QuestItemTab.Load(QUESTITEM_TABFILE))
	{
		g_DebugLog("[TASK]CAN NOT LOAD QuestItem Setting File %s", QUESTITEM_TABFILE);
	}
	if (!g_TownPortalTab.Load(TOWNPORTAL_TABFILE))
	{
		g_DebugLog("[TASK]CAN NOT LOAD TownPortal Setting File %s", TOWNPORTAL_TABFILE);
	}
	if (!g_MagicScriptTab.Load(MAGICSCRIPT_TABFILE))
	{
		g_DebugLog("[TASK]CAN NOT LOAD MagicScript Setting File %s", MAGICSCRIPT_TABFILE);
	}
	if (!g_WayPointTabFile.Load(WORLD_WAYPOINT_TABFILE))
	{
		g_DebugLog("[TASK]CAN NOT LOAD WayPoint Setting File %s", WORLD_WAYPOINT_TABFILE);
	}
	if (!g_StationTabFile.Load(WORLD_STATION_TABFILE))
	{
		g_DebugLog("[TASK]CAN NOT LOAD WayPoint Setting File %s", WORLD_STATION_TABFILE);
	}

	if (!g_StationPriceTabFile.Load(WORLD_STATIONPRICE_TABFILE))
	{
		g_DebugLog("[TASK]CAN NOT LOAD STATION PRICE TabFile %s", WORLD_STATIONPRICE_TABFILE);
	}

	if (!g_WayPointPriceTabFile.Load(WORLD_WAYPOINTPRICE_TABFILE))
	{
		g_DebugLog("[TASK]CAN NOT LOAD WAYPOINT PRICE TabFile %s", WORLD_WAYPOINTPRICE_TABFILE);
	}
	
	if (!g_DockTabFile.Load(WORLD_DOCK_TABFILE))
	{
		g_DebugLog("[TASK]CAN NOT LOAD DOCK TabFile %s", WORLD_DOCK_TABFILE);
	}

	if (!g_DockPriceTabFile.Load(WORLD_DOCKPRICE_TABFILE))
	{
		g_DebugLog("[TASK]CAN NOT LOAD DOCK PRICE TabFile %s", WORLD_DOCKPRICE_TABFILE);
	}

	int nWidth1 = g_StationPriceTabFile.GetWidth();
	int nHeight1 = g_StationPriceTabFile.GetHeight();
	int nWidth2 = g_WayPointPriceTabFile.GetWidth();
	int nHeight2 = g_WayPointPriceTabFile.GetHeight();
	int nWidth3 = g_DockPriceTabFile.GetWidth();
	int nHeight3 = g_DockPriceTabFile.GetHeight();
	
	
	KASSERT(nHeight1 * nWidth1);
	KASSERT(nHeight2 * nWidth2);
	KASSERT(nHeight3 * nWidth3);
	
	if (nWidth1 * nHeight1)
	{
		g_StationPriceTabFile.GetInteger(1,1, 1, &g_StationPriceUnit);
		g_pStationPriceTab = new int[nWidth1 * nHeight1];
		memset(g_pStationPriceTab, -1, nWidth1 * nHeight1 * sizeof(int));
	}
	
	if (nWidth2 * nHeight2)
	{
		g_WayPointPriceTabFile.GetInteger(1,1,1, &g_WayPointPriceUnit);
		g_pWayPointPriceTab = new int[nWidth2 * nHeight2];
		memset(g_pWayPointPriceTab, -1, nWidth2 * nHeight2 * sizeof(int));
	}
	
	if (nWidth3 * nHeight3)
	{
		g_DockPriceTabFile.GetInteger(1,1,1, &g_DockPriceUnit);
		g_pDockPriceTab = new int[nWidth3 * nHeight3];
		memset(g_pDockPriceTab, -1, nWidth3 * nHeight3 * sizeof(int));
	}
	
#endif
#ifndef _SERVER
	if (!g_StringResourseTabFile.Load(STRINGRESOURSE_TABFILE))
	{
		g_DebugLog("[TASK]CAN NOT LOAD %s", STRINGRESOURSE_TABFILE);
	}

	if (!g_RankTabSetting.Load(PLAYER_RANK_SETTING_TABFILE))
	{
		g_DebugLog("[TASK]CAN NOT LOAD %s", PLAYER_RANK_SETTING_TABFILE);
	}
#endif
	
	return TRUE;
}

BOOL	InitNpcSetting()
{
	memset(g_pNpcTemplate, 0, sizeof(void*) * MAX_NPCSTYLE);
		
#ifndef _SERVER
	if ( !g_NpcKindFile.Load(NPC_RES_KIND_FILE_NAME) )
	{
		g_DebugLog("Can not load %s", NPC_RES_KIND_FILE_NAME);
	}
	g_NpcResList.Init();
#endif
	
	return TRUE;
}

BOOL	InitMissleSetting()
{
	int nMissleNum = g_MisslesSetting.GetHeight() - 1;
	
	for (int i = 0; i < nMissleNum; i++)
	{
		int nMissleId = 0;
		g_MisslesSetting.GetInteger(i + 2, "MissleId", -1, &nMissleId);
		
		if (nMissleId > 0)
		{
			g_MisslesLib[nMissleId].GetInfoFromTabFile(i + 2);
			g_MisslesLib[nMissleId].m_nMissleId = nMissleId;
		}
	}
	//printf("Init MisslesLib %d\n", sizeof(g_MisslesLib));
	return TRUE;
}

extern int g_LoadSkillInfo();
BOOL	InitSkillSetting()
{
	
	if (!g_SkillManager.Init())
	{
		_ASSERT(0);
	}

#ifndef _SERVER
	if (!g_ClientWeaponSkillTabFile.Load(CLIENTWEAPONSKILL_TABFILE))
	{
		g_DebugLog("Can Not Load %s", CLIENTWEAPONSKILL_TABFILE);
	}
#endif

	return TRUE;
}
#ifdef _SERVER
BOOL	LoadNpcSettingFromBinFile(LPSTR BinFile = NPC_TEMPLATE_BINFILE)
{
	return FALSE;
}

BOOL	SaveAsBinFileFromNpcSetting(LPSTR BinFile = NPC_TEMPLATE_BINFILE)
{
	return FALSE;
}
#endif

//---------------------------------------------------------------------------

void g_ReleaseCore()
{
#ifdef _SERVER
	GameData.Save();
	
	if (g_pServer)
	{
		g_pServer->Release();
		g_pServer = NULL;
	}
	
	if (g_pStationPriceTab)
	{
		delete [] g_pStationPriceTab;
		g_pStationPriceTab = NULL;
	}

	if (g_pWayPointPriceTab)
	{
		delete [] g_pWayPointPriceTab;
		g_pWayPointPriceTab = NULL;
	}
	
	if (g_pDockPriceTab)
	{
		delete [] g_pDockPriceTab;
		g_pDockPriceTab = NULL;
	}
#else
	g_SubWorldSet.Close();
	g_ScenePlace.ClosePlace();

	if (g_pAdjustColorTab)
	{
		delete []g_pAdjustColorTab;
		g_pAdjustColorTab = NULL;
		g_ulAdjustColorCount = 0;
	}
#endif

    g_UnInitMath();
}

#ifdef _SERVER
void g_SetServer(LPVOID pServer)
{
	g_pServer = reinterpret_cast< IServer * >(pServer);
}

void g_SetTongClient(LPVOID pTong)
{
	g_pTongClient = reinterpret_cast< IClient * >(pTong);
}
#endif

#ifndef _SERVER
void g_SetClient(LPVOID pClient)
{
	g_pClient = reinterpret_cast< IClient * >(pClient);
}

unsigned int	InitAdjustColorTab()
{
	g_pAdjustColorTab = NULL;
	g_ulAdjustColorCount = 0;
	KTabFile TabFile;
	if (!TabFile.Load(ADJUSTCOLOR_TABFILE))
	{
		_ASSERT(0);
		g_DebugLog("Can't load %s", ADJUSTCOLOR_TABFILE);
		return 0;
	}

	int nHeight = TabFile.GetHeight() - 1;
	
	if (nHeight <= 0)
		return 0;

	g_pAdjustColorTab = (unsigned int *)new unsigned long [nHeight];
	g_ulAdjustColorCount = nHeight;
	for (int i = 0; i < nHeight; i ++)
	{
		BYTE bAlpha = 0;
		BYTE bRed	= 0;
		BYTE bGreen	= 0;
		BYTE bBlue	= 0;
		int nAlpha;
		int nRed;
		int nGreen;
		int nBlue;
		TabFile.GetInteger(i + 2, "ALPHA", 0x000000ff, &nAlpha);
		nAlpha	&= 0xff;
		TabFile.GetInteger(i + 2,"RED", 0, &nRed);
		nRed	&= 0xff;
		TabFile.GetInteger(i + 2,"GREEN",  0, &nGreen);
		nGreen	&= 0xff;
		TabFile.GetInteger(i + 2,"BLUE",  0, &nBlue);
		nBlue	&= 0xff;
		unsigned long ulAdjustColor = nAlpha << 24 | nRed << 16 | nGreen << 8 | nBlue;
		g_pAdjustColorTab[i] = ulAdjustColor;
	}
	return g_ulAdjustColorCount;
}
#endif


BOOL InitGameSetting()
{
	if (!g_MapTraffic.Load(MINIMAP_SETTING_FILE_INI))
	{
		g_DebugLog("Can not load %s", MINIMAP_SETTING_FILE_INI);
	}
	if (!g_MaskChangeRes.Load(CHANGERES_MASK_FILE))
	{
		g_DebugLog("Can not load %s", CHANGERES_MASK_FILE);
	}
	if (g_GameSetting.Load(GAME_SETTING_FILE_INI))
	{
#ifdef _SERVER
		g_GameSetting.GetInteger("ServerConfig", "ExpRate", 1, &g_ExpRate);
		g_GameSetting.GetInteger("ServerConfig", "MoneyRate", 1, &g_MoneyRate);
		g_GameSetting.GetInteger("ServerConfig", "SkillRate", 0, &g_SkillExpRate);
		g_GameSetting.GetInteger("ServerConfig", "Skill120Rate", 0, &g_Skill120ExpRate);
#endif
	}
	else
	{
		g_DebugLog("Can not load %s", GAME_SETTING_FILE_INI);
	}
	return TRUE;
}

int PositionToRoom(int P)
{
	switch (P)
	{
	case pos_hand:
	case pos_equip:
	case pos_equiproom:
		return room_equipment;
	case pos_equiproomex:
		return room_equipmentex;
	case pos_repositoryroom:
		return room_repository;
	case pos_repositoryroom1:
		return room_repository1;
	case pos_repositoryroom2:
		return room_repository2;
	case pos_repositoryroom3:
		return room_repository3;
	case pos_repositoryroom4:
		return room_repository4;
	case pos_repositoryroom5:
		return room_repository5;
	case pos_traderoom:
		return room_equipment;
	case pos_trade1:
		return room_equipment;
	case pos_immediacy:
		return room_immediacy;
	case pos_give:
		return room_give;
	case pos_compound:
		return room_equipment;
	case pos_compoundroom:
		return room_compound;
	default:
		return room_equipment;
	}
	return room_equipment;
}