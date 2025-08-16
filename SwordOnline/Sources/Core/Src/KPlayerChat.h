//---------------------------------------------------------------------------
// Sword3 Engine (c) 2002 by Kingsoft
//
// File:	KPlayerChat.h
// Date:	2002.10.05
// Code:	边城浪子
// Desc:	PlayerChat Class
//---------------------------------------------------------------------------

#ifndef KPLAYERCHAT_H
#define KPLAYERCHAT_H

#include	"GameDataDef.h"
#ifndef _SERVER
#include	"KNode.h"
#include	"KList.h"
#endif
#include	<string>
#ifdef _SERVER

// 每个聊天好友的数据
class CChatFriend : public KNode
{
public:
	DWORD			m_dwID;					// 好友ID
	int				m_nPlayerIdx;			// 好友在 player 数组中的位置
	char			m_szName[32];			// 好友名字

public:
	CChatFriend() {Release();};
	void			Release() {m_dwID = 0; memset(m_szName, 0, 32);};
};
#endif

#ifdef _SERVER
#include	<list>
#endif

class KPlayerChat
{
public:
	void			Release();
public:
#ifdef _SERVER
	static	void	SendSystemInfo(int nType, int nTargetIdx, char *lpszSendName, char *lpszSentence, int nSentenceLength, int nChannedID = -1);
	static	void	SendGlobalSystemInfo(char *lpszSendName, char *lpszSentence, int nSentenceLength);
	typedef std::list<std::string>	STRINGLIST;
	static	void	MakeBrother(const STRINGLIST& brothers);
	static	void	MakeEnemy(char* szPlayer, char* szEnemy);
	static	void	MakeMate(char* szPlayer, char* szMate);
	static	void	SendInfoToGM(char *lpszAccName, char *lpszRoleName, char *lpszSentence, int nSentenceLength);
	static	void	SendInfoToIP(DWORD nIP, DWORD nID, char *lpszAccName, char *lpszRoleName, char *lpszSentence, int nSentenceLength);
#endif
};

#endif

