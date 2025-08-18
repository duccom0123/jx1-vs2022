// -------------------------------------------------------------------------
//	ÎÄ¼þÃû		£º	UiGetString.cpp
//	´´½¨Õß		£º	Wooy(Wu yue)
//	´´½¨Ê±¼ä	£º	2003-3-18
//	¹¦ÄÜÃèÊö	£º	»ñµÃ×Ö·û´®½çÃæ
// -------------------------------------------------------------------------
#include "KWin32.h"
#include "KIniFile.h"

#include "../Elem/AutoLocateWnd.h"
#include "../Elem/WndMessage.h"
#include "../Elem/Wnds.h"
#include "../UiBase.h"
#include "UiGetString.h"
#include "UiInformation.h"
#include "../UiSoundSetting.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "../../../engine/src/kstrbase.h"
#include "../../../engine/src/KDebug.h"
#include "../../../core/src/coreshell.h"


#define	SCHEME_INI 	"UiGetString.ini"

extern iRepresentShell *g_pRepresentShell;
extern iCoreShell*			g_pCoreShell;

KUiGetString* KUiGetString::m_pSelf = NULL;

KUiGetString::KUiGetString()
{
	m_pRequester = NULL;
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º´ò¿ª´°¿Ú£¬·µ»ØÎ¨Ò»µÄÒ»¸öÀà¶ÔÏóÊµÀý
//--------------------------------------------------------------------------
KUiGetString* KUiGetString::OpenWindow(GET_STRING_ACTION eAction, const char* pszTitle,
						const char* pszInitString,
						KWndWindow* pRequester, unsigned int uParam,
 						int nMaxNum, int nMinLen, int nMaxLen)
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiGetString;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		m_pSelf->m_Normal.Hide();
		m_pSelf->m_Adv.Hide();
		m_pSelf->m_StringEdit.SetMaskCharacter(false);
		m_pSelf->m_StringEdit.SetLimitText(nMaxLen);
		m_pSelf->m_eAction = eAction;
		switch(m_pSelf->m_eAction)
		{
		case GSA_ADV:
			m_pSelf->m_Adv.Show();
			Wnd_SetExclusive((KWndWindow*)m_pSelf);
			break;
		case GSA_PW:
			m_pSelf->m_StringEdit.SetMaskCharacter(true);
			m_pSelf->m_Normal.Show();
			break;
		default:
			m_pSelf->m_Normal.Show();
			Wnd_SetExclusive((KWndWindow*)m_pSelf);
			break;
		}
		m_pSelf->m_nMaxNum = nMaxNum;
		m_pSelf->m_nMinLen = nMinLen;
		m_pSelf->m_nMaxLen = nMaxLen;
		m_pSelf->m_pRequester = pRequester;
		m_pSelf->m_uRequesterParam = uParam;
		m_pSelf->m_Title.SetText(pszTitle);
		m_pSelf->m_StringEdit.SetText(pszInitString);
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->BringToTop();
		m_pSelf->Show();
	}
	return m_pSelf;
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º¹Ø±ÕÏú»Ù´°¿Ú
//--------------------------------------------------------------------------
void KUiGetString::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		m_pSelf->m_pRequester = NULL;
		if (bDestroy)
		{
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
		else
			m_pSelf->Hide();
	}
}

KUiGetString*	KUiGetString::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

//ÏÔÊ¾´°¿Ú
void KUiGetString::Show()
{
	int Left, Top;
	if(m_eAction==GSA_NORMAL)
	{
	ALW_GetWndPosition(Left, Top, m_Width, m_Height);
	SetPosition(Left, Top);
	}
	else
	{
		Left = RESOLUTION_WIDTH/2 - m_Width/2;
		Top = RESOLUTION_HEIGHT/2 - m_Height/2;
		SetPosition(Left < 0 ? 0 : Left, Top < 0 ? 0 : Top);
	}
	KWndImage::Show();
	Wnd_SetFocusWnd(&m_StringEdit);
	
	if(m_eAction==GSA_PW)
	{
		
	}
}

//Òþ²Ø´°¿Ú
void KUiGetString::Hide()
{
	Wnd_SetFocusWnd(NULL);
	Wnd_ReleaseExclusive((KWndWindow*)this);
	KWndImage::Hide();
}

void KUiGetString::Breathe()
{
	if(m_pRequester)
	{
		if(m_pRequester->IsVisible()==false)
		{
			m_pRequester = NULL;
			CloseWindow(true);
		}
	}
}
//--------------------------------------------------------------------------
//	¹¦ÄÜ£º³õÊ¼»¯
//--------------------------------------------------------------------------
void KUiGetString::Initialize()
{
	AddChild(&m_Normal);
	AddChild(&m_Adv);
	AddChild(&m_Title);
	AddChild(&m_StringEdit);
	AddChild(&m_OkBtn);
	AddChild(&m_CancelBtn);
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	Wnd_AddWindow(this);
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£ºÔØÈë´°¿ÚµÄ½çÃæ·½°¸
//--------------------------------------------------------------------------
void KUiGetString::LoadScheme(const char* pScheme)
{
	if (m_pSelf)
	{
		char		Buff[128];
		KIniFile	Ini;
		sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
		if (Ini.Load(Buff))
		{
			m_pSelf->Init(&Ini, "Main");
			m_pSelf->m_Normal.Init(&Ini, "Normal");
			m_pSelf->m_Adv.Init(&Ini, "Adv");
			m_pSelf->m_Title.Init(&Ini, "Title");
			m_pSelf->m_StringEdit.Init(&Ini, "StringInput");
			m_pSelf->m_OkBtn.Init(&Ini, "OkBtn");
			m_pSelf->m_CancelBtn.Init(&Ini, "CancelBtn");
		}
	}
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º´°¿ÚÏûÏ¢º¯Êý
//--------------------------------------------------------------------------
int KUiGetString::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_OkBtn)
			OnSend();
		else if (uParam == (unsigned int)(KWndWindow*)&m_CancelBtn)
			OnCancel();
		break;
	case WM_KEYDOWN:
		if (uParam == VK_RETURN)
		{
			OnSend();
			nRet = 1;
		}
		else if (uParam == VK_ESCAPE)
		{
			OnCancel();
			nRet = 1;
		}
		break;
	case WND_N_EDIT_CHANGE:
		if (m_nMaxNum)
			OnCheckInput();
	default:
		return KWndImage::WndProc(uMsg, uParam, nParam);
	}
	return 0;
}


void KUiGetString::OnCheckInput()
{
	int nNum = m_StringEdit.GetIntNumber();
	if (nNum > m_nMaxNum)
		nNum = m_nMaxNum;
		
	char	szBuff1[16], szBuff2[16];
	itoa(nNum, szBuff1, 10);
	m_StringEdit.GetText(szBuff2, sizeof(szBuff2), true);
	if (strcmp(szBuff1, szBuff2))
		m_StringEdit.SetIntText(nNum);
}
//--------------------------------------------------------------------------
//	¹¦ÄÜ£ºÏìÓ¦µã»÷È·ÈÏ°´Å¥
//--------------------------------------------------------------------------
void KUiGetString::OnSend()
{
	char	lpszInfo[32];

	int nLen = m_StringEdit.GetText(lpszInfo, sizeof(lpszInfo), true);
	if (nLen >= m_nMinLen && nLen <= m_nMaxLen)
	{
		switch(m_eAction)
		{
		case GSA_PW:
			if(g_pCoreShell)
			g_pCoreShell->OperationRequest(GOI_UNLOCK, (unsigned int)&lpszInfo, 0);
			break;
		case GSA_INPUT:
			if(g_pCoreShell)
			g_pCoreShell->OperationRequest(GOI_INPUT, (unsigned int)&lpszInfo, 0);
			break;
		default:
			if (m_pRequester)
				m_pRequester->WndProc(WND_M_OTHER_WORK_RESULT, m_uRequesterParam, m_nMaxNum > 0 ? atoi(lpszInfo) : (int)&lpszInfo);
			break;
		}
		CloseWindow(false);
	}
	else
	{
		UIMessageBox(((nLen < m_nMinLen) ?
			"V¨n tù nhËp vµo qu¸ ng¾n" : "V¨n tù nhËp vµo qu¸ dµi"));
	}
}

void KUiGetString::OnCancel()
{
	switch(m_eAction)
	{
	case GSA_FOLK:
		if (m_pRequester)
			m_pRequester->WndProc(WND_M_OTHER_WORK_RESULT, m_uRequesterParam, -1);
		break;
	default:
		break;
	}
	CloseWindow(true);
}