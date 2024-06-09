#ifndef	KSubWorldSetH
#define	KSubWorldSetH

#include "KTimer.h"
#ifndef _SERVER
#include "KMapMusic.h"
#else
#include "KSubWorld.h"

#endif

struct MAPLIST_INFO
{
	char szName[128];
	int nKind;
};

class CORE_API KSubWorldSet
{
public:
	int		m_nLoopRate;		// 循环帧数
	int		m_MapListCount;
	MAPLIST_INFO*	m_sMapListInfo;
	
#ifndef _SERVER
	KMapMusic	m_cMusic;
#endif
private:
	KTimer	m_Timer;			// 计时器
	int		m_nGameVersion;
#ifndef _SERVER
	DWORD	m_dwPing;
#endif
public:
	KSubWorldSet();
	~KSubWorldSet();
	BOOL	LoadFile();
	int		SearchWorld(DWORD dwID);
	void	MainLoop();
	void	MessageLoop();
	BOOL	SendMessage(int nSubWorldID, DWORD	dwMsgType, int nParam1 = 0, int nParam2 = 0, int nParam3 = 0);
	int		GetGameTime(){return m_nLoopRate;};
	int		GetGameVersion() { return m_nGameVersion; }
	void	Close();
	BOOL	Load(LPSTR szFileName);
// Server上才有的几个方法
#ifdef _SERVER
	void	GetRevivalPosFromId(DWORD dwSubWorldId, int nRevivalId, POINT* pPos);
#endif
// Client上才有的几个方法
#ifndef _SERVER
	void	SetPing(DWORD dwTimer) { m_dwPing = dwTimer; }
	DWORD	GetPing() { return m_dwPing; }
	void	Paint();
#endif
};
extern CORE_API KSubWorldSet g_SubWorldSet;
#endif
