/********************************************************************
File        : UiPlayerControlBar.cpp
Author		: Hoang.JX1Team
*********************************************************************/

#include "KWin32.h"
#include "KIniFile.h"
#include "CoreShell.h"
#include "GameDataDef.h"
#include "CoreUseNameDef.h"
#include "../elem/PopupMenu.h"
#include "../elem/MouseHover.h"
#include "../elem/Wnds.h"
#include "../Elem/WndMessage.h"
#include "../UiShell.h"
#include "../UiBase.h"
#include "../UiSoundSetting.h"
#include "../../../core/src/CoreShell.h"
#include "../../../Engine/src/Text.h"
#include    "../../Engine/Src/KSG_StringProcess.h"
#include "../../../Engine/src/KDebug.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "UiPlayerControlBar.h"
#include "UiAutoPlay.h"
#include "UiTongManager.h"
#include <crtdbg.h>

#define SCHEME_INI				"UiPlayerControlBar.ini"

KUiPlayerControlBar* KUiPlayerControlBar::m_pSelf = NULL;

extern iRepresentShell*	g_pRepresentShell;
extern iCoreShell *g_pCoreShell;


KUiPlayerControlBar::KUiPlayerControlBar()
{	
	m_BuffListCount = 0;
	m_nListCount = 0;
	m_pStateSkill = NULL;
}

KUiPlayerControlBar::~KUiPlayerControlBar()
{	
	if (m_pStateSkill)
	{
		delete m_pStateSkill;
		m_pStateSkill = NULL;
	}
}

KUiPlayerControlBar* KUiPlayerControlBar::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiPlayerControlBar;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
		m_pSelf->Show();
	return m_pSelf;
}


/*********************************************************************
**********************************************************************/
KUiPlayerControlBar* KUiPlayerControlBar::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}


/*********************************************************************
**********************************************************************/
void KUiPlayerControlBar::CloseWindow(bool bDestory)
{
	if (m_pSelf)
	{
		m_pSelf->Hide();
		if (bDestory)
		{
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
	}
}


/*********************************************************************
**********************************************************************/
void KUiPlayerControlBar::Initialize()
{
	for (int i = 0; i < MAX_BUFF_ROLE; i++)
	{
		AddChild(&m_BuffImage[i]);
		AddChild(&m_txtBuffTime[i]);
	}	

	for (int i = 0; i < MAX_ITEMBUTTON; i++)
		AddChild(&m_ItemBtn[i]);
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	Wnd_AddWindow(this, WL_LOWEST);
}

void KUiPlayerControlBar::LoadScheme(const char* pScheme)
{
	if (m_pSelf)
	{
		char		Buff[128];
		sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
		if (m_pSelf->Ini.Load(Buff))
		{
			m_pSelf->KWndImage::Init(&m_pSelf->Ini, "Main");
			int nDiff, nStart, nBuffTop, ntxtBuffTop;
			m_pSelf->Ini.GetInteger2("BuffImage", "StartDiff", &nStart, &nDiff);
			m_pSelf->Ini.GetInteger("BuffImage", "BuffTop", 0, &nBuffTop);
			m_pSelf->Ini.GetInteger("BuffImage", "txtBuffTop", 0, &ntxtBuffTop);
			for (int i = 0; i < MAX_BUFF_ROLE; i++)
			{
				m_pSelf->m_BuffImage[i].Init(&m_pSelf->Ini, "BuffImage");
				m_pSelf->m_BuffImage[i].SetPosition(nStart, nBuffTop);
				m_pSelf->m_txtBuffTime[i].Init(&m_pSelf->Ini, "txtBuffTime");
				m_pSelf->m_txtBuffTime[i].SetPosition(nStart, ntxtBuffTop);
				nStart += nDiff;
			}
		m_pSelf->Ini.GetInteger("BuffList", "BuffCount", 0, &m_pSelf->m_BuffListCount);
		for (int i = 0; i < MAX_ITEMBUTTON; i++)
			m_pSelf->m_ItemBtn[i].SetPosition(HIDE_POS);
		}
	}
}
/*********************************************************************
**********************************************************************/

void KUiPlayerControlBar::Breathe()
{
	if (g_pCoreShell)
	{
		if (m_BuffListCount)
		{
			Release();
			int nCount = g_pCoreShell->GetGameData(GDI_NPC_STATE_SKILL, 0, 0);
			if (nCount)
			{
				if (m_pStateSkill = (KStateControl*)malloc(sizeof(KStateControl) * nCount))
				{
					g_pCoreShell->GetGameData(GDI_NPC_STATE_SKILL, (unsigned int)m_pStateSkill, nCount);//µ¥Ïß³ÌÖ´ÐÐ£¬nCountÖµ²»±ä
					int nSkillID;
					for (int i = 0; i < nCount; i++)
					{
						char szKey[16];
						for (int j = 0; j < m_BuffListCount; j ++)
						{
							sprintf(szKey, "Buff_%d_ID", j);
							Ini.GetInteger("BuffList", szKey, 0, &nSkillID);
							if (nSkillID == m_pStateSkill[i].nSkillId)
							{
								Add(j, &m_pStateSkill[i]);
								break;
							}
						}
					}
				}
			}
		}
	}
}

void KUiPlayerControlBar::Add(int nSkillPos, KStateControl* pStateSkill)
{
	if (m_nListCount >= MAX_BUFF_ROLE)
		return;

	char szKeyName[16], szTemp[128], pszDesc[MAX_MESSAGE_LENGTH];;
	memset(pszDesc, 0, sizeof(pszDesc));

	sprintf(szKeyName, "Buff_%d_Image", nSkillPos);
	Ini.GetString("BuffList", szKeyName, "",  szTemp, sizeof(szTemp));	

	m_BuffImage[m_nListCount].Show();
	m_BuffImage[m_nListCount].SetImage(ISI_T_SPR, szTemp, true);
	
	sprintf(szKeyName, "Buff_%d_Name", nSkillPos);
	Ini.GetString("BuffList", szKeyName, "",  pszDesc, sizeof(pszDesc));			
	strcat(pszDesc, "\n");

	sprintf(szKeyName, "Buff_%d_Desc", nSkillPos);
	Ini.GetString("BuffList", szKeyName, "",  szTemp, sizeof(szTemp));				
	if (szTemp[0])
	{
		int offset = 0, nL = 0;
		int nStrL = strlen(szTemp);
		while(szTemp[offset] != 0)
		{
			if(szTemp[offset] == '<')
			{
				if(szTemp[offset+1] == 'e' &&
					szTemp[offset+2] == 'n' &&
					szTemp[offset+3] == 't' &&
					szTemp[offset+4] == 'e' &&
					szTemp[offset+5] == 'r' &&
					szTemp[offset+6] == '>')
					nL = 0;
				if(szTemp[offset+1] == 'c' &&
					szTemp[offset+2] == 'o' &&
					szTemp[offset+3] == 'l' &&
					szTemp[offset+4] == 'o' &&
					szTemp[offset+5] == 'r')
				{
					if (szTemp[offset+6] == '>')
						nL -= 6;
					else
					{
						int k;
						for(k = 0; k<12; k++)
						{
							if(szTemp[offset+6+k] == '>') 
								break;
						}
						nL -= 6+k;
					}
				}
				if(szTemp[offset+1] == 'b' &&
					szTemp[offset+2] == 'c' &&
					szTemp[offset+3] == 'l' &&
					szTemp[offset+4] == 'r')
				{
					if (szTemp[offset+5] == '>')
						nL -= 5;
					else
					{
						if (szTemp[offset+5] == '=')
						{
							int k;
							for(k = 0; k<12; k++)
							{
								if(szTemp[offset+5+k] == '>') 
									break;
							}
							nL -= 5+k;
						}
					}
				}
			}

			if(nL == 32)
			{
				memmove(&szTemp[offset+7], &szTemp[offset], nStrL-offset+1);
				memcpy(&szTemp[offset],"<enter>",7);
				offset += 7;
				nStrL += 7;
				nL = 0;
			}
			offset++;
			nL++;
		}
		if(strlen(szTemp) > 127)
			szTemp[127] = 0;

		strcat(pszDesc, szTemp);
	}			
	strcat(pszDesc, "\n");

	if (pStateSkill->nLeftTime <= GAME_FPS)
	{
		strcpy(szTemp, "N/A");
		strcat(pszDesc, "Duy tr× vÜnh viÔn");
	}
	else
	{
		sprintf(szTemp, "%02dh:%02dm:%02ds", pStateSkill->nLeftTime/18/60/60, pStateSkill->nLeftTime/18/60%60, pStateSkill->nLeftTime/18%60);
		strcat(pszDesc, szTemp);

		if (pStateSkill->nLeftTime >= 64800)
			sprintf(szTemp, "%dh", pStateSkill->nLeftTime/18/60/60);
		else if (pStateSkill->nLeftTime >= 1080)
			sprintf(szTemp, "%dm", pStateSkill->nLeftTime/18/60);
		else
			sprintf(szTemp, "%ds", pStateSkill->nLeftTime/18);
	}
	m_BuffImage[m_nListCount].SetToolTipInfo(pszDesc, strlen(pszDesc));
	m_txtBuffTime[m_nListCount].Show();
	m_txtBuffTime[m_nListCount].SetText(szTemp);
	m_nListCount++;					
}

void KUiPlayerControlBar::Release()
{
	for (int i = m_nListCount; i < MAX_BUFF_ROLE; i ++)
	{
		m_BuffImage[i].Hide();
		m_txtBuffTime[i].Hide();
	}
	m_nListCount = 0;
}

void KUiPlayerControlBar::SetItemBtnInfo(int nBtnNo, const char* pItem)
{
	if (!pItem || !pItem[0])
		return;

	if(m_pSelf && g_pCoreShell && nBtnNo >=0 && nBtnNo < MAX_ITEMBUTTON)
	{
		m_pSelf->m_ItemBtn[nBtnNo].SetItem(pItem);
		char m_pItem[MAX_SENTENCE_LENGTH];
		memset(m_pItem, 0, sizeof(m_pItem));
		memcpy(m_pItem, pItem, sizeof(m_pItem));
		int nIdx = g_pCoreShell->GetGameData(GDI_ITEM_CHAT, true, (int)&m_pItem);
		if (nIdx)
		{
			switch (g_pCoreShell->GetGameData(GDI_ITEM_QUALITY, nIdx, 0))
			{
				case equip_magic:
					m_pSelf->m_ItemBtn[nBtnNo].SetColor(0xff6464ff,0xff00ffff,0xffff659c);
					break;
				case equip_damage:
					m_pSelf->m_ItemBtn[nBtnNo].SetColor(0xffff0042,0xff00ffff,0xffff659c);
					break;
				case equip_violet:
					m_pSelf->m_ItemBtn[nBtnNo].SetColor(0xffcc33ff,0xff00ffff,0xffff659c);
					break;
				case equip_gold:
				case equip_platina:
					m_pSelf->m_ItemBtn[nBtnNo].SetColor(0xffffd94e,0xff00ffff,0xffff659c);
					break;	
				default:
					m_pSelf->m_ItemBtn[nBtnNo].SetColor(0xffffffff,0xff00ffff,0xffff659c);
					break;
			}
			char szName[64];
			char szName2[64];
			g_pCoreShell->GetGameData(GDI_ITEM_NAME, (unsigned int)&szName, nIdx);
			int nLen = strlen(szName);
			szName2[0] = '<';
			strncpy(szName2+1,szName,nLen);
			strncpy(szName2+1+nLen,">",1);
			nLen += 2;
			szName2[nLen] = '\0';
			m_pSelf->m_ItemBtn[nBtnNo].SetSize(nLen*6,14);
			m_pSelf->m_ItemBtn[nBtnNo].SetText(szName2,nLen);
			m_pSelf->m_ItemBtn[nBtnNo].m_bHaveItem = true;
			g_pCoreShell->GetGameData(GDI_ITEM_CHAT, false, nIdx);
		}
	}
}

void KUiPlayerControlBar::ClearItemBtn()
{
	if(m_pSelf)
	{
		for (int j =0; j < MAX_ITEMBUTTON; j++)
		{
			if(m_pSelf->m_ItemBtn[j].m_X == -300 && m_pSelf->m_ItemBtn[j].m_Y == 100) continue;
			m_pSelf->m_ItemBtn[j].SetPosition(HIDE_POS);
			m_pSelf->m_ItemBtn[j].Hide();
		}
	}
}

void KUiPlayerControlBar::SetItemBtnPos(int nBtnNo, int X, int Y)
{
	if(nBtnNo >=0 && nBtnNo < MAX_ITEMBUTTON)
	{
		if(m_pSelf)
		{
			if(m_pSelf->m_ItemBtn[nBtnNo].m_bHaveItem == false) return;
			if(m_pSelf->m_ItemBtn[nBtnNo].m_X == -300 == X && m_pSelf->m_ItemBtn[nBtnNo].m_Y == 100 == Y) return;

			m_pSelf->m_ItemBtn[nBtnNo].SetPosition(X,Y);
			m_pSelf->m_ItemBtn[nBtnNo].m_X = X;
			m_pSelf->m_ItemBtn[nBtnNo].m_Y = Y;

			if(m_pSelf->m_ItemBtn[nBtnNo].m_X == -300 && m_pSelf->m_ItemBtn[nBtnNo].m_Y == 100)
				m_pSelf->m_ItemBtn[nBtnNo].Hide();
			else
				m_pSelf->m_ItemBtn[nBtnNo].Show();
		}
	}
}
