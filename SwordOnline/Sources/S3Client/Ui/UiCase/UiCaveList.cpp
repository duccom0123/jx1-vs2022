/* 
 * File:     UiCaveList.cpp
 * Desc:     世界地图
 * Author:   flying
 * Creation: 2003/7/22
 */
//-----------------------------------------------------------------------------
#include "KWin32.h"
#include "KIniFile.h"
#include "../elem/wnds.h"
#include "../Elem/WndMessage.h"
#include <crtdbg.h>
//#include "../ShortcutKey.h"
#include "UiCaveList.h"
#include "../UiBase.h"
#include "../UiSoundSetting.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "../../../Represent/iRepresent/KRepresentUnit.h"
#include "../../../core/src/CoreShell.h"
#include "../../../core/src/GameDataDef.h"

extern iRepresentShell*	g_pRepresentShell;
extern iCoreShell*		g_pCoreShell;

KUiCaveList* KUiCaveList::m_pSelf = NULL;

#define		SCHEME_INI		"UiCaveList.ini"

void MapToggleStatus();

KUiCaveList* KUiCaveList::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiCaveList;
		if (m_pSelf)
			m_pSelf->Initialize();
	}

	if (m_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->UpdateData();
		m_pSelf->Show();
		m_pSelf->BringToTop();
		Wnd_SetExclusive(m_pSelf);
	}
	return m_pSelf;
}

void KUiCaveList::CloseWindow()
{
	if (m_pSelf)
	{
		Wnd_ReleaseExclusive(m_pSelf);
		m_pSelf->Destroy();
		m_pSelf = NULL;
		MapToggleStatus();
	}
}

KUiCaveList* KUiCaveList::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	else
		return NULL;
}

//初始化
void KUiCaveList::Initialize()
{
	AddChild(&m_Sign);
	char szBuffer[128];
	g_UiBase.GetCurSchemePath(szBuffer, sizeof(szBuffer));
	strcat(szBuffer, "\\" SCHEME_INI);
	KIniFile	Ini;
	if (Ini.Load(szBuffer))
	{
		Init(&Ini, "CaveMap");
		m_Sign.Init(&Ini, "Sign");
	}
	Wnd_AddWindow(this, WL_TOPMOST);
	return;
}

int KUiCaveList::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nResult = false;

	switch(uMsg)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_KEYDOWN:
		CloseWindow();
		nResult = true;
		break;
	default:
		nResult = KWndWindow::WndProc(uMsg, uParam, nParam);
		break;
	}
	return nResult;
}

void KUiCaveList::UpdateData()
{
	m_Sign.Hide();

	if (g_pCoreShell)
	{
		KIniFile	Ini;
		if (Ini.Load("\\settings\\CaveList.ini"))
		{
			//取得世界地图的图形文件明
			char	szBuffer[128];
			if (Ini.GetString("List", "CaveMapImage", "", szBuffer, sizeof(szBuffer)))
			{
				SetImage(ISI_T_SPR, szBuffer, true);

				int nAreaX = -1, nAreaY = 0;
				KUiSceneTimeInfo Info;
				g_pCoreShell->SceneMapOperation(GSMOI_SCENE_TIME_INFO, (unsigned int)&Info, 0);
				sprintf(szBuffer, "%d_MapPos", Info.nSceneId);
				Ini.GetInteger2("List", szBuffer, &nAreaX, &nAreaY);
				if (nAreaX != -1)
				{
					int nWidth, nHeight;
					m_Sign.GetSize(&nWidth,  &nHeight);
					m_Sign.SetPosition(nAreaX - nWidth / 2, (nAreaY - nHeight / 2) - 15);
					m_Sign.Show();
				}
			}
		}
	}
}

//活动函数
void KUiCaveList::Breathe()
{
	if (m_Sign.IsVisible())
		m_Sign.NextFrame();
}