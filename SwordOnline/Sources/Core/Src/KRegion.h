#ifndef	KRegionH
#define	KRegionH

//-----------------------------------------------------------------------
#include "KEngine.h"
#include "KWorldMsg.h"
//-----------------------------------------------------------------------
#ifdef _SERVER
#define	MAX_BROADCAST_COUNT		100
#else
#define	MAX_REGION		9
#endif

#define	REGION_GRID_WIDTH	16
#define	REGION_GRID_HEIGHT	32

enum MOVE_OBJ_KIND
{
	obj_npc,
	obj_object,
	obj_obstacle,
	obj_missle,
};
#ifndef TOOLVERSION
class KRegion
#else
class CORE_API KRegion
#endif
{
	friend class	KSubWorld;
public:
	int			m_nIndex;							// µØÍ¼Ë÷Òý
	int			m_RegionID;
	KList		m_NpcList;							// ÈËÎïÁÐ±í
	KList		m_ObjList;							// Îï¼þÁÐ±í
	KList		m_MissleList;						// ×Óµ¯ÁÐ±í
	KList		m_PlayerList;						// Íæ¼ÒÁÐ±í
	int			m_nConnectRegion[8];				// ÏàÁÚµÄµØÍ¼Ë÷Òý
	int			m_nConRegionID[8];					// ÏàÁÚµÄµØÍ¼ID
	KRegion*	m_pConRegion[8];
	int			m_nRegionX;							// ÔÚÊÀ½çÖÐµÄÎ»ÖÃX£¨ÏóËØµã£©
	int			m_nRegionY;							// ÔÚÊÀ½çÖÐµÄÎ»ÖÃY£¨ÏóËØµã£©
	int			m_nWidth;
	int			m_nHeight;
private:
#ifdef _SERVER
	long		m_Obstacle[REGION_GRID_WIDTH][REGION_GRID_HEIGHT];	// µØÍ¼ÕÏ°­ÐÅÏ¢±í
	DWORD		m_dwTrap[REGION_GRID_WIDTH][REGION_GRID_HEIGHT];	// µØÍ¼trapÐÅÏ¢±í
#endif
	int			m_nNpcSyncCounter;					// Í¬²½¼ÆÊýÆ÷
	int			m_nObjSyncCounter;
	int			m_nActive;							// ÊÇ·ñ¼¤»î£¨ÊÇ·ñÓÐÍæ¼ÒÔÚ¸½½ü£©
	BYTE*		m_pNpcRef;							// Ã¿¸ö¸ñ×ÓÉÏµÄNPCÊýÄ¿
	BYTE*		m_pObjRef;							// Ã¿¸ö¸ñ×ÓÉÏµÄOBJÊýÄ¿
	BYTE*		m_pObstacleRef;							// Ã¿¸ö¸ñ×ÓÉÏµÄOBJÊýÄ¿
	BYTE*		m_pMslRef;							// Ã¿¸ö¸ñ×ÓÉÏµÄMISSLEÊýÄ¿
public:
	KRegion();
	~KRegion();
	BOOL		Init(int nWidth, int nHeight);
	BOOL		Load(int nX, int nY);
#ifdef _SERVER
	// ÔØÈë·þÎñÆ÷¶ËµØÍ¼ÉÏ±¾region µÄ objectÊý¾Ý£¨°üÀ¨npc¡¢trap¡¢boxµÈ£©
	BOOL		LoadObject(int nSubWorld, int nX, int nY);
	// ÔØÈë·þÎñÆ÷¶ËµØÍ¼ÉÏ±¾ region µÄÕÏ°­Êý¾Ý
	BOOL		LoadServerObstacle(KFile*pFile, DWORD dwDataSize);
	// ÔØÈë·þÎñÆ÷¶ËµØÍ¼ÉÏ±¾ region µÄ trap Êý¾Ý
	BOOL		LoadServerTrap(KFile*pFile, DWORD dwDataSize);
	// ÔØÈë·þÎñÆ÷¶ËµØÍ¼ÉÏ±¾ region µÄ npc Êý¾Ý
	BOOL		LoadServerNpc(int nSubWorld, KFile*pFile, DWORD dwDataSize);
	// ÔØÈë·þÎñÆ÷¶ËµØÍ¼ÉÏ±¾ region µÄ obj Êý¾Ý
	BOOL		LoadServerObj(int nSubWorld, KFile*pFile, DWORD dwDataSize);
#endif

#ifndef _SERVER
	// ÔØÈë¿Í»§¶ËµØÍ¼ÉÏ±¾region µÄ objectÊý¾Ý£¨°üÀ¨npc¡¢boxµÈ£©
	// Èç¹û bLoadNpcFlag == TRUE ÐèÒªÔØÈë clientonly npc else ²»ÔØÈë
	BOOL		LoadObject(int nSubWorld, int nX, int nY, char *lpszPath);
	// ÔØÈë¿Í»§¶ËµØÍ¼ÉÏ±¾ region µÄ clientonlynpc Êý¾Ý
	BOOL		LoadClientNpc(KPakFile *pFile, DWORD dwDataSize);
	// ÔØÈë¿Í»§¶ËµØÍ¼ÉÏ±¾ region µÄ clientonlyobj Êý¾Ý
	BOOL		LoadClientObj(KPakFile *pFile, DWORD dwDataSize);
	// ÔØÈëÕÏ°­Êý¾Ý¸øÐ¡µØÍ¼
	static void		LoadLittleMapData(int nX, int nY, char *lpszPath, BYTE *lpbtObstacle);
#endif
	void		Close();
	void		Activate();
	BYTE		GetBarrier(int MapX, int MapY, int nDx, int nDy);	//	µØÍ¼¸ß¶È

	// °´ ÏñËØµã×ø±ê * 1024 µÄ¾«¶ÈÅÐ¶ÏÄ³¸öÎ»ÖÃÊÇ·ñÕÏ°­
	// ²ÎÊý£ºnGridX nGirdY £º±¾region¸ñ×Ó×ø±ê
	// ²ÎÊý£ºnOffX nOffY £º¸ñ×ÓÄÚµÄÆ«ÒÆÁ¿(ÏñËØµã * 1024 ¾«¶È)
	// ²ÎÊý£ºbCheckNpc £ºÊÇ·ñÅÐ¶ÏnpcÐÎ³ÉµÄÕÏ°­
	// ·µ»ØÖµ£ºÕÏ°­ÀàÐÍ(if ÀàÐÍ == Obstacle_NULL ÎÞÕÏ°­)
	BYTE		GetBarrierMin(int nGridX, int nGridY, int nOffX, int nOffY, BOOL bCheckNpc, BOOL bCheckObs);

	DWORD		GetTrap(int MapX, int MapY);						//	µÃµ½Trap±àºÅ
	inline BOOL		IsActive() 
	{
#ifdef _SERVER
		return m_nActive; 
#else
		return TRUE;
#endif
	};
	int			GetRef(int nMapX, int nMapY, MOVE_OBJ_KIND nType);
	BOOL		AddRef(int nMapX, int nMapY, MOVE_OBJ_KIND nType);
	BOOL		DecRef(int nMapX, int nMapY, MOVE_OBJ_KIND nType);
	int			FindNpc(int nMapX, int nMapY, int nNpcIdx, int nRelation);
	int			FindNpcNear(int nX1, int nY1);
	int			FindObject(int nMapX, int nMapY, bool bAutoFind = false);
	int			FindObjectNear(int nX1, int nY1);
	int			FindObject(int nObjID);
	void*		GetObjNode(int nIdx);
	int			SearchNpc(DWORD dwNpcID);		// Ñ°ÕÒ±¾ÇøÓòÄÚÊÇ·ñÓÐÄ³¸öÖ¸¶¨ id µÄ npc (zroc add)
	int			SearchNpcSettingIdx(int  nNpcSettingIdx);
	int			SearchNpcName(const char* szName);
#ifdef _SERVER
	void		SendSyncData(int nClient);
	void		BroadCast(const void *pBuffer, DWORD dwSize, int &nMaxCount, int nX, int nY);
	int			FindPlayer(DWORD dwId);
	void		SetTrap(DWORD dwTrapId, int nMapX, int nMapY);
#endif

#ifndef _SERVER
	void		Paint();
#endif
	void		AddNpc(int nIdx);
	void		RemoveNpc(int nIdx);
	void		AddMissle(int nIdx);
	void		RemoveMissle(int nIdx);
	void		AddObj(int nIdx);
	void		RemoveObj(int nIdx);
	BOOL		AddPlayer(int nIdx);
	BOOL		RemovePlayer(int nIdx);

};

//--------------------------------------------------------------------------
//	Find Npc
//--------------------------------------------------------------------------
inline int KRegion::FindNpc(int nMapX, int nMapY, int nNpcIdx, int nRelation)
{
	if (m_pNpcRef[nMapY * m_nWidth + nMapX] == 0)
		return 0;

	KIndexNode *pNode = NULL;
	
	pNode = (KIndexNode *)m_NpcList.GetHead();
	
	while(pNode)
	{
		if (Npc[pNode->m_nIndex].m_MapX == nMapX && Npc[pNode->m_nIndex].m_MapY == nMapY)
		{
			if (NpcSet.GetRelation(nNpcIdx, pNode->m_nIndex) & nRelation)
			{
				return pNode->m_nIndex;
			}
		}
		pNode = (KIndexNode *)pNode->GetNext();
	}	
	return 0;
}
#endif
