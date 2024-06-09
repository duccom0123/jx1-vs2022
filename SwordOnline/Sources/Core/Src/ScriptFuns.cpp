/*******************************************************************************
// FileName			:	ScriptFuns.cpp
// FileAuthor		:	RomanDou
// FileCreateDate	:	2002-11-19 15:58:20
// FileDescription	:	脚本指令集
// Revision Count	:	
*******************************************************************************/
#ifndef WIN32
#include <string>
#endif

#include "KWin32.h"
#include "KEngine.h"
#include "KDebug.h"
#include "KStepLuaScript.h"
#include "LuaLib.h"
#include "KScriptList.h"
#include <string.h>
#include "LuaFuns.h"
#include "KCore.h"
#include "KNpc.h"
#include "KSubWorld.h"
#include "KObjSet.h"
#include "KItemSet.h"
//#include "KNetClient.h"
#include "KScriptValueSet.h"
#include "KNpcSet.h"
#include "KItemChangeRes.h"
#include "KPlayerSet.h"
#include "KPlayer.h"
#include "KSubWorldSet.h"
#include "KProtocolProcess.h"
#include "KBuySell.h"
#include "KTaskFuns.h"
#include "KPlayerDef.h"
#include "KGameData.h"
#include "KNpcTemplate.h"
#include "KTongData.h"
#include "KLadder.h"
#include "KMath.h"
#include "../../Engine/Src/Text.h"
#include "../../Engine/Src/KSG_StringProcess.h"
#include <time.h>
#ifdef _SERVER
//#include "KNetServer.h"
//#include "../MultiServer/Heaven/interface/iServer.h"
#include "../../../Headers/KProtocolDef.h"
#include "../../../Headers/KProtocol.h"
#include "../../../Headers/KRelayProtocol.h"
#include "../../../Headers/KTongProtocol.h"
#include "KNewProtocolProcess.h"
#endif
#include "KSortScript.h"
#ifndef __linux
#include "Shlwapi.h"
#include "windows.h"
#include "winbase.h"
#include <direct.h>
#else
#include "unistd.h"
#endif

#ifdef _STANDALONE
#include "KSG_StringProcess.h"
#else
#include "../../Engine/Src/KSG_StringProcess.h"
#endif

#ifndef WIN32
typedef struct  _SYSTEMTIME
{
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
}	SYSTEMTIME;
typedef struct  _FILETIME
{
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
}	FILETIME;
#endif

inline const char* _ip2a(DWORD ip) { in_addr ia; ia.s_addr = ip; return inet_ntoa(ia);}
inline DWORD _a2ip(const char* cp) { return inet_addr(cp);}

KScriptList		g_StoryScriptList;
KStepLuaScript * LuaGetScript(Lua_State * L);
int	GetPlayerIndex(Lua_State * L);
extern int g_GetPriceToStation(int,int);
extern int g_GetPriceToWayPoint(int,int);
extern int g_GetPriceToDock(int ,int );

//BitValue = GetBit(Value, BitNo)
int LuaGetBit(Lua_State * L)
{
	int nBitValue = 0;
	int nIntValue = (int)Lua_ValueToNumber(L, 1);
	int nBitNumber = (int)Lua_ValueToNumber(L, 2);
	
	if (nBitNumber >= 32 || nBitNumber <= 0) 
		goto lab_getbit;
	nBitValue = (nIntValue & (1 << (nBitNumber - 1))) != 0;
lab_getbit:
	Lua_PushNumber(L, nBitValue);
	return 1;
}

//NewBit = SetBit(Value, BitNo, BitValue)
int LuaSetBit(Lua_State * L)
{
	int nIntValue = (int)Lua_ValueToNumber(L, 1);
	int nBitNumber = (int)Lua_ValueToNumber(L, 2);
	int nBitValue = (int)Lua_ValueToNumber(L,3);
	nBitValue = (nBitValue == 1);
	
	if (nBitNumber > 32 || nBitNumber <= 0) 
		goto lab_setbit;
	
	nIntValue = (nIntValue | (1 << (nBitNumber - 1)));
lab_setbit:
	Lua_PushNumber(L, nIntValue);
	return 1;
}

//ByteValue = GetByte(Value, ByteNo)
int LuaGetByte(Lua_State * L)
{
	int nByteValue = 0;
	int nIntValue = (int)Lua_ValueToNumber(L, 1);
	int nByteNumber = (int)Lua_ValueToNumber(L, 2);
	
	if (nByteNumber > 4 || nByteNumber <= 0) 
		goto lab_getByte;
	nByteValue = (nIntValue & (0xff << ((nByteNumber - 1) * 8) )) >> ((nByteNumber - 1) * 8);
	
lab_getByte:
	Lua_PushNumber(L, nByteValue);
	return 1;
}

//NewByte = SetByte(Value, ByteNo, ByteValue)
int LuaSetByte(Lua_State * L)
{
	BYTE * pByte =	NULL;
	int nIntValue = (int)Lua_ValueToNumber(L, 1);
	int nByteNumber = (int)Lua_ValueToNumber(L, 2);
	int nByteValue = (int)Lua_ValueToNumber(L,3);
	nByteValue = (nByteValue & 0xff);
	
	if (nByteNumber > 4 || nByteNumber <= 0) 
		goto lab_setByte;
	
	pByte = (BYTE*)&nIntValue;
	*(pByte + (nByteNumber -1)) = (BYTE)nByteValue;
	//nIntValue = (nIntValue | (0xff << ((nByteNumber - 1) * 8) )) ;
lab_setByte:
	Lua_PushNumber(L, nIntValue);
	return 1;
}
//------------------------------

//============================================
int GetSubWorldIndex(Lua_State * L)
{
	Lua_GetGlobal(L, SCRIPT_SUBWORLDINDEX);
	if (lua_isnil(L,Lua_GetTopIndex(L)))
        return -1;
	int nIndex = (int)Lua_ValueToNumber(L, Lua_GetTopIndex(L));
    if (nIndex >= MAX_SUBWORLD || nIndex <= 0) 
	{
		_ASSERT(0);
		return -1;
	}
	if (SubWorld[nIndex].m_nIndex >= MAX_SUBWORLD || SubWorld[nIndex].m_nIndex < 0)
	{
		_ASSERT(0);
		return -1;
	}
    return nIndex;
}

//Idx = SubWorldID2Idx(dwID)
int LuaSubWorldIDToIndex(Lua_State * L)
{
	int nTargetSubWorld = -1;
	int nSubWorldID = 0;
	if (Lua_GetTopIndex(L) < 1)
		goto lab_subworldid2idx;
	
	nSubWorldID = (int)Lua_ValueToNumber(L, 1);
	nTargetSubWorld = g_SubWorldSet.SearchWorld(nSubWorldID);	
	
lab_subworldid2idx:
	Lua_PushNumber(L, nTargetSubWorld);
	return 1;
}

int LuaSubWorldIndexToID(Lua_State * L)
{
	int nTargetSubWorld = -1;
	int nSubWorldIndex = 0;
	if (Lua_GetTopIndex(L) < 1)
		goto lab_subworldid2idx;
	
	nSubWorldIndex = (int)Lua_ValueToNumber(L, 1);
	nTargetSubWorld = SubWorld[nSubWorldIndex].m_SubWorldID;	
	
lab_subworldid2idx:
	Lua_PushNumber(L, nTargetSubWorld);
	return 1;
}

/*
Say(sMainInfo, nSelCount, sSel1, sSel2, sSel3, .....,sSeln) 
Say(nMainInfo, nSelCount, sSel1, sSel2, sSel3, .....,sSeln) 
Say(nMainInfo, nSelCount, SelTab)
如果是客户端的则不会向服务器端发送任何操作

  Say(100, 3, 10, 23,43)
  Say("选择什么？", 2, "是/yes", "否/no");
  Say("选什么呀", 2, SelTab);
*/
//**************************************************************************************************************************************************************
//												界面脚本
//**************************************************************************************************************************************************************
int LuaSelectUI(Lua_State * L)
{
	char * strMain  = NULL;
	int nMainInfo = 0;
	int nDataType = 0;
	int nOptionNum = 0;
	int nReduceVal = 0;
	char * pImage = NULL;
	char * pContent = NULL;
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) return 0;
	Player[nPlayerIndex].m_bWaitingPlayerFeedBack = false;
	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2) return 0;

	if (nParamNum > 3 && (pImage = strstr((char *)Lua_ValueToString(L,1), "LINK:")))
	{
		nReduceVal = 2;
	}
	if (nParamNum > 2 && Lua_IsNumber(L,2+nReduceVal))
	{
		nOptionNum = (int)Lua_ValueToNumber(L,2+nReduceVal);
		if (nReduceVal && nParamNum < 5) nOptionNum = 0;
	}
	else 
	{
		//_ASSERT(0);
		//return 0;
		nOptionNum = 0;
	}
	
	if  (Lua_IsNumber(L,1+nReduceVal))
	{
		nMainInfo = (int)Lua_ValueToNumber(L,1+nReduceVal);
		nDataType = 1 ;
	}
	else if (Lua_IsString(L, 1+nReduceVal)) 	//检查主信息是字符串还是字符串标识号
	{
		strMain = (char *)Lua_ValueToString(L, 1+nReduceVal);
		nDataType = 0 ;
	}
	else
		return 0;
	
	BOOL bStringTab = FALSE;//标识传进来的选项数据存放在一个数组中，还是许多字符串里
	
	if (Lua_IsString(L,3+nReduceVal))
		bStringTab = FALSE;
	else if (Lua_IsTable(L, 3+nReduceVal))
	{
		bStringTab = TRUE;
	}
	else 
	{
		if (nOptionNum > 0) return 0;
	}
	
	if (bStringTab == FALSE)
	{
		//获得实际传入的选项的个数
		if (nOptionNum > nParamNum - 2+nReduceVal) nOptionNum = nParamNum - 2+nReduceVal;
	}
	
	if (nOptionNum > MAX_ANSWERNUM) nOptionNum = MAX_ANSWERNUM;
	
	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bUIId = UI_SELECTDIALOG;
	UiInfo.m_bParam1 = nDataType;//主信息的类型，字符串(0)或数字(1)
	UiInfo.m_bParam = (BOOL)nReduceVal;
	if (UiInfo.m_bParam)
	{
		g_StrCpyLen(UiInfo.m_szKey, pImage + 5, sizeof(UiInfo.m_szKey));
		UiInfo.m_nParam1 = (int)Lua_ValueToNumber(L,2);
	}
	else
	{
		memset(UiInfo.m_szKey, 0, sizeof(UiInfo.m_szKey));
		UiInfo.m_nParam1 = 0;
	}
	
	//主信息为字符串
	if (nDataType == 0)
	{
		if (strMain)
			sprintf(UiInfo.m_pContent, "%s", strMain);
		pContent = UiInfo.m_pContent;
	}
	else if (nDataType == 1) //主信息为数字标识
	{
		*(int *)UiInfo.m_pContent = nMainInfo;
		pContent = UiInfo.m_pContent + sizeof(int);
		*pContent = 0;
	}
			
	int nOptionCount = 0;
	for (int i  = 0; i < nOptionNum; i ++)
	{	
		char  pAnswer[100];
		pAnswer[0] = 0;
		
		if (bStringTab)
		{
			Lua_PushNumber(L, i + 1);
			Lua_RawGet(L, 3+nReduceVal);
			char * pszString = (char *)Lua_ValueToString(L, Lua_GetTopIndex(L));
			if (pszString)
			{
				g_StrCpyLen(pAnswer, pszString, 100);
			}
		}
		else 
		{
			char * pszString = (char *)Lua_ValueToString(L, i + 3+nReduceVal);
			if (pszString)
				g_StrCpyLen(pAnswer, pszString, 100);
		}
		char * pFunName = strstr(pAnswer, "/");
		
		if (pFunName)
		{
			g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerFun[i], pFunName + 1, sizeof(Player[nPlayerIndex].m_szTaskAnswerFun[0]));
			*pFunName = 0;
			sprintf(pContent, "%s|%s", pContent, pAnswer);
		}
		else 
		{
			//strcpy(Player[nPlayerIndex].m_szTaskAnswerFun[i], NORMAL_FUNCTION_NAME);
			//sprintf(pContent, "%s|%s", pContent, pAnswer);
			break;
		}
		nOptionCount++;
	}

	if(nParamNum > ((bStringTab?1:nOptionCount)+2+nReduceVal+1))
		UiInfo.m_nParam = (int)Lua_ValueToNumber(L, (bStringTab?1:nOptionCount)+2+nReduceVal+1);
	else
		UiInfo.m_nParam = -1;
	UiInfo.m_bOptionNum = nOptionCount;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;	
	Player[nPlayerIndex].m_nAvailableAnswerNum = nOptionCount;
	if (nDataType == 0)
		UiInfo.m_nBufferLen  = strlen(pContent);
	else 
		UiInfo.m_nBufferLen = strlen(pContent) + sizeof(int);
	
#ifndef _SERVER
	UiInfo.m_bParam2 = 0;
#else
	UiInfo.m_bParam2 = 1;
#endif
	
	if (nOptionNum == 0)
	{
		Player[nPlayerIndex].m_bWaitingPlayerFeedBack = false;
	}
	else
	{
		Player[nPlayerIndex].m_bWaitingPlayerFeedBack = true;
	}
	Player[nPlayerIndex].m_SelUiScriptId = Npc[Player[nPlayerIndex].m_nIndex].m_ActionScriptID;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
	return 0;
}

//AddGlobalNews(Newsstr)
int LuaAddGlobalNews(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;
	
	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bUIId = UI_NEWSINFO;
	UiInfo.m_bOptionNum = NEWSMESSAGE_NORMAL;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	
	int nMsgId = 0;
	int nTimes = 1;
	if(Lua_GetTopIndex(L) > 1)
		nTimes = (int)Lua_ValueToNumber(L,2);
	
	if (Lua_IsNumber(L,1))
	{
		nMsgId = (int)Lua_ValueToNumber(L,1);
		*((int *)(UiInfo.m_pContent)) = nMsgId;
		UiInfo.m_bParam1 = 1;
		*(int *)((char *)UiInfo.m_pContent + sizeof(int)) = nTimes;
		UiInfo.m_nBufferLen = sizeof(int) * 2;
	}
	else 
	{
		g_StrCpyLen(UiInfo.m_pContent, Lua_ValueToString(L,1), 512);
		UiInfo.m_nBufferLen = strlen(((char *)UiInfo.m_pContent));
		*(int *)((char *)UiInfo.m_pContent + UiInfo.m_nBufferLen) = nTimes;
		UiInfo.m_nBufferLen += sizeof(int);
		UiInfo.m_bParam1 = 0;
	}
	
#ifndef _SERVER
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) return 0;

	UiInfo.m_bParam2 = 0;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
#else
	UiInfo.m_bParam2 = 1;
	UiInfo.ProtocolType = (BYTE)s2c_scriptaction;
	UiInfo.m_wProtocolLong = sizeof(PLAYER_SCRIPTACTION_SYNC) - MAX_SCIRPTACTION_BUFFERNUM + UiInfo.m_nBufferLen - 1;
	g_NewProtocolProcess.BroadcastGlobal(&UiInfo, UiInfo.m_wProtocolLong + 1);
#endif
	return 0;
}


int LuaAddGlobalNews2(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;
	
	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bUIId = UI_NEWSINFO1;
	UiInfo.m_bOptionNum = NEWSMESSAGE_NORMAL;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	
	int nMsgId = 0;
	int nTimes = 1;
	if(Lua_GetTopIndex(L) > 1)
		nTimes = (int)Lua_ValueToNumber(L,2);
	
	if (Lua_IsNumber(L,1))
	{
		nMsgId = (int)Lua_ValueToNumber(L,1);
		*((int *)(UiInfo.m_pContent)) = nMsgId;
		UiInfo.m_bParam1 = 1;
		*(int *)((char *)UiInfo.m_pContent + sizeof(int)) = nTimes;
		UiInfo.m_nBufferLen = sizeof(int) * 2;
	}
	else 
	{
		g_StrCpyLen(UiInfo.m_pContent, Lua_ValueToString(L,1), 512);
		UiInfo.m_nBufferLen = strlen(((char *)UiInfo.m_pContent));
		*(int *)((char *)UiInfo.m_pContent + UiInfo.m_nBufferLen) = nTimes;
		UiInfo.m_nBufferLen += sizeof(int);
		UiInfo.m_bParam1 = 0;
	}
	
#ifndef _SERVER
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) return 0;

	UiInfo.m_bParam2 = 0;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
#else
	UiInfo.m_bParam2 = 1;
	UiInfo.ProtocolType = (BYTE)s2c_scriptaction;
	UiInfo.m_wProtocolLong = sizeof(PLAYER_SCRIPTACTION_SYNC) - MAX_SCIRPTACTION_BUFFERNUM + UiInfo.m_nBufferLen - 1;
	g_NewProtocolProcess.BroadcastGlobal(&UiInfo, UiInfo.m_wProtocolLong + 1);
#endif
	return 0;
}
//AddLocalNews(Newsstr)
int LuaAddLocalNews(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;
	
	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bUIId = UI_NEWSINFO;
	UiInfo.m_bOptionNum = NEWSMESSAGE_NORMAL;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	
	int nMsgId = 0;
	int nTimes = 1;
	if(Lua_GetTopIndex(L) > 1)
		nTimes = (int)Lua_ValueToNumber(L,2);
	
	if (Lua_IsNumber(L,1))
	{
		nMsgId = (int)Lua_ValueToNumber(L,1);
		*((int *)(UiInfo.m_pContent)) = nMsgId;
		UiInfo.m_bParam1 = 1;
		*(int *)((char *)UiInfo.m_pContent + sizeof(int)) = nTimes;
		UiInfo.m_nBufferLen = sizeof(int) * 2;
	}
	else 
	{
		g_StrCpyLen(UiInfo.m_pContent, Lua_ValueToString(L,1), 512);
		UiInfo.m_nBufferLen = strlen(((char *)UiInfo.m_pContent));
		*(int *)((char *)UiInfo.m_pContent + UiInfo.m_nBufferLen) = nTimes;
		UiInfo.m_nBufferLen += sizeof(int);
		UiInfo.m_bParam1 = 0;
	}
	
#ifndef _SERVER
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) return 0;

	UiInfo.m_bParam2 = 0;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
#else
	UiInfo.m_bParam2 = 1;
	UiInfo.ProtocolType = (BYTE)s2c_scriptaction;
	UiInfo.m_wProtocolLong = sizeof(PLAYER_SCRIPTACTION_SYNC) - MAX_SCIRPTACTION_BUFFERNUM + UiInfo.m_nBufferLen - 1;
	g_NewProtocolProcess.BroadcastLocalServer(&UiInfo, UiInfo.m_wProtocolLong + 1);
#endif
	return 0;
}

//AddGlobalCountNews(strNew/newid, time)
int LuaAddGlobalCountNews(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 2) 
		return 0;
	
	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bUIId = UI_NEWSINFO;
	UiInfo.m_bOptionNum = NEWSMESSAGE_COUNTING;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	
	int nMsgId = 0;
	
	int nTime = (int)Lua_ValueToNumber(L,2);
	
	if (nTime <= 0)
		nTime = 1;
	
	if (Lua_IsNumber(L,1))
	{
		nMsgId = (int)Lua_ValueToNumber(L,1);
		*((int *)(UiInfo.m_pContent)) = nMsgId;
		UiInfo.m_bParam1 = 1;
		*(int *)((char *)UiInfo.m_pContent + sizeof(int)) = nTime;
		UiInfo.m_nBufferLen = sizeof(int) * 2;
	}
	else 
	{
		g_StrCpyLen(UiInfo.m_pContent, Lua_ValueToString(L,1), 512);
		UiInfo.m_nBufferLen = strlen(((char *)UiInfo.m_pContent));
		*(int *)((char *)UiInfo.m_pContent + UiInfo.m_nBufferLen) = nTime;
		UiInfo.m_nBufferLen += sizeof(int);
		UiInfo.m_bParam1 = 0;
	}
	
#ifndef _SERVER
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) return 0;

	UiInfo.m_bParam2 = 0;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
#else
	UiInfo.m_bParam2 = 1;
	UiInfo.ProtocolType = (BYTE)s2c_scriptaction;
	UiInfo.m_wProtocolLong = sizeof(PLAYER_SCRIPTACTION_SYNC) - MAX_SCIRPTACTION_BUFFERNUM + UiInfo.m_nBufferLen - 1;
	g_NewProtocolProcess.BroadcastGlobal(&UiInfo, UiInfo.m_wProtocolLong + 1);
#endif
	return 0;
}

int LuaAddGlobalCountNews2(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;
	
	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bUIId = UI_NEWSINFO1;
	UiInfo.m_bOptionNum = NEWSMESSAGE_COUNTING;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	
	int nMsgId = 0;
	
	int nTime = (int)Lua_ValueToNumber(L,2);
	
	if (nTime <= 0)
		nTime = 1;
	
	if (Lua_IsNumber(L,1))
	{
		nMsgId = (int)Lua_ValueToNumber(L,1);
		*((int *)(UiInfo.m_pContent)) = nMsgId;
		UiInfo.m_bParam1 = 1;
		*(int *)((char *)UiInfo.m_pContent + sizeof(int)) = nTime;
		UiInfo.m_nBufferLen = sizeof(int) * 2;
	}
	else 
	{
		g_StrCpyLen(UiInfo.m_pContent, Lua_ValueToString(L,1), 512);
		UiInfo.m_nBufferLen = strlen(((char *)UiInfo.m_pContent));
		*(int *)((char *)UiInfo.m_pContent + UiInfo.m_nBufferLen) = nTime;
		UiInfo.m_nBufferLen += sizeof(int);
		UiInfo.m_bParam1 = 0;
	}
	
#ifndef _SERVER
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) return 0;

	UiInfo.m_bParam2 = 0;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
#else
	UiInfo.m_bParam2 = 1;
	UiInfo.ProtocolType = (BYTE)s2c_scriptaction;
	UiInfo.m_wProtocolLong = sizeof(PLAYER_SCRIPTACTION_SYNC) - MAX_SCIRPTACTION_BUFFERNUM + UiInfo.m_nBufferLen - 1;
	g_NewProtocolProcess.BroadcastGlobal(&UiInfo, UiInfo.m_wProtocolLong + 1);
#endif
	return 0;
}
//AddLocalCountNews(strNew/newid, time)
int LuaAddLocalCountNews(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 2) 
		return 0;
	
	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bUIId = UI_NEWSINFO;
	UiInfo.m_bOptionNum = NEWSMESSAGE_COUNTING;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	
	int nMsgId = 0;
	
	int nTime = (int)Lua_ValueToNumber(L,2);
	
	if (nTime <= 0)
		nTime = 1;
	
	if (Lua_IsNumber(L,1))
	{
		nMsgId = (int)Lua_ValueToNumber(L,1);
		*((int *)(UiInfo.m_pContent)) = nMsgId;
		UiInfo.m_bParam1 = 1;
		*(int *)((char *)UiInfo.m_pContent + sizeof(int)) = nTime;
		UiInfo.m_nBufferLen = sizeof(int) * 2;
	}
	else 
	{
		g_StrCpyLen(UiInfo.m_pContent, Lua_ValueToString(L,1), 512);
		UiInfo.m_nBufferLen = strlen(((char *)UiInfo.m_pContent));
		*(int *)((char *)UiInfo.m_pContent + UiInfo.m_nBufferLen) = nTime;
		UiInfo.m_nBufferLen += sizeof(int);
		UiInfo.m_bParam1 = 0;
	}
	
#ifndef _SERVER
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) return 0;

	UiInfo.m_bParam2 = 0;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
#else
	UiInfo.m_bParam2 = 1;
	UiInfo.ProtocolType = (BYTE)s2c_scriptaction;
	UiInfo.m_wProtocolLong = sizeof(PLAYER_SCRIPTACTION_SYNC) - MAX_SCIRPTACTION_BUFFERNUM + UiInfo.m_nBufferLen - 1;
	g_NewProtocolProcess.BroadcastLocalServer(&UiInfo, UiInfo.m_wProtocolLong + 1);
#endif
	return 0;
}

//AddGlobalTimeNews(strNew/newid, year,month,day,hour,mins)
int LuaAddGlobalTimeNews(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 6) 
		return 0;
	
	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bUIId = UI_NEWSINFO;
	UiInfo.m_bOptionNum = NEWSMESSAGE_TIMEEND;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	
	int nMsgId = 0;
	
	if (Lua_IsNumber(L,1))
	{
		nMsgId = (int)Lua_ValueToNumber(L,1);
		*((int *)(UiInfo.m_pContent)) = nMsgId;
		UiInfo.m_bParam1 = 1;
		UiInfo.m_nBufferLen = sizeof(int) + sizeof(SYSTEMTIME);
	}
	else 
	{
		g_StrCpyLen(UiInfo.m_pContent, Lua_ValueToString(L,1), 512);
		UiInfo.m_nBufferLen = strlen(((char *)UiInfo.m_pContent)) + sizeof(SYSTEMTIME);
		UiInfo.m_bParam1 = 0;
	}
	
	SYSTEMTIME *pSystemTime = 	(SYSTEMTIME *)((char *)UiInfo.m_pContent + UiInfo.m_nBufferLen - sizeof(SYSTEMTIME));
	memset(pSystemTime, 0, sizeof(SYSTEMTIME));
	
	SYSTEMTIME LocalTime ;
	memset(&LocalTime, 0, sizeof(SYSTEMTIME));
	
	LocalTime.wYear = (WORD)Lua_ValueToNumber(L,2);
	LocalTime.wMonth =(WORD)Lua_ValueToNumber(L,3);
	LocalTime.wDay = (WORD)Lua_ValueToNumber(L, 4);
	LocalTime.wHour = (WORD)Lua_ValueToNumber(L,5);
	LocalTime.wMinute = (WORD)Lua_ValueToNumber(L,6);
	FILETIME ft;
	FILETIME sysft;
#ifdef WIN32
	SystemTimeToFileTime(&LocalTime, &ft);
	LocalFileTimeToFileTime(&ft, &sysft);
	FileTimeToSystemTime(&sysft, pSystemTime);
#else
	memcpy(pSystemTime, &LocalTime, sizeof(LocalTime));
#endif
	
#ifndef _SERVER
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) return 0;

	UiInfo.m_bParam2 = 0;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
#else
	UiInfo.m_bParam2 = 1;
	UiInfo.ProtocolType = (BYTE)s2c_scriptaction;
	UiInfo.m_wProtocolLong = sizeof(PLAYER_SCRIPTACTION_SYNC) - MAX_SCIRPTACTION_BUFFERNUM + UiInfo.m_nBufferLen - 1;
	g_NewProtocolProcess.BroadcastGlobal(&UiInfo, UiInfo.m_wProtocolLong + 1);
#endif
	return 0;
}


int LuaAddGlobalTimeNews2(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 6) 
		return 0;
	
	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bUIId = UI_NEWSINFO1;
	UiInfo.m_bOptionNum = NEWSMESSAGE_TIMEEND;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	
	int nMsgId = 0;
	
	if (Lua_IsNumber(L,1))
	{
		nMsgId = (int)Lua_ValueToNumber(L,1);
		*((int *)(UiInfo.m_pContent)) = nMsgId;
		UiInfo.m_bParam1 = 1;
		UiInfo.m_nBufferLen = sizeof(int) + sizeof(SYSTEMTIME);
	}
	else 
	{
		g_StrCpyLen(UiInfo.m_pContent, Lua_ValueToString(L,1), 512);
		UiInfo.m_nBufferLen = strlen(((char *)UiInfo.m_pContent)) + sizeof(SYSTEMTIME);
		UiInfo.m_bParam1 = 0;
	}
	
	SYSTEMTIME *pSystemTime = 	(SYSTEMTIME *)((char *)UiInfo.m_pContent + UiInfo.m_nBufferLen - sizeof(SYSTEMTIME));
	memset(pSystemTime, 0, sizeof(SYSTEMTIME));
	
	SYSTEMTIME LocalTime ;
	memset(&LocalTime, 0, sizeof(SYSTEMTIME));
	
	LocalTime.wYear = (WORD)Lua_ValueToNumber(L,2);
	LocalTime.wMonth =(WORD)Lua_ValueToNumber(L,3);
	LocalTime.wDay = (WORD)Lua_ValueToNumber(L, 4);
	LocalTime.wHour = (WORD)Lua_ValueToNumber(L,5);
	LocalTime.wMinute = (WORD)Lua_ValueToNumber(L,6);
	FILETIME ft;
	FILETIME sysft;
#ifdef WIN32
	SystemTimeToFileTime(&LocalTime, &ft);
	LocalFileTimeToFileTime(&ft, &sysft);
	FileTimeToSystemTime(&sysft, pSystemTime);
#else
	memcpy(pSystemTime, &LocalTime, sizeof(LocalTime));
#endif
	
#ifndef _SERVER
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) return 0;

	UiInfo.m_bParam2 = 0;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
#else
	UiInfo.m_bParam2 = 1;
	UiInfo.ProtocolType = (BYTE)s2c_scriptaction;
	UiInfo.m_wProtocolLong = sizeof(PLAYER_SCRIPTACTION_SYNC) - MAX_SCIRPTACTION_BUFFERNUM + UiInfo.m_nBufferLen - 1;
	g_NewProtocolProcess.BroadcastGlobal(&UiInfo, UiInfo.m_wProtocolLong + 1);
#endif
	return 0;
}

//AddLocalTimeNews(strNew/newid, year,month,day,hour,mins)
int LuaAddLocalTimeNews(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 6) 
		return 0;
	
	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bUIId = UI_NEWSINFO;
	UiInfo.m_bOptionNum = NEWSMESSAGE_TIMEEND;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	
	int nMsgId = 0;
	
	if (Lua_IsNumber(L,1))
	{
		nMsgId = (int)Lua_ValueToNumber(L,1);
		*((int *)(UiInfo.m_pContent)) = nMsgId;
		UiInfo.m_bParam1 = 1;
		UiInfo.m_nBufferLen = sizeof(int) + sizeof(SYSTEMTIME);
	}
	else 
	{
		g_StrCpyLen(UiInfo.m_pContent, Lua_ValueToString(L,1), 512);
		UiInfo.m_nBufferLen = strlen(((char *)UiInfo.m_pContent)) + sizeof(SYSTEMTIME);
		UiInfo.m_bParam1 = 0;
	}
	
	SYSTEMTIME *pSystemTime = 	(SYSTEMTIME *)((char *)UiInfo.m_pContent + UiInfo.m_nBufferLen - sizeof(SYSTEMTIME));
	memset(pSystemTime, 0, sizeof(SYSTEMTIME));
	
	SYSTEMTIME LocalTime ;
	memset(&LocalTime, 0, sizeof(SYSTEMTIME));
	
	LocalTime.wYear = (WORD)Lua_ValueToNumber(L,2);
	LocalTime.wMonth =(WORD)Lua_ValueToNumber(L,3);
	LocalTime.wDay = (WORD)Lua_ValueToNumber(L, 4);
	LocalTime.wHour = (WORD)Lua_ValueToNumber(L,5);
	LocalTime.wMinute = (WORD)Lua_ValueToNumber(L,6);
	FILETIME ft;
	FILETIME sysft;
#ifdef WIN32
	SystemTimeToFileTime(&LocalTime, &ft);
	LocalFileTimeToFileTime(&ft, &sysft);
	FileTimeToSystemTime(&sysft, pSystemTime);
#else
	memcpy(pSystemTime, &LocalTime, sizeof(LocalTime));
#endif
	
#ifndef _SERVER
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) return 0;

	UiInfo.m_bParam2 = 0;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
#else
	UiInfo.m_bParam2 = 1;
	UiInfo.ProtocolType = (BYTE)s2c_scriptaction;
	UiInfo.m_wProtocolLong = sizeof(PLAYER_SCRIPTACTION_SYNC) - MAX_SCIRPTACTION_BUFFERNUM + UiInfo.m_nBufferLen - 1;
	g_NewProtocolProcess.BroadcastLocalServer(&UiInfo, UiInfo.m_wProtocolLong + 1);
#endif
	return 0;
}

//AddNote(str/strid)
int LuaAddNote(Lua_State * L)
{
	//int nMainInfo = 0;
	//int nDataType = 0;
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0)
		return 0;
	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 3) 
		return 0;
	
	
	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bUIId = UI_NOTEINFO;
	UiInfo.m_bParam1 = 0;//nDataType;//主信息的类型，字符串(0)或数字(1)
#ifndef _SERVER
	UiInfo.m_bParam2 = 0;
#else
	UiInfo.m_bParam2 = 1;
#endif
	
	UiInfo.m_bOptionNum = 0;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	UiInfo.m_bParam = FALSE;

	int nParam2 = 0;

	if  (Lua_IsNumber(L,3))
	{
		UiInfo.m_bParam1 = 1 ;
	}
	else if (Lua_IsString(L, 3))
	{
		UiInfo.m_bParam1 = 0 ;
	}
	else
		return 0;	

	if (nParamNum > 3)
	{
		nParam2 = (int)Lua_ValueToNumber(L, 4);
	}

	sprintf(UiInfo.m_pContent, "%s|%s",(char *)Lua_ValueToString(L, 1),
		(char *)Lua_ValueToString(L, 3));
	UiInfo.m_nParam = (int)Lua_ValueToNumber(L,2);

	int nLen = strlen(UiInfo.m_pContent);
	*(int*)(UiInfo.m_pContent + nLen) = nParam2;
	UiInfo.m_nBufferLen = nLen + sizeof(int)+1;

	Player[nPlayerIndex].DoScriptAction(&UiInfo);
	return 0;
}

int LuaAddMissionNote(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0)
		return 0;
	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 3) 
		return 0;
	
	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bUIId = UI_NOTEINFO;
	UiInfo.m_bParam1 = 0;//nDataType;//主信息的类型，字符串(0)或数字(1)
#ifndef _SERVER
	UiInfo.m_bParam2 = 0;
#else
	UiInfo.m_bParam2 = 1;
#endif
	
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	UiInfo.m_bParam = TRUE;

	int nParam2 = 0;

	if  (Lua_IsNumber(L,2))
	{
		UiInfo.m_bParam1 = 1 ;
	}
	else if (Lua_IsString(L, 2))
	{
		UiInfo.m_bParam1 = 0 ;
	}
	else
		return 0;

	if (nParamNum > 2)
	{
		nParam2 = (int)Lua_ValueToNumber(L, 3);
	}

	sprintf(UiInfo.m_pContent, "%s|%s",(char *)Lua_ValueToString(L, 1),
		(char *)Lua_ValueToString(L, 2));
	
	int nLen = strlen(UiInfo.m_pContent);
	*(int*)(UiInfo.m_pContent + nLen) = nParam2;
	UiInfo.m_nBufferLen = nLen + sizeof(int)+1;

	Player[nPlayerIndex].DoScriptAction(&UiInfo);
	return 0;
}
/*
**
**格式1:Talk(SentenceNum, CallBack-Fun(结束后的回调函数), sTalk1, sTalk2, sTalk3, sTalk4,...sTalkN);  
Talk(SentenceNum, CallBack-Fun(结束后的回调函数), nTalk1, nTalk2,nTalk3,nTalk4,...nTalkN);  
**格式2:Talk(SentenceNum, CallBack-Fun, SentenceTab);
**例子:Talk(3,"EndTalk", "玩家：请问现在几点钟了？", "雇员：现在5点钟了","太谢谢你了！");
**
*/

int LuaTalkUI(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) 
		return 0;
	Player[nPlayerIndex].m_bWaitingPlayerFeedBack = false;
	int nMainInfo = 0;
	int nDataType = 0;
	int nOptionNum = 0;
	char * pContent = NULL;
	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 3) 
		return 0;
	
	if (Lua_IsNumber(L,1))
	{
		nOptionNum = (int)Lua_ValueToNumber(L,1);
	}
	else 
	{
		_ASSERT(0);
		return 0;
	}
	
	const char * pCallBackFun = Lua_ValueToString(L,2);
	
	//检查主信息是字符串还是字符串标识号
	
	if  (Lua_IsNumber(L,3))
	{
		nDataType = 1 ;
	}
	else if (Lua_IsString(L, 3)) 
	{
		nDataType = 0 ;
	}
	else
		return 0;
	
	
	//获得实际传入的选项的个数
	if (nOptionNum > nParamNum - 2) 
		nOptionNum = nParamNum - 2;
	
	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bUIId = UI_TALKDIALOG;
	UiInfo.m_bParam1 = nDataType;//主信息的类型，字符串(0)或数字(1)
	UiInfo.m_bOptionNum = nOptionNum;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	UiInfo.m_bParam = FALSE;
	memset(UiInfo.m_szKey, 0, sizeof(UiInfo.m_szKey));
	UiInfo.m_nParam1 = 0;
	pContent = UiInfo.m_pContent;
	pContent[0] = 0;
	size_t nContentLen = 0;
	for (int i  = 0; i < nOptionNum; i ++)
	{	
		const char * pString = NULL;
		if (!nDataType)//StringInfo
		{
			pString = Lua_ValueToString(L, i + 3);
			if (nContentLen  + strlen(pString) >= MAX_SCIRPTACTION_BUFFERNUM)
			{
				nOptionNum = i;
				UiInfo.m_bOptionNum = nOptionNum;
				break;
			}
			nContentLen += strlen(pString);
			sprintf(pContent, "%s%s|", pContent, pString);
		}
		else
		{
			int j = (int)Lua_ValueToNumber(L, i + 3);
			sprintf(pContent, "%s%d|", pContent, j);
		}
	}
	UiInfo.m_nBufferLen  = strlen(pContent);
	
	if (!pCallBackFun || strlen(pCallBackFun) <= 0)
	{
		UiInfo.m_nParam = 0;
		Player[nPlayerIndex].m_nAvailableAnswerNum = 0;
		Player[nPlayerIndex].m_bWaitingPlayerFeedBack = false;
	}
	else
	{
		UiInfo.m_nParam = 1;
		Player[nPlayerIndex].m_nAvailableAnswerNum = 1;
		g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerFun[0], pCallBackFun, sizeof(Player[nPlayerIndex].m_szTaskAnswerFun[0]));
		Player[nPlayerIndex].m_bWaitingPlayerFeedBack = true;
	}
	
#ifndef _SERVER
	UiInfo.m_bParam2 = 0;
#else
	UiInfo.m_bParam2 = 1;
#endif
	Player[nPlayerIndex].m_TalkUiScriptId = Npc[Player[nPlayerIndex].m_nIndex].m_ActionScriptID;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
	return 0;
	
}

int LuaSelUI(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) 
		return 0;
	Player[nPlayerIndex].m_bWaitingPlayerFeedBack = false;
	int nMainInfo = 0;
	int nDataType = 0;
	int nOptionNum = 3;
	char * pContent = NULL;
	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 4) 
		return 0;

	const char * pCallBackFun = Lua_ValueToString(L,1);
	
	//检查主信息是字符串还是字符串标识号
	
	if  (Lua_IsNumber(L,2))
	{
		nDataType = 1 ;
	}
	else if (Lua_IsString(L, 2)) 
	{
		nDataType = 0 ;
	}
	else
		return 0;
	
	
	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bUIId = UI_TALKDIALOG;
	UiInfo.m_bParam1 = nDataType;//主信息的类型，字符串(0)或数字(1)
	UiInfo.m_bOptionNum = nOptionNum;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	UiInfo.m_bParam = TRUE;
	memset(UiInfo.m_szKey, 0, sizeof(UiInfo.m_szKey));
	if (nParamNum > 5)
		UiInfo.m_nParam1 = (int)Lua_ValueToNumber(L, 5);
	else
		UiInfo.m_nParam1 = -1;
	pContent = UiInfo.m_pContent;
	pContent[0] = 0;
	size_t nContentLen = 0;
	for (int i  = 0; i < nOptionNum; i ++)
	{	
		const char * pString = NULL;
		if (!nDataType)//StringInfo
		{
			pString = Lua_ValueToString(L, i + 2);
			if (nContentLen  + strlen(pString) >= MAX_SCIRPTACTION_BUFFERNUM)
			{
				nOptionNum = i;
				UiInfo.m_bOptionNum = nOptionNum;
				break;
			}
			nContentLen += strlen(pString);
			sprintf(pContent, "%s%s|", pContent, pString);
		}
		else
		{
			int j = (int)Lua_ValueToNumber(L, i + 2);
			sprintf(pContent, "%s%d|", pContent, j);
		}
	}

	UiInfo.m_nBufferLen  = strlen(pContent);
	
	if (!pCallBackFun || strlen(pCallBackFun) <= 0)
	{
		UiInfo.m_nParam = 0;
		Player[nPlayerIndex].m_nAvailableAnswerNum = 0;
		Player[nPlayerIndex].m_bWaitingPlayerFeedBack = false;
	}
	else
	{
		UiInfo.m_nParam = 1;
		Player[nPlayerIndex].m_nAvailableAnswerNum = 2;
		g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerFun[0], pCallBackFun, sizeof(Player[nPlayerIndex].m_szTaskAnswerFun[0]));
		Player[nPlayerIndex].m_bWaitingPlayerFeedBack = true;
	}
	
#ifndef _SERVER
	UiInfo.m_bParam2 = 0;
#else
	UiInfo.m_bParam2 = 1;
#endif
	Player[nPlayerIndex].m_TalkUiScriptId = Npc[Player[nPlayerIndex].m_nIndex].m_ActionScriptID;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
	return 0;
	
}


int LuaIncludeFile(Lua_State * L)
{
	if (Lua_GetTopIndex(L) <= 0) return 0;
	
	if (Lua_IsString(L,1))
	{
		const char * pFileName = lua_tostring(L,1);	
		char lszCurrentDirectory[MAX_PATH];
		int nLen = 0;
		if (pFileName[0] != '\\' && pFileName[0] != '/')
		{
			getcwd(lszCurrentDirectory, MAX_PATH);
			nLen = strlen(lszCurrentDirectory);
			if (lszCurrentDirectory[nLen - 1] == '\\' || lszCurrentDirectory[nLen - 1] == '/')
				lszCurrentDirectory[nLen - 1] = 0;
#ifdef WIN32
			g_StrCat(lszCurrentDirectory, "\\");
			g_StrCat(lszCurrentDirectory, (char*)pFileName);
#else
			g_StrCat(lszCurrentDirectory, "/");
			g_StrCat(lszCurrentDirectory, (char*)pFileName);
			for (int i = 0; lszCurrentDirectory[i]; i++)
			{
				if (lszCurrentDirectory[i] == '\\')
					lszCurrentDirectory[i] = '/';
			}
#endif
		}
		else
		{
			g_GetRootPath(lszCurrentDirectory);
			nLen = strlen(lszCurrentDirectory);
			if (lszCurrentDirectory[nLen - 1] == '\\' || lszCurrentDirectory[nLen - 1] == '/')
				lszCurrentDirectory[nLen - 1] = 0;
#ifdef WIN32
			g_StrCat(lszCurrentDirectory, "\\");
			g_StrCat(lszCurrentDirectory, (char*)pFileName + 1);
#else
			g_StrCat(lszCurrentDirectory, "/");
			g_StrCat(lszCurrentDirectory, (char*)pFileName + 1);
			for (int i = 0; lszCurrentDirectory[i]; i++)
			{
				if (lszCurrentDirectory[i] == '\\')
					lszCurrentDirectory[i] = '/';
			}
#endif
		}
		strlwr(lszCurrentDirectory + nLen);
		lua_dofile(L, lszCurrentDirectory);
		return 0;
	}
	else
		return 0;
}

//**************************************************************************************************************************************************************
//												任务脚本
//**************************************************************************************************************************************************************
int LuaGetTaskValue(Lua_State * L)
{
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0) 
	{
		int nValue = 0;
#ifdef _SERVER
		nValue = Player[nPlayerIndex].m_cTask.GetSaveVal((int)Lua_ValueToNumber(L,1));	
#endif
		Lua_PushNumber(L, nValue);
	}
	else
		Lua_PushNil(L);
	
	return 1;
}

int LuaGetTaskString(Lua_State * L)
{
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0) 
	{
#ifdef _SERVER
		Lua_PushString(L, Player[nPlayerIndex].m_cTask.GetSaveStr((int)Lua_ValueToNumber(L,1)));	
#endif
	}
	else
		Lua_PushNil(L);
	
	return 1;
}

int LuaSetTaskValue(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nValueIndex = (int)Lua_ValueToNumber(L, 1);

	if (nPlayerIndex <= 0) return 0;

	if(nValueIndex >MAX_TASK_SCRIPTFUNC)
		return 0;
#ifdef _SERVER
		Player[nPlayerIndex].m_cTask.SetSaveVal(nValueIndex, (char*)Lua_ValueToString(L, 2));
#endif
	return 0;
}

int LuaSyncTaskValue(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nValueIndex = (int)Lua_ValueToNumber(L, 1);

	if (nPlayerIndex <= 0) return 0;
#ifdef _SERVER
	Player[nPlayerIndex].m_cTask.SyncTaskValue(nValueIndex);
#endif
	return 0;
}

#ifndef _SERVER
#define MAX_TEMPVALUENUM_INCLIENT 500
int g_TempValue[MAX_TEMPVALUENUM_INCLIENT];
#endif

int LuaGetTempTaskValue(Lua_State * L)
{
	int nTempIndex = (int)Lua_ValueToNumber(L, Lua_GetTopIndex(L));
	
#ifdef _SERVER
	if(nTempIndex >= MAX_TEMP_TASK)
	{
		Lua_PushNil(L) ;
		return 1;
	}
	int nPlayerIndex = GetPlayerIndex(L);
	
	if (nPlayerIndex <= 0) 
	{
		Lua_PushNil(L);
		return 1;
	}
	
	int nValue = Player[nPlayerIndex].m_cTask.GetClearVal(nTempIndex);
	Lua_PushNumber(L, nValue);
#else
	
	if (nTempIndex >= 0 && nTempIndex < MAX_TEMPVALUENUM_INCLIENT)
		Lua_PushNumber(L, g_TempValue[nTempIndex]);
	else 
		Lua_PushNil(L);
#endif
	return 1;
}

int LuaGetTempTaskString(Lua_State * L)
{
	int nTempIndex = (int)Lua_ValueToNumber(L, Lua_GetTopIndex(L));
	
#ifdef _SERVER
	if(nTempIndex >= MAX_TEMP_TASK)
	{
		Lua_PushNil(L) ;
		return 1;
	}
	int nPlayerIndex = GetPlayerIndex(L);
	
	if (nPlayerIndex <= 0) 
	{
		Lua_PushNil(L);
		return 1;
	}
	
	Lua_PushString(L, (char*)Player[nPlayerIndex].m_cTask.GetClearStr(nTempIndex));
#else
	
	if (nTempIndex >= 0 && nTempIndex < MAX_TEMPVALUENUM_INCLIENT)
		Lua_PushNumber(L, g_TempValue[nTempIndex]);
	else 
		Lua_PushNil(L);
#endif
	return 1;
}

int LuaSetTempTaskValue(Lua_State * L)
{
	int nTempIndex = (int)Lua_ValueToNumber(L, Lua_GetTopIndex(L) - 1);
	char* szValue = (char*)Lua_ValueToString(L, Lua_GetTopIndex(L));
#ifdef _SERVER	
	Lua_GetGlobal(L, SCRIPT_PLAYERINDEX);
	int nPlayerIndex = (int)Lua_ValueToNumber(L, Lua_GetTopIndex(L));
	if (nPlayerIndex <= 0) return 0;
	Player[nPlayerIndex].m_cTask.SetClearVal(nTempIndex, szValue);
#else
	g_TempValue[nTempIndex] = atoi(szValue);
#endif
	return 0;
}

#ifdef _SERVER
//---------------------------------交易、买卖、打开储藏箱-----------------------
//Sale(id)
//------------------------------------------------------------------------------
int LuaSale(Lua_State * L)
{
	if (Lua_GetTopIndex(L) <= 0) return 0;
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nShopId = (int)Lua_ValueToNumber(L,1);
		int nShopMoneyUnit = moneyunit_money;
		//----在以下加入买卖的实际代码!
		if (Lua_GetTopIndex(L) > 2)
			nShopMoneyUnit = (int)Lua_ValueToNumber(L,2);
		BuySell.OpenSale(nPlayerIndex, nShopId - 1, nShopMoneyUnit);
	}
	return 0;
}

int LuaNewSale(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) return 0;

	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 4) return 0;

	int nShopNum = (int)Lua_ValueToNumber(L, 3);

	if (nShopNum > MAX_SUPERSHOP_SHOPTAB)
		nShopNum = MAX_SUPERSHOP_SHOPTAB;

	int nShopId[MAX_SUPERSHOP_SHOPTAB];
	for (int i = 0; i < nShopNum; i++)	
		nShopId[i] = (int)Lua_ValueToNumber(L, 4 + i) - 1;
	

	for (int i = 0; i < MAX_SUPERSHOP_SHOPTAB; i++)
		nShopId[i] = -1;
	

	BuySell.OpenSale(nPlayerIndex, (int)Lua_ValueToNumber(L, 1), (int)Lua_ValueToNumber(L, 2), nShopNum, nShopId);
	return 0;
}

int LuaOpenBox(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	
	SOME_BOX_SYNC command;
	command.ProtocolType = (BYTE)s2c_opensomebox;
	command.bEquipEx = FALSE;
	command.nBoxIndex = -1;
	if (Lua_GetTopIndex(L)>1)
		command.nBoxIndex = (int)Lua_ValueToNumber(L,1) -1;
	g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &command, sizeof(SOME_BOX_SYNC));
	return 0;
}

int LuaOpenEquipEx(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	
	SOME_BOX_SYNC command;
	command.ProtocolType = (BYTE)s2c_opensomebox;
	command.bEquipEx = TRUE;
	command.nBoxIndex = 0;
	g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &command, sizeof(SOME_BOX_SYNC));
	return 0;
}

//---------------------------------时间任务-------------------------------------
//SetTimer(Time, TimerTaskId)
int LuaSetTimer(Lua_State  * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount < 2 ) return 0;
	int nPlayerIndex  = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	Player[nPlayerIndex].SetTimer((DWORD) (int)Lua_ValueToNumber(L, 1), (int)Lua_ValueToNumber(L,2));
	return 0;
}

int LuaStopTimer(Lua_State * L)
{
	int nPlayerIndex  = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	Player[nPlayerIndex].CloseTimer();
	return 0;
}

int LuaGetCurTimerId(Lua_State * L)
{
	int nPlayerIndex  = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) 
	{
		Lua_PushNumber(L,0);
		return 1;
	}
	int nTimerId = Player[nPlayerIndex].m_TimerTask.GetTaskId();
	Lua_PushNumber(L, nTimerId);
	return 1;
}

int LuaGetRestTime(Lua_State * L)
{
	int nPlayerIndex  = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) 
	{
		Lua_PushNil(L);
		return 1;
	}
	int nRestTime = Player[nPlayerIndex].m_TimerTask.GetRestTime();//m_dwTimeTaskTime - g_SubWorldSet.GetGameTime();
	
	if (nRestTime > 0)
		Lua_PushNumber(L, nRestTime);
	else
		Lua_PushNumber(L, 0);
	
	return 1;
}

int LuaGetMissionRestTime(Lua_State * L)
{
	int RestTime = 0;
	if (Lua_GetTopIndex(L) >= 2)
	{
		int nSubWorldIndex = GetSubWorldIndex(L);
		if (nSubWorldIndex >= 0) 
		{
			int nMissionId = (int)Lua_ValueToNumber(L, 1);
			int nTimerId = (int)Lua_ValueToNumber(L, 2);
			
			if (nMissionId < 0 || nTimerId < 0 )
				goto lab_getmissionresttime;
			
			KMission Mission;
			Mission.SetMissionId(nMissionId);
			KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
			if (pMission)
			{
				RestTime = (int)pMission->GetTimerRestTimer(nTimerId);
			}
		}
	}
	
lab_getmissionresttime:
	Lua_PushNumber(L, RestTime);
	return 1;
}


//**************************************************************************************************************************************************************
//												组队脚本
//**************************************************************************************************************************************************************
int LuaIsLeader(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0 ) 
	{
		if (Player[nPlayerIndex].m_cTeam.m_nFlag && Player[nPlayerIndex].m_cTeam.m_nFigure == TEAM_CAPTAIN)
			Lua_PushNumber(L,1);
		else 
			Lua_PushNumber(L,0);
	}
	else 
		Lua_PushNumber(L, 0);

	return 1;
}

int LuaGetTeamId(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		if (Player[nPlayerIndex].m_cTeam.m_nFlag)
			Lua_PushNumber(L, Player[nPlayerIndex].m_cTeam.m_nID);
		else
			Lua_PushNil(L);
	}
	else 
		Lua_PushNil(L);

	return 1;
}

int LuaTeamDoScript(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	char* nValueIndex = (char*)Lua_ValueToString(L, 1);
	char* sFunc = (char*)Lua_ValueToString(L, 2);
	int	Value1 = (int)Lua_ValueToNumber(L, 3);
	int	Value2 = (int)Lua_ValueToNumber(L, 4);
	int	Value3 = (int)Lua_ValueToNumber(L, 5);
	DWORD ScriptID = g_FileName2Id(nValueIndex);
	int Count = 0;
	if (nPlayerIndex < 0)
		return 0;
	if (Player[nPlayerIndex].m_cTeam.m_nID >= 0)
	{
		int nTeamLeaderId = g_Team[Player[nPlayerIndex].m_cTeam.m_nID].m_nCaptain;
		if (nTeamLeaderId)
		{	
			Player[nTeamLeaderId].ExecuteScript3Param(ScriptID, sFunc, 0, Value1, Value2, Value3);
			Count = Count + 1;
		}
		else
			return 0;
		for (int i = 0; i < MAX_TEAM_MEMBER; i ++)
		{
			int nMemberId = g_Team[Player[nPlayerIndex].m_cTeam.m_nID].m_nMember[i] ;
			if (nMemberId)
			{
				Player[nMemberId].ExecuteScript3Param(ScriptID, sFunc, 0, Value1, Value2, Value3);
				Count = Count + 1;
				Sleep(5);
			}
		}
	}
	Lua_PushNumber(L,Count);
	return Count;
}

int LuaGetTeamSize(Lua_State * L)
{
	int nTeamSize = 0;
	int nTeamId = -1;
	if (Lua_GetTopIndex(L) >= 1)
	{
		nTeamId = Lua_ValueToNumber(L, 1);
	}
	else
	{
		int nPlayerIndex = GetPlayerIndex(L);
		if (nPlayerIndex > 0)
		{
			if (Player[nPlayerIndex].m_cTeam.m_nFlag)
				nTeamId = Player[nPlayerIndex].m_cTeam.m_nID;
			else 
				nTeamId = -1;
		}
	}
	
	if (nTeamId < 0)
		nTeamSize = 0;
	else
		nTeamSize = g_Team[nTeamId].m_nMemNum + 1;
	Lua_PushNumber(L, nTeamSize);
	return 1;
}

int LuaGetTeamMem(Lua_State * L)
{
	int nTeamId = -1;
	int nMemId = 0;
	int nResult =0;
	if (Lua_GetTopIndex(L) >= 2)
	{
		nTeamId = Lua_ValueToNumber(L, 1);
		nMemId = Lua_ValueToNumber(L, 2);
	}
	else
	{
		int nPlayerIndex = GetPlayerIndex(L);
		if (nPlayerIndex > 0)
		{
			if (Player[nPlayerIndex].m_cTeam.m_nFlag)
				nTeamId = Player[nPlayerIndex].m_cTeam.m_nID;
			else 
				nTeamId = -1;
			nMemId = Lua_ValueToNumber(L, 1);
		}
	}
	
	if (nTeamId >= 0)
		nResult = nMemId==0?g_Team[nTeamId].m_nCaptain:g_Team[nTeamId].m_nMember[nMemId-1];

	Lua_PushNumber(L, nResult);
	return 1;
}

int LuaLeaveTeam(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		if (Player[nPlayerIndex].m_cTeam.m_nFlag)
		{
			PLAYER_APPLY_LEAVE_TEAM	sLeaveTeam;
			sLeaveTeam.ProtocolType = c2s_teamapplyleave;
			sLeaveTeam.bMySelf = TRUE;
			Player[nPlayerIndex].LeaveTeam((BYTE*)&sLeaveTeam);
		}
	}
	return 0;
}

int LuaSetCreateTeamOption(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		Player[nPlayerIndex].m_cTeam.SetCreatTeamFlag(nPlayerIndex, (int)Lua_ValueToNumber(L, 1) > 0);
	}
	return 0;
}

int LuaSetFreezeTeamOption(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		Player[nPlayerIndex].m_cTeam.SetFreezeTeamFlag(nPlayerIndex, (int)Lua_ValueToNumber(L, 1) > 0);
	}
	return 0;
}
//**************************************************************************************************************************************************************
//												聊天消息脚本
//**************************************************************************************************************************************************************
extern TLua_Funcs GameScriptFuns[];
extern TLua_Funcs WorldScriptFuns[];

int g_GetGameScriptFunNum();
int g_GetWorldScriptFunNum();
//**************************************************************************************************************************************************************
//												聊天消息脚本
//**************************************************************************************************************************************************************

int	LuaMsgToPlayer(Lua_State * L)
{
	if (Lua_GetTopIndex(L) <= 0) return 0;
	int nParamNum = Lua_GetTopIndex(L);
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0) 
	{
		const char *  szMsg = (char*)Lua_ValueToString(L,1);
		if (szMsg)
		{
			int nChannelID = -1;
			if(nParamNum > 1)
				nChannelID = (int)Lua_ValueToNumber(L, 2);
			KPlayerChat::SendSystemInfo(1, nPlayerIndex, MESSAGE_BROADCAST_ANNOUCE_HEAD, (char *) szMsg, strlen(szMsg) ,nChannelID);
		}
	}	
	
	return 0;
}

int LuaMsgToTeam(Lua_State * L)
{
	if (Lua_GetTopIndex(L) <= 0	) return 0;
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		if (Player[nPlayerIndex].m_cTeam.m_nID >= 0)
		{
			const char * szMsg = Lua_ValueToString(L,1);
			Player[nPlayerIndex].SendTeamMessage(Player[nPlayerIndex].m_cTeam.m_nID, szMsg);
		}
	}
	return 0;
}

int LuaMsgToSubWorld(Lua_State * L)
{
	if (Lua_GetTopIndex(L) <= 0 ) return 0;
	int nParamNum = Lua_GetTopIndex(L);

	const char *  szMsg = (char*)Lua_ValueToString(L,1);
	if (szMsg)
	{
		int nChannelID = -1;
		if(nParamNum > 1)
			nChannelID = (int)Lua_ValueToNumber(L, 2);
		KPlayerChat::SendSystemInfo(0, 0, MESSAGE_BROADCAST_ANNOUCE_HEAD, (char *) szMsg, strlen(szMsg) ,nChannelID);
	}
		
	return 0;
}

int LuaMsgToTong(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if(nPlayerIndex <= 0) return 0;
	if(!Player[nPlayerIndex].m_cTong.m_nFlag)
		return 0;

	STONG_MESSAGE_INFO_COMMAND	TongMsg;
	TongMsg.ProtocolID = enumC2S_MESSAGE_TO_TONG;
	TongMsg.ProtocolFamily = pf_tong;
	TongMsg.dwParam = Player[nPlayerIndex].m_cTong.m_dwTongNameID;
	strcpy(TongMsg.szName, Player[nPlayerIndex].Name);
	strcpy(TongMsg.szMsg, (char *)Lua_ValueToString(L, 1));
	if (g_pTongClient)
		g_pTongClient->SendPackToServer((const void*)&TongMsg, sizeof(STONG_MESSAGE_INFO_COMMAND));

	return 0;
}

int LuaMsgToFaction(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if(nPlayerIndex <= 0) return 0;
	if(Player[nPlayerIndex].m_cFaction.m_nCurFaction < 0)
		return 0;

	STONG_MESSAGE_INFO_COMMAND	FactionMsg;
	FactionMsg.ProtocolID = enumC2S_MESSAGE_TO_FACTION;
	FactionMsg.ProtocolFamily = pf_tong;
	FactionMsg.dwParam = Player[nPlayerIndex].m_cFaction.m_nCurFaction;
	strcpy(FactionMsg.szName, Player[nPlayerIndex].Name);
	strcpy(FactionMsg.szMsg, (char *)Lua_ValueToString(L, 1));
	if (g_pTongClient)
		g_pTongClient->SendPackToServer((const void*)&FactionMsg, sizeof(STONG_MESSAGE_INFO_COMMAND));

	return 0;
}

int LuaMsgToChatRoom(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if(nPlayerIndex <= 0) return 0;
	if(!Player[nPlayerIndex].m_cRoom.m_nFlag)
		return 0;

	STONG_MESSAGE_INFO_COMMAND	RoomMsg;
	RoomMsg.ProtocolID = enumC2S_MESSAGE_TO_CHATROOM;
	RoomMsg.ProtocolFamily = pf_tong;
	RoomMsg.dwParam = Player[nPlayerIndex].m_cRoom.m_nID;
	strcpy(RoomMsg.szName, Player[nPlayerIndex].Name);
	strcpy(RoomMsg.szMsg, (char *)Lua_ValueToString(L, 1));
	if (g_pTongClient)
		g_pTongClient->SendPackToServer((const void*)&RoomMsg, sizeof(STONG_MESSAGE_INFO_COMMAND));

	return 0;
}

int LuaMsgToAroundRegion(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if(nPlayerIndex <= 0) return 0;
	int nParamNum = Lua_GetTopIndex(L);
	
	int nSubWorldIndex = g_SubWorldSet.SearchWorld((int)Lua_ValueToNumber(L,1));

	if (nSubWorldIndex >= 0 && nSubWorldIndex < MAX_SUBWORLD)
	{
		const char *  szMsg = (char*)Lua_ValueToString(L,2);
		int nChannelID = -1;
		if(nParamNum > 3)
			nChannelID = (int)Lua_ValueToNumber(L, 3);
		if (szMsg)
		{
			int nIndex = PlayerSet.GetFirstPlayer();
			while(nIndex > 0)
			{
				if (Npc[Player[nIndex].m_nIndex].m_SubWorldIndex == nSubWorldIndex)
					KPlayerChat::SendSystemInfo(1, nIndex, MESSAGE_BROADCAST_ANNOUCE_HEAD, (char *) szMsg, strlen(szMsg) , nChannelID);

				nIndex = PlayerSet.GetNextPlayer();
			}
		}
	}
	return 0;
}

//**************************************************************************************************************************************************************
//												主角脚本
//**************************************************************************************************************************************************************

//**************************************************************************************************************************************************************
//												主角脚本
//**************************************************************************************************************************************************************

/*功能：让玩家进入新的一个游戏世界
nPlayerIndex:主角的Index
nSubWorldIndex:游戏世界id
nPosX:
nPosY:
*/
//NewWorld(WorldId, X,Y)
int LuaEnterNewWorld(Lua_State * L) 
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) 
		return 0;	

	int nResult = 0;
	if (Lua_GetTopIndex(L) >= 3)
	{
		DWORD dwWorldId = (DWORD)Lua_ValueToNumber(L, 1);
		nResult = Npc[Player[nPlayerIndex].m_nIndex].ChangeWorld(dwWorldId, (int)Lua_ValueToNumber(L,2) * 32, (int)Lua_ValueToNumber(L,3) * 32);
	}
	Lua_PushNumber(L, nResult);
	return 1;
}

int LuaNpcEnterNewWorld(Lua_State * L) 
{
	int nParamCount = 0;
	if ((nParamCount = Lua_GetTopIndex(L)) < 3) return 0;
	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex <= 0) return 0;

	int nResult = 0;
	if (Lua_GetTopIndex(L) > 3)
	{
		DWORD dwWorldId = (DWORD)Lua_ValueToNumber(L, 2);
		nResult = Npc[nNpcIndex].ChangeWorld(dwWorldId, (int)Lua_ValueToNumber(L,3) * 32, (int)Lua_ValueToNumber(L,4) * 32);
	}
	Lua_PushNumber(L, nResult);
	return 1;
}

//SetPos(X,Y)
int LuaSetPos(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount != 2) return 0;
	int nPlayerIndex = GetPlayerIndex(L);
	
	int nX = (int) Lua_ValueToNumber(L,1);
	int nY = (int) Lua_ValueToNumber(L,2);
	
	if (nPlayerIndex > 0)
	{
		Npc[Player[nPlayerIndex].m_nIndex].SetPos(nX * 32, nY * 32);
	}
	return 0;
}

int LuaGetPos(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	
	if (nPlayerIndex > 0)
	{
		int nPosX = 0;
		int nPosY = 0;
		Npc[Player[nPlayerIndex].m_nIndex].GetMpsPos(&nPosX, &nPosY);
		Lua_PushNumber(L, nPosX);
		Lua_PushNumber(L, nPosY);
		Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_SubWorldIndex); 
	}
	else
		return 0;
	return 3;
}

//W,X,Y = GetWorldPos()
int LuaGetNewWorldPos(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	
	if (nPlayerIndex > 0)
	{
		int nPosX = 0;
		int nPosY = 0;
		Npc[Player[nPlayerIndex].m_nIndex].GetMpsPos(&nPosX, &nPosY);

		int nSubWorldIndex = Npc[Player[nPlayerIndex].m_nIndex].m_SubWorldIndex;
		int nSubWorldID = 0;
		if (nSubWorldIndex >= 0 && nSubWorldIndex < MAX_SUBWORLD)
		{
			nSubWorldID = SubWorld[nSubWorldIndex].m_SubWorldID;
		}

		Lua_PushNumber(L, nSubWorldID); 
		Lua_PushNumber(L, ((int)(nPosX/32)));
		Lua_PushNumber(L, ((int)(nPosY/32)));
	}
	else
	{
		Lua_PushNil(L);
		return 1;
	}
	return 3;
}


int LuaGetNewWorldName(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
	{
		int nPlayerIndex = GetPlayerIndex(L);
		
		if (nPlayerIndex > 0)
		{
			int nSubWorldID = SubWorld[Npc[Player[nPlayerIndex].m_nIndex].m_SubWorldIndex].m_SubWorldID;
			Lua_PushString(L, g_SubWorldSet.m_sMapListInfo[nSubWorldID].szName);
			return 1;
		}
	}
	else
	{
		int nSubWorldID = (int)Lua_ValueToNumber(L,1);
		Lua_PushString(L, g_SubWorldSet.m_sMapListInfo[nSubWorldID].szName);
		return 1;
	}
	return 0;
}

int LuaGetNewWorldKind(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
	{
		int nPlayerIndex = GetPlayerIndex(L);
		
		if (nPlayerIndex > 0)
		{
			int nSubWorldID = SubWorld[Npc[Player[nPlayerIndex].m_nIndex].m_SubWorldIndex].m_SubWorldID;
			Lua_PushNumber(L, g_SubWorldSet.m_sMapListInfo[nSubWorldID].nKind);
			return 1;
		}
	}
	else
	{
		int nSubWorldID = (int)Lua_ValueToNumber(L,1);
		Lua_PushNumber(L, g_SubWorldSet.m_sMapListInfo[nSubWorldID].nKind);
		return 1;
	}
	return 0;
}

int LuaGetNpcPos(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;

	int nNpcIndex = (int)Lua_ValueToNumber(L,1);
	
	if (nNpcIndex > 0)
	{
		int nPosX = 0;
		int nPosY = 0;
		Npc[nNpcIndex].GetMpsPos(&nPosX, &nPosY);

		int nSubWorldIndex = Npc[nNpcIndex].m_SubWorldIndex;
		int nSubWorldID = 0;
		if (nSubWorldIndex >= 0 && nSubWorldIndex < MAX_SUBWORLD)
		{
			nSubWorldID = SubWorld[nSubWorldIndex].m_SubWorldID;
		}

		Lua_PushNumber(L, nSubWorldID); 
		Lua_PushNumber(L, ((int)(nPosX / 32)));
		Lua_PushNumber(L, ((int)(nPosY / 32)));
		return 3;
	}
	return 0;
}

int LuaDropMoney(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) 
		return 0;	

	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
		return 0;

	int nIndex = 0;
	int nSubWorldIndex = 0;
	int nX, nY;
	int nMoney = 0;

	if (nParamNum > 1)
	{
		nIndex = Player[nPlayerIndex].m_nIndex;
		nMoney = (int)Lua_ValueToNumber(L, 1);
	}
	if (nParamNum > 2)
	{
		nIndex = (int)Lua_ValueToNumber(L, 1);
		nMoney = (int)Lua_ValueToNumber(L, 2);
	}

	if (nIndex <= 0)
	{
		nIndex = Player[nPlayerIndex].m_nIndex;
	}
	if (nMoney <= 0) 
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	Npc[nIndex].GetMpsPos(&nX, &nY);
	nSubWorldIndex = Npc[nIndex].GetSubWorldIndex();

	if (nSubWorldIndex < 0)
	{
		Lua_PushNumber(L,0);
		return 1;
	}

	POINT	ptLocal;
	KMapPos	Pos;
	ptLocal.x = nX;
	ptLocal.y = nY;
	SubWorld[nSubWorldIndex].GetFreeObjPos(ptLocal);
	
	Pos.nSubWorld = nSubWorldIndex;
	SubWorld[nSubWorldIndex].Mps2Map(ptLocal.x, ptLocal.y, 
		&Pos.nRegion, &Pos.nMapX, &Pos.nMapY, 
		&Pos.nOffX, &Pos.nOffY);
	int nObj = ObjSet.AddMoneyObj(Pos, nMoney * g_MoneyRate);
	if (nObj > 0)
	{
		Object[nObj].SetItemBelong(nPlayerIndex);
		Lua_PushNumber(L,nObj);
		return 1;
	}
	return 0;
}

int LuaDropItem(Lua_State *L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) 
		return 0;	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
		return 0;
	
	int nIndex = 0;
	int nSubWorldIndex = 0;
	int nX, nY;
	int nIdx = 0;

	if (nParamNum > 1)
	{
		nIdx = (int)Lua_ValueToNumber(L, 1);
	}
	if (nParamNum > 2)
	{
		nIndex = (int)Lua_ValueToNumber(L, 1);
		nIdx = (int)Lua_ValueToNumber(L, 2);
	}

	if (nIndex <= 0)
		nIndex = Player[nPlayerIndex].m_nIndex;

	if (nIdx <= 0) 
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	Npc[nIndex].GetMpsPos(&nX, &nY);
	nSubWorldIndex = Npc[nIndex].GetSubWorldIndex();

	if (nSubWorldIndex < 0)
	{
		Lua_PushNumber(L,0);
		return 1;
	}

	if (nIdx)
	{
		POINT	ptLocal;
		KMapPos	Pos;
		KObjItemInfo	sInfo;
		ptLocal.x = nX;
		ptLocal.y = nY;
		SubWorld[nSubWorldIndex].GetFreeObjPos(ptLocal);
		
		Pos.nSubWorld = nSubWorldIndex;
		SubWorld[nSubWorldIndex].Mps2Map(ptLocal.x, ptLocal.y, 
			&Pos.nRegion, &Pos.nMapX, &Pos.nMapY, 
			&Pos.nOffX, &Pos.nOffY);
		sInfo.m_nItemID = nIdx;
		sInfo.m_nItemWidth = Item[nIdx].GetWidth();
		sInfo.m_nItemHeight = Item[nIdx].GetHeight();
		sInfo.m_nMoneyNum = 0;
		strcpy(sInfo.m_szName, Item[nIdx].GetName());
		sInfo.m_nColorID = Item[nIdx].GetQuality();
		sInfo.m_nGenre = Item[nIdx].GetGenre();
		sInfo.m_nDetailType = Item[nIdx].GetDetailType();
		sInfo.m_nMovieFlag = 1;
		sInfo.m_nSoundFlag = 1;
		sInfo.m_bOverLook = 0;
		int nObj = ObjSet.Add(Item[nIdx].GetObjIdx(), Pos, sInfo);
		if (nObj > 0)
		{
			if (Item[nIndex].LockPick())
			{
				Object[nObj].SetEntireBelong(nPlayerIndex);
			}
			else
			{
				Object[nObj].SetItemBelong(nPlayerIndex);
			}
			Lua_PushNumber(L, nObj);
			return 1;
		}
	}
	return 0;
}

int LuaDropMapItem(Lua_State *L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 4)
		return 0;
	
	int nIndex = 0;
	int nSubWorldIndex = 0;
	int nX, nY;
	int nIdx = 0;

	if (nParamNum > 3)
	{
		nSubWorldIndex = g_SubWorldSet.SearchWorld((int)Lua_ValueToNumber(L, 1));
		nX = (int)Lua_ValueToNumber(L, 2);
		nY = (int)Lua_ValueToNumber(L, 3);
		nIdx = (int)Lua_ValueToNumber(L, 4);
	}

	if (nIdx <= 0) 
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	if (nSubWorldIndex < 0)
	{
		Lua_PushNumber(L,0);
		return 1;
	}

	if (nIdx)
	{
		POINT	ptLocal;
		KMapPos	Pos;
		KObjItemInfo	sInfo;
		ptLocal.x = nX;
		ptLocal.y = nY;
		SubWorld[nSubWorldIndex].GetFreeObjPos(ptLocal);
		
		Pos.nSubWorld = nSubWorldIndex;
		SubWorld[nSubWorldIndex].Mps2Map(ptLocal.x, ptLocal.y, 
			&Pos.nRegion, &Pos.nMapX, &Pos.nMapY, 
			&Pos.nOffX, &Pos.nOffY);
		sInfo.m_nItemID = nIdx;
		sInfo.m_nItemWidth = Item[nIdx].GetWidth();
		sInfo.m_nItemHeight = Item[nIdx].GetHeight();
		sInfo.m_nMoneyNum = 0;
		strcpy(sInfo.m_szName, Item[nIdx].GetName());
		sInfo.m_nColorID = Item[nIdx].GetQuality();
		sInfo.m_nGenre = Item[nIdx].GetGenre();
		sInfo.m_nDetailType = Item[nIdx].GetDetailType();
		sInfo.m_nMovieFlag = 1;
		sInfo.m_nSoundFlag = 1;
		sInfo.m_bOverLook = 0;
		int nObj = ObjSet.Add(Item[nIdx].GetObjIdx(), Pos, sInfo);
		if (nObj > 0)
		{
			Object[nObj].SetItemBelong(-1);
			Lua_PushNumber(L, nObj);
			return 1;
		}
	}
	return 0;
}

KTabFile g_EventItemTab;
KTabFile g_QuestItemTab;
KTabFile g_TownPortalTab;
KTabFile g_MagicScriptTab;

int LuaAddItem(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 6)
		return 0;	
	
	int nItemNature		= 0;
	int nItemClass		= 0;
	int nDetailType		= 0;
	int nParticularType	= 0;
	int nLevel			= 0;
	int nSeries			= series_num;
	int nLuck			= 0;
	int nItemLevel[MAX_ITEM_MAGICLEVEL];
	ZeroMemory(nItemLevel, sizeof(nItemLevel));

	nItemNature		= (int)Lua_ValueToNumber(L, 1);
	nItemClass		= (int)Lua_ValueToNumber(L, 2);
	nDetailType		= (int)Lua_ValueToNumber(L, 3);
	nParticularType	= (int)Lua_ValueToNumber(L, 4);
	nLevel			= (int)Lua_ValueToNumber(L, 5);
	nSeries			= (int)Lua_ValueToNumber(L, 6);

	if (nParamNum > 6)
		nLuck			= (int)Lua_ValueToNumber(L, 7);

	nItemLevel[0] = 0;
	if (nParamNum > 7)
		nItemLevel[0] = (int)Lua_ValueToNumber(L, 8);
	if (nParamNum > 12)
	{
		nItemLevel[1] = (int)Lua_ValueToNumber(L, 9);
		nItemLevel[2] = (int)Lua_ValueToNumber(L, 10);
		nItemLevel[3] = (int)Lua_ValueToNumber(L, 11);
		nItemLevel[4] = (int)Lua_ValueToNumber(L, 12);
		nItemLevel[5] = (int)Lua_ValueToNumber(L, 13);
	}
	else
	{
		for (int i = 1; i < MAX_ITEM_NORMAL_MAGICATTRIB; i ++)
			nItemLevel[i] = nItemLevel[0];
	}

	for (int i = MAX_ITEM_NORMAL_MAGICATTRIB; i < MAX_ITEM_NORMAL_MAGICATTRIB; i ++)
		nItemLevel[i] = 0;

	int nIndex = 0;
	switch(nItemNature)
	{
	case NATURE_GOLD:
	case NATURE_PLATINA:
		nIndex = ItemSet.Add(nItemNature, 0, 0, 0, MAKELONG(nLevel, nLuck), nDetailType, 0,  nItemLevel[0] == 0?NULL:nItemLevel, g_SubWorldSet.GetGameVersion());
		break;
	default:
		nIndex = ItemSet.Add(nItemNature, nItemClass, nSeries, nLevel, nLuck, nDetailType, nParticularType,  nItemLevel, g_SubWorldSet.GetGameVersion());
		break;
	}

	if (nIndex <= 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	Lua_PushNumber(L, nIndex);
	return 1;	
}

int LuaAddItemIdx(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nIdx = (int)Lua_ValueToNumber(L, 1);
	if (nIdx < 0 || nIdx >= MAX_ITEM)
		return 0;
	int nIndex = ItemSet.Add(&Item[nIdx]);
	if (nIndex <= 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	Lua_PushNumber(L, nIndex);
	return 1;	
}


int LuaAddItemID(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	
	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L,0);
		return 1;
	}
	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
	{
		Lua_PushNumber(L,0);
		return 1;
	}

	int nIndex = (int)Lua_ValueToNumber(L, 1);
	
	if (nIndex <= 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	int	P = pos_equiproom, x = 0, y = 0;
	POINT	pPos;
	if (nParamNum > 2)
	{
		P = (int)Lua_ValueToNumber(L, 2);
	}
	if (nParamNum > 3)
	{
		P = (int)Lua_ValueToNumber(L, 2);
		x = (int)Lua_ValueToNumber(L, 3);
	}
	if (nParamNum > 4)
	{
		P = (int)Lua_ValueToNumber(L, 2);
		x = (int)Lua_ValueToNumber(L, 3);
		y = (int)Lua_ValueToNumber(L, 4);
	}
	if (P < pos_hand || P > pos_num)
		return 0;

	if (nParamNum == 5 && Player[nPlayerIndex].m_ItemList.m_Room[PositionToRoom(P)].CheckRoom(x, y, Item[nIndex].GetWidth(), Item[nIndex].GetHeight()))
		Player[nPlayerIndex].m_ItemList.Add(nIndex, P, x, y, false);
	else if (nParamNum == 4 && Player[nPlayerIndex].m_ItemList.PositionToIndex(P, x) <= 0)
		Player[nPlayerIndex].m_ItemList.Add(nIndex, P, x, 0, false);
	else if (nParamNum == 3 && Player[nPlayerIndex].m_ItemList.m_Room[PositionToRoom(P)].FindRoom(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &pPos))
		Player[nPlayerIndex].m_ItemList.Add(nIndex, P, pPos.x, pPos.y, false);
	else 
		Player[nPlayerIndex].m_ItemList.InsertEquipment(nIndex, false);

	Lua_PushNumber(L, nIndex);
	return 1;
}


int LuaAddItemIDStack(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	
	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L,0);
		return 1;
	}
	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
	{
		Lua_PushNumber(L,0);
		return 1;
	}

	int nIndex = (int)Lua_ValueToNumber(L, 1);
	
	if (nIndex <= 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	int	P = pos_equiproom, x = 0, y = 0;
	POINT	pPos;
	if (nParamNum > 2)
	{
		P = (int)Lua_ValueToNumber(L, 2);
	}
	if (nParamNum > 3)
	{
		P = (int)Lua_ValueToNumber(L, 2);
		x = (int)Lua_ValueToNumber(L, 3);
	}
	if (nParamNum > 4)
	{
		P = (int)Lua_ValueToNumber(L, 2);
		x = (int)Lua_ValueToNumber(L, 3);
		y = (int)Lua_ValueToNumber(L, 4);
	}
	if (P < pos_hand || P > pos_num)
		return 0;
	if (nParamNum == 5 && Player[nPlayerIndex].m_ItemList.m_Room[PositionToRoom(P)].CheckRoom(x, y, Item[nIndex].GetWidth(), Item[nIndex].GetHeight()))
		Player[nPlayerIndex].m_ItemList.Add(nIndex, P, x, y);
	else if (nParamNum == 4 && Player[nPlayerIndex].m_ItemList.PositionToIndex(P, x) <= 0)
		Player[nPlayerIndex].m_ItemList.Add(nIndex, P, x, 0);
	else if (nParamNum == 3 && Player[nPlayerIndex].m_ItemList.m_Room[PositionToRoom(P)].FindRoom(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &pPos))
		Player[nPlayerIndex].m_ItemList.Add(nIndex, P, pPos.x, pPos.y);
	else 
		Player[nPlayerIndex].m_ItemList.InsertEquipment(nIndex, true);
	return 0;
}

int LuaGetMagicAttrib(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	int nIndex;
	if (nParamNum < 1)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	nIndex = (int)Lua_ValueToNumber(L, 1);
	if (nIndex > 0)
	{
		for (int i = 0; i < MAX_ITEM_MAGICATTRIB; i ++)
		{
			Lua_PushNumber(L, Item[nIndex].m_aryMagicAttrib[i].nAttribType);
			Lua_PushNumber(L, Item[nIndex].m_aryMagicAttrib[i].nValue[0]);
			Lua_PushNumber(L, Item[nIndex].m_aryMagicAttrib[i].nValue[2]);
		}
		return MAX_ITEM_MAGICATTRIB * 3;
	}
	else
	{
		for (int i = 0; i < MAX_ITEM_MAGICATTRIB; i ++)
		{
			Lua_PushNumber(L, 0);
			Lua_PushNumber(L, 0);
			Lua_PushNumber(L, 0);
		}
		return MAX_ITEM_MAGICATTRIB * 3;
	}
	return 0;
}


int LuaSetMagicAttrib(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	
	if (nPlayerIndex <= 0)
		return 0;
	
	int nParamNum = Lua_GetTopIndex(L);

	if (nParamNum < 24)
		return 0;

	int nIndex = (int)Lua_ValueToNumber(L, 1);

	if (nIndex > 0 && Item[nIndex].GetGenre() == item_equip)
	{
		int i = 0, k = 2;
		for (i = 0; i < MAX_ITEM_MAGICATTRIB; i ++)
		{
			Item[nIndex].m_GeneratorParam.nGeneratorLevel[i] = (int)Lua_ValueToNumber(L, k);
			Item[nIndex].m_GeneratorParam.nGeneratorLevel[i + MAX_ITEM_MAGICATTRIB] = MAKELONG((int)Lua_ValueToNumber(L, k + 1), 
				(int)Lua_ValueToNumber(L, k + 2));
			Item[nIndex].m_aryMagicAttrib[i].nAttribType = (int)Lua_ValueToNumber(L, k);
			Item[nIndex].m_aryMagicAttrib[i].nValue[0] = (int)Lua_ValueToNumber(L, k + 1);
			Item[nIndex].m_aryMagicAttrib[i].nValue[1] = -1;
			Item[nIndex].m_aryMagicAttrib[i].nValue[2] = (int)Lua_ValueToNumber(L, k + 2);
			k += 3;
		}
		for (NULL; i < MAX_ITEM_MAGICATTRIB; i ++)
		{
			Item[nIndex].m_GeneratorParam.nGeneratorLevel[i] = 0;
			Item[nIndex].m_GeneratorParam.nGeneratorLevel[i + MAX_ITEM_MAGICATTRIB] = 0;
			Item[nIndex].m_aryMagicAttrib[i].nAttribType = 0;
			Item[nIndex].m_aryMagicAttrib[i].nValue[0] = 0;
			Item[nIndex].m_aryMagicAttrib[i].nValue[1] = 0;
			Item[nIndex].m_aryMagicAttrib[i].nValue[2] = 0;
		}
		Player[nPlayerIndex].m_ItemList.SyncItemMagicAttrib(nIndex);
	}
	return 0;
}

/*
AddMagic(nPlayerIndex, nMagicID, nLevel)
DelMagic(nPlayerIndex, nMagicId)
HaveMagic(nPlayerIndex, nMagicId)
GetMagicLevel(nPlayerIndex, nMagicId)
SetMagicLevel(nPlayerIndex, nMagicId, nLevel)
ModifyMagicLevel(nPlayerIndex ,nMagicId, nDLevel)
*/
int LuaAddMagic(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	int nPlayerIndex = 0;
	if (nParamCount < 1) return 0;
	nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nSkillId = 0;
	if (Lua_IsNumber(L, 1))
	{
		nSkillId = (int)Lua_ValueToNumber (L, 1);
	}
	else 
	{
		const char * sSkillName = Lua_ValueToString(L, 1);
		g_OrdinSkillsSetting.GetInteger((char*)sSkillName, "SkillId", 0, &nSkillId);
		if (nSkillId <= 0 ) return 0;
	}
	int nSkillLevel = 0;
	if (nParamCount >=2)
		nSkillLevel = (int)Lua_ValueToNumber(L,2);
	else
		nSkillLevel = 0;

    int nRet = Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.Add(nSkillId, nSkillLevel);
	if (nRet)
	{
		Player[nPlayerIndex].UpdataCurData();
		PLAYER_SKILL_LEVEL_SYNC NewSkill;
		NewSkill.ProtocolType = s2c_playerskilllevel;
		NewSkill.m_nSkillID = nSkillId;
		NewSkill.m_nSkillLevel = Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.GetLevel(nSkillId);
		NewSkill.m_nAddLevel = Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.GetAddLevel(nSkillId);
		NewSkill.m_nSkillExp = Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.GetExp(nSkillId);
		NewSkill.m_bTempSkill = Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.IsTempSkill(nSkillId);
		NewSkill.m_nLeavePoint = Player[nPlayerIndex].m_nSkillPoint;
		g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)&NewSkill, sizeof(PLAYER_SKILL_LEVEL_SYNC));
	}
	
	Lua_PushNumber(L, nRet);
	return 1;
}

int LuaDelMagic(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	int nPlayerIndex = 0;
	if (nParamCount < 1) return 0;
	
    nPlayerIndex = GetPlayerIndex(L);
	
	if (nPlayerIndex <= 0) return 0;
	
	int nSkillId = 0;
	if (Lua_IsNumber(L, 1))
	{
		nSkillId = (int)Lua_ValueToNumber (L, 1);
	}
	else 
	{
		const char * sSkillName = Lua_ValueToString(L, 1);
		g_OrdinSkillsSetting.GetInteger((char*)sSkillName, "SkillId", 0, &nSkillId);
		if (nSkillId <= 0 ) return 0;
	}
	Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.Remove(nSkillId);

	Player[nPlayerIndex].SendSyncData_Skill();
	return 0;
}

int LuaDelAllMagic(Lua_State * L)
{
	int nPlayerIndex = 0;

    nPlayerIndex = GetPlayerIndex(L);
	
	if (nPlayerIndex <= 0) return 0;

	for (int i = 1; i < MAX_NPCSKILL; i++)
	{
		if (!Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.IsBaseSkill(
			Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.m_Skills[i].SkillId))
			Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.RemoveIdx(i);
	}

	Player[nPlayerIndex].SendSyncData_Skill();
	return 0;
}

int LuaHaveMagic(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	int nPlayerIndex = 0;
	if (nParamCount < 1) return 0;
	
	nPlayerIndex = GetPlayerIndex(L);
	
	if (nPlayerIndex <= 0) return 0;
	
	int nSkillId = 0;
	if (Lua_IsNumber(L, 1))
	{
		nSkillId = (int)Lua_ValueToNumber (L, 1);
	}
	else 
	{
		const char * sSkillName = Lua_ValueToString(L, 1);
		g_OrdinSkillsSetting.GetInteger((char *)sSkillName, "SkillId", 0, &nSkillId);
		if (nSkillId <= 0 ) 
		{
			Lua_PushNumber(L, -1);
		}
		return 1;
	}
	
	if (Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.FindSame(nSkillId))
	{
		Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.GetLevel(nSkillId));
	}
	else
	{
		Lua_PushNumber(L, -1);
	}
	
	return 1;
}

int LuaIncSkill(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	int nPlayerIndex = 0;
	if (nParamCount < 2) return 0;
	
	nPlayerIndex = GetPlayerIndex(L);
	
	if (nPlayerIndex <= 0) return 0;
	
	int nSkillId = 0, nAddLevel = 0;
	nSkillId = (int)Lua_ValueToNumber(L, 1);
	nAddLevel = (int)Lua_ValueToNumber(L, 2);
	if (nSkillId <= 0 || nAddLevel <= 0)
		return 0;
	Player[nPlayerIndex].IncSkillLevel(nSkillId, nAddLevel);
	return 0;
}

int LuaIncSkillExp(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	int nPlayerIndex = 0;
	if (nParamCount < 2) return 0;
	
	nPlayerIndex = GetPlayerIndex(L);
	
	if (nPlayerIndex <= 0) return 0;
	
	int nSkillId = 0, nAddExp = 0;
	nSkillId = (int)Lua_ValueToNumber(L, 1);
	nAddExp = (int)Lua_ValueToNumber(L, 2);
	if (nSkillId <= 0 || nAddExp <= 0)
		return 0;
	Player[nPlayerIndex].IncSkillExp(nSkillId, nAddExp);
	return 0;
}

int LuaGetSkillIdInSkillList(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) 
		return 0;
	int nSkillIndex = (int)Lua_ValueToNumber (L, 1);
	int nSkillId = 0;
	if (nSkillIndex > 0) 
	{
		nSkillId = Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.GetSkillId(nSkillIndex);
	}
	Lua_PushNumber(L, nSkillId);
	return 1;
}

int LuaSetSkillLevel(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 2) 
		return 0;
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) 
		return 0;
	int nSkillId = 0;
	
	if (Lua_IsNumber(L, 1))
	{
		nSkillId = (int)Lua_ValueToNumber (L, 1);
	}
	else 
	{
		const char * sSkillName = Lua_ValueToString(L, 1);
		g_OrdinSkillsSetting.GetInteger((char *)sSkillName, "SkillId", 0, &nSkillId);
		if (nSkillId <= 0 ) return 0;
	}
	int nSkillLevel = (int)Lua_ValueToNumber(L, 2);
	if (nSkillLevel >= 0)
		Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.SetSkillLevelDirectlyUsingId(nSkillId, nSkillLevel);
	return 0;
}

int LuaSetSkillTemp(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 2) 
		return 0;
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) 
		return 0;

	int nSkillIdx = (int)Lua_ValueToNumber (L, 1);
	BOOL bSkillTemp = (int)Lua_ValueToNumber(L, 2) > 0;

	Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.SetTempSkill(nSkillIdx, bSkillTemp);
	return 0;
}

int LuaRollBackSkills(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) 
		return 0;

	bool bRbAll = false;
	if (Lua_GetTopIndex(L) > 1)
		bRbAll = (int)Lua_ValueToNumber(L,1) > 0;

	int nTotalSkill = Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.RollBackSkills(bRbAll);
	
	Player[nPlayerIndex].SendSyncData_Skill();

	Lua_PushNumber(L, nTotalSkill);
	return 1;
}

int LuaGetMagicLevel(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	int nPlayerIndex = 0;
	
	if (nParamCount < 1) return 0;
	nPlayerIndex = GetPlayerIndex(L);
	
	if (nPlayerIndex <= 0) return 0;
	
	int nSkillId = 0;
	if (Lua_IsNumber(L, 1))
	{
		nSkillId = (int)Lua_ValueToNumber (L, 1);
	}
	else 
	{
		const char * sSkillName = Lua_ValueToString(L, 1);
		g_OrdinSkillsSetting.GetInteger((char *)sSkillName, "SkillId", 0, &nSkillId);
		if (nSkillId <= 0 ) return 0;
	}
	Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.GetLevel(nSkillId));
	return 1;
	
}
/*
int LuaSetMagicLevel(Lua_State * L)
{
int nParamCount = Lua_GetTopIndex(L);
int nPlayerIndex = 0;
int nTemp = 0;
if (nParamCount < 1) return 0;

  nPlayerIndex = GetPlayerIndex(L);
  
	if (nPlayerIndex <= 0) return 0;
	
	  int nSkillId = 0;
	  if (Lua_IsNumber(L, nTemp))
	  {
	  nSkillId = (int)Lua_ValueToNumber (L, 1);
	  }
	  else 
	  {
	  const char * sSkillName = Lua_ValueToString(L, 1);
	  nSkillId = g_OrdinSkillsSetting.FindRow((char *)sSkillName) - 2;
	  if (nSkillId <= 0 ) return 0;
	  }
	  int nNpcIndex = Player[nPlayerIndex].m_nIndex;
	  if (nNpcIndex > 0)
	  Lua_PushNumber(L,Npc[nNpcIndex].m_SkillList.SetSkillLevel(nSkillId, (int)Lua_ValueToNumber(L, 2)));
	  return 0;
	  }
*/
//**************************************************************************************************************************************************************
//												NPC操作脚本
//**************************************************************************************************************************************************************
/*nNpcTemplateId GetNpcTmpId(sName)
功能从Npc模板中获得名称为sName的Npc在模板中的Id
sName:Npc名称
nNpcTemplateID:模板中Id
*/

int LuaGetNpcTemplateID(Lua_State * L)
{
	if (Lua_GetTopIndex(L) <= 0 ) return 0 ;
	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex > 0)
	{
		Lua_PushNumber(L,Npc[nNpcIndex].m_NpcSettingIdx);
		return 1;
	}
	return 0;
}

int LuaGetNpcTempName(Lua_State * L)
{
	if (Lua_GetTopIndex(L) <= 0 ) return 0 ;

	int nNpcSettingIdx = (int)Lua_ValueToNumber(L, 1);
	char	Name[32];
	g_NpcSetting.GetString(nNpcSettingIdx + 2, "Name", "", Name, sizeof(Name));
	Lua_PushString(L,Name);
	return 1;
}

int LuaGetNpcTempTypeName(Lua_State * L)
{
	if (Lua_GetTopIndex(L) <= 0 ) return 0 ;

	int nNpcSettingIdx = (int)Lua_ValueToNumber(L, 1);
	char	szNpcTypeName[32];
	g_NpcSetting.GetString(nNpcSettingIdx + 2, "NpcResType", "", szNpcTypeName, sizeof(szNpcTypeName));
	Lua_PushString(L,szNpcTypeName);
	return 1;
}
/*
nNpcIndex AddNpc(nNpcTemplateId,nLevel, nSubWorldIndex, nPosX, nPosY )

  功能：增加一个特定的NPC
  参数：
  nNpcTemplateId: NPC在NPC模板中的id
  nLevel:Npc的等级
  nSubWorldIndex:所处的世界id
  nPosX：X (点坐标)
  nPosY：Y (点坐标)
  nNpcIndex:增加后，将返回该Npc在游戏世界的Index，如果不成功返回nil 
*/

int LuaAddNpc(Lua_State * L)
{
	char * pName = NULL;
	int	   nId = 0;
	if (Lua_GetTopIndex(L) < 6) return 0;
	
	if (Lua_IsNumber(L,1))
	{
		nId = (int)Lua_ValueToNumber(L,1);
	}
	else if	(Lua_IsString(L,1))
	{
		pName = (char *)lua_tostring(L,1);	
		nId = g_NpcSetting.FindRow((char*)pName) - 2;
	}
	else return 0;
	
	if (nId < 0 || nId > (g_NpcSetting.GetHeight()-2)) return 0;
	
	int nLevel = (int)lua_tonumber(L,2);
	//if (nLevel >= 128) nLevel = 127;
	if (nLevel < 0 ) nLevel = 1;
	
	int	nNpcIdxInfo = MAKELONG(nLevel, nId);//(nId << 7) + nLevel;
	//question
	if ((int)lua_tonumber(L, 3) != -1)
	{
		int nNpcIdx = NpcSet.Add(nNpcIdxInfo, (int)lua_tonumber(L, 3), (int)lua_tonumber(L,4), (int)lua_tonumber(L,5), (BOOL)lua_tonumber(L,6));
		Lua_PushNumber(L, nNpcIdx);
		return 1;
	}
	Lua_PushNumber(L, 0);
    return 1;
}

/*nResult DelNpc (nNpcIndex)
功能：删除一个特定的NPC
nResult:返回成功与否,1为成功,0为失败
*/
int LuaDelNpc(Lua_State * L)
{
	if (Lua_GetTopIndex(L) <= 0 ) return 0 ;
	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex > 0)
	{
		if (Npc[nNpcIndex].m_RegionIndex >= 0)
		{
			SubWorld[Npc[nNpcIndex].m_SubWorldIndex].m_Region[Npc[nNpcIndex].m_RegionIndex].RemoveNpc(nNpcIndex);
			SubWorld[Npc[nNpcIndex].m_SubWorldIndex].m_Region[Npc[nNpcIndex].m_RegionIndex].DecRef(Npc[nNpcIndex].m_MapX, Npc[nNpcIndex].m_MapY, obj_npc);
		}
		NpcSet.Remove(nNpcIndex);
	}
	return 0;
}

int LuaClearMapNpc(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
	{
		Lua_PushNumber(L , 0);
		return 1;
	}
	int nSubWorldIndex = g_SubWorldSet.SearchWorld((int)Lua_ValueToNumber(L, 1));

	int nResult = 0;
	if (nSubWorldIndex >= 0)
	{
		for (int nNpcIndex=1;nNpcIndex<MAX_NPC;++nNpcIndex)
		{
			if (Npc[nNpcIndex].m_SubWorldIndex == nSubWorldIndex)
			{
				if (Npc[nNpcIndex].m_RegionIndex >= 0)
				{
					SubWorld[Npc[nNpcIndex].m_SubWorldIndex].m_Region[Npc[nNpcIndex].m_RegionIndex].RemoveNpc(nNpcIndex);
					SubWorld[Npc[nNpcIndex].m_SubWorldIndex].m_Region[Npc[nNpcIndex].m_RegionIndex].DecRef(Npc[nNpcIndex].m_MapX, Npc[nNpcIndex].m_MapY, obj_npc);
				}
				NpcSet.Remove(nNpcIndex);
				nResult++;
			}
		}
	}
	Lua_PushNumber(L , nResult);
	return 1;
}

int LuaClearMapNpcWithName(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
	{
		Lua_PushNumber(L , 0);
		return 1;
	}
	int nSubWorldIndex = g_SubWorldSet.SearchWorld((int)Lua_ValueToNumber(L, 1));

	int nResult = 0;
	if (nSubWorldIndex >= 0)
	{
		for (int nNpcIndex=1;nNpcIndex<MAX_NPC;++nNpcIndex)
		{
			if (Npc[nNpcIndex].m_SubWorldIndex == nSubWorldIndex)
			{
				if (strcmp(Npc[nNpcIndex].Name, (char*)Lua_ValueToString(L, 2)) == 0)
				{
					if (Npc[nNpcIndex].m_RegionIndex >= 0)
					{
						SubWorld[Npc[nNpcIndex].m_SubWorldIndex].m_Region[Npc[nNpcIndex].m_RegionIndex].RemoveNpc(nNpcIndex);
						SubWorld[Npc[nNpcIndex].m_SubWorldIndex].m_Region[Npc[nNpcIndex].m_RegionIndex].DecRef(Npc[nNpcIndex].m_MapX, Npc[nNpcIndex].m_MapY, obj_npc);
					}
					NpcSet.Remove(nNpcIndex);
					nResult++;
				}
			}
		}
	}
	Lua_PushNumber(L , nResult);
	return 1;
}

/*
nDelCount DelNpcsInRgn(nSubWorld,nRegionId, nKind) 
功能：删除某个游戏世界中某个Region内的所有某类的NPC
返回:删除的Npc个数
*/

int LuaDelNpcsInRgn(Lua_State * L)
{
	//Question
	return 0;
}
/*
nDelCount DelNpcsInWld(nSubWorldId, nKind)
功能：删除某个游戏世界中的所有Npc
*/
int LuaDelNpcsInWld(Lua_State * L)
{
	return 0;
}

int LuaSyncNpc(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1 ) return 0;
	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC) return 0;
	Npc[nNpcIndex].SendSyncData(0, TRUE);
	return 0;
}

int LuaSetNpcPos(Lua_State * L)
{
	int nParamCount = 0;
	if ((nParamCount = Lua_GetTopIndex(L)) < 3) return 0;
	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex <= 0) return 0;

	int nX = (int)Lua_ValueToNumber(L,2);
	int nY = (int)Lua_ValueToNumber(L,3);

	Npc[nNpcIndex].SetPos(nX * 32, nY * 32);
	return 0;
}


int LuaSetNpcActionScript(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 2 ) return 0;
	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC) return 0;
	strcpy(Npc[nNpcIndex].ActionScript, Lua_ValueToString(L,2));
	Npc[nNpcIndex].m_ActionScriptID = g_FileName2Id((char *)Lua_ValueToString(L,2));

	if (Npc[nNpcIndex].m_Kind == kind_normal)
		NpcSet.ExecuteScript(nNpcIndex, Npc[nNpcIndex].m_ActionScriptID, "Revive", nNpcIndex);
	return 0;
}

int LuaSetNpcKind(Lua_State * L)
{	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	int nKind = (int)Lua_ValueToNumber(L, 2);
	if (nKind < kind_normal || nKind >= kind_num)
		return 0;

	Npc[nNpcIndex].m_Kind = nKind;
	return 0;
}	

int LuaSetNpcSeries(Lua_State * L)
{	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0 ;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	Npc[nNpcIndex].SetSeries((int)Lua_ValueToNumber(L, 2));
	return 0;
}	


int LuaGetNpcSeries(Lua_State * L)
{	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
		return 0 ;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;
	
	Lua_PushNumber(L,Npc[nNpcIndex].m_Series);
	return 1;
}

int LuaSetNpcExp(Lua_State * L)
{	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0 ;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	int nExp = (int)Lua_ValueToNumber(L, 2);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	Npc[nNpcIndex].m_CurrentExperience = nExp;
	if (nParamNum>2 && (int)Lua_ValueToNumber(L, 3)>0)
		Npc[nNpcIndex].m_Experience = nExp;
	return 0;
}	

int LuaSetNpcLife(Lua_State * L)
{	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0 ;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	int nLife = (int)Lua_ValueToNumber(L, 2);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	Npc[nNpcIndex].m_CurrentLife = nLife;
	Npc[nNpcIndex].m_CurrentLifeMax = nLife;

	if (nParamNum>2 && (int)Lua_ValueToNumber(L, 3)>0)
		Npc[nNpcIndex].m_LifeMax = nLife;
	return 0;
}	


int LuaGetNpcLife(Lua_State * L)
{	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
		return 0 ;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	Lua_PushNumber(L, Npc[nNpcIndex].m_CurrentLifeMax);
	return 1;
}


int LuaSetNpcLifeReplenish(Lua_State * L)
{	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0 ;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	int nLifeRep = (int)Lua_ValueToNumber(L, 2);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	Npc[nNpcIndex].m_CurrentLifeReplenish = nLifeRep;
	if (nParamNum>2 && (int)Lua_ValueToNumber(L, 3)>0)
		Npc[nNpcIndex].m_LifeReplenish = nLifeRep;
	return 0;
}	

int LuaSetNpcAR(Lua_State * L)
{	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0 ;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	int nAttackR = (int)Lua_ValueToNumber(L, 2);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	Npc[nNpcIndex].m_CurrentAttackRating = nAttackR;
	if (nParamNum>2 && (int)Lua_ValueToNumber(L, 3)>0)
		Npc[nNpcIndex].m_AttackRating = nAttackR;
	return 0;
}	

int LuaSetNpcDefense(Lua_State * L)
{	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0 ;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	int nDefense = (int)Lua_ValueToNumber(L, 2);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	Npc[nNpcIndex].m_CurrentDefend = nDefense;
	if (nParamNum>2 && (int)Lua_ValueToNumber(L, 3)>0)
		Npc[nNpcIndex].m_Defend = nDefense;
	return 0;
}	

int LuaSetNpcDamage(Lua_State * L)
{	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 3)
		return 0 ;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	Npc[nNpcIndex].SetPhysicsDamage((int)Lua_ValueToNumber(L, 2), (int)Lua_ValueToNumber(L, 3));
	return 0;
}	


int LuaSetNpcDmgEx(Lua_State * L)
{	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 6)
		return 0 ;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	if ((nParamNum > 6) && (int)Lua_ValueToNumber(L, 7))
	{
		Npc[nNpcIndex].m_CurrentAddPhysicsDamage = (int)Lua_ValueToNumber(L, 2);
		Npc[nNpcIndex].m_CurrentPoisonDamage.nValue[0] = (int)Lua_ValueToNumber(L, 3);
		if (Npc[nNpcIndex].m_CurrentPoisonDamage.nValue[0] > 0)
		{
			Npc[nNpcIndex].m_CurrentPoisonDamage.nValue[1] = POISON_DAMAGE_TIME;//pMagic->nValue[1];
			Npc[nNpcIndex].m_CurrentPoisonDamage.nValue[2] = POISON_DAMAGE_INTERVAL;//pMagic->nValue[2];
		}
		else
		{
			Npc[nNpcIndex].m_CurrentPoisonDamage.nValue[0] = 0;
			Npc[nNpcIndex].m_CurrentPoisonDamage.nValue[1] = 0;
			Npc[nNpcIndex].m_CurrentPoisonDamage.nValue[2] = 0;
		}
		Npc[nNpcIndex].m_CurrentColdDamage.nValue[0] = (int)Lua_ValueToNumber(L, 4);
		Npc[nNpcIndex].m_CurrentColdDamage.nValue[2] = (int)Lua_ValueToNumber(L, 4);
		if (Npc[nNpcIndex].m_CurrentColdDamage.nValue[0] > 0 && Npc[nNpcIndex].m_CurrentColdMagic.nValue[2] > 0)
			Npc[nNpcIndex].m_CurrentColdDamage.nValue[1] = COLD_DAMAGE_TIME;
		else
		{
			Npc[nNpcIndex].m_CurrentColdDamage.nValue[0] = 0;
			Npc[nNpcIndex].m_CurrentColdDamage.nValue[2] = 0;
			Npc[nNpcIndex].m_CurrentColdDamage.nValue[1] = 0;
		}
		Npc[nNpcIndex].m_CurrentFireDamage.nValue[0] = (int)Lua_ValueToNumber(L, 5);
		Npc[nNpcIndex].m_CurrentFireDamage.nValue[2] = (int)Lua_ValueToNumber(L, 5);
		Npc[nNpcIndex].m_CurrentLightDamage.nValue[0] = (int)Lua_ValueToNumber(L, 6);
		Npc[nNpcIndex].m_CurrentLightDamage.nValue[2] = (int)Lua_ValueToNumber(L, 6);
	}
	else
	{
		Npc[nNpcIndex].m_CurrentAddPhysicsMagic = (int)Lua_ValueToNumber(L, 2);
		Npc[nNpcIndex].m_CurrentPoisonMagic.nValue[0] = (int)Lua_ValueToNumber(L, 3);
		if (Npc[nNpcIndex].m_CurrentPoisonDamage.nValue[0] > 0)
		{
			Npc[nNpcIndex].m_CurrentPoisonMagic.nValue[1] = POISON_DAMAGE_TIME;//pMagic->nValue[1];
			Npc[nNpcIndex].m_CurrentPoisonMagic.nValue[2] = POISON_DAMAGE_INTERVAL;//pMagic->nValue[2];
		}
		else
		{
			Npc[nNpcIndex].m_CurrentPoisonMagic.nValue[0] = 0;
			Npc[nNpcIndex].m_CurrentPoisonMagic.nValue[1] = 0;
			Npc[nNpcIndex].m_CurrentPoisonMagic.nValue[2] = 0;
		}
		Npc[nNpcIndex].m_CurrentColdMagic.nValue[0] = (int)Lua_ValueToNumber(L, 4);
		Npc[nNpcIndex].m_CurrentColdMagic.nValue[2] = (int)Lua_ValueToNumber(L, 4);
		if (Npc[nNpcIndex].m_CurrentColdMagic.nValue[0] > 0 && Npc[nNpcIndex].m_CurrentColdMagic.nValue[2] > 0)
			Npc[nNpcIndex].m_CurrentColdMagic.nValue[1] = COLD_DAMAGE_TIME;
		else
		{
			Npc[nNpcIndex].m_CurrentColdMagic.nValue[0] = 0;
			Npc[nNpcIndex].m_CurrentColdMagic.nValue[2] = 0;
			Npc[nNpcIndex].m_CurrentColdMagic.nValue[1] = 0;
		}
		Npc[nNpcIndex].m_CurrentFireMagic.nValue[0] = (int)Lua_ValueToNumber(L, 5);
		Npc[nNpcIndex].m_CurrentFireMagic.nValue[2] = (int)Lua_ValueToNumber(L, 5);
		Npc[nNpcIndex].m_CurrentLightMagic.nValue[0] = (int)Lua_ValueToNumber(L, 6);
		Npc[nNpcIndex].m_CurrentLightMagic.nValue[2] = (int)Lua_ValueToNumber(L, 6);
	}
	return 0;
}	

int LuaSetNpcResist(Lua_State * L)
{	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 6)
		return 0 ;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	Npc[nNpcIndex].m_CurrentPhysicsResistMax = (int)Lua_ValueToNumber(L, 2);
	Npc[nNpcIndex].m_CurrentPoisonResistMax = (int)Lua_ValueToNumber(L, 3);
	Npc[nNpcIndex].m_CurrentLightResistMax = (int)Lua_ValueToNumber(L, 4);
	Npc[nNpcIndex].m_CurrentFireResistMax = (int)Lua_ValueToNumber(L, 5);
	Npc[nNpcIndex].m_CurrentColdResistMax = (int)Lua_ValueToNumber(L, 6);		
		
	Npc[nNpcIndex].m_CurrentPhysicsResist = (int)Lua_ValueToNumber(L, 2);
	Npc[nNpcIndex].m_CurrentPoisonResist = (int)Lua_ValueToNumber(L, 3);
	Npc[nNpcIndex].m_CurrentLightResist = (int)Lua_ValueToNumber(L, 4);
	Npc[nNpcIndex].m_CurrentFireResist = (int)Lua_ValueToNumber(L, 5);
	Npc[nNpcIndex].m_CurrentColdResist = (int)Lua_ValueToNumber(L, 6);

	if ((nParamNum > 6) && (int)Lua_ValueToNumber(L, 7))
	{
		Npc[nNpcIndex].m_PhysicsResist = (int)Lua_ValueToNumber(L, 2);
		Npc[nNpcIndex].m_PoisonResist = (int)Lua_ValueToNumber(L, 3);
		Npc[nNpcIndex].m_LightResist = (int)Lua_ValueToNumber(L, 4);
		Npc[nNpcIndex].m_FireResist = (int)Lua_ValueToNumber(L, 5);
		Npc[nNpcIndex].m_ColdResist = (int)Lua_ValueToNumber(L, 6);

		Npc[nNpcIndex].m_PhysicsResistMax = (int)Lua_ValueToNumber(L, 2);
		Npc[nNpcIndex].m_PoisonResistMax = (int)Lua_ValueToNumber(L, 3);
		Npc[nNpcIndex].m_LightResistMax = (int)Lua_ValueToNumber(L, 4);
		Npc[nNpcIndex].m_FireResistMax = (int)Lua_ValueToNumber(L, 5);
		Npc[nNpcIndex].m_ColdResistMax = (int)Lua_ValueToNumber(L, 6);	
	}
	return 0;
}	

int LuaSetNpcRevTime(Lua_State * L)
{	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0 ;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	Npc[nNpcIndex].SetReviveFrame((int)Lua_ValueToNumber(L, 2));
	return 0;
}

int LuaSetNpcSpeed(Lua_State * L)
{	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0 ;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	
	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	Npc[nNpcIndex].m_CurrentWalkSpeed = (int)Lua_ValueToNumber(L, 2);
	if (nParamNum>2 && (int)Lua_ValueToNumber(L, 3)>0)
		Npc[nNpcIndex].m_WalkSpeed = (int)Lua_ValueToNumber(L, 2);
	return 0;
}

int LuaSetNpcHitRecover(Lua_State *L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0;
	
	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	Npc[nNpcIndex].m_CurrentHitRecover = (int)Lua_ValueToNumber(L, 2);

	if (nParamNum>2 && (int)Lua_ValueToNumber(L, 3)>0)
		Npc[nNpcIndex].m_HitRecover = (int)Lua_ValueToNumber(L, 2);
	return 0;
}

int LuaSetNpcBoss(Lua_State * L)
{	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0 ;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	Npc[nNpcIndex].m_btSpecial = (int)Lua_ValueToNumber(L, 2);
	return 0;
}

int LuaGetNpcBoss(Lua_State * L)
{	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
		return 0 ;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	Lua_PushNumber(L, Npc[nNpcIndex].m_btSpecial);
	return 1;
}

int LuaIsBlueBoss(Lua_State * L)
{	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
		return 0 ;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	if (Npc[nNpcIndex].m_btSpecial == npc_blue)
		Lua_PushNumber(L, 1);
	else
		Lua_PushNumber(L, 0);
	return 1;
}

int LuaGetNpcExpRate(Lua_State * L)
{	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) 
	{
		Lua_PushNumber(L,0);
		return 0;
	}
	Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_CurrentExpEnhance);
	return 1;
}

int LuaIsRideHorse(Lua_State * L)
{	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) 
	{
		Lua_PushNumber(L,0);
		return 0;
	}
	Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_bRideHorse);
	return 1;
}

int LuaSetNpcRemoveDeath(Lua_State * L)
{	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0 ;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	Npc[nNpcIndex].m_bNpcRemoveDeath = (BOOL)Lua_ValueToNumber(L, 2);
	return 0;
}	

int LuaSetNpcTimeout(Lua_State * L)
{	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0 ;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	Npc[nNpcIndex].m_nNpcTimeout = (int)Lua_ValueToNumber(L, 2) + g_SubWorldSet.GetGameTime();
	return 0;
}	

int LuaGetNpcTimeout(Lua_State * L)
{	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
		return 0 ;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	Lua_PushNumber(L, g_SubWorldSet.GetGameTime() - Npc[nNpcIndex].m_nNpcTimeout < 0 ? 0 : g_SubWorldSet.GetGameTime() - Npc[nNpcIndex].m_nNpcTimeout);
	return 1;
}	

int LuaSetNpcParam(Lua_State * L)
{	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0 ;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	if (nParamNum > 2)
		Npc[nNpcIndex].m_nNpcParam[(int)Lua_ValueToNumber(L,2)] = (int)Lua_ValueToNumber(L,3);
	else
		Npc[nNpcIndex].m_nNpcParam[0] = (int)Lua_ValueToNumber(L,2);
	return 0;
}	

int LuaGetNpcParam(Lua_State * L)
{	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
		return 0 ;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	if (nParamNum > 1)
		Lua_PushNumber(L, Npc[nNpcIndex].m_nNpcParam[(int)Lua_ValueToNumber(L,2)]);
	else
		Lua_PushNumber(L, Npc[nNpcIndex].m_nNpcParam[0]);
	return 1;
}	

int LuaSetNpcOwner(Lua_State * L)
{	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 3)
		return 0 ;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;
	strcpy(Npc[nNpcIndex].Owner, (char*)Lua_ValueToString(L,2));
//	g_DebugLog("%s %d",Npc[nNpcIndex].Owner,nNpcIndex);
	if (nParamNum >= 3)
		Npc[nNpcIndex].m_bNpcFollowFindPath = (BOOL)Lua_ValueToNumber(L,3);

	Npc[nNpcIndex].m_uFindPathTime = g_SubWorldSet.GetGameTime();
	return 0;
}

int LuaGetNpcOwner(Lua_State * L)
{	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
		
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
		return 0 ;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;
		
	lua_pushstring(L, Npc[nNpcIndex].Owner);
	
	return 1;
}

int LuaSetNpcFindPathTime(Lua_State * L)
{	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 3)
		return 0 ;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	DWORD dwTime = (DWORD)Lua_ValueToNumber(L, 2);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	if (dwTime != -1)
		Npc[nNpcIndex].m_uFindPathMaxTime = dwTime*20;
	else
		Npc[nNpcIndex].m_uFindPathMaxTime = -1;
//	g_DebugLog("%s %d",Npc[nNpcIndex].m_uFindPathMaxTime,nNpcIndex);	
	return 0;
}

int LuaSetNpcName(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
		return 0 ;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	if (Lua_IsNumber(L,2))
	{
		KTabFile Replace;
		Replace.Load(NPC_NAME_FILE);
		Replace.GetString((int)Lua_ValueToNumber(L,2)+2, "targetname","", Npc[nNpcIndex].Name, sizeof(Npc[nNpcIndex].Name));
	}
	else if (Lua_IsString(L,2))
		strcpy(Npc[nNpcIndex].Name, (char*)Lua_ValueToString(L,2));

	return 0;
}

int LuaGetNpcName(Lua_State * L)
{	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
		return 0 ;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	Lua_PushString(L, Npc[nNpcIndex].Name);
	return 1;
}

int LuaGetNpcID(Lua_State * L)
{	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
		return 0 ;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	Lua_PushNumber(L, Npc[nNpcIndex].m_dwID);
	return 1;
}

int LuaSetNpcSkill(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 4) 
		return 0;
	
	int nNpcIdx = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIdx <= 0 || nNpcIdx >= MAX_NPC)
		return 0;
	int nSkillID = (int)Lua_ValueToNumber(L, 2);
	if (nSkillID <= 0)
		return 0;
	int nSkillLevel = (int)Lua_ValueToNumber(L, 3);
	if (nSkillLevel < 1)
		nSkillLevel = 1;
	int nSkillPos = (int)Lua_ValueToNumber(L, 4);
	if (nSkillPos < 0 || nSkillPos > MAX_NPCSKILL)
		return 0;
	Npc[nNpcIdx].m_SkillList.SetNpcSkill(nSkillPos, nSkillID, nSkillLevel);
	return 1;
}

int LuaSetNpcDropScript(Lua_State *L)
{
	if (Lua_GetTopIndex(L) < 2 ) return 0;
	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC) return 0;
	Npc[nNpcIndex].m_DropScriptID = g_FileName2Id((char *)Lua_ValueToString(L,2));
	return 0;
}

int LuaSetPlayerRevivalPos(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) 
		return 0;
	int nParamCount = Lua_GetTopIndex(L);
	if(nParamCount < 1)
		return 0;
	int nSubWorldId = 0;
	int nRevId = 0;
	
	if (nParamCount > 2)
	{
		nSubWorldId = (int) Lua_ValueToNumber(L, 1);
		nRevId = (int) Lua_ValueToNumber(L, 2);
	}
	else
	{
		nRevId = (int) Lua_ValueToNumber(L, 1);
	}
	
	Player[nPlayerIndex].SetRevivalPos(nSubWorldId, nRevId);
	return 0;
}

int LuaGetPlayerRevivalPos(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) 
		return 0;
	int nParamCount = Lua_GetTopIndex(L);
	if(nParamCount < 1)
		return 0;
	int nSubWorldId = 0;
	int nRevId = 0;
	
	if (nParamCount > 2)
	{
		nSubWorldId = (int) Lua_ValueToNumber(L, 1);
		nRevId = (int) Lua_ValueToNumber(L, 2);
	}
	else
	{
		nRevId = (int) Lua_ValueToNumber(L, 1);
	}

	POINT Pos;
	g_SubWorldSet.GetRevivalPosFromId(nSubWorldId ? nSubWorldId : SubWorld[Npc[Player[nPlayerIndex].m_nIndex].m_SubWorldIndex].m_SubWorldID, nRevId, &Pos);
	Lua_PushNumber(L, nSubWorldId);
	Lua_PushNumber(L, Pos.x);
	Lua_PushNumber(L, Pos.y);
	return 3;
}

int LuaGetPlayerRevival(Lua_State * L)
{	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) 
	{
		return 0;
	}

	POINT Pos;
	Player[nPlayerIndex].GetLoginRevival(&Pos);
	Lua_PushNumber(L, Pos.x);
	Lua_PushNumber(L, Pos.y);
	return 2;
}

int LuaGetPlayerRevivalID(Lua_State * L)
{	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) 
	{
		return 0;
	}

	Lua_PushNumber(L, Player[nPlayerIndex].GetLoginRevivalID());
	return 1;
}
//**************************************************************************************************************************************************************
//												聊天消息脚本
//**************************************************************************************************************************************************************

//**********************************************************************************************
//							主角属性获得
//**********************************************************************************************


#define MacroFun_GetPlayerInfoInt(L, MemberName) { int nPlayerIndex = GetPlayerIndex(L);\
	if (nPlayerIndex > 0){	int nNpcIndex = Player[nPlayerIndex].m_nIndex;	if (nNpcIndex > 0)Lua_PushNumber(L, Npc[nNpcIndex].MemberName);\
	else Lua_PushNil(L);}\
	else Lua_PushNil(L);\
return 1;}														

//阵营
int LuaGetPlayerCurrentCamp(Lua_State * L)
{
	MacroFun_GetPlayerInfoInt(L, m_CurrentCamp);
}

int LuaGetPlayerCamp(Lua_State * L)
{
	MacroFun_GetPlayerInfoInt(L, m_Camp);
}

int LuaSetPlayerCamp(Lua_State * L)
{
	int nValue = (int)Lua_ValueToNumber(L,1);
	if (nValue < 0 ) return 0;
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		Npc[Player[nPlayerIndex].m_nIndex].SetCamp(nValue);
	}
	return 0;
}

int LuaSetPlayerCurrentCamp(Lua_State * L)
{
	int nValue = (int)Lua_ValueToNumber(L,1);
	if (nValue < 0 ) return 0;
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		Npc[Player[nPlayerIndex].m_nIndex].SetCurrentCamp(nValue);
	}
	return 0;
}

int LuaSetNpcCurCamp(Lua_State * L)
{
	int nNpcIndex = (int)Lua_ValueToNumber(L,1);
	if (nNpcIndex <= 0 && nNpcIndex > MAX_NPC) return 0;
	int nValue = (int )Lua_ValueToNumber(L,2);
	if (nValue >= camp_num) return 0;
		Npc[nNpcIndex].SetCurrentCamp(nValue);
	return 0;
}

int LuaRestorePlayerCamp(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		Npc[Player[nPlayerIndex].m_nIndex].RestoreCurrentCamp();
	}
	return 0;
}

int LuaOpenTong(Lua_State * L)
{	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) return 0;
	
	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bUIId = UI_OPENTONGUI;
	UiInfo.m_bOptionNum = 0;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	
	int nMsgId = 0;

	UiInfo.m_bParam1 = 0;
	UiInfo.m_nBufferLen = sizeof(int);
	
#ifndef _SERVER
	UiInfo.m_bParam2 = 0;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
#else
	UiInfo.m_bParam2 = 1;
	UiInfo.ProtocolType = (BYTE)s2c_scriptaction;
	UiInfo.m_wProtocolLong = sizeof(PLAYER_SCRIPTACTION_SYNC) - MAX_SCIRPTACTION_BUFFERNUM + UiInfo.m_nBufferLen - 1;
	g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)&UiInfo, UiInfo.m_wProtocolLong + 1 );
#endif
	return 0;
}

int LuaJoinTong(Lua_State * L)
{	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) return 0;
	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0;

	if (Npc[Player[nPlayerIndex].m_nIndex].m_Camp == camp_begin)
		return 0;

	Player[nPlayerIndex].m_cTong.JoinTong((char*)Lua_ValueToString(L, 1));
	return 0;
}


int LuaCreateTong(Lua_State * L)
{	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) return 0;
	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 3)
		return 0;

	if (Npc[Player[nPlayerIndex].m_nIndex].m_Camp == camp_begin)
		return 0;

	if ((int)Lua_ValueToNumber(L, 2) < camp_justice || (int)Lua_ValueToNumber(L, 2) > camp_balance)
		return 0;

	char* lpszTongName = (char*)Lua_ValueToString(L, 1);

	TONG_CREATE_SYNC	sCreate;
	sCreate.ProtocolType = s2c_createtong;
	sCreate.m_btCamp = (int)Lua_ValueToNumber(L, 2);
	if (strlen(lpszTongName) < sizeof(sCreate.m_szName))
		strcpy(sCreate.m_szName, lpszTongName);
	else
	{
		memcpy(sCreate.m_szName, lpszTongName, sizeof(sCreate.m_szName) - 1);
		sCreate.m_szName[sizeof(sCreate.m_szName) - 1] = 0;
	}
	if (g_pServer)
		g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &sCreate, sizeof(TONG_CREATE_SYNC));

	return 0;
}

int LuaGetTongFlag(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		Lua_PushNumber(L, Player[nPlayerIndex].m_cTong.m_nFlag);
		return 1;
	}
	return 0;
}

int LuaGetTongName(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		Lua_PushString(L, Player[nPlayerIndex].m_cTong.m_szName);
		Lua_PushNumber(L, Player[nPlayerIndex].m_cTong.m_dwTongNameID);
		return 2;
	}
	return 0;
}

int LuaGetTongCamp(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		Lua_PushNumber(L, Player[nPlayerIndex].m_cTong.m_nCamp);
		return 1;
	}
	return 0;
}

int LuaGetTongMemNum(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		Lua_PushNumber(L, Player[nPlayerIndex].m_cTong.m_dwMemberNum);
		Lua_PushNumber(L, Player[nPlayerIndex].m_cTong.m_btManagerNum);
		Lua_PushNumber(L, Player[nPlayerIndex].m_cTong.m_btDirectorNum);
		return 3;
	}
	return 0;
}

int LuaGetTongFigure(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		Lua_PushNumber(L, Player[nPlayerIndex].m_cTong.m_nFigure);
		return 1;
	}
	return 0;
}

int LuaGetTongMoney(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		Lua_PushNumber(L, Player[nPlayerIndex].m_cTong.m_dwMoney);
		return 1;
	}
	return 0;
}

int LuaGetTongLevel(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		Lua_PushNumber(L, Player[nPlayerIndex].m_cTong.m_btLevel);
		return 1;
	}
	return 0;
}

int LuaGetTongEff(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		Lua_PushNumber(L, Player[nPlayerIndex].m_cTong.m_dwTotalEff);
		return 1;
	}
	return 0;
}

int LuaGetTongParam(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		Lua_PushNumber(L, Player[nPlayerIndex].m_cTong.m_nTongParam);
		return 1;
	}
	return 0;
}

int LuaGetTongJoinTm(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
	if (Lua_GetTopIndex(L) > 1)
		nPlayerIndex = (int)Lua_ValueToNumber(L, 1);
	if (nPlayerIndex > 0)
	{
		Lua_PushNumber(L, (int)Player[nPlayerIndex].m_cTong.m_nJoinTm);
		return 1;
	}
	return 0;
}

int LuaCommendMaster(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		STONG_ACCEPT_MASTER_COMMAND	sAccept;

		sAccept.ProtocolFamily	= pf_tong;
		sAccept.ProtocolID		= enumC2S_TONG_ACCEPT_MASTER;
		sAccept.m_dwParam		= nPlayerIndex;
		sAccept.m_dwTongNameID	= g_FileName2Id(Player[nPlayerIndex].m_cTong.m_szName);
		sAccept.m_btFigure		= Player[nPlayerIndex].m_cTong.m_nFigure;
		sAccept.m_btPos			= 1;
		sAccept.m_btAcceptFalg = Player[nPlayerIndex].m_cTong.CheckGetMasterPower();
		memcpy(sAccept.m_szName, Npc[Player[nPlayerIndex].m_nIndex].Name, sizeof(Npc[Player[nPlayerIndex].m_nIndex].Name));
		
		if (g_pTongClient)
			g_pTongClient->SendPackToServer((const void*)&sAccept, sizeof(STONG_ACCEPT_MASTER_COMMAND));
	}
	return 0;
}


int LuaSetTongLevel(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nParamNum = Lua_GetTopIndex(L);
		if (nParamNum > 1)
		{
			STONG_CHANGE_TONG_INFO_COMMAND	sLevel;
			sLevel.ProtocolID = enumC2S_CHANGE_TONG_LEVEL;
			sLevel.ProtocolFamily = pf_tong;
			sLevel.m_dwTongNameID = g_FileName2Id(Player[nPlayerIndex].m_cTong.m_szName);
			sLevel.m_dwParam = nPlayerIndex;
			sLevel.m_nValue = (int)Lua_ValueToNumber(L, 1);
			if (g_pTongClient)
				g_pTongClient->SendPackToServer((const void*)&sLevel, sizeof(STONG_CHANGE_TONG_INFO_COMMAND));
		}
	}
	return 0;
}

int LuaSetTongMoney(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nParamNum = Lua_GetTopIndex(L);
		if (nParamNum > 1)
		{
			STONG_CHANGE_TONG_INFO_COMMAND	sMoney;
			sMoney.ProtocolID = enumC2S_CHANGE_TONG_MONEY;
			sMoney.ProtocolFamily = pf_tong;
			sMoney.m_dwTongNameID = g_FileName2Id(Player[nPlayerIndex].m_cTong.m_szName);
			sMoney.m_dwParam = nPlayerIndex;
			sMoney.m_nValue = (int)Lua_ValueToNumber(L, 1);
			if (g_pTongClient)
				g_pTongClient->SendPackToServer((const void*)&sMoney, sizeof(STONG_CHANGE_TONG_INFO_COMMAND));
		}
	}
	return 0;
}

int LuaSetTongEff(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nParamNum = Lua_GetTopIndex(L);
		if (nParamNum > 1)
		{
			STONG_CHANGE_TONG_INFO_COMMAND	sEff;
			sEff.ProtocolID = enumC2S_CHANGE_TONG_EFF;
			sEff.ProtocolFamily = pf_tong;
			sEff.m_dwTongNameID = g_FileName2Id(Player[nPlayerIndex].m_cTong.m_szName);
			sEff.m_dwParam = nPlayerIndex;
			sEff.m_nValue = (int)Lua_ValueToNumber(L, 1);
			if (g_pTongClient)
				g_pTongClient->SendPackToServer((const void*)&sEff, sizeof(STONG_CHANGE_TONG_INFO_COMMAND));
		}
	}
	return 0;
}

int LuaSetTongMemEffLW(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nParamNum = Lua_GetTopIndex(L);
		if (nParamNum > 1)
		{
			STONG_CHANGE_TONG_MEMBEREFF_COMMAND	sMemEff;
			sMemEff.ProtocolFamily	= pf_tong;
			sMemEff.ProtocolID		= enumC2S_CHANGE_TONG_MEMBEREFF;
			sMemEff.m_nValue1		= KSG_StringSetValue(3, Player[nPlayerIndex].m_cTong.m_nSaveEff, defTONG_EFF_LASTWEEK, (int)Lua_ValueToNumber(L, 1));
			sMemEff.m_nValue2		= 0;
			sMemEff.m_dwParam		= nPlayerIndex;
			sMemEff.m_dwTongNameID	= g_FileName2Id(Player[nPlayerIndex].m_cTong.m_szName);
			sMemEff.m_btFigure	= Player[nPlayerIndex].m_cTong.m_nFigure;
			memcpy(sMemEff.m_szName, Npc[Player[nPlayerIndex].m_nIndex].Name, sizeof(Npc[Player[nPlayerIndex].m_nIndex].Name));
			if (g_pTongClient)
					g_pTongClient->SendPackToServer((const void*)&sMemEff, sizeof(sMemEff));
		}
	}
	return 0;
}

int LuaSetTongMemEffTW(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nParamNum = Lua_GetTopIndex(L);
		if (nParamNum > 1)
		{
			STONG_CHANGE_TONG_MEMBEREFF_COMMAND	sMemEff;
			sMemEff.ProtocolFamily	= pf_tong;
			sMemEff.ProtocolID		= enumC2S_CHANGE_TONG_MEMBEREFF;
			sMemEff.m_nValue1		= KSG_StringSetValue(3, Player[nPlayerIndex].m_cTong.m_nSaveEff, defTONG_EFF_THISWEEK, (int)Lua_ValueToNumber(L,1));
			sMemEff.m_nValue1		= KSG_StringSetValue(3, sMemEff.m_nValue1, defTONG_EFF_USEABLE, 
				KSG_StringGetValue(3, Player[nPlayerIndex].m_cTong.m_nSaveEff, defTONG_EFF_USEABLE)+(int)Lua_ValueToNumber(L,1));
			sMemEff.m_nValue2		= (int)Lua_ValueToNumber(L, 1);
			sMemEff.m_dwParam		= nPlayerIndex;
			sMemEff.m_dwTongNameID	= g_FileName2Id(Player[nPlayerIndex].m_cTong.m_szName);
			sMemEff.m_btFigure	= Player[nPlayerIndex].m_cTong.m_nFigure;
			memcpy(sMemEff.m_szName, Npc[Player[nPlayerIndex].m_nIndex].Name, sizeof(Npc[Player[nPlayerIndex].m_nIndex].Name));
			if (g_pTongClient)
					g_pTongClient->SendPackToServer((const void*)&sMemEff, sizeof(sMemEff));
		}
	}
	return 0;
}

int LuaSetTongMemEffUB(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nParamNum = Lua_GetTopIndex(L);
		if (nParamNum > 1)
		{
			STONG_CHANGE_TONG_MEMBEREFF_COMMAND	sMemEff;
			sMemEff.ProtocolFamily	= pf_tong;
			sMemEff.ProtocolID		= enumC2S_CHANGE_TONG_MEMBEREFF;
			sMemEff.m_nValue1		= KSG_StringSetValue(3, Player[nPlayerIndex].m_cTong.m_nSaveEff, defTONG_EFF_USEABLE, (int)Lua_ValueToNumber(L,1));
			sMemEff.m_nValue2		= 0;
			sMemEff.m_dwParam		= nPlayerIndex;
			sMemEff.m_dwTongNameID	= g_FileName2Id(Player[nPlayerIndex].m_cTong.m_szName);
			sMemEff.m_btFigure	= Player[nPlayerIndex].m_cTong.m_nFigure;
			memcpy(sMemEff.m_szName, Npc[Player[nPlayerIndex].m_nIndex].Name, sizeof(Npc[Player[nPlayerIndex].m_nIndex].Name));
			if (g_pTongClient)
					g_pTongClient->SendPackToServer((const void*)&sMemEff, sizeof(sMemEff));
		}
	}
	return 0;
}

int LuaGetTongMemEff(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
	if (Lua_GetTopIndex(L) > 1)
		nPlayerIndex = (int)Lua_ValueToNumber(L, 1);
	if (nPlayerIndex > 0)
	{
		Lua_PushNumber(L, KSG_StringGetValue(3,(int)Player[nPlayerIndex].m_cTong.m_nSaveEff,defTONG_EFF_LASTWEEK));
		Lua_PushNumber(L, KSG_StringGetValue(3,(int)Player[nPlayerIndex].m_cTong.m_nSaveEff,defTONG_EFF_THISWEEK));
		Lua_PushNumber(L, KSG_StringGetValue(3,(int)Player[nPlayerIndex].m_cTong.m_nSaveEff,defTONG_EFF_USEABLE));
		return 3;
	}
	return 0;
}

int LuaSetTongParam(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nParamNum = Lua_GetTopIndex(L);
		if (nParamNum > 1)
		{
			STONG_CHANGE_TONG_INFO_COMMAND	TongParam;
			TongParam.ProtocolID = enumC2S_TONG_CHANGE_TONGPARAM;
			TongParam.ProtocolFamily = pf_tong;
			TongParam.m_dwTongNameID = g_FileName2Id(Player[nPlayerIndex].m_cTong.m_szName);
			TongParam.m_dwParam = nPlayerIndex;
			TongParam.m_nValue = (int)Lua_ValueToNumber(L, 1);
			if (g_pTongClient)
				g_pTongClient->SendPackToServer((const void*)&TongParam, sizeof(STONG_CHANGE_TONG_INFO_COMMAND));
		}
	}
	return 0;
}

//门派
int LuaGetPlayerFaction(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);				
	if (nPlayerIndex > 0)								
	{	
		char szName[32];
		Player[nPlayerIndex].GetFaction(szName);
		Lua_PushString(L, szName);
	}
	else
	{
		Lua_PushString(L,"");
	}
	return 1;
}

int LuaGetPlayerFactionName(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);				
	if (nPlayerIndex > 0)								
	{	
		char szName[32];
		Player[nPlayerIndex].GetFactionName(szName);
		Lua_PushString(L, szName);
	}
	else
	{
		Lua_PushString(L,"");
	}
	return 1;
}

int LuaGetPlayerFactionCamp(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);				
	if (nPlayerIndex > 0)								
		Lua_PushNumber(L, Player[nPlayerIndex].GetFactionCamp());
	else
		Lua_PushNumber(L, -1);

	return 1;
}

int LuaGetPlayerFactionNo(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);				
	if (nPlayerIndex > 0)								
		Lua_PushNumber(L, Player[nPlayerIndex].GetFactionNo());
	else
		Lua_PushNumber(L, -1);

	return 1;
}

int LuaChangePlayerFaction(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);				
	int nResult = 0;
	if (nPlayerIndex > 0)							
	{
		const char * szFactionName = Lua_ValueToString(L,1);
		Player[nPlayerIndex].LeaveCurFaction();
		if (strlen(szFactionName) == 0) 
		{
			nResult = 1;
		}
		else
		{
			nResult = Player[nPlayerIndex].AddFaction((char *)szFactionName);
		}
	}
	Lua_PushNumber(L, nResult);
	return 1;
}

//抗性 *************************************************************************************
//0表示当前,1表示原始的,2表示最大的
int LuaGetPlayerColdResist(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nType = (int)Lua_ValueToNumber(L,1);
		switch((int)Lua_ValueToNumber(L,1))
		{
		case 0:
			Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_CurrentColdResist);break;
		case 1:
			Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_ColdResist); break;
		case 2:
			Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_ColdResistMax);break;
		default:
			Lua_PushNil(L);
		} 
	}
	else 
		Lua_PushNil(L);
	return 1;
}

int LuaSetPlayerColdResist(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nValue = (int)Lua_ValueToNumber(L,2);
		if (nValue < 0) nValue = 0;
		if (nValue > Npc[Player[nPlayerIndex].m_nIndex].m_ColdResistMax) nValue = Npc[Player[nPlayerIndex].m_nIndex].m_ColdResistMax;
		
		int nType = (int)Lua_ValueToNumber(L,1);
		
		switch((int)Lua_ValueToNumber(L,1))
		{
		case 0:
			Npc[Player[nPlayerIndex].m_nIndex].m_CurrentColdResist = nValue;
			break;
			
		case 1:
			Npc[Player[nPlayerIndex].m_nIndex].m_ColdResist = nValue;
			break;
		case 2:
			Npc[Player[nPlayerIndex].m_nIndex].m_ColdResistMax = nValue;
			break;
		}
		
		
	} 
	return 0;
	
}

int LuaGetPlayerFireResist(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		
		int nType = (int)Lua_ValueToNumber(L,1);
		switch((int)Lua_ValueToNumber(L,1))
		{
		case 0:
			Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_CurrentFireResist);break;
		case 1:
			Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_FireResist); break;
		case 2:
			Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_FireResistMax);break;
		default:
			Lua_PushNil(L);
		}
	} 
	else 
		Lua_PushNil(L);
	return 1;
}

int LuaSetPlayerFireResist(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nValue = (int)Lua_ValueToNumber(L,2);
		if (nValue < 0) nValue = 0;
		if (nValue > Npc[Player[nPlayerIndex].m_nIndex].m_FireResistMax)
			nValue = Npc[Player[nPlayerIndex].m_nIndex].m_FireResistMax;
		
		int nType = (int)Lua_ValueToNumber(L,1);
		
		switch(nType)
		{
		case 0:
			Npc[Player[nPlayerIndex].m_nIndex].m_CurrentFireResist = nValue;
			break;
			
		case 1:
			Npc[Player[nPlayerIndex].m_nIndex].m_FireResist = nValue;
			break;
		case 2:
			Npc[Player[nPlayerIndex].m_nIndex].m_FireResistMax = nValue;
			break;
		}
	} 
	return 0;
}


int LuaGetPlayerLightResist(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nType = (int)Lua_ValueToNumber(L,1);
		switch((int)Lua_ValueToNumber(L,1))
		{
		case 0:
			Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_CurrentLightResist);break;
		case 1:
			Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_LightResist); break;
		case 2:
			Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_LightResistMax);break;
		default:
			Lua_PushNil(L);
		}
	} 
	else
		Lua_PushNil(L);
	return 1;
}

int LuaSetPlayerLightResist(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		
		int nValue = (int)Lua_ValueToNumber(L,2);
		if (nValue < 0) nValue = 0;
		if (nValue > Npc[Player[nPlayerIndex].m_nIndex].m_LightResistMax) nValue = Npc[Player[nPlayerIndex].m_nIndex].m_LightResistMax;
		
		int nType = (int)Lua_ValueToNumber(L,1);
		
		switch((int)Lua_ValueToNumber(L,1))
		{
		case 0:
			Npc[Player[nPlayerIndex].m_nIndex].m_CurrentLightResist = nValue;
			break;
			
		case 1:
			Npc[Player[nPlayerIndex].m_nIndex].m_LightResist = nValue;
			break;
		case 2:
			Npc[Player[nPlayerIndex].m_nIndex].m_LightResistMax = nValue;
			break;
		}
	} 
	else 
		Lua_PushNil(L);
	return 0;
}


int LuaGetPlayerPoisonResist(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nType = (int)Lua_ValueToNumber(L,1);
		switch((int)Lua_ValueToNumber(L,1))
		{
		case 0:
			Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_CurrentPoisonResist);break;
		case 1:
			Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_PoisonResist); break;
		case 2:
			Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_PoisonResistMax);break;
		default:
			Lua_PushNil(L);
		}
	} 
	else
		Lua_PushNil(L);
	return 1;
}

int LuaSetPlayerPoisonResist(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		{
			int nValue = (int)Lua_ValueToNumber(L,2);
            if (nValue < 0) nValue = 0;
            if (nValue > Npc[Player[nPlayerIndex].m_nIndex].m_PoisonResistMax) nValue = Npc[Player[nPlayerIndex].m_nIndex].m_PoisonResistMax;
			
            int nType = (int)Lua_ValueToNumber(L,1);
			
			switch((int)Lua_ValueToNumber(L,1))
			{
			case 0:
				Npc[Player[nPlayerIndex].m_nIndex].m_CurrentPoisonResist = nValue;
				break;
				
			case 1:
				Npc[Player[nPlayerIndex].m_nIndex].m_PoisonResist = nValue;
                break;
			case 2:
				Npc[Player[nPlayerIndex].m_nIndex].m_PoisonResistMax = nValue;
                break;
			}
		}
		
	} 
	
	
	return 0;
	
}

int LuaGetPlayerPhysicsResist(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		
		{
			int nType = (int)Lua_ValueToNumber(L,1);
			switch((int)Lua_ValueToNumber(L,1))
			{
			case 0:
				Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_CurrentPhysicsResist);break;
			case 1:
				Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_PhysicsResist); break;
			case 2:
				Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_PhysicsResistMax);break;
			default:
				Lua_PushNil(L);
			}
		}
		
	} 
	else
		Lua_PushNil(L);
	return 1;
}



int LuaSetPlayerPhysicsResist(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		
		{
			int nValue = (int)Lua_ValueToNumber(L,2);
            if (nValue < 0) nValue = 0;
            if (nValue > Npc[Player[nPlayerIndex].m_nIndex].m_PhysicsResistMax) nValue = Npc[Player[nPlayerIndex].m_nIndex].m_PhysicsResistMax;
			
            int nType = (int)Lua_ValueToNumber(L,1);
			
			switch((int)Lua_ValueToNumber(L,1))
			{
			case 0:
				Npc[Player[nPlayerIndex].m_nIndex].m_CurrentPhysicsResist = nValue;
				break;
				
			case 1:
				Npc[Player[nPlayerIndex].m_nIndex].m_PhysicsResist = nValue;
                break;
			case 2:
				Npc[Player[nPlayerIndex].m_nIndex].m_PhysicsResistMax = nValue;
                break;
			}
		}
		
	} 
	return 0;
	
}

int LuaGetNextExp(Lua_State *L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
        Lua_PushNumber(L, Player[nPlayerIndex].m_nNextLevelExp);
	}
	else
		Lua_PushNil(L);
	return 1;
}

//经验值*********************************************************************
int LuaGetPlayerExp(Lua_State *L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
        Lua_PushNumber(L, Player[nPlayerIndex].m_nExp);
	}
	else
		Lua_PushNil(L);
	return 1;
}

//AddExp(200,10,0)
int LuaModifyPlayerExp(Lua_State * L)
{
	int bAllTeamGet = 0;
	if (Lua_GetTopIndex(L) >= 3)	
		bAllTeamGet = (int)Lua_ValueToNumber(L,3);
	
	DWORD nDValue = (DWORD)Lua_ValueToNumber(L,1);
	DWORD nTarLevel = (DWORD)Lua_ValueToNumber(L,2);
	
	int nPlayerIndex = GetPlayerIndex(L);				
	if (nPlayerIndex > 0)								
	{									
		if (bAllTeamGet)
			Player[nPlayerIndex].AddExp(nDValue, nTarLevel);
		else
			Player[nPlayerIndex].AddSelfExp(nDValue, nTarLevel);
	}
	return 0;
}

int LuaAddOwnExp(Lua_State * L)
{
	if (Lua_GetTopIndex(L) <=0 ) return 0;
	int nPlayerIndex = GetPlayerIndex(L);				
	if (nPlayerIndex > 0)								
	{									
		int nExp = (int)Lua_ValueToNumber(L,1);
		if(nExp < MAX_INT)
			Player[nPlayerIndex].DirectAddExp(nExp);
	}
	return 0;
}

int LuaAddStackExp(Lua_State * L)
{
    if (Lua_GetTopIndex(L) < 1 )
        return 0;
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0)
        return 0;
        
    int nExp = (DWORD)Lua_ValueToNumber(L,1);
    if (nExp > 0)
    {
        while (nExp > 0)
        {
            int nExpAdd = Player[nPlayerIndex].m_nNextLevelExp - Player[nPlayerIndex].m_nExp;
            if (nExp >= nExpAdd)
            {
                nExp = nExp - nExpAdd;
            }
            else
            {
                nExpAdd = nExp;
                nExp = 0;
            }
            Player[nPlayerIndex].DirectAddExp(nExpAdd);
        }
    }
    return 0;
}


int LuaGetPlayerLevel(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_Level);
	} 
	else
		Lua_PushNil(L);
	return 1;
}

int LuaGetPlayerLife(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		
		{
			int nType = (int)Lua_ValueToNumber(L,1);
			switch((int)Lua_ValueToNumber(L,1))
			{
			case 0:
				Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_CurrentLife);break;
			case 1:
				Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_CurrentLifeMax); break;
			case 2:
				Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_LifeMax);break;
			default:
				Lua_PushNil(L);
			}
		}
		
	} 
	else
		Lua_PushNil(L);
	return 1;
}

int LuaRestorePlayerLife(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		Npc[Player[nPlayerIndex].m_nIndex].RestoreLife();
	}
	return 0;
}

int LuaRestorePlayerMana(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		Npc[Player[nPlayerIndex].m_nIndex].RestoreMana();
	}
	return 0;
}

int LuaRestorePlayerStamina(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		Npc[Player[nPlayerIndex].m_nIndex].RestoreStamina();
	}
	return 0;
}

int LuaGetPlayerLifeReplenish(Lua_State * L)
{
	MacroFun_GetPlayerInfoInt( L, m_LifeReplenish);
}


int LuaGetPlayerMana(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		
		{
			int nType = (int)Lua_ValueToNumber(L,1);
			switch((int)Lua_ValueToNumber(L,1))
			{
			case 0:
				Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_CurrentMana);break;
			case 1:
				Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_ManaMax); break;
			case 2:
				Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_ManaMax);break;
			default:
				Lua_PushNil(L);
			}
		}
		
	} 
	else
		Lua_PushNil(L);
	return 1;
}

int LuaGetPlayerStamina(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		
		{
			int nType = (int)Lua_ValueToNumber(L,1);
			switch((int)Lua_ValueToNumber(L,1))
			{
			case 0:
				Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_CurrentStamina);break;
			case 1:
				Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_StaminaMax); break;
			case 2:
				Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_StaminaMax);break;
			default:
				Lua_PushNil(L);
			}
		}
		
	} 
	else
		Lua_PushNil(L);
	return 1;
}


int LuaGetPlayerManaReplenish(Lua_State * L)
{
	MacroFun_GetPlayerInfoInt(L , m_ManaReplenish);
}

int LuaGetPlayerDefend(Lua_State * L)
{
    MacroFun_GetPlayerInfoInt(L , m_Defend);
}

int LuaGetPlayerSex(Lua_State * L)
{
	MacroFun_GetPlayerInfoInt(L , m_nSex);
}

int LuaGetPlayerIndex(Lua_State * L)
{
	MacroFun_GetPlayerInfoInt(L , GetPlayerIdx());
	return 0;
}


int LuaGetPlayerNpcIdx(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
	int nIndex = nPlayerIndex;
	if (nPlayerIndex > 0)
	{
		if (Lua_GetTopIndex(L) > 1)
			nIndex = (int)Lua_ValueToNumber(L, 1);
        Lua_PushNumber(L, Player[nIndex].m_nIndex);
	}
	else
		Lua_PushNil(L);
	return 1;
}

int LuaGetPlayerAccount(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
	int nIndex = nPlayerIndex;
	if (nPlayerIndex > 0)
	{
		if (Lua_GetTopIndex(L) > 1)
			nIndex = (int)Lua_ValueToNumber(L, 1);
        Lua_PushString(L, Player[nIndex].AccountName);
	}
	else
		Lua_PushNil(L);
	
	return 1;
	
}

int LuaGetPlayerSeries(Lua_State * L)
{
	MacroFun_GetPlayerInfoInt(L , m_Series);
}

int LuaSetPlayerSeries(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nValue = (int)Lua_ValueToNumber(L,1);
		Npc[Player[nPlayerIndex].m_nIndex].SetSeries(nValue);
	} 
	return 0;
    
}

int LuaGetPlayerCount(Lua_State * L)
{
	Lua_PushNumber(L, PlayerSet.GetPlayerNumber());
	return 1;
}

int LuaGetNpcCount(Lua_State * L)
{
	Lua_PushNumber(L, NpcSet.GetNpcNumber());
	return 1;
}

int LuaGetTotalItem(Lua_State * L)
{
	Lua_PushNumber(L, ItemSet.GetItemNumber());
	return 1;
}

int LuaGetPlayerName(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
	int nIndex = nPlayerIndex;
	if (nPlayerIndex > 0)
	{
		if (Lua_GetTopIndex(L) > 1)
			nIndex = (int)Lua_ValueToNumber(L, 1);
        Lua_PushString(L, Player[nIndex].Name);
	}
	else
		Lua_PushNil(L);
	
	return 1;
	
}

int LuaGetPlayerID(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
        Lua_PushNumber(L, Player[nPlayerIndex].m_dwID);
	}
	else
		Lua_PushNil(L);
	
	return 1;
}

int LuaGetMateName(Lua_State * L)
{	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;

	Lua_PushString(L, Player[nPlayerIndex].m_cTask.GetSaveStr(TASKVALUE_BASEDATA_MATENAME));
	return 1;
}

int LuaFindPlayer(Lua_State * L)
{
	int nIndex = 0;
	if (Lua_IsNumber(L, 1))
	{
		DWORD dwID = (DWORD)Lua_ValueToNumber(L,1);
		nIndex = NpcSet.SearchUUID(dwID);
	}
	else if (Lua_IsString(L, 1))
	{
		const char*	pszName = (const char*)Lua_ValueToString(L,1);
		nIndex = PlayerSet.GetFirstPlayer();
		while(nIndex > 0)
		{
			if (strcmp(Player[nIndex].Name, pszName) == 0)
				break;

			nIndex = PlayerSet.GetNextPlayer();
		}
	}
	Lua_PushNumber(L, nIndex);
	return 1;
}

int LuaFindNearNpc(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nIndex = 0;
		if (Lua_IsNumber(L,1))
			nIndex = Player[nPlayerIndex].FindNearNpc((int)Lua_ValueToNumber(L,1), 
				Lua_GetTopIndex(L) > 2?(int)Lua_ValueToNumber(L,2):0);
		else if (Lua_IsString(L,1))
			nIndex = Player[nPlayerIndex].FindNearNpc((char*)Lua_ValueToString(L,1), 
				Lua_GetTopIndex(L) > 2?(int)Lua_ValueToNumber(L,2):0);
		Lua_PushNumber(L, nIndex);
		return 1;
	}
	return 0;
}

int LuaFindAroundNpc(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		if (Lua_IsNumber(L, 1))
		{
			int nIndex = Player[nPlayerIndex].FindAroundNpc((DWORD)Lua_ValueToNumber(L,1));
			Lua_PushNumber(L, nIndex);
			return 1;
		}
		else
			Lua_PushNil(L);
	}
	return 0;
}

int LuaGetPlayerLeadExp(Lua_State * L)
{
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
        Lua_PushNumber(L, Player[nPlayerIndex].m_nLeadExp);
	}
	else
		Lua_PushNil(L);
	
	return 1;
}

int LuaGetPlayerLeadLevel(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
        Lua_PushNumber(L, Player[nPlayerIndex].m_nLeadLevel);
	}
	else
		Lua_PushNil(L);
	
	return 1;
}



int LuaGetPlayerRestAttributePoint(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
        Lua_PushNumber(L, Player[nPlayerIndex].m_nAttributePoint);
	}
	else
		Lua_PushNil(L);
	
	return 1;
}

int LuaGetPlayerRestSkillPoint(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
        Lua_PushNumber(L, Player[nPlayerIndex].m_nSkillPoint);
	}
	else
		Lua_PushNil(L);
	
	return 1;
}

/*
int LuaModifyPlayerRestSkillPoint(Lua_State *L)
{
//Question
int nPlayerIndex = GetPlayerIndex(L);
if (nPlayerIndex > 0)
{
int nDValue = (int)Lua_ValueToNumber(L, 1);

  int nNewSkillPoint = Player[nPlayerIndex].m_nSkillPoint + nDValue;
  if (nNewSkillPoint < 0 ) return 0;
  
	Player[nPlayerIndex].m_nSkillPoint = nNewSkillPoint;
	}
	
	  return 0;
	  }
*/

//基本属性
int LuaGetPlayerLucky(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nType = (int)Lua_ValueToNumber(L,1);
		switch((int)Lua_ValueToNumber(L,1))
		{
		case 0:
			Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_CurrentLucky);
			break;
		case 1:
			Lua_PushNumber(L, Player[nPlayerIndex].m_nLucky);
			break;
		case 2:
			Lua_PushNumber(L, Player[nPlayerIndex].m_nLucky);
			break;
		default:
			Lua_PushNil(L);
		}
		
	} 
	return 1;
}

int LuaGetPlayerEngergy(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		
		int nType = (int)Lua_ValueToNumber(L,1);
		switch((int)Lua_ValueToNumber(L,1))
		{
		case 0:
			Lua_PushNumber(L, Player[nPlayerIndex].m_nCurEngergy);break;
		case 1:
			Lua_PushNumber(L, Player[nPlayerIndex].m_nEngergy);break;
		case 2:
			Lua_PushNumber(L, Player[nPlayerIndex].m_nEngergy);break;
		default:
			Lua_PushNil(L);
		}
	}
	
	return 1;
}

int LuaResetBaseAttribute(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	
	int nParamCount = 0;
	if ( (nParamCount = Lua_GetTopIndex(L)) < 2) return 0;

	if (nPlayerIndex > 0)
	{		
		PLAYER_ADD_BASE_ATTRIBUTE_COMMAND cmd;
		cmd.m_btAttribute = (int)Lua_ValueToNumber(L,1);;
		cmd.m_nAddNo = (int)Lua_ValueToNumber(L,2);;
		cmd.ProtocolType = c2s_playeraddbaseattribute;
		Player[nPlayerIndex].ResetBaseAttribute((BYTE*)&cmd);
	}
	return 1;
}

int LuaResetProp(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);

	int nResult = 0;
	if (nPlayerIndex > 0)
		nResult = Player[nPlayerIndex].ResetProp();

	Lua_PushNumber(L, nResult);
	return 1;
	
}

int LuaSetPlayerEngergy(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
		return 0;
    int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nValue = (int)Lua_ValueToNumber(L, 1);
		if (nValue < 0 && Player[nPlayerIndex].m_nEngergy - nValue <= 0)
			nValue = 0;
		Player[nPlayerIndex].m_nAttributePoint -= nValue;
		Player[nPlayerIndex].SetBaseEngergy(nValue);
	}
	return 1;
}

int LuaGetPlayerDexterity(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		
		int nType = (int)Lua_ValueToNumber(L,1);
		switch((int)Lua_ValueToNumber(L,1))
		{
		case 0:
			Lua_PushNumber(L, Player[nPlayerIndex].m_nCurDexterity);break;
		case 1:
			Lua_PushNumber(L, Player[nPlayerIndex].m_nDexterity);break;
		case 2:
			Lua_PushNumber(L, Player[nPlayerIndex].m_nDexterity);break;
		default:
			Lua_PushNil(L);
		}
		
	} 
	return 1;
}

int LuaSetPlayerDexterity(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
		return 0;
    int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nValue = (int)Lua_ValueToNumber(L, 1);
		if (nValue < 0 && Player[nPlayerIndex].m_nDexterity - nValue <= 0)
			nValue = 0;
		Player[nPlayerIndex].m_nAttributePoint -= nValue;
		Player[nPlayerIndex].SetBaseDexterity(nValue);
	}
	return 1;
}

int LuaGetPlayerStrength(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nType = (int)Lua_ValueToNumber(L,1);
		switch((int)Lua_ValueToNumber(L,1))
		{
		case 0:
			Lua_PushNumber(L, Player[nPlayerIndex].m_nCurStrength);break;
		case 1:
			Lua_PushNumber(L, Player[nPlayerIndex].m_nStrength);break;
		case 2:
			Lua_PushNumber(L, Player[nPlayerIndex].m_nStrength);break;
		default:
			Lua_PushNil(L);
		}
		
	} 
	return 1;
	
}

int LuaSetPlayerStrength(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
		return 0;
    int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nValue = (int)Lua_ValueToNumber(L, 1);
		if (nValue < 0 && Player[nPlayerIndex].m_nStrength - nValue <= 0)
			nValue = 0;
		Player[nPlayerIndex].m_nAttributePoint -= nValue;
		Player[nPlayerIndex].SetBaseStrength(nValue);
	}
	return 1;
}

int LuaGetPlayerVitality(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		
		int nType = (int)Lua_ValueToNumber(L,1);
		switch((int)Lua_ValueToNumber(L,1))
		{
		case 0:
			Lua_PushNumber(L, Player[nPlayerIndex].m_nCurVitality);break;
		case 1:
			Lua_PushNumber(L, Player[nPlayerIndex].m_nVitality);break;
		case 2:
			Lua_PushNumber(L, Player[nPlayerIndex].m_nVitality);break;
		default:
			Lua_PushNil(L);
		}
		
	} 
	return 1;
	
}

int LuaSetPlayerVitality(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
		return 0;
    int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nValue = (int)Lua_ValueToNumber(L, 1);
		if (nValue < 0 && Player[nPlayerIndex].m_nVitality - nValue <= 0)
			nValue = 0;
		Player[nPlayerIndex].m_nAttributePoint -= nValue;
		Player[nPlayerIndex].SetBaseVitality(nValue);
	}
	return 1;
}

int LuaGetPlayerCashMoney(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
	
    if (nPlayerIndex > 0)
	{
		if(Lua_GetTopIndex(L) > 1)
		{
			int nType = (int)Lua_ValueToNumber(L,1);
			if(nType == 1)
				Lua_PushNumber(L, Player[nPlayerIndex].m_ItemList.GetMoney(room_repository));
			else if(nType == 2)
				Lua_PushNumber(L, Player[nPlayerIndex].m_ItemList.GetMoney(room_equipment)+Player[nPlayerIndex].m_ItemList.GetMoney(room_repository));
		}
		else
			Lua_PushNumber(L, Player[nPlayerIndex].m_ItemList.GetMoney(room_equipment));
	}
	else Lua_PushNumber(L,0);
	
	return 1;
}

int LuaPlayerPayMoney(Lua_State * L)
{
    
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0)
	{
        int nMoney = (int)Lua_ValueToNumber(L, 1);
        if (nMoney <= 0) return 0;
		if(Lua_GetTopIndex(L) > 2)
		{
			int nType = (int)Lua_ValueToNumber(L,2);
			if(nType == 1)
				Player[nPlayerIndex].m_ItemList.AddMoney(room_repository, -nMoney);
			else if(nType == 2)
			{
				int nLMoney = nMoney - Player[nPlayerIndex].m_ItemList.GetMoney(room_equipment);
				if(nLMoney > 0)
				{
					Player[nPlayerIndex].m_ItemList.AddMoney(room_repository, Player[nPlayerIndex].m_ItemList.GetMoney(room_equipment));
					Player[nPlayerIndex].m_ItemList.AddMoney(room_equipment, -nLMoney);
				}
				else
					Player[nPlayerIndex].m_ItemList.AddMoney(room_repository, nMoney);
			}
			Lua_PushNumber(L, 1);
		}
		else
		{
			if (Player[nPlayerIndex].Pay(nMoney))
				Lua_PushNumber(L, 1);
			else
				Lua_PushNumber(L, 0);
		}
	}
	else
		Lua_PushNumber(L, 0);
	
	return 1;
}

int LuaPlayerEarnMoney (Lua_State  *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0)
	{
        int nMoney = (int)Lua_ValueToNumber(L, 1);
        if (nMoney <= 0) return 0;
        Player[nPlayerIndex].Earn(nMoney);
	}
	return 0;	
}

int LuaPlayerPrePayMoney(Lua_State *L)
{   
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0)
	{
        int nMoney = (int)Lua_ValueToNumber(L, 1);
        if (nMoney <= 0) return 0;
        if (Player[nPlayerIndex].PrePay(nMoney))
			Lua_PushNumber(L, 1);
		else
			Lua_PushNumber(L, 0);
	}
	else
		Lua_PushNumber(L, 0);
	
	return 1;
}

int LuaGetPlayerFortune(Lua_State *L)
{   
    int nPlayerIndex = GetPlayerIndex(L);
	
    if (nPlayerIndex > 0)
	{
		Lua_PushNumber(L, Player[nPlayerIndex].m_ItemList.GetPlayerFortune());
	}
	else Lua_PushNumber(L,0);
	
	return 1;
}

//Attack dwID, Damage
int LuaAttackNpc(Lua_State * L)
{
	int nParamCount = 0;
	if ( (nParamCount = Lua_GetTopIndex(L)) < 2) return 0;
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	
	DWORD nNpcID = (DWORD)Lua_ValueToNumber(L,1);
	int nNpcIndex = Player[nPlayerIndex].FindAroundNpc(nNpcID);//NpcSet.SearchID(nNpcID);
	if (nNpcIndex <= 0) return 0;
	
	KMagicAttrib DamageMagicAttribs[MAX_MISSLE_DAMAGEATTRIB];
	memset(DamageMagicAttribs, 0, sizeof(DamageMagicAttribs));
	
	DamageMagicAttribs[0].nAttribType = magic_attackrating_v;
	DamageMagicAttribs[0].nValue[0] = Npc[nNpcIndex].m_CurrentLife;
	
	DamageMagicAttribs[1].nAttribType = magic_ignoredefense_p;
	DamageMagicAttribs[1].nValue[0] = 1;
	
	for (int i = 0; i < nParamCount - 1; i++)
	{
		int nVlau = (int)Lua_ValueToNumber(L, 2 + i);
		DamageMagicAttribs[i + 2].nValue[0] = (int)Lua_ValueToNumber(L, 2 + i);
		DamageMagicAttribs[i + 2].nValue[2] = (int)Lua_ValueToNumber(L, 2 + i);
	}
	
	Npc[nNpcIndex].ReceiveDamage(Player[nPlayerIndex].m_nIndex, -1, 0, DamageMagicAttribs, 0, 1, 0);
	return 0;
}

int LuaSetPlayerChatForbiddenFlag(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0)
		return 0;
	int nFlag = Lua_ValueToNumber(L, 1);
	Player[nPlayerIndex].SetChatForbiddenTm(nFlag);
	return 0;
}

// SetRoleChatFlag(roleName,flag)
int LuaSetRoleChatFlag(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);

	if ( nParamCount < 1)
		return 0;

	const char*	pszName = (const char*)Lua_ValueToString(L, 1);
	int nIndex = PlayerSet.GetFirstPlayer();
	while(nIndex > 0)
	{
		if (strcmp(Player[nIndex].Name, pszName) == 0)
			break;

		nIndex = PlayerSet.GetNextPlayer();
	}


	if (nIndex && Player[nIndex].m_nNetConnectIdx >= 0)
	{
		int nFlag;
		if ( nParamCount >=2 )
		{
			nFlag = (int) Lua_ValueToNumber(L, 2);
		}
		else
			nFlag=1;
		Player[nIndex ].SetChatForbiddenTm(nFlag);
	}
	return 0;
}

//ShutDownServer bIsSavePlayer
int LuaShutDownServer(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);

	g_ReleaseCore();	

	return 0;
}

int LuaKickOutPlayer(Lua_State *L)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;

	const char*	pszName = (const char*)Lua_ValueToString(L, 1);
	
	int nIndex = PlayerSet.GetFirstPlayer();
	while(nIndex > 0)
	{
		if (strcmp(Player[nIndex].Name, pszName) == 0)
			break;
		
		nIndex = PlayerSet.GetNextPlayer();
	}
	if (nIndex && Player[nIndex].m_nNetConnectIdx >= 0)
	{
		printf("GM Kick out specific player.\n");
		g_pServer->ShutdownClient(Player[nIndex].m_nNetConnectIdx);
	}
	return 0;
}

int LuaKickOutAccount(Lua_State *L)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;

	const char*	pszName = (const char*)Lua_ValueToString(L, 1);
	
	int nIndex = PlayerSet.GetFirstPlayer();
	while(nIndex > 0)
	{
		if (strcmpi(Player[nIndex].AccountName, pszName) == 0)
			break;
		
		nIndex = PlayerSet.GetNextPlayer();
	}
	if (nIndex && Player[nIndex].m_nNetConnectIdx >= 0)
		g_pServer->ShutdownClient(Player[nIndex].m_nNetConnectIdx);

	return 0;
}

int LuaKickOutSelf(Lua_State *L)
{
	int nIndex = GetPlayerIndex(L);
	
	if (nIndex <= 0)
		return 0;
	
	if (Player[nIndex].m_nNetConnectIdx >= 0)
		g_pServer->ShutdownClient(Player[nIndex].m_nNetConnectIdx);
	return 0;
}

int LuaKillNpc(Lua_State * L)
{
	int nParamCount = 0;
	if ( (nParamCount = Lua_GetTopIndex(L)) < 1) return 0;
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	
	DWORD nNpcID = (DWORD)Lua_ValueToNumber(L,1);
	int nNpcIndex = Player[nPlayerIndex].FindAroundNpc(nNpcID);
	if (nNpcIndex <= 0) return 0;
	KMagicAttrib DamageMagicAttribs[MAX_MISSLE_DAMAGEATTRIB];
	memset(DamageMagicAttribs, 0, sizeof(DamageMagicAttribs));
	DamageMagicAttribs[9].nAttribType = magic_physicsdamage_v;
	DamageMagicAttribs[9].nValue[0] = 100000;
	DamageMagicAttribs[9].nValue[2] = 100000;
	Npc[nNpcIndex].ReceiveDamage(Player[nPlayerIndex].m_nIndex, -1, 0, DamageMagicAttribs, 0, 1, 0);
	return 0;
}

int LuaKillPlayer(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) 
		return 0;
	KMagicAttrib DamageMagicAttribs[MAX_MISSLE_DAMAGEATTRIB];
	memset(DamageMagicAttribs, 0, sizeof(DamageMagicAttribs));
	DamageMagicAttribs[9].nAttribType = magic_physicsdamage_v;
	DamageMagicAttribs[9].nValue[0] = 100000;
	DamageMagicAttribs[9].nValue[2] = 100000;
	Npc[Player[nPlayerIndex].m_nIndex].ReceiveDamage(Player[nPlayerIndex].m_nIndex, -1, 0, DamageMagicAttribs, 0, 1, 0);
	return 0;
}

int LuaSetFightState(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	Npc[Player[nPlayerIndex].m_nIndex].SetFightMode(Lua_ValueToNumber(L,1) != 0);
	return 0;
}

int LuaGetFightState(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_FightMode);
	return 1;
}

int LuaSetLevel(Lua_State * L)
{	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)	
	{
		Lua_PushNumber(L,0);
		return 0;
	}
	
	if (Lua_IsNumber(L,1))
	{
		int nValue = (int)Lua_ValueToNumber(L, 1);
		Player[nPlayerIndex].SetLevel(nValue);
		Lua_PushNumber(L,1);
	}
	else
		Lua_PushNumber(L,0);
	
	return 1;
}


int LuaGetLevel(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)		Lua_PushNumber(L,0);
	
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_Level);
	return 1;
}
//
int	LuaUseTownPortal(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	
	int nResult = Player[nPlayerIndex].UseTownPortal();
	Lua_PushNumber(L, nResult);
	return 1;
}

int LuaReturnFromTownPortal(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	
	if (Player[nPlayerIndex].BackToTownPortal())
		Lua_PushNumber(L, 1);
	else
		Lua_PushNumber(L, 0);
	return 1;
}
#endif

int GetPlayerIndex(Lua_State * L)
{
	Lua_GetGlobal(L, SCRIPT_PLAYERINDEX);
    if (lua_isnil(L,Lua_GetTopIndex(L)))
        return -1;
	int nIndex = (int)Lua_ValueToNumber(L, Lua_GetTopIndex(L));
    if (nIndex >= MAX_PLAYER || nIndex <= 0) 
	{
		_ASSERT(0);
		return -1;
	}	if (Player[nIndex].m_nIndex >= MAX_NPC || Player[nIndex].m_nIndex < 0)
	{
		_ASSERT(0);
		return -1;
	}
    return nIndex;
}

int GetObjIndex(Lua_State * L)
{
	Lua_GetGlobal(L, SCRIPT_OBJINDEX);
    if (lua_isnil(L,Lua_GetTopIndex(L)))
        return -1;
	int nIndex = (int)Lua_ValueToNumber(L, Lua_GetTopIndex(L));
    if (nIndex >= MAX_OBJECT || nIndex <= 0) 
	{
		_ASSERT(0);
		return -1;
	}
	if (Object[nIndex].m_nIndex != nIndex)
	{
		_ASSERT(0);
		return -1;
	}
    return nIndex;
}




int  LuaMessage(Lua_State * L)
{
	const char * szString;
	szString  = lua_tostring (L,1);
	g_DebugLog((char *)szString);
	return 0;
}
#ifdef _SERVER
//AddStation(N)
int LuaAddPlayerWayPoint(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nWayPoint = (int)Lua_ValueToNumber(L,1);
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	{
		KIndexNode * pNode = (KIndexNode*) Player[nPlayerIndex].m_PlayerWayPointList.GetHead();
		while(pNode)
		{
			if (pNode->m_nIndex == nWayPoint) return 0;
			pNode = (KIndexNode*)pNode->GetNext();
		}
		
		KIndexNode * pNewNode = new KIndexNode;
		pNewNode->m_nIndex = nWayPoint;
		int nCount = Player[nPlayerIndex].m_PlayerWayPointList.GetNodeCount();
		for (int i = 0; i < nCount - 2; i ++ )
		{
			KIndexNode *  pDelNode = (KIndexNode*)Player[nPlayerIndex].m_PlayerWayPointList.RemoveHead();
			delete pDelNode;
		}
		Player[nPlayerIndex].m_PlayerWayPointList.AddTail(pNewNode);
	}
	return 0;
}

int LuaAddPlayerStation(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nStation = (int )Lua_ValueToNumber(L,1);
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	{
		KIndexNode * pNode = (KIndexNode*) Player[nPlayerIndex].m_PlayerStationList.GetHead();
		while(pNode)
		{
			if (pNode->m_nIndex == nStation) return 0;
			pNode = (KIndexNode*)pNode->GetNext();
		}
		
		KIndexNode * pNewNode = new KIndexNode;
		pNewNode->m_nIndex = nStation;
		Player[nPlayerIndex].m_PlayerStationList.AddTail(pNewNode);
	}
	return 0;
}

int LuaGetPlayerStationCount(Lua_State *L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	Lua_PushNumber(L, Player[nPlayerIndex].m_PlayerStationList.GetNodeCount());
	return 1;
}

//获得当前玩家有效城市列表中的第n个（除去当前所在城市）
int LuaGetPlayerStation(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	
	if (Lua_GetTopIndex(L) < 2)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	
	int nStationId = 0;
	KIndexNode * pNode = 	(KIndexNode*)Player[nPlayerIndex].m_PlayerStationList.GetHead();
	if (pNode)
	{
		int nNo = (int )Lua_ValueToNumber(L, 1);
		int nCurStation = (int) Lua_ValueToNumber(L,2);
		int nVisitNo = 0;
		while(pNode)
		{
			if (pNode->m_nIndex != nCurStation && g_GetPriceToStation( nCurStation , pNode->m_nIndex) > 0) 
			{
				nVisitNo ++;
				if (nVisitNo == nNo)
				{
					nStationId = pNode->m_nIndex;
					break;
				}
			}
			pNode = (KIndexNode*)pNode->GetNext();
		}
	}
	
	Lua_PushNumber(L, nStationId);
	
	return 1;
}

int LuaGetPlayerWayPoint(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	
	KIndexNode * pNode = (KIndexNode*)	Player[nPlayerIndex].m_PlayerWayPointList.GetHead();
	if (pNode)
	{
		int nNo = (int)Lua_ValueToNumber(L, 1);
		if (nNo > TASKVALUE_MAXWAYPOINT_COUNT) 
			Lua_PushNumber(L, 0);
		else
		{
			for (int i = 0; i < nNo - 1; i ++)
			{
				if (pNode == NULL ) break;
				pNode = (KIndexNode *)pNode->GetNext();
			}
			
			if (pNode)
				Lua_PushNumber(L, pNode->m_nIndex);
			else
				Lua_PushNumber(L, 0);
			
		}
	}
	else
		Lua_PushNumber(L, 0);
	
	return 1;
}
//根据传入的城市唯一id获得其名称
int LuaGetStationName(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	if (Lua_GetTopIndex(L) <= 0 ) 
	{
		Lua_PushString(L, "");
		return 1;
	}
	int nStationId = (int)Lua_ValueToNumber(L,1);
	char szName[50];
	g_StationTabFile.GetString(nStationId + 1, "DESC", NORMAL_UNCLEAR_WORD,  szName, 50 );
	Lua_PushString(L, szName);
	return 1;
}

int LuaGetWayPointName(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	if (Lua_GetTopIndex(L) <= 0 ) 
	{
		Lua_PushString(L, "");
		return 1;
	}
	int nWayPointId = (int)Lua_ValueToNumber(L,1);
	char szName[50];
	g_WayPointTabFile.GetString(nWayPointId + 1, "DESC", NORMAL_UNCLEAR_WORD,  szName, 50 );
	Lua_PushString(L, szName);
	return 1;
}

//GetCityCount 
int LuaGetAllStationCount(Lua_State * L)
{
	int nCityCount = g_StationTabFile.GetHeight() - 1;
	if (nCityCount < 0) nCityCount = 0;
	Lua_PushNumber(L,nCityCount);
	return 1;
}

//cityid, price = GetCity(citynum, curcity)
int LuaGetCity(Lua_State * L)
{
	return 0;
}


int LuaGetPriceToWayPoint(Lua_State *L)
{
	
	int nCurStation = (int)Lua_ValueToNumber(L,1);
	int nDesWayPoint = (int)Lua_ValueToNumber(L,2);
	Lua_PushNumber(L, g_GetPriceToWayPoint(nCurStation, nDesWayPoint));
	return 1;
}

int LuaGetPriceToStation(Lua_State *L)
{
	int nCurStation	 = (int)Lua_ValueToNumber(L,1);
	int nNextStation = (int)Lua_ValueToNumber(L,2);
	Lua_PushNumber(L, g_GetPriceToStation(nCurStation, nNextStation));
	return 1;
}

int LuaGetStationPos(Lua_State * L)
{
	int nStationId = (int)Lua_ValueToNumber(L,1);
	char szPos[100] ;
	int nCount = 0;
	int nRow = g_StationTabFile.FindColumn("COUNT");
	g_StationTabFile.GetInteger(nStationId + 1, nRow,  0, &nCount);
	if (nCount <= 0) return 0;
	int nRandSect = g_Random(100) % nCount + 1;
	char szSectName[32];
	sprintf(szSectName, "SECT%d", nRandSect);
	char szValue[100];
	nRow = g_StationTabFile.FindColumn(szSectName);
	g_StationTabFile.GetString(nStationId + 1, nRow, "0,0,0", szValue, 100);
	
    int nX, nY, nWorld;
    const char *pcszTemp = szValue;
    
    nWorld = KSG_StringGetInt(&pcszTemp, 0);
    KSG_StringSkipSymbol(&pcszTemp, ',');
    nX = KSG_StringGetInt(&pcszTemp, 0);
    KSG_StringSkipSymbol(&pcszTemp, ',');
    nY = KSG_StringGetInt(&pcszTemp, 0);
	//sscanf(szValue, "%d,%d,%d", &nWorld, &nX, &nY);
	
    Lua_PushNumber(L,nWorld);
	Lua_PushNumber(L,nX);
	Lua_PushNumber(L,nY);
	return 3;
}

int LuaGetWayPointPos(Lua_State * L)
{
	int nWayPointId = (int)Lua_ValueToNumber(L,1);
	char szPos[100] ;
	int nCount = 0;
	char szValue[30];
	int nRow;
	nRow = g_WayPointTabFile.FindColumn("SECT");
	g_WayPointTabFile.GetString(nWayPointId + 1, nRow, "0,0,0", szValue, 30);
	int nX, nY, nWorld;
    const char *pcszTemp = szValue;
    
    nWorld = KSG_StringGetInt(&pcszTemp, 0);
    KSG_StringSkipSymbol(&pcszTemp, ',');
    nX = KSG_StringGetInt(&pcszTemp, 0);
    KSG_StringSkipSymbol(&pcszTemp, ',');
    nY = KSG_StringGetInt(&pcszTemp, 0);
	//sscanf(szValue, "%d,%d,%d", &nWorld, &nX, &nY);
	
	Lua_PushNumber(L,nWorld);
	Lua_PushNumber(L,nX);
	Lua_PushNumber(L,nY);
	return 3;
}

int LuaGetWayPointFight(Lua_State * L)
{
	int nWayPointId = (int)Lua_ValueToNumber(L,1);
	int nFight;
	int nRow;
	nRow = g_WayPointTabFile.FindColumn("FightState");
	g_WayPointTabFile.GetInteger(nWayPointId + 1, nRow, 0, &nFight);
	
	Lua_PushNumber(L,nFight);
	return 1;
}

int LuaGetRank(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;

	Lua_PushNumber(L,Npc[Player[nPlayerIndex].m_nIndex].m_RankID);
	return 1;
}

int LuaSetRank(Lua_State * L)
{
	BYTE nRankID = (BYTE)Lua_ValueToNumber(L,1);
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;

	Npc[Player[nPlayerIndex].m_nIndex].m_RankID = nRankID;
	return 1;	
}

int LuaGetExpandRank(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;

	Lua_PushString(L, Npc[Player[nPlayerIndex].m_nIndex].m_CurExpandRank.szName);
	return 1;
}

int LuaSetExpandRank(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0;

	KExpandRank tmp;
	tmp.Release();

	strcpy(tmp.szName, (char*)Lua_ValueToString(L,1));
	if (nParamNum > 2)
		tmp.dwColor = TGetColor((char*)Lua_ValueToString(L,2));
	if (nParamNum > 3)
		tmp.nStateGraphics = (int)Lua_ValueToNumber(L,3);
	if (nParamNum > 4)
		tmp.dwLeftTime = (DWORD)Lua_ValueToNumber(L,4);

	Npc[Player[nPlayerIndex].m_nIndex].SetExpandRank(&tmp);
	return 1;	
}

int LuaRestoreExpandRank(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;

	Npc[Player[nPlayerIndex].m_nIndex].SetExpandRank(&Npc[Player[nPlayerIndex].m_nIndex].m_ExpandRank);
	return 1;	
}

int LuaGetEquipItemEx(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;

	if (Player[nPlayerIndex].m_dwEquipExpandTime - KSG_GetCurSec() > 0)
		Lua_PushNumber(L, Player[nPlayerIndex].m_dwEquipExpandTime);
	else
		Lua_PushNumber(L, 0);
	return 1;
}

int LuaSetEquipItemEx(Lua_State * L)
{
	int nValue = (int)Lua_ValueToNumber(L, 1);
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	
	Player[nPlayerIndex].SetEquipExpandTime((int)Lua_ValueToNumber(L, 1));
	return 0;
}

int LuaGetExpandBox(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;

	Lua_PushNumber(L, Player[nPlayerIndex].m_btRepositoryNum);
	return 1;
}

int LuaSetExpandBox(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	
	Player[nPlayerIndex].SetExpandBoxNum((int)Lua_ValueToNumber(L, 1));
	return 0;
}


int LuaSetObjPropState(Lua_State *L)
{
	int  nParamNum = ( int ) Lua_GetTopIndex(L);
	int nState  = 1;
	
	if (nParamNum >= 1)
	{
		nState = (int)Lua_ValueToNumber(L,1);
		nState = (nState == 0)?0 : 1;
	}
	
	int nIndex = 0;
	if ((nIndex = GetObjIndex(L)) < 0) 
		return 0;
	
	Object[nIndex].SetState(nState);
	return 0;
}


int	LuaGetServerName(Lua_State * L)
{
	char szServerName[100]  ;
	unsigned long   stServerNameLen = 100;

#ifndef __linux
	if (GetComputerName(szServerName, &stServerNameLen))
	{
		Lua_PushString(L, szServerName);
	}
	else
#else
	if (SOCKET_ERROR != gethostname(szServerName, sizeof(szServerName)))
	{
		Lua_PushString(L, szServerName);
	}
	else
#endif
	Lua_PushString(L, "");

	return 1;
}

//GetWharfCount(nDock)
int LuaGetDockCount(Lua_State * L)
{
	int nCount = 0;
	int nCurStation = 0;
	int nTotalCount = 0;
	int i  = 0;
	if (Lua_GetTopIndex(L) < 1 )  
	{
		goto DockCount;
	}
	
	nCurStation = (int)Lua_ValueToNumber(L,1);
	nTotalCount = g_DockPriceTabFile.GetHeight() - 1;
	
	for (i = 0; i < nTotalCount; i ++)
	{
		int nPrice = g_GetPriceToDock(nCurStation, i + 1);
		if (nPrice > 0) nCount ++;
	}
	
DockCount:
	Lua_PushNumber(L, nCount);
	return 1;
}

int LuaGetDockPrice(Lua_State * L)
{
	int nCurDock = (int)Lua_ValueToNumber(L,1);
	int nDesDock = (int)Lua_ValueToNumber(L,2);
	Lua_PushNumber(L, g_GetPriceToDock(nCurDock, nDesDock));
	return 1;
}

int LuaGetDock(Lua_State * L)
{
	int nCurDock = (int)Lua_ValueToNumber(L, 1);
	int nDock = (int)Lua_ValueToNumber(L, 2);
	int nCount = 0;
	int nTotalCount = g_DockPriceTabFile.GetHeight() - 1;
	int nGetDock = 0;
	
	for (int i = 0; i < nTotalCount; i ++)
	{
		int nPrice = g_GetPriceToDock(nCurDock, i + 1);
		if (nPrice > 0) 
		{
			nCount ++ ;
			if (nCount == nDock)
			{
				nGetDock = i + 1;
				break;
			}
		}
	}
	Lua_PushNumber(L, nGetDock);
	return 1;
}

int LuaGetDockName(Lua_State * L)
{
	int nDock  = (int)Lua_ValueToNumber(L, 1);
	char szName[100] ;
	
	if (nDock > g_DockPriceTabFile.GetHeight() - 1)
	{
		strcpy(szName, "未知码头");
		goto DockName;
	}
	
	g_DockTabFile.GetString(nDock + 1, "DESC", "未知码头", szName, 100);
	
DockName:
	Lua_PushString (L, szName);
	return 1;
}

int LuaGetDockPos(Lua_State * L)
{
	int nDock  = (int)Lua_ValueToNumber(L, 1);
	if (nDock > g_DockTabFile.GetHeight() - 1) 
	{
		printf("GetWharfPos Script Is Error!");
		return 0;
	}
	
	char szPos[100] ;
	int nCount = 0;
	int nRow = g_DockTabFile.FindColumn("COUNT");
	g_DockTabFile.GetInteger(nDock + 1, nRow,  0, &nCount);
	if (nCount <= 0) return 0;
	int nRandSect = g_Random(100) % nCount + 1;
	char szSectName[32];
	sprintf(szSectName, "SECT%d", nRandSect);
	char szValue[100];
	nRow = g_DockTabFile.FindColumn(szSectName);
	g_DockTabFile.GetString(nDock + 1, nRow, "0,0,0", szValue, 100);
	int nX, nY, nWorld;
    const char *pcszTemp = szValue;
    
    nWorld = KSG_StringGetInt(&pcszTemp, 0);
    KSG_StringSkipSymbol(&pcszTemp, ',');
    nX = KSG_StringGetInt(&pcszTemp, 0);
    KSG_StringSkipSymbol(&pcszTemp, ',');
    nY = KSG_StringGetInt(&pcszTemp, 0);
	//sscanf(szValue, "%d,%d,%d", &nWorld, &nX, &nY);
	
	Lua_PushNumber(L,nWorld);
	Lua_PushNumber(L,nX);
	Lua_PushNumber(L,nY);
	return 3;
}

int LuaGetWayPointFightState(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	int nFightState = 0;
	if (Lua_GetTopIndex(L) > 0 ) 
	{
		int nWayPointId = (int)Lua_ValueToNumber(L,1);
		g_WayPointTabFile.GetInteger(nWayPointId + 1, "FightState", 0,  &nFightState);
	}
	Lua_PushNumber(L, nFightState);
	return 1;
}

// SetMission(valueid, value)
int LuaSetMission(Lua_State * L)
{
	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex < 0) 
		return 0;
	
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount < 2) 
		return 0;
	
	int nValueId = (int)Lua_ValueToNumber(L, 1);
	char* szValue = (char*)Lua_ValueToString(L, 2);
	
	if (nValueId  < 0)
		return 0;

	SubWorld[nSubWorldIndex].m_MissionArray.SetMission(nValueId, szValue);
	return 0;
}

int LuaGetMissionValue(Lua_State * L)
{
	int nResultValue = 0;
	int nSubWorldIndex = -1;
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount < 1) 
		goto lab_getmissionvalue;
	
	nSubWorldIndex = GetSubWorldIndex(L);
	
	if (nSubWorldIndex >= 0) 
	{
		int  nValueId = (int)Lua_ValueToNumber(L, 1);
		if (nValueId >= 0)
			nResultValue = SubWorld[nSubWorldIndex].m_MissionArray.GetMissionValue(nValueId);
	}
	
lab_getmissionvalue:
	Lua_PushNumber(L, nResultValue);
	return 1;
}

int LuaGetMissionString(Lua_State * L)
{
	int nSubWorldIndex = -1;
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount < 1) 
		goto lab_getmissionstring;
	
	nSubWorldIndex = GetSubWorldIndex(L);
	
	if (nSubWorldIndex >= 0) 
	{
		int  nValueId = (int)Lua_ValueToNumber(L, 1);
		if (nValueId >= 0)
		{
			Lua_PushString(L, SubWorld[nSubWorldIndex].m_MissionArray.GetMissionString(nValueId));
			return 1;
		}
	}
	
lab_getmissionstring:
	Lua_PushNil(L);
	return 1;
}

// SetMissionValue(mapid/mapname, valueid, value)
int LuaSetGlobalMission(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount < 2) 
		return 0;
	
	int nValueId = (int)Lua_ValueToNumber(L, 1);
	char* szValue = (char*)Lua_ValueToString(L, 2);
	
	if (nValueId  < 0)
		return 0;
	g_GlobalMissionArray.SetMission(nValueId, szValue);
	return 0;
}

int LuaGetGlobalMissionValue(Lua_State * L)
{
	int nResultValue = 0;
	int nValueId = 0;
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount < 1) 
		goto lab_getglobalmissionvalue;
	nValueId = (int)Lua_ValueToNumber(L, 1);
	if (nValueId < 0)
		goto lab_getglobalmissionvalue;
	
	nResultValue = g_GlobalMissionArray.GetMissionValue(nValueId);
	
lab_getglobalmissionvalue:
	Lua_PushNumber(L, nResultValue);
	return 1;
}

int LuaGetGlobalMissionString(Lua_State * L)
{
	int nValueId = 0;
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount < 1) 
		goto lab_getglobalmissionstring;
	nValueId = (int)Lua_ValueToNumber(L, 1);
	if (nValueId < 0)
		goto lab_getglobalmissionstring;
	
	Lua_PushString(L, g_GlobalMissionArray.GetMissionString(nValueId));
	
lab_getglobalmissionstring:
	Lua_PushNil(L);
	return 1;
}

int LuaGetMissionName(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;
	
	int nMissionId = (int)Lua_ValueToNumber(L, 1);
	if (nMissionId < 0 )
		return 0;
	
	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex < 0) 
		return 0;

	KMission Mission;
	Mission.SetMissionId(nMissionId);
	KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);

	if (pMission)
	{
		Lua_PushString(L, pMission->GetMissionName());
		return 1;
	}
	return 0;
}

//StartMission(missionid)
int LuaInitMission(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;
	
	int nMissionId = (int)Lua_ValueToNumber(L, 1);
	if (nMissionId < 0 )
		return 0;
	
	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex < 0) 
		return 0;

	KMission Mission;
	Mission.SetMissionId(nMissionId);
	KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
	if (pMission)
	{
		_ASSERT(0);
		return 0;
	}
	
	pMission = SubWorld[nSubWorldIndex].m_MissionArray.Add();
	if (pMission)
	{
		pMission->m_MissionPlayer.Clear();
		pMission->m_MissionNpc.Clear();
		pMission->SetMissionId(nMissionId);
		if(Lua_GetTopIndex(L) > 4)
		{
			int nParam[MAX_GLBMISSION_PARAM];
			for(int i = 0; i < MAX_GLBMISSION_PARAM; i++)
				nParam[i] = (int)Lua_ValueToNumber(L, 4+i);
			pMission->SetMissionLadder((char*)Lua_ValueToString(L,2), (int)Lua_ValueToNumber(L,3), nParam);
		}
		char szScript[MAX_PATH];
		sprintf(szScript, MISSIONTASK_SCRIPTFILE, nMissionId);
		if (szScript[0])
		{
			KLuaScript * pScript =(KLuaScript*) g_GetScript(szScript);
			Lua_PushNumber(pScript->m_LuaState, nSubWorldIndex);
			pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);
			pScript->CallFunction("InitMission", 0, "d", nMissionId);
		}
	}
	return 0;
}

int LuaRunMission(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;
	
	int nMissionId = (int)Lua_ValueToNumber(L, 1);
	if (nMissionId < 0 )
		return 0;
	
	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex < 0) 
		return 0;

	KMission Mission;
	Mission.SetMissionId(nMissionId);
	KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);

	if (pMission)
	{
		char szScript[MAX_PATH];
		sprintf(szScript, MISSIONTASK_SCRIPTFILE, nMissionId);
		if (szScript[0])
		{
			KLuaScript * pScript =(KLuaScript*) g_GetScript(szScript);
			Lua_PushNumber(pScript->m_LuaState, nSubWorldIndex);
			pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);
			pScript->CallFunction("RunMission", 0, "d", nMissionId);
		}
	}
	return 0;
}

int LuaIsMission(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;
	
	int nMissionId = (int)Lua_ValueToNumber(L, 1);
	if (nMissionId < 0 )
		return 0;
	
	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex < 0) 
		return 0;

	KMission Mission;
	Mission.SetMissionId(nMissionId);
	KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
	if (pMission)
		Lua_PushNumber(L, 1);
	else
		Lua_PushNumber(L, 0);
	return 1;
}

int LuaGetMSLadder(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount < 2) 
		return 0;
	
	int nMissionId = (int)Lua_ValueToNumber(L, 1);
	if (nMissionId < 0 )
		return 0;
	
	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex < 0) 
		return 0;
	KMission Mission;
	Mission.SetMissionId(nMissionId);
	KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
	if (pMission)
	{
		int nOrdinal = (int)Lua_ValueToNumber(L,2);
		if(nOrdinal >= 0 && nOrdinal < MISSION_STATNUM)
		{
			Lua_PushString(L, pMission->m_MissionLadder[nOrdinal].Name);
			Lua_PushNumber(L, pMission->m_MissionLadder[nOrdinal].ucGroup);
			Lua_PushNumber(L, pMission->m_MissionLadder[nOrdinal].nParam[pMission->GetMissionLadderParam()]);
			return 3;
		}
		
	}
	return 0;
}

//CloseMission(missionId)
int LuaCloseMission(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;
	
	int nMissionId = (int)Lua_ValueToNumber(L, 1);
	if (nMissionId < 0 )
		return 0;
	
	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex < 0) 
		return 0;
	KMission StopMission;
	StopMission.SetMissionId(nMissionId);
	KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&StopMission);
	if (pMission)
	{
		char szScript[MAX_PATH];
		sprintf(szScript, MISSIONTASK_SCRIPTFILE, nMissionId);
		if (szScript[0])
		{
			KLuaScript * pScript =(KLuaScript*) g_GetScript(szScript);
			Lua_PushNumber(pScript->m_LuaState, nSubWorldIndex);
			pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);
			pScript->CallFunction("EndMission", 0, "d", nMissionId);
		}
		pMission->StopMission();
		SubWorld[nSubWorldIndex].m_MissionArray.Remove(pMission);
		
	}
	return 0;
}
//StopMissionTimer(missionid, timerid)
int LuaStopMissionTimer(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 2) 
		return 0;
	int nMissionId = (int)Lua_ValueToNumber(L, 1);
	int nTimerId = (int)Lua_ValueToNumber(L, 2);
	int nSubWorldIndex = GetSubWorldIndex(L);
	
	if (nMissionId < 0 || nTimerId < 0 ) 
		return 0;
	
	if (nSubWorldIndex >= 0) 
	{
		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			KTimerTaskFun StopTimer;
			StopTimer.SetTimer(1, nTimerId);
			KTimerTaskFun * pTimer = pMission->m_cTimerTaskSet.GetData(&StopTimer);
			if (pTimer)
			{
				pTimer->CloseTimer();
				pMission->m_cTimerTaskSet.Remove(pTimer);
			}
		}
		
	}
	
	return 0;
}

//StartMissionTimer(missionid, timerid, time)
int LuaStartMissionTimer(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 3) 
		return 0;
	int nMissionId = (int)Lua_ValueToNumber(L, 1);
	int nTimerId = (int)Lua_ValueToNumber(L, 2);
	int nTimeInterval = (int)Lua_ValueToNumber(L, 3);
	int nSubWorldIndex = GetSubWorldIndex(L);
	
	if (nMissionId < 0 || nTimerId < 0 || nTimeInterval < 0) 
		return 0;
	
	if (nSubWorldIndex >= 0) 
	{
		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			KTimerTaskFun * pTimer = pMission->m_cTimerTaskSet.Add();
			if (pTimer)
			{
				pTimer->SetTimer(nTimeInterval, nTimerId);
			}
		}
		
	}
	return 0;
}
//SetTempRev(worldid, x, y)
int LuaSetDeathRevivalPos(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	
	if (nPlayerIndex <= 0) 
		return 0;
	int nParamCount = Lua_GetTopIndex(L);
	
	PLAYER_REVIVAL_POS * pTempRev = Player[nPlayerIndex].GetDeathRevivalPos();
	
	if (nParamCount > 2)
	{
		pTempRev->m_nSubWorldID  = (int) Lua_ValueToNumber(L, 1);
		pTempRev->m_nMpsX = (int) Lua_ValueToNumber(L, 2);
		pTempRev->m_nMpsY = (int) Lua_ValueToNumber(L, 3); 
	}
	else if (nParamCount == 1)
	{
		pTempRev->m_nSubWorldID = SubWorld[Npc[Player[nPlayerIndex].m_nIndex].m_SubWorldIndex].m_SubWorldID;
		POINT Pos;
		int nRevId = (int) Lua_ValueToNumber(L, 1);
		g_SubWorldSet.GetRevivalPosFromId(pTempRev->m_nSubWorldID, nRevId, &Pos);
		pTempRev->m_ReviveID = nRevId;
		pTempRev->m_nMpsX = Pos.x;
		pTempRev->m_nMpsY = Pos.y;
	}
	else 
	{
		return 0;
	}
	
	return 0;
}


int LuaPlayerExecuteRevive(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	
	if (nPlayerIndex <= 0) 
		return 0;
	int nParamCount = Lua_GetTopIndex(L);
	
	int nReviveType = REMOTE_REVIVE_TYPE;
	
	if (nParamCount > 1)
		nReviveType  = (int) Lua_ValueToNumber(L, 1);

	Player[nPlayerIndex].Revive(nReviveType);
	return 0;
}

//AddMSPlayer(MissionId, PlayerIndex, groupid); / AddMSPlayer(MissionId, groupid)
int LuaAddMissionPlayer(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount < 2) 
		return 0;
	int nMissionId = 0;
	int nPlayerIndex = 0;
	int nGroupId = 0;
	if (nParamCount >=3)
	{
		nMissionId = (int)Lua_ValueToNumber(L,1);
		nPlayerIndex = (int )Lua_ValueToNumber(L,2);
		nGroupId = (int) Lua_ValueToNumber(L,3);
	}
	else
	{
		nMissionId = (int)Lua_ValueToNumber(L,1);
		nGroupId = (int) Lua_ValueToNumber(L,2);
		nPlayerIndex = GetPlayerIndex(L);
	}
	
	if (nMissionId < 0 || nPlayerIndex <= 0 || nGroupId <0)
		return 0;
	
	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0) 
	{
		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			int nPlayerDataIdx = pMission->AddPlayer(nPlayerIndex, Player[nPlayerIndex].m_dwID, nGroupId);
			Lua_PushNumber(L, nPlayerDataIdx);
			return 1;
		}
	}
	return 0;
}

int LuaAddMissionNpc(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount < 2) 
		return 0;

	int nMissionId = (int)Lua_ValueToNumber(L,1);
	int	nNpcIndex = (int )Lua_ValueToNumber(L,2);
	int nGroupId = 0;
	if (nParamCount >= 3)
		nGroupId = (int )Lua_ValueToNumber(L,3);
	
	if (nMissionId < 0 || nNpcIndex <= 0 || nGroupId <0)
		return 0;
	
	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0) 
	{
		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			int nNpcDataIdx = pMission->AddNpc(nNpcIndex, Npc[nNpcIndex].m_dwID, nGroupId);
			Lua_PushNumber(L, nNpcDataIdx);
			return 1;
		}
	}
	return 0;
}

int LuaSetMissionGroup(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	
	if (Lua_GetTopIndex(L) < 2)
		return 0;
		
	Npc[Player[nPlayerIndex].m_nIndex].m_nMissionGroup = (int)Lua_ValueToNumber(L,1);
	Player[nPlayerIndex].SendMSGroup();
	return 0;
}

int LuaRevivalAllNpc(Lua_State * L)
{
	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0) 
	{
		SubWorld[nSubWorldIndex].RevivalAllNpc();
	}
	return 0;
}

//RemoveMSPlayer(MissionId, PlayerIndex, groupid)
int LuaRemoveMissionPlayer(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount < 2) 
		return 0;
	int nMissionId = 0;
	int nPlayerIndex = 0;
	int nGroupId = 0;
	if (nParamCount >=2)
	{
		nMissionId = (int)Lua_ValueToNumber(L,1);
		nPlayerIndex = (int )Lua_ValueToNumber(L,2);
	}
	else
	{
		nMissionId = (int)Lua_ValueToNumber(L,1);
		nPlayerIndex = GetPlayerIndex(L);
	}
	
	if (nMissionId < 0 || nPlayerIndex <= 0)
		return 0;
	
	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0) 
	{
		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			pMission->RemovePlayer(nPlayerIndex, Player[nPlayerIndex].m_dwID);
		}
	}
	return 0;
}

int LuaRemoveMissionNpc(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount < 2) 
		return 0;
	int nMissionId = (int)Lua_ValueToNumber(L,1);
	int	nNpcIndex = (int )Lua_ValueToNumber(L,2);
	
	if (nMissionId < 0 || nNpcIndex <= 0)
		return 0;
	
	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0) 
	{
		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			pMission->RemoveNpc(nNpcIndex);
		}
	}
	return 0;
}

//MSMsg2Group(missionid, string , group)
int LuaMissionMsg2Group(Lua_State * L)
{
	int nMissionId = (int)Lua_ValueToNumber(L,1);
	char * strMsg = (char *)Lua_ValueToString(L, 2);
	int	nGroupId = (int) Lua_ValueToNumber(L, 3);
	
	if (nMissionId < 0 || !strMsg || nGroupId <0)
		return 0;
	
	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0) 
	{
		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			int nChannelID = -1;
			if(Lua_GetTopIndex(L) > 4)
				nChannelID = (int)Lua_ValueToNumber(L, 4);
			pMission->Msg2Group(strMsg, nGroupId, nChannelID);
		}
	}
	
	return 0;
}

//MSMsg2Group(missionid, string)
int LuaMissionMsg2All(Lua_State * L)
{
	int nMissionId = (int)Lua_ValueToNumber(L,1);
	char * strMsg = (char *)Lua_ValueToString(L, 2);
	
	if (nMissionId < 0 || !strMsg)
		return 0;
	
	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0) 
	{
		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			int nChannelID = -1;
			if(Lua_GetTopIndex(L) > 3)
				nChannelID = (int)Lua_ValueToNumber(L, 3);
			pMission->Msg2All(strMsg, nChannelID);
		}
	}
	
	return 0;
}

int LuaMissionPlayerCount(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	unsigned long ulCount = 0;
	int nMissionId = 0;
	int nGroupId = -1;
	int nSubWorldIndex = 0;
	if (nParamCount < 1) 
		goto lab_getmissionplayercount;
	
	nMissionId = (int)Lua_ValueToNumber(L,1);
	if (nParamCount > 1)
		nGroupId = (int)Lua_ValueToNumber(L,2);

	if (nMissionId < 0)
		goto lab_getmissionplayercount;
	
	nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0) 
	{
		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			if (nGroupId >= 0)
				ulCount = pMission->GetGroupPlayerCount(nGroupId);
			else
				ulCount = pMission->GetPlayerCount();
		}
	}
	
lab_getmissionplayercount:
	Lua_PushNumber(L, ulCount);
	return 1;
}

int LuaMissionNpcCount(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	unsigned long ulCount = 0;
	int nMissionId = 0;
	int nGroupId = -1;
	int nSubWorldIndex = 0;
	if (nParamCount < 1) 
		goto lab_getmissionnpccount;
	
	nMissionId = (int)Lua_ValueToNumber(L,1);
	if (nParamCount > 1)
		nGroupId = (int)Lua_ValueToNumber(L,2);

	if (nMissionId < 0)
		goto lab_getmissionnpccount;
	
	nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0) 
	{
		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			if (nGroupId >= 0)
				ulCount = pMission->GetGroupNpcCount(nGroupId);
			else
				ulCount = pMission->GetNpcCount();
		}
	}
	
lab_getmissionnpccount:
	Lua_PushNumber(L, ulCount);
	return 1;
}

int LuaSetPlayerDeathScript(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		return 0;
	char * szScript = (char *)Lua_ValueToString(L, 1);
	Player[nPlayerIndex].m_dwDeathScriptId = g_FileName2Id(szScript);
	return 0;
}

int LuaSetPlayerDamageScript(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		return 0;
	char * szScript = (char *)Lua_ValueToString(L, 1);
	Player[nPlayerIndex].m_dwDamageScriptId = g_FileName2Id(szScript);
	return 0;
}

int LuaNpcIndexToPlayerIndex(Lua_State * L)
{
	int nResult = 0;
	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex <=  0 || nNpcIndex >= MAX_NPC)
		goto lab_npcindextoplayerindex;
	
	if (Npc[nNpcIndex].m_Index > 0 && Npc[nNpcIndex].IsPlayer())
	{
		if (Npc[nNpcIndex].GetPlayerIdx() > 0)
			nResult = Npc[nNpcIndex].GetPlayerIdx();
	}
	
lab_npcindextoplayerindex:
	Lua_PushNumber(L, nResult);
	return 1;
	
}
//  
int LuaGetMissionPlayer_PlayerIndex(Lua_State * L)
{
	unsigned long nResult = 0;
	int nSubWorldIndex = 0;
	if (Lua_GetTopIndex(L) < 2) 
		goto lab_getmissionplayer_npcindex;
	
	nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0) 
	{
		int nMissionId = (int)Lua_ValueToNumber(L, 1);
		int nDataIndex = (int)Lua_ValueToNumber(L, 2);
		if (nMissionId < 0 || nDataIndex < 0)
			goto lab_getmissionplayer_npcindex;
		
		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			nResult = pMission->GetMissionPlayer_PlayerIndex(nDataIndex);
		}
	}
	
lab_getmissionplayer_npcindex:
	Lua_PushNumber(L,nResult);
	return 1;
}

int LuaGetMissionPlayer_DataIndex(Lua_State * L)
{
	unsigned long nResult = 0;
	int nSubWorldIndex = 0;
	if (Lua_GetTopIndex(L) < 2) 
		goto lab_getmissionplayer_dataindex;
	
	nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0) 
	{
		int nMissionId = (int)Lua_ValueToNumber(L, 1);
		int nPlayerIndex = (int)Lua_ValueToNumber(L, 2);
		if (nMissionId < 0 || nPlayerIndex < 0)
			goto lab_getmissionplayer_dataindex;
		
		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			nResult = pMission->GetMissionPlayer_DataIndex(Player[nPlayerIndex].Name, nPlayerIndex, Player[nPlayerIndex].m_dwID);
		}
	}
	
lab_getmissionplayer_dataindex:
	Lua_PushNumber(L,nResult);
	return 1;
}

//SetMPParam(missionid, nDidx, vid, v)
int LuaSetMissionPlayerParam(Lua_State * L)
{
	int nSubWorldIndex = 0;
	if (Lua_GetTopIndex(L) < 4) 
		return 0;
	
	nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0) 
	{
		int nMissionId = (int)Lua_ValueToNumber(L, 1);
		int nDataIndex = (int)Lua_ValueToNumber(L, 2);
		int nParamId =	 (int)Lua_ValueToNumber(L ,3);
		int nValue =	 (int )Lua_ValueToNumber(L, 4);

		if (nMissionId < 0 || nDataIndex < 0 || nParamId > MAX_MISSION_PARAM)
			return 0;

		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			//pMission->m_MissionPlayer.SetParam(nDataIndex, nParamId, nValue);
			pMission->SetPlayerParam(nDataIndex, nParamId, nValue);
		}
	}
	return 0;
}

int LuaGetMissionPlayerParam(Lua_State * L)
{
	int nResult = 0;
	int nSubWorldIndex = 0;
	if (Lua_GetTopIndex(L) < 3) 
		goto lab_getmissionplayerparam;
	
	nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0) 
	{
		int nMissionId = (int)Lua_ValueToNumber(L, 1);
		int nDataIndex = (int)Lua_ValueToNumber(L, 2);
		int nParamId =	 (int)Lua_ValueToNumber(L ,3);
		
		if (nMissionId < 0 || nDataIndex < 0 || nParamId > MAX_MISSION_PARAM)
			goto lab_getmissionplayerparam;
		
		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			nResult = pMission->m_MissionPlayer.GetParam(nDataIndex, nParamId);
		}
	}
lab_getmissionplayerparam:
	Lua_PushNumber(L, nResult);
	return 1;
}

int LuaGetPlayerMissionGroup(Lua_State * L)
{
	int nResult = 0;
	int nSubWorldIndex = 0;
	if (Lua_GetTopIndex(L) < 2) 
		goto lab_getmissionplayergroup;
	
	nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0) 
	{
		int nMissionId = (int)Lua_ValueToNumber(L, 1);
		int nNpcIndex = (int)Lua_ValueToNumber(L, 2);
		
		if (nMissionId < 0 || nNpcIndex < 0)
			goto lab_getmissionplayergroup;
		
		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			nResult = pMission->GetMissionPlayer_GroupId(nNpcIndex);
		}			
	}
lab_getmissionplayergroup:
	Lua_PushNumber(L ,nResult);
	return 1;
	
}

int LuaSetPlayerRevivalOptionWhenLogout(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		return 0;
	int nType = (int)Lua_ValueToNumber(L, 1);
	
	if (nType)
		Player[nPlayerIndex].SetLoginType(1);
	else
		Player[nPlayerIndex].SetLoginType(0);
	
	return 0;
}


int LuaSetPlayerPKValue(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;	
	int nPKValue = (int)Lua_ValueToNumber(L,1);
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		return 0;
	Player[nPlayerIndex].m_cPK.SetPKValue(nPKValue);
	return 0;
}

int LuaGetPlayerPKValue(Lua_State * L)
{
	int nPKValue = 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		goto lab_getplayerpkvalue;
	
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		goto lab_getplayerpkvalue;
	nPKValue = Player[nPlayerIndex].m_cPK.GetPKValue();
	
lab_getplayerpkvalue:
	Lua_PushNumber(L, nPKValue);
	return 1;
}

int LuaGetGameTime(Lua_State * L)
{
	Lua_PushNumber(L, g_SubWorldSet.GetGameTime());
	return 1;
}

int LuaGetPlayerLoginTime(Lua_State * L)
{
	int nResult = 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		goto lab_getplayerlogintime;
	
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		goto lab_getplayerlogintime;
	nResult = Player[nPlayerIndex].m_dwLoginTime;
	
lab_getplayerlogintime:
	Lua_PushNumber(L, nResult);
	return 1;
}

int LuaGetPlayerOnlineTime(Lua_State * L)
{
	int nResult = 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		goto lab_getplayeronlinetime;
	
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		goto lab_getplayeronlinetime;
	nResult = g_SubWorldSet.GetGameTime() - Player[nPlayerIndex].m_dwLoginTime;
	
lab_getplayeronlinetime:
	Lua_PushNumber(L, nResult);
	return 1;
}

int LuaOfflineLive(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;

	S2C_PLAYER_SYNC	sMsg;
	sMsg.ProtocolType = s2c_playersync;
	sMsg.m_wLength = sizeof(S2C_PLAYER_SYNC) - 1;
	sMsg.m_lpBuf = 0;
	sMsg.m_wMsgID = enumS2C_PLAYERSYNC_ID_EXIT;
	g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
	return 0;
}

int LuaSetValue(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 2) 
		return 0;	
	int nValueIndex = (int)Lua_ValueToNumber(L,1);
	int nValue = (int)Lua_ValueToNumber(L,2);
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		return 0;

	if (nValueIndex < 0 || nValueIndex >= MAX_STATTASK)
		return 0;
	Player[nPlayerIndex].m_cTask.SetSaveVal(nValueIndex,nValue);
	return 0;
}

int LuaAddValue(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 2) 
		return 0;	
	int nValueIndex = (int)Lua_ValueToNumber(L,1);
	int nValue = (int)Lua_ValueToNumber(L,2);
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		return 0;

	if (nValueIndex < 0 || nValueIndex >= MAX_STATTASK)
		return 0;
	Player[nPlayerIndex].m_cTask.SetSaveVal(nValueIndex, 
		Player[nPlayerIndex].m_cTask.GetSaveVal(nValueIndex)+nValue);
	return 0;
}


int LuaGetValue(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;	
	int nValue = 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		goto lab_getplayervalue;
	
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		goto lab_getplayervalue;
		nValue = Player[nPlayerIndex].m_cTask.GetSaveVal((int)Lua_ValueToNumber(L,1));
	
lab_getplayervalue:
	Lua_PushNumber(L, nValue);
	return 1;
}

int LuaSetPlayerReputeValue(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;	
	int nReputeValue = (int)Lua_ValueToNumber(L,1);
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		return 0;
	Player[nPlayerIndex].m_cTask.SetSaveVal(TASKVALUE_STATTASK_REPUTE, nReputeValue);
	return 0;
}

int LuaAddPlayerReputeValue(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;	
	int nReputeValue = (int)Lua_ValueToNumber(L,1);
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		return 0;
	Player[nPlayerIndex].m_cTask.SetSaveVal(TASKVALUE_STATTASK_REPUTE, 
		Player[nPlayerIndex].m_cTask.GetSaveVal(TASKVALUE_STATTASK_REPUTE)+nReputeValue);
	return 0;
}

int LuaGetPlayerReputeValue(Lua_State * L)
{
	int nReputeValue = 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		goto lab_getplayerreputevalue;
	
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		goto lab_getplayerreputevalue;
		nReputeValue = Player[nPlayerIndex].m_cTask.GetSaveVal(TASKVALUE_STATTASK_REPUTE);
	
lab_getplayerreputevalue:
	Lua_PushNumber(L, nReputeValue);
	return 1;
}

int LuaSetPlayerFuYuanValue(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;	
	int nFuYuanValue = (int)Lua_ValueToNumber(L,1);
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		return 0;
	Player[nPlayerIndex].m_cTask.SetSaveVal(TASKVALUE_STATTASK_FUYUAN,nFuYuanValue);
	return 0;
}


int LuaAddPlayerFuYuanValue(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;	
	int nFuYuanValue = (int)Lua_ValueToNumber(L,1);
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		return 0;
	Player[nPlayerIndex].m_cTask.SetSaveVal(TASKVALUE_STATTASK_FUYUAN, 
		Player[nPlayerIndex].m_cTask.GetSaveVal(TASKVALUE_STATTASK_FUYUAN)+nFuYuanValue);
	return 0;
}

int LuaGetPlayerFuYuanValue(Lua_State * L)
{
	int nFuYuanValue = 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		goto lab_getplayerfuyuanvalue;
	
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		goto lab_getplayerfuyuanvalue;
		nFuYuanValue = Player[nPlayerIndex].m_cTask.GetSaveVal(TASKVALUE_STATTASK_FUYUAN);
	
lab_getplayerfuyuanvalue:
	Lua_PushNumber(L, nFuYuanValue);
	return 1;
}

int LuaAddPlayerAccumValue(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;	
	int nAccumValue = (int)Lua_ValueToNumber(L,1);
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		return 0;
	Player[nPlayerIndex].m_cTask.SetSaveVal(TASKVALUE_STATTASK_ACCUM, 
		Player[nPlayerIndex].m_cTask.GetSaveVal(TASKVALUE_STATTASK_ACCUM)+nAccumValue);
	return 0;
}

int LuaGetPlayerAccumValue(Lua_State * L)
{
	int nAccumValue = 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		goto lab_getplayeraccumvalue;
	
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		goto lab_getplayeraccumvalue;
		nAccumValue = Player[nPlayerIndex].m_cTask.GetSaveVal(TASKVALUE_STATTASK_ACCUM);
	
lab_getplayeraccumvalue:
	Lua_PushNumber(L, nAccumValue);
	return 1;
}

int LuaAddPlayerHonorValue(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;	
	int nHonorValue = (int)Lua_ValueToNumber(L,1);
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		return 0;
	Player[nPlayerIndex].m_cTask.SetSaveVal(TASKVALUE_STATTASK_HONOR, 
		Player[nPlayerIndex].m_cTask.GetSaveVal(TASKVALUE_STATTASK_HONOR)+nHonorValue);
	return 0;
}

int LuaGetPlayerHonorValue(Lua_State * L)
{
	int nHonorValue = 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		goto lab_getplayerhonorvalue;
	
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		goto lab_getplayerhonorvalue;
		nHonorValue = Player[nPlayerIndex].m_cTask.GetSaveVal(TASKVALUE_STATTASK_HONOR);
	
lab_getplayerhonorvalue:
	Lua_PushNumber(L, nHonorValue);
	return 1;
}

int LuaAddPlayerRespectValue(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;	
	int nRespectValue = (int)Lua_ValueToNumber(L,1);
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		return 0;
	Player[nPlayerIndex].m_cTask.SetSaveVal(TASKVALUE_STATTASK_RESPECT, 
		Player[nPlayerIndex].m_cTask.GetSaveVal(TASKVALUE_STATTASK_RESPECT)+nRespectValue);
	return 0;
}

int LuaGetPlayerRespectValue(Lua_State * L)
{
	int nRespectValue = 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		goto lab_getplayerrespectvalue;
	
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		goto lab_getplayerrespectvalue;
		nRespectValue = Player[nPlayerIndex].m_cTask.GetSaveVal(TASKVALUE_STATTASK_RESPECT);
	
lab_getplayerrespectvalue:
	Lua_PushNumber(L, nRespectValue);
	return 1;
}

int LuaAddPlayerTranslifeValue(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;	
	int nTranslifeValue = (int)Lua_ValueToNumber(L,1);
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		return 0;
	Npc[Player[nPlayerIndex].m_nIndex].m_byTranslife += nTranslifeValue;
	return 0;
}

int LuaGetPlayerTranslifeValue(Lua_State * L)
{
	int nTranslifeValue = 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		goto lab_getplayertranslifevalue;
	
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		goto lab_getplayertranslifevalue;
		nTranslifeValue = Npc[Player[nPlayerIndex].m_nIndex].m_byTranslife;
	
lab_getplayertranslifevalue:
	Lua_PushNumber(L, nTranslifeValue);
	return 1;
}

int LuaAddPlayerViprankValue(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;	
	int nViprankValue = (int)Lua_ValueToNumber(L,1);
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		return 0;
	Player[nPlayerIndex].m_cTask.SetSaveVal(TASKVALUE_STATTASK_VIPRANK, 
		Player[nPlayerIndex].m_cTask.GetSaveVal(TASKVALUE_STATTASK_VIPRANK)+nViprankValue);
	return 0;
}

int LuaGetPlayerViprankValue(Lua_State * L)
{
	int nViprankValue = 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		goto lab_getplayerviprankvalue;
	
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		goto lab_getplayerviprankvalue;
		nViprankValue = Player[nPlayerIndex].m_cTask.GetSaveVal(TASKVALUE_STATTASK_VIPRANK);
	
lab_getplayerviprankvalue:
	Lua_PushNumber(L, nViprankValue);
	return 1;
}


int	LuaGetCurNpcIndex(Lua_State * L)
{
	int nNpcIndex = 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		goto lab_getcurnpcindex;
	
lab_getcurnpcindex:
	Lua_PushNumber(L, Player[nPlayerIndex].m_nIndex);
	return 1;
}



//showladder(count, ladderid1, ladderid2.....)
int LuaShowLadder(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount < 2) 
		return 0;
	int nLadderCount = (DWORD) Lua_ValueToNumber(L, 1);
	if (nLadderCount <= 0)
		return 0;
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	
	if (nLadderCount > nParamCount - 1 )
		nLadderCount = nParamCount - 1;
	BYTE Buffer[sizeof(LADDER_LIST) + 50 * sizeof(DWORD)];
	LADDER_LIST * pLadderList = (LADDER_LIST*)&Buffer;
	pLadderList->ProtocolType = s2c_ladderlist;
	pLadderList->nCount = nLadderCount;
	pLadderList->wSize = sizeof(LADDER_LIST) + nLadderCount * sizeof(DWORD) - 1;
	for (int i = 0; i < nLadderCount; i ++)
	{
		pLadderList->dwLadderID[i] = (DWORD)Lua_ValueToNumber(L, i + 2);
	}
	g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &Buffer, pLadderList->wSize + 1);
	return 0;
	
}

int LuaGetLadder(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount < 2) 
		return 0;
	
	void* pData = (void *)Ladder.GetTopTen((DWORD)Lua_ValueToNumber(L,1));
	if (pData)
	{
		TRoleList	StatData[10];
		memcpy(StatData, pData, sizeof(StatData));
		int nOrdinal = (int)Lua_ValueToNumber(L,2);
		if(nOrdinal >= 0 && nOrdinal < 10)
		{
			Lua_PushString(L, StatData[nOrdinal].Name);
			Lua_PushNumber(L, StatData[nOrdinal].nValue);
			Lua_PushNumber(L, StatData[nOrdinal].bySort);
			return 3;
		}
	}
	return 0;
}

int LuaSwearBrother(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;

	int nTeamID = Lua_ValueToNumber(L, 1);
	
	if (nTeamID >= MAX_TEAM || nTeamID < 0)
		return 0;
	
	KPlayerChat::STRINGLIST BrotherList;
	_ASSERT(g_Team[nTeamID].m_nCaptain > 0);

	std::string strCapName;
	strCapName = Npc[Player[g_Team[nTeamID].m_nCaptain].m_nIndex].Name;
	BrotherList.push_back(strCapName);

	for (int i  = 0; i < g_Team[nTeamID].m_nMemNum; i++)
	{
		int nPlayerIndex = g_Team[nTeamID].m_nMember[i];
		if ( nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
		{
			std::string strName;
			strName = 	Npc[Player[nPlayerIndex].m_nIndex].Name;
			BrotherList.push_back(strName);
		}
	}
	
	KPlayerChat::MakeBrother(BrotherList);
	return 0;
}

int LuaMakeEnemy(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;

	if (Lua_GetTopIndex(L) < 2) 
		return 0;
	char* szEnemy = (char*)Lua_ValueToString(L, 1);
	
	if (szEnemy[0])
	{
		KPlayerChat::MakeEnemy(Player[nPlayerIndex].Name, szEnemy);	
	}
	return 0;
}

int LuaMakeMate(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;

	if (Lua_GetTopIndex(L) < 2) 
		return 0;

	Player[nPlayerIndex].m_cTask.SetSaveVal(TASKVALUE_BASEDATA_MATENAME, (char*)Lua_ValueToString(L, 1));
	return 0;
}

int LuaDeleteMate(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	
	const char* szMateName = Player[nPlayerIndex].m_cTask.GetSaveStr(TASKVALUE_BASEDATA_MATENAME);
	if (szMateName[0])
		Player[nPlayerIndex].m_cTask.SetSaveVal(TASKVALUE_BASEDATA_MATENAME, "");

	return 0;
}

int LuaAddLeadExp(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	
	int nLeadExp = (int)Lua_ValueToNumber(L, 1);
	Player[nPlayerIndex].AddLeadExp(nLeadExp);
	return 0;
}

int LuaSetLeadLevel(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	
	int nLeadLevel = (int)Lua_ValueToNumber(L, 1);
	Player[nPlayerIndex].SetLeadLevel(nLeadLevel);
	return 0;
}

int LuaGetLeadLevel(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nLeadLevel = 0;
	if (nPlayerIndex <= 0)
		goto lab_getleadlevel;
	nLeadLevel = (int)Player[nPlayerIndex].m_nLeadLevel;
	
lab_getleadlevel:
	Lua_PushNumber(L, nLeadLevel);
	return 1;
}

int LuaAddMagicPoint(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;

	Player[nPlayerIndex].m_nSkillPoint += (int)Lua_ValueToNumber(L, 1);
	if (Player[nPlayerIndex].m_nSkillPoint < 0)
		Player[nPlayerIndex].m_nSkillPoint = 0;
	
	S2C_PLAYER_SYNC	sMsg;
	sMsg.ProtocolType = s2c_playersync;
	sMsg.m_wLength = sizeof(S2C_PLAYER_SYNC) - 1;
	sMsg.m_lpBuf = (LPVOID)Player[nPlayerIndex].m_nSkillPoint ;
	sMsg.m_wMsgID = enumS2C_PLAYERSYNC_ID_MAGICPOINT;
	g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
	return 0;
}

int LuaGetMagicPoint(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nSkillPoint = 0;
	if (nPlayerIndex <= 0)
		goto lab_getmagicpoint;
	
lab_getmagicpoint:
	Lua_PushNumber(L, Player[nPlayerIndex].m_nSkillPoint);
	return 1;
}

int LuaAddPropPoint(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nPropPoint = 0;
	if (nPlayerIndex <= 0)
		return 0;

	Player[nPlayerIndex].m_nAttributePoint += (int)Lua_ValueToNumber(L, 1);
	if (Player[nPlayerIndex].m_nAttributePoint < 0)
		Player[nPlayerIndex].m_nAttributePoint = 0;
	
	S2C_PLAYER_SYNC	sMsg;
	sMsg.ProtocolType = s2c_playersync;
	sMsg.m_wLength = sizeof(S2C_PLAYER_SYNC) - 1;
	sMsg.m_lpBuf = (LPVOID)Player[nPlayerIndex].m_nAttributePoint ;
	sMsg.m_wMsgID = enumS2C_PLAYERSYNC_ID_PROPPOINT;
	g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
	return 0;
}

int LuaSetExtPoint(Lua_State * L)
{
	int nResult = 0;
	int nExtPoint = 0;
	int nChange = 1;
	int nPlayerIndex = 0;
	if (Lua_GetTopIndex(L) < 1)
		goto lab_setextpoint;
	
	nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		goto lab_setextpoint;
	nExtPoint = Lua_ValueToNumber(L, 1);
	if (nExtPoint < 0)
		goto lab_setextpoint;
	Player[nPlayerIndex].SetExtPoint(nExtPoint, nChange);

lab_setextpoint:
	Lua_PushNumber(L, 0);
	return 1;
}

int LuaAddExtPoint(Lua_State * L)
{
	int nResult = 0;
	int nExtPoint = 0;
	int nChange = 1;
	int nPlayerIndex = 0;
	if (Lua_GetTopIndex(L) < 1)
		goto lab_setextpoint;
	
	nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		goto lab_setextpoint;
	nExtPoint = Lua_ValueToNumber(L, 1);
	if (nExtPoint < 0)
		goto lab_setextpoint;
	Player[nPlayerIndex].AddExtPoint(nExtPoint, nChange);

lab_setextpoint:
	Lua_PushNumber(L, 0);
	return 1;
}

int LuaPayExtPoint(Lua_State * L)
{
	int nResult = 0;
	int nPay = 0;
	int nPlayerIndex = 0;
	if (Lua_GetTopIndex(L) < 1)
		goto lab_payextpoint;
	
	nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		goto lab_payextpoint;
	nPay = Lua_ValueToNumber(L, 1);
	if (nPay < 0)
		goto lab_payextpoint;
	nResult = Player[nPlayerIndex].PayExtPoint(nPay);

lab_payextpoint:
	Lua_PushNumber(L, nResult);
	return 1;
}

//PayExtPoint
int LuaGetExtPoint(Lua_State * L)
{
	int nResult = 0;
	int nPlayerIndex = 0;
	nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		goto lab_getextpoint;
	
	nResult = Player[nPlayerIndex].GetExtPoint();
	
lab_getextpoint:
	Lua_PushNumber(L, nResult);
	return 1;
}

int LuaGetRestPropPoint(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nPropPoint = 0;
	if (nPlayerIndex <= 0)
		goto lab_getrestproppoint;
	nPropPoint = Player[nPlayerIndex].m_nAttributePoint;
	
lab_getrestproppoint:
	Lua_PushNumber(L, nPropPoint);
	return 1;
}

//Msg2GM(str, id)
int LuaMsgToGameMaster(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	if ( nParamCount < 1 ) 
		return 0;
	int nParamID = 0;
	const char * szMsg = Lua_ValueToString(L, 1) ;
	if (!szMsg) 
		return 0;

	if (nParamCount < 2)
	{
		nParamID = 0;
	}
	else
	{
		nParamID = (int) Lua_ValueToNumber(L, 2);
	}

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0) 
	{
		char szID[32];

		sprintf(szID, "%d", nParamID);
		//KPlayerChat::SendInfoToGM(Player[nPlayerIndex].AccountName, Npc[Player[nPlayerIndex].m_nIndex].Name, (char *) szMsg, strlen(szMsg) );
		KPlayerChat::SendInfoToGM(MESSAGE_SYSTEM_ANNOUCE_HEAD, szID, (char *) szMsg, strlen(szMsg) );
	}
	return 0;
}

//Msg2IP(IP, ID, str)
int LuaMsgToIP(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	if ( nParamCount < 3 ) 
		return 0;
	int nIP = 0;
	const char * szIP = Lua_ValueToString(L, 1) ;
	nIP = _a2ip(szIP);
	if (nIP == 0)
		return 0;

	int nID = (int) Lua_ValueToNumber(L, 2);
	const char * szMsg = Lua_ValueToString(L, 3) ;
	if (!szMsg) 
		return 0;

	int nParamID = 0;
	if (nParamCount < 4)
	{
		nParamID = 0;
	}
	else
	{
		nParamID = (int) Lua_ValueToNumber(L, 4);
	}
	char szID[32];
	sprintf(szID, "%d", nParamID);
	KPlayerChat::SendInfoToIP(nIP, nID, MESSAGE_SYSTEM_ANNOUCE_HEAD, szID, (char *) szMsg, strlen(szMsg) );
	return 0;
}

int LuaGetIP(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	char szDesMsg[200];
	szDesMsg[0] = 0;
	KPlayer * pPlayer = NULL;
	if (nPlayerIndex <= 0)
		goto lab_getplayerip;
	pPlayer = &Player[nPlayerIndex];
	strcpy(szDesMsg, g_pServer->GetClientInfo(pPlayer->m_nNetConnectIdx));
	
lab_getplayerip:
	Lua_PushString(L, szDesMsg);
	return 1;
}

int LuaSetDeathPunish(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		if ((int)Lua_ValueToNumber(L, 1))
			Npc[Player[nPlayerIndex].m_nIndex].m_nCurPKPunishState = enumDEATH_MODE_PKBATTLE_PUNISH;
		else
			Npc[Player[nPlayerIndex].m_nIndex].m_nCurPKPunishState = 0;
			
	}
	return 0;
}

int LuaSetReviveNow(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		Npc[Player[nPlayerIndex].m_nIndex].m_bReviveNow = (BOOL)Lua_ValueToNumber(L, 1);
	}
	return 0;
}


int LuaNpcChat(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 2)
		return 0;
	
	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	
	if (nNpcIndex > 0 || nNpcIndex < MAX_NPC)
	{
		char* szMsg = (char*)Lua_ValueToString(L,2);
		int nLen= TEncodeText(szMsg, strlen(szMsg));
		if(nLen >= MAX_MESSAGE_LENGTH)
			szMsg[nLen-1]='\0';
		NPC_CHAT_SYNC	command;
		command.ProtocolType = s2c_npcchat;
		command.ID = Npc[nNpcIndex].m_dwID;
		strcpy(command.szMsg, szMsg);
		command.nMsgLen=nLen;
		Npc[nNpcIndex].SendDataToNearRegion(&command, sizeof(NPC_CHAT_SYNC));
	}
	
	return 0;
}

int LuaHideNpc(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 2)
		return 0;
	
	int nNpcIndex  = 0;
	
	if (Lua_IsNumber(L,1))
	{
		nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	}
	else 
	{
		const char * szName = Lua_ValueToString(L, 1);
		int nSubWorldIndex = GetSubWorldIndex(L);
		if (nSubWorldIndex < 0)
			return 0;
		
		nNpcIndex = SubWorld[nSubWorldIndex].FindNpcFromName(szName);
	}
	
	if (nNpcIndex > 0 || nNpcIndex < MAX_NPC)
	{
		int nFrame = Lua_ValueToNumber(L, 2);
		if (nFrame <= 0)
			nFrame = 1;
		
		Npc[nNpcIndex].ExecuteRevive();
		Npc[nNpcIndex].m_Frames.nTotalFrame = nFrame;
		Npc[nNpcIndex].m_Frames.nCurrentFrame = 0;
	}
	
	return 0;
}

int LuaAddSkillState(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		return 0;
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 4) 
		return 0;
	int nSkillId = (int)Lua_ValueToNumber(L, 1);
	int nSkillLevel = (int)Lua_ValueToNumber(L, 2);
	int nIfMagic = (int)Lua_ValueToNumber(L, 3);
	int nTime	 = (int)Lua_ValueToNumber(L, 4);
	BOOL bOverLook = FALSE;
	if(nParamNum > 5)
		bOverLook = (BOOL)Lua_ValueToNumber(L, 5);

	if (nTime <= 0)
		nTime = -1;

	if (nIfMagic)
	{
		KSkill *pSkill = (KSkill*)g_SkillManager.GetSkill(nSkillId, nSkillLevel);
		pSkill->CastStateSkill(Player[nPlayerIndex].m_nIndex, 0, 0, nTime, bOverLook);
	}
	else
	{
		KMagicAttrib DamageMagicAttribs[MAX_MISSLE_DAMAGEATTRIB];
		memset(DamageMagicAttribs, 0, sizeof(DamageMagicAttribs));
		DamageMagicAttribs[0].nAttribType = magic_attackrating_v;
		DamageMagicAttribs[0].nValue[0] = 0;
		Npc[Player[nPlayerIndex].m_nIndex].SetStateSkillEffect(Player[nPlayerIndex].m_nIndex, nSkillId, nSkillLevel, DamageMagicAttribs, 1, nTime, bOverLook);
	}
	return 0;
}

int LuaAddNpcSkillState(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 5) 
		return 0;	

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);

	if (nNpcIndex < 0 || nNpcIndex >= MAX_NPC) return 0;

	int nSkillId = (int)Lua_ValueToNumber(L, 2);
	int nSkillLevel = (int)Lua_ValueToNumber(L, 3);
	int nIfMagic = (int)Lua_ValueToNumber(L, 4);
	int nTime	 = (int)Lua_ValueToNumber(L, 5);

	if (nIfMagic)
	{
		KSkill *pSkill = (KSkill*)g_SkillManager.GetSkill(nSkillId, nSkillLevel);
		pSkill->CastStateSkill(nNpcIndex, 0, 0, nTime, TRUE);
	}
	else
	{
		KMagicAttrib DamageMagicAttribs[MAX_MISSLE_DAMAGEATTRIB];
		memset(DamageMagicAttribs, 0, sizeof(DamageMagicAttribs));
		DamageMagicAttribs[0].nAttribType = magic_attackrating_v;
		DamageMagicAttribs[0].nValue[0] = 0;
		Npc[nNpcIndex].SetStateSkillEffect(nNpcIndex, nSkillId, nSkillLevel, DamageMagicAttribs, 1, nTime, TRUE);
	}
	return 0;
}


int LuaIgnoreState(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		return 0;
	Npc[Player[nPlayerIndex].m_nIndex].IgnoreState(FALSE);
	return 0;
}

int LuaCastSkill(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	
	if (Lua_GetTopIndex(L) < 3)
		return 0;
	
	int nSkillId = (int)Lua_ValueToNumber(L, 1);
	int nSkillLevel = (int)Lua_ValueToNumber(L, 2);
	if (nSkillId < MAX_SKILL && nSkillLevel < MAX_SKILLLEVEL)
		Npc[Player[nPlayerIndex].m_nIndex].Cast(nSkillId, nSkillLevel);
	return 0;
}

int LuaNpcCastSkill(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 3)
		return 0;
	
	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex > 0 || nNpcIndex < MAX_NPC)
	{
		int nSkillId = (int)Lua_ValueToNumber(L, 2);
		int nSkillLevel = (int)Lua_ValueToNumber(L, 3);
		if (nSkillId < MAX_SKILL && nSkillLevel < MAX_SKILLLEVEL)
			Npc[nNpcIndex].Cast(nSkillId, nSkillLevel);
	}
	return 0;
}

int LuaSetMask(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;

	int nMaskType = (int)Lua_ValueToNumber(L,1);
	if(nMaskType > 0)
	{
		Player[nPlayerIndex].m_ItemList.SetMaskLock(TRUE);
		Npc[Player[nPlayerIndex].m_nIndex].m_MaskType = nMaskType;
	}
	else
	{
		Player[nPlayerIndex].m_ItemList.SetMaskLock(FALSE);
		int nMaskIdx = Player[nPlayerIndex].m_ItemList.GetEquipment(itempart_mask);
		if (nMaskIdx > 0)
			g_MaskChangeRes.GetInteger(Item[nMaskIdx].GetParticular() + 2, 2, 0, &Npc[Player[nPlayerIndex].m_nIndex].m_MaskType);
		else
			Npc[Player[nPlayerIndex].m_nIndex].m_MaskType = 0;
	}
	return 0;
}

int LuaAddTrap(Lua_State * L)
{
	int nSubWorldIndex = g_SubWorldSet.SearchWorld((int)Lua_ValueToNumber(L, 1));

	if (nSubWorldIndex == -1)
		return 0;

	int nRegion, nMapX, nMapY, nOffX, nOffY;

	SubWorld[nSubWorldIndex].Mps2Map((int)Lua_ValueToNumber(L, 2), (int)Lua_ValueToNumber(L, 3), &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
	if (nRegion == -1)
		return 0;

	DWORD dwTrapId = (DWORD)g_FileName2Id((char*)Lua_ValueToString(L, 4));
	SubWorld[nSubWorldIndex].m_Region[nRegion].SetTrap(dwTrapId, nMapX, nMapY);
	return 0;
}

int LuaAddObj(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 5)
	{
		Lua_PushNumber(L,0);
		return 1;
	}

	int nObjID = (int)Lua_ValueToNumber(L,1);
	if (nObjID <= 0 || nObjID >= ObjSet.m_cTabFile.GetHeight())
	{
		Lua_PushNumber(L,0);
		return 1;
	}
	int nSubWorldIndex	= g_SubWorldSet.SearchWorld((int)Lua_ValueToNumber(L,2));

	if (nSubWorldIndex == -1)
		return 0;

	KMapPos	Pos;

	Pos.nSubWorld = nSubWorldIndex;
	SubWorld[nSubWorldIndex].Mps2Map((int)Lua_ValueToNumber(L,3),(int)Lua_ValueToNumber(L,4), 
	&Pos.nRegion, &Pos.nMapX, &Pos.nMapY, 
	&Pos.nOffX, &Pos.nOffY);
	KObjItemInfo	sInfo;
	sInfo.m_nItemID = 0;
	sInfo.m_nItemWidth = 0;
	sInfo.m_nItemHeight = 0;
	sInfo.m_nMoneyNum = 0;
	sInfo.m_szName[0] = 0;
	sInfo.m_nColorID = 0;
	sInfo.m_nGenre = 0;
	sInfo.m_nDetailType = 0;
	sInfo.m_nMovieFlag = 1;
	sInfo.m_nSoundFlag = 1;
	sInfo.m_bOverLook = 0;
	int nObj = ObjSet.Add(nObjID, Pos, sInfo);
	if (nObj <= 0) return 0;

	Object[nObj].SetScriptFile((char *)Lua_ValueToString(L,5));
	if (nParamNum > 5)
		Object[nObj].SetImageDir((int)Lua_ValueToNumber(L,6));
	if (nParamNum > 6)
		Object[nObj].SetState((int)Lua_ValueToNumber(L,7));

	Lua_PushNumber(L, nObj);
    return 1;
}

int LuaDelObj(Lua_State * L)
{
	if (Lua_GetTopIndex(L) <= 0 ) return 0 ;

	int nObjIndex = 0;
	if(Lua_GetTopIndex(L) >= 3)
	{
		int nSubWorldIndex	= g_SubWorldSet.SearchWorld((int)Lua_ValueToNumber(L,1));

		if (nSubWorldIndex == -1)
			return 0;

		KMapPos	Pos;

		Pos.nSubWorld = nSubWorldIndex;
		SubWorld[nSubWorldIndex].Mps2Map((int)Lua_ValueToNumber(L,2),(int)Lua_ValueToNumber(L,3), 
		&Pos.nRegion, &Pos.nMapX, &Pos.nMapY, 
		&Pos.nOffX, &Pos.nOffY);
		nObjIndex = ObjSet.SearchObjAt(Pos);
	}
	else
		nObjIndex = (int)Lua_ValueToNumber(L, 1);

	if (nObjIndex > 0)
	{
		Object[nObjIndex].SyncRemove(FALSE);
		if (Object[nObjIndex].m_nRegionIdx >= 0)
			SubWorld[Object[nObjIndex].m_nSubWorldID].m_Region[Object[nObjIndex].m_nRegionIdx].RemoveObj(nObjIndex);
		ObjSet.Remove(nObjIndex);
	}
	return 0;
}

int LuaSetObjScript(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
	{
		Lua_PushNumber(L,0);
		return 1;
	}
	int nObjId;
	char* szScriptName;
	nObjId		= (int)Lua_ValueToNumber(L,1);
	szScriptName	= (char *)Lua_ValueToString(L,2);

	if (nObjId > 0)
	{
		if (szScriptName)
		{
			Object[nObjId].SetScriptFile(szScriptName);
			Object[nObjId].m_dwScriptID = g_FileName2Id((char *)Lua_ValueToString(L,2));
		}
	}
    return 0;
}

int LuaSetObjValue(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
	{
		Lua_PushNumber(L,0);
		return 1;
	}
	int nId, nValue;
	nId		= (int)Lua_ValueToNumber(L,1);
	nValue	= (int)Lua_ValueToNumber(L,2);

	if (nId > 0)
	{
		Object[nId].SetObjValue(nValue);
	}
    return 0;
}

int LuaGetObjValue(Lua_State * L)
{	
	if (Lua_GetTopIndex(L) <= 0 ) return 0 ;
	int nObjIndex = (int)Lua_ValueToNumber(L, 1);
	if (nObjIndex > 0)
	{
		Lua_PushNumber(L, Object[nObjIndex].GetObjValue());
	}
	return 1;
}

int LuaSetObjPickExecute(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
	{
		Lua_PushNumber(L,0);
		return 1;
	}
	int nId, nValue;
	nId		= (int)Lua_ValueToNumber(L,1);
	nValue	= (int)Lua_ValueToNumber(L,2);

	if (nId > 0)
	{
		Object[nId].SetObjPickExecute(nValue > 0);
	}
    return 0;
}

int LuaOpenGive(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;

	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 3)
		return 0;

	Player[nPlayerIndex].m_szTaskExcuteFun[0] = 0;

	PLAYER_GIVE PGive;
	PGive.ProtocolType = (BYTE)s2c_opengive;
	strcpy(PGive.m_szName, (char *)Lua_ValueToString(L,1));
	strcpy(PGive.m_szInitString, (char *)Lua_ValueToString(L,2));
	strcpy(Player[nPlayerIndex].m_szTaskExcuteFun, (char *)Lua_ValueToString(L, 3));
	Player[nPlayerIndex].m_dwTaskExcuteScriptId = Npc[Player[nPlayerIndex].m_nIndex].m_ActionScriptID;
	g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &PGive, sizeof(PLAYER_GIVE));

	return 0;
}

int LuaRemoveRoom(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L,0);
		return 1;
	}
	if (Lua_GetTopIndex(L) < 1)
	{
		return 0;
	}
	int nRoom = (int)Lua_ValueToNumber(L, 1);
	if (nRoom < room_equipment || nRoom > room_num)
		return 0;
	Player[nPlayerIndex].m_ItemList.RemoveRoom(nRoom);
	return 0;
}

int LuaCalcFreeItemCellCount(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L,0);
		return 1;
	}
	int nWidth = 1;
	int nHeight = 1;
	int nRoom = room_equipment;
	if (Lua_GetTopIndex(L) > 2)
	{
		nWidth = (int)Lua_ValueToNumber(L, 1);
		nHeight = (int)Lua_ValueToNumber(L, 2);
	}
	if (Lua_GetTopIndex(L) > 3)
	{
		nRoom = (int)Lua_ValueToNumber(L, 3);
	}
	if (nRoom < room_equipment || nRoom > room_num)
		return 0;
	int nCount = Player[nPlayerIndex].m_ItemList.CalcFreeItemCellCount(nWidth, nHeight, nRoom);
	Lua_PushNumber(L, nCount);
	return 1;
}

int LuaIsNumber(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L,0);
		return 1;
	}
	if (Lua_IsNumber(L, 1))
	{	
		Lua_PushNumber(L,1);
		return 1;
	}
	else
	{	
		Lua_PushNumber(L,0);
		return 1;
	}
	return 1;
}

int LuaIsTable(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L,0);
		return 1;
	}
	if (Lua_IsTable(L, 1))
	{	
		Lua_PushNumber(L,1);
		return 1;
	}
	else
	{	
		Lua_PushNumber(L,0);
		return 1;
	}
	return 1;
}

int LuaOpenURL(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L,0);
		return 1;
	}
	if (Lua_IsString(L, 1))
	{	
		ShellExecute(NULL,"open",(char*)Lua_ValueToString(L,1),NULL,NULL,SW_SHOWNORMAL);
		return 0;
	}
	return 0;
}

int LuaOpenExplore(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L,0);
		return 1;
	}
	if (Lua_IsString(L, 1))
	{	
		ShellExecute(NULL,"explore",(char*)Lua_ValueToString(L,1),NULL,NULL,SW_SHOWNORMAL);
		return 0;
	}
	return 0;
}

int LuaModifyAttrib(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L,0);
		return 1;
	}
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 5)
		return 0;
	KNpc *pNPC = &Npc[Player[nPlayerIndex].m_nIndex];
	KMagicAttrib pBaseAttrib;
	pBaseAttrib.nAttribType = (int)Lua_ValueToNumber(L, 1);
	pBaseAttrib.nValue[0] = (int)Lua_ValueToNumber(L, 2);
	pBaseAttrib.nValue[1] = (int)Lua_ValueToNumber(L, 3);
	pBaseAttrib.nValue[2] = (int)Lua_ValueToNumber(L, 4);
	KMagicAttrib *pAttrib = &pBaseAttrib;
	if (-1 != pAttrib->nAttribType)
	{
		pNPC->ModifyAttrib(pNPC->m_Index, (void *)pAttrib);
	}
	return 0;
}


int LuaRANDOM(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);

	if (nParamNum < 1)
		return 0;
	int nResult = 0;
	if (nParamNum > 1)
		nResult = GetRandomNumber((int)Lua_ValueToNumber(L, 1), (int)Lua_ValueToNumber(L, 2));
	else
		nResult = GetRandomNumber(0, (int)Lua_ValueToNumber(L, 1));
	Lua_PushNumber(L, nResult);
	return 1;
}

int LuaRANDOMC(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);

	if (nParamNum < 2)
		return 0;
	
	srand( (unsigned)time( NULL ) );
	if (Lua_IsTable(L,1))
	{
		Lua_PushNumber(L, (int)Lua_ValueToNumber(L,2));
		Lua_RawGet(L, 1);
		Lua_PushNumber(L, (int)Lua_ValueToNumber(L, Lua_GetTopIndex(L)));
	}
	else if (Lua_IsTable(L,2))
	{
		int nResult = ::GetRandomNumber(1, (int)Lua_ValueToNumber(L,1));
		Lua_PushNumber(L, nResult);
		Lua_RawGet(L, 2);
		Lua_PushNumber(L, (int)Lua_ValueToNumber(L, (int)Lua_ValueToNumber(L, Lua_GetTopIndex(L))));
	}
	else
	{
		int nResult = ::GetRandomNumber(1, nParamNum);
		Lua_PushNumber(L, (int)Lua_ValueToNumber(L, nResult));
	}
	return 1;
}

int LuaIsMyItem(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		if (Lua_GetTopIndex(L) > 1)
		{
			int nGameIdx = (int)Lua_ValueToNumber(L, 1);
			if (Player[nPlayerIndex].m_ItemList.SearchID(Item[nGameIdx].GetID()))
				Lua_PushNumber(L, 1);
			else
				Lua_PushNumber(L, 0);
		}
	}
	return 1;
}

int LuaInput(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0;

	Player[nPlayerIndex].m_dwTaskExcuteScriptId = Npc[Player[nPlayerIndex].m_nIndex].m_ActionScriptID;
	strcpy(Player[nPlayerIndex].m_szTaskExcuteFun, (char *)Lua_ValueToString(L,1));
	int max = 0;
	if(nParamNum > 2)
		max=(int)Lua_ValueToNumber(L,2);

	S2C_PLAYER_SYNC	sMsg;
	sMsg.ProtocolType = s2c_playersync;
	sMsg.m_wLength = sizeof(S2C_PLAYER_SYNC) - 1;
	sMsg.m_wMsgID = enumS2C_PLAYERSYNC_ID_INPUT;
	sMsg.m_lpBuf = (LPVOID)max;
	g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		
	return 0;
}

int LuaGetInput(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;

	Lua_PushString(L, Player[nPlayerIndex].m_szLastInput);
	return 1;
}

int LuaOpenEnchase(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0)
		return 0;

	S2C_PLAYER_SYNC	sMsg;
	sMsg.ProtocolType = s2c_playersync;
	sMsg.m_wLength = sizeof(S2C_PLAYER_SYNC) - 1;
	sMsg.m_wMsgID = enumS2C_PLAYERSYNC_ID_ENCHASE;
	sMsg.m_lpBuf = 0;
	g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
	return 0;
}

int LuaEnchase(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0)
		return 0;
	Player[nPlayerIndex].Enchase(2,(int)Lua_ValueToNumber(L,1),(int)Lua_ValueToNumber(L,2),(int)Lua_ValueToNumber(L,3));
	return 0;	
}


int LuaCheckRoom(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nIndex, nWidth = 0, nHeight = 0;;

	if (Lua_GetTopIndex(L) >= 2)
	{
		nIndex = (int)Lua_ValueToNumber(L,1);
		nWidth = Item[nIndex].GetWidth();
		nHeight = Item[nIndex].GetHeight();
	}
	if (Lua_GetTopIndex(L) >= 3)
	{
		nWidth = (int)Lua_ValueToNumber(L,1);
		nHeight = (int)Lua_ValueToNumber(L,2);
	}
	if (nPlayerIndex <= 0)
		return 0;

	if (nWidth <= 0 || nHeight <= 0)
		return 0;

	POINT ItemSize;
	ItemSize.x = nWidth;
	ItemSize.y = nHeight;
	ItemPos	sItemPos;
	if ( FALSE == Player[nPlayerIndex].m_ItemList.SearchPosition(ItemSize, &sItemPos) )	
		Lua_PushNumber(L, 0);
	else
		Lua_PushNumber(L, 1);

	return 1;
}

int LuaSendMessageInfo(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1) 
		return 0;
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) return 0;
	
	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bUIId = UI_MSGINFO;
	UiInfo.m_bOptionNum = 1;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	UiInfo.m_nParam = SMT_NORMAL;
	UiInfo.m_nParam1 = SMCT_NONE;
	if(nParamNum > 1)
		UiInfo.m_nParam = (int)Lua_ValueToNumber(L,2);
	if(nParamNum > 2)
		UiInfo.m_nParam1 = (int)Lua_ValueToNumber(L,3);
	
	int nMsgId = 0;
	
	if (Lua_IsNumber(L,1))
	{
		nMsgId = (int)Lua_ValueToNumber(L,1);
		*((int *)(UiInfo.m_pContent)) = nMsgId;
		UiInfo.m_bParam1 = 1;
		UiInfo.m_nBufferLen = sizeof(int);
	}
	else 
	{
		
		g_StrCpyLen(UiInfo.m_pContent, Lua_ValueToString(L,1), 256);
		UiInfo.m_nBufferLen = strlen(((char *)UiInfo.m_pContent));
		UiInfo.m_bParam1 = 0;
	}
	
#ifndef _SERVER
	UiInfo.m_bParam2 = 0;
#else
	UiInfo.m_bParam2 = 1;
#endif
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
	return 0;
}

int LuaRemoveServerItem(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	
	int nIdx = (int)Lua_ValueToNumber(L,1);
	
	if (!nIdx)
		return 0;
	
	ItemSet.Remove(nIdx);
	return 0;
}

int LuaRemoveItemIdx(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 3)
	{
		Lua_PushNumber(L, 0);
	}
	else
	{
		if(Player[nPlayerIndex].m_ItemList.RemoveItem((int)Lua_ValueToNumber(L,1),(int)Lua_ValueToNumber(L,2)))
			Lua_PushNumber(L,1);
	}
	return 1;
}

int LuaLockMoveItem(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1) return 0;

	LockMoveItem LockMove;
	LockMove.bLock = (BOOL)Lua_ValueToNumber(L,1);
	LockMove.nPlace = 0;
	Player[nPlayerIndex].SetLockMove(&LockMove);
	return 0;
}

int LuaSyncItem(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nItemIdx = (int)Lua_ValueToNumber(L,1);


	if (nItemIdx <= 0 || nItemIdx > MAX_ITEM)
		return 0;

	Player[nPlayerIndex].m_ItemList.SyncItem(nItemIdx);
	return 0;
}

int LuaSetTempItem(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2) return 0;
	
	int nIdx = (int)Lua_ValueToNumber(L, 1);

	if (nIdx > 0)
	{
		Item[nIdx].SetTemp((BOOL)Lua_ValueToNumber(L, 2));
		Player[nPlayerIndex].m_ItemList.SyncItem(nIdx);
		Lua_PushNumber(L,nIdx);
		return 1;
	}
	return 0;
}

int LuaSetLevelItem(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2) return 0;
	
	int nIdx = (int)Lua_ValueToNumber(L, 1);

	if (nIdx > 0)
	{
		Item[nIdx].SetLevel((int)Lua_ValueToNumber(L, 2));
		Player[nPlayerIndex].m_ItemList.SyncItem(nIdx);
		Lua_PushNumber(L,nIdx);
		return 1;
	}
	return 0;
}

int LuaSetSeriesItem(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2) return 0;
	
	int nIdx = (int)Lua_ValueToNumber(L, 1);

	if (nIdx > 0)
	{
		Item[nIdx].SetSeries((int)Lua_ValueToNumber(L, 2));
		Player[nPlayerIndex].m_ItemList.SyncItem(nIdx);
		Lua_PushNumber(L,nIdx);
		return 1;
	}
	return 0;
}

int LuaSetParamItem(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2) return 0;
	
	int nIdx = (int)Lua_ValueToNumber(L, 1);

	if (nIdx > 0)
	{
		Item[nIdx].SetParam((int)Lua_ValueToNumber(L, 2));
		Player[nPlayerIndex].m_ItemList.SyncItem(nIdx);
		Lua_PushNumber(L,nIdx);
		return 1;
	}
	return 0;
}

int LuaSetFortuneItem(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2) return 0;
	
	int nIdx = (int)Lua_ValueToNumber(L, 1);

	if (nIdx > 0)
	{
		Item[nIdx].SetFortune((int)Lua_ValueToNumber(L, 2));
		Player[nPlayerIndex].m_ItemList.SyncItem(nIdx);
		Lua_PushNumber(L,nIdx);
		return 1;
	}
	return 0;
}

int LuaSetTimeItem(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 3) return 0;
	
	int nIdx = (int)Lua_ValueToNumber(L, 1);

	if (nIdx > 0)
	{
		Item[nIdx].SetExpireTime(KSG_GetCurSec()+(int)Lua_ValueToNumber(L, 2));
		Player[nPlayerIndex].m_ItemList.SyncItem(nIdx);
		Lua_PushNumber(L,nIdx);
		return 1;
	}
	return 0;
}

int LuaAddTimeItem(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 3) return 0;
	
	int nIdx = (int)Lua_ValueToNumber(L, 1);

	if (nIdx > 0)
	{
		if (Item[nIdx].GetExpireTime())
			Item[nIdx].SetExpireTime(Item[nIdx].GetExpireTime() + (int)Lua_ValueToNumber(L, 2));
		else
			Item[nIdx].SetExpireTime(KSG_GetCurSec()+(int)Lua_ValueToNumber(L, 2));

		Player[nPlayerIndex].m_ItemList.SyncItem(nIdx);
		Lua_PushNumber(L,nIdx);
		return 1;
	}
	return 0;
}


int LuaLockItem(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2) return 0;
	
	if (nParamNum > 4)
	{
		int nIdx = (int)Lua_ValueToNumber(L, 1);
		if (nIdx > 0)
		{
			Item[nIdx].SetLockTrade((BOOL)Lua_ValueToNumber(L,2));
			Item[nIdx].SetLockSell((BOOL)Lua_ValueToNumber(L,3));
			Item[nIdx].SetLockDrop((BOOL)Lua_ValueToNumber(L,4));
			Player[nPlayerIndex].m_ItemList.SyncItem(nIdx);
			Lua_PushNumber(L,nIdx);
			return 1;
		}
	}
	else
	{
		int nIdx = (int)Lua_ValueToNumber(L, 1);
		int nLock = LOCK_STATE_FOREVER;
		if (nIdx > 0)
		{
			if (nParamNum > 2)
				nLock = (int)Lua_ValueToNumber(L, 2);
			Item[nIdx].LockItem(nLock);
			Player[nPlayerIndex].m_ItemList.SyncItem(nIdx);
			Lua_PushNumber(L,nIdx);
			return 1;
		}
	}
	return 0;
}


int LuaSetStackItem(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2) return 0;
	
	int nIdx = (int)Lua_ValueToNumber(L, 1);

	if (nIdx > 0)
	{
		Item[nIdx].SetStackNum((int)Lua_ValueToNumber(L, 2));
		Player[nPlayerIndex].m_ItemList.SyncItem(nIdx);
		Lua_PushNumber(L,nIdx);
		return 1;
	}
	return 0;
}

int LuaSetMantleItem(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2) return 0;
	
	int nIdx = (int)Lua_ValueToNumber(L, 1);

	if (nIdx > 0)
	{
		if (Item[nIdx].GetGenre() == item_equip && 
			Item[nIdx].GetDetailType() == equip_armor)
		{
			Item[nIdx].SetMantle((int)Lua_ValueToNumber(L, 2));
			Player[nPlayerIndex].m_ItemList.SyncItem(nIdx);

			if (Player[nPlayerIndex].m_ItemList.GetEquipment(itempart_mantle)<=0)
				Npc[Player[nPlayerIndex].m_nIndex].m_MantleType = Item[nIdx].GetMantle();
		}
		Lua_PushNumber(L,nIdx);
		return 1;
	}
	return 0;
}

int LuaSetFlashItem(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2) return 0;
	
	int nIdx = (int)Lua_ValueToNumber(L, 1);

	if (nIdx > 0)
	{
		Item[nIdx].SetFlash(TGetColor((char*)Lua_ValueToString(L, 2)));
		Player[nPlayerIndex].m_ItemList.SyncItem(nIdx);
		Lua_PushNumber(L,nIdx);
		return 1;
	}
	return 0;
}

int LuaGetFreeObjPos(Lua_State * L)
{
	int nSubWorldIndex, nNpcIndex;

	POINT	ptLocal;
	if (Lua_GetTopIndex(L) > 2) 
	{
		nNpcIndex = (int)Lua_ValueToNumber(L,1);
		ptLocal.x = (int)Lua_ValueToNumber(L,2);
		ptLocal.y = (int)Lua_ValueToNumber(L,3);
	}
	else if (Lua_GetTopIndex(L) > 1) 
	{
		int nPlayerIndex = GetPlayerIndex(L);
		if (nPlayerIndex <= 0) return 0;
		nNpcIndex = Player[nPlayerIndex].m_nIndex;
		nSubWorldIndex = Npc[nNpcIndex].m_SubWorldIndex;
		ptLocal.x = (int)Lua_ValueToNumber(L,1);
		ptLocal.y = (int)Lua_ValueToNumber(L,2);
	}
	else
		return 0;

	SubWorld[nSubWorldIndex].GetFreeObjPos(ptLocal);
	Lua_PushNumber(L, (int)(ptLocal.x/32));
	Lua_PushNumber(L, (int)(ptLocal.y/32));
	return 2;
}

int LuaOpenRankData(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	
	S2C_PLAYER_SYNC	sMsg;
	sMsg.ProtocolType = s2c_playersync;
	sMsg.m_wLength = sizeof(S2C_PLAYER_SYNC) - 1;
	sMsg.m_wMsgID = enumS2C_PLAYERSYNC_ID_RANKDATA;
	sMsg.m_lpBuf = 0;
	g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);

	return 0;
}

int LuaSetSavePw(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
		
	Player[nPlayerIndex].SetSavePw((char*)Lua_ValueToString(L,1), TRUE);
	return 0;
}

int LuaGetSavePw(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
		
	Lua_PushString(L, (char*)Player[nPlayerIndex].m_cTask.GetSaveStr(TASKVALUE_BASEDATA_PASSWORD));
	return 1;
}

int LuaSetLockState(Lua_State * L)
{
	int bLock = (int)Lua_ValueToNumber(L,1);
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L,0);
		return 1;
	}

	Player[nPlayerIndex].SetLockState(bLock > 0);
	return 0;
}

int LuaGetLockState(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L,0);
		return 1;
	}
	if (Player[nPlayerIndex].GetLockState())
	{	
		Lua_PushNumber(L,1);
	}
	else
	{
		Lua_PushNumber(L,0);
	}
	return 1;
}

int LuaIsHideNpc(Lua_State * L)
{
	int nRet = -1;
	if (Lua_GetTopIndex(L) == 1)
	{
		int nNpcIndex  = 0;
		nNpcIndex = (int)Lua_ValueToNumber(L, 1);
		if (nNpcIndex > 0 || nNpcIndex < MAX_NPC)
		{
			if (Npc[nNpcIndex].IsAlive())
				nRet = 0;
			else
				nRet = 1;
		}
	}
	Lua_PushNumber(L ,nRet);
	return 1;
}

int LuaSetPKState(Lua_State *L)
{
	int nPKSet = 0;
	int nPKState = 0;
	int nPlayerIndex = GetPlayerIndex(L);
	int nParamNum = Lua_GetTopIndex(L);
	if (nPlayerIndex > 0)
	{
		if (nParamNum <= 2)
		{
			nPKSet = (int)Lua_ValueToNumber(L, 1);
			nPKState = Player[nPlayerIndex].m_cPK.GetLockPKState();
		}
		else
		{
			nPKSet = (int)Lua_ValueToNumber(L, 1);
			nPKState = (int)Lua_ValueToNumber(L, 2);
		}
		if (nPKSet < 0 || nPKSet >= enumPKNum)
			return 0;

		if (nPKState < enumPKLogNothing || nPKSet >= enumPKLogNum)
			return 0;

		Player[nPlayerIndex].m_cPK.SetLockPKState(nPKSet, nPKState);
		Lua_PushNumber(L, 1);
	}
	else 
	{
		Lua_PushNumber(L,0);
	}
	return 1;
}

int LuaForbidChangePK(Lua_State *L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum > 1)
	{
		Player[nPlayerIndex].m_cPK.SetLockPKState(Player[nPlayerIndex].m_cPK.GetNormalPKState(), (int)Lua_ValueToNumber(L, 1));
	}
	return 0;
}

int LuaGetPKState(Lua_State *L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nResult = Player[nPlayerIndex].m_cPK.GetLockPKState();
		Lua_PushNumber(L, nResult);
	}
	else 
	{
		Lua_PushNumber(L,0);
	}
	return 1;
}

int LuaGetNormalPKState(Lua_State *L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nResult = Player[nPlayerIndex].m_cPK.GetNormalPKState();
		Lua_PushNumber(L, nResult);
	}
	else 
	{
		Lua_PushNumber(L,0);
	}
	return 1;
}

int LuaPaceBar(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L,0);
		return 1;
	}
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount<2)
		return 0;

	strcpy(Player[nPlayerIndex].m_szTaskExcuteFun, (char *)Lua_ValueToString(L,1));
	Player[nPlayerIndex].m_nPaceBarTime = (int)Lua_ValueToNumber(L,2);
	Player[nPlayerIndex].m_nPaceBarTimeMax = (int)Lua_ValueToNumber(L,2);
	Player[nPlayerIndex].m_dwTaskExcuteScriptId = Npc[Player[nPlayerIndex].m_nIndex].m_ActionScriptID;

	return 0;
}

int LuaDelItem(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum > 0)
	{
		int nPlayerIndex = GetPlayerIndex(L);
		if (nPlayerIndex > 0)
		{
			int nItemNature, nItemGenre, nDetailType, nLevel, nSeries,P = pos_equiproom, nResult = 0;
			nItemNature = nItemGenre = nDetailType = nLevel = nSeries = -1;

			int nDelNum = (int)Lua_ValueToNumber(L, 1);

			if (nDelNum)
			{
				nItemNature = (int)Lua_ValueToNumber(L, 2);
				nItemGenre = (int)Lua_ValueToNumber(L, 3);

				if (nParamNum > 3)
					nDetailType = (int)Lua_ValueToNumber(L, 4);	

				if (nParamNum > 4)
					nLevel = (int)Lua_ValueToNumber(L, 5);	

				if (nParamNum > 5)
					nSeries = (int)Lua_ValueToNumber(L, 6);

				if (nParamNum > 6)
					P = (int)Lua_ValueToNumber(L, 7);	

				Lua_PushNumber(L, Player[nPlayerIndex].m_ItemList.RemoveCommonItem(nDelNum,nItemNature,nItemGenre,nDetailType,nLevel,nSeries,P));
				return 1;
			}
		}
	}		
	Lua_PushNumber(L, 0);
	return 1;
}


int LuaGetItemCount(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum > 0)
	{
		int nPlayerIndex = GetPlayerIndex(L);
		if (nPlayerIndex > 0)
		{
			int nItemNature, nItemGenre, nDetailType, nLevel, nSeries,P = pos_equiproom;
			nItemNature = nItemGenre = nDetailType = nLevel = nSeries = -1;

			nItemNature = (int)Lua_ValueToNumber(L, 1);
			nItemGenre = (int)Lua_ValueToNumber(L, 2);

			if (nParamNum > 2)
				nDetailType = (int)Lua_ValueToNumber(L, 3);

			if (nParamNum > 3)
				nLevel = (int)Lua_ValueToNumber(L, 4);

			if (nParamNum > 4)
				nSeries = (int)Lua_ValueToNumber(L, 5);

			if (nParamNum > 5)
				P = (int)Lua_ValueToNumber(L, 6);	

			Lua_PushNumber(L, Player[nPlayerIndex].m_ItemList.CountCommonItem(nItemNature, nItemGenre,nDetailType,nLevel,nSeries,P));
			return 1;
		}
	}
	Lua_PushNumber(L, 0);
	return 1;
}

int LuaGetItemCountRoom(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum > 0)
	{
		int nPlayerIndex = GetPlayerIndex(L);
		if (nPlayerIndex > 0)
			Lua_PushNumber(L, Player[nPlayerIndex].m_ItemList.GetItemCountRoom((int)Lua_ValueToNumber(L, 1)));
	}
	return 1;
}

int LuaGetTrade(Lua_State * L)
{	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	
	if (Player[nPlayerIndex].m_PTrade.nTrade)
		Lua_PushNumber(L, 1);
	else
		Lua_PushNumber(L, 0);
	return 1;
}

int LuaForbidUseTownP(Lua_State * L)
{	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;

	if (Lua_GetTopIndex(L) < 2)
		return 0;
	Player[nPlayerIndex].ForbidUseTownP(Lua_ValueToNumber(L,1) > 0);
	return 0;
}

int LuaForbidTrade(Lua_State * L)
{	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;

	if (Lua_GetTopIndex(L) < 2)
		return 0;

	Player[nPlayerIndex].ForbidTrade((BOOL)Lua_ValueToNumber(L, 1));
	return 0;
}

int LuaForbidEnmity(Lua_State * L)
{	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;

	if (Lua_GetTopIndex(L) < 2)
		return 0;

	Player[nPlayerIndex].ForbidEnmity((BOOL)Lua_ValueToNumber(L, 1));
	return 0;
}

int LuaForbidName(Lua_State * L)
{	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;

	if (Lua_GetTopIndex(L) < 2)
		return 0;

	Player[nPlayerIndex].ForbidName((int)Lua_ValueToNumber(L, 1));
	return 0;
}

int LuaForbidCamp(Lua_State * L)
{	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;

	if (Lua_GetTopIndex(L) < 2)
		return 0;

	Player[nPlayerIndex].ForbidCamp((BOOL)Lua_ValueToNumber(L, 1));
	return 0;
}

int LuaGetDistance(Lua_State * L)
{
	Lua_PushNumber(L, g_GetDistance((int)Lua_ValueToNumber(L,1),(int)Lua_ValueToNumber(L,2),
		(int)Lua_ValueToNumber(L,3),(int)Lua_ValueToNumber(L,4)));
	return 1;
}

int LuaGetDir(Lua_State * L)
{
	Lua_PushNumber(L, g_GetDirIndex((int)Lua_ValueToNumber(L,1),(int)Lua_ValueToNumber(L,2),
		(int)Lua_ValueToNumber(L,3),(int)Lua_ValueToNumber(L,4)));
	return 1;
}

int LuaDirSin(Lua_State * L)
{
	Lua_PushNumber(L, g_DirSin((int)Lua_ValueToNumber(L,1),64));
	return 1;
}

int LuaDirCos(Lua_State * L)
{
	Lua_PushNumber(L, g_DirCos((int)Lua_ValueToNumber(L,1),64));
	return 1;
}

int LuaFileExists(Lua_State * L)
{
	char * szFileName = (char *)Lua_ValueToString(L, 1);
	if (g_FileExists(szFileName))
		Lua_PushNumber(L, 1);
	else
		Lua_PushNumber(L, 0);
	return 1;
}

int LuaFileName2Id(Lua_State * L)
{
	char * szScript = (char *)Lua_ValueToString(L, 1);
	Lua_PushNumber(L, g_FileName2Id(szScript));
	return 1;
}

int LuaTabFile_Load(Lua_State * L)
{
	KTabFile TabFile;
	char * szFileName = (char *)Lua_ValueToString(L,1);
	if (TabFile.Load(szFileName))
		return 1;
	else
		return 0;
	return 1;
}


int LuaIniFile_IsSectionExist(Lua_State * L)
{
	KIniFile IniFile;
	char * szFileName = (char *)Lua_ValueToString(L,1);
	char * szSection = (char *)Lua_ValueToString(L,2);
	if (IniFile.Load(szFileName))
	{
		if (IniFile.IsSectionExist(szSection))
		{
			Lua_PushNumber(L, 1);
			return 1;
		}
	}
	Lua_PushNumber(L, 0);
	return 1;
}

int LuaIniFile_Load(Lua_State * L)
{
	KIniFile IniFile;
	char * szFileName = (char *)Lua_ValueToString(L,1);
	if (IniFile.Load(szFileName))
		Lua_PushNumber(L, 1);
	else
		Lua_PushNumber(L, 0);
	return 1;
}

int LuaIniFile_GetStr(Lua_State * L)
{
	KIniFile IniFile;
	char * szFileName = (char *)Lua_ValueToString(L,1);
	char * szSectName = (char *)Lua_ValueToString(L,2);
	char * szKeyName = (char *)Lua_ValueToString(L,3);
	char szValue[128];
	if (IniFile.Load(szFileName))
	{
		IniFile.GetString(szSectName,szKeyName,"",szValue, sizeof(szValue));
		Lua_PushString(L, szValue);
		return 1;
	}
	return 0;
}

int LuaIniFile_GetInt(Lua_State * L)
{
	KIniFile IniFile;
	char * szFileName = (char *)Lua_ValueToString(L,1);
	char * szSectName = (char *)Lua_ValueToString(L,2);
	char * szKeyName = (char *)Lua_ValueToString(L,3);
	int nValue;
	if (IniFile.Load(szFileName))
	{
		IniFile.GetInteger(szSectName,szKeyName,0,&nValue);
		Lua_PushNumber(L, nValue);
		return 1;
	}
	return 0;
}

int LuaIniFile_GetInt2(Lua_State * L)
{
	KIniFile IniFile;
	char * szFileName = (char *)Lua_ValueToString(L,1);
	char * szSectName = (char *)Lua_ValueToString(L,2);
	char * szKeyName = (char *)Lua_ValueToString(L,3);
	int nValue1, nValue2;
	if (IniFile.Load(szFileName))
	{
		IniFile.GetInteger2(szSectName,szKeyName,&nValue1,&nValue2);
		Lua_PushNumber(L, nValue1);
		Lua_PushNumber(L, nValue2);
		return 2;
	}
	return 0;
}

int LuaIniFile_Save(Lua_State * L)
{
	KIniFile IniFile;
	char * szFileName = (char *)Lua_ValueToString(L,1);

	IniFile.Save(szFileName);
	return 0;
}

int LuaIniFile_SaveStr(Lua_State * L)
{
	KIniFile IniFile;
	char * szFileName = (char *)Lua_ValueToString(L,1);
	char * szSectName = (char *)Lua_ValueToString(L,2);
	char * szKeyName = (char *)Lua_ValueToString(L,3);
	char * szValue = (char *)Lua_ValueToString(L,4);
	if (IniFile.Load(szFileName))
	{
		IniFile.WriteString(szSectName,szKeyName,szValue);
		IniFile.Save(szFileName);
	}
	return 0;
}

int LuaIniFile_SaveInt(Lua_State * L)
{
	KIniFile IniFile;
	char * szFileName = (char *)Lua_ValueToString(L,1);
	char * szSectName = (char *)Lua_ValueToString(L,2);
	char * szKeyName = (char *)Lua_ValueToString(L,3);
	int nValue = (int)Lua_ValueToNumber(L,4);
	if (IniFile.Load(szFileName))
	{
		IniFile.WriteInteger(szSectName,szKeyName,nValue);
		IniFile.Save(szFileName);
	}
	return 0;
}

int LuaIniFile_SaveInt2(Lua_State * L)
{
	KIniFile IniFile;
	char * szFileName = (char *)Lua_ValueToString(L,1);
	char * szSectName = (char *)Lua_ValueToString(L,2);
	char * szKeyName = (char *)Lua_ValueToString(L,3);
	int nValue1 = (int)Lua_ValueToNumber(L,4);
	int nValue2 = (int)Lua_ValueToNumber(L,5);
	if (IniFile.Load(szFileName))
	{
		IniFile.WriteInteger2(szSectName,szKeyName,nValue1,nValue2);
		IniFile.Save(szFileName);
	}
	return 0;
}

int LuaTabFile_GetRowCount(Lua_State * L)
{
	KTabFile TabFile;
	char * szFileName = (char *)Lua_ValueToString(L,1);
	int nCount = 0;
	TabFile.Load(szFileName);
	nCount = TabFile.GetHeight();
	Lua_PushNumber(L, nCount);
	return 1;
}

int LuaTabFile_GetCell(Lua_State * L)
{
	KTabFile TabFile;
	char szString[128];
	char * szFileName = (char *)Lua_ValueToString(L, 1);
	if (TabFile.Load(szFileName))
	{
		if (Lua_IsNumber(L, 2) && Lua_IsNumber(L, 3))
		{
			int nRow = (int)Lua_ValueToNumber(L, 2);
			int nColumn = (int)Lua_ValueToNumber(L, 3);
			TabFile.GetString(nRow, nColumn, "", szString, sizeof(szString));
		}
		else if (Lua_IsNumber(L, 2) && Lua_IsString(L, 3))
		{
			int nRow = (int)Lua_ValueToNumber(L, 2);
			char szColumn[32];
			strcpy(szColumn, Lua_ValueToString(L, 3));
			TabFile.GetString(nRow, szColumn, "", szString, sizeof(szString));
		}
		else if (Lua_IsString(L, 2) && Lua_IsString(L, 3))
		{
			char szRow[32];
			char szColumn[32];
			strcpy(szRow, Lua_ValueToString(L, 2));
			strcpy(szColumn, Lua_ValueToString(L, 3));
			TabFile.GetString(szRow, szColumn, "", szString, sizeof(szString));
		}
		else
			return 0;

		Lua_PushString(L, szString);
		TabFile.Clear();
		return 1;
	}
	return 0;
}

int LuaGetDataInt(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) return 0;
	int nId = (int)Lua_ValueToNumber(L,1);
	Lua_PushNumber(L, GameData.GetDataInt(nId));
	return 1;
}

int LuaGetDataStr(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) return 0;
	int nId = (int)Lua_ValueToNumber(L,1);
	Lua_PushString(L, GameData.GetDataStr(nId));
	return 1;
}

int LuaSetData(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 2) return 0;
	int nNo = (int)Lua_ValueToNumber(L,1);
	char* szValue = (char*)Lua_ValueToString(L,2);
	GameData.SetData(nNo, szValue);
	return 0;
}

int LuaSendReport(Lua_State * L)
{
	return 0;
}

int LuaGetTime(Lua_State * L)
{
	time_t rawtime;
	struct tm * timeinfo;
	
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );

	Lua_PushNumber(L, timeinfo->tm_year + 1900);
	Lua_PushNumber(L, timeinfo->tm_mon + 1);	
	Lua_PushNumber(L, timeinfo->tm_mday);
	Lua_PushNumber(L, timeinfo->tm_hour);
	Lua_PushNumber(L, timeinfo->tm_min);
	Lua_PushNumber(L, timeinfo->tm_sec);
	Lua_PushNumber(L, timeinfo->tm_wday);
	Lua_PushNumber(L, timeinfo->tm_yday);
	return 8;
}

int LuaGetTimeDate(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0;

	time_t rawtime=(int)Lua_ValueToNumber(L,2)+1451581200;

	struct tm * timeinfo = localtime(&rawtime);
	
	const char* pszKey = (char*)Lua_ValueToString(L,1);

	char pszTimeFormat[256];
	if (strftime(pszTimeFormat, sizeof(pszTimeFormat), pszKey, timeinfo))
	{
		Lua_PushString(L, pszTimeFormat);
		return 1;
	}
	return 0;
}

int LuaGetLocalDate(Lua_State * L)
{
	time_t rawtime;
	struct tm * timeinfo;
	
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );

	const char* pszKey = (char*)Lua_ValueToString(L,1);

	char pszTimeFormat[256];
	if (strftime(pszTimeFormat, sizeof(pszTimeFormat), pszKey, timeinfo))
	{
		Lua_PushString(L, pszTimeFormat);
		return 1;
	}
	return 0;
}

int LuaAddDataGr(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) return 0;

	char* szName1 = (char*)Lua_ValueToString(L, 1);
	int nNameId = g_FileName2Id(szName1);

	KDataGroup Info;
	Info.nNameId = nNameId;
	memset(Info.nValue, 0, sizeof(Info.nValue));
	strcpy(Info.szName1, szName1);
	memset(Info.szName2, 0, sizeof(Info.szName2));

	Lua_PushNumber(L, GameData.AddDataGr(&Info));
	return 1;
}

int LuaSetDataGr(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 8) return 0;
	int i=0,j=0;
	KDataGroup Info;
	j++;
	int nGroup = (int)Lua_ValueToNumber(L,j);
	j++;
	Info.nNameId = (int)Lua_ValueToNumber(L,j);
	j++;
	for (i = 0; i < MAX_DATAGROUP_VALUE; i++)
		Info.nValue[i] = (int)Lua_ValueToNumber(L,j+i);
	strcpy(Info.szName1, (char*)Lua_ValueToString(L, MAX_DATAGROUP_VALUE+j));
	strcpy(Info.szName2, (char*)Lua_ValueToString(L, MAX_DATAGROUP_VALUE+j+1));
	GameData.SetDataGr(nGroup, &Info);
	return 0;
}

int LuaGetDataGr(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) return 0;

	int nNo = (int)Lua_ValueToNumber(L, 1);
	
	Lua_PushNumber(L, GameData.GetDataGr_dwName(nNo)); 
	for (int j = 0; j < MAX_DATAGROUP_VALUE; j++)
		Lua_PushNumber(L, GameData.GetDataGr_nValue(nNo, j)); 
	Lua_PushString(L, GameData.GetStrDataGr_Name1(nNo));
	Lua_PushString(L, GameData.GetStrDataGr_Name2(nNo));
	return 7;

}

int LuaFindDataId(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) return 0;
	
	int nNo = (int)Lua_ValueToNumber(L, 1);

	Lua_PushNumber(L, GameData.FindDataId((DWORD)Lua_ValueToNumber(L, 1)));
	return 1;
}

int LuaSaveDataFile(Lua_State * L)
{
	GameData.Save();
	return 0;
}


int LuaChatRoom_Create(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if(nPlayerIndex <= 0) return 0;
	if (Lua_GetTopIndex(L) < 4) return 0;
	BOOL bIsGmRoom = FALSE;
	if(Lua_GetTopIndex(L) >4)
		bIsGmRoom = (BOOL)Lua_ValueToNumber(L,4);
	if(Player[nPlayerIndex].m_cRoom.CreateChatRoom((char*)Lua_ValueToString(L,1),(int)Lua_ValueToNumber(L,2),(int)Lua_ValueToNumber(L,3),bIsGmRoom))
		Lua_PushNumber(L,1);
	else
		Lua_PushNumber(L,0);
	return 1;
}

int LuaChatRoom_AddTime(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if(nPlayerIndex <= 0) return 0;
	if (Lua_GetTopIndex(L) < 2) return 0;
	if(Player[nPlayerIndex].m_cRoom.AddTime((char*)Lua_ValueToString(L,1), (int)Lua_ValueToNumber(L,2)))
		Lua_PushNumber(L,1);
	else
		Lua_PushNumber(L,0);
	return 1;
}
#endif

int LuaGetCurServerSec(Lua_State * L)
{
	Lua_PushNumber(L, KSG_GetCurSec());
	return 1;
}

int LuaSetNumber(Lua_State * L)
{
	int nResult = 0;
	if (Lua_GetTopIndex(L) < 4)
		goto lab_setnumber;

	nResult = KSG_StringSetValue((int)Lua_ValueToNumber(L,1),
		(int)Lua_ValueToNumber(L,2),
		(int)Lua_ValueToNumber(L,3),
		(int)Lua_ValueToNumber(L,4));
	
lab_setnumber:
	Lua_PushNumber(L, nResult);
	return 1;
}

int LuaGetNumber(Lua_State * L)
{
	int nResult = 0;
	if (Lua_GetTopIndex(L) < 3)
		goto lab_getnumber;
	nResult = KSG_StringGetValue((int)Lua_ValueToNumber(L,1),
		(int)Lua_ValueToNumber(L,2),
		(int)Lua_ValueToNumber(L,3));

lab_getnumber:
	Lua_PushNumber(L, nResult);
	return 1;
}

int LuaGetNameItem(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1) return 0;
	
	int nIdx = (int)Lua_ValueToNumber(L, 1);
	if (nIdx > 0)
	{
		Lua_PushString(L, Item[nIdx].GetName());
		return 1;
	}
	return 0;
}

int LuaGetParamItem(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1) return 0;
	
	int nIdx = (int)Lua_ValueToNumber(L, 1);
	if (nIdx > 0)
	{
		Lua_PushNumber(L, Item[nIdx].GetParam());
		return 1;
	}
	return 0;
}

int LuaGetFortuneItem(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1) return 0;
	
	int nIdx = (int)Lua_ValueToNumber(L, 1);
	if (nIdx > 0)
	{
		Lua_PushNumber(L, Item[nIdx].GetFortune());
		return 1;
	}
	return 0;
}

int LuaGetLockItem(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1) return 0;
	
	int nIdx = (int)Lua_ValueToNumber(L, 1);
	if (nIdx > 0)
	{
		Lua_PushNumber(L, Item[nIdx].GetLock()->nState);
		if (KSG_GetCurSec() < Item[nIdx].GetLock()->dwLockTime)
			Lua_PushNumber(L, Item[nIdx].GetLock()->dwLockTime - KSG_GetCurSec());
		else
			Lua_PushNumber(L, Item[nIdx].GetLock()->dwLockTime);
		return 2;
	}
	Lua_PushNumber(L, 0);
	Lua_PushNumber(L, 0);
	return 2;
}

int LuaGetStackItem(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1) return 0;
	
	int nIdx = (int)Lua_ValueToNumber(L, 1);
	if (nIdx > 0)
	{
		Lua_PushNumber(L, Item[nIdx].GetStackNum());
		return 1;
	}
	return 0;
}

int LuaGetMaxStackItem(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1) return 0;
	
	int nIdx = (int)Lua_ValueToNumber(L, 1);
	if (nIdx > 0)
	{
		Lua_PushNumber(L, Item[nIdx].GetMaxStackNum());
		return 1;
	}
	return 0;
}

int LuaGetMantleItem(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1) return 0;
	
	int nIdx = (int)Lua_ValueToNumber(L, 1);
	if (nIdx > 0)
	{
		Lua_PushNumber(L, Item[nIdx].GetMantle());
		return 1;
	}
	return 0;
}


int LuaFindItem(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum > 0)
	{
		int nPlayerIndex = GetPlayerIndex(L);
		if (nPlayerIndex > 0)
		{
			if (nParamNum <= 1)
			{
				int nIdx = Player[nPlayerIndex].m_ItemList.FindSame((int)Lua_ValueToNumber(L, 1));
				if (nIdx)
				{
					Lua_PushNumber(L, Player[nPlayerIndex].m_ItemList.m_Items[nIdx].nPlace);
					Lua_PushNumber(L, Player[nPlayerIndex].m_ItemList.m_Items[nIdx].nX);
					Lua_PushNumber(L, Player[nPlayerIndex].m_ItemList.m_Items[nIdx].nY);
					return 3;
				}
				else
					return 0;
			}
			int nItemGenre, nDetailType, nLevel, nSeries;
			nItemGenre = nDetailType = nLevel = nSeries = -1;

			nItemGenre = (int)Lua_ValueToNumber(L, 1);

			if (nParamNum > 1)
				nDetailType = (int)Lua_ValueToNumber(L, 2);

			if (nParamNum > 2)
				nLevel = (int)Lua_ValueToNumber(L, 3);

			if (nParamNum > 3)
				nSeries = (int)Lua_ValueToNumber(L, 4);

			int nIdx = Player[nPlayerIndex].m_ItemList.FindItem(NATURE_NORMAL, nItemGenre,nDetailType,nLevel,nSeries);

			if (nIdx)
			{
				Lua_PushNumber(L, Player[nPlayerIndex].m_ItemList.m_Items[nIdx].nIdx);
				Lua_PushNumber(L, Player[nPlayerIndex].m_ItemList.m_Items[nIdx].nPlace);
				Lua_PushNumber(L, Player[nPlayerIndex].m_ItemList.m_Items[nIdx].nX);
				Lua_PushNumber(L, Player[nPlayerIndex].m_ItemList.m_Items[nIdx].nY);
				return 4;
			}
		}
	}
	Lua_PushNumber(L, 0);
	Lua_PushNumber(L, 0);
	Lua_PushNumber(L, 0);
	Lua_PushNumber(L, 0);
	return 4;
}

int LuaFindGoldItem(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum > 0)
	{
		int nPlayerIndex = GetPlayerIndex(L);
		if (nPlayerIndex > 0)
		{
			int nIndex = 0;

			nIndex = (int)Lua_ValueToNumber(L, 1);

			int nIdx = Player[nPlayerIndex].m_ItemList.FindItemByIndex(NATURE_GOLD, nIndex);

			if (nIdx)
			{
				Lua_PushNumber(L, Player[nPlayerIndex].m_ItemList.m_Items[nIdx].nIdx);
				Lua_PushNumber(L, Player[nPlayerIndex].m_ItemList.m_Items[nIdx].nPlace);
				Lua_PushNumber(L, Player[nPlayerIndex].m_ItemList.m_Items[nIdx].nX);
				Lua_PushNumber(L, Player[nPlayerIndex].m_ItemList.m_Items[nIdx].nY);
				return 4;
			}
		}
	}
	Lua_PushNil(L);
	return 1;
}

int LuaFindItemEx(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	
	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L,0);
		return 1;
	}
	int nParamNum = Lua_GetTopIndex(L);

	if (nParamNum <= 2)
	{
		int nIndex, nItemGenre = -1, nDetail = -1, nParticur = -1, nLevel = -1, nSeries = series_num, nLuck = 0, nStackNum = 0;
		nIndex = (int)Lua_ValueToNumber(L, 1);
		if (nIndex > 0)
		{
			nItemGenre = Item[nIndex].GetGenre();
			nDetail = Item[nIndex].GetDetailType();
			nParticur = Item[nIndex].GetParticular();
			nLevel = Item[nIndex].GetLevel();
			nSeries = Item[nIndex].GetSeries();
			nLuck = Item[nIndex].m_GeneratorParam.nLuck;
			nStackNum = Item[nIndex].GetStackNum();
		}

		Lua_PushNumber(L, nItemGenre);
		Lua_PushNumber(L, nDetail);
		Lua_PushNumber(L, nParticur);
		Lua_PushNumber(L, nLevel);
		Lua_PushNumber(L, nSeries);
		Lua_PushNumber(L, nLuck);
		Lua_PushNumber(L, nStackNum);
		return 7;
	}

	int nIndex = 0, nItemGenre = -1, nDetail = -1, nParticur = -1, nLevel = -1, nSeries = series_num, nLuck = 0, nStackNum = 0;
	if (nParamNum > 2)
	{
		nIndex = Player[nPlayerIndex].m_ItemList.PositionToIndex((int)Lua_ValueToNumber(L, 1), (int)Lua_ValueToNumber(L, 2));
	}
	if (nParamNum > 3)
	{
		nIndex = Player[nPlayerIndex].m_ItemList.m_Room[PositionToRoom((int)Lua_ValueToNumber(L, 1))].FindItem((int)Lua_ValueToNumber(L, 2), (int)Lua_ValueToNumber(L, 3));
	}

	if (nIndex > 0)
	{
		nItemGenre = Item[nIndex].GetGenre();
		nDetail = Item[nIndex].GetDetailType();
		nParticur = Item[nIndex].GetParticular();
		nLevel = Item[nIndex].GetLevel();
		nSeries = Item[nIndex].GetSeries();
		nLuck			= Item[nIndex].m_GeneratorParam.nLuck;
		nStackNum = Item[nIndex].GetStackNum();
	}
	Lua_PushNumber(L, nIndex);
	Lua_PushNumber(L, nItemGenre);
	Lua_PushNumber(L, nDetail);
	Lua_PushNumber(L, nParticur);
	Lua_PushNumber(L, nLevel);
	Lua_PushNumber(L, nSeries);
	Lua_PushNumber(L, nLuck);
	Lua_PushNumber(L, nStackNum);
	return 8;
}

int LuaPlayMusic(Lua_State * L)//PlayMusic(musicname,loop=1, vol );
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) return 0;
	
	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bUIId = UI_PLAYMUSIC;
	UiInfo.m_bOptionNum = 1;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	
	int nMsgId = 0;
	
	g_StrCpyLen(UiInfo.m_pContent, Lua_ValueToString(L,1), sizeof(UiInfo.m_pContent));
	UiInfo.m_nBufferLen = strlen(((char *)UiInfo.m_pContent));
	UiInfo.m_bParam1 = 0;
	
#ifndef _SERVER
	UiInfo.m_bParam2 = 0;
#else
	UiInfo.m_bParam2 = 1;
#endif
	
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
	return 0;
}

int LuaFadeInMusic(Lua_State * L)
{
	return 0;
}

int LuaFadeOutMusic(Lua_State * L)
{
	return 0;
}




#ifndef _SERVER

int LuaPlaySound(Lua_State * L)
{
	return 0;
}

int LuaPlaySprMovie(Lua_State * L)
{
	return 0;
}
#endif
TLua_Funcs GameScriptFuns[] = 
{	
	{"Say", LuaSelectUI},
	{"Talk", LuaTalkUI},
	{"Sel", LuaSelUI},
	{"GetTaskTemp", LuaGetTempTaskValue},
	{"GetTaskTempS", LuaGetTempTaskString},
	{"SetTaskTemp", LuaSetTempTaskValue},
	{"Message", LuaMessage},
	
	{"GetBit",	LuaGetBit},
	{"GetByte",	LuaGetByte},
	{"SetBit",	LuaSetBit},
	{"SetByte",	LuaSetByte},
	{"Include",LuaIncludeFile},
	{"AddNews",LuaAddGlobalNews},
	{"AddNews2",LuaAddGlobalNews2},
	{"AddTimeNews",LuaAddGlobalTimeNews},
	{"AddTimeNews2",LuaAddGlobalTimeNews2},
	{"AddCountNews",LuaAddGlobalCountNews},
	{"AddCountNews2",LuaAddGlobalCountNews2},
	{"AddLocalNews",LuaAddLocalNews},
	{"AddLocalTimeNews",LuaAddLocalTimeNews},
	{"AddLocalCountNews",LuaAddLocalCountNews	},
	

#ifdef _SERVER
	{"PutMessage"	,	LuaSendMessageInfo},
	{"GetGameTime",	LuaGetGameTime},
	{"GetLoginTime", LuaGetPlayerLoginTime},
	{"GetOnlineTime", LuaGetPlayerOnlineTime},
	{"OfflineLive", LuaOfflineLive},

	{"SetValue", LuaSetValue},
	{"AddValue", LuaAddValue},
	{"GetValue",LuaGetValue},	
	{"SetRepute", LuaSetPlayerReputeValue},
	{"AddRepute", LuaAddPlayerReputeValue},
	{"GetRepute",LuaGetPlayerReputeValue},	
	{"AddTranslife", LuaAddPlayerTranslifeValue},
	{"GetTranslife",LuaGetPlayerTranslifeValue},		
	{"AddViprank", LuaAddPlayerViprankValue},
	{"GetViprank",LuaGetPlayerViprankValue},		
	{"SetFuYuan", LuaSetPlayerFuYuanValue},
	{"AddFuYuan", LuaAddPlayerFuYuanValue},
	{"GetFuYuan",LuaGetPlayerFuYuanValue},
	{"AddAccum", LuaAddPlayerAccumValue},
	{"GetAccum",LuaGetPlayerAccumValue},
	{"AddHonor", LuaAddPlayerHonorValue},
	{"GetHonor",LuaGetPlayerHonorValue},
	{"AddRespect", LuaAddPlayerRespectValue},
	{"GetRespect",LuaGetPlayerRespectValue},
	{"GetNpcIdx", LuaGetCurNpcIndex},
	
	{"SetTimer",		LuaSetTimer},		//SetTimer(时间量, 时间TaskId):给玩家打开计时器,时间到时将自动调用OnTimer函数
	{"StopTimer",		LuaStopTimer},		//StopTimer()：关闭当前玩家的计时器
	{"GetRestTime",		LuaGetRestTime},	//GetRestTime:获得计时器将触发的剩于时间	
	{"GetTimerId",		LuaGetCurTimerId},	//CurTimerId = GetTimerId():获得当前执行的计时器的id,如果没有则返回0
	{"GetTask",			LuaGetTaskValue},	//GetTask(任务号):获得当前玩家该任务号的值
	{"GetTaskS",		LuaGetTaskString},	//GetTask(任务号):获得当前玩家该任务号的值
	{"SetTask",			LuaSetTaskValue},	//SetTask(任务号,值):设置任务值
	{"SyncTaskValue",	LuaSyncTaskValue},	//SetTask(任务号,值):设置任务值
	{"IsCaptain",		LuaIsLeader},		//IsCaptain()是否为队长
	{"GetTeam",			LuaGetTeamId},		//GetTeam()返回玩家队伍ID
	{"GetTeamSize",		LuaGetTeamSize},	//GetTeamSize()
	{"GetTeamMem",		LuaGetTeamMem},
	{"LeaveTeam",		LuaLeaveTeam},		//LeaveTeam()让玩家离开自身队伍
	{"TeamDoScript",	LuaTeamDoScript},	//TeamDoScript(links,func) return number player has set
	{"Msg2Player",		LuaMsgToPlayer	},	//Msg2Player(消息)
	{"Msg2Team",		LuaMsgToTeam},		//Msg2Team(消息)通知玩家的组
	{"Msg2SubWorld",	LuaMsgToSubWorld},	//Msg2SubWorld(消息)通知世界
	{"Msg2Faction",		LuaMsgToFaction},
	{"Msg2Tong",		LuaMsgToTong},
	{"Msg2Room",		LuaMsgToChatRoom},
	{"Msg2Region",		LuaMsgToAroundRegion},//Msg2Region(消息)通知周围Region
	{"Msg2GM",			LuaMsgToGameMaster}, //Msg2GM(StrInfo)
	{"Msg2IP",			LuaMsgToIP}, //Msg2IP(IP, ID, StrInfo)
	{"GetIP",			LuaGetIP},
	{"SetPos",			LuaSetPos},			//SetPos(x,y)进入某点	
	{"GetPos",			LuaGetPos},			//GetPos() return x,y,subworldindex
	{"GetWorldPos",		LuaGetNewWorldPos},	//W,X,Y = GetWorldPos()返回于NewWorld配陪的坐标
	{"GetWorldName",	LuaGetNewWorldName},
	{"GetWorldKind",	LuaGetNewWorldKind},
	{"GetFreePos",		LuaGetFreeObjPos},
	{"GetNpcPos",		LuaGetNpcPos},
	{"NewWorld",		LuaEnterNewWorld},
	{"NpcNewWorld",		LuaNpcEnterNewWorld},

	{"DropMoney",		LuaDropMoney},
	{"DropItem",		LuaDropItem},	
	{"DropMapItem",		LuaDropMapItem},	
///////////////////////////////////////////
	{"AddItem",				LuaAddItem},
	{"AddItemIdx",			LuaAddItemIdx},
	{"AddItemID",			LuaAddItemID},
	{"AddItemIDStack",		LuaAddItemIDStack},

	{"DelItem",				LuaDelItem},
	{"ConsumeItem",			LuaDelItem},
	{"GetItemCount",		LuaGetItemCount},
	{"GetItemCountRoom",	LuaGetItemCountRoom},

	{"GetMagicAttrib",		LuaGetMagicAttrib},
	{"SetMagicAttrib",		LuaSetMagicAttrib},

	{"AddMagic",		LuaAddMagic},		//AddMagic(魔法id或魔法名)给玩家加某个魔法
	{"DelMagic",		LuaDelMagic},		//DelMagic(魔法id或魔法名)
	{"DelAllMagic",		LuaDelAllMagic},		//DelMagic(魔法id或魔法名)
	{"HaveMagic",		LuaHaveMagic},		//HaveMagic(魔法id或魔法名)返回0或1
	{"IncSkill",		LuaIncSkill},
	{"IncSkillExp",		LuaIncSkillExp},
	{"GetMagicLevel",	LuaGetMagicLevel},	//GetMagicLevel(魔法id或魔法名)返回等级
	{"AddMagicPoint",	LuaAddMagicPoint},
	{"GetMagicPoint",	LuaGetMagicPoint},
	
	{"SubWorldID2Idx",	LuaSubWorldIDToIndex}, //SubWorldID2Idx
	{"SubWorldIdx2ID",	LuaSubWorldIndexToID}, //SubWorldID2Idx
	
	
	{"AddLeadExp",		LuaAddLeadExp},
	{"SetLeadLevel",	LuaSetLeadLevel},
	{"GetLeadLevel",	LuaGetLeadLevel},
	
	{"SetFightState",	LuaSetFightState}, 
	{"GetFightState",	LuaGetFightState},
	
	{"GetNpcTemplateID",LuaGetNpcTemplateID},
	{"GetNpcSettingIdx",LuaGetNpcTemplateID},
	{"GetNpcTempName",	LuaGetNpcTempName},
	{"GetNpcTempTypeName",	LuaGetNpcTempTypeName},
	{"AddNpc",			LuaAddNpc},			//AddNpc(人物模板id或人物模板名,所处世界id，点坐标x,点坐标y),返回npcid值
	{"DelNpc",			LuaDelNpc},			//DelNpc(Npcid)
	{"ClearMapNpc",		LuaClearMapNpc}, 
	{"ClearMapNpcWithName",		LuaClearMapNpcWithName}, 
	{"SetNpcKind",		LuaSetNpcKind},
	{"SetNpcSeries",	LuaSetNpcSeries},
	{"SetNpcSer",		LuaSetNpcSeries},
	{"GetNpcSer",		LuaGetNpcSeries},
	{"SetNpcExp",		LuaSetNpcExp},
	{"SetNpcLife",		LuaSetNpcLife},
	{"GetNpcLife",		LuaGetNpcLife},
	{"SetNpcReplenish",	LuaSetNpcLifeReplenish},
	{"SetNpcAR",		LuaSetNpcAR},
	{"SetNpcDefense",	LuaSetNpcDefense},
	{"SetNpcDamage",	LuaSetNpcDamage},
	{"SetNpcDmgEx",		LuaSetNpcDmgEx},
	{"SetNpcResist",	LuaSetNpcResist},
	{"SetNpcRevTime",	LuaSetNpcRevTime},
	{"SetNpcSpeed",		LuaSetNpcSpeed},
	{"SetNpcHitRecover",LuaSetNpcHitRecover},
	{"SetNpcBoss",		LuaSetNpcBoss},
	{"GetNpcBoss",		LuaGetNpcBoss},
	{"IsBlueBoss",		LuaIsBlueBoss},
	{"GetNpcExpRate",	LuaGetNpcExpRate},
	{"IsRideHorse",		LuaIsRideHorse},
	{"SyncNpc",			LuaSyncNpc},
	{"SetNpcPos",		LuaSetNpcPos},
	{"SetNpcScript",	LuaSetNpcActionScript},	//SetNpcScript(npcid, 脚本文件名)设置npc当前的脚本
	{"SetNpcName",		LuaSetNpcName},
	{"GetNpcName",		LuaGetNpcName},
	{"GetNpcID",		LuaGetNpcID},
	{"SetNpcSkill",		LuaSetNpcSkill},
	{"SetNpcDropScript",LuaSetNpcDropScript},
	{"SetNpcRemoveDeath",	LuaSetNpcRemoveDeath},
	{"SetNpcTimeout",	LuaSetNpcTimeout},
	{"GetNpcTimeout",	LuaGetNpcTimeout},
	{"SetNpcParam",		LuaSetNpcParam},
	{"SetNpcValue",		LuaSetNpcParam},
	{"GetNpcParam",		LuaGetNpcParam},
	{"GetNpcValue",		LuaGetNpcParam},
	{"SetNpcOwner",		LuaSetNpcOwner},
	{"GetNpcOwner",		LuaGetNpcOwner},
	{"SetNpcFindPathTime",LuaSetNpcFindPathTime}, 
	{"SetNpcCurCamp",	LuaSetNpcCurCamp},
	{"SetRevPos",		LuaSetPlayerRevivalPos},//SetRevPos(点位置X，点位置Y)设置玩家的当前世界的等入点位置
	{"Rev2Pos",			LuaGetPlayerRevivalPos},//SetRevPos(点位置X，点位置Y)设置玩家的当前世界的等入点位置
	{"GetCurRev",		LuaGetPlayerRevival},
	{"GetCurRevID",		LuaGetPlayerRevivalID},
	{"SetTempRevPos",	LuaSetDeathRevivalPos}, //SetTempRevPos(subworldid, x, y ) or SetTempRevPos(id);
	{"Revive",			LuaPlayerExecuteRevive}, //SetTempRevPos(subworldid, x, y ) or SetTempRevPos(id);
	{"GetCurCamp",		LuaGetPlayerCurrentCamp},//GetCurCamp()获得玩家的当前阵营
	{"GetCamp",			LuaGetPlayerCamp},//GetCamp()获得玩家阵营
	{"SetCurCamp",		LuaSetPlayerCurrentCamp},//SetCurCamp(阵营号):设置玩家当前阵营
	{"SetCamp",			LuaSetPlayerCamp},		  //SetCamp(阵营号):设置阵营	
	{"RestoreCamp",		LuaRestorePlayerCamp},//RestoreCamp()恢复阵营
	{"GetFaction",		LuaGetPlayerFaction,},//GetFaction()获得玩家的门派名 
	{"GetFactionName",	LuaGetPlayerFactionName,},
	{"GetFactionCamp",	LuaGetPlayerFactionCamp,},
	{"GetFactionNo",	LuaGetPlayerFactionNo},
	{"SetFaction",		LuaChangePlayerFaction},  //SetFaction(门派名):设置玩家门派名
	{"GetColdR",		LuaGetPlayerColdResist},	
	{"SetColdR",		LuaSetPlayerColdResist},
	{"GetFireR",		LuaGetPlayerFireResist},
	{"SetFireR",		LuaSetPlayerFireResist},
	{"GetLightR",		LuaGetPlayerLightResist},
	{"SetLightR",		LuaSetPlayerLightResist},
	{"GetPoisonR",		LuaGetPlayerPoisonResist},
	{"SetPoisonR",		LuaSetPlayerPoisonResist},
	{"GetPhyR",			LuaGetPlayerPhysicsResist},
	{"SetPhyR",			LuaSetPlayerPhysicsResist},
	{"GetNextExp",		LuaGetNextExp},			//GetExp():获得玩家的当前经验值
	{"GetExp",			LuaGetPlayerExp	},			//GetExp():获得玩家的当前经验值
	{"AddExp",			LuaModifyPlayerExp},		//AddExp(经验值，对方等级，是否组队共享经验值)
	{"AddOwnExp",		LuaAddOwnExp},				//AddOwnExp(Exp)，给玩家直接加经验
	{"AddStackExp",		LuaAddStackExp},				//AddSumExp(Exp)，给玩家直接加经验
	{"GetLife",			LuaGetPlayerLife},			//GetLife()获得玩家的生命值
	{"RestoreLife",		LuaRestorePlayerLife},		//RestoreLife()恢复玩家的生命
	{"GetMana",			LuaGetPlayerMana},			//GetMana()获得玩家的Mana
	{"RestoreMana",		LuaRestorePlayerMana},		//RestoreMana()恢复玩家的Mana
	{"GetStamina",		LuaGetPlayerStamina},		//GetStamina()获得玩家Stamina
	{"RestoreStamina",	LuaRestorePlayerStamina},	//RestoreMana()恢复玩家的Stamina
	{"GetDefend",		LuaGetPlayerDefend},		//GetDefend()获得玩家的防御力
	{"GetSex",			LuaGetPlayerSex},			//GetSex()获得玩家的性别
	{"GetSeries",		LuaGetPlayerSeries},		//GetSeries()获得玩家的系0man/1woman
	{"SetSeries",		LuaSetPlayerSeries},		//SetSeries(性别号)
	{"GetName",			LuaGetPlayerName},			//GetName()获得玩家的姓名
	{"GetPlayerNpcIdx",	LuaGetPlayerNpcIdx},
	{"GetMateName",		LuaGetMateName},
	{"GetAccount",		LuaGetPlayerAccount},
	{"GetUUID",			LuaGetPlayerID},			//GetUUID()获得玩家的唯一ID
	{"FindPlayer",		LuaFindPlayer},			//GetUUID()获得玩家的唯一ID
	{"FindNearNpc",		LuaFindNearNpc},
	{"FindAroundNpc",	LuaFindAroundNpc},
	{"GetLeadExp",		LuaGetPlayerLeadExp},		//GetLeadExp()获得玩家的统率经验值
	{"GetLeadLevel",	LuaGetPlayerLeadLevel},		//GetLeadLevel()获得玩家的统率等级
	{"SetLevel",		LuaSetLevel},
	{"GetLevel",		LuaGetLevel},				//GetLevel()GetPlayers Level
	{"GetPlayerLevel",	LuaGetLevel},				//GetLevel()GetPlayers Level
	{"GetRestAP",		LuaGetPlayerRestAttributePoint},//GetRestAP()获得玩家的剩于属性点数
	{"GetRestSP",		LuaGetPlayerRestSkillPoint},	//GetRestSP()获得玩家的剩于技能点数	
	{"GetLucky",		LuaGetPlayerLucky},			//GetLucky()获得玩家的幸运值
	{"GetEng",			LuaGetPlayerEngergy},		//GetEng()获得玩家的力量值Eng
	{"AddEng",			LuaSetPlayerEngergy},		//AddEng(Value)获得玩家的力量值Eng
	{"GetDex",			LuaGetPlayerDexterity},		//GetDex()获得玩家的Dex
	{"AddDex",			LuaSetPlayerDexterity},		//AddDex(Value)获得玩家的力量值Eng
	{"GetStrg",			LuaGetPlayerStrength},		//GetStrg()
	{"AddStrg",			LuaSetPlayerStrength},		//AddStrg(Value)获得玩家的力量值Eng
	{"GetVit",			LuaGetPlayerVitality},		//GetVit()
	{"AddVit",			LuaSetPlayerVitality},		//AddVit(Value)获得玩家的力量值Eng
	{"ResetBaseAttrib",	LuaResetBaseAttribute},		// 设置基本属性
	{"ResetProp",		LuaResetProp},
	{"GetCash",			LuaGetPlayerCashMoney},		//GetCash()获得玩家的现金
	{"Pay",				LuaPlayerPayMoney},			//Pay(金额数)扣除玩家金钱成功返回1，失败返回0
	{"Earn",			LuaPlayerEarnMoney},		//Earn(金额数)增加玩家金钱
	{"PrePay",			LuaPlayerPrePayMoney},		//付定金，成功返回1，失败返回0
	{"GetPlayerFortune",LuaGetPlayerFortune},		//GetCash()获得玩家的现金
	{"AttackNpc",		LuaAttackNpc},				//AttackNpc(NpcDwid,物理伤害值，冰，火，电，毒）
	{"KillNpc",			LuaKillNpc},				//KillNpc(NpcDWID)
	{"KillPlayer",		LuaKillPlayer},				//KillPlayer();
	{"Sale",			LuaSale},					//Sale(SaleId)买卖，SaleId为便卖的物品信息列表id
	{"NewSale",			LuaNewSale},							
	{"UseTownPortal",	LuaUseTownPortal},
	{"ReturnFromPortal",LuaReturnFromTownPortal},
	{"OpenBox",			LuaOpenBox},
	{"OpenEquipEx",		LuaOpenEquipEx},
	{"AddStation",		LuaAddPlayerStation},
	{"AddTermini",		LuaAddPlayerWayPoint},
	{"GetStation",		LuaGetPlayerStation	},
	{"GetStationCount", LuaGetPlayerStationCount},
	
	{"GetCityCount",	LuaGetAllStationCount},
	{"GetCity",			LuaGetCity},
	
	{"GetWayPoint",			LuaGetPlayerWayPoint},
	{"GetStationName",		LuaGetStationName},
	{"GetWayPointName",		LuaGetWayPointName},
	{"GetPrice2Station",	LuaGetPriceToStation},
	{"GetPrice2WayPoint",	LuaGetPriceToWayPoint	},
	{"GetStationPos",		LuaGetStationPos},
	{"GetWayPointPos",		LuaGetWayPointPos},
	{"GetWayPointFight",	LuaGetWayPointFight},
	{"GetPlayerCount",		LuaGetPlayerCount},
	{"GetNpcCount",			LuaGetNpcCount},
	{"GetTotalItem",		LuaGetTotalItem},
	{"GetRank",				LuaGetRank},
	{"SetRank",				LuaSetRank},
	{"SetRankEx",			LuaSetExpandRank},
	{"GetRankEx",			LuaGetExpandRank},
	{"RestoreRankEx",		LuaRestoreExpandRank},
	{"GetEquipItemEx",		LuaGetEquipItemEx},
	{"SetEquipItemEx",		LuaSetEquipItemEx},
	{"GetExpandBox",		LuaGetExpandBox},
	{"SetExpandBox",		LuaSetExpandBox},
	{"SetPropState",		LuaSetObjPropState},
	{"GetServerName",		LuaGetServerName},	
	
	//------------------Station Script ---------------
	{"GetWharfName",	LuaGetDockName},
	{"GetWharfCount",	LuaGetDockCount},
	{"GetWharfPrice",	LuaGetDockPrice},
	{"GetWharf",		LuaGetDock},
	{"GetWharfPos",		LuaGetDockPos},
	{"GetTerminiFState", LuaGetWayPointFightState},
	//------------------------------------------------
	{"KickOutPlayer",	LuaKickOutPlayer},
	{"KickOutSelf",		LuaKickOutSelf},
	{"KickOutAccount",	LuaKickOutAccount},
	{"GetSkillId",		LuaGetSkillIdInSkillList},
	{"SetSkillLevel",	LuaSetSkillLevel},
	{"SetSkillTemp",	LuaSetSkillTemp},
	{"SetChatFlag",		LuaSetPlayerChatForbiddenFlag},
	//------------------------------------------------
	
	{"AddNote", LuaAddNote},
	{"AddMissionNote", LuaAddMissionNote},
	//-----------------Mission Script-----------------
	{"GetMissionV", LuaGetMissionValue},//GetMissionV(Vid)
	{"GetMissionS", LuaGetMissionString},
	{"SetMission", LuaSetMission},//SetMissionV(Vid, Value)
	{"GetGlbMissionV", LuaGetGlobalMissionValue	},
	{"GetGlbMissionS", LuaGetGlobalMissionString	},
	{"SetGlbMission", LuaSetGlobalMission	},
	{"GetMissionName", LuaGetMissionName},//OpenMission(missionid)
	{"OpenMission", LuaInitMission},//OpenMission(missionid)
	{"RunMission", LuaRunMission},
	{"CloseMission", LuaCloseMission},//CloseMission(missionid)
	{"IsMission", LuaIsMission},
	{"GetMSLadder",LuaGetMSLadder},
	{"StartMissionTimer", LuaStartMissionTimer},////StartMissionTimer(missionid, timerid, time)
	{"StopMissionTimer", LuaStopMissionTimer},
	{"GetMSRestTime", LuaGetMissionRestTime}, //GetMSRestTime(missionid, timerid)
	{"GetMSIdxGroup",LuaGetPlayerMissionGroup},//GetPlayerGroup(missionid, playerid);
	
	{"AddMSPlayer", LuaAddMissionPlayer},
	{"AddMSNpc", LuaAddMissionNpc},
	{"DelMSPlayer", LuaRemoveMissionPlayer},
	{"DelMSNpc", LuaRemoveMissionNpc},
	{"SetMSGroup", LuaSetMissionGroup},
	{"PIdx2MSDIdx", LuaGetMissionPlayer_DataIndex},//(missionid, pidx)
	{"MSDIdx2PIdx", LuaGetMissionPlayer_PlayerIndex},//(missionid, dataidx)
	{"NpcIdx2PIdx", LuaNpcIndexToPlayerIndex},
	{"GetMSPlayerCount", LuaMissionPlayerCount},//GetMSPlayerCount(missionid, group = 0)
	{"GetMSNpcCount", LuaMissionNpcCount},//GetMSPlayerCount(missionid, group = 0)
	
	{"RevivalAllNpc",	LuaRevivalAllNpc},
	
	{"SetPMParam", LuaSetMissionPlayerParam},
	{"GetPMParam", LuaGetMissionPlayerParam},
	{"Msg2MSGroup", LuaMissionMsg2Group},
	{"Msg2MSAll", LuaMissionMsg2All},
	{"SetDeathScript", LuaSetPlayerDeathScript},
	{"SetDmgScript", LuaSetPlayerDamageScript},
	
	{"NpcChat", LuaNpcChat}	,
	{"HideNpc", LuaHideNpc}	,//HideNpc(npcindex/npcname, hidetime)
	{"SetLogoutRV", LuaSetPlayerRevivalOptionWhenLogout},
	{"SetCreateTeam",LuaSetCreateTeamOption},
	{"ForbidTeam",LuaSetFreezeTeamOption},
	{"GetPK", LuaGetPlayerPKValue},  //pkValue = GetPK() 
	{"SetPK", LuaSetPlayerPKValue}, //SetPK(pkValue)
	//------------------------------------------------
	//排名相关函数
	{"ShowLadder", LuaShowLadder}, //ShowLadder(LadderCount, LadderId1,LadderId2,...);
	{"GetLadder",  LuaGetLadder},
	//------------------------------------------------

	{"OpenTong",			LuaOpenTong},	//OpenTong()通知玩家打开帮会界面
	{"CreateTong",			LuaCreateTong},
	{"JoinTong",			LuaJoinTong},	//OpenTong()通知玩家打开帮会界面
	{"CommendMaster",		LuaCommendMaster},
	{"GetTongFlag",			LuaGetTongFlag},
	{"GetTongName",			LuaGetTongName},
	{"GetTongCamp",			LuaGetTongCamp},
	{"GetTongMemNum",		LuaGetTongMemNum},
	{"GetTongFigure",		LuaGetTongFigure},
	{"GetTongMoney",		LuaGetTongMoney},
	{"GetTongLevel",		LuaGetTongLevel},
	{"GetTongEff",			LuaGetTongEff},
	{"GetTongParam",		LuaGetTongParam},
	{"GetTongJoinTm",		LuaGetTongJoinTm},
	{"SetTongLevel",		LuaSetTongLevel},
	{"SetTongMoney",		LuaSetTongMoney},
	{"SetTongEff",			LuaSetTongEff},
	{"SetTongParam",		LuaSetTongParam},
	{"SetTongMemELW",		LuaSetTongMemEffLW},
	{"SetTongMemETW",		LuaSetTongMemEffTW},
	{"SetTongMemEUB",		LuaSetTongMemEffUB},
	{"GetTongMemEff",		LuaGetTongMemEff},
	{"SetPunish",			LuaSetDeathPunish},// SetPunish(0/1)
	{"SetReviveNow",		LuaSetReviveNow},
	//-------------------------------------------------
	//结拜
	//{"SwearBrother", LuaSwearBrother}, // ret = SwearBrother(TeamId);
	//{"MakeEnemy",	LuaMakeEnemy}, //结仇 MakeEnemy(enemyname)
	{"MakeMate", LuaMakeMate}, // ret = MakeMate(matename);
	//{"UnMarry", LuaDeleteMate}, // ret = DeleteMate(matename);
	{"RollbackSkill", LuaRollBackSkills},
	//-------------------------------------------------

	{"AddProp",		LuaAddPropPoint},//加玩家属性点
	{"AddPropPoint",LuaAddPropPoint},//加玩家属性点
	{"GetProp",		LuaGetRestPropPoint},
	
	{"SetExtPoint",	LuaSetExtPoint},
	{"AddExtPoint",	LuaAddExtPoint},
	{"GetExtPoint",	LuaGetExtPoint},
	{"PayExtPoint",	LuaPayExtPoint},

	{"AddSkillState",	LuaAddSkillState},
	{"AddNpcSkillState",LuaAddNpcSkillState},
	{"IgnoreState",		LuaIgnoreState},
	{"CastSkill",		LuaCastSkill},
	{"NpcCastSkill",	LuaNpcCastSkill},
	{"SetMask",			LuaSetMask},	

	{"AddTrap",			LuaAddTrap},

	{"AddObj",			LuaAddObj},
	{"SetObjValue",		LuaSetObjValue},
	{"GetObjValue",		LuaGetObjValue},
	{"SetObjPickExecute",	LuaSetObjPickExecute},
	{"SetObjScript",	LuaSetObjScript},
	{"DelObj",			LuaDelObj},
	{"RemoveItem",		LuaRemoveItemIdx},
	{"LockMoveItem",	LuaLockMoveItem},
	{"SyncItem",		LuaSyncItem},

	{"SetTempItem",		LuaSetTempItem},
	{"SetLevelItem",	LuaSetLevelItem},
	{"SetSeriesItem",	LuaSetSeriesItem},
	{"SetParamItem",	LuaSetParamItem},
	{"SetFortuneItem",	LuaSetFortuneItem},
	{"SetExpireTimeItem",LuaSetTimeItem},
	{"SetTimeItem",		LuaSetTimeItem},
	{"AddExpireTimeItem",LuaAddTimeItem},
	{"AddTimeItem",		LuaAddTimeItem},
	{"LockItem",		LuaLockItem},
	{"SetStackItem",	LuaSetStackItem},
	{"SetMantleItem",	LuaSetMantleItem},
	{"SetFlashItem",	LuaSetFlashItem},

	{"SetSavePw",			LuaSetSavePw},
	{"GetSavePw",			LuaGetSavePw},
	{"SetLockState",		LuaSetLockState},
	{"GetLockState",		LuaGetLockState},
	{"CheckRoom",			LuaCheckRoom},

	{"OpenRankData",		LuaOpenRankData},
	{"Input",				LuaInput},
	{"GetInput",			LuaGetInput},
	{"OpenEnchase",			LuaOpenEnchase},
	{"Enchase",				LuaEnchase},
	{"GiveItemUI",			LuaOpenGive},

	{"SetPKMode",			LuaSetPKState},
	{"ForbidChangePK",		LuaForbidChangePK},
	{"GetPKState",			LuaGetPKState},
	{"GetNormalPKState",	LuaGetNormalPKState},

	{"IsHideNpc",			LuaIsHideNpc},
	{"PaceBar",				LuaPaceBar},
	{"GetTrade",			LuaGetTrade},
	{"ForbidUseTownP",		LuaForbidUseTownP},
	{"ForbidTrade",			LuaForbidTrade},
	{"ForbidEnmity",		LuaForbidEnmity},
	{"ForbidName",			LuaForbidName},
	{"ForbidCamp",			LuaForbidCamp},

	{"GetDistance",			LuaGetDistance},
	{"GetDir",				LuaGetDir},
	{"DirSin",				LuaDirSin},
	{"DirCos",				LuaDirCos},
	{"FileExists",			LuaFileExists},
	{"FileName2Id",			LuaFileName2Id},
	{"TabFile_Load",		LuaTabFile_Load},
	{"IniFile_IsSectionExist",		LuaIniFile_IsSectionExist},
	{"IniFile_Load",		LuaIniFile_Load},
	{"IniFile_GetStr",		LuaIniFile_GetStr},
	{"IniFile_GetInt",		LuaIniFile_GetInt},
	{"IniFile_GetInt2",		LuaIniFile_GetInt2},
	{"IniFile_Save",		LuaIniFile_Save},
	{"IniFile_SaveStr",		LuaIniFile_SaveStr},
	{"IniFile_SaveInt",		LuaIniFile_SaveInt},
	{"IniFile_SaveInt2",		LuaIniFile_SaveInt2},
	{"TabFile_GetRowCount",	LuaTabFile_GetRowCount},
	{"TabFile_GetCell",		LuaTabFile_GetCell},
	{"RemoveRoom",			LuaRemoveRoom},
	{"CalcFreeItemCellCount",LuaCalcFreeItemCellCount},
	{"IsNumber",			LuaIsNumber},
	{"IsTable",				LuaIsTable},
	{"OpenURL",				LuaOpenURL},
	{"OpenExplore",			LuaOpenExplore},
	{"ModifyAttrib",		LuaModifyAttrib},
	{"RANDOM",				LuaRANDOM},
	{"RANDOMC",				LuaRANDOMC},
	{"IsMyItem",			LuaIsMyItem},
	{"GetDataInt",			LuaGetDataInt},
	{"GetDataStr",			LuaGetDataStr},
	{"SetData",				LuaSetData},
	{"SendReport",			LuaSendReport},
	{"GetTime",				LuaGetTime},
	{"GetTimeDate",			LuaGetTimeDate},
	{"GetLocalDate",		LuaGetLocalDate},

	{"AddDataGr",			LuaAddDataGr},
	{"SetDataGr",			LuaSetDataGr},
	{"GetDataGr",			LuaGetDataGr},
	{"FindDataId",			LuaFindDataId},
	{"SaveDataFile",		LuaSaveDataFile},

	{"ChatRoom_Create",		LuaChatRoom_Create},
	{"ChatRoom_AddTime",	LuaChatRoom_AddTime},
#else 
	{"PlaySound", LuaPlaySound}, //PlaySound(Sound);
	{"PlaySprMovie",LuaPlaySprMovie},//PlaySprMovie(npcindex, Movie, times)
#endif
	{"GetCurServerSec",		LuaGetCurServerSec},
	{"SetNumber",			LuaSetNumber},
	{"GetNumber",			LuaGetNumber},

	{"GetNameItem",		LuaGetNameItem},
	{"GetParamItem",	LuaGetParamItem},
	{"GetFortuneItem",	LuaGetFortuneItem},
	{"GetLockItem",		LuaGetLockItem},
	{"GetStackItem",	LuaGetStackItem},
	{"GetMaxStackItem",	LuaGetMaxStackItem},
	{"GetMantleItem",	LuaGetMantleItem},
	{"FindItem",		LuaFindItem},
	{"FindGoldItem",	LuaFindGoldItem},
	{"FindItemEx",		LuaFindItemEx},

	{"PlayMusic", LuaPlayMusic}, //PlayMusic(Music,Loop)
	{"FadeInMusic",LuaFadeInMusic},
	{"FadeOutMusic",LuaFadeOutMusic},
};


TLua_Funcs WorldScriptFuns[] =// 非指对玩家的脚本指令集
{
	//通用指令

	{"AddLocalNews",LuaAddLocalNews},
	{"AddLoaclTimeNews",LuaAddLocalTimeNews},
	{"AddLocalCountNews",LuaAddLocalCountNews	},
	//服务器端脚本指令
#ifdef _SERVER
	{"Msg2SubWorld",	LuaMsgToSubWorld},	//Msg2SubWorld(消息)通知世界
	{"Msg2IP",			LuaMsgToIP}, //Msg2IP(IP, ID, StrInfo)	
	{"SubWorldID2Idx",	LuaSubWorldIDToIndex}, //SubWorldID2Idx
	{"GetServerName",	LuaGetServerName},
	{"KickOutPlayer",	LuaKickOutPlayer},
	{"KickOutAccount",	LuaKickOutAccount},
	{"SetRoleChatFlag",	LuaSetRoleChatFlag},
	{"ShutDownServer",	LuaShutDownServer},	
#endif
}; 

int g_GetGameScriptFunNum()
{
	return sizeof(GameScriptFuns)  / sizeof(GameScriptFuns[0]);
}

int g_GetWorldScriptFunNum()
{
	return sizeof(WorldScriptFuns)  / sizeof(WorldScriptFuns[0]);
}