//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KObj.cpp
// Date:	2002.01.06
// Code:	边城浪子
// Desc:	Obj Class
//---------------------------------------------------------------------------
#include "KCore.h"
#include "KNpc.h"
#include "KObjSet.h"
#include "KPlayer.h"
#include "KSubWorld.h"
#include "KItemSet.h"
#include "KItem.h"
#include "LuaFuns.h"
#include "KSortScript.h"
#ifdef _SERVER
//#include "KNetServer.h"
//#include "../MultiServer/Heaven/Interface/iServer.h"
#else
#include "../../Represent/iRepresent/iRepresentshell.h"
#include "scene/KScenePlaceC.h"
#include "ImgRef.h"
#include "KOption.h"
#endif
#include "KObj.h"

#define		OBJ_SHOW_NAME_Y_OFF		48

KObj	Object[MAX_OBJECT];

//-------------------------------------------------------------------------
//	功能：	初始化
//-------------------------------------------------------------------------
KObj::KObj()
{
	Release();
}

//-------------------------------------------------------------------------
//	功能：	物件清空
//-------------------------------------------------------------------------
void	KObj::Release()
{
	m_nID				= -1;
	m_nKind				= 0;
	m_nBelongRegion		= -1;

	m_nIndex			= 0;
	m_nSubWorldID		= 0;
	m_nRegionIdx		= 0;
	m_nMapX				= 0;
	m_nMapY				= 0;
	m_nOffX				= 0;
	m_nOffY				= 0;
	m_nDir				= 0;

#ifndef _SERVER
	m_nLayer			= 1;
	m_nHeight			= 0;

	m_sObjLight.Release();
	m_Image.uImage = 0;
	m_SceneID = 0;
#endif

	m_nState			= 0;
	m_nLifeTime			= 0;
	m_nBornTime			= 0;
	m_nObjValue			= 0;

#ifdef _SERVER
	if (m_nItemDataID)
	{
		ItemSet.Remove(m_nItemDataID);
	}
#endif
	m_nItemDataID		= 0;
	m_nItemWidth		= 0;
	m_nItemHeight		= 0;
	m_nMoneyNum			= 0;

	m_szName[0]			= 0;
	m_dwScriptID		= 0;
	m_nColorID			= 0;
	m_nGenre			= 0;
	m_nDetailType		= 0;
	m_bOverLook			= 0;

#ifdef _SERVER
	m_bPickExecute		= FALSE;
	m_nBelong			= -1;
	m_nBelongTime		= 0;
	m_cImage.Release();
#endif

#ifndef _SERVER
	m_szImageName[0]	= 0;
	m_szImageDropName[0]= 0;
	m_szSoundName[0]	= 0;
	m_nDropState		= 0;
	m_cImage.Release();
	m_cImageDrop.Release();
	this->m_dwNameColor = 0x00ffffff;
#endif

	m_dwTimer			= 0;

	memset(m_btBar, 0, sizeof(m_btBar));
	Polygon.Clear();

#ifndef _SERVER
	m_pSoundNode		= NULL;
	m_pWave				= NULL;
#endif
}

//-------------------------------------------------------------------------
//	功能：	设定物件的索引值
//-------------------------------------------------------------------------
void	KObj::SetIndex(int nIndex)
{
	if (nIndex >= 0)
		m_nIndex = nIndex;
	else
		m_nIndex = 0;
}

//-------------------------------------------------------------------------
//	功能：	设定物件世界唯一 ID （注：只在客户端存在的物件其 ID 统一为 0）
//-------------------------------------------------------------------------
void	KObj::SetWorldID(int nID)
{
	if (nID < 0)
		m_nID = 0;
	else
		m_nID = nID;
}

//-------------------------------------------------------------------------
//	功能：	设定物件的生命时间 （注：只对尸体、装备等类型物件有效）
//-------------------------------------------------------------------------
void	KObj::SetLifeTime(int nLifeTime)
{
	if (nLifeTime < 0)
		return;
	m_nLifeTime = nLifeTime;
}

//-------------------------------------------------------------------------
//	功能：	设定物件状态
//-------------------------------------------------------------------------
void	KObj::SetState(int nState, int nPlaySoundFlag/* = 0*/)
{
	if (nState < 0)
		return;
	m_nState = nState;
	switch (m_nKind)
	{
	case Obj_Kind_Box:
		if (nState == OBJ_BOX_STATE_CLOSE)
			BoxClose();
		else if (nState == OBJ_BOX_STATE_OPEN)
			BoxOpen();
		break;
	case Obj_Kind_Door:
		if (nState == OBJ_DOOR_STATE_CLOSE)
			DoorClose();
		else if (nState == OBJ_DOOR_STATE_OPEN)
			DoorOpen();
		break;
	case Obj_Kind_Prop:
		if (nState == OBJ_PROP_STATE_HIDE)
		{
			m_nBornTime = m_nLifeTime;
		}
		break;
	}

#ifndef _SERVER
	if (nPlaySoundFlag)
		PlaySound();
#endif

#ifdef _SERVER
	SyncState();
#endif
}

//-------------------------------------------------------------------------
//	功能：	宝箱打开
//-------------------------------------------------------------------------
void	KObj::BoxOpen()
{
#ifndef _SERVER
	if (m_nState != 1)
		PlaySound();
#endif
	m_nState = 1;
	m_nBornTime = m_nLifeTime;
}

//-------------------------------------------------------------------------
//	功能：	宝箱关闭
//-------------------------------------------------------------------------
void	KObj::BoxClose()
{
#ifndef _SERVER
	if (m_nState != 0)
		PlaySound();
#endif
	m_nState = 0;
	m_nBornTime = 0;
}

//-------------------------------------------------------------------------
//	功能：	门打开
//-------------------------------------------------------------------------
void	KObj::DoorOpen()
{
#ifndef _SERVER
	if (m_nState != 1)
		PlaySound();
#endif
	m_nState = 1;

	// 缺少处理物件障碍
}

//-------------------------------------------------------------------------
//	功能：	门关闭
//-------------------------------------------------------------------------
void	KObj::DoorClose()
{
#ifndef _SERVER
	if (m_nState != 0)
		PlaySound();
#endif
	m_nState = 0;

	// 缺少处理物件障碍
}

BOOL	KObj::SetDir(int n64Dir)
{
	if (n64Dir < 0 || n64Dir >= 64)
	{
		n64Dir = 0;
		_ASSERT(0);
	}

#ifdef _SERVER
	m_nDir = n64Dir;
	m_cImage.SetCurDir64(n64Dir);
	SyncDir();
#else
	m_nDir = n64Dir;
	m_cImage.SetCurDir64(n64Dir);
	m_cImageDrop.SetCurDir64(n64Dir);
#endif

	return TRUE;
}

void	KObj::SetScriptFile(char *lpszScriptFile)
{
	char	szScript[80];
	if ( !lpszScriptFile || strlen(lpszScriptFile) >= sizeof(szScript))
	{
		g_DebugLog("[error]Script FileName Error!!!");
	}
	else
	{
		if (lpszScriptFile[0])
		{
			if (lpszScriptFile[0] == '.')
				g_StrCpyLen(szScript, &lpszScriptFile[1], sizeof(szScript));
			else
				g_StrCpyLen(szScript, lpszScriptFile, sizeof(szScript));
			g_StrLower(szScript);
			m_dwScriptID = g_FileName2Id(szScript);
		}
	}
}

void	KObj::SetImageDir(int nDir)
{
#ifdef _SERVER
	m_cImage.SetCurDir(nDir);
	SetDir(m_cImage.m_nCurDir * 64 / m_cImage.m_nTotalDir);
#else
	m_cImage.SetCurDir(nDir);
	SetDir(m_cImage.m_nCurDir * 64 / m_cImage.m_nTotalDir);
#endif
}

#ifdef _SERVER
void	KObj::SetItemDataID(int nItemDataID)
{
	if (nItemDataID >= 0)
		m_nItemDataID = nItemDataID;
}
#endif


#ifndef _SERVER
void KObj::DrawInfo(int nFontSize)
{
	int		nMpsX, nMpsY, nHeightOff;
	DWORD	dwColor;
	if (m_nKind != Obj_Kind_Item && m_nKind != Obj_Kind_Money)
		return;

	if (m_nKind == Obj_Kind_Money)
	{	
		char szName[128];
		GetMpsPos(&nMpsX, &nMpsY);
		nHeightOff = OBJ_SHOW_NAME_Y_OFF;
		if (m_nMoneyNum < MONEY_FLOOR)
		{
			sprintf(szName,"%d lng",m_nMoneyNum);
			g_pRepresent->OutputText(nFontSize, szName, KRF_ZERO_END, nMpsX - 12 * g_StrLen(szName) / 4, nMpsY, 0x00ffffff, 0, nHeightOff);
		}
		else if ((m_nMoneyNum % MONEY_FLOOR) == 0)
		{
			sprintf(szName,"%d v筺 lng",m_nMoneyNum / MONEY_FLOOR);
			g_pRepresent->OutputText(nFontSize, szName, KRF_ZERO_END, nMpsX - 12 * g_StrLen(szName) / 4, nMpsY, 0x00ffffff, 0, nHeightOff);
		}
		else
		{
			sprintf(szName,"%d v筺 %d lng",m_nMoneyNum / MONEY_FLOOR, m_nMoneyNum % MONEY_FLOOR);
			g_pRepresent->OutputText(nFontSize, szName, KRF_ZERO_END, nMpsX - 12 * g_StrLen(szName) / 4, nMpsY, 0x00ffffff, 0, nHeightOff);
		}

	}
	else if (m_nKind == Obj_Kind_Item)
	{
		GetMpsPos(&nMpsX, &nMpsY);
		nHeightOff = OBJ_SHOW_NAME_Y_OFF;
		g_pRepresent->OutputText(nFontSize, m_szName, KRF_ZERO_END, nMpsX - 12 * g_StrLen(m_szName) / 4, nMpsY, this->m_dwNameColor, 0, nHeightOff);
	}
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：	物件绘制
//-------------------------------------------------------------------------
void KObj::Draw()
{
	if ( m_nIndex <= 0 )
		return;
	if ( !m_cImage.CheckExist() )
		return;
	if (m_bDrawFlag)
		return;
	if (m_nRegionIdx < 0 || m_nRegionIdx >= 9)
		return;

	int			x, y;
	SubWorld[m_nSubWorldID].Map2Mps(m_nRegionIdx, m_nMapX, m_nMapY, m_nOffX, m_nOffY, &x, &y);
//	SubWorld[m_nSubWorldID].Mps2Screen(&x, &y);

	m_Image.Color.Color_b.a = 255;
	m_Image.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
	m_Image.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
	m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
	m_Image.nType = ISI_T_SPR;
	m_Image.oPosition.nZ = 0;
	if (m_nKind == Obj_Kind_Item && m_nDropState == 1)	// 物品掉出动画
	{
		m_Image.nFrame = m_cImageDrop.m_nCurFrame;
		m_Image.oPosition.nX = x;// - m_cImageDrop.m_nCgXpos;
		m_Image.oPosition.nY = y;// - m_cImageDrop.m_nCgYpos * 2;
		strcpy(m_Image.szImage, m_cImageDrop.m_szName);
	}
	else
	{
		m_Image.nFrame = m_cImage.m_nCurFrame;
		m_Image.oPosition.nX = x;// - m_cImage.m_nCgXpos;
		m_Image.oPosition.nY = y;// - m_cImage.m_nCgYpos * 2;
		strcpy(m_Image.szImage, m_cImage.m_szName);
	}

	switch(m_nKind)
	{
	case Obj_Kind_MapObj:
		g_pRepresent->DrawPrimitives(1, &m_Image, RU_T_IMAGE, 0);	
		break;
	case Obj_Kind_Prop:
		if ( m_nState == OBJ_PROP_STATE_DISPLAY )
			g_pRepresent->DrawPrimitives(1, &m_Image, RU_T_IMAGE, 0);	
		break;
	default:
		g_pRepresent->DrawPrimitives(1, &m_Image, RU_T_IMAGE, 0);	
		break;
	}
}

#endif

//-------------------------------------------------------------------------
//	功能：	各种物件的处理
//-------------------------------------------------------------------------
void	KObj::Activate()
{
	if ( m_nIndex <= 0 )
		return;

#ifdef _SERVER
	int		nEnemyIndex, x, y;
#else
	int		nMask = IPOT_RL_OBJECT | IPOT_RL_INFRONTOF_ALL;
#endif

	switch(m_nKind)
	{
#ifndef _SERVER
	case Obj_Kind_MapObj:					// 地图物件动画
		if (m_nState == 0)					// 地图物件动画循环播放
			m_cImage.GetNextFrame();
		nMask = IPOT_RL_OBJECT | IPOT_RL_INFRONTOF_ALL;
		break;
	case Obj_Kind_Light:					// 光源
		if (m_nState == 0)
			m_cImage.GetNextFrame();
		nMask = IPOT_RL_OBJECT | IPOT_RL_INFRONTOF_ALL;
		break;
	case Obj_Kind_LoopSound:				// 循环音效
		PlayLoopSound();
		break;
	case Obj_Kind_RandSound:				// 随机音效
		PlayRandSound();
		break;
	case Obj_Kind_Body:						// 尸体逐渐消失
		m_cImage.GetNextFrame(FALSE);		// 尸体动画单方向播放
		m_nLifeTime--;
		if (m_nLifeTime <= 0)
			Remove(FALSE);
		nMask = IPOT_RL_COVER_GROUND | IPOT_RL_INFRONTOF_ALL;
		break;
#endif
	case Obj_Kind_Box:						// 宝箱
#ifdef _SERVER
		if (m_nState == OBJ_BOX_STATE_OPEN)	// 宝箱关闭重生
		{
			m_nBornTime--;
			if (m_nBornTime <= 0)
				SetState(OBJ_BOX_STATE_CLOSE);
		}
#else
		if (m_nState == OBJ_BOX_STATE_CLOSE)// 宝箱关闭状态
			m_cImage.GetPrevFrame(FALSE);
		else// if (m_nState == OBJ_BOX_STATE_OPEN)// 宝箱打开状态
			m_cImage.GetNextFrame(FALSE);
		nMask = IPOT_RL_OBJECT | IPOT_RL_INFRONTOF_ALL;
#endif
		break;
	case Obj_Kind_Door:						// 门
#ifdef _SERVER
#else
		if (m_nState == OBJ_DOOR_STATE_CLOSE)// 门关闭状态
			m_cImage.GetPrevFrame(FALSE);
		else// if (m_nState == OBJ_DOOR_STATE_OPEN)// 门打开状态
			m_cImage.GetNextFrame(FALSE);
		nMask = IPOT_RL_OBJECT | IPOT_RL_INFRONTOF_ALL;
#endif
		break;
	case Obj_Kind_Item:						// 装备动画循环播放
#ifdef _SERVER
		if (m_nBelong >= 0)
		{
			m_nBelongTime--;
			if (m_nBelongTime <= 0)
			{
				m_nBelongTime = 0;
				m_nBelong = -1;
				
			}
		}
		m_nLifeTime--;
		if (m_nLifeTime <= 0)
			Remove(FALSE);
#else
		if (this->m_nDropState == 1)		// 物品掉出动画
		{
			if (m_cImageDrop.GetNextFrame(FALSE))
			{
				if (m_cImageDrop.CheckEnd())
				{
					m_nDropState = 0;			// 物品掉出动画播放完了，改为放置循环动画
					m_Image.uImage = 0;
				}
			}
			nMask = IPOT_RL_OBJECT | IPOT_RL_INFRONTOF_ALL;
		}
		else
		{
			if (this->m_nDropState == 2)
			{
				if (m_cImage.GetNextFrame(FALSE))
				{
					if (m_cImage.CheckEnd())
						this->m_nDropState = 0;
				}
			}
			else
			{
				if (g_Random(40) == 0)
				{
					m_nDropState = 2;
				}
				else
				{
					m_cImage.SetDirStart();
				}
			}
			nMask = IPOT_RL_COVER_GROUND | IPOT_RL_INFRONTOF_ALL;
		}
		m_nLifeTime--;
		if (m_nLifeTime <= -100)
			Remove(FALSE);
#endif
		break;
	case Obj_Kind_Money:
#ifdef _SERVER
		if (m_nBelong >= 0)
		{
			m_nBelongTime--;
			if (m_nBelongTime <= 0)
			{
				m_nBelongTime = 0;
				m_nBelong = -1;
			}
		}
		m_nLifeTime--;
		if (m_nLifeTime <= 0)
			Remove(FALSE);
#else
		m_cImage.GetNextFrame();
		nMask = IPOT_RL_COVER_GROUND | IPOT_RL_INFRONTOF_ALL;
		m_nLifeTime--;
		if (m_nLifeTime <= -100)
			Remove(FALSE);
#endif
		break;
	case Obj_Kind_Prop:
#ifdef _SERVER
		if (m_nState == OBJ_PROP_STATE_HIDE)	// 道具隐藏状态
		{
			m_nBornTime--;
			if (m_nBornTime <= 0)
				SetState(OBJ_PROP_STATE_DISPLAY);// 道具重生
		}
#else
		if (m_nState == OBJ_PROP_STATE_DISPLAY)
		{
			m_cImage.GetNextFrame();
			nMask = IPOT_RL_OBJECT | IPOT_RL_INFRONTOF_ALL;
		}
#endif
		break;
	case Obj_Kind_Trap:
		break;
	}
#ifndef _SERVER
	int nMpsX, nMpsY;
	SubWorld[m_nSubWorldID].Map2Mps(m_nRegionIdx, m_nMapX, m_nMapY, m_nOffX, m_nOffY, &nMpsX, &nMpsY);
	g_ScenePlace.MoveObject(CGOG_OBJECT, m_nIndex, nMpsX, nMpsY, 0, m_SceneID, nMask);
#endif
}

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：	播放循环音效
//-------------------------------------------------------------------------
void	KObj::PlayLoopSound()
{
	if (!m_szSoundName[0])
		return;

	m_pSoundNode = (KCacheNode*) g_SoundCache.GetNode(m_szSoundName, (KCacheNode * )m_pSoundNode);
	m_pWave = (KWavSound*)m_pSoundNode->m_lpData;
	if (m_pWave)
	{
		if (!m_pWave->IsPlaying())
		{
			m_pWave->Play(GetSoundPan(), GetSoundVolume(), 0);
		}
		else
		{
			m_pWave->SetPan(GetSoundPan());
			m_pWave->SetVolume(GetSoundVolume());
		}
	}
}

//-------------------------------------------------------------------------
//	功能：	播放随机音效
//-------------------------------------------------------------------------
void	KObj::PlayRandSound()
{
	if (!m_szSoundName[0])
		return;

	if (g_Random(500) != 0)
		return;

	m_pSoundNode = (KCacheNode*) g_SoundCache.GetNode(m_szSoundName, (KCacheNode * )m_pSoundNode);
	m_pWave = (KWavSound*)m_pSoundNode->m_lpData;
	if (m_pWave)
	{
		if (m_pWave->IsPlaying())
			return;
		m_pWave->Play(GetSoundPan(), GetSoundVolume(), 0);
	}
}

//-------------------------------------------------------------------------
//	功能：	得到声音音相大小
//-------------------------------------------------------------------------
int		KObj::GetSoundPan()
{
	int		nNpcX, nNpcY, nObjX, nObjY;

	SubWorld[Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SubWorldIndex].Map2Mps(
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_RegionIndex,
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_MapX,
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_MapY,
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_OffX,
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_OffY,
		&nNpcX,
		&nNpcY);
	SubWorld[m_nSubWorldID].Map2Mps(
		m_nRegionIdx,
		m_nMapX,
		m_nMapY,
		m_nOffX,
		m_nOffY,
		&nObjX,
		&nObjY);

	return (nObjX - nNpcX) * 5;
}

//-------------------------------------------------------------------------
//	功能：	得到声音音量大小
//-------------------------------------------------------------------------
int		KObj::GetSoundVolume()
{
	int		nNpcX, nNpcY, nObjX, nObjY;

	SubWorld[Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SubWorldIndex].Map2Mps(
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_RegionIndex,
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_MapX,
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_MapY,
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_OffX,
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_OffY,
		&nNpcX,
		&nNpcY);
	SubWorld[m_nSubWorldID].Map2Mps(
		m_nRegionIdx,
		m_nMapX,
		m_nMapY,
		m_nOffX,
		m_nOffY,
		&nObjX,
		&nObjY);

	return ((10000 - (abs(nObjX - nNpcX) + abs(nObjY - nNpcY) * 2)) * Option.GetSndVolume() / 100) - 10000;
//	return -((abs(nObjX - nNpcX) + abs(nObjY - nNpcY) * 2) * Option.GetSndVolume() / 100);
}

//-------------------------------------------------------------------------
//	功能：	播放声音
//-------------------------------------------------------------------------
void	KObj::PlaySound()
{
	if (!m_szSoundName[0])
		return;

	m_pSoundNode = (KCacheNode*) g_SoundCache.GetNode(m_szSoundName, (KCacheNode * )m_pSoundNode);
	m_pWave = (KWavSound*)m_pSoundNode->m_lpData;
	if (m_pWave)
	{
		if (m_pWave->IsPlaying())
			return;
		m_pWave->Play(GetSoundPan(), GetSoundVolume(), 0);
	}
}

#endif

//-------------------------------------------------------------------------
//	功能：	运行物件脚本
//-------------------------------------------------------------------------
void	KObj::ExecScript(int nPlayerIdx)
{
	if (!m_dwScriptID)
		return;
	if (nPlayerIdx < 0)
		return;
	DWORD dwScriptId = m_dwScriptID;//g_FileName2Id(m_szScriptName);
	KLuaScript * pScript = (KLuaScript*)g_GetScript(dwScriptId);
	try
	{
		if (pScript)
		{
			if (Player[nPlayerIdx].m_nIndex < 0) return ;
			Npc[Player[nPlayerIdx].m_nIndex].m_ActionScriptID = dwScriptId;
			Lua_PushNumber(pScript->m_LuaState, Player[nPlayerIdx].GetPlayerIndex());
			pScript->SetGlobalName(SCRIPT_PLAYERINDEX);
			
			Lua_PushNumber(pScript->m_LuaState, Player[nPlayerIdx].GetPlayerID());
			pScript->SetGlobalName(SCRIPT_PLAYERID);
			
			Lua_PushNumber(pScript->m_LuaState, Npc[Player[nPlayerIdx].m_nIndex].m_SubWorldIndex);
			pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);

			Lua_PushNumber(pScript->m_LuaState, m_nIndex);
			pScript->SetGlobalName(SCRIPT_OBJINDEX);
			
			int nTopIndex = 0;
			pScript->SafeCallBegin(&nTopIndex);
			
			BOOL bResult = FALSE;
			bResult = pScript->CallFunction(NORMAL_FUNCTION_NAME, 0, "d", m_nIndex);
			pScript->SafeCallEnd(nTopIndex);
			
		}
	}
	catch(...)
	{
		printf("-->Error Execute: %u [%s]\n", dwScriptId, NORMAL_FUNCTION_NAME);
	}
	return;
}

#ifdef _SERVER
BOOL	KObj::SyncAdd(int nClient)
{
	OBJ_ADD_SYNC	cObjAdd;
	int				nTempX, nTempY;

	cObjAdd.ProtocolType = (BYTE)s2c_objadd;
	cObjAdd.m_nID = m_nID;
	cObjAdd.m_nDataID = m_nDataID;
	cObjAdd.m_btDir = m_nDir;
	cObjAdd.m_btState = m_nState;
	cObjAdd.m_wCurFrame = m_cImage.m_nCurFrame;
	SubWorld[m_nSubWorldID].Map2Mps(m_nRegionIdx, m_nMapX, m_nMapY, m_nOffX, m_nOffY, &nTempX, &nTempY);
	cObjAdd.m_nXpos = nTempX;
	cObjAdd.m_nYpos = nTempY;
	cObjAdd.m_nMoneyNum = m_nMoneyNum;
	cObjAdd.m_nItemID = m_nItemDataID;
	cObjAdd.m_btItemWidth = m_nItemWidth;
	cObjAdd.m_btItemHeight = m_nItemHeight;
	cObjAdd.m_btColorID = this->m_nColorID;
	cObjAdd.m_nGenre = this->m_nGenre;
	cObjAdd.m_nDetailType = this->m_nDetailType;
	cObjAdd.m_btFlag = 0;
	if (this->m_bOverLook)
		cObjAdd.m_btFlag |= 0x04;
	strcpy(cObjAdd.m_szName, this->m_szName);
	cObjAdd.m_wLength = sizeof(OBJ_ADD_SYNC) - 1 - sizeof(cObjAdd.m_szName) + strlen(cObjAdd.m_szName);

	g_pServer->PackDataToClient(nClient, (BYTE*)&cObjAdd, cObjAdd.m_wLength + 1);

	return TRUE;
}

void	KObj::SyncState()
{
	OBJ_SYNC_STATE	cObjState;

	cObjState.ProtocolType = (BYTE)s2c_syncobjstate;
	cObjState.m_nID = m_nID;
	cObjState.m_btState = (BYTE)m_nState;

	POINT	POff[8] = 
	{
		{0, 32},
		{-16, 32},
		{-16, 0},
		{-16, -32},
		{0, -32},
		{16, -32},
		{16, 0},
		{16, 32},
	};
	int nMaxCount = MAX_BROADCAST_COUNT;
	SubWorld[m_nSubWorldID].m_Region[m_nRegionIdx].BroadCast((BYTE*)&cObjState, sizeof(OBJ_SYNC_STATE), nMaxCount, m_nMapX, m_nMapY);
	int nConRegion;
	for (int i = 0; i < 8; i++)
	{
		nConRegion = SubWorld[m_nSubWorldID].m_Region[m_nRegionIdx].m_nConnectRegion[i];
		if (nConRegion == -1)
			continue;
		SubWorld[m_nSubWorldID].m_Region[nConRegion].BroadCast((BYTE*)&cObjState, sizeof(OBJ_SYNC_STATE), nMaxCount, m_nMapX - POff[i].x, m_nMapY - POff[i].y);
	}
}

void	KObj::SyncDir()
{
	OBJ_SYNC_DIR	cObjDir;

	cObjDir.ProtocolType = (BYTE)s2c_syncobjdir;
	cObjDir.m_nID = m_nID;
	cObjDir.m_btDir = (BYTE)m_nDir;

	POINT	POff[8] = 
	{
		{0, 32},
		{-16, 32},
		{-16, 0},
		{-16, -32},
		{0, -32},
		{16, -32},
		{16, 0},
		{16, 32},
	};
	int nMaxCount = MAX_BROADCAST_COUNT;
	SubWorld[m_nSubWorldID].m_Region[m_nRegionIdx].BroadCast((BYTE*)&cObjDir, sizeof(OBJ_SYNC_DIR), nMaxCount, m_nMapX, m_nMapY);
	int nConRegion;
	for (int i = 0; i < 8; i++)
	{
		nConRegion = SubWorld[m_nSubWorldID].m_Region[m_nRegionIdx].m_nConnectRegion[i];
		if (nConRegion == -1)
			continue;
		SubWorld[m_nSubWorldID].m_Region[nConRegion].BroadCast((BYTE*)&cObjDir, sizeof(OBJ_SYNC_DIR), nMaxCount, m_nMapX - POff[i].x, m_nMapY - POff[i].y);
	}
}

void	KObj::SyncRemove(BOOL bSoundFlag)
{
	OBJ_SYNC_REMOVE		cObjSyncRemove;

	cObjSyncRemove.ProtocolType = (BYTE)s2c_objremove;
	cObjSyncRemove.m_nID = m_nID;
	cObjSyncRemove.m_btSoundFlag = bSoundFlag;

	POINT	POff[8] = 
	{
		{0, 32},
		{-16, 32},
		{-16, 0},
		{-16, -32},
		{0, -32},
		{16, -32},
		{16, 0},
		{16, 32},
	};
	int nMaxCount = MAX_BROADCAST_COUNT;
	SubWorld[m_nSubWorldID].m_Region[m_nRegionIdx].BroadCast((BYTE*)&cObjSyncRemove, sizeof(OBJ_SYNC_REMOVE), nMaxCount, m_nMapX, m_nMapY);
	int nConRegion;
	for (int i = 0; i < 8; i++)
	{
		nConRegion = SubWorld[m_nSubWorldID].m_Region[m_nRegionIdx].m_nConnectRegion[i];
		if (nConRegion == -1)
			continue;
		SubWorld[m_nSubWorldID].m_Region[nConRegion].BroadCast((BYTE*)&cObjSyncRemove, sizeof(OBJ_SYNC_REMOVE), nMaxCount, m_nMapX - POff[i].x, m_nMapY - POff[i].y);
	}
}

int		KObj::GetItemDataID()
{
	return m_nItemDataID;
}

#endif

#ifdef _SERVER
void	KObj::SetItemBelong(int nPlayerIdx)
{
	m_nBelong = nPlayerIdx;
	if (m_nBelong >= 0)
		this->m_nBelongTime = OBJ_BELONG_TIME;
	else
		this->m_nBelongTime = 0;
}
#endif

#ifdef _SERVER
void	KObj::SetEntireBelong(int nPlayerIdx)
{
	m_nBelong = nPlayerIdx;
	if (m_nBelong >= 0)
		this->m_nBelongTime = this->m_nLifeTime * 2;
	else
		this->m_nBelongTime = 0;
}
#endif

void	KObj::Remove(BOOL bSoundFlag)
{
#ifdef _SERVER
	SyncRemove(bSoundFlag);
#else
	if (bSoundFlag)
	{
		switch (m_nKind)
		{
		case Obj_Kind_Money:
			PlaySound();
			break;
		case Obj_Kind_Box:
		case Obj_Kind_Item:
		case Obj_Kind_Door:
		case Obj_Kind_Prop:
			break;
		}
	}
	m_Image.uImage = 0;
	g_ScenePlace.RemoveObject(CGOG_OBJECT, m_nIndex, m_SceneID);
#endif
	SubWorld[m_nSubWorldID].m_WorldMessage.Send(GWM_OBJ_DEL, m_nIndex);
}

int		KObj::GetKind()
{
	return m_nKind;
}

//-------------------------------------------------------------------------
//	功能：	凸多边形转换为障碍信息
//-------------------------------------------------------------------------
void	KObj::PolygonChangeToBar(
								 KPolygon Polygon,	// 凸多边形
								 int nGridWidth,	// 格子长
								 int nGridHeight,	// 格子宽
								 int nTableWidth,	// 表格长
								 int nTableHeight,	// 表格宽
								 BYTE *lpbBarTable)	// 表格内容
{
	if ( !lpbBarTable )
		return;
	if (nGridWidth <= 0 || nGridHeight <= 0 || nTableWidth <= 0 || nTableHeight <= 0)
		return;

	int		nTemp, nTempLT, nTempRT, nTempLB, nTempRB, nFlag = 0;
	POINT	TempPos;
	for (int i = 0; i < nTableWidth * nTableHeight; i++)
	{
		Polygon.GetCenterPos(&TempPos);
		// 左上
		TempPos.x += ((i % nTableWidth) * nGridWidth) - ((nTableWidth / 2) * nGridWidth + nGridWidth / 2);
		TempPos.y += ((i / nTableWidth) * nGridHeight) - ((nTableHeight / 2) * nGridHeight + nGridHeight / 2);
		nTempLT = Polygon.IsPointInPolygon(TempPos);
		// 右上
		TempPos.x += nGridWidth;
		nTempRT = Polygon.IsPointInPolygon(TempPos);
		// 左下
		TempPos.x -= nGridWidth;
		TempPos.y += nGridHeight;
		nTempLB = Polygon.IsPointInPolygon(TempPos);
		// 右下
		TempPos.x += nGridWidth;
		nTempRB = Polygon.IsPointInPolygon(TempPos);

		nTemp = nTempLT + nTempRT + nTempLB + nTempRB;
		if (nTemp == 0)
			lpbBarTable[i] = Obj_Bar_Empty;
		else if (nTemp > 1)
		{
			lpbBarTable[i] = Obj_Bar_Full;
			nFlag = 1;
		}
		else
		{
			if (nTempLT)
				lpbBarTable[i] = Obj_Bar_LT;
			else if (nTempRT)
				lpbBarTable[i] = Obj_Bar_RT;
			else if (nTempLB)
				lpbBarTable[i] = Obj_Bar_LB;
			else if (nTempRB)
				lpbBarTable[i] = Obj_Bar_RB;
		}
	}

	lpbBarTable[(nTableHeight / 2) * nTableWidth + nTableWidth / 2] = Obj_Bar_Full;
}


#ifdef _SERVER
ServerImage::ServerImage()
{
	Release();
}

void	ServerImage::Release()
{
	m_nTotalFrame = 1;
	m_nCurFrame = 0;
	m_nTotalDir = 1;
	m_nCurDir = 0;
	m_dwTimer = 0;
	m_dwInterval = 0;
	m_nDirFrames = 1;
}

//---------------------------------------------------------------------------
//	功能：	设定总帧数
//---------------------------------------------------------------------------
void	ServerImage::SetTotalFrame(int nTotalFrame)
{
	if (nTotalFrame > 0)
	{
		m_nTotalFrame = nTotalFrame;
		m_nDirFrames = m_nTotalFrame / m_nTotalDir;
	}
}

//---------------------------------------------------------------------------
//	功能：	设定当前帧
//---------------------------------------------------------------------------
void	ServerImage::SetCurFrame(int nCurFrame)
{
	if (nCurFrame < 0 || nCurFrame >= m_nTotalFrame)
		return;
	m_nCurFrame = nCurFrame;
	if (m_nTotalFrame && m_nTotalDir)
		m_nCurDir = nCurFrame / m_nDirFrames;
	else
		m_nCurDir = 0;
	m_dwTimer = SubWorld[0].m_dwCurrentTime;
}

//---------------------------------------------------------------------------
//	功能：	设定总方向数
//---------------------------------------------------------------------------
void	ServerImage::SetTotalDir(int nTotalDir)
{
	if (nTotalDir > 0)
	{
		m_nTotalDir = nTotalDir;
		m_nDirFrames = m_nTotalFrame / m_nTotalDir;
	}
}

//---------------------------------------------------------------------------
//	功能：	设定当前方向
//---------------------------------------------------------------------------
BOOL	ServerImage::SetCurDir(int nDir)
{
	if (m_nCurDir == nDir)
		return TRUE;
	if (nDir < 0 || nDir >= m_nTotalDir)
		return FALSE;
	m_nCurDir = nDir;
	m_nCurFrame = m_nDirFrames * nDir;
	m_dwTimer = SubWorld[0].m_dwCurrentTime;
	return FALSE;
}

//---------------------------------------------------------------------------
//	功能：	设定帧间隔
//---------------------------------------------------------------------------
void	ServerImage::SetInterVal(DWORD dwInterval)
{
	m_dwInterval = dwInterval;
}

//---------------------------------------------------------------------------
//	功能：	获得单方向帧数
//---------------------------------------------------------------------------
int		ServerImage::GetOneDirFrames()
{
	return m_nDirFrames;
}

//---------------------------------------------------------------------------
//	功能：	判断动画是否播放到最后，当前是第 0 帧
//---------------------------------------------------------------------------
BOOL	ServerImage::CheckEnd()
{
	if (m_nCurFrame == m_nDirFrames * (m_nCurDir + 1) - 1)
		return TRUE;
	return FALSE;
}

//---------------------------------------------------------------------------
//	功能：	设定当前方向的当前帧为第一帧
//---------------------------------------------------------------------------
void	ServerImage::SetDirStart()
{
	m_nCurFrame = m_nCurDir * m_nDirFrames;
}

//---------------------------------------------------------------------------
//	功能：	设定当前方向的当前帧为最后一帧
//---------------------------------------------------------------------------
void	ServerImage::SetDirEnd()
{
	m_nCurFrame = (m_nCurDir + 1) * m_nDirFrames - 1;
}

//---------------------------------------------------------------------------
//	功能：	取得当前方向的下一帧
//---------------------------------------------------------------------------
BOOL	ServerImage::GetNextFrame(BOOL bLoop)
{
	if (SubWorld[0].m_dwCurrentTime - m_dwTimer >= m_dwInterval)
	{
		m_dwTimer = SubWorld[0].m_dwCurrentTime;
		m_nCurFrame++;
		if (m_nCurFrame >= m_nDirFrames * (m_nCurDir + 1))
		{
			if (bLoop)
				m_nCurFrame = m_nDirFrames * m_nCurDir;
			else
				m_nCurFrame = m_nDirFrames * (m_nCurDir + 1) - 1;
		}
		return TRUE;
	}

	return FALSE;
}

//---------------------------------------------------------------------------
//	功能：	取得当前方向的前一帧
//---------------------------------------------------------------------------
BOOL	ServerImage::GetPrevFrame(BOOL bLoop)
{
	if (SubWorld[0].m_dwCurrentTime - m_dwTimer >= m_dwInterval)
	{
		m_dwTimer = SubWorld[0].m_dwCurrentTime;
		m_nCurFrame--;
		if (m_nCurFrame < m_nDirFrames * m_nCurDir)
		{
			if (bLoop)
				m_nCurFrame = m_nDirFrames * (m_nCurDir + 1) - 1;
			else
				m_nCurFrame = m_nDirFrames * m_nCurDir;
		}
		return TRUE;
	}

	return FALSE;
}

//---------------------------------------------------------------------------
//	功能：	设定当前方向(方向需从64方向转换到真正的方向)
//---------------------------------------------------------------------------
BOOL	ServerImage::SetCurDir64(int nDir)
{
	if (nDir < 0 || nDir >= 64)
		return FALSE;

	int nTempDir;

	nTempDir = (nDir + (32 / m_nTotalDir)) / (64 / m_nTotalDir);
	if (nTempDir >= m_nTotalDir)
		nTempDir -= m_nTotalDir;
	if (m_nCurDir == nTempDir)
		return TRUE;
	m_nCurDir = nTempDir;
	m_nCurFrame = m_nDirFrames * nTempDir;
	m_dwTimer = SubWorld[0].m_dwCurrentTime;
	return FALSE;
}

//---------------------------------------------------------------------------
//	功能：	获得当前方向第几帧
//---------------------------------------------------------------------------
int		ServerImage::GetCurDirFrameNo()
{
	return m_nCurFrame - m_nCurDir * m_nDirFrames;
}

#endif

void KObj::GetMpsPos(int *pX, int *pY)
{
	SubWorld[m_nSubWorldID].Map2Mps(m_nRegionIdx, m_nMapX, m_nMapY, m_nOffX, m_nOffY, pX, pY);
}

#ifndef _SERVER
void KObj::DrawBorder()
{
	if (m_bDrawFlag)
		return;

	m_Image.bRenderStyle = IMAGE_RENDER_STYLE_BORDER_RECT;
	switch(m_nKind)
	{
	case Obj_Kind_MapObj:
		g_pRepresent->DrawPrimitives(1, &m_Image, RU_T_IMAGE, 0);	
		break;
	case Obj_Kind_Prop:
		if ( m_nState == OBJ_PROP_STATE_DISPLAY )
			g_pRepresent->DrawPrimitives(1, &m_Image, RU_T_IMAGE, 0);	
		break;
	default:
		g_pRepresent->DrawPrimitives(1, &m_Image, RU_T_IMAGE, 0);	
		break;
	}
	m_Image.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
}
#endif


int KObj::GetDistanceSquare(int nNpcIndex)
{
	int	nRet = 0;
	if ( m_nSubWorldID != Npc[nNpcIndex].m_SubWorldIndex)
		return -1;
	
	int XOff = 0;
	int YOff = 0;
	
	if (m_nRegionIdx == Npc[nNpcIndex].m_RegionIndex)
	{
		XOff = (m_nMapX - Npc[nNpcIndex].GetMapX()) * REGION_CELL_SIZE_X;
		XOff += (m_nOffX - Npc[nNpcIndex].GetOffX()) >> 10;
		
		YOff = (m_nMapY - Npc[nNpcIndex].GetMapY()) * REGION_CELL_SIZE_Y;
		YOff += (m_nOffY - Npc[nNpcIndex].GetOffY()) >> 10;
	}
	else
	{
		int X1, Y1;
		int X2, Y2;
		GetMpsPos( &X1, &Y1 );
		Npc[nNpcIndex].GetMpsPos( &X2, &Y2 );
		
		XOff = (X2 - X1);
		YOff = (Y2 - Y1);		
	}
	
	nRet = (int)(XOff * XOff + YOff * YOff);
	
	return nRet;	
}
