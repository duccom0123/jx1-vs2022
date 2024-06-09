//#include <objbase.h>
//#include <crtdbg.h>
#include "KCore.h"
#include "KPlayer.h"
#include "KNpc.h"
#include "KNpcSet.h"
#include "KSubWorld.h"
#include "GameDataDef.h"
#include "KProtocolProcess.h"
#ifdef _SERVER
//#include "KNetServer.h"
//#include "../MultiServer/Heaven/Interface/iServer.h"
#include "KSubWorldSet.h"
#include "CoreServerShell.h"
#endif
#include "KPlayerSet.h"
#include "KTongData.h"
#include "Text.h"
#include "Cryptography/EDOneTimePad.h"
#include "CoreUseNameDef.h"
#include <shlwapi.h>

KTongData TongData;

KTongData::KTongData()
{
}

BOOL	KTongData::Init()
{
#ifdef _SERVER
	m_nNumTong = 0;
	for (int i = 0; i< MAX_TONG_COUNT; i++)
		m_sTong[i].Init();
#endif

	if ( !m_sTongLevel.Init())
		return FALSE;
	
	KIniFile	cTongFile;
	if (cTongFile.Load(defTONG_PARAM_FILE))
	{
		cTongFile.GetInteger("Main", "RequireLv", 60, &m_sTongParam.m_nLevel);
		cTongFile.GetInteger("Main", "RequireLeadLv", 10, &m_sTongParam.m_nLeadLevel);
		cTongFile.GetInteger("Main", "RequireMn", 20000000, &m_sTongParam.m_nMoney);
		cTongFile.GetInteger3("Main", "RequireItem", &m_sTongParam.m_nItemCount, &m_sTongParam.m_nItemGenre, &m_sTongParam.m_nItemDetail);
		cTongFile.GetInteger("Main", "RequireRp", 300, &m_sTongParam.m_nRepute);
		cTongFile.GetInteger("Main", "RequireFy", 500, &m_sTongParam.m_nFuYuan);
		cTongFile.GetInteger("Main", "Camp", 1000000, &m_sTongParam.m_nMoneyChangeCamp);
		cTongFile.GetInteger("Main", "SaveJiyu", 10000000, &m_sTongParam.m_nMoneySaveJiyu);
		cTongFile.GetInteger("Main", "LeavMn", 100000, &m_sTongParam.m_nLeaveMoney);
		cTongFile.GetInteger("Main", "LeaveRp", 100, &m_sTongParam.m_nLeaveRepute);
		cTongFile.GetInteger("Main", "LeaveTm", 86400, &m_sTongParam.m_nLeaveTime);
	}
	else
	{
		m_sTongParam.m_nLevel		= 60;
		m_sTongParam.m_nLeadLevel	= 10;
		m_sTongParam.m_nMoney		= 20000000;
		m_sTongParam.m_nItemCount	= 1;
		m_sTongParam.m_nItemGenre	= item_task;
		m_sTongParam.m_nItemDetail	= 195;
		m_sTongParam.m_nRepute		= 300;
		m_sTongParam.m_nFuYuan		= 500;
		m_sTongParam.m_nLeaveMoney	= 100000;
		m_sTongParam.m_nLeaveRepute = 100;
		m_sTongParam.m_nLeaveTime	= 86400;
		m_sTongParam.m_nMoneyChangeCamp	= 1000000;
		m_sTongParam.m_nMoneySaveJiyu = 10000000;
	}
	return TRUE;
}
///////////////////////////////////////////
#ifdef _SERVER
KOneTong::KOneTong()
{
	m_dwNameID = 0;
}

BOOL	KOneTong::Init()
{
	return TRUE;
}
#endif

/////////////////////////////////////////////////////////
KTongLevelAdd::KTongLevelAdd()
{
	memset(m_nExpVal, 0, sizeof(m_nExpVal));
	memset(m_nEffVal, 0, sizeof(m_nEffVal));
	memset(m_nMemberNum, 0, sizeof(m_nMemberNum));
}

BOOL	KTongLevelAdd::Init()
{
	int			i;
	KTabFile	TongLevel;

	if ( !TongLevel.Load(defTONG_LEVEL_FILE) )
		return FALSE;
	for (i = 0; i < MAX_TONG_LEVEL; i++)
	{
		TongLevel.GetInteger(i + 2, 2, 0, (int*)&m_nExpVal[i]);
		TongLevel.GetInteger(i + 2, 3, 1, (int*)&m_nEffVal[i]);
		TongLevel.GetInteger(i + 2, 4, 1, (int*)&m_nMemberNum[i]);
	}
	return TRUE;
}

int		KTongLevelAdd::GetExp(int nLevel)
{
	if (nLevel >= 1 && nLevel <= MAX_TONG_LEVEL)
		return m_nExpVal[nLevel - 1];
	return 0;
}

int		KTongLevelAdd::GetEff(int nLevel)
{
	if (nLevel >= 1 && nLevel <= MAX_TONG_LEVEL)
		return m_nEffVal[nLevel - 1];
	return 0;
}

int		KTongLevelAdd::GetMemNum(int nLevel)
{
	if (nLevel >= 1 && nLevel <= MAX_TONG_LEVEL)
		return m_nMemberNum[nLevel - 1];
	return 0;
}