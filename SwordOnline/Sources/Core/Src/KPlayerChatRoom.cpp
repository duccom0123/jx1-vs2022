//---------------------------------------------------------------------------
// Sword3 Engine (c) 2002 by Kingsoft
//
// File:	KPlayerChatRoom.cpp
// Date:	2002.01.06
// Code:	±ß³ÇÀË×Ó
// Desc:	Team Class
//---------------------------------------------------------------------------

#include	"KCore.h"
#include	"KNpc.h"
#include	"KSG_StringProcess.h"
#ifdef _SERVER
//#include	"KNetServer.h"
//#include "../../Headers/IServer.h"
#else
//#include	"KNetClient.h"
#include "../../Headers/IClient.h"
#include	"CoreShell.h"
#endif
#include	"KPlayer.h"
#include	"KPlayerSet.h"
#include	"KPlayerChatRoom.h"
//#include	"MyAssert.h"

#define	DEFAULT_LEFTTIME 8*60*60

#ifdef _SERVER
KChatRoom		g_ChatRoom[MAX_CHATROOM];
KChatRoomSet	g_ChatRoomSet;
#endif

#ifndef _SERVER
KPlayerChatRoom::KPlayerChatRoom()
{
	Release();
}
#endif

#ifndef _SERVER
void	KPlayerChatRoom::Release()
{
	memset(m_szRoomName, 0, sizeof(m_szRoomName));
	m_nID = -1;
	m_nFlag = 0;
	m_nFigure = CHATROOM_NOTHING;
}
#endif


#ifndef _SERVER
//---------------------------------------------------------------------------
//	¹¦ÄÜ£º¸üÐÂ½çÃæÏÔÊ¾
//---------------------------------------------------------------------------
void	KPlayerChatRoom::JoinRoom(const char* szRoomName, int nId, CHATROOM_FIGURE eFigure)
{
	memcpy(m_szRoomName, szRoomName, strlen(szRoomName));
	m_nID = nId;
	m_nFlag = 1;
	m_nFigure = (int)eFigure;

	CoreDataChanged(GDCNI_CHATROOM_JOIN, (unsigned int)szRoomName, (int)eFigure);

	KSystemMessage	sMsg;
	sprintf(sMsg.szMessage, MSG_CHATROOM_SELF_JOIN, szRoomName);
	sMsg.eType = SMT_NORMAL;
	sMsg.byConfirmType = SMCT_NONE;
	sMsg.byPriority = 0;
	sMsg.byParamSize = 0;
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
}
#endif

#ifdef _SERVER
void	KPlayerChatRoom::Active()
{
	if(m_nFlag && m_nID >= 0 && m_nFigure == CHATROOM_HOST)
	{
		if(g_ChatRoom[m_nID].m_nRoomDiceTime > 0)
		{
			g_ChatRoom[m_nID].m_nRoomDiceTime--;
			if(g_ChatRoom[m_nID].m_nRoomDiceTime == 0)
			{
				g_ChatRoom[m_nID].Dice(::GetRandomNumber(1,2));
			}
		}
		if(g_ChatRoom[m_nID].m_nRoomLeftTime == -1)
			return;
		if(g_ChatRoom[m_nID].m_nRoomLeftTime <= KSG_GetCurSec() && g_ChatRoom[m_nID].m_nRoomDiceTime <= 0)
			g_ChatRoom[m_nID].RemoveLeftRoom();
	}
}
//---------------------------------------------------------------------------
//	¹¦ÄÜ£ºÇå¿Õ
//---------------------------------------------------------------------------
void	KPlayerChatRoom::Init(int nPlayerIdx)
{
	this->m_nPlayerIndex = nPlayerIdx;
	Release();
}

void	KPlayerChatRoom::Release()
{
	m_nFlag = 0;
	m_nID = -1;
	m_nFigure = CHATROOM_NOTHING;
}
#endif

#ifdef _SERVER
BOOL	KPlayerChatRoom::CreateChatRoom(char* szRoomName, int nRoomPw, int nLefttime, BOOL bIsGmRoom)
{
	int nNameLen = strlen(szRoomName);
	if ( !szRoomName[0] || nNameLen <= 0 || nNameLen >10)
	{
		PLAYER_SEND_CREATE_CHATROOM_FALSE	sCreateFalse;
		sCreateFalse.ProtocolType = s2c_chatroomcreatefalse;
		sCreateFalse.m_btErrorID = ChatRoom_Create_Error_Name;
		g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sCreateFalse, sizeof(PLAYER_SEND_CREATE_CHATROOM_FALSE));
		return FALSE;
	}
	if ( nRoomPw < 0 || nRoomPw >9999)
	{
		PLAYER_SEND_CREATE_CHATROOM_FALSE	sCreateFalse;
		sCreateFalse.ProtocolType = s2c_chatroomcreatefalse;
		sCreateFalse.m_btErrorID = ChatRoom_Create_Error_Pass;
		g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sCreateFalse, sizeof(PLAYER_SEND_CREATE_CHATROOM_FALSE));
		return FALSE;
	}
	// ´´½¨¶ÓÎé
	int nRet = g_ChatRoomSet.CreateChatRoom(m_nPlayerIndex, szRoomName, nRoomPw,nLefttime,bIsGmRoom);
	if (nRet >= 0)	// ¶ÓÎé´´½¨³É¹¦
	{
		PLAYER_SEND_CHATROOM_STATE	sCreateSuccess;
		sCreateSuccess.ProtocolType = s2c_chatroomstate;
		sCreateSuccess.m_btMsgId = ChatRoom_Create_Success;
		strcpy(sCreateSuccess.lpszName, szRoomName);
		g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sCreateSuccess, sizeof(PLAYER_SEND_CHATROOM_STATE));
		return TRUE;
	}
	else if (nRet == -1)					// ¶ÓÎé´´½¨Ê§°Ü£ºÍ¬Ãû
	{
		PLAYER_SEND_CREATE_CHATROOM_FALSE	sCreateFalse;
		sCreateFalse.ProtocolType = s2c_chatroomcreatefalse;
		sCreateFalse.m_btErrorID = ChatRoom_Create_Error_SameName;
		g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sCreateFalse, sizeof(PLAYER_SEND_CREATE_CHATROOM_FALSE));
		return FALSE;
	}
	else if (nRet == -2)
	{
		PLAYER_SEND_CREATE_CHATROOM_FALSE	sCreateFalse;
		sCreateFalse.ProtocolType = s2c_chatroomcreatefalse;
		sCreateFalse.m_btErrorID = ChatRoom_Create_Error_SameHost;
		g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sCreateFalse, sizeof(PLAYER_SEND_CREATE_CHATROOM_FALSE));
		return FALSE;
	}
	else if (nRet == -3)
	{
		PLAYER_SEND_CREATE_CHATROOM_FALSE	sCreateFalse;
		sCreateFalse.ProtocolType = s2c_chatroomcreatefalse;
		sCreateFalse.m_btErrorID = ChatRoom_Create_Error_RoomFull;
		g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sCreateFalse, sizeof(PLAYER_SEND_CREATE_CHATROOM_FALSE));
		return FALSE;
	}
	else	// ÆäËû´íÎó
	{
		return FALSE;
	}

	return TRUE;
}

BOOL KPlayerChatRoom::AddTime(const char* lpszName, int nLefttime)
{
	return g_ChatRoomSet.AddTime(lpszName, nLefttime);
}

BOOL KPlayerChatRoom::RequestList()
{
	g_ChatRoomSet.RequestRoomList(m_nPlayerIndex);

	if(m_nFlag && m_nID >= 0)
		g_ChatRoomSet.RequestList(m_nPlayerIndex, m_nID);

	return TRUE;
}

BOOL KPlayerChatRoom::JoinRoom(int nRoomIndex, int nRoomPw)
{
	if(nRoomIndex < 0 || nRoomIndex >= MAX_CHATROOM)
		return FALSE;
	if(m_nFlag == 0)
	{
		m_nID = g_ChatRoomSet.JoinRoom(m_nPlayerIndex, nRoomIndex, nRoomPw);
		if(m_nID >= 0)
		{
			m_nFlag = 1;

			PLAYER_SEND_CHATROOM_STATE	sJoinRoom;
			sJoinRoom.ProtocolType = s2c_chatroomstate;
			sJoinRoom.nId = m_nID;
			if(m_nPlayerIndex == g_ChatRoom[m_nID].m_nRoomHost)
			{
				m_nFigure = CHATROOM_HOST;
				sJoinRoom.m_btMsgId = ChatRoom_Join_Host;
			}
			else
			{
				m_nFigure = CHATROOM_MEMBER;
				sJoinRoom.m_btMsgId = ChatRoom_Join_Member;
			}
			strcpy(sJoinRoom.lpszName, g_ChatRoom[m_nID].m_szRoomName);
			g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sJoinRoom, sizeof(PLAYER_SEND_CHATROOM_STATE));
		}
	}
	return TRUE;
}

BOOL KPlayerChatRoom::LeaveRoom()
{
	if(m_nFlag && m_nID >= 0)
	{
		if(g_ChatRoomSet.LeaveRoom(m_nPlayerIndex, m_nID))
		{
			m_nFlag = 0;
			m_nID = -1;
			m_nFigure = CHATROOM_NOTHING;
		}
	}
	return TRUE;
}

BOOL KPlayerChatRoom::DeleteRoom()
{
	if(m_nFlag && m_nID >= 0 && m_nFigure == CHATROOM_HOST)
	{
		if(g_ChatRoomSet.DeleteRoom(m_nPlayerIndex, m_nID))
		{
			m_nFlag = 0;
			m_nID = -1;
			m_nFigure = CHATROOM_NOTHING;
		}
	}
	return TRUE;
}


BOOL KPlayerChatRoom::ForceLeaveRoom()
{
	if(m_nFlag && m_nID >= 0)
	{
		if(g_ChatRoomSet.ForceLeaveRoom(m_nPlayerIndex, m_nID))
		{
			m_nFlag = 0;
			m_nID = -1;
			m_nFigure = CHATROOM_NOTHING;
		}
	}
	return TRUE;
}

BOOL KPlayerChatRoom::Roomcommand(int n)
{
	int nValueIndex = KSG_StringGetValue(4,n,1);
	int nType = KSG_StringGetValue(4,n,2);
	if(nValueIndex >= 0)
	{
		if(m_nFlag && m_nID >= 0)
		{
			if(nType == 0)
			{
				if(m_nFigure == CHATROOM_HOST)
					return g_ChatRoomSet.AddBlackList(m_nPlayerIndex, m_nID, nValueIndex);
			}
			else if(nType==1)
			{
				if(m_nFigure == CHATROOM_HOST)
					return g_ChatRoomSet.RemoveBlackList(m_nPlayerIndex, m_nID, nValueIndex);
			}
			else if(nType==2)
			{
				if(m_nFigure == CHATROOM_HOST)
					return g_ChatRoomSet.ChangePw(m_nPlayerIndex, m_nID, nValueIndex);
			}
			else if(nType==3)
			{
				if(m_nFigure == CHATROOM_HOST)
					return g_ChatRoomSet.OpenGame(m_nPlayerIndex, m_nID, nValueIndex);
			}
			else if(nType==4)
			{
				if(m_nFigure == CHATROOM_MEMBER)
					return g_ChatRoomSet.Hight(m_nPlayerIndex, m_nID, nValueIndex);
			}
			else if(nType==5)
			{
				if(m_nFigure == CHATROOM_MEMBER)
					return g_ChatRoomSet.Low(m_nPlayerIndex, m_nID, nValueIndex);
			}
		}
	}
	return TRUE;
}
#endif

#ifdef _SERVER
//---------------------------------------------------------------------------
//	¹¦ÄÜ£º¹¹Ôìº¯Êý
//---------------------------------------------------------------------------
KChatRoom::KChatRoom()
{
	Release();
}

//---------------------------------------------------------------------------
//	¹¦ÄÜ£ºÇå¿Õ
//---------------------------------------------------------------------------
void	KChatRoom::Release()
{
	m_bIsGmRoom = FALSE;
	m_szRoomHost[0] = 0;
	m_nRoomHost = 0;
	m_szRoomName[0] = 0;
	m_nRoomLeftTime = 0;
	m_nRoomDiceTime = 0;
	m_nRoomDiceMax = 0;
	m_nRoomPw = 0;
	m_nMemNum = 0;
	memset(m_nMember, 0, sizeof(m_nMember));
	memset(m_nMemParam, 0, sizeof(m_nMemParam));
	memset(m_szBlackName, 0, sizeof(m_szBlackName));
}


void	KChatRoom::RemoveLeftRoom()
{
	for(int i=0; i < MAX_CHATROOM_MEMBER; i++)
	{
		if(m_nMember[i] > 0 && m_nMember[i] < MAX_PLAYER)
		{
			if(Player[m_nMember[i]].m_nNetConnectIdx != -1 && Player[m_nMember[i]].m_cRoom.m_nFlag)
				Player[m_nMember[i]].m_cRoom.ForceLeaveRoom();
		}
		m_nMember[i] = 0;
		m_nMemParam[i] = 0;
	}
	m_bIsGmRoom= FALSE;
	m_szRoomHost[0] = 0;
	m_nRoomHost = 0;
	m_szRoomName[0] = 0;
	m_nRoomLeftTime = 0;
	m_nRoomDiceTime = 0;
	m_nRoomDiceMax = 0;
	m_nRoomPw = 0;
	m_nMemNum = 0;
	memset(m_szBlackName, 0, sizeof(m_szBlackName));
}

void	KChatRoom::DeleteRoom()
{
	for(int i=0; i < MAX_CHATROOM_MEMBER; i++)
	{
		if(m_nMember[i] > 0 && m_nMember[i] < MAX_PLAYER)
		{
			if(m_nMember[i] == m_nRoomHost)
				continue;
			if(Player[m_nMember[i]].m_nNetConnectIdx != -1 && Player[m_nMember[i]].m_cRoom.m_nFlag)
				Player[m_nMember[i]].m_cRoom.ForceLeaveRoom();
		}
		m_nMember[i] = 0;
		m_nMemParam[i] = 0;
	}
	m_bIsGmRoom= FALSE;
	m_szRoomHost[0] = 0;
	m_nRoomHost = 0;
	m_szRoomName[0] = 0;
	m_nRoomLeftTime = 0;
	m_nRoomDiceTime = 0;
	m_nRoomDiceMax = 0;
	m_nRoomPw = 0;
	m_nMemNum = 0;
	memset(m_szBlackName, 0, sizeof(m_szBlackName));
}
//---------------------------------------------------------------------------
//	¹¦ÄÜ£ºÉè¶¨ Team ÔÚ g_ChatRoom ÖÐµÄÎ»ÖÃ
//---------------------------------------------------------------------------
void	KChatRoom::SetIndex(int nIndex)
{
	m_nIndex = nIndex;
}

//---------------------------------------------------------------------------
//	¹¦ÄÜ£ºÑ°ÕÒ¶ÓÔ±¿ÕÎ»
//---------------------------------------------------------------------------
int		KChatRoom::FindFree()
{
	for (int i = 0; i < MAX_CHATROOM_MEMBER; i++)
	{
		if (!m_nMember[i])
			return i;
	}
	return -1;
}

int		KChatRoom::FindFreeBlackList()
{
	for (int i = 0; i < MAX_CHATROOM_MEMBER; i++)
	{
		if (m_szBlackName[i][0] ==0)
			return i;
	}
	return -1;
}

BOOL		KChatRoom::CheckBlackList(const char *lpszName)
{
	for (int i = 0; i < MAX_CHATROOM_MEMBER; i++)
	{
		if (strcmp(m_szBlackName[i],lpszName) ==0)
			return TRUE;
	}
	return FALSE;
}

//---------------------------------------------------------------------------
//	¹¦ÄÜ£ºÑ°ÕÒ¾ßÓÐÖ¸¶¨npc idµÄ¶ÓÔ±£¨²»°üÀ¨¶Ó³¤£©
//	·µ»ØÖµ£º¶ÓÔ±ÔÚ m_nMember Êý×éÖÐµÄÎ»ÖÃ
//---------------------------------------------------------------------------
BOOL		KChatRoom::CheckMemberName(const char *lpszName)
{
	for (int i = 0; i <	MAX_CHATROOM_MEMBER; i++)
	{
		if (m_nMember[i] && (strcmp(Npc[Player[m_nMember[i]].m_nIndex].Name,lpszName) == 0))
			return TRUE;
	}
	return FALSE;
}

BOOL		KChatRoom::OpenGame(int ntype, int n)
{
	for (int i = 0; i <	MAX_CHATROOM_MEMBER; i++)
	{
		if(m_nMember[i])
		{
			if (m_nMember[i] == m_nRoomHost)
			{
				//m_nMemParam[i] = 0;
				m_nMemParam[i] = KSG_StringSetValue(4, m_nMemParam[i], 1, ntype);
				m_nMemParam[i] = KSG_StringSetValue(4, m_nMemParam[i], 2, n);
			}
		}
	}
	return TRUE;
}

BOOL		KChatRoom::AddParam(const char *lpszName, int ntype, int n)
{
	for (int i = 0; i <	MAX_CHATROOM_MEMBER; i++)
	{
		if (m_nMember[i] && (strcmp(Npc[Player[m_nMember[i]].m_nIndex].Name,lpszName) == 0) && m_nMemParam[i] == 0)
		{
			//m_nMemParam[i] = 0;
			m_nMemParam[i] = KSG_StringSetValue(4, m_nMemParam[i], 1, ntype);
			m_nMemParam[i] = KSG_StringSetValue(4, m_nMemParam[i], 2, n);
			return TRUE;
		}
	}
	return FALSE;
}
//---------------------------------------------------------------------------
//	¹¦ÄÜ£º´´½¨Ò»Ö§¶ÓÎé
//---------------------------------------------------------------------------
BOOL	KChatRoom::CreateChatRoom(int nIdx, const char* szRoomName, int nRoomPw, int nLefttime,BOOL bIsGmRoom)
{
	DeleteRoom();
	m_bIsGmRoom = bIsGmRoom;
	strcpy(m_szRoomHost, Npc[Player[nIdx].m_nIndex].Name);
	strcpy(m_szRoomName, szRoomName);
	m_nRoomLeftTime = nLefttime;
	m_nRoomPw = nRoomPw;
	return TRUE;
}


void	KChatRoom::MsgAll(const char* szMsg)
{
	for (int i = 0; i <	MAX_CHATROOM_MEMBER; i++)
	{
		if (m_nMember[i])
		{
			KPlayerChat::SendSystemInfo(1, m_nMember[i], MESSAGE_ROOM_ANNOUCE_HEAD, (char *) szMsg, strlen(szMsg));
		}
	}
}


void	KChatRoom::Dice(int nIsHight)
{
	int nLeftPoint = 0, nHostIndex;
	for (int i = 0; i <	MAX_CHATROOM_MEMBER; i++)
	{
		if (m_nMember[i])
		{
			if(m_nMember[i] == m_nRoomHost)
			{
				m_nMemParam[i] = KSG_StringSetValue(1, m_nMemParam[i], 9, nIsHight);
				nHostIndex = i;
				continue;
			}
			if(KSG_StringGetValue(4, m_nMemParam[i], 1) > 0)
			{
				if(KSG_StringGetValue(4, m_nMemParam[i], 1)== nIsHight)
				{
					Player[m_nMember[i]].AddExtPoint(KSG_StringGetValue(4, m_nMemParam[i], 2)*2, 1);
					nLeftPoint -= KSG_StringGetValue(4, m_nMemParam[i], 2);
					m_nMemParam[i] = KSG_StringSetValue(4, m_nMemParam[i], 1, 3);
					m_nMemParam[i] = KSG_StringSetValue(4, m_nMemParam[i], 2, KSG_StringGetValue(4, m_nMemParam[i], 2)*2);
				}
				else
				{
					nLeftPoint += KSG_StringGetValue(4, m_nMemParam[i], 2);
					m_nMemParam[i] = KSG_StringSetValue(4, m_nMemParam[i], 1, 4);
					m_nMemParam[i] = KSG_StringSetValue(4, m_nMemParam[i], 2, KSG_StringGetValue(4, m_nMemParam[i], 2));
				}
			}
		}
	}
	SHOW_MSG_SYNC	sMsg;
	sMsg.ProtocolType = s2c_msgshow;
	sMsg.m_wMsgID = enumMSG_ID_CHATROOM_REVERSE;
	sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID) + sizeof(m_szRoomName);
	sMsg.AllocateBuffer(sMsg.m_wLength + 1);
	memcpy(sMsg.m_lpBuf, &sMsg, sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID));
	memcpy((char*)sMsg.m_lpBuf + sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID), m_szRoomName, sizeof(m_szRoomName));
	for (int i = 0; i <	MAX_CHATROOM_MEMBER; i++)
	{
		if (m_nMember[i])
		{
			if(i==nHostIndex)
			{
				Player[m_nRoomHost].AddExtPoint(nLeftPoint+ KSG_StringGetValue(4, m_nMemParam[i], 2),1);
				m_nMemParam[i] = KSG_StringSetValue(4, m_nMemParam[i], 2, nLeftPoint+ KSG_StringGetValue(4, m_nMemParam[i], 2));
			}
			g_pServer->PackDataToClient(Player[m_nMember[i]].m_nNetConnectIdx, sMsg.m_lpBuf, sMsg.m_wLength + 1);
		}
	}
}
//---------------------------------------------------------------------------
//	¹¦ÄÜ£º³õÊ¼»¯
//---------------------------------------------------------------------------
void	KChatRoomSet::Init()
{
	for (int i = 0; i < MAX_CHATROOM; i++)
	{
		g_ChatRoom[i].SetIndex(i);
	}
}

//---------------------------------------------------------------------------
//	¹¦ÄÜ£º²îÕÒ g_ChatRoom ÖÐµÄ¿ÕÎ»
//---------------------------------------------------------------------------
int		KChatRoomSet::FindFree()
{
	for (int i = 0; i < MAX_CHATROOM; i++)
	{
		if (g_ChatRoom[i].m_nRoomLeftTime == -1)
			continue;
		if (g_ChatRoom[i].m_szRoomHost[0] == 0)
			return i;
		if(g_ChatRoom[i].m_szRoomHost[0] && g_ChatRoom[i].m_nRoomLeftTime < KSG_GetCurSec())
		{
			g_ChatRoom[i].RemoveLeftRoom();
			return i;
		}
	}
	return -1;
}

//---------------------------------------------------------------------------
//	¹¦ÄÜ£º´´½¨Ò»Ö§¶ÓÎé
//---------------------------------------------------------------------------
int		KChatRoomSet::CreateChatRoom(int nIdx, const char* szRoomName, int nRoomPw, int nLefttime,BOOL bIsGmRoom)
{
	int nRoomID;
	nRoomID = FindFree();
	if (nRoomID < 0)
		return -3;
	if (!CheckHost(Npc[Player[nIdx].m_nIndex].Name))
		return -2;
	if (FindName(szRoomName) >=0)
		return -1;
	if (g_ChatRoom[nRoomID].CreateChatRoom(nIdx, szRoomName, nRoomPw, nLefttime,bIsGmRoom))
		return 1;

	return -1;
}

BOOL		KChatRoomSet::AddTime(const char* szRoomName, int nLeftTime)
{
	int nRoomID =FindName(szRoomName);
	if(nRoomID >=0)
	{
		g_ChatRoom[nRoomID].m_nRoomLeftTime+= nLeftTime;
		return TRUE;
	}
	return FALSE;
}

BOOL	KChatRoomSet::CheckHost(const char *szRoomHost)
{
	for (int i = 0; i < MAX_CHATROOM; i++)
	{
		if (strcmp(g_ChatRoom[i].m_szRoomHost, szRoomHost)== 0)
			return FALSE;
	}
	return TRUE;
}

int	KChatRoomSet::FindName(const char *lpszName)
{
	for (int i = 0; i < MAX_CHATROOM; i++)
	{
		if (strcmp(g_ChatRoom[i].m_szRoomName, lpszName)== 0)
			return i;
	}
	return -1;
}


BOOL	KChatRoomSet::RequestRoomList(int nPlayerIndex)
{
	for (int i = 0; i < MAX_CHATROOM; i++)
	{
		if(g_ChatRoom[i].m_szRoomName[0])
		{
			PLAYER_REQUEST_CHATROOM_LIST	sList;
			sList.ProtocolType = s2c_requestchatroomlist;
			strcpy(sList.lpszRoomHost, g_ChatRoom[i].m_szRoomHost);
			strcpy(sList.lpszRoomName, g_ChatRoom[i].m_szRoomName);
			sList.byRoomMemNum = g_ChatRoom[i].m_nMemNum;
			if(g_ChatRoom[i].m_nRoomLeftTime == -1)
				sList.nRoomLeftTime = -1;
			else
				sList.nRoomLeftTime = g_ChatRoom[i].m_nRoomLeftTime - KSG_GetCurSec();
			sList.nRoomParam = 0;
			sList.nRoomParam = KSG_StringSetValue(4, sList.nRoomParam, 1, i);
			if(g_ChatRoom[i].m_bIsGmRoom)
				sList.nRoomParam = KSG_StringSetValue(1, sList.nRoomParam, 5, 1);
			if(g_ChatRoom[i].m_nRoomPw != 0)
				sList.nRoomParam = KSG_StringSetValue(1, sList.nRoomParam, 6, 1);
			if(g_ChatRoom[i].m_nRoomDiceTime > 0)
				sList.nRoomParam = KSG_StringSetValue(1, sList.nRoomParam, 7, 1);
			g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sList, sizeof(PLAYER_REQUEST_CHATROOM_LIST));
		}
	}
	return TRUE;
}

BOOL	KChatRoomSet::RequestList(int nPlayerIndex, int nRoomID)
{
	if(g_ChatRoom[nRoomID].m_szRoomName[0])
	{
		PLAYER_REQUEST_LIST pList;
		for(int i = 0;i < MAX_CHATROOM_MEMBER; i++)
		{
			if(g_ChatRoom[nRoomID].m_nMember[i] > 0)
			{
				pList.ProtocolType = s2c_requestmemberlist;
				strcpy(pList.lpszName, Npc[Player[g_ChatRoom[nRoomID].m_nMember[i]].m_nIndex].Name);
				pList.nMemParam = 0;
				if(g_ChatRoom[nRoomID].m_nMember[i]== g_ChatRoom[nRoomID].m_nRoomHost)
				{
					pList.nMemParam = KSG_StringSetValue(4, pList.nMemParam, 1, g_ChatRoom[nRoomID].m_nRoomDiceTime);
					pList.nMemParam = KSG_StringSetValue(4, pList.nMemParam, 2, KSG_StringGetValue(4, g_ChatRoom[nRoomID].m_nMemParam[i], 2));
					pList.nMemParam = KSG_StringSetValue(1, pList.nMemParam, 9, KSG_StringGetValue(1, g_ChatRoom[nRoomID].m_nMemParam[i], 9));
					pList.bIsRoomHost = TRUE;
				}
				else
				{
					pList.nMemParam = KSG_StringSetValue(4, pList.nMemParam, 1, KSG_StringGetValue(4, g_ChatRoom[nRoomID].m_nMemParam[i], 1));
					pList.nMemParam = KSG_StringSetValue(4, pList.nMemParam, 2, KSG_StringGetValue(4, g_ChatRoom[nRoomID].m_nMemParam[i], 2));
					pList.nMemParam = KSG_StringSetValue(1, pList.nMemParam, 9, KSG_StringGetValue(1, g_ChatRoom[nRoomID].m_nMemParam[i], 9));
					pList.bIsRoomHost = FALSE;
				}
				pList.nMemIndex = i;
				g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)&pList, sizeof(PLAYER_REQUEST_LIST));
			}
			if(g_ChatRoom[nRoomID].m_szBlackName[i][0])
			{
				pList.ProtocolType = s2c_requestblacklist;
				strcpy(pList.lpszName, g_ChatRoom[nRoomID].m_szBlackName[i]);
				pList.nMemParam = 0;
				pList.nMemIndex = i;
				pList.bIsRoomHost = FALSE;
				g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)&pList, sizeof(PLAYER_REQUEST_LIST));
			}
		}
	}
	return TRUE;
}

int	KChatRoomSet::JoinRoom(int nPlayerIndex, int nRoomIndex, int nRoomPw)
{
	if(g_ChatRoom[nRoomIndex].m_szRoomName[0])
	{
		if(g_ChatRoom[nRoomIndex].CheckBlackList(Npc[Player[nPlayerIndex].m_nIndex].Name))
		{
			PLAYER_SEND_CHATROOM_STATE	sJoinRoom;
			sJoinRoom.ProtocolType = s2c_chatroomstate;
			sJoinRoom.nId = 0;
			sJoinRoom.m_btMsgId = ChatRoom_Join_Fail2;
			strcpy(sJoinRoom.lpszName, g_ChatRoom[nRoomIndex].m_szRoomName);
			g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sJoinRoom, sizeof(PLAYER_SEND_CHATROOM_STATE));
			return -1;
		}
		if(g_ChatRoom[nRoomIndex].m_nRoomPw == nRoomPw)
		{
			int j = g_ChatRoom[nRoomIndex].FindFree();
			if(j >= 0)
			{
				SHOW_MSG_SYNC	sMsg;
				sMsg.ProtocolType = s2c_msgshow;
				sMsg.m_wMsgID = enumMSG_ID_CHATROOM_JOIN;
				sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID) + sizeof(Npc[Player[nPlayerIndex].m_nIndex].Name);
				sMsg.AllocateBuffer(sMsg.m_wLength + 1);
				memcpy(sMsg.m_lpBuf, &sMsg, sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID));
				memcpy((char*)sMsg.m_lpBuf + sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID), Npc[Player[nPlayerIndex].m_nIndex].Name, sizeof(Npc[Player[nPlayerIndex].m_nIndex].Name));
				for(int k = 0;k < MAX_CHATROOM_MEMBER; k++)
				{
					if (g_ChatRoom[nRoomIndex].m_nMember[k] > 0)
					{
						g_pServer->PackDataToClient(Player[g_ChatRoom[nRoomIndex].m_nMember[k]].m_nNetConnectIdx, sMsg.m_lpBuf, sMsg.m_wLength + 1);
					}
				}
				if(strcmp(Npc[Player[nPlayerIndex].m_nIndex].Name, g_ChatRoom[nRoomIndex].m_szRoomHost)==0)
					g_ChatRoom[nRoomIndex].m_nRoomHost = nPlayerIndex;
				g_ChatRoom[nRoomIndex].m_nMember[j] = nPlayerIndex;
				g_ChatRoom[nRoomIndex].m_nMemNum++;
				return nRoomIndex;
			}
			else
			{
				PLAYER_SEND_CHATROOM_STATE	sJoinRoom;
				sJoinRoom.ProtocolType = s2c_chatroomstate;
				sJoinRoom.nId = 0;
				sJoinRoom.m_btMsgId = ChatRoom_Join_Fail3;
				strcpy(sJoinRoom.lpszName, g_ChatRoom[nRoomIndex].m_szRoomName);
				g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sJoinRoom, sizeof(PLAYER_SEND_CHATROOM_STATE));
			}
		}
		else
		{
			PLAYER_SEND_CHATROOM_STATE	sJoinRoom;
			sJoinRoom.ProtocolType = s2c_chatroomstate;
			sJoinRoom.nId = 0;
			sJoinRoom.m_btMsgId = ChatRoom_Join_Fail1;
			strcpy(sJoinRoom.lpszName, g_ChatRoom[nRoomIndex].m_szRoomName);
			g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sJoinRoom, sizeof(PLAYER_SEND_CHATROOM_STATE));
		}
	}
	return -1;
}

BOOL	KChatRoomSet::LeaveRoom(int nPlayerIndex, int nRoomID)
{
	if(g_ChatRoom[nRoomID].m_szRoomName[0])
	{
		for(int i = 0;i < MAX_CHATROOM_MEMBER; i++)
		{
			if (g_ChatRoom[nRoomID].m_nMember[i] > 0 && g_ChatRoom[nRoomID].m_nMember[i] == nPlayerIndex)
			{
				if(g_ChatRoom[nRoomID].m_nRoomDiceTime > 0)
				{
					if(nPlayerIndex== g_ChatRoom[nRoomID].m_nRoomHost)
					{
						for(int j = 0;j < MAX_CHATROOM_MEMBER; j++)
						{
							if(g_ChatRoom[nRoomID].m_nMember[j]== g_ChatRoom[nRoomID].m_nRoomHost)
							{
								Player[nPlayerIndex].AddExtPoint(KSG_StringGetValue(4, g_ChatRoom[nRoomID].m_nMemParam[j], 2) /2, 1);
							}
							else
							{
								if(KSG_StringGetValue(4, g_ChatRoom[nRoomID].m_nMemParam[j], 1)==1 || 
									KSG_StringGetValue(4, g_ChatRoom[nRoomID].m_nMemParam[j], 1)==2)
								{
									Player[nPlayerIndex].AddExtPoint(KSG_StringGetValue(4, g_ChatRoom[nRoomID].m_nMemParam[j], 2), 1);
								}
							}
							g_ChatRoom[nRoomID].m_nMemParam[j] = 0;
						}
						g_ChatRoom[nRoomID].m_nRoomDiceTime = 0;
					}
					else
					{
						if(KSG_StringGetValue(4, g_ChatRoom[nRoomID].m_nMemParam[i], 1)==1 || 
							KSG_StringGetValue(4, g_ChatRoom[nRoomID].m_nMemParam[i], 1)==2)
						{
							Player[nPlayerIndex].AddExtPoint(KSG_StringGetValue(4, g_ChatRoom[nRoomID].m_nMemParam[i], 2) /2, 1);
						}
						g_ChatRoom[nRoomID].m_nMemParam[i] = 0;
					}
				}
				if(nPlayerIndex== g_ChatRoom[nRoomID].m_nRoomHost)
					g_ChatRoom[nRoomID].m_nRoomHost = 0;
				g_ChatRoom[nRoomID].m_nMember[i] = 0;
				g_ChatRoom[nRoomID].m_nMemParam[i] = 0;
				g_ChatRoom[nRoomID].m_nMemNum--;
				PLAYER_SEND_CHATROOM_STATE	sLeaveRoom;
				sLeaveRoom.ProtocolType = s2c_chatroomstate;
				sLeaveRoom.nId = nRoomID;
				sLeaveRoom.m_btMsgId = ChatRoom_Leave;
				strcpy(sLeaveRoom.lpszName, g_ChatRoom[nRoomID].m_szRoomName);
				g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sLeaveRoom, sizeof(PLAYER_SEND_CHATROOM_STATE));
				break;
			}
		}
		for(int i = 0;i < MAX_CHATROOM_MEMBER; i++)
		{
			if (g_ChatRoom[nRoomID].m_nMember[i] > 0)
			{
				if(g_ChatRoom[nRoomID].m_nMember[i] == nPlayerIndex)
					continue;
				SHOW_MSG_SYNC	sMsg;
				sMsg.ProtocolType = s2c_msgshow;
				sMsg.m_wMsgID = enumMSG_ID_CHATROOM_LEAVE;
				sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID) + sizeof(Npc[Player[nPlayerIndex].m_nIndex].Name);
				sMsg.AllocateBuffer(sMsg.m_wLength + 1);
				memcpy(sMsg.m_lpBuf, &sMsg, sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID));
				memcpy((char*)sMsg.m_lpBuf + sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID), Npc[Player[nPlayerIndex].m_nIndex].Name, sizeof(Npc[Player[nPlayerIndex].m_nIndex].Name));
				g_pServer->PackDataToClient(Player[g_ChatRoom[nRoomID].m_nMember[i]].m_nNetConnectIdx, sMsg.m_lpBuf, sMsg.m_wLength + 1);
			}
		}
	}
	return TRUE;
}

BOOL	KChatRoomSet::DeleteRoom(int nPlayerIndex, int nRoomID)
{
	if(g_ChatRoom[nRoomID].m_szRoomName[0])
	{
		if(nPlayerIndex== g_ChatRoom[nRoomID].m_nRoomHost)
		{
			if(g_ChatRoom[nRoomID].m_nRoomDiceTime > 0)
				return FALSE;

			PLAYER_SEND_CHATROOM_STATE	sDeleteRoom;
			sDeleteRoom.ProtocolType = s2c_chatroomstate;
			sDeleteRoom.nId = nRoomID;
			sDeleteRoom.m_btMsgId = ChatRoom_Delete;
			strcpy(sDeleteRoom.lpszName, g_ChatRoom[nRoomID].m_szRoomName);
			g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sDeleteRoom, sizeof(PLAYER_SEND_CHATROOM_STATE));

			g_ChatRoom[nRoomID].DeleteRoom();
		}
	}
	return TRUE;
}

BOOL	KChatRoomSet::ForceLeaveRoom(int nPlayerIndex, int nRoomID)
{
	if(g_ChatRoom[nRoomID].m_szRoomName[0])
	{
		for(int i = 0;i < MAX_CHATROOM_MEMBER; i++)
		{
			if (g_ChatRoom[nRoomID].m_nMember[i] > 0)
			{
				if(g_ChatRoom[nRoomID].m_nMember[i] == nPlayerIndex)
				{
					g_ChatRoom[nRoomID].m_nMember[i] = 0;
					g_ChatRoom[nRoomID].m_nMemNum--;
					PLAYER_SEND_CHATROOM_STATE	sLeaveRoom;
					sLeaveRoom.ProtocolType = s2c_chatroomstate;
					sLeaveRoom.nId = nRoomID;
					sLeaveRoom.m_btMsgId = ChatRoom_ForceLeave;
					strcpy(sLeaveRoom.lpszName, g_ChatRoom[nRoomID].m_szRoomName);
					g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sLeaveRoom, sizeof(PLAYER_SEND_CHATROOM_STATE));
				}
			}
		}
	}
	return TRUE;
}

BOOL	KChatRoomSet::AddBlackList(int nPlayerIndex, int nRoomID, int nListIndex)
{
	if(g_ChatRoom[nRoomID].m_szRoomName[0] && g_ChatRoom[nRoomID].m_nMember[nListIndex] > 0)
	{
		if(nPlayerIndex== g_ChatRoom[nRoomID].m_nRoomHost)
		{
			if(g_ChatRoom[nRoomID].m_nMember[nListIndex]== g_ChatRoom[nRoomID].m_nRoomHost)
				return FALSE;
			if(g_ChatRoom[nRoomID].CheckMemberName(Npc[Player[g_ChatRoom[nRoomID].m_nMember[nListIndex]].m_nIndex].Name))
			{
				if(Player[g_ChatRoom[nRoomID].m_nMember[nListIndex]].m_nIndex > 0)
				{
					int i= g_ChatRoom[nRoomID].FindFreeBlackList();
					if(i >= 0)
					{
						strcpy(g_ChatRoom[nRoomID].m_szBlackName[i], Npc[Player[g_ChatRoom[nRoomID].m_nMember[nListIndex]].m_nIndex].Name);
						SHOW_MSG_SYNC	sMsg;
						sMsg.ProtocolType = s2c_msgshow;
						sMsg.m_wMsgID = enumMSG_ID_CHATROOM_BEKICK;
						sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID) + sizeof(Npc[Player[g_ChatRoom[nRoomID].m_nMember[nListIndex]].m_nIndex].Name);
						sMsg.AllocateBuffer(sMsg.m_wLength + 1);
						memcpy(sMsg.m_lpBuf, &sMsg, sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID));
						memcpy((char*)sMsg.m_lpBuf + sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID), Npc[Player[g_ChatRoom[nRoomID].m_nMember[nListIndex]].m_nIndex].Name, sizeof(Npc[Player[g_ChatRoom[nRoomID].m_nMember[nListIndex]].m_nIndex].Name));

						for(i = 0;i < MAX_CHATROOM_MEMBER; i++)
						{
							if (g_ChatRoom[nRoomID].m_nMember[i] > 0)
							{
								if(g_ChatRoom[nRoomID].m_nMember[i] == g_ChatRoom[nRoomID].m_nMember[nListIndex] || 
									g_ChatRoom[nRoomID].m_nMember[i] == g_ChatRoom[nRoomID].m_nRoomHost)
									continue;
								g_pServer->PackDataToClient(Player[g_ChatRoom[nRoomID].m_nMember[i]].m_nNetConnectIdx, sMsg.m_lpBuf, sMsg.m_wLength + 1);
							}
						}
						PLAYER_SEND_CHATROOM_STATE	sLeaveRoom;
						sLeaveRoom.ProtocolType = s2c_chatroomstate;
						sLeaveRoom.nId = nRoomID;
						sLeaveRoom.m_btMsgId = ChatRoom_BeKick;
						strcpy(sLeaveRoom.lpszName, g_ChatRoom[nRoomID].m_szRoomName);
						g_pServer->PackDataToClient(Player[g_ChatRoom[nRoomID].m_nMember[nListIndex]].m_nNetConnectIdx, (BYTE*)&sLeaveRoom, sizeof(PLAYER_SEND_CHATROOM_STATE));

						if(g_ChatRoom[nRoomID].m_nRoomDiceTime > 0)
						{
							if(KSG_StringGetValue(4, g_ChatRoom[nRoomID].m_nMemParam[nListIndex], 1)==1 || 
								KSG_StringGetValue(4, g_ChatRoom[nRoomID].m_nMemParam[nListIndex], 1)==2)
							{
								Player[g_ChatRoom[nRoomID].m_nMember[nListIndex]].AddExtPoint(KSG_StringGetValue(4, g_ChatRoom[nRoomID].m_nMemParam[nListIndex], 2), 1);
							}
						}
						Player[g_ChatRoom[nRoomID].m_nMember[nListIndex]].m_cRoom.Release();
						g_ChatRoom[nRoomID].m_nMember[nListIndex] = 0;
						g_ChatRoom[nRoomID].m_nMemParam[nListIndex] = 0;
						g_ChatRoom[nRoomID].m_nMemNum--;

						PLAYER_SEND_CHATROOM_STATE	sUpdate;
						sUpdate.ProtocolType = s2c_chatroomstate;
						sUpdate.nId = nRoomID;
						sUpdate.m_btMsgId = ChatRoom_AddBlackList;
						strcpy(sUpdate.lpszName, g_ChatRoom[nRoomID].m_szRoomName);
						g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sUpdate, sizeof(PLAYER_SEND_CHATROOM_STATE));
						return TRUE;
					}
				}
			}
		}
	}
	PLAYER_SEND_CHATROOM_STATE	sLeaveRoom;
	sLeaveRoom.ProtocolType = s2c_chatroomstate;
	sLeaveRoom.nId = nRoomID;
	sLeaveRoom.m_btMsgId = ChatRoom_AddBlackList_Fail;
	strcpy(sLeaveRoom.lpszName, Npc[Player[g_ChatRoom[nRoomID].m_nMember[nListIndex]].m_nIndex].Name);
	g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sLeaveRoom, sizeof(PLAYER_SEND_CHATROOM_STATE));
	return FALSE;
}

BOOL	KChatRoomSet::RemoveBlackList(int nPlayerIndex, int nRoomID, int nListIndex)
{
	if(g_ChatRoom[nRoomID].m_szRoomName[0] && g_ChatRoom[nRoomID].m_szBlackName[nListIndex][0] > 0)
	{
		if(nPlayerIndex== g_ChatRoom[nRoomID].m_nRoomHost)
		{
			memset(g_ChatRoom[nRoomID].m_szBlackName[nListIndex], 0, 16);
			PLAYER_SEND_CHATROOM_STATE	sUpdate;
			sUpdate.ProtocolType = s2c_chatroomstate;
			sUpdate.nId = nRoomID;
			sUpdate.m_btMsgId = ChatRoom_RemoveBlackList;
			strcpy(sUpdate.lpszName, g_ChatRoom[nRoomID].m_szRoomName);
			g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sUpdate, sizeof(PLAYER_SEND_CHATROOM_STATE));
			return TRUE;
		}
	}
	return FALSE;
}

BOOL	KChatRoomSet::ChangePw(int nPlayerIndex, int nRoomID, int nRoomPw)
{
	if(g_ChatRoom[nRoomID].m_szRoomName[0])
	{
		if(nPlayerIndex== g_ChatRoom[nRoomID].m_nRoomHost)
		{
			g_ChatRoom[nRoomID].m_nRoomPw = nRoomPw;

			PLAYER_SEND_CHATROOM_STATE	sUpdate;
			sUpdate.ProtocolType = s2c_chatroomstate;
			sUpdate.nId = nRoomID;
			sUpdate.m_btMsgId = ChatRoom_ChangePw;
			strcpy(sUpdate.lpszName, g_ChatRoom[nRoomID].m_szRoomName);
			g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sUpdate, sizeof(PLAYER_SEND_CHATROOM_STATE));
			return TRUE;
		}
	}
	return FALSE;
}

#define INTERVAL_TIME 120
#define TAX_RATE 5
BOOL	KChatRoomSet::OpenGame(int nPlayerIndex, int nRoomID, int n)
{
	if(g_ChatRoom[nRoomID].m_szRoomName[0])
	{
		if(nPlayerIndex== g_ChatRoom[nRoomID].m_nRoomHost && g_ChatRoom[nRoomID].m_nRoomDiceTime == 0)
		{
			char szMsg[128];
			if(n >0 && Player[nPlayerIndex].GetExtPoint() > (n* MAX_CHATROOM_MEMBER + (n *TAX_RATE / MAX_PERCENT)))
			{
				SHOW_MSG_SYNC	sMsg;
				sMsg.ProtocolType = s2c_msgshow;
				sMsg.m_wMsgID = enumMSG_ID_CHATROOM_OPENGAME;
				sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
				sMsg.m_lpBuf = (std::unique_ptr<BYTE[]>*)n;
				for(int i =0; i < MAX_CHATROOM_MEMBER; i++)
				{
					g_ChatRoom[nRoomID].m_nMemParam[i] = 0;
					if(g_ChatRoom[nRoomID].m_nMember[i])
					{
						g_pServer->PackDataToClient(Player[g_ChatRoom[nRoomID].m_nMember[i]].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
					}
				}
				sMsg.m_lpBuf = 0;

				g_ChatRoom[nRoomID].m_nRoomDiceTime = INTERVAL_TIME * GAME_FPS;
				g_ChatRoom[nRoomID].m_nRoomDiceMax = n;
				Player[nPlayerIndex].PayExtPoint(n* MAX_CHATROOM_MEMBER + (n* MAX_CHATROOM_MEMBER *TAX_RATE / MAX_PERCENT));
				g_ChatRoom[nRoomID].OpenGame(0,n* MAX_CHATROOM_MEMBER);
				sprintf(szMsg, "B¾t ®Çu l­ît ch¬i míi thµnh c«ng, khÊu trõ %d ®ång vµ %d(5%%) ®ång phÝ tæn", n* MAX_CHATROOM_MEMBER, (n* MAX_CHATROOM_MEMBER *TAX_RATE / MAX_PERCENT));
			}
			else
				sprintf(szMsg, "Kh©u trõ thÊt b¹i, kh«ng thÓ më trß ch¬i");
			Player[nPlayerIndex].SendSystemMessage(MESSAGE_SYSTEM_ANNOUCE_HEAD, szMsg);
		}
	}
	return TRUE;
}

BOOL	KChatRoomSet::Hight(int nPlayerIndex, int nRoomID, int n)
{
	if(g_ChatRoom[nRoomID].m_szRoomName[0])
	{
		if(nPlayerIndex!= g_ChatRoom[nRoomID].m_nRoomHost && g_ChatRoom[nRoomID].m_nRoomDiceTime > 0)
		{
			if(n >0)
			{
				char szMsg[128];
				if (Player[nPlayerIndex].GetExtPoint() > n)
				{
					if (n <= g_ChatRoom[nRoomID].m_nRoomDiceMax)
					{
						if(g_ChatRoom[nRoomID].AddParam(Npc[Player[nPlayerIndex].m_nIndex].Name, 1,n))
						{
							Player[nPlayerIndex].PayExtPoint(n);
							PLAYER_SEND_CHATROOM_STATE	sRequest;
							sRequest.ProtocolType = s2c_chatroomstate;
							sRequest.nId = nRoomID;
							sRequest.m_btMsgId = -1;
							strcpy(sRequest.lpszName, g_ChatRoom[nRoomID].m_szRoomName);
							g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sRequest, sizeof(PLAYER_SEND_CHATROOM_STATE));

							sprintf(szMsg, "B¹n ®Æt %d ®iÓm cho Tµi", n);
						}
						else
							sprintf(szMsg, "Tr­íc ®ã b¹n ®· ®Æt c­îc råi! nÕu ch­a cã thÓ lµ mét lçi, h·y rêi phßng vµ thö l¹i");
					}
					else
						sprintf(szMsg, "Sè ®iÓm b¹n nhËp vµo v­ît qu¸ %d ®iÓm", g_ChatRoom[nRoomID].m_nRoomDiceMax);
				}
				else
					sprintf(szMsg, "Sè ®iÓm b¹n nhËp vµo v­ît qu¸ sè xu b¹n hiÖn cã");

				Player[nPlayerIndex].SendSystemMessage(MESSAGE_SYSTEM_ANNOUCE_HEAD, szMsg);
			}
		}
	}
	return TRUE;
}

BOOL	KChatRoomSet::Low(int nPlayerIndex, int nRoomID, int n)
{
	if(g_ChatRoom[nRoomID].m_szRoomName[0])
	{
		if(nPlayerIndex!= g_ChatRoom[nRoomID].m_nRoomHost && g_ChatRoom[nRoomID].m_nRoomDiceTime > 0)
		{
			if(n >0)
			{
				char szMsg[128];
				if (Player[nPlayerIndex].GetExtPoint() > n)
				{
					if (n <= g_ChatRoom[nRoomID].m_nRoomDiceMax)
					{
						if(g_ChatRoom[nRoomID].AddParam(Npc[Player[nPlayerIndex].m_nIndex].Name, 2,n))
						{
							Player[nPlayerIndex].PayExtPoint(n);
							PLAYER_SEND_CHATROOM_STATE	sRequest;
							sRequest.ProtocolType = s2c_chatroomstate;
							sRequest.nId = nRoomID;
							sRequest.m_btMsgId = -1;
							strcpy(sRequest.lpszName, g_ChatRoom[nRoomID].m_szRoomName);
							g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sRequest, sizeof(PLAYER_SEND_CHATROOM_STATE));

							sprintf(szMsg, "B¹n ®Æt %d ®iÓm cho XØu", n);
						}
						else
							sprintf(szMsg, "Tr­íc ®ã b¹n ®· ®Æt c­îc råi! nÕu ch­a cã thÓ lµ mét lçi, h·y rêi phßng vµ thö l¹i");
					}
					else
						sprintf(szMsg, "Sè ®iÓm b¹n nhËp vµo v­ît qu¸ %d ®iÓm", g_ChatRoom[nRoomID].m_nRoomDiceMax);
				}
				else
					sprintf(szMsg, "Sè ®iÓm b¹n nhËp vµo v­ît qu¸ sè xu b¹n hiÖn cã");

				Player[nPlayerIndex].SendSystemMessage(MESSAGE_SYSTEM_ANNOUCE_HEAD, szMsg);
			}
		}
	}
	return TRUE;
}
#endif






















