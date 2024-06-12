// *****************Editer	: duccom0123 EditTime:	2024/06/12 11:48:43*********************
// µØÍ¼µÄÀà¶¨ÒåÊµÏÖ
// Copyright : Kingsoft 2003
// Author    : wooy(wu yue)
// CreateTime: 2003-7-8
// ***************************************************************************************
#include "KCore.h"
#include "ScenePlaceMapC.h"
#include "crtdbg.h"
#include "../KPlayer.h"
#include "../ImgRef.h"
#include "../../Represent/iRepresent/iRepresentShell.h"
#include "../GameDataDef.h"
#include "../CoreShell.h"
#include "KScenePlaceRegionC.h"
#include "SceneDataDef.h"
#include    "../../Engine/Src/KSG_StringProcess.h"
#include "KScenePlaceC.h"
extern KScenePlaceC	g_ScenePlace;

#define	PLACE_MAP_FILE_NAME_APPEND		"24.jpg"
#define	PLACE_MAP_SAVE_SECTION			"MAIN"
#define	PLACE_MAP_SYMBOL_SIZE			5
#define	PLACE_MAP_FLAG_FILE_PATH		"\\spr\\ui3\\Ð¡µØÍ¼\\µØÍ¼Ð¡ÆìÖÄ.spr"
#define	MAP_SETTING_FILE				"\\Ui\\Setting.ini"
#define	TRAFFIC_INI_FILE				"\\Settings\\MapTraffic.ini"
#define	RIGHT_BOTTOM_NO_LIMIT			0x7fffffff

KScenePlaceMapC::KScenePlaceMapC()
{
	m_bHavePicMap = FALSE;
	m_bInited = FALSE;
	m_bPaintLine = FALSE;
	m_szEntireMapFile[0] = 0;
	m_EntireMapLTPosition.x = m_EntireMapLTPosition.y = 0;
	m_FocusPosition.x = m_FocusPosition.y = 0;
	m_FocusLimit.left = m_FocusLimit.right = m_FocusLimit.top = m_FocusLimit.bottom = 0;
	m_PicLoadedLTPosition.x = m_PicLoadedLTPosition.y = 0;
	m_Size.cx = m_Size.cy = 0;
	m_MapCoverArea.left = m_MapCoverArea.right = m_MapCoverArea.top = m_MapCoverArea.bottom = 0;
	m_PaintCell.left = m_PaintCell.right = m_PaintCell.top = m_PaintCell.bottom = 0;
	m_uMapShowElems = SCENE_PLACE_MAP_ELEM_NONE;
	memset(&m_ElemsList, 0, sizeof(m_ElemsList));
	m_pEntireMap = NULL;
	m_DirectPos.x = m_DirectPos.y = 0;
}

KScenePlaceMapC::~KScenePlaceMapC()
{
	Terminate();
}

//³õÊ¼»¯
bool KScenePlaceMapC::Initialize()
{
	if (m_bInited == false && g_pRepresent)
	{
		int nCount = MAP_CELL_MAX_RANGE * MAP_CELL_MAX_RANGE;
		int	nIndex, i;
		MAP_CELL* pCell = &m_ElemsList[0][0];
		for (nIndex = i = 0; nIndex < nCount && i < 0x03335688; i++)
		{
			sprintf(pCell->szImageName, "*PlaceMap_%d*", i);
			pCell->uImageId = g_pRepresent->CreateImage(
				pCell->szImageName, MAP_CELL_MAP_WIDTH, MAP_CELL_MAP_HEIGHT, ISI_T_BITMAP16);
			if (pCell->uImageId)
			{
				pCell++;
				nIndex++;
			}
		}
		m_bInited = true;

		KIniFile	ColorSetting;
		char		szBuffer[64];
		if (ColorSetting.Load(MAP_SETTING_FILE))
		{
			const char* pcszTemp;
			KRColor		Color;
			Color.Color_dw = 0;

			// Ö÷½ÇÑÕÉ«
			ColorSetting.GetString("Map", "SelfColor", "255,255,255", szBuffer, sizeof(szBuffer));
            pcszTemp = szBuffer;
            Color.Color_b.r = KSG_StringGetInt(&pcszTemp, 255);
            KSG_StringSkipSymbol(&pcszTemp, ',');
			Color.Color_b.g = KSG_StringGetInt(&pcszTemp, 255);
            KSG_StringSkipSymbol(&pcszTemp, ',');
			Color.Color_b.b = KSG_StringGetInt(&pcszTemp, 255);
			m_uSelfColor = Color.Color_dw;
			
			// ¶ÓÓÑÑÕÉ«
			ColorSetting.GetString("Map", "TeammateColor", "255,255,255", szBuffer, sizeof(szBuffer));
            pcszTemp = szBuffer;
			Color.Color_b.r = KSG_StringGetInt(&pcszTemp, 255);
            KSG_StringSkipSymbol(&pcszTemp, ',');
			Color.Color_b.g = KSG_StringGetInt(&pcszTemp, 255);
            KSG_StringSkipSymbol(&pcszTemp, ',');
			Color.Color_b.b = KSG_StringGetInt(&pcszTemp, 255);
			m_uTeammateColor = Color.Color_dw;

			// ÆäËûÍæ¼ÒÑÕÉ«
			ColorSetting.GetString("Map", "PlayerColor", "255,255,255", szBuffer, sizeof(szBuffer));
            pcszTemp = szBuffer;
			Color.Color_b.r = KSG_StringGetInt(&pcszTemp, 255);
            KSG_StringSkipSymbol(&pcszTemp, ',');
			Color.Color_b.g = KSG_StringGetInt(&pcszTemp, 255);
            KSG_StringSkipSymbol(&pcszTemp, ',');
			Color.Color_b.b = KSG_StringGetInt(&pcszTemp, 255);
			m_uPlayerColor = Color.Color_dw;

			// Õ½¶·npcÑÕÉ«
			ColorSetting.GetString("Map", "FightNpcColor", "255,255,255", szBuffer, sizeof(szBuffer));
            pcszTemp = szBuffer;
			Color.Color_b.r = KSG_StringGetInt(&pcszTemp, 255);
            KSG_StringSkipSymbol(&pcszTemp, ',');
			Color.Color_b.g = KSG_StringGetInt(&pcszTemp, 255);
            KSG_StringSkipSymbol(&pcszTemp, ',');
			Color.Color_b.b = KSG_StringGetInt(&pcszTemp, 255);
			m_uFightNpcColor = Color.Color_dw;

			// ÆÕÍ¨npcÑÕÉ«
			ColorSetting.GetString("Map", "NormalNpcColor", "255,255,255", szBuffer, sizeof(szBuffer));
            pcszTemp = szBuffer;
			Color.Color_b.r = KSG_StringGetInt(&pcszTemp, 255);
            KSG_StringSkipSymbol(&pcszTemp, ',');
			Color.Color_b.g = KSG_StringGetInt(&pcszTemp, 255);
            KSG_StringSkipSymbol(&pcszTemp, ',');
			Color.Color_b.b = KSG_StringGetInt(&pcszTemp, 255);
			m_uNormalNpcColor = Color.Color_dw;
			
			ColorSetting.GetString("Map", "SelfPartnerColor", "255,255,255", szBuffer, sizeof(szBuffer));
            pcszTemp = szBuffer;
			Color.Color_b.r = KSG_StringGetInt(&pcszTemp, 255);
            KSG_StringSkipSymbol(&pcszTemp, ',');
			Color.Color_b.g = KSG_StringGetInt(&pcszTemp, 255);
            KSG_StringSkipSymbol(&pcszTemp, ',');
			Color.Color_b.b = KSG_StringGetInt(&pcszTemp, 255);
			m_uSelfPartnerColor = Color.Color_dw;

			ColorSetting.GetString("Map", "OtherPartnerColor", "255,255,255", szBuffer, sizeof(szBuffer));
            pcszTemp = szBuffer;
			Color.Color_b.r = KSG_StringGetInt(&pcszTemp, 255);
            KSG_StringSkipSymbol(&pcszTemp, ',');
			Color.Color_b.g = KSG_StringGetInt(&pcszTemp, 255);
            KSG_StringSkipSymbol(&pcszTemp, ',');
			Color.Color_b.b = KSG_StringGetInt(&pcszTemp, 255);
			m_uOtherPartnerColor = Color.Color_dw;

			ColorSetting.GetString("Map", "OtherPlayerColor", "255,255,255", szBuffer, sizeof(szBuffer));
            pcszTemp = szBuffer;
			Color.Color_b.r = KSG_StringGetInt(&pcszTemp, 255);
            KSG_StringSkipSymbol(&pcszTemp, ',');
			Color.Color_b.g = KSG_StringGetInt(&pcszTemp, 255);
            KSG_StringSkipSymbol(&pcszTemp, ',');
			Color.Color_b.b = KSG_StringGetInt(&pcszTemp, 255);
			m_uOtherPlayerColor = Color.Color_dw;
			
			ColorSetting.GetString("Map", "OtherNpcColor", "255,255,255", szBuffer, sizeof(szBuffer));
            pcszTemp = szBuffer;
			Color.Color_b.r = KSG_StringGetInt(&pcszTemp, 255);
            KSG_StringSkipSymbol(&pcszTemp, ',');
			Color.Color_b.g = KSG_StringGetInt(&pcszTemp, 255);
            KSG_StringSkipSymbol(&pcszTemp, ',');
			Color.Color_b.b = KSG_StringGetInt(&pcszTemp, 255);
			m_uOtherNpcColor = Color.Color_dw;
			
			ColorSetting.GetString("Map", "SelfNpcColor", "255,255,255", szBuffer, sizeof(szBuffer));
            pcszTemp = szBuffer;
			Color.Color_b.r = KSG_StringGetInt(&pcszTemp, 255);
            KSG_StringSkipSymbol(&pcszTemp, ',');
			Color.Color_b.g = KSG_StringGetInt(&pcszTemp, 255);
            KSG_StringSkipSymbol(&pcszTemp, ',');
			Color.Color_b.b = KSG_StringGetInt(&pcszTemp, 255);
			m_uSelfNpcColor = Color.Color_dw;
			
			ColorSetting.GetString("Map", "SelfPlayerColor", "255,255,255", szBuffer, sizeof(szBuffer));
            pcszTemp = szBuffer;
			Color.Color_b.r = KSG_StringGetInt(&pcszTemp, 255);
            KSG_StringSkipSymbol(&pcszTemp, ',');
			Color.Color_b.g = KSG_StringGetInt(&pcszTemp, 255);
            KSG_StringSkipSymbol(&pcszTemp, ',');
			Color.Color_b.b = KSG_StringGetInt(&pcszTemp, 255);
			m_uSelfPlayerColor = Color.Color_dw;
		}
	}

	return m_bInited;
}

//½áÊø¶ÔÏó¹¦ÄÜ¡£ÊÍ·Å¶ÔÏóµÄÈ«²¿Êý¾ÝÓë¶¯Ì¬¹¹ÔìµÄ×ÊÔ´¡£
void KScenePlaceMapC::Terminate()
{
	if (m_bInited && g_pRepresent)
	{
		for (int v = 0; v < MAP_CELL_MAX_RANGE; v++)
		{
			for (int h = 0; h < MAP_CELL_MAX_RANGE; h++)
			{
				if (m_ElemsList[v][h].uImageId)
				{
					g_pRepresent->FreeImage(m_ElemsList[v][h].szImageName);
					m_ElemsList[v][h].uImageId = 0;
				}
				m_ElemsList[v][h].szImageName[0] = 0;
			}
		}
	}
}

//ÉèÖÃ³¡¾°µØÍ¼µÄ°üº¬µÄÔªËØ
void KScenePlaceMapC::SetShowElemsFlag(unsigned int uShowElemsFlag)
{
	m_uMapShowElems = uShowElemsFlag;
	if (m_uMapShowElems & SCENE_PLACE_MAP_ELEM_PIC)
		SetFocusPosition(m_FocusPosition.x, m_FocusPosition.y, true);
}

//¶ÁÈ¡ÉèÖÃ
BOOL KScenePlaceMapC::Load(KIniFile* pSetting, const char* pszScenePlaceRootPath)
{
	if (Initialize() == FALSE)
		return FALSE;
	Free();
	m_bHavePicMap = FALSE;
	if (pSetting && pszScenePlaceRootPath)
	{
		sprintf(m_szEntireMapFile, "%s" PLACE_MAP_FILE_NAME_APPEND, pszScenePlaceRootPath);
		m_bHavePicMap = g_FileExists(m_szEntireMapFile);
		if (m_bHavePicMap)
		{
			m_EntireMapLTPosition.x = -1;
			pSetting->GetInteger2(PLACE_MAP_SAVE_SECTION, "MapLTRegionIndex",
				(int*)&m_EntireMapLTPosition.x, (int*)m_EntireMapLTPosition.y);
			if (m_EntireMapLTPosition.x == -1)
			{
				RECT	rc;
				pSetting->GetRect(PLACE_MAP_SAVE_SECTION, "rect", &rc);
				m_EntireMapLTPosition.x = rc.left;
				m_EntireMapLTPosition.y = rc.top;
			}
			if (m_EntireMapLTPosition.x != -1)
			{
				m_EntireMapLTPosition.x *= KScenePlaceRegionC::RWPP_AREGION_WIDTH;
				m_EntireMapLTPosition.y *= KScenePlaceRegionC::RWPP_AREGION_HEIGHT;

				m_PicLoadedLTPosition.x = 0;
				m_PicLoadedLTPosition.y = 0;

				m_FocusLimit.left = m_EntireMapLTPosition.x + (m_Size.cx * MAP_SCALE_H / 2);
				m_FocusLimit.top  = m_EntireMapLTPosition.y + (m_Size.cy * MAP_SCALE_V / 2);
				m_FocusLimit.bottom = m_FocusLimit.right = RIGHT_BOTTOM_NO_LIMIT;

				m_bHavePicMap = TRUE;
			}
			else
			{
				m_bHavePicMap = FALSE;
			}
		}
	}
	return m_bHavePicMap;
}

//ÉèÖÃ³¡¾°µØÍ¼µÄ½¹µã
void KScenePlaceMapC::SetFocusPosition(int nX, int nY, bool bChangedRegion)
{
	if (nX < m_FocusLimit.left)
		m_FocusPosition.x = m_FocusLimit.left;
	else if (nX > m_FocusLimit.right)
		m_FocusPosition.x = m_FocusLimit.right;
	else
		m_FocusPosition.x = nX;
	if (nY < m_FocusLimit.top)
		m_FocusPosition.y = m_FocusLimit.top;
	else if (nY > m_FocusLimit.bottom)
		m_FocusPosition.y = m_FocusLimit.bottom;
	else
		m_FocusPosition.y = nY;

	m_MapCoverArea.left = m_FocusPosition.x / MAP_SCALE_H - m_Size.cx / 2;
	m_MapCoverArea.top  = m_FocusPosition.y / MAP_SCALE_V - m_Size.cy / 2;
	m_MapCoverArea.right  = m_MapCoverArea.left + m_Size.cx;
	m_MapCoverArea.bottom = m_MapCoverArea.top  + m_Size.cy;
	if (m_uMapShowElems & SCENE_PLACE_MAP_ELEM_PIC)
	{
		if (bChangedRegion && m_bHavePicMap)
		{
			POINT	CellIndex;
			CellIndex.x = (m_FocusPosition.x - m_PicLoadedLTPosition.x) / MAP_CELL_SCENE_WIDTH;
			CellIndex.y = (m_FocusPosition.y - m_PicLoadedLTPosition.y) / MAP_CELL_SCENE_HEIGHT;
			if (CellIndex.x != MAP_CELL_CENTRE_INDEX || //MAP_CELL_FOCUS_INDEX_MIN || CellIndex.x > MAP_CELL_FOCUS_INDEX_MAX ||
				CellIndex.y != MAP_CELL_CENTRE_INDEX)	//MAP_CELL_FOCUS_INDEX_MIN || CellIndex.y > MAP_CELL_FOCUS_INDEX_MAX)
			{
				m_PicLoadedLTPosition.x += (CellIndex.x - MAP_CELL_CENTRE_INDEX) * MAP_CELL_SCENE_WIDTH;
				m_PicLoadedLTPosition.y += (CellIndex.y - MAP_CELL_CENTRE_INDEX) * MAP_CELL_SCENE_HEIGHT;
				FillCellsPicInfo();
			}
		}
		CalcPicLayout();
	}
}

void KScenePlaceMapC::FillCellsPicInfo()
{
	if (m_pEntireMap == NULL)
	{
		if (m_bHavePicMap)
		{
			KBitmapDataBuffInfo	Info;
			short *pBuff = (short*)g_pRepresent->GetBitmapDataBuffer(
				m_ElemsList[0][0].szImageName, &Info);
			if (pBuff)
			{
				g_pRepresent->ReleaseBitmapDataBuffer(
						m_ElemsList[0][0].szImageName, pBuff);
				unsigned int uMask16 = -1;
				if (Info.eFormat == BDBF_16BIT_555)
					uMask16 = RGB_555;
				else if (Info.eFormat == BDBF_16BIT_565)
					uMask16 = RGB_565;
				if (uMask16 >= 0)
					m_pEntireMap = get_jpg_image(m_szEntireMapFile, uMask16);
			}			
		}
		if (m_pEntireMap)
		{
			m_FocusLimit.right = m_FocusLimit.left + (m_pEntireMap->nWidth - m_Size.cx)* MAP_SCALE_H;
			m_FocusLimit.bottom = m_FocusLimit.top + (m_pEntireMap->nHeight - m_Size.cy)* MAP_SCALE_V;
			if (m_FocusLimit.right < m_FocusLimit.left)
				m_FocusLimit.right = m_FocusLimit.left;
			if (m_FocusLimit.bottom < m_FocusLimit.top)
				m_FocusLimit.bottom = m_FocusLimit.top;
		}
		else
		{
			m_bHavePicMap = FALSE;
			CoreDataChanged(GDCNI_SWITCHING_MAPMODE, FALSE, 0);
			return;
		}
	}

	int	nStartX = (m_PicLoadedLTPosition.x - m_EntireMapLTPosition.x) / MAP_SCALE_H;
	int nStartY = (m_PicLoadedLTPosition.y - m_EntireMapLTPosition.y) / MAP_SCALE_V;

	SIZE	PicEntireSize;
	PicEntireSize.cx = m_pEntireMap->nWidth;
	PicEntireSize.cy = m_pEntireMap->nHeight;
	POINT	DestPos;
	
	int	h, v, x, y, nFromX, nFromY, nToX, nToY;
	for (v = 0, y = nStartY; v < MAP_CELL_MAX_RANGE; v++, y += MAP_CELL_MAP_HEIGHT)
	{
		nToY = y + MAP_CELL_MAP_HEIGHT;
		bool bCleared = false;
		if (y < 0 || nToY > PicEntireSize.cy)
		{//×ÝÏòÃ»ÓÐÈ«²¿ÂäÔÚÍ¼ÄÚ
			for (h = 0; h < MAP_CELL_MAX_RANGE; h++)
			{
				g_pRepresent->ClearImageData(m_ElemsList[v][h].szImageName,
					m_ElemsList[v][h].uImageId, m_ElemsList[v][h].sISPosition);
			}
			bCleared = true;
		}
		if (y < PicEntireSize.cy && nToY > 0)
		{
			if (y >= 0)
			{
				nFromY = y;
				DestPos.y = 0;
			}
			else
			{
				nFromY = 0;
				DestPos.y = -y;
			}

			if (nToY > PicEntireSize.cy)
				nToY = PicEntireSize.cy;

			for (h = 0, x = nStartX; h < MAP_CELL_MAX_RANGE; h++, x += MAP_CELL_MAP_WIDTH)
			{
				nToX = x + MAP_CELL_MAP_WIDTH;
				if (bCleared == false && (x < 0 || nToX > PicEntireSize.cx))
				{//ºáÏòÃ»ÓÐÈ«²¿ÂäÔÚÍ¼ÄÚ
					g_pRepresent->ClearImageData(m_ElemsList[v][h].szImageName,
						m_ElemsList[v][h].uImageId, m_ElemsList[v][h].sISPosition);
				}

				if (x < PicEntireSize.cx && nToX > 0)
				{
					if (x >= 0)
					{
						nFromX = x;
						DestPos.x = 0;
					}
					else
					{
						nFromX = 0;
						DestPos.x = -x;
					}
					if (nToX > PicEntireSize.cx)
						nToX = PicEntireSize.cx;
					
					short *pBuf = (short*)g_pRepresent->GetBitmapDataBuffer(
						m_ElemsList[v][h].szImageName, NULL);
					if (pBuf)
					{
						short* pEntire = (short*)(&m_pEntireMap->Data) +
							nFromY * PicEntireSize.cx + nFromX;
						short* pDest = pBuf + MAP_CELL_MAP_WIDTH * DestPos.y + DestPos.x;
						for (int i = nFromY; i < nToY; i++)
						{
							memcpy(pDest, pEntire, (nToX - nFromX) * 2);
							pEntire += PicEntireSize.cx;
							pDest += MAP_CELL_MAP_WIDTH;
						}
						g_pRepresent->ReleaseBitmapDataBuffer(
							m_ElemsList[v][h].szImageName, pBuf);
					}
				}
			}
		}
	}
}

void KScenePlaceMapC::CalcPicLayout()
{
	POINT	ShowLTPos;

	ShowLTPos.x = m_MapCoverArea.left - m_PicLoadedLTPosition.x / MAP_SCALE_H ;
	ShowLTPos.y = m_MapCoverArea.top  - m_PicLoadedLTPosition.y / MAP_SCALE_V ;
//	ShowLTPos.x = (m_FocusPosition.x - m_PicLoadedLTPosition.x) / MAP_SCALE_H - m_Size.cx / 2;
//	ShowLTPos.y = (m_FocusPosition.y - m_PicLoadedLTPosition.y) / MAP_SCALE_V - m_Size.cy / 2;

	m_PaintCell.left = ShowLTPos.x / MAP_CELL_MAP_WIDTH;
	m_PaintCell.top = ShowLTPos.y / MAP_CELL_MAP_HEIGHT;

	m_FirstCellSkipWidth.cx = ShowLTPos.x - m_PaintCell.left * MAP_CELL_MAP_WIDTH;
	m_FirstCellSkipWidth.cy = ShowLTPos.y - m_PaintCell.top * MAP_CELL_MAP_HEIGHT;

	int nTemp = m_Size.cx + m_FirstCellSkipWidth.cx + MAP_CELL_MAP_WIDTH - 1;
	m_PaintCell.right = nTemp / MAP_CELL_MAP_WIDTH;
	m_LastCellSkipHeight.cx = nTemp - MAP_CELL_MAP_WIDTH * m_PaintCell.right;
	m_PaintCell.right += m_PaintCell.left;

	nTemp = m_Size.cy + m_FirstCellSkipWidth.cy + MAP_CELL_MAP_HEIGHT - 1;
	m_PaintCell.bottom = nTemp / MAP_CELL_MAP_HEIGHT;
	m_LastCellSkipHeight.cy = nTemp - MAP_CELL_MAP_HEIGHT * m_PaintCell.bottom;
	m_PaintCell.bottom += m_PaintCell.top;
}

//Çå³ý
void KScenePlaceMapC::Free()
{
	if (m_pEntireMap)
	{
		release_image(m_pEntireMap);
		m_pEntireMap = NULL;
	}
}

//»æÖÆ
void KScenePlaceMapC::PaintMap(int nX, int nY)
{
	m_MapPos.left = nX;
	m_MapPos.top = nY;
	m_MapPos.right = m_MapPos.left + m_Size.cx-1;
	m_MapPos.bottom = m_MapPos.top + m_Size.cy-1;

	if (m_bHavePicMap && g_pRepresent)
	{
		//----»æÖÆËõÂÔÍ¼----
		if (m_uMapShowElems & SCENE_PLACE_MAP_ELEM_PIC)
			PaintMapPic(nX, nY);

		//---»æÖÆ¶ÓÓÑÎ»ÖÃ----
		PaintCharacters(nX, nY);

		//---»æÖÆ×Ô¼ºÎ»ÖÃ----
		int nNpcIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
		if (nNpcIdx >= 0 && Npc[nNpcIdx].m_RegionIndex >= 0)
		{
			int nNpcX = LOWORD(Npc[nNpcIdx].m_dwRegionID) * MAP_A_REGION_NUM_MAP_PIXEL_H +
				Npc[nNpcIdx].m_MapX * 2;
			int nNpcY = HIWORD(Npc[nNpcIdx].m_dwRegionID) * MAP_A_REGION_NUM_MAP_PIXEL_V +
				Npc[nNpcIdx].m_MapY;

			if (nNpcX >= m_MapCoverArea.left && nNpcX < m_MapCoverArea.right &&
				nNpcY >= m_MapCoverArea.top  && nNpcY < m_MapCoverArea.bottom)
			{
				KRUShadow	FootSpot;
				FootSpot.Color.Color_dw = m_uSelfColor;
				FootSpot.oPosition.nX = nX + nNpcX - m_MapCoverArea.left - 1;
				FootSpot.oPosition.nY = nY + nNpcY - m_MapCoverArea.top  - 1;
				FootSpot.oEndPos.nX = FootSpot.oPosition.nX + 3;
				FootSpot.oEndPos.nY = FootSpot.oPosition.nY + 3;
				g_pRepresent->DrawPrimitives(1, &FootSpot, RU_T_SHADOW, true);
			}
			PaintFindPos(nX + m_DirectPos.x - m_MapCoverArea.left - 1, 
				nY + m_DirectPos.y - m_MapCoverArea.top  - 1, 
				nX + nNpcX - m_MapCoverArea.left - 1, 
				nY + nNpcY - m_MapCoverArea.top  - 1);
			PaintSymbol(nX);
		}
	}
}

void KScenePlaceMapC::LoadSymbol(int nSubWorldID)
{
	char szSectName[10];
	itoa(nSubWorldID, szSectName, 10);

	if (g_MapTraffic.IsSectionExist(szSectName))
	{
		char szKeyName[128];
		for(int j = 0; j < defMAX_NUM_SYMBOL; j++)
		{
			m_SymbolMap[j].Release();

			itoa(nSubWorldID, szSectName, 10);
			sprintf(szKeyName, "%d_Type", j + 1);
			g_MapTraffic.GetInteger(szSectName, szKeyName, -1, &m_SymbolMap[j].Kind);

			if (m_SymbolMap[j].Kind <= -1)
				continue;
			
			int nValue1, nValue2;
			switch (m_SymbolMap[j].Kind)
			{
				case MapDirect:
					sprintf(szKeyName, "%d_Point", j + 1);
					g_MapTraffic.GetInteger2(szSectName, szKeyName, &nValue1, &nValue2);
					m_SymbolMap[j].Point.x = nValue1;
					m_SymbolMap[j].Point.y = nValue2;
					sprintf(szKeyName, "%d_TargetPoint", j + 1);
					g_MapTraffic.GetInteger2(szSectName, szKeyName, &nValue1, &nValue2);
					m_SymbolMap[j].TPoint.x = nValue1;
					m_SymbolMap[j].TPoint.y = nValue2;
					sprintf(szKeyName, "%d_Index", j + 1);
					g_MapTraffic.GetInteger(szSectName, szKeyName, 0, &m_SymbolMap[j].MapIndex);
					memset(m_SymbolMap[j].Content, 0, sizeof(m_SymbolMap[j].Content));
				break;
				case SymPoint:
				case SymName:
					sprintf(szKeyName, "%d_Point", j + 1);
					g_MapTraffic.GetInteger2(szSectName, szKeyName, &nValue1, &nValue2);
					m_SymbolMap[j].Point.x = nValue1;
					m_SymbolMap[j].Point.y = nValue2;
					m_SymbolMap[j].TPoint.x = 0;
					m_SymbolMap[j].TPoint.y = 0;
					m_SymbolMap[j].MapIndex = 0;
					sprintf(szKeyName, "%d_Content", j + 1);
					g_MapTraffic.GetString(szSectName, szKeyName, "", m_SymbolMap[j].Content, sizeof(m_SymbolMap[j].Content));
				break;
			}
		}
	}
	else
	{
		for(int j = 0; j < defMAX_NUM_SYMBOL; j++)
			m_SymbolMap[j].Release();
	}
}

void KScenePlaceMapC::PaintSymbol(int nX)
{
	if (g_pRepresent)
	{		
		int nNpcX = 0;
		int nNpcY = 0;
		for(int j = 0; j < defMAX_NUM_SYMBOL; j++)
		{
			if (m_SymbolMap[j].Kind <= -1)
				continue;

			nNpcX = m_SymbolMap[j].Point.x + nX + (m_FocusLimit.left / MAP_SCALE_H - m_Size.cx / 2) - m_MapCoverArea.left - PLACE_MAP_SYMBOL_SIZE;
			nNpcY = m_SymbolMap[j].Point.y + m_MapPos.top + (m_FocusLimit.top / MAP_SCALE_V - m_Size.cy / 2) - m_MapCoverArea.top - PLACE_MAP_SYMBOL_SIZE;
			switch (m_SymbolMap[j].Kind)
			{
				case MapDirect:
				break;
				case SymName:
				{
					POINT NamePoint;
					RECT NamePos;
					int NameLen = g_StrLen(m_SymbolMap[j].Content) * NORMAL_FONTSIZE / 2;
					NamePoint.x = NamePos.left = nNpcX - (NameLen / 2);
					NamePos.right = nNpcX + (NameLen / 2);
					NamePoint.y = NamePos.top = nNpcY;
					NamePos.bottom = nNpcY + (NORMAL_FONTSIZE + 1);

					if (NamePos.left < m_MapPos.left)
						NamePoint.x = m_MapPos.left;
					if (NamePos.right > m_MapPos.right - (NORMAL_FONTSIZE / 2))
						NamePoint.x -= (NamePos.right - m_MapPos.right + (NORMAL_FONTSIZE / 2));
					if (NamePos.top < m_MapPos.top)
						NamePoint.y = m_MapPos.top;
					if (NamePos.bottom > m_MapPos.bottom)
						NamePoint.y -= (NamePos.bottom - m_MapPos.bottom);
					
					if (NamePoint.x > nNpcX)
						continue;
					if (NamePoint.y + (NORMAL_FONTSIZE + 1) < nNpcY)
						continue;
					if (NamePoint.x < NamePos.left - (NameLen / 2))
						continue;
					if (NamePoint.y > NamePos.bottom)
						continue;
						
					g_pRepresent->OutputText(NORMAL_FONTSIZE, m_SymbolMap[j].Content, KRF_ZERO_END, NamePoint.x, NamePoint.y, 0x00ffff00);
				}
				break;
				case SymPoint:
				{
					KRUImage	RUSymbol;
					strcpy(RUSymbol.szImage, m_SymbolMap[j].Content);

					KImageParam	Param;
					g_pRepresent->GetImageParam(m_SymbolMap[j].Content, &Param, ISI_T_SPR);
					
					BYTE btSymbolW = Param.nWidth;
					BYTE btSymbolH = Param.nHeight;					
					
					RUSymbol.nType = ISI_T_SPR;
					RUSymbol.Color.Color_b.a = 255;
					RUSymbol.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
					RUSymbol.uImage = 0;
					RUSymbol.nISPosition = IMAGE_IS_POSITION_INIT;
					RUSymbol.bRenderFlag = 0;
					RUSymbol.nFrame = 0;
					RUSymbol.oPosition.nX = nNpcX;
					RUSymbol.oPosition.nY = nNpcY;
					RUSymbol.oPosition.nZ = 0;

					RECT PosSymbol;
					
					PosSymbol.left = RUSymbol.oPosition.nX + btSymbolW;
					PosSymbol.right = RUSymbol.oPosition.nX;
					PosSymbol.top = RUSymbol.oPosition.nY + btSymbolH;
					PosSymbol.bottom = RUSymbol.oPosition.nY + btSymbolH;
					
					if ((PosSymbol.left - btSymbolW) < m_MapPos.left)
						RUSymbol.oPosition.nX += m_MapPos.left - (PosSymbol.left - btSymbolW);
					if (PosSymbol.top - btSymbolH < m_MapPos.top)
						RUSymbol.oPosition.nY += m_MapPos.top - (PosSymbol.bottom - btSymbolH);
					if (PosSymbol.bottom > m_MapPos.bottom)
						RUSymbol.oPosition.nY -= PosSymbol.bottom - m_MapPos.bottom;
					if (PosSymbol.right + btSymbolW > m_MapPos.right)
						RUSymbol.oPosition.nX -= (PosSymbol.right + btSymbolW) - m_MapPos.right;
						
					if (PosSymbol.left < m_MapPos.left || 
						PosSymbol.top < m_MapPos.top || 
						PosSymbol.right > m_MapPos.right || 
						PosSymbol.bottom > m_MapPos.bottom + btSymbolH)
						continue;	
					g_pRepresent->DrawPrimitives(1, &RUSymbol, RU_T_IMAGE, true);
				}
				break;
			}
		}
	}
}

void KScenePlaceMapC::PaintFindPos(int nX, int nY, int nDesX, int nDesY)
{
	if (!g_ScenePlace.bPaintMode)
		return;
	
	int nNpcIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	int nNpcX = LOWORD(Npc[nNpcIdx].m_dwRegionID) * MAP_A_REGION_NUM_MAP_PIXEL_H + Npc[nNpcIdx].m_MapX * 2;
	int nNpcY = HIWORD(Npc[nNpcIdx].m_dwRegionID) * MAP_A_REGION_NUM_MAP_PIXEL_V + Npc[nNpcIdx].m_MapY;

	BOOL bPaintFlag = TRUE;

	if (m_bPaintLine && 
		m_DirectPos.x - nNpcX > -PLACE_MAP_SYMBOL_SIZE && 
		m_DirectPos.x - nNpcX < PLACE_MAP_SYMBOL_SIZE && 
		m_DirectPos.y - nNpcY > -PLACE_MAP_SYMBOL_SIZE && 
		m_DirectPos.y - nNpcY < PLACE_MAP_SYMBOL_SIZE)
		g_ScenePlace.bPaintMode = false;

	if (nNpcIdx && Npc[nNpcIdx].m_RegionIndex >= 0)
	{
		if (nX < m_MapPos.left)
		{
			nX = m_MapPos.left;
			bPaintFlag = FALSE;
		}
		if (nY < m_MapPos.top)
		{
			nY = m_MapPos.top;
			bPaintFlag = FALSE;
		}
		if (nX > m_MapPos.right)
		{
			nX = m_MapPos.right;
			bPaintFlag = FALSE;
		}
		if (nY > m_MapPos.bottom)
		{
			nY = m_MapPos.bottom;
			bPaintFlag = FALSE;
		}

		if (nDesX < m_MapPos.left)
			nDesX = m_MapPos.left;
		if (nDesY < m_MapPos.top)
			nDesY = m_MapPos.top;
		if (nDesX > m_MapPos.right)
			nDesX = m_MapPos.right;
		if (nDesY > m_MapPos.bottom)
			nDesY = m_MapPos.bottom;
				
		if (m_bPaintLine)
		{
			KRULine Line;
			Line.oPosition.nX = nX;
			Line.oPosition.nY = nY;
			Line.Color.Color_dw = 0xff00ff00;
			Line.oPosition.nZ = Line.oEndPos.nZ = 0;
			Line.oEndPos.nX = nDesX + 1;
			Line.oEndPos.nY = nDesY + 1;
			g_pRepresent->DrawPrimitives(1, &Line, RU_T_LINE, 1);
		}

		if (bPaintFlag)
		{
			KRUImage Image;
			Image.nType = ISI_T_SPR;
			Image.Color.Color_b.a = 255;
			Image.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
			Image.uImage = 0;
			Image.nFrame = 0;
			Image.nISPosition = IMAGE_IS_POSITION_INIT;
			Image.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
			strcpy(Image.szImage, PLACE_MAP_FLAG_FILE_PATH);
			Image.oPosition.nX = nX;
			Image.oPosition.nY = nY - 12;
			Image.oPosition.nZ = 0;
			g_pRepresent->DrawPrimitives(1, &Image, RU_T_IMAGE, 1);
		}
	}
}
//////////////////////////////////////////////////////////////////////////

//»æÖÆ¶ÓÓÑÎ»ÖÃ
void KScenePlaceMapC::PaintCharacters(int nX, int nY)
{
	bool bCharacters = (m_uMapShowElems & SCENE_PLACE_MAP_ELEM_CHARACTER) != 0;
	bool bPartners = (m_uMapShowElems & SCENE_PLACE_MAP_ELEM_PARTNER) != 0;
	if (bCharacters == false || bPartners == false)
		return;

#define	MAX_NUM_PARTNER		8
	POINT		oPartnerPos[MAX_NUM_PARTNER];
	int			nNumPartner = 0;
	bool		bIsInTeam = Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag;
#define	MAX_NUM_CHARACTER	40
	KRUShadow	FootSpot[MAX_NUM_CHARACTER];
	int			nNumSpot = 0;

	// ÏÔÊ¾ÆäËûÍæ¼ÒºÍÆÕÍ¨npc
	int nNpcIdx = 0;
	while (nNpcIdx = NpcSet.GetNextIdx(nNpcIdx))
	{
		if (Npc[nNpcIdx].m_RegionIndex == -1)
			continue;
	
		int nNpcX = LOWORD(Npc[nNpcIdx].m_dwRegionID) * MAP_A_REGION_NUM_MAP_PIXEL_H +
						Npc[nNpcIdx].m_MapX * 2;
		int nNpcY = HIWORD(Npc[nNpcIdx].m_dwRegionID) * MAP_A_REGION_NUM_MAP_PIXEL_V +
						Npc[nNpcIdx].m_MapY;
		if (nNpcX < m_MapCoverArea.left || nNpcX >= m_MapCoverArea.right ||
			nNpcY < m_MapCoverArea.top  || nNpcY >= m_MapCoverArea.bottom)
		{
			continue;
		}

		bool			bValidNpc = false;
		bool			bIsInMsgr = ((Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nMissionGroup != -1) && 
								(Npc[nNpcIdx].m_nMissionGroup != -1));		
		unsigned int	uColor;

		if (Npc[nNpcIdx].m_Kind == kind_normal && bCharacters)
		{
			bValidNpc = true;
			uColor = m_uFightNpcColor;
			
			if (bIsInMsgr)
			{
				if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nMissionGroup == Npc[nNpcIdx].m_nMissionGroup)
					uColor = m_uSelfNpcColor;
				else
					uColor = m_uOtherNpcColor;
			}
		}
		else if (Npc[nNpcIdx].m_Kind == kind_dialoger && bCharacters)
		{
			bValidNpc = true;
			uColor = m_uNormalNpcColor;
		}
		else if (Npc[nNpcIdx].m_Kind == kind_player && nNpcIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex)
		{
			if (bPartners && //ÒªÏÔÊ¾¶ÓÓÑ
				bIsInTeam && //Ö÷½ÇÊÇ×é¶Ó×´Ì¬
				((DWORD)g_Team[0].m_nCaptain == Npc[nNpcIdx].m_dwID ||	//ÊÇÖ÷½ÇËùÔÚ¶ÓÎéµÄ¶Ó³¤
					g_Team[0].FindMemberID(Npc[nNpcIdx].m_dwID) >= 0) &&	//ÊÇÖ÷½ÇËùÔÚ¶ÓÎéµÄ¶ÓÔ±
				nNumPartner < MAX_NUM_PARTNER)	
			{
				oPartnerPos[nNumPartner].x = nNpcX;
				oPartnerPos[nNumPartner].y = nNpcY;
				nNumPartner ++;
			}
			else if (bCharacters)
			{
				bValidNpc = true;
				uColor = m_uPlayerColor;
				
				if (NpcSet.GetRelation(Player[CLIENT_PLAYER_INDEX].m_nIndex, nNpcIdx) == relation_ally)
				{
					uColor = m_uSelfPartnerColor;
				}
				if (bIsInMsgr)
				{
					if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nMissionGroup == Npc[nNpcIdx].m_nMissionGroup)
						uColor = m_uSelfPlayerColor;
					else
						uColor = m_uOtherPlayerColor;
				}				
			}
			if(Npc[nNpcIdx].m_HideState.nTime > 0)
			{
				bValidNpc = false;
			}
		}
		if (bValidNpc)
		{
			FootSpot[nNumSpot].Color.Color_dw = uColor;
			FootSpot[nNumSpot].oPosition.nX = nX + nNpcX - m_MapCoverArea.left - 1;
			FootSpot[nNumSpot].oPosition.nY = nY + nNpcY - m_MapCoverArea.top  - 1;
			FootSpot[nNumSpot].oEndPos.nX = FootSpot[nNumSpot].oPosition.nX + 3;
			FootSpot[nNumSpot].oEndPos.nY = FootSpot[nNumSpot].oPosition.nY + 3;
			nNumSpot++;
			if (nNumSpot == MAX_NUM_CHARACTER)
			{
				g_pRepresent->DrawPrimitives(MAX_NUM_CHARACTER, &FootSpot[0], RU_T_SHADOW, true);
				nNumSpot = 0;
			}
		}
	}

	if (nNumSpot)
	{
		g_pRepresent->DrawPrimitives(nNumSpot, &FootSpot[0], RU_T_SHADOW, true);
		nNumSpot = 0;
	}
	if (nNumPartner)
	{
		_ASSERT(nNumPartner <= MAX_NUM_CHARACTER);
		for (int i = 0; i < nNumPartner; i++)
		{
			FootSpot[i].Color.Color_dw = m_uTeammateColor;
			FootSpot[i].oPosition.nX = nX + oPartnerPos[i].x - m_MapCoverArea.left - 1;
			FootSpot[i].oPosition.nY = nY + oPartnerPos[i].y - m_MapCoverArea.top  - 1;
			FootSpot[i].oEndPos.nX = FootSpot[i].oPosition.nX + 3;
			FootSpot[i].oEndPos.nY = FootSpot[i].oPosition.nY + 3;
		}
		g_pRepresent->DrawPrimitives(nNumPartner, &FootSpot[0], RU_T_SHADOW, true);		
	}
}

//»æÖÆËõÂÔÍ¼
void KScenePlaceMapC::PaintMapPic(int nX, int nY)
{
	//_ASSERT(g_pRepresent);

	KRUImagePart	Img;
	Img.bRenderFlag = 0;
	Img.bRenderStyle = IMAGE_RENDER_STYLE_OPACITY;
	Img.Color.Color_dw = 0;
	Img.nFrame = 0;
	Img.nType = ISI_T_BITMAP16;
	Img.oPosition.nY = nY;

	for (int v = m_PaintCell.top; v < m_PaintCell.bottom; v++)
	{
		if (v != m_PaintCell.top)
			Img.oImgLTPos.nY = 0;
		else
			Img.oImgLTPos.nY = m_FirstCellSkipWidth.cy;
		if (v != m_PaintCell.bottom - 1)
			Img.oImgRBPos.nY = MAP_CELL_MAP_HEIGHT;
		else
			Img.oImgRBPos.nY = m_LastCellSkipHeight.cy;

		Img.oPosition.nX = nX;
		for (int h = m_PaintCell.left; h < m_PaintCell.right; h++)
		{
			if (h != m_PaintCell.left)
				Img.oImgLTPos.nX = 0;
			else
				Img.oImgLTPos.nX = m_FirstCellSkipWidth.cx;
			if (h != m_PaintCell.right - 1)
				Img.oImgRBPos.nX = MAP_CELL_MAP_WIDTH;
			else
				Img.oImgRBPos.nX = m_LastCellSkipHeight.cx;

			Img.nISPosition = m_ElemsList[v][h].sISPosition;
			strcpy(Img.szImage, m_ElemsList[v][h].szImageName);
			Img.uImage = m_ElemsList[v][h].uImageId;
			g_pRepresent->DrawPrimitives(1, &Img, RU_T_IMAGE_PART, true);
			m_ElemsList[v][h].sISPosition = Img.nISPosition;
			m_ElemsList[v][h].uImageId = Img.uImage;

			Img.oPosition.nX += Img.oImgRBPos.nX - Img.oImgLTPos.nX;
		}
		Img.oPosition.nY += Img.oImgRBPos.nY - Img.oImgLTPos.nY;
	};
}

//ÉèÖÃÐ¡µØÍ¼µÄ´óÐ¡£¨µ¥Î»£ºÏñËØµã£©
void KScenePlaceMapC::SetSize(int cx, int cy)
{
	if (m_Size.cx != cx || m_Size.cy != cy)
	{
		if (cx > MAP_MAX_SUPPORT_WIDTH)
			cx = MAP_MAX_SUPPORT_WIDTH;
		else if (cx < 0)
			cx = 0;
		if (cy > MAP_MAX_SUPPORT_HEIGHT)
			cy = MAP_MAX_SUPPORT_HEIGHT;
		else if (cy < 0)
			cy = 0;
		int nDLimit = (cx - m_Size.cx) * MAP_SCALE_H / 2;
		m_FocusLimit.left += nDLimit;
		if (m_FocusLimit.right != RIGHT_BOTTOM_NO_LIMIT)
		{
			m_FocusLimit.right -= nDLimit;
			if (m_FocusLimit.right < m_FocusLimit.left)
				m_FocusLimit.right = m_FocusLimit.left;
		}
		nDLimit = (cy - m_Size.cy) * MAP_SCALE_V / 2;
		m_FocusLimit.top += nDLimit;
		if (m_FocusLimit.bottom != RIGHT_BOTTOM_NO_LIMIT)
		{
			m_FocusLimit.bottom -= nDLimit;
			if (m_FocusLimit.bottom < m_FocusLimit.top)
				m_FocusLimit.bottom = m_FocusLimit.top;
		}

		m_Size.cx = cx;
		m_Size.cy = cy;
		SetFocusPosition(m_FocusPosition.x, m_FocusPosition.y, true);
	}
}

//·µ»ØÖµ±íÊ¾ÊÇ·ñÓÐµØÍ¼
int KScenePlaceMapC::GetMapRect(RECT* pRect)
{
	if (pRect)
	{
		pRect->left = m_EntireMapLTPosition.x;
		pRect->top  = m_EntireMapLTPosition.y;
		if (m_pEntireMap)
		{
			pRect->right = m_EntireMapLTPosition.x + m_pEntireMap->nWidth * MAP_SCALE_H;
			pRect->bottom = m_EntireMapLTPosition.y + m_pEntireMap->nHeight * MAP_SCALE_V;
		}
		else
		{
			pRect->right = m_EntireMapLTPosition.x;
			pRect->bottom = m_EntireMapLTPosition.y;
		}
	}
	return m_bHavePicMap;
}

BOOL KScenePlaceMapC::OnDirectMap(int nX, int nY)
{
	int nNpcX, nNpcY;

	for(int j = 0; j < defMAX_NUM_SYMBOL; j++)
	{
		if (m_SymbolMap[j].Kind != MapDirect)
			continue;
		nNpcX = m_SymbolMap[j].Point.x + m_MapPos.left + (m_FocusLimit.left / MAP_SCALE_H - m_Size.cx / 2) - m_MapCoverArea.left;
		nNpcY = m_SymbolMap[j].Point.y + (m_FocusLimit.top / MAP_SCALE_V - m_Size.cy / 2) - m_MapCoverArea.top;
		if ((nX >= nNpcX - PLACE_MAP_SYMBOL_SIZE && nX <= nNpcX + PLACE_MAP_SYMBOL_SIZE ) && 
			(nY >= nNpcY - PLACE_MAP_SYMBOL_SIZE && nY <= nNpcY + PLACE_MAP_SYMBOL_SIZE ))
			return TRUE;			
	}
	return FALSE;
}

void KScenePlaceMapC::DoDirectMap(int nX, int nY)
{
	int nNpcX, nNpcY;

	for(int j = 0; j < defMAX_NUM_SYMBOL; j++)
	{
		if (m_SymbolMap[j].Kind != MapDirect)
			continue;
		nNpcX = m_SymbolMap[j].Point.x + m_MapPos.left + (m_FocusLimit.left / MAP_SCALE_H - m_Size.cx / 2) - m_MapCoverArea.left;
		nNpcY = m_SymbolMap[j].Point.y + (m_FocusLimit.top / MAP_SCALE_V - m_Size.cy / 2) - m_MapCoverArea.top;
		if ((nX >= nNpcX - PLACE_MAP_SYMBOL_SIZE && nX <= nNpcX + PLACE_MAP_SYMBOL_SIZE ) && 
			(nY >= nNpcY - PLACE_MAP_SYMBOL_SIZE && nY <= nNpcY + PLACE_MAP_SYMBOL_SIZE ))
		{
			break;
		}
	}
}

void KScenePlaceMapC::DirectFindPos(int nX, int nY, BOOL bSync, BOOL bPaintLine)
{
	if(nX == 0 || nY == 0)
	{
		g_ScenePlace.bPaintMode = false;
		return;
	}

	int nDesX, nDesY;

	if (bSync)
	{
		nDesX = nX - m_MapPos.left + m_MapCoverArea.left + 1;
		nDesY = nY - m_MapPos.top + m_MapCoverArea.top + 1;
		if ( nDesX < m_MapCoverArea.left || 
			nDesY < m_MapCoverArea.top || 
			nDesX > m_MapCoverArea.right || 
			nDesY > m_MapCoverArea.bottom)
		{
			return;
		}
	}
	else
	{
		nDesX = nX * MAP_SCALE_H + 8;
		nDesY = nY * MAP_SCALE_H + 8;
	}
	m_DirectPos.x = nDesX;
	m_DirectPos.y = nDesY;
	m_bPaintLine = bPaintLine;
}

void KScenePlaceMapC::AutoRunTo(int nX, int nY)
{
}