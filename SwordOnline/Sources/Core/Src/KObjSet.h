//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KObjSet.h
// Date:	2002.01.06
// Code:	边城浪子
// Desc:	Obj Class
//---------------------------------------------------------------------------
#ifndef	KObjSetH
#define	KObjSetH

#include "KObj.h"
#include "KLinkArray.h"

#define		MAX_OBJ_NAME_COLOR		16

typedef struct
{
	char	m_szCopyRight[31];
	BYTE	m_btVersion;
	DWORD	m_dwNum;
} KObjMapDataHead;

typedef struct
{
	int		m_nTemplateID;
	int		m_nState;
	int		m_nDir;
	int		m_nPos[3];
	DWORD	m_dwScriptLength;
	char	m_szScript[80];
} KObjMapData;

typedef struct
{
	int		m_nItemID;
	int		m_nMoneyNum;
	int		m_nItemWidth;
	int		m_nItemHeight;
	int		m_nColorID;
	int		m_nGenre;
	int		m_nDetailType;
	int		m_nMovieFlag;
	int		m_nSoundFlag;
	BOOL	m_bOverLook;
	char	m_szName[FILE_NAME_LENGTH];
} KObjItemInfo;

class KObjSet
{
public:
	KTabFile	m_cTabFile;
	KTabFile	m_cMoneyFile;
	int			m_nObjID;
#ifndef _SERVER
	DWORD		m_dwNameColor[MAX_OBJ_NAME_COLOR];
#endif
private:
	KLinkArray	m_UseIdx;
	KLinkArray	m_FreeIdx;
public:
	KObjSet();
	~KObjSet();
	BOOL	Init();
	int		GetID();
	void	Remove(int nIdx);
#ifdef _SERVER
	// 添加一个obj，返回在obj数组中的位置编号（如果 < 0 ，失败）
	int		Add(int nDataID, KMapPos MapPos, KObjItemInfo sItemInfo);

	// 从obj数据文件中载入相应数据
	int		AddData(int nDataID, KMapPos MapPos, int nMoneyNum = 0, int nItemID = 0, int nItemWidth = 0, int nItemHeight = 0);

	int		AddMoneyObj(KMapPos MapPos, int nMoneyNum);

	// 服务器端载入一个Region的所有Obj
	BOOL	ServerLoadRegionObj(char *lpszMapPath, int nRegionX, int nRegionY, int nSubWorld);

	// 服务器端载入一个Region的所有Obj
	BOOL	ServerLoadRegionObj(int nSubWorld, KFile*pFile, DWORD dwDataSize);

	// 设定obj所带的物件信息，包括物件id，物件在装备栏中的长、宽
	BOOL	SetObjItem(int nObjIndex, int nItemID, int nItemWidth, int nItemHeight);

	BOOL	SyncAdd(int nID, int nClient);

	int		SearchObjAt(KMapPos MapPos);
#endif

#ifndef _SERVER
	int		AddData(int nDataID, int nSubWorld, int nRegion, int nMapX, int nMapY, int nOffX, int nOffY);
	int		ClientAdd(int nID, int nDataID, int nState, int nDir, int nCurFrame, int nXpos, int nYpos, KObjItemInfo sInfo);
	BOOL	ClientLoadRegionObj(char *lpszMapPath, int nRegionX, int nRegionY, int nSubWorld, int nRegion);
	BOOL	ClientLoadRegionObj(KPakFile *pFile, DWORD dwDataSize);
	void	RemoveIfClientOnly(int nIdx);
	int		SearchObjAt(int nX, int nY, int nRange);
	DWORD	GetNameColor(int nColorID);
#endif
	int		FindID(int nID);
	int		FindName(char *lpszObjName);
private:
	int		FindFree();
	int		CheckClientKind(int nKind);
	int		GetDataIDKind(int nDataID);
#ifdef _SERVER
	int		GetMoneyDataId(int nMoney);
#endif
};

extern	KObjSet	ObjSet;
extern	char	g_szObjKind[Obj_Kind_Num][32];

#endif






