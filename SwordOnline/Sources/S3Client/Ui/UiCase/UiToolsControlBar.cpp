/*****************************************************************************************
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../elem/wnds.h"
#include "../Elem/WndMessage.h"
#include "../UiBase.h"
#include "UiToolsControlBar.h"
#include "UiChatCentre.h"
#include "UiTaskNote.h"

#include "../ShortcutKey.h"
#include "../../../core/src/coreshell.h"
#include "../../../Engine/src/KDebug.h"
#include "GameDataDef.h"
extern iCoreShell*		g_pCoreShell;

#define	SCHEME_INI			"UiToolsControlBar.ini"

char szArray_NormalPK[][64] = 
{
	"LuyÖn c«ng",
	"ChiÕn ®Êu",
	"§å s¸t",
//	"Bang chiÕn"
};

KUiToolsControlBar* KUiToolsControlBar::m_pSelf = NULL;

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
KUiToolsControlBar* KUiToolsControlBar::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiToolsControlBar;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
		m_pSelf->Show();
	return m_pSelf;
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
void KUiToolsControlBar::CloseWindow()
{
	if (m_pSelf)
	{
		m_pSelf->Destroy();
		m_pSelf = NULL;
	}
}


void KUiToolsControlBar::Initialize()
{
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	m_Style &= ~WND_S_VISIBLE;
	Wnd_AddWindow(this, WL_NORMAL);
	AddChild(&m_Friend);
	AddChild(&m_Options);
}


void KUiToolsControlBar::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	if (m_pSelf)
	{
		sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
		if (Ini.Load(Buff))
		{
			m_pSelf->Init(&Ini, "Main");
			m_pSelf->m_Friend.Init(&Ini, "Friend");
			m_pSelf->m_Options.Init(&Ini, "Options");
		}
	}
}
int KUiToolsControlBar::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_Friend)
			KShortcutKeyCentre::ExcuteScript(SCK_SHORTCUT_FRIEND);
		else if (uParam == (unsigned int)(KWndWindow*)&m_Options)
			KShortcutKeyCentre::ExcuteScript(SCK_SHORTCUT_SYSTEM);	
		break;
	case WND_M_MENUITEM_SELECTED:
		if (uParam == (unsigned int)(KWndWindow*)this)
		{
			if ((short)(LOWORD(nParam) >= 0))
				SwitchPK((short)(LOWORD(nParam)));
		}
		break;		
	default:
		nRet = KWndImage::WndProc(uMsg, uParam, nParam);
	}
	return nRet;
}

void KUiToolsControlBar::DefaultScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	if (m_pSelf)
	{
		sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
		if (Ini.Load(Buff))
		{
			int	nValue1, nValue2;
			Ini.GetInteger("Main", "Left",  0, &nValue1);
			Ini.GetInteger("Main", "Top",   0, &nValue2);
			m_pSelf->SetPosition(nValue1, nValue2);
		}
	}
}

void KUiToolsControlBar::Breathe()
{
	UpdateData();
}

void KUiToolsControlBar::SwitchPK()
{
	int nActionDataCount = sizeof(szArray_NormalPK) / sizeof(szArray_NormalPK[0]);
	struct KPopupMenuData* pSelUnitMenu = (KPopupMenuData*)malloc(MENU_DATA_SIZE(nActionDataCount));
	if (pSelUnitMenu == NULL)
		return;
	KPopupMenu::InitMenuData(pSelUnitMenu, nActionDataCount);
	pSelUnitMenu->nNumItem = 0;
	pSelUnitMenu->usMenuFlag |= PM_F_AUTO_DEL_WHEN_HIDE;

	int bTongFlag = g_pCoreShell->TongOperation(GTOI_TONG_FLAG, 0, 0);
	for (int i = 0; i < nActionDataCount; i++)
	{
		if (i == enumPKTongWar && !bTongFlag)
			continue;
		strncpy(pSelUnitMenu->Items[pSelUnitMenu->nNumItem].szData, szArray_NormalPK[i], sizeof(szArray_NormalPK[i]));
		pSelUnitMenu->Items[pSelUnitMenu->nNumItem].szData[sizeof(pSelUnitMenu->Items[pSelUnitMenu->nNumItem].szData) - 1] = 0;
		pSelUnitMenu->Items[pSelUnitMenu->nNumItem].uDataLen = strlen(pSelUnitMenu->Items[pSelUnitMenu->nNumItem].szData);
		pSelUnitMenu->nNumItem++;
	}
	int x, y;
	Wnd_GetCursorPos(&x, &y);
	pSelUnitMenu->nX = x;
	pSelUnitMenu->nY = y;
	KPopupMenu::Popup(pSelUnitMenu, m_pSelf, 0);
}


void KUiToolsControlBar::SwitchFastPK()
{
	int nPKSet = g_pCoreShell->GetGameData(GDI_PK_SETTING, NULL, NULL);
	if (nPKSet != enumPKNormal && nPKSet != enumPKWar)
		g_pCoreShell->OperationRequest(GOI_PK_SETTING, 0, enumPKNormal);
	else
	{
		if (nPKSet == enumPKNormal)
			g_pCoreShell->OperationRequest(GOI_PK_SETTING, 0, enumPKWar);
		else
			g_pCoreShell->OperationRequest(GOI_PK_SETTING, 0, enumPKNormal);
	}
}

void KUiToolsControlBar::SwitchPK(int nPKSet)
{
	switch(nPKSet)
	{
	case Exercises:
			g_pCoreShell->OperationRequest(GOI_PK_SETTING, 0, enumPKNormal);
		break;
	case Fighting:
			g_pCoreShell->OperationRequest(GOI_PK_SETTING, 0, enumPKWar);
		break;
	case Murder:
			g_pCoreShell->OperationRequest(GOI_PK_SETTING, 0, enumPKMurder);
		break;
	case Tongwar:
			g_pCoreShell->OperationRequest(GOI_PK_SETTING, 0, enumPKTongWar);
		break;
	}
}

