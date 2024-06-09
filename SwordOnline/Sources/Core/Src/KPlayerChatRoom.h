#ifndef KPLAYERCHATROOM_H
#define KPLAYERCHATROOM_H

#include	"KPlayerDef.h"

#define		MAX_CHATROOM			20
#define		MAX_CHATROOM_MEMBER		50		// 最大队员数量(不包括队长)

#ifdef _SERVER
class KPlayerChatRoom	// 服务器端玩家的组队信息
{
	friend class KPlayer;
public:
	int		m_nID;				// 如果已经组队，队伍的 id
	int		m_nFlag;
	int		m_nFigure;			// 如果已经组队，player 的身份：TEAM_CAPTAIN TEAM_MEMBER

private:
	int		m_nPlayerIndex;
public:
	KPlayerChatRoom() {m_nPlayerIndex = 0;};
	void	Active();
	void	Init(int nPlayerIdx);
	void	Release();
	BOOL	CreateChatRoom(char* szRoomName, int nRoomPw, int nLefttime, BOOL bIsGmRoom);
	BOOL	AddTime(const char* lpszName, int nLefttime);
	BOOL	RequestList();
	BOOL	JoinRoom(int nRoomIndex, int nRoomPw);
	BOOL	LeaveRoom();
	BOOL	DeleteRoom();
	BOOL	ForceLeaveRoom();
	BOOL	Roomcommand(int n);
};
#endif

#ifndef _SERVER
class KPlayerChatRoom	// 客户端玩家的组队信息
{
public:
	char	m_szRoomName[10];
	int		m_nID;
	int		m_nFlag;
	int		m_nFigure;

public:
	KPlayerChatRoom();
	void	Release();

	void	JoinRoom(const char* szRoomName, int nId, CHATROOM_FIGURE eFigure);								// 更新界面显示
};
#endif

// 此队伍是否为空通过判断队长 id 来决定，当为 -1 时为空（每个队伍必定有一个队长）
#ifdef _SERVER
class KChatRoom
{
private:
	int		m_nIndex;									// 本 Team 在 g_ChatRoom 中的位置
public:
	BOOL	m_bIsGmRoom;
	char	m_szRoomHost[16];
	int		m_nRoomHost;
	char	m_szRoomName[10];
	int		m_nRoomLeftTime;
	int		m_nRoomDiceTime;
	int		m_nRoomDiceMax;
	int		m_nRoomPw;
	int		m_nMember[MAX_CHATROOM_MEMBER];					// 所有队员 id ，服务器端用 player index ，客户端用 npc id ，-1 为空
	int		m_nMemParam[MAX_CHATROOM_MEMBER];
	int		m_nMemNum;									// 已有队员数量(不包括队长)
	char	m_szBlackName[MAX_CHATROOM_MEMBER][16];

public:
	KChatRoom();											// 构造函数
	void	Release();									// 清空
	void	RemoveLeftRoom();									// 清空
	void	DeleteRoom();									// 清空
	void	SetIndex(int nIndex);						// 设定 Team 在 g_ChatRoom 中的位置
	int		FindFree();									// 寻找队员空位
	int		FindFreeBlackList();									// 寻找队员空位
	BOOL	OpenGame(int ntype, int n);
	BOOL	AddParam(const char* lpszName, int ntype, int n);
	BOOL	CheckMemberName(const char *lpszName);				// 寻找具有指定npc id的队员（不包括队长）
	BOOL	CheckBlackList(const char *lpszName);
	BOOL	CreateChatRoom(int nIdx, const char* szRoomName, int nRoomPw, int nLefttime,BOOL bIsGmRoom);// 创建一支队伍
	void	MsgAll(const char* szMsg);
	void	Dice(int nIsHight);
};

extern	KChatRoom	g_ChatRoom[MAX_CHATROOM];


class KChatRoomSet
{
public:
	void	Init();										// 初始化
	int		CreateChatRoom(int nIdx, const char* szRoomName, int nRoomPw, int nLefttime,BOOL bIsGmRoom);	// 创建一支队伍
	BOOL	AddTime(const char* lpszName, int nLefttime);
	BOOL	RequestRoomList(int nPlayerIndex);
	BOOL	RequestList(int nPlayerIndex, int nRoomID);
	int		JoinRoom(int nPlayerIndex, int nRoomIndex, int nRoomPw);
	BOOL	LeaveRoom(int nPlayerIndex, int nRoomID);
	BOOL	DeleteRoom(int nPlayerIndex, int nRoomID);
	BOOL	ForceLeaveRoom(int nPlayerIndex, int nRoomID);
	BOOL	AddBlackList(int nPlayerIndex, int nRoomID, int nListIndex);
	BOOL	RemoveBlackList(int nPlayerIndex, int nRoomID, int nListIndex);
	BOOL	ChangePw(int nPlayerIndex, int nRoomID, int nRoomPw);
	BOOL	OpenGame(int nPlayerIndex, int nRoomID, int n);
	BOOL	Hight(int nPlayerIndex, int nRoomID, int n);
	BOOL	Low(int nPlayerIndex, int nRoomID, int n);
private:
	int		FindFree();
	BOOL	CheckHost(const char *lpszHost);					// 判断队名是否可用
	int		FindName(const char *lpszName);					// 判断队名是否可用
};

extern	KChatRoomSet	g_ChatRoomSet;

#endif

#endif
