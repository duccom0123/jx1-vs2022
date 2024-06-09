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
#include "KGameData.h"
#include "Text.h"
#include "Cryptography/EDOneTimePad.h"
#include "CoreUseNameDef.h"
#include <shlwapi.h>

#define	DATA_SAVE_SEC			"DATA"
#define	DATA_SAVE_KEY			"Data%d"
	
#define	DATA_GROUP_SEC			"DATAGROUP%d"
#define	DATA_GROUP_NAMEID_KEY	"Data1"
#define	DATA_GROUP_NAME1_KEY	"Data2"
#define	DATA_GROUP_NAME2_KEY	"Data3"
#define	DATA_GROUP_VALUE_KEY	"DataV%d"

KGameData GameData;
#ifdef _SERVER
KGameData::KGameData()
{
	memset(m_szSave, 0, sizeof(m_szSave));
	for (int i=0;i<MAX_PLAYER;i++)
		m_sDataGroup[i].Clear();
}

BOOL	KGameData::Init()
{
	m_nNumGroup = 0;

	if ( !InitGameData())
		return FALSE;

	return TRUE;
}

void KGameData::SetData(int nNo, char* szValue)
{
	if (nNo < 0 || nNo >= MAX_DATA_INT)
		return;

	strcpy(m_szSave[nNo], szValue);
}

int KGameData::GetDataInt(int nNo)
{
	if (nNo < 0 || nNo >= MAX_DATA_INT)
		return 0;

	if (m_szSave[nNo][0])
		return atoi(m_szSave[nNo]);

	return 0;
}

const char* KGameData::GetDataStr(int nNo)
{
	if (nNo < 0 || nNo >= MAX_DATA_INT)
		return "";

	return m_szSave[nNo];
}

int KGameData::AddDataGr(KDataGroup* pInfo)
{
	int nGroup = FindDataId(pInfo->nNameId);
	if (nGroup > -1)
	{
		return nGroup;
	}
	int i = FindFreeDataGr();
	if (i >= 0)
	{
		m_sDataGroup[i].nNameId = pInfo->nNameId;
		for (int j = 0; j < MAX_DATAGROUP_VALUE; j++)
			m_sDataGroup[i].nValue[j] = pInfo->nValue[j];
		strcpy(m_sDataGroup[i].szName1, pInfo->szName1);
		strcpy(m_sDataGroup[i].szName2, pInfo->szName2);
		nGroup = i;
		
		m_nNumGroup++;
	}
	return nGroup;
}

void KGameData::SetDataGr(int nGroup, KDataGroup* pInfo)
{
	if (nGroup >= 0)
	{
		m_sDataGroup[nGroup].nNameId = pInfo->nNameId;
		for (int i = 0; i < MAX_DATAGROUP_VALUE; i++)
			m_sDataGroup[nGroup].nValue[i] = pInfo->nValue[i];
		strcpy(m_sDataGroup[nGroup].szName1, pInfo->szName1);
		strcpy(m_sDataGroup[nGroup].szName2, pInfo->szName2);
	}
}

int KGameData::FindFreeDataGr()
{
	for (int i = 0; i < MAX_PLAYER; i ++)
	{
		if (!m_sDataGroup[i].nNameId)
			return i;
	}
	return -1;
}

int KGameData::FindDataId(DWORD dwId)
{
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		if (m_sDataGroup[i].nNameId == dwId)
			return i;
	}
	return -1;
}

int KGameData::GetDataGr_dwName(int nNo)
{
	return m_sDataGroup[nNo].nNameId;
}

int KGameData::GetDataGr_nValue(int nNo, int nIndex)
{
	return m_sDataGroup[nNo].nValue[nIndex];
}

char* KGameData::GetStrDataGr_Name1(int nNo)
{
	return m_sDataGroup[nNo].szName1;
}

char* KGameData::GetStrDataGr_Name2(int nNo)
{
	return m_sDataGroup[nNo].szName2;
}

BOOL KGameData::InitGameData()
{
	KIniFile Ini;
	if (!Ini.Load(GAME_DATA_FILE_INI))
		Ini.Save(GAME_DATA_FILE_INI);


	for (int i=0;i<GameDataNum;i++)
	{

		switch (i)
		{
			case GameDataInt:
				for (int j=0;j<MAX_DATA_INT;j++)
				{
					char szSectName[16];
					char szKeyName[16];
					char szName[32];
					sprintf(szKeyName, DATA_SAVE_KEY, j);
					Ini.GetString(DATA_SAVE_SEC, szKeyName, "", m_szSave[j], sizeof(m_szSave[j]));
				}
				break;
			case GameDataGr:
				for (int j=0;j<MAX_PLAYER;j++)
				{
					char szSectName[16];
					char szKeyName[16];
					char szName[32];
					sprintf(szSectName, DATA_GROUP_SEC, j);
					Ini.GetInteger(szSectName, DATA_GROUP_NAMEID_KEY, 0, &m_sDataGroup[j].nNameId);
					szName[0] = 0;
					Ini.GetString(szSectName, DATA_GROUP_NAME1_KEY, "", szName, sizeof(szName));
					if (szName[0])
						strcpy(m_sDataGroup[j].szName1, szName);
					szName[0] = 0;
					Ini.GetString(szSectName, DATA_GROUP_NAME2_KEY, "", szName, sizeof(szName));
					if (szName[0])
						strcpy(m_sDataGroup[j].szName2, szName);
					for (int k=0;k<MAX_DATAGROUP_VALUE;k++)
					{
						sprintf(szKeyName, DATA_GROUP_VALUE_KEY, k);
						Ini.GetInteger(szSectName, szKeyName, 0, &m_sDataGroup[j].nValue[k]);
					}
					if (m_sDataGroup[j].nNameId)
						m_nNumGroup++;
				}
				break;
			default:
				break;
		}
	}
	return TRUE;
}

BOOL KGameData::Save()
{
	KIniFile Ini;

	char szSectName[32];
	char szKeyName[32];

	for (int i=0;i<MAX_DATA_INT;i++)
	{

		if (!m_szSave[i][0])
			continue;
		sprintf(szKeyName, DATA_SAVE_KEY, i);
		Ini.WriteString(DATA_SAVE_SEC, szKeyName, m_szSave[i]);
	}
	
	for (int i=0;i<MAX_PLAYER;i++)
	{
		if (!m_sDataGroup[i].nNameId)
			continue;
		sprintf(szSectName, DATA_GROUP_SEC, i);
		Ini.WriteInteger(szSectName, DATA_GROUP_NAMEID_KEY, m_sDataGroup[i].nNameId);
		Ini.WriteString(szSectName, DATA_GROUP_NAME1_KEY, m_sDataGroup[i].szName1);
		Ini.WriteString(szSectName, DATA_GROUP_NAME2_KEY, m_sDataGroup[i].szName2);
		for (int j=0;j<MAX_DATAGROUP_VALUE;j++)
		{
			sprintf(szKeyName, DATA_GROUP_VALUE_KEY, j);
			Ini.WriteInteger(szSectName, szKeyName, m_sDataGroup[i].nValue[j]);
		}
	}
	Ini.Save(GAME_DATA_FILE_INI);
	return TRUE;
}
#endif