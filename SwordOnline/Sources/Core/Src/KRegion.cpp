#include "KCore.h"
#include "KNpc.h"
#include "KNpcSet.h"
#include "KMissleSet.h"
#include "KObjSet.h"
#include "KPlayerSet.h"
#include "KPlayer.h"
#include "KMissle.h"
#include "KObj.h"
#include "KWorldMsg.h"
#include "KSubWorld.h"
#ifdef _SERVER
//#include "KNetServer.h"
//#include "../MultiServer/Heaven/Interface/iServer.h"
#endif
#include "Scene/ObstacleDef.h"
#include "Scene/SceneDataDef.h"
#include "KRegion.h"
#include "MyAssert.H"


KRegion::KRegion()
{
	m_nIndex		= -1;
	m_RegionID		= -1;
	m_nActive		= 0;
	m_nNpcSyncCounter = 0;
	m_nObjSyncCounter = 0;
	m_nWidth		= 0;
	m_nHeight		= 0;
	ZeroMemory(m_nConnectRegion, 8 * sizeof(int));
	m_pNpcRef		= NULL;
	m_pObjRef		= NULL;
	m_pObstacleRef	= NULL;
	m_pMslRef		= NULL;
#ifdef _SERVER
	memset(m_dwTrap, 0, sizeof(m_dwTrap));
#endif
}

KRegion::~KRegion()
{
	if (m_pMslRef)
		delete [] m_pMslRef;
	if (m_pNpcRef)
		delete [] m_pNpcRef;
	if (m_pObjRef)
		delete [] m_pObjRef;
	if(m_pObstacleRef)
		delete [] m_pObstacleRef;
}

BOOL KRegion::Init(int nWidth, int nHeight)
{
	m_nWidth	= nWidth;
	m_nHeight	= nHeight;

	if (!m_pNpcRef)
		m_pNpcRef = new BYTE[nWidth * nHeight];
	if (!m_pNpcRef)
		return FALSE;
	ZeroMemory(m_pNpcRef, nWidth * nHeight);

	if (!m_pObjRef)
		m_pObjRef = new BYTE[nWidth * nHeight];
	if (!m_pObjRef)
		return FALSE;
	ZeroMemory(m_pObjRef, nWidth * nHeight);

	if (!m_pObstacleRef)
		m_pObstacleRef = new BYTE[nWidth * nHeight];
	if (!m_pObstacleRef)
		return FALSE;
	ZeroMemory(m_pObstacleRef, nWidth * nHeight);

	if (!m_pMslRef)
		m_pMslRef = new BYTE[nWidth * nHeight];
	if (!m_pMslRef)
		return FALSE;
	ZeroMemory(m_pMslRef, nWidth * nHeight);
	
	return TRUE;
}

BOOL KRegion::Load(int nX, int nY)
{
#ifndef _SERVER
	Close();
#endif

	m_RegionID = MAKELONG(nX, nY);

	m_nRegionX = nX * 512;
	m_nRegionY = nY * 1024;

	// ÏÂ·½
	m_nConRegionID[0] = MAKELONG(nX, nY + 1);
	// ×óÏÂ·½
	m_nConRegionID[1] = MAKELONG(nX - 1, nY + 1);
	// ×ó·½
	m_nConRegionID[2] = MAKELONG(nX - 1, nY);
	// ×óÉÏ·½
	m_nConRegionID[3] = MAKELONG(nX - 1, nY - 1);
	// ÉÏ·½
	m_nConRegionID[4] = MAKELONG(nX, nY - 1);
	// ÓÒÉÏ·½
	m_nConRegionID[5] = MAKELONG(nX + 1, nY - 1);
	// ÓÒ·½
	m_nConRegionID[6] = MAKELONG(nX + 1, nY);
	// ÓÒÏÂ·½
	m_nConRegionID[7] = MAKELONG(nX + 1, nY + 1);

	return TRUE;
}

#ifdef _SERVER
//----------------------------------------------------------------------
//	¹¦ÄÜ£ºÔØÈë·þÎñÆ÷¶ËµØÍ¼ÉÏ±¾region µÄ objectÊý¾Ý£¨°üÀ¨npc¡¢trap¡¢boxµÈ£©
//	×¢Òâ£ºÊ¹ÓÃ´Ëº¯ÊýÖ®Ç°±ØÐë±£Ö¤µ±Ç°Â·¾¶ÊÇ±¾µØÍ¼Â·¾¶
//----------------------------------------------------------------------
BOOL KRegion::LoadObject(int nSubWorld, int nX, int nY)
{

	KFile	cData;
	char		szFilePath[80];
	char		szFile[80];

	g_GetFilePath(szFile);

	sprintf(szFilePath, "\\%sv_%03d", szFile, nY);
	//g_SetFilePath(szFilePath);

	//sprintf(szFile, "%s\\%03d_%s", szFilePath, nX, REGION_COMBIN_FILE_NAME_SERVER);
	sprintf(szFile, "%s\\%03d_%s", szFilePath, nX, REGION_COMBIN_FILE_NAME_CLIENT);
	if (cData.Open(szFile))
	{
		DWORD	dwHeadSize;
		DWORD	dwMaxElemFile = 0;
		KCombinFileSection	sElemFile[REGION_ELEM_FILE_COUNT];

		if (cData.Size() < sizeof(DWORD) + sizeof(KCombinFileSection) * REGION_ELEM_FILE_COUNT)
		{
			//ZeroMemory(m_Obstacle, sizeof(m_Obstacle));
			goto gotoCLOSE;
		}
		cData.Read(&dwMaxElemFile, sizeof(DWORD));
		if (dwMaxElemFile > REGION_ELEM_FILE_COUNT)
		{
			cData.Read(sElemFile, sizeof(sElemFile));
			cData.Seek(sizeof(KCombinFileSection) * (dwMaxElemFile - REGION_ELEM_FILE_COUNT), FILE_CURRENT);
		}
		else
		{
			cData.Read(sElemFile, sizeof(sElemFile));
		}
		dwHeadSize = sizeof(DWORD) + sizeof(KCombinFileSection) * dwMaxElemFile;

		// ÔØÈëÕÏ°­Êý¾Ý
		cData.Seek(dwHeadSize + sElemFile[REGION_OBSTACLE_FILE_INDEX].uOffset, FILE_BEGIN);
		LoadServerObstacle(&cData, sElemFile[REGION_OBSTACLE_FILE_INDEX].uLength);

		// ÔØÈëtrapÐÅÏ¢
		cData.Seek(dwHeadSize + sElemFile[REGION_TRAP_FILE_INDEX].uOffset, FILE_BEGIN);
//		LoadServerTrap(&cData, sElemFile[REGION_TRAP_FILE_INDEX].uLength);

		// Phuc tat load npc mac dinh
		cData.Seek(dwHeadSize + sElemFile[REGION_NPC_FILE_INDEX].uOffset, FILE_BEGIN);
		LoadServerNpc(nSubWorld, &cData, sElemFile[REGION_NPC_FILE_INDEX].uLength);
		// Phuc tat load npc mac dinh

		// ÔØÈëobjÊý¾Ý
		cData.Seek(dwHeadSize + sElemFile[REGION_OBJ_FILE_INDEX].uOffset, FILE_BEGIN);
		LoadServerObj(nSubWorld, &cData, sElemFile[REGION_OBJ_FILE_INDEX].uLength);

gotoCLOSE:
		cData.Close();
	}
	else
	{
		KFile	cObstacle;

//		g_SetFilePath(szFilePath);
		sprintf(szFile, "%s\\%03d_%s", szFilePath, nX, REGION_OBSTACLE_FILE);
		if (cObstacle.Open(szFile))
		{
			if (cObstacle.Size() != sizeof(m_Obstacle))
				ZeroMemory(m_Obstacle, sizeof(m_Obstacle));
			else
				cObstacle.Read((LPVOID)m_Obstacle, sizeof(m_Obstacle));
			cObstacle.Close();
		}
		else
		{
		
		}

/*		KPakFile	cTrapData, cNpcData;

		// ÔØÈëtrapÐÅÏ¢
		KTrapFileHead	sTrapFileHead;
		KSPTrap			sTrapCell;
		int				i, j;

//		g_SetFilePath(szFilePath);
		sprintf(szFile, "%s\\%03d_%s", szFilePath, nX, REGION_TRAP_FILE);
		memset(this->m_dwTrap, 0, sizeof(m_dwTrap));
		if (!cTrapData.Open(szFile))
			goto TRAP_CLOSE;
		if (cTrapData.Size() < sizeof(KTrapFileHead))
			goto TRAP_CLOSE;
		cTrapData.Read(&sTrapFileHead, sizeof(KTrapFileHead));
		if (sTrapFileHead.uNumTrap * sizeof(KSPTrap) + sizeof(KTrapFileHead) != cTrapData.Size())
			goto TRAP_CLOSE;
		for (i = 0; i < sTrapFileHead.uNumTrap; i++)
		{
			cTrapData.Read(&sTrapCell, sizeof(KSPTrap));
			if (sTrapCell.cY >= REGION_GRID_HEIGHT || sTrapCell.cX + sTrapCell.cNumCell - 1 >= REGION_GRID_WIDTH)
				continue;
			for (j = 0; j < sTrapCell.cNumCell; j++)
			{
				m_dwTrap[sTrapCell.cX + j][sTrapCell.cY] = sTrapCell.uTrapId;
			}
		}

TRAP_CLOSE:
		cTrapData.Close();

		// ÔØÈënpcÊý¾Ý
		KNpcFileHead	sNpcFileHead;
		KSPNpc			sNpcCell;

//		g_SetFilePath(szFilePath);
		sprintf(szFile, "%s\\%03d_%s", szFilePath, nX, REGION_NPC_FILE_SERVER);
		if (!cNpcData.Open(szFile))
			goto NPC_CLOSE;
		if (cNpcData.Size() < sizeof(KNpcFileHead))
			goto NPC_CLOSE;
		cNpcData.Read(&sNpcFileHead, sizeof(KNpcFileHead));
		for (i = 0; i < sNpcFileHead.uNumNpc; i++)
		{
			cNpcData.Read(&sNpcCell, sizeof(KSPNpc) - sizeof(sNpcCell.szScript));
			if (sNpcCell.nScriptNameLen < sizeof(sNpcCell.szScript))
			{
				cNpcData.Read(sNpcCell.szScript, sNpcCell.nScriptNameLen);
				sNpcCell.szScript[sNpcCell.nScriptNameLen] = 0;
			}
			else
			{
				sNpcCell.szScript[0] = 0;
			}
			
			NpcSet.Add(nSubWorld, &sNpcCell);
		}
		g_DebugLog("[TEST]Region%x have %d npc", m_RegionID, sNpcFileHead.uNumNpc);
		
NPC_CLOSE:
		cNpcData.Close();

		// ÔØÈëobjÊý¾Ý
		ObjSet.ServerLoadRegionObj(szFilePath, nX, nY, nSubWorld);*/
	}

	return TRUE;
}
#endif

#ifndef _SERVER
//----------------------------------------------------------------------
//	¹¦ÄÜ£ºÔØÈë¿Í»§¶ËµØÍ¼ÉÏ±¾region µÄ objectÊý¾Ý£¨°üÀ¨npc¡¢boxµÈ£©
//	Èç¹û bLoadNpcFlag == TRUE ÐèÒªÔØÈë clientonly npc else ²»ÔØÈë
//----------------------------------------------------------------------
BOOL KRegion::LoadObject(int nSubWorld, int nX, int nY, char *lpszPath)
{
#ifdef TOOLVERSION
	return TRUE;
#endif

	return TRUE;
}
#endif

#ifdef _SERVER
//----------------------------------------------------------------------
//	¹¦ÄÜ£ºÔØÈë·þÎñÆ÷¶ËµØÍ¼ÉÏ±¾ region µÄÕÏ°­Êý¾Ý
//----------------------------------------------------------------------
BOOL	KRegion::LoadServerObstacle(KFile *pFile, DWORD dwDataSize)
{
	if (!pFile || dwDataSize != sizeof(this->m_Obstacle))
	{
		memset(m_Obstacle, 0, sizeof(m_Obstacle));
		return FALSE;
	}
	pFile->Read((LPVOID)m_Obstacle, sizeof(m_Obstacle));

	return TRUE;
}
#endif

#ifdef _SERVER
//----------------------------------------------------------------------
//	¹¦ÄÜ£ºÔØÈë·þÎñÆ÷¶ËµØÍ¼ÉÏ±¾ region µÄ trap Êý¾Ý
//----------------------------------------------------------------------
BOOL	KRegion::LoadServerTrap(KFile*pFile, DWORD dwDataSize)
{
	memset(m_dwTrap, 0, sizeof(m_dwTrap));
	if (!pFile || dwDataSize < sizeof(KTrapFileHead))
		return FALSE;

	KTrapFileHead	sTrapFileHead;
	KSPTrap			sTrapCell;
	int				i, j;

	pFile->Read(&sTrapFileHead, sizeof(KTrapFileHead));
	if (sTrapFileHead.uNumTrap * sizeof(KSPTrap) + sizeof(KTrapFileHead) != dwDataSize)
		return FALSE;
	for (i = 0; i < sTrapFileHead.uNumTrap; i++)
	{
		pFile->Read(&sTrapCell, sizeof(KSPTrap));
		if (sTrapCell.cY >= REGION_GRID_HEIGHT || sTrapCell.cX + sTrapCell.cNumCell - 1 >= REGION_GRID_WIDTH)
			continue;
		for (j = 0; j < sTrapCell.cNumCell; j++)
		{
			m_dwTrap[sTrapCell.cX + j][sTrapCell.cY] = sTrapCell.uTrapId;
		}
	}

	return TRUE;
}
#endif

#ifdef _SERVER
//----------------------------------------------------------------------
//	¹¦ÄÜ£ºÔØÈë·þÎñÆ÷¶ËµØÍ¼ÉÏ±¾ region µÄ npc Êý¾Ý
//----------------------------------------------------------------------
BOOL	KRegion::LoadServerNpc(int nSubWorld, KFile*pFile, DWORD dwDataSize)
{
	if (!pFile || dwDataSize < sizeof(KNpcFileHead))
		return FALSE;

	KNpcFileHead	sNpcFileHead;
	KSPNpc			sNpcCell;

	pFile->Read(&sNpcFileHead, sizeof(KNpcFileHead));

	for (int i = 0; i < sNpcFileHead.uNumNpc; i++)
	{
		pFile->Read(&sNpcCell, sizeof(KSPNpc) - sizeof(sNpcCell.szScript));
		if (sNpcCell.nScriptNameLen < sizeof(sNpcCell.szScript))
		{
			pFile->Read(sNpcCell.szScript, sNpcCell.nScriptNameLen);
			sNpcCell.szScript[sNpcCell.nScriptNameLen] = 0;
		}
		else
		{
			sNpcCell.szScript[0] = 0;
			pFile->Seek(sNpcCell.nScriptNameLen, FILE_CURRENT);
		}

		NpcSet.Add(nSubWorld, &sNpcCell);
	}

	return TRUE;
}
#endif

#ifdef _SERVER
//----------------------------------------------------------------------
//	¹¦ÄÜ£ºÔØÈë·þÎñÆ÷¶ËµØÍ¼ÉÏ±¾ region µÄ obj Êý¾Ý
//----------------------------------------------------------------------
BOOL	KRegion::LoadServerObj(int nSubWorld, KFile*pFile, DWORD dwDataSize)
{
	return ObjSet.ServerLoadRegionObj(nSubWorld, pFile, dwDataSize);
}
#endif

#ifndef _SERVER
//----------------------------------------------------------------------
//	¹¦ÄÜ£ºÔØÈë¿Í»§¶ËµØÍ¼ÉÏ±¾ region µÄ clientonlynpc Êý¾Ý
//----------------------------------------------------------------------
BOOL	KRegion::LoadClientNpc(KPakFile *pFile, DWORD dwDataSize)
{
	if (!pFile || dwDataSize < sizeof(KNpcFileHead))
		return FALSE;

	KNpcFileHead	sNpcFileHead;
	KSPNpc			sNpcCell;
	DWORD			i;
	KClientNpcID	sTempID;
	int				nNpcNo;

	pFile->Read(&sNpcFileHead, sizeof(KNpcFileHead));
	for (i = 0; i < sNpcFileHead.uNumNpc; i++)
	{
		pFile->Read(&sNpcCell, sizeof(KSPNpc) - sizeof(sNpcCell.szScript));
		if (sNpcCell.nScriptNameLen < sizeof(sNpcCell.szScript))
		{
			pFile->Read(sNpcCell.szScript, sNpcCell.nScriptNameLen);
			sNpcCell.szScript[sNpcCell.nScriptNameLen] = 0;
		}
		else
		{
			sNpcCell.szScript[0] = 0;
			pFile->Seek(sNpcCell.nScriptNameLen, FILE_CURRENT);
		}
		sTempID.m_dwRegionID = m_RegionID;
		sTempID.m_nNo = i;
		nNpcNo = NpcSet.SearchClientID(sTempID);
		if (nNpcNo == 0)
		{
			int nIdx = NpcSet.AddClientNpc(sNpcCell.nTemplateID, LOWORD(m_RegionID), HIWORD(m_RegionID), sNpcCell.nPositionX, sNpcCell.nPositionY, i);
			if (nIdx > 0)
			{
				Npc[nIdx].m_Kind = sNpcCell.shKind;
				Npc[nIdx].SendCommand(do_stand);
				Npc[nIdx].m_Dir = Npc[nIdx].GetNormalNpcStandDir(sNpcCell.nCurFrame);
			}
		}
	}

	return TRUE;
}
#endif

#ifndef _SERVER
//----------------------------------------------------------------------
//	¹¦ÄÜ£ºÔØÈë¿Í»§¶ËµØÍ¼ÉÏ±¾ region µÄ clientonlyobj Êý¾Ý
//----------------------------------------------------------------------
BOOL	KRegion::LoadClientObj(KPakFile *pFile, DWORD dwDataSize)
{
	return ObjSet.ClientLoadRegionObj(pFile, dwDataSize);
}
#endif

#ifndef _SERVER
//----------------------------------------------------------------------
//	¹¦ÄÜ£ºÔØÈëÕÏ°­Êý¾Ý¸øÐ¡µØÍ¼
//----------------------------------------------------------------------
void	KRegion::LoadLittleMapData(int nX, int nY, char *lpszPath, BYTE *lpbtObstacle)
{
	if (!lpbtObstacle)
		return;

	char	szPath[FILE_NAME_LENGTH], szFile[FILE_NAME_LENGTH];
	int		i, j;

	if (!lpszPath || !lpszPath[0] || strlen(lpszPath) >= FILE_NAME_LENGTH)
		return ;
	sprintf(szPath, "\\%s\\v_%03d", lpszPath, nY);

	KPakFile	cData;
	long		nTempTable[REGION_GRID_WIDTH][REGION_GRID_HEIGHT];	// µØÍ¼ÕÏ°­ÐÅÏ¢±í

//	g_SetFilePath(szPath);
	sprintf(szFile, "%s\\%03d_%s", szPath, nX, REGION_COMBIN_FILE_NAME_CLIENT);
	if (cData.Open(szFile))
	{
		DWORD	dwHeadSize;
		DWORD	dwMaxElemFile = 0;
		KCombinFileSection	sElemFile[REGION_ELEM_FILE_COUNT];

		if (cData.Size() < sizeof(DWORD) + sizeof(KCombinFileSection) * REGION_ELEM_FILE_COUNT)
		{
			ZeroMemory(nTempTable, sizeof(nTempTable));
		}
		else
		{
			cData.Read(&dwMaxElemFile, sizeof(DWORD));
			if (dwMaxElemFile > REGION_ELEM_FILE_COUNT)
			{
				cData.Read(sElemFile, sizeof(sElemFile));
				cData.Seek(sizeof(KCombinFileSection) * (dwMaxElemFile - REGION_ELEM_FILE_COUNT), FILE_CURRENT);
			}
			else
			{
				cData.Read(sElemFile, sizeof(sElemFile));
			}

			if (sElemFile[REGION_OBSTACLE_FILE_INDEX].uLength == sizeof(nTempTable))
			{
				dwHeadSize = sizeof(DWORD) + sizeof(KCombinFileSection) * dwMaxElemFile;
				cData.Seek(dwHeadSize + sElemFile[REGION_OBSTACLE_FILE_INDEX].uOffset, FILE_BEGIN);
				cData.Read(nTempTable, sizeof(nTempTable));
			}
			else
			{
				ZeroMemory(nTempTable, sizeof(nTempTable));
			}
		}

		cData.Close();
	}
	else
	{
		sprintf(szFile, "%03d_%s", nX, REGION_OBSTACLE_FILE);
		if (cData.Open(szFile))
			cData.Read((LPVOID)nTempTable, sizeof(nTempTable));
		else
			ZeroMemory(nTempTable, sizeof(nTempTable));
		cData.Close();
	}
	
	for (i = 0; i < REGION_GRID_HEIGHT; i++)
	{
		for (j = 0; j < REGION_GRID_WIDTH; j++)
		{
			lpbtObstacle[i * REGION_GRID_WIDTH + j] = (BYTE)nTempTable[j][i];
		}
	}
}
#endif

void KRegion::Activate()
{
	KIndexNode *pNode = NULL;
	KIndexNode *pTmpNode = NULL;
	int	nCounter = 0;

	pNode = (KIndexNode *)m_NpcList.GetHead();

	while(pNode)
	{
		pTmpNode = (KIndexNode *)pNode->GetNext();
		int nNpcIdx = pNode->m_nIndex;
#ifdef _SERVER
		if ((nCounter == m_nNpcSyncCounter / 2) && (m_nNpcSyncCounter & 1))
		{
			// ·¢ËÍÍ¬²½ÐÅºÅ
			Npc[nNpcIdx].NormalSync();
		}
		nCounter++;
#endif
		//printf("Region [%03d:%03d] NPC %s active...\n", m_nRegionX, m_nRegionY, Npc[nNpcIdx].Name);	//[wxb 2003-7-29]
		Npc[nNpcIdx].Activate();
		pNode = pTmpNode;
	}
	m_nNpcSyncCounter++;
	if (m_nNpcSyncCounter > m_NpcList.GetNodeCount() * 2)
	{
		m_nNpcSyncCounter = 0;
	}
	nCounter = 0;
	pNode = (KIndexNode *)m_ObjList.GetHead();
	while(pNode)
	{
		pTmpNode = (KIndexNode *)pNode->GetNext();
#ifdef _SERVER
		if ((nCounter == m_nObjSyncCounter / 2) && (m_nObjSyncCounter & 1))
		{
			Object[pNode->m_nIndex].SyncState();
		}
		nCounter++;
#endif
		Object[pNode->m_nIndex].Activate();
		pNode = pTmpNode;
	}
	m_nObjSyncCounter++;
	if (m_nObjSyncCounter > m_ObjList.GetNodeCount() * 2)
	{
		m_nObjSyncCounter = 0;
	}

	pNode = (KIndexNode *)m_MissleList.GetHead();
	while(pNode)
	{
		pTmpNode = (KIndexNode *)pNode->GetNext();
//		g_DebugLog("[Missle]Missle%d,Activate,in R%d", pNode->m_nIndex, this->m_nIndex);
		Missle[pNode->m_nIndex].Activate();		
		pNode = pTmpNode;
	}
#ifdef _SERVER	
	pNode = (KIndexNode *)m_PlayerList.GetHead();
	while(pNode)
	{
		pTmpNode = (KIndexNode *)pNode->GetNext();
		Player[pNode->m_nIndex].Active();
		pNode = pTmpNode;		
	}
#endif

#ifndef _SERVER
	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_RegionIndex == m_nIndex)	// ÊÇPlayerËùÔÚµÄRegion
	{
		Player[CLIENT_PLAYER_INDEX].Active();
	}
#endif
}

void KRegion::AddNpc(int nIdx)
{
	if (nIdx > 0 && nIdx < MAX_NPC)
	{
		//_ASSERT(Npc[nIdx].m_Node.m_Ref == 0);
		if (Npc[nIdx].m_Node.m_Ref == 0)
		{
			m_NpcList.AddTail(&Npc[nIdx].m_Node);
			Npc[nIdx].m_Node.AddRef();
		}
	}
}

void KRegion::RemoveNpc(int nIdx)
{
	if (nIdx <= 0 || nIdx >= MAX_NPC)
		return;

	//_ASSERT(Npc[nIdx].m_Node.m_Ref > 0);

	if (Npc[nIdx].m_Node.m_Ref > 0)
	{
		Npc[nIdx].m_Node.Remove();
		Npc[nIdx].m_Node.Release();
	}
#ifndef _SERVER
	Npc[nIdx].RemoveRes();
#endif
}

void KRegion::AddMissle(int nId)
{
	if (nId > 0 && nId < MAX_MISSLE)
	{
		_ASSERT(Missle[nId].m_Node.m_Ref == 0);
		if (Missle[nId].m_Node.m_Ref == 0)
		{
			m_MissleList.AddTail(&Missle[nId].m_Node);
			Missle[nId].m_Node.AddRef();
		}
	}
}

void KRegion::RemoveMissle(int nId)
{
	if (nId > 0 && nId < MAX_MISSLE)
	{
//		_ASSERT(Missle[nId].m_Node.m_Ref > 0);
		if (Missle[nId].m_Node.m_Ref > 0)
		{
			Missle[nId].m_Node.Remove();
			Missle[nId].m_Node.Release();
		}
	}
}

void KRegion::AddObj(int nIdx)
{
	KIndexNode *pNode = NULL;
	
	pNode = new KIndexNode;
	pNode->m_nIndex = nIdx;
	m_ObjList.AddTail(pNode);

	if (Object[nIdx].m_nMapX >= 0 && Object[nIdx].m_nMapY >= 0)
	{
		AddRef(Object[nIdx].m_nMapX, Object[nIdx].m_nMapY, obj_object);

		if(Object[nIdx].m_nKind == Obj_Kind_Obstacle)
		{
			AddRef(Object[nIdx].m_nMapX, Object[nIdx].m_nMapY, obj_obstacle);
		}
	}
}



void KRegion::RemoveObj(int nIdx)
{
	KIndexNode *pNode = NULL;
	
	pNode = (KIndexNode *)m_ObjList.GetHead();
	
	while(pNode)
	{
		if (pNode->m_nIndex == nIdx)
		{
			pNode->Remove();
			delete pNode;
			break;
		}
		pNode = (KIndexNode *)pNode->GetNext();
	}
	if (Object[nIdx].m_nMapX > 0 && Object[nIdx].m_nMapY > 0)
	{
		DecRef(Object[nIdx].m_nMapX, Object[nIdx].m_nMapY, obj_object);

		if(Object[nIdx].m_nKind == Obj_Kind_Obstacle)
		{
			DecRef(Object[nIdx].m_nMapX, Object[nIdx].m_nMapY, obj_obstacle);
		}
	}
}

DWORD KRegion::GetTrap(int nMapX, int nMapY)
{
#ifdef _SERVER
	if (nMapX < 0 || nMapY < 0 || nMapX >= REGION_GRID_WIDTH || nMapY >= REGION_GRID_HEIGHT)
	{
		return 0;
	}
	return m_dwTrap[nMapX][nMapY];
#else
	return 0;
#endif
}

BYTE KRegion::GetBarrier(int nMapX, int nMapY, int nDx, int nDy)
{
#ifdef _SERVER	
	long lType, lInfo;
	long lRet = 0;
	
	lInfo = m_Obstacle[nMapX][nMapY];
	lRet = lInfo & 0x0000000f;
	lType = (lInfo >> 4) & 0x0000000f;

	switch(lType)
	{
	case Obstacle_LT:
		if (nDx + nDy > 32)
			lRet = Obstacle_NULL;
		break;
	case Obstacle_RT:
		if (nDx < nDy)
			lRet = Obstacle_NULL;
		break;
	case Obstacle_LB:
		if (nDx > nDy)
			lRet = Obstacle_NULL;
		break;
	case Obstacle_RB:
		if (nDx + nDy < 32)
			lRet = Obstacle_NULL;
		break;
	default:
		break;
	}
	if (lRet != Obstacle_NULL)
		return lRet;
	if (m_pNpcRef)
	{
		if (m_pNpcRef[nMapY * m_nWidth + nMapX] > 0)
			return Obstacle_JumpFly;
	}
	//if (m_pObstacleRef)
	//{
	//	if (m_pObstacleRef[nMapY * m_nWidth + nMapX] > 0)
	//		return Obstacle_JumpFly;
	//}
	return Obstacle_NULL;
#else
	if (m_pNpcRef)
	{
		if (m_pNpcRef[nMapY * m_nWidth + nMapX] > 0)
			return Obstacle_JumpFly;
	}
	//if (m_pObstacleRef)
	//{
	//	if (m_pObstacleRef[nMapY * m_nWidth + nMapX] > 0)
	//		return Obstacle_JumpFly;
	//}
	return Obstacle_NULL;
#endif
}

//----------------------------------------------------------------------------
//	¹¦ÄÜ£º°´ ÏñËØµã×ø±ê * 1024 µÄ¾«¶ÈÅÐ¶ÏÄ³¸öÎ»ÖÃÊÇ·ñÕÏ°­
//	²ÎÊý£ºnGridX nGirdY £º±¾region¸ñ×Ó×ø±ê
//	²ÎÊý£ºnOffX nOffY £º¸ñ×ÓÄÚµÄÆ«ÒÆÁ¿(ÏñËØµã * 1024 ¾«¶È)
//	²ÎÊý£ºbCheckNpc £ºÊÇ·ñÅÐ¶ÏnpcÐÎ³ÉµÄÕÏ°­
//	·µ»ØÖµ£ºÕÏ°­ÀàÐÍ(if ÀàÐÍ == Obstacle_NULL ÎÞÕÏ°­)
//----------------------------------------------------------------------------
BYTE	KRegion::GetBarrierMin(int nGridX, int nGridY, int nOffX, int nOffY, BOOL bCheckNpc, BOOL bCheckObs)
{
#ifdef _SERVER
	_ASSERT(0 <= nGridX && nGridX < REGION_GRID_WIDTH && 0 <= nGridY && nGridY < REGION_GRID_HEIGHT);
	long lType;
	long lRet = Obstacle_NULL;
	
	lRet = m_Obstacle[nGridX][nGridY] & 0x0000000f;
	lType = (m_Obstacle[nGridX][nGridY] >> 4) & 0x0000000f;

	if (lRet == Obstacle_NULL)
	{
		if (bCheckNpc && m_pNpcRef)
		{
			if (m_pNpcRef[nGridY * m_nWidth + nGridX] > 0)
				return Obstacle_JumpFly;
		}
		//if (bCheckObs && m_pObstacleRef)
		//{
		//	if (m_pObstacleRef[nGridY * m_nWidth + nGridX] > 0)
		//		return Obstacle_JumpFly;
		//}
		return Obstacle_NULL;
	}

	switch(lType)
	{
	case Obstacle_LT:
		if (nOffX + nOffY > 32 * 1024)
			return Obstacle_NULL;
		break;
	case Obstacle_RT:
		if (nOffX < nOffY)
			return Obstacle_NULL;
		break;
	case Obstacle_LB:
		if (nOffX > nOffY)
			return Obstacle_NULL;
		break;
	case Obstacle_RB:
		if (nOffX + nOffY < 32 * 1024)
			return Obstacle_NULL;
		break;
	default:
		break;
	}

	return lRet;

#else
	_ASSERT(0 <= nGridX && nGridX < REGION_GRID_WIDTH && 0 <= nGridY && nGridY < REGION_GRID_HEIGHT);
	if (bCheckNpc && m_pNpcRef)
	{
		if (m_pNpcRef[nGridY * m_nWidth + nGridX] > 0)
			return Obstacle_JumpFly;
	}
	//if (bCheckObs && m_pObstacleRef)
	//{
	//	if (m_pObstacleRef[nGridY * m_nWidth + nGridX] > 0)
	//		return Obstacle_JumpFly;
	//}
	return Obstacle_NULL;
#endif
}

int KRegion::GetRef(int nMapX, int nMapY, MOVE_OBJ_KIND nType)
{
	int nRet = 0;
	if (nMapX >= m_nWidth || nMapY >= m_nHeight)
		return 0;

	switch(nType)
	{
	case obj_npc:
		nRet = (int)m_pNpcRef[nMapY * m_nWidth + nMapX];
		break;
	case obj_object:
		nRet = (int)m_pObjRef[nMapY * m_nWidth + nMapX];
		break;
	case obj_obstacle:
		nRet = (int)m_pObstacleRef[nMapY * m_nWidth + nMapX];
		break;
	case obj_missle:
		nRet = (int)m_pMslRef[nMapY * m_nWidth + nMapX];
		break;
	default:
		break;
	}
	return nRet;
}

BOOL KRegion::AddRef(int nMapX, int nMapY, MOVE_OBJ_KIND nType)
{
	BYTE* pBuffer = NULL;
	int nRef = 0;

	if (nMapX >= m_nWidth || nMapY >= m_nHeight)
		return FALSE;
	
	switch(nType)
	{
	case obj_npc:
		pBuffer = m_pNpcRef;
		break;
	case obj_object:
		pBuffer = m_pObjRef;
		break;
	case obj_obstacle:
		pBuffer = m_pObstacleRef;
		break;
	case obj_missle:
		pBuffer = m_pObjRef;
		break;
	default:
		break;
	}
	if (pBuffer)
	{
		nRef = (int)pBuffer[nMapY * m_nWidth + nMapX];
		if (nRef == 255)
			return FALSE;
		else
		{
			pBuffer[nMapY * m_nWidth + nMapX]++;
			return TRUE;
		}
	}
	else
	{
		return FALSE;
	}
}

BOOL KRegion::DecRef(int nMapX, int nMapY, MOVE_OBJ_KIND nType)
{
	BYTE* pBuffer = NULL;
	int nRef = 0;
	
	if (nMapX >= m_nWidth || nMapY >= m_nHeight)
		return FALSE;
	
	switch(nType)
	{
	case obj_npc:
		pBuffer = m_pNpcRef;
		break;
	case obj_object:
		pBuffer = m_pObjRef;
		break;
	case obj_obstacle:
		pBuffer = m_pObstacleRef;
		break;
	case obj_missle:
		pBuffer = m_pObjRef;
		break;
	default:
		break;
	}
	if (pBuffer)
	{
		nRef = (int)pBuffer[nMapY * m_nWidth + nMapX];
		if (nRef == 0)
		{
//			_ASSERT(0);
			return FALSE;
		}
		else
		{
			pBuffer[nMapY * m_nWidth + nMapX]--;
			return TRUE;
		}
	}
	else
	{
		return FALSE;
	}
}

BOOL KRegion::AddPlayer(int nIdx)
{
	if (nIdx > 0 && nIdx < MAX_PLAYER)
	{
		_ASSERT(Player[nIdx].m_Node.m_Ref == 0);
		if (Player[nIdx].m_Node.m_Ref == 0)
		{
			m_PlayerList.AddTail(&Player[nIdx].m_Node);
			Player[nIdx].m_Node.AddRef();
			return TRUE;
		}
	}
	return FALSE;
}

BOOL KRegion::RemovePlayer(int nIdx)
{
	if (nIdx > 0 && nIdx < MAX_PLAYER)
	{
		if (Player[nIdx].m_Node.m_Ref > 0)
		{
			Player[nIdx].m_Node.Remove();
			Player[nIdx].m_Node.Release();
			return TRUE;
		}
	}
	return FALSE;
}

//-------------------------------------------------------------------------
//	¹¦ÄÜ£ºÑ°ÕÒ±¾ÇøÓòÄÚÊÇ·ñÓÐÄ³¸öÖ¸¶¨ id µÄ npc
//-------------------------------------------------------------------------
int		KRegion::SearchNpc(DWORD dwNpcID)
{
	KIndexNode *pNode = NULL;

	pNode = (KIndexNode *)m_NpcList.GetHead();
	while(pNode)
	{
		if (Npc[pNode->m_nIndex].m_dwID == dwNpcID)
			return pNode->m_nIndex;
		pNode = (KIndexNode *)pNode->GetNext();
	}	

	return 0;
}

int		KRegion::SearchNpcSettingIdx(int  nNpcSettingIdx)
{
	KIndexNode *pNode = NULL;

	pNode = (KIndexNode *)m_NpcList.GetHead();
	while(pNode)
	{
		if (Npc[pNode->m_nIndex].m_NpcSettingIdx == nNpcSettingIdx)
			return pNode->m_nIndex;
		pNode = (KIndexNode *)pNode->GetNext();
	}	

	return 0;
}

int		KRegion::SearchNpcName(const char*  szName)
{
	KIndexNode *pNode = NULL;

	pNode = (KIndexNode *)m_NpcList.GetHead();
	while(pNode)
	{
		if (strcmp(Npc[pNode->m_nIndex].Name, szName) == 0)
			return pNode->m_nIndex;
		pNode = (KIndexNode *)pNode->GetNext();
	}	

	return 0;
}

int KRegion::FindNpcNear(int nX1, int nY1)
{
	KIndexNode *pNode = NULL;

	pNode = (KIndexNode *)m_NpcList.GetHead();

	while(pNode)
	{
		int nX2, nY2;
		Npc[pNode->m_nIndex].GetMpsPos(&nX2, &nY2);

		if (Npc[pNode->m_nIndex].m_dwID != Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID)
			return pNode->m_nIndex;

		pNode = (KIndexNode *)pNode->GetNext();
	}	
	return 0;
}

int KRegion::FindObject(int nMapX, int nMapY, bool bAutoFind)
{
	KIndexNode *pNode = NULL;
	
	pNode = (KIndexNode *)m_ObjList.GetHead();
	
	while(pNode)
	{
		if (Object[pNode->m_nIndex].m_nMapX == nMapX && Object[pNode->m_nIndex].m_nMapY == nMapY)
		{
			if (bAutoFind)
			{
				if (Object[pNode->m_nIndex].m_nKind == Obj_Kind_Money || 
					Object[pNode->m_nIndex].m_nKind == Obj_Kind_Item)
					return pNode->m_nIndex;
			}
			else
				return pNode->m_nIndex;
		}
		pNode = (KIndexNode *)pNode->GetNext();
	}	
	return 0;
}

int KRegion::FindObjectNear(int nX1, int nY1)
{
	KIndexNode *pNode = NULL;
	
	pNode = (KIndexNode *)m_ObjList.GetHead();
	
	while(pNode)
	{
		int nX2, nY2;
		Object[pNode->m_nIndex].GetMpsPos(&nX2, &nY2);
		if ((nX1 - nX2) * (nX1 - nX2) + (nY1 - nY2) * (nY1 - nY2) < PLAYER_PICKUP_CLIENT_DISTANCE * PLAYER_PICKUP_CLIENT_DISTANCE)
		{
			return pNode->m_nIndex;
		}
		pNode = (KIndexNode *)pNode->GetNext();
	}	
	return 0;
}

int KRegion::FindObject(int nObjID)
{
	KIndexNode *pNode = NULL;
	
	pNode = (KIndexNode *)m_ObjList.GetHead();
	
	while(pNode)
	{
		if (Object[pNode->m_nIndex].m_nID == nObjID)
		{
			return pNode->m_nIndex;
		}
		pNode = (KIndexNode *)pNode->GetNext();
	}	
	return 0;
}

#ifdef	_SERVER
void KRegion::SendSyncData(int nClient)
{
	KIndexNode *pNode = NULL;

	pNode = (KIndexNode *)m_NpcList.GetHead();
	while(pNode)
	{
		Npc[pNode->m_nIndex].SendSyncData(nClient);
		pNode = (KIndexNode *)pNode->GetNext();
	}
	pNode = (KIndexNode *)m_ObjList.GetHead();
	while(pNode)
	{
		Object[pNode->m_nIndex].SyncAdd(nClient);
		pNode = (KIndexNode *)pNode->GetNext();
	}
}

void KRegion::BroadCast(const void* pBuffer, DWORD dwSize, int &nMaxCount, int nOX, int nOY)
{
#define	MAX_SYNC_RANGE	25
	KIndexNode *pNode = NULL;

	pNode = (KIndexNode *)m_PlayerList.GetHead();
	while(pNode && nMaxCount > 0)
	{
//		_ASSERT(pNode->m_nIndex > 0 && pNode->m_nIndex < MAX_PLAYER);
		int nPlayerIndex = pNode->m_nIndex;
		int nNpcIndex = Player[nPlayerIndex].m_nIndex;
		int nTX = Npc[nNpcIndex].m_MapX;
		int nTY = Npc[nNpcIndex].m_MapY;
		int nDX = nTX - nOX;
		int nDY = nTY - nOY;
		if (Player[pNode->m_nIndex].m_nNetConnectIdx >= 0 
//			&& (nDX * nDX + nDY * nDY) <= MAX_SYNC_RANGE * MAX_SYNC_RANGE
			&& Player[pNode->m_nIndex].m_bSleepMode == FALSE)
			g_pServer->PackDataToClient(Player[pNode->m_nIndex].m_nNetConnectIdx, (BYTE*)pBuffer, dwSize);
		nMaxCount--;
		pNode = (KIndexNode *)pNode->GetNext();
	}
}

//---------------------------------------------------------------------
// ²éÕÒ¸ÃRegionÖÐNpcIDÎªdwIdµÄPlayerË÷Òý
//---------------------------------------------------------------------
int KRegion::FindPlayer(DWORD dwId)
{
	KIndexNode *pNode = NULL;
	int	nRet = -1;

	pNode = (KIndexNode *)m_PlayerList.GetHead();
	while(pNode)
	{
		if (Npc[Player[pNode->m_nIndex].m_nIndex].m_dwID == dwId)
		{
			nRet = pNode->m_nIndex;
			break;
		}
		pNode = (KIndexNode *)pNode->GetNext();
	}
	return nRet;
}

void	KRegion::SetTrap(DWORD dwTrapId, int nMapX, int nMapY)
{
	m_dwTrap[nMapX][nMapY] = dwTrapId;
}

#endif

#ifndef _SERVER
void KRegion::Paint()
{
	KIndexNode *pNode = NULL;

	pNode = (KIndexNode *)m_NpcList.GetHead();
	while(pNode)
	{
		Npc[pNode->m_nIndex].Paint();
		pNode = (KIndexNode *)pNode->GetNext();
	}
	pNode = (KIndexNode *)m_MissleList.GetHead();
	while(pNode)
	{
		Missle[pNode->m_nIndex].Paint();
		pNode = (KIndexNode *)pNode->GetNext();
	}
	pNode = (KIndexNode *)m_ObjList.GetHead();
	while(pNode)
	{
//		Object[pNode->m_nIndex].Paint();		need add -spe
		pNode = (KIndexNode *)pNode->GetNext();
	}
}
#endif

void* KRegion::GetObjNode(int nIdx)
{
	KIndexNode *pNode = NULL;

	pNode = (KIndexNode *)m_ObjList.GetHead();

	while(pNode)
	{
		if (pNode->m_nIndex == nIdx)
		{
			break;
		}
		pNode = (KIndexNode *)pNode->GetNext();
	}
	return pNode;
}

void KRegion::Close()		// Çå³ýRegionÖÐµÄ¼¸¸öÁ´±í£¨ËùÖ¸ÏòµÄÄÚÈÝÃ»ÓÐ±»Çå³ý£©
{
	KIndexNode* pNode = NULL;
	KIndexNode* pTempNode = NULL;

	if (!m_nWidth || !m_nHeight)
		return;
	
	if (m_pNpcRef)
	{
		ZeroMemory(m_pNpcRef, m_nWidth * m_nHeight);
	}
	if (m_pObjRef)
	{
		ZeroMemory(m_pObjRef, m_nWidth * m_nHeight);
	}
	if (m_pObstacleRef)
	{
		ZeroMemory(m_pObstacleRef, m_nWidth * m_nHeight);
	}
	if (m_pMslRef)
	{
		ZeroMemory(m_pMslRef, m_nWidth * m_nHeight);
	}
	pNode = (KIndexNode *)m_NpcList.GetHead();
	while(pNode)
	{
		pTempNode = pNode;
		pNode = (KIndexNode *)pNode->GetNext();
//		NpcSet.Remove(pTempNode->m_nIndex);
		Npc[pTempNode->m_nIndex].m_RegionIndex = -1;
		RemoveNpc(pTempNode->m_nIndex);
	//	delete pTempNode;
	}

	pNode = (KIndexNode *)m_MissleList.GetHead();
	while(pNode)
	{
		pTempNode = pNode;
		pNode = (KIndexNode *)pNode->GetNext();
	//	g_DebugLog("[Missle]Missle%dClose", pTempNode->m_nIndex);
		MissleSet.Remove(pTempNode->m_nIndex);
		Missle[pTempNode->m_nIndex].m_nRegionId = -1;
		pTempNode->Remove();
		pTempNode->Release();
	}

	// Í¬Ê±Çå³ý clientonly ÀàÐÍµÄ obj ---- zroc add
	pNode = (KIndexNode *)m_ObjList.GetHead();
	while(pNode)
	{
		pTempNode = pNode;
		pNode = (KIndexNode *)pNode->GetNext();
#ifndef _SERVER
//		ObjSet.RemoveIfClientOnly(pTempNode->m_nIndex);
		// zroc change 2003.07.01
//		ObjSet.Remove(pTempNode->m_nIndex);
		Object[pTempNode->m_nIndex].Remove(FALSE);
#endif
		ObjSet.Remove(pTempNode->m_nIndex); // TODO:
		Object[pTempNode->m_nIndex].m_nRegionIdx = -1;
		pTempNode->Remove();
		delete pTempNode;
	}

	pNode = (KIndexNode *)m_PlayerList.GetHead();
	while(pNode)
	{
		pTempNode = pNode;
		pNode = (KIndexNode *)pNode->GetNext();
//#ifdef _SERVER
//		PlayerSet.Remove(pTempNode->m_nIndex);
//#else
//		Player[CLIENT_PLAYER_INDEX].Release();
		pTempNode->Remove();
		pTempNode->Release();
	//	delete pTempNode;
//#endif
	}

	m_RegionID	= -1;
	m_nIndex	= -1;
	m_nActive	= 0;
	memset(m_nConnectRegion, -1, sizeof(m_nConnectRegion));
	memset(m_nConRegionID, -1, sizeof(m_nConRegionID));

}
