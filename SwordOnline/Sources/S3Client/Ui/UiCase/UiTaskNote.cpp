#pragma once

#include "KWin32.h"
#include "KIniFile.h"
#include "GameDataDef.h"
#include <time.h>
#include "../Elem/wnds.h"
#include "../Elem/WndMessage.h"
#include "../UiSoundSetting.h"
#include "UiFaceSelector.h"
#include "UiTaskNote.h"
#include "UiTaskDataFile.h"
#include "../../Represent/iRepresent/iRepresentShell.h"
#include "../../../core/src/CoreShell.h"
#include "../../../core/src/gamedatadef.h"
#include "../../../Engine/src/Text.h"
#include "../../../Engine/src/KDebug.h"
#include "../UiBase.h"
#include "UiSysMsgCentre.h"

extern IInlinePicEngineSink *g_pIInlinePicSink;
extern iCoreShell*		g_pCoreShell;
extern iRepresentShell*	g_pRepresentShell;


#define 	SCHEME_INI_TASKNOTE			"UiTaskNote.ini"
#define 	SCHEME_INI_MISSIONNOTE		"UiTaskNote-MissionNote.ini"
#define		DEFAULT_TASK				"\\spr\\ui3\\新任务系统\\任务图标\\icon_task_003.spr"


KUiTaskNote*	KUiTaskNote::m_pSelf = NULL;

// constructor. 
KUiTaskNote::KUiTaskNote()
{
	m_uNotOKPicIndex = -1;
	m_uOKPicIndex = -1;
	m_nTabSel = 0;
	memset(m_nTaskSel,-1,sizeof(m_nTaskSel));
	m_uCurHeaderId = 0;
}

// destructor
KUiTaskNote::~KUiTaskNote()
{
}

// opennnnnnnnnnnnnnnnnnnnnnnnnnn
KUiTaskNote* KUiTaskNote::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiTaskNote;
		if (m_pSelf)
		{
			m_pSelf->Initialize();			
			m_pSelf->InitData();
		}
	}
	if (m_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->BringToTop();
		m_pSelf->Show();
	}
	return m_pSelf;
}

// ....
void KUiTaskNote::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		m_pSelf->Hide();
		if (bDestroy)
		{
			m_pSelf->SaveData();
			KTaskDataFile::ClearAll();
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
	}
}

// is it visible?
KUiTaskNote* KUiTaskNote::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

void KUiTaskNote::InitData()
{
	KTaskDataFile::LoadData();
	UpdateData(m_nTabSel);
}

//保存数据
void KUiTaskNote::SaveData()
{
	KTaskDataFile::SaveData();
}

// Load the face
void KUiTaskNote::LoadScheme(const char* pScheme)
{
	if (m_pSelf)
	{
		char		Buff[128];
		KIniFile	Ini;
		sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_TASKNOTE);
		if (Ini.Load(Buff))
		{
			m_pSelf->Init(&Ini, "Main");
			m_pSelf->m_BtnGut.Init(&Ini, "BtnGut");
			m_pSelf->m_BtnTask.Init(&Ini, "BtnTask");
			m_pSelf->m_BtnEvent.Init(&Ini, "BtnEvent");
			m_pSelf->m_icon.Init(&Ini, "icon");
			m_pSelf->m_RecordList.Init(&Ini, "RList");
			
			if (g_pIInlinePicSink)
			{
				Ini.GetString("RList", "Hook14", "", Buff, MAX_PATH);
				g_pIInlinePicSink->AddCustomInlinePic((WORD&)m_pSelf->m_uNotOKPicIndex, Buff);
				Ini.GetString("RList", "Hook14Ok", "", Buff, MAX_PATH);
				g_pIInlinePicSink->AddCustomInlinePic((WORD&)m_pSelf->m_uOKPicIndex, Buff);
				Ini.GetString("RList", "ExplainColor", "", Buff, MAX_PATH);
				m_pSelf->m_uExplainColor.Color_dw = GetColor(Buff);
				Ini.GetString("RList", "HookColor", "", Buff, MAX_PATH);
				m_pSelf->m_uNotOKColor.Color_dw = GetColor(Buff);
				m_pSelf->m_uNotOKColor.Color_b.a = KTC_COLOR;
				Ini.GetString("RList", "HookOkColor", "", Buff, MAX_PATH);
				m_pSelf->m_uOKColor.Color_dw = GetColor(Buff);
				m_pSelf->m_uOKColor.Color_b.a = KTC_COLOR;
				Ini.GetString("RList", "HookFnColor", "", Buff, MAX_PATH);
				m_pSelf->m_uFnColor.Color_dw = GetColor(Buff);
				m_pSelf->m_uFnColor.Color_b.a = KTC_COLOR;
			}
			for(int i =0; i<MAX_TASK_HEAD;i++)
			{
				sprintf(Buff, "%d", i);
				Ini.GetString("Main", Buff, DEFAULT_TASK, m_pSelf->m_szTaskImage[i][0], sizeof(m_pSelf->m_szTaskImage[i][0]));
				sprintf(Buff, "%d_Tip", i);
				Ini.GetString("Main", Buff, NORMAL_UNCLEAR_WORD, m_pSelf->m_szTaskImage[i][1], sizeof(m_pSelf->m_szTaskImage[i][1]));
			}
			for(int i=0;i<MAX_TASK_ON_PAGE;i++)
			{
				m_pSelf->m_TBtnTaskSel[i].btn.Init(&Ini, "btn");
			}
			Ini.GetInteger2("MainPage", "Dir", &m_pSelf->m_nDirLeft, &m_pSelf->m_nDirTop);
			Ini.GetString("MainPage", "BorderImage", "", m_pSelf->m_szBorderImage, sizeof(m_pSelf->m_szBorderImage));
			Ini.GetString("MainPage", "OnImage", "", m_pSelf->m_szOnImage, sizeof(m_pSelf->m_szOnImage));
			m_pSelf->m_BtnClose.Init(&Ini, "BtnClose");
		}
	}
}

// Initialize
void KUiTaskNote::Initialize()
{
	AddChild(&m_BtnGut);
	AddChild(&m_BtnTask);
	AddChild(&m_BtnEvent);
	for(int i=0;i<MAX_TASK_ON_PAGE;i++)
	{
		AddChild(&m_TBtnTaskSel[i].btn);
		AddChild(&m_TBtnTaskSel[i].onimg);
		AddChild(&m_TBtnTaskSel[i].bdimg);
	}
	AddChild(&m_icon);
	AddChild(&m_RecordList);
	AddChild(&m_BtnClose);
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, sizeof(Scheme));
	LoadScheme(Scheme);

	Wnd_AddWindow(this);
}

int KUiTaskNote::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nResult = false;
	switch (uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_BtnClose)
		{
			OnClose();
			nResult = true;
		}
		else if(uParam == (unsigned int)(KWndWindow*)&m_BtnGut)
		{
			UpdateData(0);
			nResult = true;
		}
		else if(uParam == (unsigned int)(KWndWindow*)&m_BtnTask)
		{
			UpdateData(1);
			nResult = true;
		}
		else if(uParam == (unsigned int)(KWndWindow*)&m_BtnEvent)
		{
			UpdateData(2);
			nResult = true;
		}
		else
		{
			for(int i=0;i<MAX_TASK_ON_PAGE;i++)
			{
				if(uParam == (unsigned int)(KWndWindow*)&m_TBtnTaskSel[i].btn)
				{
					UpdateList(i);
					break;
				}
			}
		}
		break;
	case WND_N_BUTTON_OVER:
		{
		for(int i=0;i<MAX_TASK_ON_PAGE;i++)
		{
			m_TBtnTaskSel[i].onimg.Hide();
			if(uParam == (unsigned int)(KWndWindow*)&m_TBtnTaskSel[i].btn)
			{
				m_TBtnTaskSel[i].onimg.Show();
				Wnd_SwitchCursor(MOUSE_CURSOR_FINGER);
			}
		}
		nResult = true;
		}
		break;
	case WND_M_MOUSE_LEAVE:
	case WND_N_MOUSE_LEAVE_CHILD:
		{
		Wnd_SwitchCursor(MOUSE_CURSOR_NORMAL);
		for(int i=0;i<MAX_TASK_ON_PAGE;i++)
			m_TBtnTaskSel[i].onimg.Hide();
		}
		nResult = true;
		break;
	default:
		nResult = KWndShowAnimate::WndProc(uMsg, uParam, nParam);
		break;
	}
	return nResult;
}

void KUiTaskNote::OnClose()
{
	CloseWindow(false);
}

void KUiTaskNote::WakeUp(KMissionRecord* pRecord)
{
	if (pRecord->nHeaderLen && pRecord->nContentLen)
	{
		if (m_pSelf == NULL)
			KTaskDataFile::LoadData();
		TASK_SYSTEM_RECORD Record;
		Record.tTime = time(&Record.tTime);
		Record.uReserved = pRecord->uValue;
		Record.uHeaderID = atoi(pRecord->szHeader);
		Record.nAct = pRecord->nRecordAct;
		Record.nContentLen = pRecord->nContentLen;
		Record.pBuffer = pRecord->szContent;
		bool bInserted = KTaskDataFile::InsertSystemRecord(&Record);

		if (m_pSelf == NULL)
		{
			if (bInserted)
				KTaskDataFile::SaveData();
			KTaskDataFile::ClearAll();
		}
		else if (bInserted)
		{
			m_pSelf->UpdateData(m_pSelf->m_nTabSel);
		}
	}
}

void KUiTaskNote::UpdateData(int nSel)
{
	if(nSel >= 0 && nSel < MAX_TASK_PAGE)
	{
		m_BtnGut.CheckButton(false);
		m_BtnTask.CheckButton(false);
		m_BtnEvent.CheckButton(false);
		if(nSel == 0)
			m_BtnGut.CheckButton(true);
		else if(nSel == 1)
			m_BtnTask.CheckButton(true);
		else if(nSel == 2)
			m_BtnEvent.CheckButton(true);

		int nCnt = KTaskDataFile::GetSystemRecordCount();
		if (nCnt > 0)
		{
			int i,j=0,nLeft=29,nTop=48;
			for (i = 0; i < nCnt; i++)
			{
				const TASK_SYSTEM_RECORD* pRecord = KTaskDataFile::GetSystemRecord(i);
				if (pRecord == NULL)
					break;
				if(pRecord->bIsHeader)
				{
					if(pRecord->uHeaderID >= (nSel*MAX_TASK_ON_PAGE) && pRecord->uHeaderID < ((nSel+1)*MAX_TASK_ON_PAGE))
					{
						m_TBtnTaskSel[j].btn.Show();
						m_TBtnTaskSel[i].onimg.Hide();
						m_TBtnTaskSel[i].bdimg.Hide();
						m_TBtnTaskSel[j].btn.SetImage(ISI_T_SPR, m_szTaskImage[pRecord->uHeaderID][0]);
						m_TBtnTaskSel[j].bdimg.SetImage(ISI_T_SPR, m_szBorderImage);
						m_TBtnTaskSel[j].onimg.SetImage(ISI_T_SPR, m_szOnImage);
						m_TBtnTaskSel[j].btn.SetToolTipInfo(m_szTaskImage[pRecord->uHeaderID][1], strlen(m_szTaskImage[pRecord->uHeaderID][1]));
						m_TBtnTaskSel[j].id = pRecord->uHeaderID;
						m_TBtnTaskSel[j].btn.SetPosition(nLeft+j*m_nDirLeft,nTop+(j>=MAX_TASK_ON_PAGE?m_nDirTop:0));
						m_TBtnTaskSel[j].onimg.SetPosition(nLeft+j*m_nDirLeft,nTop+(j>=MAX_TASK_ON_PAGE?m_nDirTop:0));
						m_TBtnTaskSel[j].bdimg.SetPosition(nLeft+j*m_nDirLeft,nTop+(j>=MAX_TASK_ON_PAGE?m_nDirTop:0));
						j++;
					}
				}
			}
			for(NULL; j<MAX_TASK_ON_PAGE;j++)
				m_TBtnTaskSel[j].btn.Hide();
				
			m_nTabSel = nSel;
			UpdateList(m_nTaskSel[nSel]);
		}
	}
}

void KUiTaskNote::UpdateList(int nId)
{
	m_RecordList.GetMessageListBox()->Clear();
	m_nTaskSel[m_nTabSel]=nId;
	for(int i=0;i<MAX_TASK_ON_PAGE;i++)
	{
		m_TBtnTaskSel[i].bdimg.Hide();
		if(i== nId)
			m_TBtnTaskSel[i].bdimg.Show();
	}
	m_icon.SetImage(ISI_T_SPR,"");
	if (nId >= 0)
	{
		m_uCurHeaderId = m_TBtnTaskSel[nId].id;
		m_icon.SetImage(ISI_T_SPR, m_szTaskImage[m_uCurHeaderId][0]);

		int nOffset=0,nCnt=0,nCntList=0,nCntLine=0,nListCount=KTaskDataFile::GetSystemHeaderCount(m_uCurHeaderId);
		nCnt = KTaskDataFile::GetSystemRecordCount();
		m_RecordList.GetMessageListBox()->SetCapability(nListCount);	//+4是多留些空间
		for (int i = 0; i < nCnt; i++)
		{
			const TASK_SYSTEM_RECORD* pRecord = KTaskDataFile::GetSystemRecord(i);
			if (pRecord == NULL)
				break;
			if (pRecord->cBuffer[0] && pRecord->uHeaderID == m_uCurHeaderId)
			{
				nCntList++;
				char szRecord[MAX_MESSAGE_LENGTH];
				if(pRecord->nAct == REA_EXPLAIN)
				{
					szRecord[0] = KTC_COLOR;
					szRecord[1] = m_uExplainColor.Color_b.r;
					szRecord[2] = m_uExplainColor.Color_b.g;
					szRecord[3] = m_uExplainColor.Color_b.b;
					nOffset = 4;
				}
				else if(pRecord->nAct == REA_FINISH)
				{
					szRecord[0] = KTC_COLOR;
					szRecord[1] = m_uFnColor.Color_b.r;
					szRecord[2] = m_uFnColor.Color_b.g;
					szRecord[3] = m_uFnColor.Color_b.b;
					nOffset = 4;
				}
				else
				{
					szRecord[0] = KTC_INLINE_PIC;
					if(nCntList==nListCount && pRecord->nAct == REA_PERFORM)
					{
						*((WORD*)(szRecord + 1)) = m_uNotOKPicIndex;
						nOffset = 3;
						szRecord[nOffset] = m_uNotOKColor.Color_b.a;
						nOffset++;
						szRecord[nOffset] = m_uNotOKColor.Color_b.r;
						nOffset++;
						szRecord[nOffset] = m_uNotOKColor.Color_b.g;
						nOffset++;
						szRecord[nOffset] = m_uNotOKColor.Color_b.b;
						nOffset++;
					}
					else
					{
						*((WORD*)(szRecord + 1)) = m_uOKPicIndex;
						nOffset = 3;
						szRecord[nOffset] = m_uOKColor.Color_b.a;
						nOffset++;
						szRecord[nOffset] = m_uOKColor.Color_b.r;
						nOffset++;
						szRecord[nOffset] = m_uOKColor.Color_b.g;
						nOffset++;
						szRecord[nOffset] = m_uOKColor.Color_b.b;
						nOffset++;
					}
				}
				strcpy(szRecord + nOffset, pRecord->cBuffer);
				int nLen = TEncodeText(szRecord, pRecord->nContentLen+nOffset);
				m_RecordList.GetMessageListBox()->AddOneMessage(szRecord, nLen);

				nCntLine+=m_RecordList.GetMessageListBox()->GetItemLineCount(i);
			}
		}
		m_RecordList.GetScrollBar()->SetScrollPos(m_RecordList.GetScrollBar()->GetMaxValue());
	}	
}
/////////////////////////////////////
/////////////////////////////////////
KUiMissionNote*	KUiMissionNote::m_pSelf = NULL;

// constructor. 
KUiMissionNote::KUiMissionNote()
{
}

// destructor
KUiMissionNote::~KUiMissionNote()
{
}

// opennnnnnnnnnnnnnnnnnnnnnnnnnn
KUiMissionNote* KUiMissionNote::OpenWindow(KMissionRecord* pRecord)
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiMissionNote;
		if (m_pSelf)
		{
			m_pSelf->Initialize();			
		}
	}
	if (m_pSelf)
	{
		m_pSelf->m_DestText.Clear();
		if (pRecord->nHeaderLen && pRecord->nContentLen)
		{
			m_pSelf->m_Name.SetText(pRecord->szHeader);
			int nLen = TEncodeText(pRecord->szContent, pRecord->nContentLen);
			m_pSelf->m_DestText.AddOneMessage(pRecord->szContent, nLen, 0);
		}
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->BringToTop();
		m_pSelf->Show();
	}
	return m_pSelf;
}

// ....
void KUiMissionNote::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		m_pSelf->Hide();
		if (bDestroy)
		{
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
	}
}

KUiMissionNote* KUiMissionNote::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}


void KUiMissionNote::LoadScheme(const char* pScheme)
{
	if (m_pSelf)
	{
		char		Buff[128];
		KIniFile	Ini;
		sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_MISSIONNOTE);
		if (Ini.Load(Buff))
		{
			m_pSelf->Init(&Ini, "Main");
			m_pSelf->m_Name.Init(&Ini, "Name");
			m_pSelf->m_DestText.Init(&Ini, "DestText");
			m_pSelf->m_CloseBtn.Init(&Ini, "CloseBtn");
		}
	}
}

// Initialize
void KUiMissionNote::Initialize()
{
	AddChild(&m_Name);
	AddChild(&m_DestText);
	AddChild(&m_CloseBtn);
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, sizeof(Scheme));
	LoadScheme(Scheme);

	Wnd_AddWindow(this);
}

// Deal with the window messages.
int KUiMissionNote::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nResult = false;
	if (uMsg == WND_N_BUTTON_CLICK)
	{
		if (uParam == (unsigned int)(KWndWindow*)&m_CloseBtn)
		{
			CloseWindow(true);
			nResult = true;
		}	
	}
	else
	{
		nResult = KWndShadow::WndProc(uMsg, uParam, nParam);
	}
	return nResult;
}

int	KUiMissionNote::PtInWindow(int x, int y)
{
	int nRet=0;
	if (KWndWindow::PtInWindow(x, y))
		nRet= m_CloseBtn.PtInWindow(x, y);

	return nRet;
}